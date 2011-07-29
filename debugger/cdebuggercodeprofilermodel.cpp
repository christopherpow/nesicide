#include "cdebuggercodeprofilermodel.h"

#include "ccc65interface.h"
#include "emulator_core.h"

#include "ccodedatalogger.h"

static char modelStringBuffer [ 2048 ];

CDebuggerCodeProfilerModel::CDebuggerCodeProfilerModel(QObject *parent) :
    QAbstractTableModel(parent)
{
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

QVariant CDebuggerCodeProfilerModel::data(const QModelIndex& index, int role) const
{
   unsigned int addr;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   switch ( index.column() )
   {
      case 0:
         return m_symbols.at(index.row());
         break;
      case 1:
         return "bwahaha";
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
         return QString("# Calls");
         break;
      }
   }
   return QVariant();
}

int CDebuggerCodeProfilerModel::rowCount(const QModelIndex&) const
{
   int rows = 0;
   unsigned int addr;
   unsigned int absAddr;

   foreach ( QString symbol, m_symbols )
   {
      addr = CCC65Interface::getSymbolAddress(symbol);
//      absAddr = CCC65Interface::...figure out how to get the absolute address of a symbol

   }

   // Loops:
   // For symbol in symbols list
   //   Get symbol absolute address
   //   Get log information from logger for address
   //   If logged count > 0 and it was executed not read/written, rows += 1
   return m_symbols.count();
}

int CDebuggerCodeProfilerModel::columnCount(const QModelIndex&) const
{
   return 2;
}

void CDebuggerCodeProfilerModel::update()
{
   m_symbols = CCC65Interface::getSymbolsForSourceFile(""); // CPTODO: File doesn't matter (yet).

   emit layoutChanged();
}
