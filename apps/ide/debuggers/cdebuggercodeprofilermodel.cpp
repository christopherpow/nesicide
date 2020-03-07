#include <QFileInfo>

#include "cdebuggercodeprofilermodel.h"

#include "ccc65interface.h"
#include "nes_emulator_core.h"

#include "ccodedatalogger.h"

static char modelStringBuffer [ 2048 ];

CDebuggerCodeProfilerModel::CDebuggerCodeProfilerModel(QObject *parent) :
    QAbstractTableModel(parent)
{
   m_currentSortColumn = CodeProfilerCol_Symbol;
   m_currentSortOrder = Qt::DescendingOrder;
   m_currentItemCount = 0;
}

CDebuggerCodeProfilerModel::~CDebuggerCodeProfilerModel()
{
}

Qt::ItemFlags CDebuggerCodeProfilerModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( index.column() == CodeProfilerCol_Symbol )
   {
      flags |= Qt::ItemIsDragEnabled;
   }
   return flags;
}

QModelIndex CDebuggerCodeProfilerModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
   if ( (row >= 0) && (row < m_items.count()) )
   {
      CCodeDataLogger* pLogger;
      unsigned int addr;
      unsigned int absAddr;

      addr = CCC65Interface::instance()->getSymbolAddress(m_items.at(row).symbol);
      absAddr = CCC65Interface::instance()->getSymbolPhysicalAddress(m_items.at(row).symbol);

      if ( addr >= MEM_32KB )
      {
         pLogger = nesGetPhysicalPRGROMCodeDataLoggerDatabase(absAddr);
         return createIndex(row,column,pLogger);
      }
      return QModelIndex();
   }
   return QModelIndex();
}

QVariant CDebuggerCodeProfilerModel::data(const QModelIndex& index, int role) const
{
   CCodeDataLogger* pLogger = reinterpret_cast<CCodeDataLogger*>(index.internalPointer());
   unsigned int addr;
   unsigned int absAddr;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   switch ( index.column() )
   {
   case CodeProfilerCol_Symbol:
      return m_items.at(index.row()).symbol;
      break;
   case CodeProfilerCol_Address:
      return m_items.at(index.row()).address;
      break;
   case CodeProfilerCol_Size:
      return QVariant(m_items.at(index.row()).size);
      break;
   case CodeProfilerCol_Calls:
      return QVariant(m_items.at(index.row()).count);
      break;
   case CodeProfilerCol_File:
      return m_items.at(index.row()).file;
      break;
   }
   return QVariant();
}

QVariant CDebuggerCodeProfilerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case CodeProfilerCol_Symbol:
         return QString("Symbol");
         break;
      case CodeProfilerCol_Address:
         return QString("Address");
         break;
      case CodeProfilerCol_Size:
         return QString("Size");
         break;
      case CodeProfilerCol_Calls:
         return QString("# Calls");
         break;
      case CodeProfilerCol_File:
         return QString("File");
         break;
      }
   }
   return QVariant();
}

int CDebuggerCodeProfilerModel::rowCount(const QModelIndex&) const
{
   return m_items.count();
}

int CDebuggerCodeProfilerModel::columnCount(const QModelIndex&) const
{
   return CodeProfilerCol_MAX;
}

void CDebuggerCodeProfilerModel::update()
{
   QStringList symbols = CCC65Interface::instance()->getSymbolsForSourceFile(""); // CPTODO: File doesn't matter (yet).
   CCodeDataLogger* pLogger;
   unsigned int addr;
   unsigned int absAddr;
   ProfiledItem item;
   QFileInfo fileInfo;
   unsigned int mask;

   foreach ( QString symbol, symbols )
   {
      // CPTODO: Temporary hack to get around temporary labels.
      if ( !symbol.startsWith('@') )
      {
         addr = CCC65Interface::instance()->getSymbolAddress(symbol);
         absAddr = CCC65Interface::instance()->getSymbolPhysicalAddress(symbol);

         if ( absAddr != -1 )
         {
            pLogger = NULL;
            if ( addr >= 0x8000 )
            {
               pLogger = nesGetPhysicalPRGROMCodeDataLoggerDatabase(absAddr);
            }
            else if ( addr >= 0x6000 )
            {
               pLogger = nesGetPhysicalSRAMCodeDataLoggerDatabase(absAddr);
            }
            else if ( addr >= 0x5C00 )
            {
               pLogger = nesGetEXRAMCodeDataLoggerDatabase();
            }
            else if ( addr < 0x800 )
            {
               pLogger = nesGetCpuCodeDataLoggerDatabase();
            }
            if ( pLogger )
            {
               mask = pLogger->GetMask();
               if ( (pLogger->GetCount(addr&mask)) &&
                    (pLogger->GetType(addr&mask) == eLogger_InstructionFetch) )
               {
                  item.symbol = symbol;
                  item.size = CCC65Interface::instance()->getSymbolSize(symbol);
                  item.file = CCC65Interface::instance()->getSourceFileFromSymbol(symbol);
                  fileInfo.setFile(item.file);

                  nesGetPrintablePhysicalAddress(modelStringBuffer,addr,absAddr);
                  item.address = modelStringBuffer;
                  item.count = pLogger->GetCount(addr&mask);
                  if ( !m_items.contains(item) )
                  {
                     m_items.append(item);
                  }
                  else
                  {
                     m_items.replace(m_items.indexOf(item),item);
                  }
               }
            }
         }
      }
   }

   sort(m_currentSortColumn,m_currentSortOrder);
}

void CDebuggerCodeProfilerModel::sort(int column, Qt::SortOrder order)
{
   int idx1;
   int idx2;
   QString strData1;
   QString strData2;
   unsigned int uiData1;
   unsigned int uiData2;

   if ( (column != m_currentSortColumn) ||
        (order != m_currentSortOrder) ||
        (m_items.count() != m_currentItemCount) )
   {
      for ( idx1 = 0; idx1 < m_items.count(); idx1++ )
      {
         for ( idx2 = idx1; idx2 < m_items.count(); idx2++ )
         {
            switch ( column )
            {
            case CodeProfilerCol_Size:
            case CodeProfilerCol_Calls:
               // These columns require integer sorting.
               uiData1 = data(index(idx1,column),Qt::DisplayRole).toInt();
               uiData2 = data(index(idx2,column),Qt::DisplayRole).toInt();
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
