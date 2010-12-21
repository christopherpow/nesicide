#include "ccartridge.h"

CCartridge::CCartridge(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);

   // Initialize this node's attributes
   m_mirrorMode = GameMirrorMode::NoMirroring;
   m_mapperNumber = 0;
   m_hasBatteryBackedRam = false;

   // Allocate children
   m_pPrgRomBanks = new CPRGROMBanks(this);
   m_pChrRomBanks = new CCHRROMBanks(this);
}

CCartridge::~CCartridge()
{
   if (m_pChrRomBanks)
   {
      delete m_pChrRomBanks;
   }

   if (m_pPrgRomBanks)
   {
      delete m_pPrgRomBanks;
   }
}

void CCartridge::initializeProject()
{
   // Initialize this node's attributes
   m_mirrorMode = GameMirrorMode::NoMirroring;
   m_mapperNumber = 0;
   m_hasBatteryBackedRam = false;
   
   // Initialize child nodes
   m_pPrgRomBanks->initializeProject();
   m_pChrRomBanks->initializeProject();
   
   // Add child nodes to tree
   appendChild(m_pPrgRomBanks);
   appendChild(m_pChrRomBanks);
}

void CCartridge::terminateProject()
{
   // Terminate child nodes
   m_pPrgRomBanks->terminateProject();
   m_pChrRomBanks->terminateProject();
   
   // Remove child nodes from tree
   removeChild(m_pPrgRomBanks);
   removeChild(m_pChrRomBanks);
}

CPRGROMBanks* CCartridge::getPrgRomBanks()
{
   return m_pPrgRomBanks;
}

CCHRROMBanks* CCartridge::getChrRomBanks()
{
   return m_pChrRomBanks;
}

bool CCartridge::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the cartridge
   QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

   // Export the iNES header
   cartridgeElement.setAttribute("mapperNumber", m_mapperNumber);
   cartridgeElement.setAttribute("mirrorMode", m_mirrorMode);
   cartridgeElement.setAttribute("hasBatteryBackedRam", m_hasBatteryBackedRam);

   // Export the PRG-ROM banks
   if (!m_pPrgRomBanks->serialize(doc, cartridgeElement))
   {
      return false;
   }

   // Export the CHR-ROM banks
   if (!m_pChrRomBanks->serialize(doc, cartridgeElement))
   {
      return false;
   }

   return true;
}

bool CCartridge::deserialize(QDomDocument& doc, QDomNode& node)
{
   // Read in the DOM element
   QDomElement cartridgeElement = doc.documentElement();

   setMapperNumber(cartridgeElement.attribute("mapperNumber").toInt());
   setMirrorMode((GameMirrorMode::eGameMirrorMode)
                 cartridgeElement.attribute("mirrorMode").toInt());
   setBatteryBackedRam(cartridgeElement.attribute("hasBatteryBackedRam").toInt() == 1);

   // Import the PRG-ROM banks
   if (!m_pPrgRomBanks->deserialize(doc, cartridgeElement))
   {
      return false;
   }

   // Import the CHR-ROM banks
   if (!m_pChrRomBanks->deserialize(doc, cartridgeElement))
   {
      return false;
   }

   return true;
}

QString CCartridge::caption() const
{
   return "Cartridge";
}

GameMirrorMode::eGameMirrorMode CCartridge::getMirrorMode()
{
   return m_mirrorMode;
}

void CCartridge::setMirrorMode(GameMirrorMode::eGameMirrorMode enumValue)
{
   m_mirrorMode = enumValue;
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
