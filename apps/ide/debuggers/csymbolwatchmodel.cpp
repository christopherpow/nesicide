#include <QInputDialog>

#include "csymbolwatchmodel.h"

#include "ccc65interface.h"
#include "nes_emulator_core.h"

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
   int symbolIdx;
   int size;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < m_items.count() )
   {
      // Get symbol's index in debug information from its segment.
      symbolIdx = CCC65Interface::instance()->getSymbolIndexFromSegment(m_items.at(index.row()).symbol,m_items.at(index.row()).segment);

      switch ( index.column() )
      {
         case SymbolWatchCol_Symbol:
            return m_items.at(index.row()).symbol;
            break;
         case SymbolWatchCol_Address:
            // Get symbol's information based on its name and index.
            addr = CCC65Interface::instance()->getSymbolAddress(m_items.at(index.row()).symbol,symbolIdx);
            absAddr = CCC65Interface::instance()->getSymbolPhysicalAddress(m_items.at(index.row()).symbol,symbolIdx);
            if ( addr != -1 )
            {
               nesGetPrintablePhysicalAddress(modelStringBuffer,addr,absAddr);
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case SymbolWatchCol_Size:
            size = CCC65Interface::instance()->getSymbolSize(m_items.at(index.row()).symbol,symbolIdx);
            if ( size )
            {
               return QVariant(size);
            }
            else
            {
               return QVariant("?");
            }
            break;
         case SymbolWatchCol_Value:
            addr = CCC65Interface::instance()->getSymbolAddress(m_items.at(index.row()).symbol,symbolIdx);
            if ( addr != -1 )
            {
               char* bufferPtr = modelStringBuffer;
               unsigned int symbolSize = CCC65Interface::instance()->getSymbolSize(m_items.at(index.row()).symbol,symbolIdx);

               // If symbol size <= 10 print values as an array, seperated by commas
               if ((symbolSize > 0) && (symbolSize <= 10))
               {
                  unsigned int i=0;
                  for( ; i < symbolSize; ++i)
                  {
                     bufferPtr += sprintf(bufferPtr, "%02X", nesGetMemory(addr + i));
                     if ( i < (symbolSize-1) )
                     {
                        bufferPtr += sprintf(bufferPtr, ",");
                     }
                  }

                  // If symbol is 2 bytes, print 16bit value in parentheses.
                  if (symbolSize == 2)
                  {
                     sprintf(bufferPtr, " ($%02X%02X)",  nesGetMemory(addr + 1),  nesGetMemory(addr + 0));
                  }
                  else if (symbolSize == 3) // Same for 24 bit values.
                  {
                     sprintf(bufferPtr, " ($%02X%02X%02X)",  nesGetMemory(addr + 2), nesGetMemory(addr + 1),  nesGetMemory(addr + 0));
                  }
               }
               else
               {
                  return QVariant("?");
               }
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case SymbolWatchCol_Segment:
            return CCC65Interface::instance()->getSymbolSegmentName(m_items.at(index.row()).symbol,symbolIdx);
            break;
         case SymbolWatchCol_File:
            return CCC65Interface::instance()->getSourceFileFromSymbol(m_items.at(index.row()).symbol);
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

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
   bool ok = false;
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QStringList symbols;
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
            addr = CCC65Interface::instance()->getSymbolAddress(m_items.at(index.row()).symbol,CCC65Interface::instance()->getSymbolIndexFromSegment(value.toString(),m_items.at(index.row()).segment));
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
      case SymbolWatchCol_Segment:
         return QString("Segment");
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
      beginRemoveRows(parent,row,row+count-1);
      for ( idx = row+count-1; idx >= row; idx-- )
      {
         m_items.removeAt(idx);
      }
      endRemoveRows();
      return true;
   }
   return false;
}

void CSymbolWatchModel::insertRow(QString text, int addr, const QModelIndex& parent)
{
   WatchedItem item;

   beginInsertRows(parent,m_items.count(),m_items.count());
   item.symbol = text;
   item.segment = resolveSymbol(text,addr);
   m_items.append(item);
   endInsertRows();
}

int CSymbolWatchModel::resolveSymbol(QString text,int addr)
{
   unsigned int checkAddr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QString symbolFile;
   QStringList symbols;
   QString selStr;
   int selIdx = -1;
   bool ok;

   count = CCC65Interface::instance()->getSymbolMatchCount(text);
   if ( count > 1 )
   {
      for ( idx = 0; idx < count; idx++ )
      {
         checkAddr = CCC65Interface::instance()->getSymbolAddress(text,idx);
         if ( checkAddr == addr )
         {
            selIdx = idx;
            break;
         }
         absAddr = CCC65Interface::instance()->getSymbolPhysicalAddress(text,idx);
         symbolFile = CCC65Interface::instance()->getSourceFileFromSymbol(text);

         symbol = text;
         symbol += " @";
         nesGetPrintablePhysicalAddress(modelStringBuffer,checkAddr,absAddr);
         symbol += modelStringBuffer;
         symbol += " in ";
         symbol += symbolFile;
         symbols.append(symbol);
      }
      if ( selIdx < 0 )
      {
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
   }
   return CCC65Interface::instance()->getSymbolSegment(text,selIdx);
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
