#include "ccartridge.h"

CCartridge::CCartridge()
{
    m_enumMirrorMode = GameMirrorMode::NoMirroring;
    m_indexOfMapperNumber = 0;
    m_hasBatteryBackedRam = false;

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


GameMirrorMode::eGameMirrorMode CCartridge::get_enumMirrorMode()
{
    return m_enumMirrorMode;
}

void CCartridge::set_enumMirrorMode(GameMirrorMode::eGameMirrorMode enumValue)
{
    m_enumMirrorMode = enumValue;
}

qint8 CCartridge::get_indexOfMapperNumber()
{
    return m_indexOfMapperNumber;
}

void CCartridge::set_indexOfMapperNumber(qint8 indexOfValue)
{
    m_indexOfMapperNumber = indexOfValue;
}

bool CCartridge::get_hasBatteryBackedRam()
{
    return m_hasBatteryBackedRam;
}

void CCartridge::set_hasBatteryBackedRam(bool hasBatteryBackedRam)
{
    m_hasBatteryBackedRam = hasBatteryBackedRam;
}


bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the cartridge
    QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

    // Export the iNES header
    cartridgeElement.setAttribute("mapperNumber", m_indexOfMapperNumber);
    cartridgeElement.setAttribute("mirrorMode", m_enumMirrorMode);
    cartridgeElement.setAttribute("hasBatteryBackedRam", m_hasBatteryBackedRam);

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
