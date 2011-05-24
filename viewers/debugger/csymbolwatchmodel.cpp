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
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < symbols.count() )
   {
      switch ( index.column() )
      {
         case 0:
            return symbols.at(index.row());
            break;
         case 1:
            return QVariant("23f0");
            break;
      }
   }
   else
   {
      switch ( index.column() )
      {
         case 0:
            return QVariant("<click to add>");
            break;
         case 1:
            return QVariant();
            break;
      }
   }
   return QVariant();
}

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;

   switch ( index.column() )
   {
   case 0:
      if ( index.row() < symbols.count() )
      {
         symbols.replace(index.row(),value.toString());
         emit layoutChanged();
         ok = true;
      }
      else
      {
         symbols.append(value.toString());
         emit layoutChanged();
         ok = true;
      }
      break;
   case 1:
      emit dataChanged(index,index);
      ok = true;
      break;
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
   return symbols.count()+1;
}

int CSymbolWatchModel::columnCount(const QModelIndex&) const
{
   return 2;
}

void CSymbolWatchModel::update()
{
   emit layoutChanged();
}
