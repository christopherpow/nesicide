#include <QFileInfo>

#include "cdebuggercodeprofilermodel.h"

#include "ccc65interface.h"
#include "emulator_core.h"

#include "ccodedatalogger.h"

static char modelStringBuffer [ 2048 ];

CDebuggerCodeProfilerModel::CDebuggerCodeProfilerModel(QObject *parent) :
    QAbstractTableModel(parent)
{
   m_currentSortColumn = 0;
   m_currentSortOrder = Qt::DescendingOrder;
}

CDebuggerCodeProfilerModel::~CDebuggerCodeProfilerModel()
{
}

Qt::ItemFlags CDebuggerCodeProfilerModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( index.column() == 0 )
   {
      flags |= Qt::ItemIsDragEnabled;
   }
   return flags;
}

QModelIndex CDebuggerCodeProfilerModel::index(int row, int column, const QModelIndex &parent) const
{
   if ( (row >= 0) && (row < m_items.count()) )
   {
      CCodeDataLogger* pLogger;
      unsigned int addr;
      unsigned int absAddr;

      addr = CCC65Interface::getSymbolAddress(m_items.at(row).symbol);
      absAddr = CCC65Interface::getSymbolAbsoluteAddress(m_items.at(row).symbol);

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
      case 0:
         return m_items.at(index.row()).symbol;
         break;
      case 1:
         return m_items.at(index.row()).address;
         break;
      case 2:
         return QVariant(m_items.at(index.row()).count);
         break;
      case 3:
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
      case 0:
         return QString("Symbol");
         break;
      case 1:
         return QString("Address");
         break;
      case 2:
         return QString("# Calls");
         break;
      case 3:
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
   return 4;
}

void CDebuggerCodeProfilerModel::update()
{
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile(""); // CPTODO: File doesn't matter (yet).
   CCodeDataLogger* pLogger;
   unsigned int addr;
   unsigned int absAddr;
   ProfiledItem item;
   QFileInfo fileInfo;

   m_items.clear();
   foreach ( QString symbol, symbols )
   {
      addr = CCC65Interface::getSymbolAddress(symbol);
      absAddr = CCC65Interface::getSymbolAbsoluteAddress(symbol);

      if ( addr >= MEM_32KB )
      {
         pLogger = nesGetPhysicalPRGROMCodeDataLoggerDatabase(absAddr);
         if ( (pLogger->GetCount(addr&MASK_8KB)) &&
              (pLogger->GetType(addr&MASK_8KB) == eLogger_InstructionFetch) )
         {
            item.symbol = symbol;
            item.file = CCC65Interface::getSourceFileFromSymbol(symbol);
            fileInfo.setFile(item.file);
            if ( !fileInfo.exists() )
            {
               item.file += "[not found]";
            }
            nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
            item.address = modelStringBuffer;
            item.count = pLogger->GetCount(addr&MASK_8KB);
            m_items.append(item);
         }
      }
   }

   sort(m_currentSortColumn,m_currentSortOrder);
}

void CDebuggerCodeProfilerModel::sort(int column, Qt::SortOrder order)
{
   int idx1;
   int idx2;
   ProfiledItem item1;
   ProfiledItem item2;

   m_currentSortColumn = column;
   m_currentSortOrder = order;

   for ( idx1 = 0; idx1 < m_items.count(); idx1++ )
   {
      for ( idx2 = idx1; idx2 < m_items.count(); idx2++ )
      {
         item1 = m_items.at(idx1);
         item2 = m_items.at(idx2);
         switch ( column )
         {
         case 0:
            switch ( order )
            {
            case Qt::AscendingOrder:
               if ( item1.symbol > item2.symbol )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            case Qt::DescendingOrder:
               if ( item2.symbol > item1.symbol )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            }
            break;
         case 1:
            switch ( order )
            {
            case Qt::AscendingOrder:
               if ( item1.address > item2.address )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            case Qt::DescendingOrder:
               if ( item2.address > item1.address )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            }
            break;
         case 2:
            switch ( order )
            {
            case Qt::AscendingOrder:
               if ( item1.count > item2.count )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            case Qt::DescendingOrder:
               if ( item2.count > item1.count )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            }
            break;
         case 3:
            switch ( order )
            {
            case Qt::AscendingOrder:
               if ( item1.file > item2.file )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            case Qt::DescendingOrder:
               if ( item2.file > item1.file )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            }
            break;
         }
      }
   }

   emit layoutChanged();
}
