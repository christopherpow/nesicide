#include <QFileDialog>

#include "cnesicideproject.h"

#include "ccartridge.h"

CCartridge::CCartridge(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);

   // Initialize this node's attributes
   m_mirrorMode = HorizontalMirroring;
   m_mapperNumber = 0;
   m_hasBatteryBackedRam = false;
   m_fourScreen = false;

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

// CPTODO: PUT THIS IN NEW INFRASTRUCTURE JSOLO
//void CCartridge::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
//{
//   const QString EXPORT_ROM_TEXT = "Export NES ROM";

//   QMenu menu(parent);
//   menu.addAction(EXPORT_ROM_TEXT);

//   QAction* ret = menu.exec(event->globalPos());

//   if (ret)
//   {
//      if (ret->text() == EXPORT_ROM_TEXT)
//      {
//         exportROM();
//      }
//   }
//}

void CCartridge::exportROM()
{
    QString fileName = QFileDialog::getSaveFileName(NULL,"Export NES ROM",QDir::currentPath(),"iNES Files (*.nes)");

    if ( !fileName.isEmpty() )
    {
        CNesicideProject::instance()->createRomFromProject(fileName);
    }
}

void CCartridge::initializeProject()
{
   // Initialize this node's attributes
   m_mirrorMode = HorizontalMirroring;
   m_mapperNumber = 0;
   m_hasBatteryBackedRam = false;
   m_fourScreen = false;

   // Initialize child nodes
   m_pPrgRomBanks->initializeProject();
   m_pChrRomBanks->initializeProject();

   // Add child nodes to tree
   appendChild(m_pPrgRomBanks);
   appendChild(m_pChrRomBanks);
}

void CCartridge::terminateProject()
{
   // Initialize this node's attributes
   m_mirrorMode = HorizontalMirroring;
   m_mapperNumber = 0;
   m_hasBatteryBackedRam = false;
   m_fourScreen = false;

   // Terminate child nodes
   m_pPrgRomBanks->terminateProject();
   m_pChrRomBanks->terminateProject();

   // Remove child nodes from tree
   removeChild(m_pPrgRomBanks);
   removeChild(m_pChrRomBanks);
}

bool CCartridge::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the cartridge
   QDomElement cartridgeElement = addElement( doc, node, "cartridge" );

   // Export the iNES header
   cartridgeElement.setAttribute("mapperNumber", m_mapperNumber);
   cartridgeElement.setAttribute("mirrorMode", m_mirrorMode);
   cartridgeElement.setAttribute("hasBatteryBackedRam", m_hasBatteryBackedRam);
   cartridgeElement.setAttribute("fourScreen",m_fourScreen);

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

bool CCartridge::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   // Read in the DOM element
   QDomElement cartridgeElement = node.toElement();

   setMapperNumber(cartridgeElement.attribute("mapperNumber").toInt());
   setMirrorMode((eMirrorMode)cartridgeElement.attribute("mirrorMode").toInt());
   setBatteryBackedRam(cartridgeElement.attribute("hasBatteryBackedRam").toInt() == 1);
   setFourScreen(cartridgeElement.attribute("fourScreen").toInt() == 1);

   // Import the PRG-ROM banks
   if (!m_pPrgRomBanks->deserialize(doc,cartridgeElement,errors))
   {
      return false;
   }

   // Import the CHR-ROM banks
   if (!m_pChrRomBanks->deserialize(doc,cartridgeElement,errors))
   {
      return false;
   }

   return true;
}

QString CCartridge::caption() const
{
   return "Cartridge";
}
