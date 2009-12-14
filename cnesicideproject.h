#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include <QString>
#include <QMessageBox>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprojectprimitives.h"
#include "ccartridge.h"
#include "defaultnespalette.h"

namespace GameMirrorMode {
    typedef enum {
        NoMirroring = 0,
        HorizontalMirroring = 1,
        VerticalMirroring = 2,
        FourScreenMirroring = 3
    } eGameMirrorMode;
}

typedef QColor CPaletteEntry;

class CNesicideProject : public IXMLSerializable, public IProjectTreeViewItem
{

public:
    // Global Constructor / Destructors
    CNesicideProject();
    ~CNesicideProject();

    // Public Functionality
    void initializeProject();
    bool createProjectFromRom(QString fileName);

    // Member Getters
    QString get_projectTitle();
    GameMirrorMode::eGameMirrorMode get_enumMirrorMode();
    qint8 get_indexOfMapperNumber();
    bool get_hasBatteryBackedRam();
    QList<CPaletteEntry> *get_pointerToListOfProjectPaletteEntries();
    bool get_isInitialized();

    // Member Setters
    void set_projectTitle(QString value);
    void set_enumMirrorMode(GameMirrorMode::eGameMirrorMode enumValue);
    void set_indexOfMapperNumber(qint8 indexOfValue);
    void set_hasBatteryBackedRam(bool hasBatteryBackedRam);
    void set_pointerToListOfProjectPaletteEntries(QList<CPaletteEntry> *pointerToListOfProjectPaletteEntries);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}

private:
    QString m_projectTitle;                                         // The visible title of the project
    GameMirrorMode::eGameMirrorMode m_enumMirrorMode;               // Mirror mode used in the emulator
    qint8 m_indexOfMapperNumber;                                    // Numeric ID of the cartridge mapper
    bool m_hasBatteryBackedRam;                                     // Memory can be saved via RAM kept valid with a battery
    QList<CPaletteEntry> *m_pointerToListOfProjectPaletteEntries;   // List of palette entries for the emulator.
    bool m_isInitialized;                                           // Is the project initialized?

    CProjectPrimitives *m_pointerToProjectPrimitives;
    CCartridge *m_pointerToCartridge;

};

#endif // CNESICIDEPROJECT_H
