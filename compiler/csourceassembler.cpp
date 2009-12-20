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
    if (!convertOpcodesToDBs(source))
        return false;


    builderTextLogger.write("--------------------------------------------------------------------------");
    for (int i=0; i < source->length(); i++)
        builderTextLogger.write(source->value(i));
    builderTextLogger.write("--------------------------------------------------------------------------");


    delete binaryData;
    return true;
}

bool CSourceAssembler::stripComments(QStringList *source)
{
    QStringList list = source->replaceInStrings(QRegExp("[^\"];.*$"), "");
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
    return true;
}
