#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{
    qint8 *binaryData = new qint8[0x4000];
    memset(binaryData, 0, 0x4000);
    CSourceItem *rootSource = nesicideProject->getProject()->getMainSource();
    if (!rootSource)
    {
        builderTextLogger.write("Error: No main source has been defined.");
        builderTextLogger.write("<i>You can select the main source to compile in the project properties dialog.</i>");
        return false;
    }

    builderTextLogger.write("Assembling '" + rootSource->get_sourceName() + "'...");
    QStringList *source = new QStringList(rootSource->get_sourceCode().split('\n'));

    stripComments(source);

    if (!getLabels(source))
    {
        delete binaryData;
        return false;
    }

    if (!convertOpcodesToDBs(source))
    {
        delete binaryData;
        return false;
    }


    builderTextLogger.write("--------------------------------------------------------------------------");
    for (int i=0; i < source->length(); i++)
        builderTextLogger.write(source->value(i));
    builderTextLogger.write("--------------------------------------------------------------------------");


    delete binaryData;
    return true;
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
    for (int lineIdx = 0; lineIdx < source->length(); lineIdx++)
    {
        QString curLine = QString(source->at(lineIdx)).trimmed();
        QString firstWord;

        if ((curLine.at(0).toAscii() != '.') && (!curLine.isEmpty()))
        {
            // Find the first 'word' of the lineQString param0 = getParamItem(&curLine.mid(firstWord.length()), 0);
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
            if ((curLine == firstWord) && (AssemblerInstructionItems[instructionIdx].impl.cycles > 0))
            {
                // IMPLIED
                curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].impl.opcode, 16).toUpper();
            } else if ((curLine == firstWord) && (AssemblerInstructionItems[instructionIdx].accum.cycles > 0)) {
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
                                  + ", " + param0;
                    } else if ((AssemblerInstructionItems[instructionIdx].indirect.cycles > 0) &&
                               (param0.at(0) == '(') && (param0.at(param0.length()-1) == ')')
                        && (isLabel(param0))) {
                        // INDIRECT (Label)
                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].indirect.opcode, 16).toUpper()
                                  + ", " + param0;
                    } else if ((AssemblerInstructionItems[instructionIdx].indirect.cycles > 0) &&
                               (param0.at(0) == '(') && (param0.at(param0.length()-1) == ')')) {
                        // INDIRECT (Non Label)
                        bool ok;
                        int immValue = numberToInt(&ok, param0.mid(1));
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
                                  + ", " + param0;

                    }
                    else if (AssemblerInstructionItems[instructionIdx].rel.cycles > 0)
                    {
                        // RELATIVE (Non Label)
                        bool ok;
                        int immValue = numberToInt(&ok, param0.mid(1));
                        if (!ok)
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Invalid relative address specified on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }

                        if ((immValue < -127) || (immValue > 128))
                        {
                            // TODO: Highlight the errors on the code editor (if visible)
                            builderTextLogger.write("<font color='red'>Error: Relative address value out of range (" +
                                                    QString::number(immValue) + ") on line " +
                                                    QString::number(lineIdx + 1) + ".</font>");
                            return false;
                        }

                        curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].rel.opcode, 16).toUpper()
                                  + ", $" + QString::number(immValue, 16).toUpper();


                    } else {
                        bool ok;
                        int immValue = numberToInt(&ok, param0.mid(1));
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

                if ((param1.trimmed().replace(' ', "").toUpper() == ",X)")
                    && (AssemblerInstructionItems[instructionIdx].ind_x.cycles > 0)
                    && (param0.trimmed().at(0) == '(')) {
                    // ($##,X) // PREINDEXED INDIRECT
                } else if (param1.trimmed().replace(' ', "").toUpper() == ",X") {
                    bool ok;
                    int immValue = numberToInt(&ok, param0.mid(1));
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage or Absolute value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if ((AssemblerInstructionItems[instructionIdx].zpage_x.cycles > 0) && (immValue <= 0xFF)) {
                        // $##,X // ZEROPAGE INDEXED X

                    } else if ((AssemblerInstructionItems[instructionIdx].abs_x.cycles > 0) && (immValue <= 0xFFFF)) {
                        // $####,X // ABSOLUTE INDEXED X

                    } else {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage, X or Absolute, X value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }


                } else if ((param1.trimmed().replace(' ', "").toUpper() == ",Y)")
                    && (AssemblerInstructionItems[instructionIdx].ind_y.cycles > 0)
                            && (param0.trimmed().at(0) == '(')) {
                    //($##),Y // POSTINDEXED INDIRECT

                } else if (param1.trimmed().replace(' ', "").toUpper() == ",Y") {

                    bool ok;
                    int immValue = numberToInt(&ok, param0.mid(1));
                    if (!ok)
                    {
                        // TODO: Highlight the errors on the code editor (if visible)
                        builderTextLogger.write("<font color='red'>Error: Invalid ZeroPage or Absolute value specified "
                                                "on line " + QString::number(lineIdx + 1) + ".</font>");
                        return false;
                    }

                    if ((AssemblerInstructionItems[instructionIdx].zpage_x.cycles > 0) && (immValue <= 0xFF)) {
                        // $##,Y // ZEROPAGE INDEXED Y

                    } else if ((AssemblerInstructionItems[instructionIdx].abs_x.cycles > 0) && (immValue <= 0xFFFF)) {
                        // $####,Y // ABSOLUTE INDEXED Y

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

bool CSourceAssembler::isLabel(QString param)
{
    for (int i = 0; i < m_labelEntries.count(); i++)
    {
        if (m_labelEntries.at(i).labelName == param)
            return true;
    }
    return false;
}

int CSourceAssembler::numberToInt(bool *ok, QString number)
{

    if (number.at(0) == '$') {
        return number.mid(1).toInt(ok, 16);

    } else if (number.at(0) == '%') {
        int base = 0;
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
        return number.mid(1).toInt(ok, 0);
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
        builderTextLogger.write("Label Found [" + QString::number(lineIdx+1) + "]: " + labelEntry.labelName);
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
