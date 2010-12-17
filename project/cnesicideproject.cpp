#include "cnesicideproject.h"
#include "main.h"

#include "cnessystempalette.h"

extern "C" int PASM_include ( char* objname, char** objdata, int* size )
{
   if ( nesicideProject )
   {
      return nesicideProject->findSource ( objname, objdata, size );
   }

   return 0;
}

CNesicideProject::CNesicideProject()
{
   m_isInitialized = false;
   m_pProjectPaletteEntries = new QList<CPaletteEntry>();
   m_pProject = (CProject*)NULL;
   m_pCartridge = (CCartridge*)NULL;
   m_projectTitle = "(No project loaded)";
   InitTreeItem();
}

CNesicideProject::~CNesicideProject()
{
   if (m_pCartridge)
   {
      delete m_pCartridge;
   }

   if (m_pProject)
   {
      delete m_pProject;
   }

   if (m_pProjectPaletteEntries)
   {
      delete m_pProjectPaletteEntries;
   }
}

int CNesicideProject::findSource ( char* objname, char** objdata, int* size )
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* source;

   (*objdata) = NULL;
   (*size) = 0;

   while ( iter.current() != NULL )
   {
      if ( iter.current()->caption() == objname )
      {
         source = dynamic_cast<CSourceItem*>(iter.current());
         if ( source )
         {
            (*objdata) = source->get_sourceCode().toLatin1().data();
            (*size) = strlen((*objdata));
         }
         break;
      }
      
      iter.next();
   }
   return (*size);
}

QList<CPaletteEntry> *CNesicideProject::getProjectPaletteEntries()
{
   return m_pProjectPaletteEntries;
}

void CNesicideProject::setProjectPaletteEntries
(QList<CPaletteEntry> *pProjectPaletteEntries)
{
   m_pProjectPaletteEntries = pProjectPaletteEntries;
}

bool CNesicideProject::isInitialized()
{
   return m_isInitialized;
}

CProject* CNesicideProject::getProject()
{
   return m_pProject;
}

void CNesicideProject::setProject(CProject* project)
{
   m_pProject = project;
}

void CNesicideProject::initializeProject()
{
   initializeNodes();

   // Load the default palette into the project
   if (!m_pProjectPaletteEntries)
   {
      m_pProjectPaletteEntries = new QList<CPaletteEntry>();
   }

   m_pProjectPaletteEntries->clear();

   for (int col=0; col <= 64; col++)
   {
      m_pProjectPaletteEntries->append(QColor(nesGetPaletteRedComponent(col),
                                              nesGetPaletteGreenComponent(col),
                                              nesGetPaletteBlueComponent(col)));
   }

   // Notify the fact that the project data has been initialized properly
   m_isInitialized = true;
}

void CNesicideProject::terminateProject()
{
   // Destroying the project node destroys everything else
   if (m_pCartridge)
   {
      this->removeChild(m_pCartridge);
      delete m_pCartridge;
      m_pCartridge = NULL;
   }

   if (m_pProject)
   {
      this->removeChild(m_pProject);
      delete m_pProject;
      m_pProject = NULL;
   }

   // Notify the fact that the project data is no longer valid
   m_isInitialized = false;
}


void CNesicideProject::initializeNodes()
{
   if (m_pCartridge)
   {
      this->removeChild(m_pCartridge);
      delete m_pCartridge;
   }

   if (m_pProject)
   {
      this->removeChild(m_pProject);
      delete m_pProject;
   }

   // No need to keep deleting and reloading this, just clear it
   m_pProjectPaletteEntries->clear();

   // Create our Project node and append it as a child to this tree
   m_pProject = new CProject();
   m_pProject->InitTreeItem(this);
   this->appendChild(m_pProject);

   // Create our Cartridge node and append it as a child to this tree
   m_pCartridge = new CCartridge();
   m_pCartridge->InitTreeItem(this);
   this->appendChild(m_pCartridge);
}

bool CNesicideProject::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create a DOM element to define the node this object represents, and add it to the
   // parent node assigned in from the caller.
   QDomElement projectElement = addElement( doc, node, "nesicideproject" );

   // Set some variables as tags to this node.
   projectElement.setAttribute("version", 0.3);
   projectElement.setAttribute("title", m_projectTitle);

   // Create the root palette element, and give it a version attribute
   QDomElement rootPaletteElement = addElement( doc, projectElement, "nesicidepalette" );

   // Loop through all palette entries, and for each entry add an <entry /> tag that has the
   // index, as well as the RGB properties of the palette.
   for (int indexOfCurrentPaletteEntry=0; indexOfCurrentPaletteEntry <= 0x3F; indexOfCurrentPaletteEntry++)
   {
      QDomElement elm = addElement( doc, rootPaletteElement, "entry");
      elm.setAttribute("index", indexOfCurrentPaletteEntry);
      elm.setAttribute("r", m_pProjectPaletteEntries->at(indexOfCurrentPaletteEntry).red());
      elm.setAttribute("g", m_pProjectPaletteEntries->at(indexOfCurrentPaletteEntry).green());
      elm.setAttribute("b", m_pProjectPaletteEntries->at(indexOfCurrentPaletteEntry).blue());
   }

   // Now serialize all child objects
   if (m_pCartridge)
   {
      if (!m_pCartridge->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   if (m_pProject)
   {
      if (!m_pProject->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   // CPTODO: implement this
   // CNES::serialize(doc, projectElement);

   return true;
}

bool CNesicideProject::deserialize(QDomDocument& doc, QDomNode& node)
{
   m_isInitialized = false;
   initializeNodes();

   // Read in the DOM element
   QDomElement projectElement = doc.documentElement();

   if (projectElement.isNull())
   {
      return false;
   }

   // For now, error out if the file version is not what we expect it to be. Eventually
   // we need to split up the loader into versions for backwards compatibility.
   if (projectElement.attribute("version", "") != "0.3")
   {
      return false;
   }

   // Load our properties. Note that the default value is returned if an attribute is missing.
   // This is the expected behavior.
   m_projectTitle = projectElement.attribute("title", "Untitled Project");

   // Initialize the palette.
   for (int color = 0; color < 64; color++)
   {
      m_pProjectPaletteEntries->append(QColor(nesGetPaletteRedComponent(color),
                                              nesGetPaletteGreenComponent(color),
                                              nesGetPaletteBlueComponent(color)));
   }

   // Now loop through the child elements and process the ones we find
   QDomNode child = projectElement.firstChild();

   do
   {
      if (child.nodeName() == "nesicidepalette")
      {

         QDomNode paletteNode = child.firstChild();

         do
         {
            QDomElement paletteItem = paletteNode.toElement();

            if (paletteItem.isNull())
            {
               return false;
            }

            if ((!paletteItem.hasAttribute("index"))
                  || (!paletteItem.hasAttribute("r"))
                  || (!paletteItem.hasAttribute("g"))
                  || (!paletteItem.hasAttribute("b")))
            {
               return false;
            }

            int nodeIndex = paletteItem.attribute("index").toInt();

            if ((nodeIndex < 0) || (nodeIndex > 0x3F))
            {
               return false;
            }

            CPaletteEntry palEntry;
            m_pProjectPaletteEntries->replace(nodeIndex,
                                              QColor(paletteItem.attribute("r").toInt(),
                                                    paletteItem.attribute("g").toInt(), paletteItem.attribute("b").toInt()));

         }
         while (!(paletteNode = paletteNode.nextSibling()).isNull());

      }
      else if (child.nodeName() == "project")
      {

         if (!m_pProject->deserialize(doc, child))
         {
            return false;
         }

      }
      else if (child.nodeName() == "cartridge")
      {

      }
      else
      {
         return false;
      }

   }
   while (!(child = child.nextSibling()).isNull());

   m_isInitialized = true;
   return true;
}

QString CNesicideProject::caption() const
{
   return QString("NESICIDE");
}

CCartridge* CNesicideProject::getCartridge()
{
   return m_pCartridge;
}

void CNesicideProject::setCartridge(CCartridge* pCartridge)
{
   m_pCartridge = pCartridge;
}

QString CNesicideProject::getProjectTitle()
{
   return m_projectTitle;
}

void CNesicideProject::setProjectTitle(QString value)
{
   m_projectTitle = value;
}


bool CNesicideProject::createProjectFromRom(QString fileName)
{
   QString str;

   // Make sure our pointers are in order..
   if (!m_pCartridge)
   {
      m_pCartridge = new CCartridge();
   }

   if (!m_pCartridge->getPointerToChrRomBanks())
   {
      return false;
   }

   if (!m_pCartridge->getPointerToPrgRomBanks())
   {
      return false;
   }


   QFile fileIn (fileName);

   if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
   {
      m_projectTitle = "Imported ROM";
      initializeProject();

      QDataStream fs(&fileIn);

      // Check the NES header
      char nesHeader[4] = {'N', 'E', 'S', 0x1A};
      char nesTest[4] = {0, 0, 0, 0};
      fs.readRawData(nesTest,4);

      if (memcmp(nesHeader, nesTest,4))
      {
         // Header check failed, quit
         fileIn.close();
         QMessageBox::information(0, "Error", "Invalid ROM format.\nCannot create project.");
         return false;
      }

      // Number of 16 KB PRG-ROM banks
      qint8 numPrgRomBanks;
      fs >> numPrgRomBanks;

      // Get the number of 8 KB CHR-ROM / VROM banks
      qint8 numChrRomBanks;
      fs >> numChrRomBanks;

      // ROM Control Byte 1:
      // - Bit 0 - Indicates the type of mirroring used by the game
      //   where 0 indicates horizontal mirroring, 1 indicates
      //   vertical mirroring.
      //
      // - Bit 1 - Indicates the presence of battery-backed RAM at
      //   memory locations $6000-$7FFF.
      //
      // - Bit 2 - Indicates the presence of a 512-byte trainer at
      //   memory locations $7000-$71FF.
      //
      // - Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
      //   mirroring should be used.
      //
      // - Bits 4-7 - Four lower bits of the mapper number.
      qint8 romCB1;
      fs >> romCB1;

      // First extract the mirror mode
      if (romCB1 & 0x08)
      {
         m_pCartridge->setMirrorMode(GameMirrorMode::FourScreenMirroring);
      }
      else if (romCB1 & 0x01)
      {
         m_pCartridge->setMirrorMode(GameMirrorMode::VerticalMirroring);
      }
      else
      {
         m_pCartridge->setMirrorMode(GameMirrorMode::HorizontalMirroring);
      }

      // Now extract the two flags (battery backed ram and trainer)
      m_pCartridge->setBatteryBackedRam(romCB1 & 0x02);
      bool hasTrainer = (romCB1 & 0x04);

      // ROM Control Byte 2:
      //  Bits 0-3 - Reserved for future usage and should all be 0.
      //  Bits 4-7 - Four upper bits of the mapper number.
      qint8 romCB2;
      fs >> romCB2;

      if ( romCB2&0x0F )
      {
         romCB2 = 0x00;
         QMessageBox::information(0, "Warning", "Invalid iNES header format.\nSave the project to fix.");
      }

      // Extract the upper four bits of the mapper number
      m_pCartridge->setMapperNumber(((romCB1>>4)&0x0F)|(romCB2&0xF0));

      // Number of 8 KB RAM banks. For compatibility with previous
      // versions of the iNES format, assume 1 page of RAM when
      // this is 0.
      qint8 numRamBanks;
      fs >> numRamBanks;

      if (numRamBanks == 0)
      {
         numRamBanks = 1;
      }

      // Skip the 7 reserved bytes
      qint8 skip;

      for (int i=0; i<7; i++)
      {
         fs >> skip;
      }

      // Extract the trainer (if it exists)
      if (hasTrainer)
      {
         // TODO: Handle trainer. Skipping for now.
         for (int i=0; i<512; i++)
         {
            fs >> skip;
         }
      }

      // Load the PRG-ROM banks (16KB each)
      for (int bank=0; bank<numPrgRomBanks; bank++)
      {
         // Create the ROM bank and load in the binary data
         CPRGROMBank* romBank = new CPRGROMBank();
         romBank->set_indexOfPrgRomBank(
            m_pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->count());
         fs.readRawData((char*)romBank->get_pointerToBankData(),0x4000);

         // Attach the rom bank to the rom banks object
         romBank->InitTreeItem(m_pCartridge->getPointerToPrgRomBanks());
         m_pCartridge->getPointerToPrgRomBanks()->appendChild(romBank);
         m_pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->append(romBank);

      }

      // Load the CHR-ROM banks (8KB each)
      for (int bank=0; bank<numChrRomBanks; bank++)
      {
         // Create the ROM bank and load in the binary data
         CCHRROMBank* romBank = new CCHRROMBank();
         romBank->bankID = m_pCartridge->getPointerToChrRomBanks()->banks.count();
         fs.readRawData((char*)romBank->data,0x2000);

         // Attach the rom bank to the rom banks object
         romBank->InitTreeItem(m_pCartridge->getPointerToChrRomBanks());
         m_pCartridge->getPointerToChrRomBanks()->appendChild(romBank);
         m_pCartridge->getPointerToChrRomBanks()->banks.append(romBank);

      }

      generalTextLogger.clear();

      str = "<b>Searcing internal game database: ";
      str += gameDatabase.getGameDBAuthor();
      str += ", ";
      str += gameDatabase.getGameDBTimestamp();
      str += "...</b>";
      generalTextLogger.write(str);

      bool gameFoundInDB = gameDatabase.find(m_pCartridge);

      if ( gameFoundInDB )
      {
         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SHA1: ";
         str += gameDatabase.getSHA1();
         generalTextLogger.write(str);

         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Name: ";
         str += gameDatabase.getName();
         generalTextLogger.write(str);

         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Publisher (date): ";
         str += gameDatabase.getPublisher();
         str += " (";
         str += gameDatabase.getDate();
         str += ")";
         generalTextLogger.write(str);

         str = "NESICIDE - ";
         str += gameDatabase.getName();

         // Set main window title...
         nesicideWindow->setWindowTitle(str);

         // Do NTSC/PAL autodetecting

         // Check cartridge validity?
      }
      else
      {
         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i><font color=\"red\">Not found.</font></i>";
         generalTextLogger.write(str);

         str = "NESICIDE - ";
         str += fileName;
         str += " [Homebrew]";

         // Set main window title...
         nesicideWindow->setWindowTitle(str);
      }

      str = "<b>Game loaded.</b>";
      generalTextLogger.write(str);

      fileIn.close();
   }

   return true;
}
