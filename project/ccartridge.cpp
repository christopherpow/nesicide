#include "ccartridge.h"

CCartridge::CCartridge()
{
    m_enumMirrorMode = GameMirrorMode::NoMirroring;
    m_mapperNumber = 0;
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

CPRGROMBanks *CCartridge::getPointerToPrgRomBanks()
{
    return m_pointerToPrgRomBanks;
}

CCHRROMBanks *CCartridge::getPointerToChrRomBanks()
{
    return m_pointerToChrRomBanks;
}

bool CCartridge::serialize(QDomDocument &doc, QDomNode &node)
{
   // Create the root element for the cartridge
   QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

   // Export the iNES header
   cartridgeElement.setAttribute("mapperNumber", m_mapperNumber);
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

bool CCartridge::deserialize(QDomDocument &doc, QDomNode &node)
{
   // Read in the DOM element
   QDomElement cartridgeElement = doc.documentElement();

   setMapperNumber(cartridgeElement.attribute("mapperNumber").toInt());
   setMirrorMode((GameMirrorMode::eGameMirrorMode)
   cartridgeElement.attribute("mirrorMode").toInt());
   setBatteryBackedRam(cartridgeElement.attribute("hasBatteryBackedRam").toInt() == 1);

   // Export the PRG-ROM banks
   if (!m_pointerToPrgRomBanks->deserialize(doc, cartridgeElement))
      return false;

   // Export the CHR-ROM banks
   if (!m_pointerToChrRomBanks->deserialize(doc, cartridgeElement))
      return false;

   return true;
}

QString CCartridge::caption() const
{
    return "Cartridge";
}

GameMirrorMode::eGameMirrorMode CCartridge::getMirrorMode()
{
    return m_enumMirrorMode;
}

void CCartridge::setMirrorMode(GameMirrorMode::eGameMirrorMode enumValue)
{
    m_enumMirrorMode = enumValue;
}

qint8 CCartridge::getMapperNumber()
{
    return m_mapperNumber;
}

void CCartridge::setMapperNumber(qint8 mapperNumber)
{
    m_mapperNumber = mapperNumber;
}

bool CCartridge::isBatteryBackedRam()
{
    return m_hasBatteryBackedRam;
}

void CCartridge::setBatteryBackedRam(bool hasBatteryBackedRam)
{
    m_hasBatteryBackedRam = hasBatteryBackedRam;
}
