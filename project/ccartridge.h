#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include <QtGlobal>

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombanks.h"
#include "cchrrombanks.h"

namespace GameMirrorMode {
    typedef enum {
        NoMirroring = 0,
        HorizontalMirroring = 1,
        VerticalMirroring = 2,
        FourScreenMirroring = 3
    } eGameMirrorMode;
}

class CCartridge : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CCartridge();
    virtual ~CCartridge();

    // Member Getters
    GameMirrorMode::eGameMirrorMode getMirrorMode();
    qint8 getMapperNumber();
    bool isBatteryBackedRam();

    CPRGROMBanks *getPointerToPrgRomBanks();
    CCHRROMBanks *getPointerToChrRomBanks();

    // Member Setters
    void setMirrorMode(GameMirrorMode::eGameMirrorMode enumValue);
    void setMapperNumber(qint8 indexOfValue);
    void setBatteryBackedRam(bool hasBatteryBackedRam);

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

private:
    CPRGROMBanks *m_pointerToPrgRomBanks;
    CCHRROMBanks *m_pointerToChrRomBanks;
    GameMirrorMode::eGameMirrorMode m_enumMirrorMode;               // Mirror mode used in the emulator
    bool m_hasBatteryBackedRam;                                     // Memory can be saved via RAM kept valid with a battery
    qint8 m_mapperNumber;                                           // Numeric ID of the cartridge mapper

};

#endif // CCARTRIDGE_H
