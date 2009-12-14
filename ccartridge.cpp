#include "ccartridge.h"

CCartridge::CCartridge()
{
    prgromBanks = new CPRGROMBanks();
    prgromBanks->InitTreeItem(this);
    this->appendChild(prgromBanks);

    chrromBanks = new CCHRROMBanks();
    chrromBanks->InitTreeItem(this);
    this->appendChild(chrromBanks);

    mapperNumber = 0; // no mapper default
}

bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the cartridge
    QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

    // Export the iNES header
    cartridgeElement.setAttribute("mapperNumber", this->mapperNumber);
    cartridgeElement.setAttribute("mirrorMode", this->mirrorMode);
    cartridgeElement.setAttribute("hasBatteryBackedRam", this->hasBatteryBackedRam);

    // Export the PRG-ROM banks
    if (!prgromBanks->serialize(doc, cartridgeElement))
        return false;

    // Export the CHR-ROM banks
    if (!chrromBanks->serialize(doc, cartridgeElement))
        return false;

    return true;
}

bool CCartridge::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}


QString CCartridge::caption() const
{
    return "Cartridge";
}
