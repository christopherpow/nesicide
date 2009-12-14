#include "ccartridge.h"

CCartridge::CCartridge()
{
    m_pointerToPrgRomBanks = new CPRGROMBanks();
    m_pointerToPrgRomBanks->InitTreeItem(this);
    this->appendChild(m_pointerToPrgRomBanks);

    m_pointerToChrRomBanks = new CCHRROMBanks();
    m_pointerToChrRomBanks->InitTreeItem(this);
    this->appendChild(m_pointerToChrRomBanks);
}

CCartridge::~CCartridge()
{
    if (m_pointerToChrRomBanks)
        delete m_pointerToChrRomBanks;

    if (m_pointerToPrgRomBanks)
        delete m_pointerToPrgRomBanks;
}

CPRGROMBanks *CCartridge::get_pointerToPrgRomBanks()
{
    return m_pointerToPrgRomBanks;
}

CCHRROMBanks *CCartridge::get_pointerToChrRomBanks()
{
    return m_pointerToChrRomBanks;
}

bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the cartridge
    QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

    // Export the PRG-ROM banks
    if (!m_pointerToPrgRomBanks->serialize(doc, cartridgeElement))
        return false;

    // Export the CHR-ROM banks
    if (!m_pointerToChrRomBanks->serialize(doc, cartridgeElement))
        return false;

    return true;
}

bool CCartridge::deserialize(QDomDocument &, QDomNode &)
{
    return false;
}


QString CCartridge::caption() const
{
    return "Cartridge";
}
