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
#include <QUuid>

#include "cprojecttabwidget.h"

class IProjectTreeViewItem
{
public:
   void InitTreeItem(QString iconResource,IProjectTreeViewItem* parent = 0)
   {
      parentItem = parent;
      ident = new QUuid();
      (*ident) = QUuid::createUuid();
      if ( iconResource.isEmpty() )
      {
         _icon = QIcon(":/resources/RR_page_white.png");
      }
      else
      {
         _icon = QIcon(iconResource);
      }
   }

   QIcon icon()
   {
      return _icon;
   }

   QString uuid()
   {
      return ident->toString();
   }

   void setUuid(QString uuid)
   {
      if ( ident ) delete ident;
      ident = new QUuid(uuid);
   }

   void FreeTreeItem()
   {
      qDeleteAll(childItems);
   }

   void appendChild(IProjectTreeViewItem* child)
   {
      childItems.append(child);
   }

   void removeChild(IProjectTreeViewItem* child)
   {
      childItems.removeAll(child);
   }

   IProjectTreeViewItem* child(int row)
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
      {
         return parentItem->childItems.indexOf(const_cast<IProjectTreeViewItem*>(this));
      }

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

   IProjectTreeViewItem* parent()
   {
      return parentItem;
   }

   virtual QString caption() const = 0;
   virtual void openItemEvent(CProjectTabWidget* tabWidget) = 0;
   virtual void saveItemEvent() = 0;
   virtual void closeItemEvent() = 0;
   virtual bool canChangeName() = 0;
   virtual bool onNameChanged(QString newValue) = 0;
   virtual bool exportData() = 0;

   virtual ~IProjectTreeViewItem() {};

private:
   QList<IProjectTreeViewItem*> childItems;
   IProjectTreeViewItem* parentItem;
   QUuid* ident;
   QIcon _icon;
};

// Iterator class for walking through a project, starting from either a specific trunk
// node or from the project root node.
// Example usages:
//
// Walk the entire project printing the captions of each project item.
//   IProjectTreeViewItemIterator iter();
//   while ( iter.current() != NULL )
//   {
//      qDebug(iter.current()->caption().toLatin1().data());
//      iter.next();
//   }
//
// Walk a subtree of the project printing the captions of each project item.
//   IProjectTreeViewItemIterator iter(pCartridge);
//   while ( iter.current() != NULL )
//   {
//      qDebug(iter.current()->caption().toLatin1().data());
//      iter.next();
//   }
class IProjectTreeViewItemIterator
{
public:
   IProjectTreeViewItemIterator()
   {
      m_pBase = NULL; // CPTODO: fix this!
      m_pAt = NULL; // CPTODO: fix this!
      m_nodeIndex = 0;
   }

   IProjectTreeViewItemIterator(IProjectTreeViewItem* pTVI)
   {
      m_pBase = pTVI;
      m_pAt = pTVI;
      m_nodeIndex = 0;
   }

   void reset()
   {
      m_pAt = m_pBase;
      m_nodeIndex = 0;
   }

   void reset(IProjectTreeViewItem* pTVI)
   {
      m_pBase = pTVI;
      m_pAt = pTVI;
      m_nodeIndex = 0;
   }

   void next()
   {
      IProjectTreeViewItem* pNode = m_pBase;
      IProjectTreeViewItem* pAt = NULL;

      m_nodeIndex++;
      m_node = 0;

      walk(pNode,&pAt);

      m_pAt = pAt;
   }

   IProjectTreeViewItem* current()
   {
      return m_pAt;
   }

private:
   void walk(IProjectTreeViewItem* p, IProjectTreeViewItem** l)
   {
      if ( m_node == m_nodeIndex )
      {
         (*l) = p;
         return;
      }

      if ( !(*l) )
      {
         for (int i=0; i < p->childCount(); i++)
         {
            m_node++;
            walk(p->child(i),l);

            if ( (*l) )
            {
               break;
            }
         }
      }
   }

   IProjectTreeViewItem* m_pBase;
   IProjectTreeViewItem* m_pAt;
   int                   m_nodeIndex;
   int                   m_node;
};

#endif // IPROJECTTREEVIEWITEM_H
