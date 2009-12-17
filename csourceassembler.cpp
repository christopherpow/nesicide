#include "csourceassembler.h"

CSourceAssembler::CSourceAssembler()
{
}

qint8 *CSourceAssembler::assemble(CSourceItem *rootSource)
{
    qint8 *binaryData = new qint8[0x4000];
    memset(binaryData, 0, 0x4000);
    int instructionPointer = 0;

    QStringList source = rootSource->get_sourceCode().split('\n', QString::SkipEmptyParts);


    return binaryData;
}

void CSourceAssembler::stripComments(QStringList *source)
{
    QStringList list = source->replaceInStrings(QRegExp("[^\"];.*$"), "");
    source->clear();
    source->append(list);
}
