#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include <QtGlobal>

#if defined ( IDE_BUILD )
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombanks.h"
#include "cchrrombanks.h"
#else
#include "cnesrom.h"
#endif

namespace GameMirrorMode {
    typedef enum {
        NoMirroring = 0,
        HorizontalMirroring = 1,
        VerticalMirroring = 2,
        FourScreenMirroring = 3
    } eGameMirrorMode;
}

#if defined ( IDE_BUILD )
class CCartridge : public IXMLSerializable, public IProjectTreeViewItem
#else
class CCartridge
#endif
{
public:
    CCartridge();
    virtual ~CCartridge();

    // Member Getters
    GameMirrorMode::eGameMirrorMode getMirrorMode();
    qint8 getMapperNumber();
    bool isBatteryBackedRam();

#if defined ( IDE_BUILD )
    CPRGROMBanks *getPointerToPrgRomBanks();
    CCHRROMBanks *getPointerToChrRomBanks();
#else
    int getNumPrgRomBanks() { return m_numPrgBanks; }
    char* getPointerToPrgRomBank(int bank) { return m_prgBank[bank]; }
    void setNumPrgRomBanks(int banks) { m_numPrgBanks = banks; }
    int getNumChrRomBanks() { return m_numChrBanks; }
    char* getPointerToChrRomBank(int bank) { return m_chrBank[bank]; }
    void setNumChrRomBanks(int banks) { m_numChrBanks = banks; }
#endif

    // Member Setters
    void setMirrorMode(GameMirrorMode::eGameMirrorMode enumValue);
    void setMapperNumber(qint8 indexOfValue);
    void setBatteryBackedRam(bool hasBatteryBackedRam);

#if defined ( IDE_BUILD )
    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }
#endif

private:
#if defined ( IDE_BUILD )
    CPRGROMBanks *m_pointerToPrgRomBanks;
    CCHRROMBanks *m_pointerToChrRomBanks;
#else
    int m_numPrgBanks;
    char m_prgBank [ NUM_ROM_BANKS ][ MEM_16KB ];
    int m_numChrBanks;
    char m_chrBank [ NUM_ROM_BANKS ][ MEM_8KB ];
#endif
    GameMirrorMode::eGameMirrorMode m_enumMirrorMode;               // Mirror mode used in the emulator
    bool m_hasBatteryBackedRam;                                     // Memory can be saved via RAM kept valid with a battery
    qint8 m_mapperNumber;                                           // Numeric ID of the cartridge mapper

};

#endif // CCARTRIDGE_H
