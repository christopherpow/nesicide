#include "csymbolwatchmodel.h"

CSymbolWatchModel::CSymbolWatchModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

CSymbolWatchModel::~CSymbolWatchModel()
{
}

Qt::ItemFlags CSymbolWatchModel::flags(const QModelIndex&) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CSymbolWatchModel::data(const QModelIndex& index, int role) const
{
   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   switch ( index.column() )
   {
      case 0:
         return QVariant("symbol");
         break;
      case 1:
         return QVariant("23f0");
         break;
   }

   return QVariant();
}

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;

   if ( index.column() == 1 )
   {
      emit dataChanged(index,index);
      ok = true;
   }
   return ok;
}

QVariant CSymbolWatchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case 0:
         return QString("Symbol");
         break;
      case 1:
         return QString("Value");
         break;
      }
   }
}

int CSymbolWatchModel::rowCount(const QModelIndex&) const
{
   return 1;
}

int CSymbolWatchModel::columnCount(const QModelIndex&) const
{
   return 2;
}

void CSymbolWatchModel::update()
{
   emit layoutChanged();
}
