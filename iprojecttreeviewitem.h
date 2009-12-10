#ifndef IPROJECTTREEVIEWITEM_H
#define IPROJECTTREEVIEWITEM_H

#include <QList>
#include <QString>
#include <QVariant>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTreeView>
#include <QObject>
#include <QTableView>

class IProjectTreeViewItem
{

public:
    void InitTreeItem(IProjectTreeViewItem *parent = 0)
    {
        parentItem = parent;
    }

    void FreeTreeItem()
    {
         qDeleteAll(childItems);
    }

    void appendChild(IProjectTreeViewItem *child)
    {
         childItems.append(child);
    }

    IProjectTreeViewItem *child(int row)
    {
        return childItems.value(row);
    }

    int childCount() const
    {
        return childItems.count();
    }

    int row() const
    {
        if (parentItem)
                 return parentItem->childItems.indexOf(const_cast<IProjectTreeViewItem*>(this));

             return 0;
    }

    int columnCount() const
    {
        return 1;
    }

    QVariant data(int column) const
    {
        return caption();
    }

    IProjectTreeViewItem *parent()
    {
        return parentItem;
    }

    virtual QString caption() const = 0;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent) = 0;
    virtual void openItemEvent(QTabWidget *tabWidget) = 0;
private:
    QList<IProjectTreeViewItem*> childItems;
    IProjectTreeViewItem *parentItem;
};

#endif // IPROJECTTREEVIEWITEM_H
