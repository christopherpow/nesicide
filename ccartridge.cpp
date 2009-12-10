#include "ccartridge.h"

CCartridge::CCartridge()
{
    prgromBanks = new CPRGROMBanks();
    prgromBanks->InitTreeItem(this);
    this->appendChild(prgromBanks);
}

bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}


QString CCartridge::caption() const
{
    return "Cartridge";
}
