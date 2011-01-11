#include "cprojecttreeview.h"

CProjectTreeView::CProjectTreeView(QWidget* parent)
   : QTreeView(parent)
{
}

void CProjectTreeView::contextMenuEvent(QContextMenuEvent* event)
{
   IProjectTreeViewItem* item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem*)this->indexAt(event->pos()).internalPointer());

   if (item)
   {
      item->contextMenuEvent(event, (QTreeView*)this);
   }
}

void CProjectTreeView::mouseDoubleClickEvent (QMouseEvent* event)
{
   IProjectTreeViewItem* item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem*)this->indexAt(event->pos()).internalPointer());

   if (item)
   {
      item->openItemEvent(m_pTarget);
      emit projectTreeView_openItem(item->caption());
   }
}

void CProjectTreeView::keyPressEvent ( QKeyEvent* e )
{
   if ((e->key() == Qt::Key_Backspace))
   {
      e->accept();

      IProjectTreeViewItem* item = const_cast<IProjectTreeViewItem*>((IProjectTreeViewItem*)this->selectedIndexes().first().internalPointer());

      if (!item)
      {
         return;
      }

      if (item->canChangeName())
      {
         edit(this->selectedIndexes().first());
      }
   }
   else
   {
      e->ignore();
   }
}

void CProjectTreeView::fileNavigator_fileChanged(QString file)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource && 
           (pSource->caption() == file) )
      {
         pSource->openItemEvent(m_pTarget);
         emit projectTreeView_openItem(pSource->caption());
         break;
      }
      iter.next();
   }
}

void CProjectTreeView::fileNavigator_symbolChanged(QString file, QString symbol, int linenumber)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource &&
           (pSource->caption() == file) )
      {
         pSource->openItemEvent(m_pTarget);
//         emit projectTreeView_ensureItemOpen(pSource->caption());
         pSource->getEditor()->selectLine(linenumber);
         break;
      }
      iter.next();
   }
}
