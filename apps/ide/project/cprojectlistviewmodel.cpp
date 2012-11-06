#include "cprojectlistviewmodel.h"

CProjectListViewModel::CProjectListViewModel(QObject* parent)
   : QAbstractItemModel(parent)
{
}

QVariant CProjectListViewModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   return QVariant();
}

Qt::ItemFlags CProjectListViewModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   if ( !index.isValid() )
   {
      return flags;
   }

   return flags;
}

bool CProjectListViewModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   return false;
}

QVariant CProjectListViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   return  QVariant();
}

QModelIndex CProjectListViewModel::index(int row, int column, const QModelIndex& parent) const
{
   return QModelIndex();
}

int CProjectListViewModel::rowCount(const QModelIndex& parent) const
{
   return 0;
}

int CProjectListViewModel::columnCount(const QModelIndex& parent) const
{
   return 1;
}

void CProjectListViewModel::layoutChangedEvent()
{
   this->layoutChanged();
}
