#include "cchrrombanks.h"

CCHRROMBanks::CCHRROMBanks()
{
    banks.clear();
}

// IXMLSerializable Interface Implementation
bool CCHRROMBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM banks
    QDomElement chrromElement = addElement( doc, node, "chrrombanks" );

    for (int i=0; i<banks.count(); i++)
        if (!banks.at(i)->serialize(doc, chrromElement))
            return false;

    return true;
}

bool CCHRROMBanks::deserialize(QDomDocument &, QDomNode &)
{
    return false;
}

// IProjectTreeViewItem Interface Implmentation
QString CCHRROMBanks::caption() const
{
    return "CHR-ROM Banks";
}
