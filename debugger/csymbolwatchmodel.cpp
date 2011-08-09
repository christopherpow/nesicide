#include <QInputDialog>

#include "csymbolwatchmodel.h"

#include "ccc65interface.h"
#include "emulator_core.h"

static char modelStringBuffer [ 2048 ];

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CSymbolWatchModel::CSymbolWatchModel(bool editable,QObject *parent) :
    QAbstractTableModel(parent)
{
   m_currentSortColumn = SymbolWatchCol_Symbol;
   m_currentSortOrder = Qt::DescendingOrder;
   m_currentItemCount = 0;
   m_editable = editable;
}

CSymbolWatchModel::~CSymbolWatchModel()
{
}

Qt::ItemFlags CSymbolWatchModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( ((m_editable) &&
        (index.column() == SymbolWatchCol_Symbol)) ||
        (index.column() == SymbolWatchCol_Value) )
   {
      flags |= Qt::ItemIsEditable;
   }
   if ( (index.column() == SymbolWatchCol_Symbol) &&
        (index.row() < m_items.count()) )
   {
      flags |= Qt::ItemIsDragEnabled;
   }
   return flags;
}

QVariant CSymbolWatchModel::data(const QModelIndex& index, int role) const
{
   int addr;
   int absAddr;
   QString data;
   int count;
   int idx;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < m_items.count() )
   {
      // Get symbol's index in debug information from its segment.
      count = CCC65Interface::getSymbolMatchCount(m_items.at(index.row()).symbol);
      for ( idx = 0; idx < count; idx++ )
      {
         if ( m_items.at(index.row()).segment == CCC65Interface::getSymbolSegment(m_items.at(index.row()).symbol,idx) )
         {
            break;
         }
      }

      switch ( index.column() )
      {
         case SymbolWatchCol_Symbol:
            return m_items.at(index.row()).symbol;
            break;
         case SymbolWatchCol_Address:
            // Get symbol's information based on its name and index.
            addr = CCC65Interface::getSymbolAddress(m_items.at(index.row()).symbol,idx);
            absAddr = CCC65Interface::getSymbolAbsoluteAddress(m_items.at(index.row()).symbol,idx);
            if ( addr != -1 )
            {
               nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case SymbolWatchCol_Size:
            return QVariant(m_items.at(index.row()).size);
            break;
         case SymbolWatchCol_Value:
            addr = CCC65Interface::getSymbolAddress(m_items.at(index.row()).symbol,idx);
            if ( addr != -1 )
            {
               sprintf(modelStringBuffer,"%02X",nesGetMemory(addr));
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case SymbolWatchCol_File:
            return CCC65Interface::getSourceFileFromSymbol(m_items.at(index.row()).symbol);
            break;
      }
   }
   else
   {
      switch ( index.column() )
      {
         case SymbolWatchCol_Symbol:
            return QVariant(CLICK_TO_ADD_OR_EDIT);
            break;
      }
   }
   return QVariant();
}

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QStringList symbols;
   QString selStr;
   int selIdx = 0;
   WatchedItem item;

   switch ( index.column() )
   {
      case SymbolWatchCol_Symbol:
         if ( m_editable )
         {
            if ( index.row() < m_items.count() )
            {
               item.symbol = value.toString();
               item.segment = resolveSymbol(value.toString());
               item.size = CCC65Interface::getSymbolSize(value.toString());
               m_items.replace(index.row(),item);
               emit layoutChanged();
               ok = true;
            }
            else
            {
               if ( (!value.toString().isEmpty()) &&
                    (value != CLICK_TO_ADD_OR_EDIT) )
               {
                  beginInsertRows(QModelIndex(),m_items.count()+1,m_items.count()+1);
                  item.symbol = value.toString();
                  item.segment = resolveSymbol(value.toString());
                  item.size = CCC65Interface::getSymbolSize(value.toString());
                  m_items.append(item);
                  endInsertRows();

                  ok = true;
               }
            }
         }
         break;
      default:
         ok = false;
         break;
      case SymbolWatchCol_Value:
         if ( index.row() < m_items.count() )
         {
            addr = CCC65Interface::getSymbolAddress(m_items.at(index.row()).symbol);
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
      case SymbolWatchCol_Symbol:
         return QString("Symbol");
         break;
      case SymbolWatchCol_Address:
         return QString("Address");
         break;
      case SymbolWatchCol_Size:
         return QString("Size");
         break;
      case SymbolWatchCol_Value:
         return QString("Value");
         break;
      case SymbolWatchCol_File:
         return QString("File");
         break;
      }
   }
   return QVariant();
}

int CSymbolWatchModel::rowCount(const QModelIndex&) const
{
   if ( m_editable )
   {
      return m_items.count()+1;
   }
   else
   {
      return m_items.count();
   }
}

int CSymbolWatchModel::columnCount(const QModelIndex&) const
{
   return SymbolWatchCol_MAX;
}

void CSymbolWatchModel::update()
{
   sort(m_currentSortColumn,m_currentSortOrder);
}

void CSymbolWatchModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < m_items.count() )
   {
      beginRemoveRows(parent,row,row);
      m_items.removeAt(row);
      endRemoveRows();
   }
}

bool CSymbolWatchModel::removeRows(int row, int count, const QModelIndex &parent)
{
   int idx;

   if ( row < m_items.count() )
   {
      beginRemoveRows(parent,row,row);
      for ( idx = 0; idx < count; idx++ )
      {
         m_items.removeAt(row);
      }
      endRemoveRows();
      return true;
   }
   return false;
}

void CSymbolWatchModel::insertRow(QString text, const QModelIndex& parent)
{
   WatchedItem item;

   beginInsertRows(parent,m_items.count(),m_items.count());
   item.symbol = text;
   item.segment = resolveSymbol(text);
   item.size = CCC65Interface::getSymbolSize(text);
   m_items.append(item);
   endInsertRows();
}

int CSymbolWatchModel::resolveSymbol(QString text)
{
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QString symbolFile;
   QStringList symbols;
   QString selStr;
   int selIdx = 0;
   bool ok;

   count = CCC65Interface::getSymbolMatchCount(text);
   if ( count > 1 )
   {
      for ( idx = 0; idx < count; idx++ )
      {
         addr = CCC65Interface::getSymbolAddress(text,idx);
         absAddr = CCC65Interface::getSymbolAbsoluteAddress(text,idx);
         symbolFile = CCC65Interface::getSourceFileFromSymbol(text);

         symbol = text;
         symbol += " @";
         nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
         symbol += modelStringBuffer;
         symbol += " in ";
         symbol += symbolFile;
         symbols.append(symbol);
      }
      selStr = QInputDialog::getItem(0,"Help!","Symbol has multiple possible matches, pick one:",symbols,0,false,&ok);
      if ( ok )
      {
         for ( selIdx = 0; selIdx < count; selIdx++ )
         {
            if ( symbols.at(selIdx) == selStr )
            {
               break;
            }
         }
      }
      else
      {
         selIdx = 0;
      }
   }
   return CCC65Interface::getSymbolSegment(text,selIdx);
}

void CSymbolWatchModel::sort(int column, Qt::SortOrder order)
{
   int idx1;
   int idx2;
   QString strData1;
   QString strData2;
   unsigned int uiData1;
   unsigned int uiData2;

   if ( (m_currentSortColumn != column) ||
        (m_currentSortOrder != order) ||
        (m_currentItemCount != m_items.count()) )
   {
      for ( idx1 = 0; idx1 < m_items.count(); idx1++ )
      {
         for ( idx2 = idx1; idx2 < m_items.count(); idx2++ )
         {
            switch ( column )
            {
            case SymbolWatchCol_Size:
            case SymbolWatchCol_Value:
               // These columns require integer sort.
               uiData1 = data(index(idx1,column),Qt::DisplayRole).toString().toInt(NULL,16);
               uiData2 = data(index(idx2,column),Qt::DisplayRole).toString().toInt(NULL,16);
               switch ( order )
               {
               case Qt::AscendingOrder:
                  if ( uiData1 > uiData2 )
                  {
                     m_items.swap(idx1,idx2);
                  }
                  break;
               case Qt::DescendingOrder:
                  if ( uiData2 > uiData1 )
                  {
                     m_items.swap(idx1,idx2);
                  }
                  break;
               }
               break;
            default:
               // Every other column can use string sorting.
               strData1 = data(index(idx1,column),Qt::DisplayRole).toString();
               strData2 = data(index(idx2,column),Qt::DisplayRole).toString();
               switch ( order )
               {
               case Qt::AscendingOrder:
                  if ( strData1 > strData2 )
                  {
                     m_items.swap(idx1,idx2);
                  }
                  break;
               case Qt::DescendingOrder:
                  if ( strData2 > strData1 )
                  {
                     m_items.swap(idx1,idx2);
                  }
                  break;
               }
               break;
            }
         }
      }
   }

   m_currentSortColumn = column;
   m_currentSortOrder = order;
   m_currentItemCount = m_items.count();

   emit layoutChanged();
}
