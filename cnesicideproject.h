#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include <QString>
#include <QMessageBox>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprojectprimitives.h"
#include "ccartridge.h"
#include "defaultnespalette.h"

typedef QColor CPaletteEntry;

class CNesicideProject : public IXMLSerializable, public IProjectTreeViewItem
{

public:
    // Global Constructor / Destructors
    CNesicideProject();
    ~CNesicideProject();


    // Helper functions
    void initializeProject();
    bool createProjectFromRom(QString fileName);

    // Member Getters
    QString get_projectTitle();
    QList<CPaletteEntry> *get_pointerToListOfProjectPaletteEntries();
    bool get_isInitialized();
    CCartridge *get_pointerToCartridge();

    // Member Setters
    void set_projectTitle(QString value);
    void set_pointerToListOfProjectPaletteEntries(QList<CPaletteEntry> *pointerToListOfProjectPaletteEntries);
    void set_pointerToCartridge(CCartridge *pointerToCartridge);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}

private:
    QString m_projectTitle;                                         // The visible title of the project
    QList<CPaletteEntry> *m_pointerToListOfProjectPaletteEntries;   // List of palette entries for the emulator.
    bool m_isInitialized;                                           // Is the project initialized?

    CProjectPrimitives *m_pointerToProjectPrimitives;
    CCartridge *m_pointerToCartridge;

};

#endif // CNESICIDEPROJECT_H
