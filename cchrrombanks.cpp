#include "cchrrombanks.h"

CCHRROMBanks::CCHRROMBanks()
{
    banks.clear();
}

// IXMLSerializable Interface Implementation
bool CCHRROMBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

// IProjectTreeViewItem Interface Implmentation
QString CCHRROMBanks::caption() const
{
    return "CHR-ROM Banks";
}
