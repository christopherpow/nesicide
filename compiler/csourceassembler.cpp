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
            if ((curLine == firstWord) && (AssemblerInstructionItems[instructionIdx].impl.cycles > 0))
            {
                curLine = ".db $" + QString::number(AssemblerInstructionItems[instructionIdx].impl.opcode, 16).toUpper();
            }

        }

        source->replace(lineIdx, curLine);
    }
    return true;
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
