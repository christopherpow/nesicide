#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include <QString>
#include <QMessageBox>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "ccartridge.h"
#include "defaultnespalette.h"
#include "iprojecttreeviewitem.h"
#include "cproject.h"

typedef QColor CPaletteEntry;

class CNesicideProject : public IXMLSerializable, public IProjectTreeViewItem
{

public:
    // Global Constructor / Destructors
    CNesicideProject();
    virtual ~CNesicideProject();

    // Search functions
    int findSource ( char* objname, char** objdata, int* size );

    // Helper functions
    void initializeProject();
    bool createProjectFromRom(QString fileName);
    void terminateProject();

    // Member Getters
    QString get_projectTitle();
    QList<CPaletteEntry> *get_pointerToListOfProjectPaletteEntries();
    bool get_isInitialized();
    CCartridge *get_pointerToCartridge();
    CProject *getProject();

    // Member Setters
    void set_projectTitle(QString value);
    void set_pointerToListOfProjectPaletteEntries(QList<CPaletteEntry> *pointerToListOfProjectPaletteEntries);
    void set_pointerToCartridge(CCartridge *pointerToCartridge);
    void setProject(CProject *project);

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
    void initializeNodes();

    QString m_projectTitle;                                         // The visible title of the project
    QList<CPaletteEntry> *m_pointerToListOfProjectPaletteEntries;   // List of palette entries for the emulator.
    bool m_isInitialized;                                           // Is the project initialized?

    CProject *m_pointerToProject;
    CCartridge *m_pointerToCartridge;

};

extern CNesicideProject *nesicideProject;

#endif // CNESICIDEPROJECT_H
