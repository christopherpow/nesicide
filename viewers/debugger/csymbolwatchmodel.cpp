#include "csymbolwatchmodel.h"

#include "ccc65interface.h"
#include "emulator_core.h"

static char modelStringBuffer [ 2048 ];

static const char* CLICK_TO_ADD = "<click to add>";

CSymbolWatchModel::CSymbolWatchModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

CSymbolWatchModel::~CSymbolWatchModel()
{
}

Qt::ItemFlags CSymbolWatchModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( index.column() != 1 )
   {
      flags |= Qt::ItemIsEditable;
   }
   return flags;
}

QVariant CSymbolWatchModel::data(const QModelIndex& index, int role) const
{
   unsigned int addr;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < m_symbols.count() )
   {
      switch ( index.column() )
      {
         case 0:
            return m_symbols.at(index.row());
            break;
         case 1:
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()));
            if ( addr != 0xFFFFFFFF )
            {
               sprintf(modelStringBuffer,"%04X",addr);
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case 2:
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()));
            if ( addr != 0xFFFFFFFF )
            {
               sprintf(modelStringBuffer,"%02X",nesGetCPUMemory(addr));
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
      }
   }
   else
   {
      switch ( index.column() )
      {
         case 0:
            return QVariant(CLICK_TO_ADD);
            break;
         case 1:
            return QVariant();
            break;
         case 2:
            return QVariant();
            break;
      }
   }
   return QVariant();
}

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;
   unsigned int addr;

   switch ( index.column() )
   {
      case 0:
         if ( index.row() < m_symbols.count() )
         {
            m_symbols.replace(index.row(),value.toString());
            emit layoutChanged();
            ok = true;
         }
         else
         {
            if ( (!value.toString().isEmpty()) &&
                 (value != CLICK_TO_ADD) )
            {
               m_symbols.append(value.toString());
               emit layoutChanged();
               ok = true;
            }
         }
         break;
      case 1:
         ok = false;
         break;
      case 2:
         if ( index.row() < m_symbols.count() )
         {
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()));
            if ( addr != 0xFFFFFFFF )
            {
               nesSetCPUMemory(addr,value.toString().toInt(&ok,16));
            }
            emit dataChanged(index,index);
         }
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
         return QString("Address");
         break;
      case 2:
         return QString("Value");
         break;
      }
   }
}

int CSymbolWatchModel::rowCount(const QModelIndex&) const
{
   return m_symbols.count()+1;
}

int CSymbolWatchModel::columnCount(const QModelIndex&) const
{
   return 3;
}

void CSymbolWatchModel::update()
{
   emit layoutChanged();
}

void CSymbolWatchModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < m_symbols.count() )
   {
      beginRemoveRows(parent,row,row);
      m_symbols.removeAt(row);
      endRemoveRows();
   }
}

void CSymbolWatchModel::insertRow(QString text, const QModelIndex& parent)
{
   beginInsertRows(parent,m_symbols.count(),m_symbols.count());
   m_symbols.append(text);
   endInsertRows();
}
