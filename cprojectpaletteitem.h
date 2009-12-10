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
};

#endif // CPROJECTPALETTEITEM_H
