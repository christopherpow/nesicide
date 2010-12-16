#include "cprojecttreeviewmodel.h"

CProjectTreeViewModel::CProjectTreeViewModel(QObject* parent, IProjectTreeViewItem* root)
   : QAbstractItemModel(parent)
{
   rootItem = root;
}

QVariant CProjectTreeViewModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   IProjectTreeViewItem* item = static_cast<IProjectTreeViewItem*>(index.internalPointer());

   return item->caption();
}

Qt::ItemFlags CProjectTreeViewModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   if ( !index.isValid() )
   {
      return flags;
   }

   IProjectTreeViewItem* item = static_cast<IProjectTreeViewItem*>(index.internalPointer());

   if (!item)
   {
      return flags;
   }

   if (item->canChangeName())
   {
      flags |= Qt::ItemIsEditable;
   }

   return flags;
}

bool CProjectTreeViewModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   IProjectTreeViewItem* item = static_cast<IProjectTreeViewItem*>(index.internalPointer());
   return item->onNameChanged(value.toString());
}

QVariant CProjectTreeViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   return  QString("");
}

QModelIndex CProjectTreeViewModel::index(int row, int column, const QModelIndex& parent) const
{
   if (!rootItem)
   {
      return QModelIndex();
   }

   IProjectTreeViewItem* parentItem;

   if (!parent.isValid())
   {
      parentItem = rootItem;
   }
   else
   {
      parentItem = static_cast<IProjectTreeViewItem*>(parent.internalPointer());
   }

   IProjectTreeViewItem* childItem = parentItem->child(row);

   if (childItem)
   {
      return createIndex(row, column, childItem);
   }
   else
   {
      return QModelIndex();
   }
}

QModelIndex CProjectTreeViewModel::parent(const QModelIndex& index) const
{
   if (!index.isValid())
   {
      return QModelIndex();
   }

   IProjectTreeViewItem* childItem = static_cast<IProjectTreeViewItem*>(index.internalPointer());
   IProjectTreeViewItem* parentItem = childItem->parent();

   if (parentItem == rootItem)
   {
      return QModelIndex();
   }

   return createIndex(parentItem->row(), 0, parentItem);
}

int CProjectTreeViewModel::rowCount(const QModelIndex& parent) const
{
   IProjectTreeViewItem* parentItem;

   if (parent.column() > 0)
   {
      return 0;
   }

   if (!parent.isValid())
   {
      parentItem = rootItem;
   }
   else
   {
      parentItem = static_cast<IProjectTreeViewItem*>(parent.internalPointer());
   }


   return parentItem->childCount();
}

int CProjectTreeViewModel::columnCount(const QModelIndex& parent) const
{
   return 1;
}

void CProjectTreeViewModel::layoutChangedEvent()
{
   this->layoutChanged();

}
