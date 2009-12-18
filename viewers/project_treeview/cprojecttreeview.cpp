#include "cprojecttreeview.h"

CProjectTreeView::CProjectTreeView(QWidget *parent)
    : QTreeView(parent)
{
}

void CProjectTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    IProjectTreeViewItem *item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem *)this->indexAt(event->pos()).internalPointer());
    if (item)
        item->contextMenuEvent(event, (QTreeView *)this);

}

void CProjectTreeView::mouseDoubleClickEvent (QMouseEvent * event)
{
    IProjectTreeViewItem *item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem *)this->indexAt(event->pos()).internalPointer());
    if (item)
        item->openItemEvent(mdiTabWidget);
}

void CProjectTreeView::keyPressEvent ( QKeyEvent * e )
{
    if ((e->key() == Qt::Key_Backspace))
    {
        e->accept();

        IProjectTreeViewItem *item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem *)this->selectedIndexes().first().internalPointer());
        if (!item)
            return;

        if (item->canChangeName())
            edit(this->selectedIndexes().first());
    }
    else
        e->ignore();
}
