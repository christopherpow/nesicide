#include "cnesicideproject.h"

CNesicideProject::CNesicideProject()
{
    this->ProjectTitle = "(No project loaded)";
    this->mirrorMode = GameMirrorMode::NoMirroring;
    isInitialized = false;
}

void CNesicideProject::initializeProject()
{
    this->InitTreeItem();

    projectPrimitives = new CProjectPrimitives();
    projectPrimitives->InitTreeItem(this);
    this->appendChild(projectPrimitives);

    cartridge = new CCartridge();
    cartridge->InitTreeItem(this);
    this->appendChild(cartridge);

    // Load the default palette
    projectPalette.clear();
    for (int row=0; row <= 0x3; row++) {
        for (int col=0; col <= 0xF; col++){
            projectPalette.append(QColor(defaultPalette[(row << 4) + col][0],
                                         defaultPalette[(row << 4) + col][1],
                                         defaultPalette[(row << 4) + col][2]));
        }
    }

    isInitialized = true;
}

bool CNesicideProject::getIsInitialized()
{
    return this->isInitialized;
}

CNesicideProject::~CNesicideProject()
{
    delete projectPrimitives;
}

bool CNesicideProject::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement projectElement = addElement( doc, node, "nesicideproject" );
    projectElement.setAttribute("version", 0.2);
    projectElement.setAttribute("title", this->ProjectTitle);
    projectElement.setAttribute("mirrorMode", this->mirrorMode);
    projectElement.setAttribute("hasBatteryBackedRam", this->hasBatteryBackedRam);

    // Add the palette data

    // Create the root palette element, and give it a version attribute
    QDomElement rootPaletteElement = addElement( doc, projectElement, "nesicidepalette" );

    // Loop through all palette entries, and for each entry add an <entry /> tag that has the
    // index, as well as the RGB properties of the palette.
    for (int i=0; i <= 0x3F; i++)
    {
        QDomElement elm = addElement( doc, rootPaletteElement, "entry");
        elm.setAttribute("index", i);
        elm.setAttribute("r", projectPalette.at(i).red());
        elm.setAttribute("g", projectPalette.at(i).green());
        elm.setAttribute("b", projectPalette.at(i).blue());
    }

    // Now serialize all child objects
    if (!cartridge->serialize(doc, projectElement))
        return false;

    return true;
}

bool CNesicideProject::deserialize(QDomDocument &doc, QDomNode &node)
{
    return true;
}

QString CNesicideProject::caption() const
{
    return QString("Project");
}



void CNesicideProject::createProjectFromRom(QString fileName)
{
    QFile fileIn (fileName);


    if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
    {
        this->ProjectTitle = "Imported ROM";
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
            return;
        }
        // Number of 16 KB PRG-ROM banks
        qint8 numPrgRomBanks;
        fs >> numPrgRomBanks;

        // Get the number of 8 KB CHR-ROM / VROM banks
        qint8 numChrRomBanks;
        fs >> numChrRomBanks;

        // ROM Control Byte 1:
        // • Bit 0 - Indicates the type of mirroring used by the game
        //   where 0 indicates horizontal mirroring, 1 indicates
        //   vertical mirroring.
        //
        // • Bit 1 - Indicates the presence of battery-backed RAM at
        //   memory locations $6000-$7FFF.
        //
        // • Bit 2 - Indicates the presence of a 512-byte trainer at
        //   memory locations $7000-$71FF.
        //
        // • Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
        //   mirroring should be used.
        //
        // • Bits 4-7 - Four lower bits of the mapper number.
        qint8 romCB1;
        fs >> romCB1;

        // First extract the mirror mode
        if (romCB1 & 0x08)
            this->mirrorMode = GameMirrorMode::FourScreenMirroring;
        else if (romCB1 & 0x01)
            this->mirrorMode = GameMirrorMode::VerticalMirroring;
        else
            this->mirrorMode = GameMirrorMode::HorizontalMirroring;

        // Now extract the two flags (battery backed ram and trainer)
        hasBatteryBackedRam = (romCB1 & 0x02);
        bool hasTrainer = (romCB1 & 0x04);

        // Extract the four lower bits of the mapper number
        this->mapperNumber = (romCB1 >> 4);

        // ROM Control Byte 2:
        // • Bits 0-3 - Reserved for future usage and should all be 0.
        // • Bits 4-7 - Four upper bits of the mapper number.
        qint8 romCB2;
        fs >> romCB2;

        // Extract the upper four bits of the mapper number
        this->mapperNumber |= (romCB2 & 0xF0);

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
            romBank->bankID = cartridge->prgromBanks->banks.count();
            for (int i=0; i<0x4000; i++)
                fs >> romBank->data[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(cartridge->prgromBanks);
            cartridge->prgromBanks->appendChild(romBank);
            cartridge->prgromBanks->banks.append(romBank);

        }

        // Load the CHR-ROM banks (8kb at 0x2000 bytes)
        for (int bank=0; bank<numChrRomBanks; bank++)
        {
            // Create the ROM bank and load in the binary data
            CCHRROMBank *romBank = new CCHRROMBank();
            romBank->bankID = cartridge->chrromBanks->banks.count();
            for (int i=0; i<0x2000; i++)
                fs >> romBank->data[i];

            // Attach the rom bank to the rom banks object
            romBank->InitTreeItem(cartridge->chrromBanks);
            cartridge->chrromBanks->appendChild(romBank);
            cartridge->chrromBanks->banks.append(romBank);

        }


        fileIn.close();
    }
}
