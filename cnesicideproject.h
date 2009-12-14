#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include <QString>
#include <QMessageBox>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprojectprimitives.h"
#include "ccartridge.h"
#include "defaultnespalette.h"

class CNesicideProject : public IXMLSerializable, public IProjectTreeViewItem
{

public:

    CNesicideProject();
    ~CNesicideProject();

    // The following properties must be saved/loaded with the project file =============
    QString ProjectTitle; // The visible title of the project
    QList<QColor> projectPalette;
    // =================================================================================

    // Helper functions
    void initializeProject();
    bool getIsInitialized();
    void createProjectFromRom(QString fileName);

    // Project container classes
    CProjectPrimitives *projectPrimitives;
    CCartridge *cartridge;

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent) {};
    virtual void openItemEvent(QTabWidget *tabWidget) {};

private:
    bool isInitialized;
};

#endif // CNESICIDEPROJECT_H
