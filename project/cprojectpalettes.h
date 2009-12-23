#ifndef CPROJECTPALETTES_H
#define CPROJECTPALETTES_H

#include "iprojecttreeviewitem.h"
#include "cprojectpaletteitem.h"
#include "cprojecttreeviewmodel.h"

class CProjectPalettes : public IProjectTreeViewItem
{

public:
    CProjectPalettes();
    virtual ~CProjectPalettes();
    void newPaletteEvent();

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }

};

#endif // CPROJECTPALETTES_H
