#include "cprgrombanks.h"

CPRGROMBanks::CPRGROMBanks()
{
    banks.clear();
}

bool CPRGROMBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM banks
    QDomElement prgromElement = addElement( doc, node, "prgrombanks" );

    for (int i=0; i<banks.count(); i++)
        if (!banks[i]->serialize(doc, prgromElement))
            return false;

    return true;
}

bool CPRGROMBanks::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CPRGROMBanks::caption() const
{
    return "PRG-ROM Banks";
}
