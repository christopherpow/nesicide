#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "pasm_lib.h"

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{
    CSourceItem *rootSource = nesicideProject->getProject()->getMainSource();
    CPRGROMBanks *prgRomBanks = nesicideProject->get_pointerToCartridge()->getPointerToPrgRomBanks();
    char* romData = NULL;
    int romLength = 0;

    if (!rootSource)
    {
        builderTextLogger.write("Error: No main source has been defined.");
        builderTextLogger.write("<i>You can select the main source to compile in the project properties dialog.</i>");
        return false;
    }

    builderTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembling '" + rootSource->get_sourceName() + "'...");

#if defined ( SARBIN_ASSEMBLER )
    QStringList *source = new QStringList(rootSource->get_sourceCode().split('\n'));

    stripComments(source);

    // Process the labels into a table and remove the syntax that declares them
    if (!getLabels(source))
        return false;

    // Convert opcodes to .db directives
    if (!convertOpcodesToDBs(source))
        return false;

    // Assemble source
    if (!assembleSource(source))
        return false;

    // Resolve labels
    if (!resolveLabels())
        return false;
#else
   pasm_assemble ( rootSource->get_sourceCode().toLatin1().data(), &romData, &romLength, NULL );

   // Initialize our first bank
   CPRGROMBank *curBank = new CPRGROMBank();
   curBank->set_indexOfPrgRomBank(prgRomBanks->get_pointerToArrayOfBanks()->count());
   memset(curBank->get_pointerToBankData(), 0, 0x4000);
   curBank->InitTreeItem(prgRomBanks);
   prgRomBanks->appendChild(curBank);
   prgRomBanks->get_pointerToArrayOfBanks()->append(curBank);

   curBank->set_pointerToBankData ( (quint8*)romData );

#endif

    return true;
}

bool CSourceAssembler::resolveLabels()
{
    CPRGROMBanks *prgRomBanks = nesicideProject->get_pointerToCartridge()->getPointerToPrgRomBanks();

    for (int labelIdx = 0; labelIdx < m_labelUseList.count(); labelIdx++)
    {
        LabelEntry_s sourceLabel;
        for (int i = 0; i < m_labelEntries.count(); i++) {
            if (m_labelEntries.at(i).labelName == m_labelUseList.at(labelIdx).labelName)
            {
                sourceLabel = m_labelEntries.at(i);
                break;
            }
        }

        if (sourceLabel.labelName == "") {
            builderTextLogger.write("<font color='red'>Internal error while processing labels: Label not found.</font>");
            return false;
        }

        LabelEntry_s labelEntry = m_labelUseList.value(labelIdx);
        CPRGROMBank *destPrgRomBank = prgRomBanks->get_pointerToArrayOfBanks()->at(labelEntry.bank);

        if (labelEntry.byteSize == 2) {

            // Absolute addressing
            uint labelPos = sourceLabel.origin + sourceLabel.offset + (sourceLabel.bank * 0x4000);
            destPrgRomBank->get_pointerToBankData()[labelEntry.offset] = (labelPos & 0xFF);
            destPrgRomBank->get_pointerToBankData()[labelEntry.offset + 1] = (labelPos >> 8);

        } else {

            // Relative addressing
            int sourceLabelPos = sourceLabel.origin + sourceLabel.offset + (sourceLabel.bank * 0x4000);
            int destLabelPos = labelEntry.origin + labelEntry.offset + (labelEntry.bank * 0x4000) + 1;
            int diff = sourceLabelPos- destLabelPos;
            if ((diff < -128) || (diff > 127)) {
                builderTextLogger.write("<font color='red'>Label error: Relative address out of range.</font>");
                return false;
            }

            destPrgRomBank->get_pointerToBankData()[labelEntry.offset] = calculate2sCompelment(diff);

        }
    }
    return true;
}

bool CSourceAssembler::assembleSource(QStringList *source)
{

    CPRGROMBanks *prgRomBanks = nesicideProject->get_pointerToCartridge()->getPointerToPrgRomBanks();

    // Delete the existing CHR-ROM banks.
    while (prgRomBanks->childCount() > 0)
    {
        CPRGROMBank *bank = (CPRGROMBank *)prgRomBanks->child(0);
        prgRomBanks->get_pointerToArrayOfBanks()->removeFirst();;
        prgRomBanks->removeChild(bank);
        delete bank;
    }

    int bankPtr = 0x0000;

    // Initialize our first bank
    CPRGROMBank *curBank = new CPRGROMBank();
    curBank->set_indexOfPrgRomBank(prgRomBanks->get_pointerToArrayOfBanks()->count());
    memset(curBank->get_pointerToBankData(), 0, sizeof(quint8) * 0x4000);
    curBank->InitTreeItem(prgRomBanks);
    prgRomBanks->appendChild(curBank);
    prgRomBanks->get_pointerToArrayOfBanks()->append(curBank);

    int currentOrg = 0x0000;
    m_labelUseList.clear();

    for (int lineIdx = 0; lineIdx < source->length(); lineIdx++)
    {
        QString curLine = QString(source->at(lineIdx)).trimmed();

        // If the label is on this line, then set the current offset and origin
        for (int labelIdx = 0; labelIdx < m_labelEntries.count(); labelIdx++)
        {
            if (m_labelEntries.at(labelIdx).lineNumber == lineIdx) {
                // This line is where a label is, so set its value accordingly
                LabelEntry_s labelEntry = m_labelEntries.at(labelIdx);
                labelEntry.origin = currentOrg;
                labelEntry.offset = bankPtr;
                labelEntry.bank = curBank->get_indexOfPrgRomBank();
                m_labelEntries.replace(labelIdx, labelEntry);
            }
        }

        // If the line is empty there is nothing else to do
        if (curLine.isEmpty())
            continue;

        // If there is a line that doesn't start with a dot, then something messed up in
        // the previous passes.
        if (curLine.at(0) != '.')
        {
            QString error = "<font color='red'>Error: Internal assembler problem. Offending line:\n";
            error.append(curLine);
            error.append("</font>");
            builderTextLogger.write(error);
            return false;
        }

        QString directive = curLine.mid(1, curLine.indexOf(' ') - 1);
        if (directive == "padto") {
            bool ok;
            int numValue = numberToInt(&ok, curLine.mid(7).trimmed());
            if ((!ok) || (numValue < 0) || (numValue > 0x3FFF))
            {
                builderTextLogger.write("<font color='red'>Error: Invalid pad-to address specified on line " +
                                        QString::number(lineIdx + 1) + ".</font>");
                return false;
            }

            while (bankPtr < numValue) {
                BANK_WRITEBYTE(0x00);
            }
        } else if (directive == "incbin") {
            QString *errorMsg = new QString();
            QString strValue = processString(curLine.mid(8).trimmed(), errorMsg);
            if (!errorMsg->isEmpty()) {
                errorMsg->prepend("<font color='red'>Error: ");
                errorMsg->append("on line " + QString::number(lineIdx + 1) + ".</font>");
                builderTextLogger.write(*errorMsg);
                delete errorMsg;
                return false;
            }
            delete errorMsg;
            CBinaryFiles *binaryFiles = nesicideProject->getProject()->getBinaryFiles();
            CBinaryFile *binFile = (CBinaryFile *)NULL;
            for (int binIdx = 0; binIdx < binaryFiles->getBinaryFileList()->count(); binIdx++)
            {
                if (binaryFiles->getBinaryFileList()->value(binIdx)->caption() == strValue)
                {
                    binFile = binaryFiles->getBinaryFileList()->value(binIdx);
                    break;
                }
            }

            if (!binFile)
            {
                builderTextLogger.write("<font color='red'>Error: Invalid binary file name specified on line " +
                                        QString::number(lineIdx + 1) + ".</font>");
                return false;
            }

            for (int byteIdx = 0; byteIdx < binFile->getBinaryData()->count(); byteIdx++)
                BANK_WRITEBYTE((quint8)binFile->getBinaryData()->at(byteIdx));

            return true;

        } else if (directive == "db") {
            // .db Data Byte Directive
            QStringList items = curLine.mid(4).trimmed().split(',', QString::SkipEmptyParts);
            for (int itemIdx = 0; itemIdx < items.count(); itemIdx++)
            {
                QString curValue = items.at(itemIdx).trimmed();

                if (isLabel(curValue) || ((curValue.split('|').count() == 2) && (isLabel(curValue.split('|').at(1)))))
                {
                    QStringList labelDefList = curValue.split('|');
                    if (labelDefList.count() != 2)
                    {
                        builderTextLogger.write("<font color='red'>Error: Invalid label definition specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if (labelDefList.at(0) == "abs")
                    {
                        LabelEntry_s labelEntry;
                        labelEntry.labelName = labelDefList.at(1);
                        labelEntry.offset = bankPtr;
                        labelEntry.origin = currentOrg;
                        labelEntry.byteSize = 2;
                        labelEntry.bank = curBank->get_indexOfPrgRomBank();
                        m_labelUseList.append(labelEntry);
                        BANK_WRITEBYTE(0x00);
                        BANK_WRITEBYTE(0x00);
                    } else if (labelDefList.at(0) == "rel") {
                        LabelEntry_s labelEntry;
                        labelEntry.labelName = labelDefList.at(1);
                        labelEntry.offset = bankPtr;
                        labelEntry.origin = currentOrg;
                        labelEntry.byteSize = 1;
                        labelEntry.bank = curBank->get_indexOfPrgRomBank();
                        m_labelUseList.append(labelEntry);
                        BANK_WRITEBYTE(0x00)
                    } else {
                        builderTextLogger.write("<font color='red'>Error: Invalid label value type specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }


                } else {

                    bool ok;
                    int numValue = numberToInt(&ok, curValue);
                    if ((!ok) || (numValue < 0) || (numValue > 0xFF))
                    {
                        builderTextLogger.write("<font color='red'>Error: Invalid numeric value specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }
                    BANK_WRITEBYTE((quint8)numValue)
                }
            }
        } else if (directive == "dw") {
            // .db Data Byte Directive
            QStringList items = curLine.mid(4).trimmed().split(',', QString::SkipEmptyParts);
            for (int itemIdx = 0; itemIdx < items.count(); itemIdx++)
            {
                QString curValue = items.at(itemIdx).trimmed();

                if (isLabel(curValue) || ((curValue.split('|').count() == 2) && (isLabel(curValue.split('|').at(1)))))
                {
                    QStringList labelDefList = curValue.split('|');
                    if (labelDefList.count() != 2)
                    {
                        builderTextLogger.write("<font color='red'>Error: Invalid label definition specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if (labelDefList.at(0) == "abs")
                    {
                        LabelEntry_s labelEntry;
                        labelEntry.labelName = labelDefList.at(1);
                        labelEntry.offset = bankPtr;
                        labelEntry.origin = currentOrg;
                        labelEntry.byteSize = 2;
                        labelEntry.bank = curBank->get_indexOfPrgRomBank();
                        m_labelUseList.append(labelEntry);
                        BANK_WRITEBYTE(0x00);
                        BANK_WRITEBYTE(0x00);
                    } else if (labelDefList.at(0) == "rel") {
                        LabelEntry_s labelEntry;
                        labelEntry.labelName = labelDefList.at(1);
                        labelEntry.offset = bankPtr;
                        labelEntry.origin = currentOrg;
                        labelEntry.byteSize = 1;
                        labelEntry.bank = curBank->get_indexOfPrgRomBank();
                        m_labelUseList.append(labelEntry);
                        BANK_WRITEBYTE(0x00)
                    } else {
                        builderTextLogger.write("<font color='red'>Error: Invalid label value type specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }


                } else {

                    bool ok;
                    int numValue = numberToInt(&ok, curValue);
                    if ((!ok) || (numValue < 0) || (numValue > 0xFFFF))
                    {
                        builderTextLogger.write("<font color='red'>Error: Invalid numeric value specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    BANK_WRITEBYTE((quint8)(numValue & 0xFF))
                    BANK_WRITEBYTE((quint8)(numValue >> 8))
                }
            }
        } else if (directive == "org")
        {
            // .org Label Origin Directive
            bool ok;
            int numValue = numberToInt(&ok, curLine.mid(5).trimmed());
            if ((!ok) || (numValue < 0))
            {
                builderTextLogger.write("<font color='red'>Error: Invalid origin address specified on line " +
                                        QString::number(lineIdx + 1) + ".</font>");
                return false;
            }
            currentOrg = numValue;
        }
    }

    // If we are at the start of a new bank, then the bank is blank and we can delete it
    if (bankPtr == 0) {
        prgRomBanks->get_pointerToArrayOfBanks()->removeAll(curBank);
        prgRomBanks->removeChild(curBank);
        delete curBank;
    }

    return true;

}

QString CSourceAssembler::processString(QString stringDef, QString *errorMsg)
{
    errorMsg->clear();

    if ((stringDef.length() < 2) || (stringDef.at(0) != '\"')
        || (stringDef.at(stringDef.length()-1) != '\"')
        || ((stringDef.at(stringDef.length()-1) == '\"') && (stringDef.at(stringDef.length()-1) == '\\')))
    {
        errorMsg->append("Invalid string syntax");
        return QString("");
    }

    QString result;
    bool isEscaped = false;
    for (int i=1; i<stringDef.length()-1; i++)
    {
        if (!isEscaped) {
            if (stringDef.at(i) == '\\') {
                isEscaped = true;
                continue;
            } else {
                result.append(stringDef.at(i));
            }
        } else {
            result.append(stringDef.at(i));
            isEscaped = false;
        }
    }

    return result;
}

bool CSourceAssembler::stripComments(QStringList *source)
{
    // TODO: either a much better regex or a manual scan
    QStringList list = source->replaceInStrings(QRegExp(";.*$"), "");
    source->clear();
    source->append(list);
    return true;
}

bool CSourceAssembler::trimBlankLines(QStringList *source)
{
    bool result = false;
    for (int i=0; i< source->length(); i++)
        if (source->value(i).trimmed().isEmpty())
        {
            source->removeAt(i);
            i--;
            result = true;
        }
    return result;
}

bool CSourceAssembler::convertOpcodesToDBs(QStringList *source)
{
    for (int lineIdx = 0; lineIdx < source->count(); lineIdx++)
    {
        QString curLine = QString(source->value(lineIdx)).trimmed();
        QString firstWord;

        if (!curLine.isEmpty())
        if (curLine.at(0) != QChar('.'))
        {
            // Find the first 'word' of the line
            if (curLine.indexOf(' ') > -1)
                firstWord = curLine.mid(0, curLine.indexOf(' ')).toUpper();
            else
                firstWord = curLine.toUpper();

            int instructionIdx = 0;

            // Loop through the instructions until we (hopefully) find the right one.
            // Note that the instruction list has a final blank mnemonic entry for end-of-list detection.
            do
            {
                if (AssemblerInstructionItems[instructionIdx].mnemonic == firstWord)
                    break;
            } while (!AssemblerInstructionItems[++instructionIdx].mnemonic.isEmpty());

            // If the instruciton index is on the footer entry, we have an invalid opcode.
            if (AssemblerInstructionItems[instructionIdx].mnemonic.isEmpty())
            {
                // TODO: Highlight the errors on the code editor (if visible)
                builderTextLogger.write("<font color='red'>Error: Invalid opcode '" + firstWord + "' on line " +
                                        QString::number(lineIdx + 1) + ".</font>");
                return false;
            }

            // We have a valid opcode, so let's process it.

            // For implied, there should be nothing after the opcode.
            if ((curLine.length() == firstWord.length()) && (AssemblerInstructionItems[instructionIdx].impl.cycles > 0))
            {
                // IMPLIED
                curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].impl.opcode, 16).toUpper();
            } else if ((curLine.length() == firstWord.length()) && (AssemblerInstructionItems[instructionIdx].accum.cycles > 0)) {
                // ACCUMULATOR
                curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].accum.opcode, 16).toUpper();
            } else if (getParamCount(curLine.mid(firstWord.length())) == 1) {
                QString param0 = getParamItem(curLine.mid(firstWord.length()), 0);
                if (param0.at(0) == '#')
                {
                    // IMMEDIATE
                    bool ok;
                    int immValue = numberToInt(&ok, param0.mid(1));
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid immediate specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].imm.opcode, 16).toUpper()
                              + ", $" + QString::number(immValue, 16).toUpper();

                } else {
                    if ((AssemblerInstructionItems[instructionIdx].abs.cycles > 0)
                        && (isLabel(param0))) {
                        // ABSOLUTE (Label)
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].abs.opcode, 16).toUpper()
                                  + ", abs|" + param0;
                    } else if ((AssemblerInstructionItems[instructionIdx].indirect.cycles > 0) &&
                               (param0.at(0) == '(') && (param0.at(param0.length()-1) == ')')
                        && (isLabel(param0))) {
                        // INDIRECT (Label)
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].indirect.opcode, 16).toUpper()
                                  + ", abs|" + param0;
                    } else if ((AssemblerInstructionItems[instructionIdx].indirect.cycles > 0) &&
                               (param0.at(0) == '(') && (param0.at(param0.length()-1) == ')')) {
                        // INDIRECT (Non Label)
                        bool ok;
                        int immValue = numberToInt(&ok, param0);
                        if (!ok)
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Invalid indirect address specified on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].indirect.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue & 0xFF, 16).toUpper()
                                  + ", $" + QString::number((immValue >> 8) & 0xFF, 16).toUpper();

                    } else if ((AssemblerInstructionItems[instructionIdx].rel.cycles > 0)
                        && (isLabel(param0))) {
                        // RELATIVE (Label)
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].rel.opcode, 16).toUpper()
                                  + ", rel|" + param0;

                    }
                    else if (AssemblerInstructionItems[instructionIdx].rel.cycles > 0)
                    {
                        // RELATIVE (Non Label)
                        bool ok;
                        int immValue = numberToInt(&ok, param0);
                        if (!ok)
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Invalid relative address specified on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }

                        if ((immValue < -128) || (immValue > 127))
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Relative address value out of range (" +
                                                    QString::number(immValue) + ") on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }


                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].rel.opcode, 16).toUpper()
                                  + ", $" + QString::number(calculate2sCompelment(immValue), 16).toUpper();



                    } else {
                        bool ok;
                        int immValue = numberToInt(&ok, param0);
                        if (!ok)
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage or Absolute value specified "
                                                    "on line " + QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }

                        if ((immValue > 0xFFFF) || (immValue < 0))
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Absolute value specified is out of range (" +
                                                    QString::number(immValue) + ") "
                                                    "on line " + QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }

                        if ((immValue <= 0xFF) && (AssemblerInstructionItems[instructionIdx].zpage.cycles > 0))
                        {
                            // ZEROPAGE
                            curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].zpage.opcode, 16).toUpper()
                                      + ", $" + QString::number(immValue, 16).toUpper();
                        } else if  (AssemblerInstructionItems[instructionIdx].abs.cycles > 0){
                            // ABSOLUTE (Non Label)
                            curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].abs.opcode, 16).toUpper()
                                      + ", $" + QString::number(immValue & 0xFF, 16).toUpper()
                                      + ", $" + QString::number((immValue >> 8) & 0xFF, 16).toUpper();
                        } else {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Invalid combination of operand and opcode on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }
                    }

                }
            } else if (getParamCount(curLine.mid(firstWord.length())) == 2) {
                QString param0 = getParamItem(curLine.mid(firstWord.length()), 0);
                QString param1 = getParamItem(curLine.mid(firstWord.length()), 1);

                if ((param1.trimmed().replace(' ', "").toUpper() == "X)")
                    && (AssemblerInstructionItems[instructionIdx].ind_x.cycles > 0)
                    && (param0.trimmed().at(0) == '(')) {
                    // ($##,X) // PREINDEXED INDIRECT

                    bool ok;
                    int immValue = numberToInt(&ok, param0);
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid value specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].ind_x.opcode, 16).toUpper()
                              + ", $" + QString::number(immValue, 16).toUpper();



                } else if (param1.trimmed().replace(' ', "").toUpper() == "X") {
                    bool ok;
                    int immValue = numberToInt(&ok, param0);
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage or Absolute value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if ((AssemblerInstructionItems[instructionIdx].zpage_x.cycles > 0) && (immValue <= 0xFF)) {
                        // $##,X // ZEROPAGE INDEXED X
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].zpage_x.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue, 16).toUpper();

                    } else if ((AssemblerInstructionItems[instructionIdx].abs_x.cycles > 0) && (immValue <= 0xFFFF)) {
                        // $####,X // ABSOLUTE INDEXED X
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].abs_x.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue & 0xFF, 16).toUpper()
                                  + ", $" + QString::number((immValue >> 8) & 0xFF, 16).toUpper();

                    } else {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage, X or Absolute, X value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }


                } else if ((param1.trimmed().replace(' ', "").toUpper() == "Y")
                    && (AssemblerInstructionItems[instructionIdx].ind_y.cycles > 0)
                            && (param0.trimmed().at(0) == '(')
                            && (param0.trimmed().at(param0.trimmed().length()-1) == ')')) {
                    //($##),Y // POSTINDEXED INDIRECT
                    bool ok;
                    int immValue = numberToInt(&ok, param0.mid(1, param0.length() - 2));

                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid value specified on line " +
                                                QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].ind_y.opcode, 16).toUpper()
                              + ", $" + QString::number(immValue, 16).toUpper();



                } else if (param1.trimmed().replace(' ', "").toUpper() == "Y") {

                    bool ok;
                    int immValue = numberToInt(&ok, param0);
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage or Absolute value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if ((AssemblerInstructionItems[instructionIdx].zpage_x.cycles > 0) && (immValue <= 0xFF)) {
                        // $##,Y // ZEROPAGE INDEXED Y
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].zpage_y.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue, 16).toUpper();

                    } else if ((AssemblerInstructionItems[instructionIdx].abs_x.cycles > 0) && (immValue <= 0xFFFF)) {
                        // $####,Y // ABSOLUTE INDEXED Y
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].abs_y.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue & 0xFF, 16).toUpper()
                                  + ", $" + QString::number((immValue >> 8) & 0xFF, 16).toUpper();
                    } else {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage, Y or Absolute, Y value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                }

            } else {
                // TODO: Highlight the errors on the code editor (if visible)
                builderTextLogger.write("<font color='red'>Error: Invalid combination of operand and opcode on line " +
                                        QString::number(lineIdx + 1) + ".</font>");
                return false;
            }

        }

        source->replace(lineIdx, curLine);
    }
    return true;
}

quint8 CSourceAssembler::calculate2sCompelment(int value)
{
    if (value > 0)
        return (quint8)value;

    // Negative value - do 2's compliment
    quint8 newValue = abs(value);
    newValue = ~newValue;
    newValue += 1;
    return newValue;
}

bool CSourceAssembler::isLabel(QString param)
{
    for (int i = 0; i < m_labelEntries.count(); i++)
    {
        if (m_labelEntries.at(i).labelName == param)
            return true;
    }
    return false;
}

uint CSourceAssembler::numberToInt(bool *ok, QString number)
{
    number = number.trimmed();

    if (number.at(0) == '$') {
        return number.mid(1).toUInt(ok, 16);

    } else if (number.at(0) == '%') {
        int base = 1;
        int immValue = 0;
        for (int i=number.length()-1; i >= 0; i--) {
            if (number.at(i) == '%')
                break;
            if ((number.at(i) == '0') || (number.at(i) == '1')) {
               immValue += ((number.at(i) == '1' ? 1 : 0) << base);
            } else {
                *ok = false;
                return 0;
            }
            base++;

            *ok = true;
            return immValue;
        }
    } else {
        return number.mid(1).toUInt(ok, 0);
    }

    *ok = false;
    return 0;
}

bool CSourceAssembler::getLabels(QStringList *source)
{
    m_labelEntries.clear();
    for (int lineIdx = 0; lineIdx < source->length(); lineIdx++)
    {
        QString curLine = QString(source->at(lineIdx).trimmed());

        // Detect a label ':' identifier
        int labelPos = curLine.indexOf(QString(":"));
        if (labelPos == -1)
            continue;

        // There should not be a non alpha-numeric (or a space) before the :
        bool isLabel = true;
        for (int i=0; i < labelPos; i++)
        {
            if (curLine.at(i).isSpace() || curLine.at(i).isSymbol())
            {
                isLabel = false;
                break;
            }
        }

        if (!isLabel)
            continue;

        // This is a label
        LabelEntry_s labelEntry;
        labelEntry.labelName = curLine.mid(0, labelPos);
        labelEntry.lineNumber = lineIdx;
        m_labelEntries.append(labelEntry);

        curLine = curLine.mid(labelPos + 1);

        source->replace(lineIdx, curLine);
    }

    return true;
}

int CSourceAssembler::getParamCount(QString sourceLine)
{
    if (sourceLine.indexOf(',') > -1)
        return 2;
    else
        return 1;
}

QString CSourceAssembler::getParamItem(QString sourceLine, int paramNum)
{
    QString result;

    if (paramNum > 1)
        return result;

    if ((sourceLine.indexOf(',') == -1) && (paramNum > 0))
        return result;

    if ((sourceLine.indexOf(',') == -1) && (paramNum == 0))
        return sourceLine.trimmed();

    if ((sourceLine.indexOf(',') > -1) && (paramNum == 0))
        return sourceLine.mid(0, sourceLine.indexOf(','));

    if ((sourceLine.indexOf(',') > -1) && (paramNum == 1))
        return sourceLine.mid(sourceLine.indexOf(',') + 1);

    return result;
}
