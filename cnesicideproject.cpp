#include "cnesicideproject.h"
#include "main.h"

CNesicideProject::CNesicideProject()
{
    m_isInitialized = false;
    m_pointerToCartridge = (CCartridge *)NULL;
    m_pointerToListOfProjectPaletteEntries = new QList<CPaletteEntry>();
    m_pointerToProject = (CProject*)NULL;
    m_projectTitle = "(No project loaded)";
}

CNesicideProject::~CNesicideProject()
{
    if (m_pointerToCartridge)
        delete m_pointerToCartridge;

    if (m_pointerToProject)
        delete m_pointerToProject;

    if (m_pointerToListOfProjectPaletteEntries)
        delete m_pointerToListOfProjectPaletteEntries;
}


QList<CPaletteEntry> *CNesicideProject::get_pointerToListOfProjectPaletteEntries()
{
    return m_pointerToListOfProjectPaletteEntries;
}

void CNesicideProject::set_pointerToListOfProjectPaletteEntries
        (QList<CPaletteEntry> *pointerToListOfProjectPaletteEntries)
{
    m_pointerToListOfProjectPaletteEntries = pointerToListOfProjectPaletteEntries;
}

bool CNesicideProject::get_isInitialized()
{
    return m_isInitialized;
}


void CNesicideProject::initializeProject()
{
    // Set the parent of this tree item to NULL since it is the top level item.
    this->InitTreeItem();

    // Create our Project node and append it as a child to this tree
    m_pointerToProject = new CProject();
    m_pointerToProject->InitTreeItem(this);
    this->appendChild(m_pointerToProject);

    // Create our Cartridge node and append it as a child to this tree
    m_pointerToCartridge = new CCartridge();
    m_pointerToCartridge->InitTreeItem(this);
    this->appendChild(m_pointerToCartridge);

    // Load the default palette into the project
    if (!m_pointerToListOfProjectPaletteEntries)
        m_pointerToListOfProjectPaletteEntries = new QList<CPaletteEntry>();

    m_pointerToListOfProjectPaletteEntries->clear();
    for (int row=0; row <= 0x3; row++) {
        for (int col=0; col <= 0xF; col++){
            m_pointerToListOfProjectPaletteEntries->append(QColor(defaultPalette[(row << 4) + col][0],
                                                                  defaultPalette[(row << 4) + col][1],
                                                                  defaultPalette[(row << 4) + col][2]));
        }
    }

    // Notify the fact that the project data has been initialized properly
    m_isInitialized = true;
}

bool CNesicideProject::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create a DOM element to define the node this object represents, and add it to the
    // parent node assigned in from the caller.
    QDomElement projectElement = addElement( doc, node, "nesicideproject" );

    // Set some variables as tags to this node.
    projectElement.setAttribute("version", 0.2);
    projectElement.setAttribute("title", m_projectTitle);
    projectElement.setAttribute("mirrorMode", m_pointerToCartridge->get_enumMirrorMode());
    projectElement.setAttribute("hasBatteryBackedRam", m_pointerToCartridge->get_hasBatteryBackedRam());

    // Create the root palette element, and give it a version attribute
    QDomElement rootPaletteElement = addElement( doc, projectElement, "nesicidepalette" );

    // Loop through all palette entries, and for each entry add an <entry /> tag that has the
    // index, as well as the RGB properties of the palette.
    for (int indexOfCurrentPaletteEntry=0; indexOfCurrentPaletteEntry <= 0x3F; indexOfCurrentPaletteEntry++)
    {
        QDomElement elm = addElement( doc, rootPaletteElement, "entry");
        elm.setAttribute("index", indexOfCurrentPaletteEntry);
        elm.setAttribute("r", m_pointerToListOfProjectPaletteEntries->at(indexOfCurrentPaletteEntry).red());
        elm.setAttribute("g", m_pointerToListOfProjectPaletteEntries->at(indexOfCurrentPaletteEntry).green());
        elm.setAttribute("b", m_pointerToListOfProjectPaletteEntries->at(indexOfCurrentPaletteEntry).blue());
    }

    // Now serialize all child objects
    if (m_pointerToCartridge)
    {
        if (!m_pointerToCartridge->serialize(doc, projectElement))
            return false;
    } else
        return false;

    if (m_pointerToProject)
    {
        if (!m_pointerToProject->serialize(doc, projectElement))
            return false;
    } else
        return false;

    // CPTODO: implement this
    // CNES::serialize(doc, projectElement);

    return true;
}

bool CNesicideProject::deserialize(QDomDocument&, QDomNode&)
{
    return true;
}

QString CNesicideProject::caption() const
{
    return QString("Project");
}

CCartridge *CNesicideProject::get_pointerToCartridge()
{
    return m_pointerToCartridge;
}

void CNesicideProject::set_pointerToCartridge(CCartridge *pointerToCartridge)
{
    m_pointerToCartridge = pointerToCartridge;
}

QString CNesicideProject::get_projectTitle()
{
    return m_projectTitle;
}

void CNesicideProject::set_projectTitle(QString value)
{
    m_projectTitle = value;
}


bool CNesicideProject::createProjectFromRom(QString fileName)
{
    // Make sure our pointers are in order..
    if (!m_pointerToCartridge)
        m_pointerToCartridge = new CCartridge();

    if (!m_pointerToCartridge->get_pointerToChrRomBanks())
        return false;

    if (!m_pointerToCartridge->get_pointerToPrgRomBanks())
        return false;


    QFile fileIn (fileName);

    if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
    {
        m_projectTitle = "Imported ROM";
        initializeProject();

        QDataStream fs(&fileIn);

        // Check the NES header
        char nesHeader[5] = {'N', 'E', 'S', 0x1A, 0x00};
        char nesTest[5] = {0, 0, 0, 0, 0};
        for (int i=0; i<4; i++)
            fs >> (qint8&)nesTest[i];
        if (strcmp(nesHeader, nesTest))
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
            m_pointerToCartridge->set_enumMirrorMode(GameMirrorMode::FourScreenMirroring);
        else if (romCB1 & 0x01)
            m_pointerToCartridge->set_enumMirrorMode(GameMirrorMode::VerticalMirroring);
        else
            m_pointerToCartridge->set_enumMirrorMode(GameMirrorMode::HorizontalMirroring);

        // Now extract the two flags (battery backed ram and trainer)
        m_pointerToCartridge->set_hasBatteryBackedRam(romCB1 & 0x02);
        bool hasTrainer = (romCB1 & 0x04);

        // Extract the four lower bits of the mapper number
        m_pointerToCartridge->set_indexOfMapperNumber(romCB1 >> 4);

        // ROM Control Byte 2:
        // • Bits 0-3 - Reserved for future usage and should all be 0.
        // • Bits 4-7 - Four upper bits of the mapper number.
        qint8 romCB2;
        fs >> romCB2;

        // Extract the upper four bits of the mapper number
        m_pointerToCartridge->set_indexOfMapperNumber(m_pointerToCartridge->get_indexOfMapperNumber() | (romCB2 & 0xF0));

        // Number of 8 KB RAM banks. For compatibility with previous
        // versions of the iNES format, assume 1 page of RAM when
        // this is 0.
        qint8 numRamBanks;
        fs >> numRamBanks;
        if (numRamBanks == 0)
            numRamBanks = 1;

        // Skip the 7 reserved bytes
        qint8 skip;
        for (int i=0; i<7; i++)
            fs >> skip;

        // Extract the trainer (if it exists)
        if (hasTrainer)
        {
            // TODO: Handle trainer. Skipping for now.
            for (int i=0; i<512; i++)
                fs >> skip;
        }

        // Load the PRG-ROM banks (16kb at 0x4000 bytes)
        for (int bank=0; bank<numPrgRomBanks; bank++)
        {
            // Create the ROM bank and load in the binary data
            CPRGROMBank *romBank = new CPRGROMBank();
            romBank->set_indexOfPrgRomBank(
                    m_pointerToCartridge->get_pointerToPrgRomBanks()->get_pointerToArrayOfBanks()->count());
            for (int i=0; i<0x4000; i++)
                fs >> romBank->get_pointerToBankData()[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(m_pointerToCartridge->get_pointerToPrgRomBanks());
            m_pointerToCartridge->get_pointerToPrgRomBanks()->appendChild(romBank);
            m_pointerToCartridge->get_pointerToPrgRomBanks()->get_pointerToArrayOfBanks()->append(romBank);

        }

        // Load the CHR-ROM banks (8kb at 0x2000 bytes)
        for (int bank=0; bank<numChrRomBanks; bank++)
        {
            // Create the ROM bank and load in the binary data
            CCHRROMBank *romBank = new CCHRROMBank();
            romBank->bankID = m_pointerToCartridge->get_pointerToChrRomBanks()->banks.count();
            for (int i=0; i<0x2000; i++)
                fs >> romBank->data[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(m_pointerToCartridge->get_pointerToChrRomBanks());
            m_pointerToCartridge->get_pointerToChrRomBanks()->appendChild(romBank);
            m_pointerToCartridge->get_pointerToChrRomBanks()->banks.append(romBank);

        }

        emulator->setCartridge(m_pointerToCartridge);

        fileIn.close();
    }

    return true;
}
