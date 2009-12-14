#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

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
    ~CCartridge();

    // Member Getters
    GameMirrorMode::eGameMirrorMode get_enumMirrorMode();
    qint8 get_indexOfMapperNumber();
    bool get_hasBatteryBackedRam();
    CPRGROMBanks *get_pointerToPrgRomBanks();
    CCHRROMBanks *get_pointerToChrRomBanks();

    // Member Setters
    void set_enumMirrorMode(GameMirrorMode::eGameMirrorMode enumValue);
    void set_indexOfMapperNumber(qint8 indexOfValue);
    void set_hasBatteryBackedRam(bool hasBatteryBackedRam);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}

private:
    CPRGROMBanks *m_pointerToPrgRomBanks;
    CCHRROMBanks *m_pointerToChrRomBanks;
    GameMirrorMode::eGameMirrorMode m_enumMirrorMode;               // Mirror mode used in the emulator
    bool m_hasBatteryBackedRam;                                     // Memory can be saved via RAM kept valid with a battery
    qint8 m_indexOfMapperNumber;                                    // Numeric ID of the cartridge mapper

};

#endif // CCARTRIDGE_H
