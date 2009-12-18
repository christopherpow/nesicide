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

    void removeChild(IProjectTreeViewItem *child)
    {
        childItems.removeAll(child);
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

    QVariant data(int) const
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
    virtual bool onCloseQuery() = 0;
    virtual void onClose() = 0;
    virtual int getTabIndex() = 0;
    virtual bool isDocumentSaveable() = 0;
    virtual void onSaveDocument() = 0;
private:
    QList<IProjectTreeViewItem*> childItems;
    IProjectTreeViewItem *parentItem;
};

#endif // IPROJECTTREEVIEWITEM_H
