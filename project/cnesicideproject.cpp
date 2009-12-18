#include "cnesicideproject.h"
#include "main.h"

CNesicideProject *nesicideProject;

CNesicideProject::CNesicideProject()
{
    m_isInitialized = false;
    m_pointerToCartridge = (CCartridge *)NULL;
    m_pointerToListOfProjectPaletteEntries = new QList<CPaletteEntry>();
    m_pointerToProject = (CProject*)NULL;
    m_projectTitle = "(No project loaded)";
    InitTreeItem();
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
    initializeNodes();

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

void CNesicideProject::initializeNodes()
{
    if (m_pointerToCartridge)
    {
        this->removeChild(m_pointerToCartridge);
        delete m_pointerToCartridge;
    }

    if (m_pointerToProject)
    {
        this->removeChild(m_pointerToProject);
        delete m_pointerToProject;
    }

    // No need to keep deleting and reloading this, just clear it
    m_pointerToListOfProjectPaletteEntries->clear();


    // Create our Project node and append it as a child to this tree
    m_pointerToProject = new CProject();
    m_pointerToProject->InitTreeItem(this);
    this->appendChild(m_pointerToProject);

    // Create our Cartridge node and append it as a child to this tree
    m_pointerToCartridge = new CCartridge();
    m_pointerToCartridge->InitTreeItem(this);
    this->appendChild(m_pointerToCartridge);


}

bool CNesicideProject::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create a DOM element to define the node this object represents, and add it to the
    // parent node assigned in from the caller.
    QDomElement projectElement = addElement( doc, node, "nesicideproject" );

    // Set some variables as tags to this node.
    projectElement.setAttribute("version", 0.2);
    projectElement.setAttribute("title", m_projectTitle);
    projectElement.setAttribute("mirrorMode", m_pointerToCartridge->getMirrorMode());
    projectElement.setAttribute("hasBatteryBackedRam", m_pointerToCartridge->isBatteryBackedRam());

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

bool CNesicideProject::deserialize(QDomDocument &doc, QDomNode &node)
{
    m_isInitialized = false;
    initializeNodes();

    // Read in the DOM element
    QDomElement projectElement = doc.documentElement();

    if (projectElement.isNull())
        return false;

    // For now, error out if the file version is not what we expect it to be. Eventually
    // we need to split up the loader into versions for backwards compatibility.
    if (projectElement.attribute("version", "") != "0.2")
        return false;

    // Load our properties. Note that the default value is returned if an attribute is missing.
    // This is the expected behavior.
    m_projectTitle = projectElement.attribute("title", "Untitled Project");
    m_pointerToCartridge->setMirrorMode((GameMirrorMode::eGameMirrorMode)
                                             projectElement.attribute("mirrorMode").toInt());
    m_pointerToCartridge->setBatteryBackedRam(projectElement.attribute("hasBatteryBackedRam").toInt() == 1);

    // Initialize the palette.
    for (int row=0; row <= 0x3; row++) {
        for (int col=0; col <= 0xF; col++){
            m_pointerToListOfProjectPaletteEntries->append(QColor(defaultPalette[(row << 4) + col][0],
                                                                  defaultPalette[(row << 4) + col][1],
                                                                  defaultPalette[(row << 4) + col][2]));
        }
    }

    // Now loop through the child elements and process the ones we find
    QDomNode child = projectElement.firstChild();
    do
    {
        if (child.nodeName() == "nesicidepalette") {

            QDomNode paletteNode = child.firstChild();
            do
            {
                QDomElement paletteItem = paletteNode.toElement();

                if (paletteItem.isNull())
                    return false;

                if ((!paletteItem.hasAttribute("index"))
                    || (!paletteItem.hasAttribute("r"))
                    || (!paletteItem.hasAttribute("g"))
                    || (!paletteItem.hasAttribute("b")))
                    return false;

                int nodeIndex = paletteItem.attribute("index").toInt();

                if ((nodeIndex < 0) || (nodeIndex > 0x3F))
                    return false;

                CPaletteEntry palEntry;
                m_pointerToListOfProjectPaletteEntries->replace(nodeIndex,
                    QColor(paletteItem.attribute("r").toInt(),
                           paletteItem.attribute("g").toInt(), paletteItem.attribute("b").toInt()));

            } while (!(paletteNode = paletteNode.nextSibling()).isNull());

        } else if (child.nodeName() == "project") {

            if (!m_pointerToProject->deserialize(doc, child))
                return false;

        } else if (child.nodeName() == "cartridge") {

        } else
            return false;

    } while (!(child = child.nextSibling()).isNull());


    m_isInitialized = true;
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

    if (!m_pointerToCartridge->getPointerToChrRomBanks())
        return false;

    if (!m_pointerToCartridge->getPointerToPrgRomBanks())
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
            m_pointerToCartridge->setMirrorMode(GameMirrorMode::FourScreenMirroring);
        else if (romCB1 & 0x01)
            m_pointerToCartridge->setMirrorMode(GameMirrorMode::VerticalMirroring);
        else
            m_pointerToCartridge->setMirrorMode(GameMirrorMode::HorizontalMirroring);

        // Now extract the two flags (battery backed ram and trainer)
        m_pointerToCartridge->setBatteryBackedRam(romCB1 & 0x02);
        bool hasTrainer = (romCB1 & 0x04);

        // Extract the four lower bits of the mapper number
        m_pointerToCartridge->setMapperNumber(romCB1 >> 4);

        // ROM Control Byte 2:
        // • Bits 0-3 - Reserved for future usage and should all be 0.
        // • Bits 4-7 - Four upper bits of the mapper number.
        qint8 romCB2;
        fs >> romCB2;

        // Extract the upper four bits of the mapper number
        m_pointerToCartridge->setMapperNumber(m_pointerToCartridge->getMapperNumber() | (romCB2 & 0xF0));

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
                    m_pointerToCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->count());
            for (int i=0; i<0x4000; i++)
                fs >> romBank->get_pointerToBankData()[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(m_pointerToCartridge->getPointerToPrgRomBanks());
            m_pointerToCartridge->getPointerToPrgRomBanks()->appendChild(romBank);
            m_pointerToCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->append(romBank);

        }

        // Load the CHR-ROM banks (8kb at 0x2000 bytes)
        for (int bank=0; bank<numChrRomBanks; bank++)
        {
            // Create the ROM bank and load in the binary data
            CCHRROMBank *romBank = new CCHRROMBank();
            romBank->bankID = m_pointerToCartridge->getPointerToChrRomBanks()->banks.count();
            for (int i=0; i<0x2000; i++)
                fs >> romBank->data[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(m_pointerToCartridge->getPointerToChrRomBanks());
            m_pointerToCartridge->getPointerToChrRomBanks()->appendChild(romBank);
            m_pointerToCartridge->getPointerToChrRomBanks()->banks.append(romBank);

        }

        emulator->setCartridge(m_pointerToCartridge);

        fileIn.close();
    }

    return true;
}
