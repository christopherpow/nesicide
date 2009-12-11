#include "ccartridge.h"

CCartridge::CCartridge()
{
    prgromBanks = new CPRGROMBanks();
    prgromBanks->InitTreeItem(this);
    this->appendChild(prgromBanks);

    chrromBanks = new CCHRROMBanks();
    chrromBanks->InitTreeItem(this);
    this->appendChild(chrromBanks);
}

bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

bool CCartridge::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}


QString CCartridge::caption() const
{
    return "Cartridge";
}
