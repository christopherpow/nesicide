#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include <QString>
#include <QMessageBox>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprojectprimitives.h"
#include "ccartridge.h"

namespace GameMirrorMode {
    typedef enum {
        NoMirroring = 0,
        HorizontalMirroring = 1,
        VerticalMirroring = 2,
        FourScreenMirroring = 3
    } eGameMirrorMode;
}

class CNesicideProject : public IXMLSerializable, public IProjectTreeViewItem
{

public:

    CNesicideProject();
    ~CNesicideProject();

    QString ProjectTitle; // The visible title of the project
    GameMirrorMode::eGameMirrorMode mirrorMode;
    qint8 mapperNumber;
    bool hasBatteryBackedRam;

    void initializeProject();
    bool getIsInitialized();
    void createProjectFromRom(QString fileName);

    // Project container classes
    CProjectPrimitives *projectPrimitives;
    CCartridge *cartridge;

    // IXMLSerializable Interface Implementation
    bool serialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent) {};
    virtual void openItemEvent(QTabWidget *tabWidget) {};

private:
    bool isInitialized;
};

#endif // CNESICIDEPROJECT_H
