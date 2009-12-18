#ifndef CPROJECTPALETTEITEM_H
#define CPROJECTPALETTEITEM_H

#include <QString>
#include <QTreeWidgetItem>

#include "iprojecttreeviewitem.h"
#include "paletteeditorwindow.h"

class CProjectPaletteItem : public IProjectTreeViewItem
{
public:
    CProjectPaletteItem();
    QString name;

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget *tabWidget);
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }
};

#endif // CPROJECTPALETTEITEM_H
