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

QModelIndex CDebuggerCodeProfilerModel::index(int row, int column, const QModelIndex &parent) const
{
   if ( (row >= 0) && (row < m_symbols.count()) )
   {
      CCodeDataLogger* pLogger;
      unsigned int addr;
      unsigned int absAddr;

      addr = CCC65Interface::getSymbolAddress(m_symbols.at(row));
      absAddr = CCC65Interface::getSymbolAbsoluteAddress(m_symbols.at(row));

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
         return m_symbols.at(index.row());
         break;
      case 1:
         // Get symbol's information based on its name and index.
         // CPTODO: fix segmenting stuff later!
         addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()),0/*idx*/);
         absAddr = CCC65Interface::getSymbolAbsoluteAddress(m_symbols.at(index.row()),0/*idx*/);
         if ( addr != 0xFFFFFFFF )
         {
            nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant("ERROR: Unresolved");
         }
         break;
      case 2:
         // Get symbol's information based on its name and index.
         // CPTODO: fix segmenting stuff later!
         addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()),0/*idx*/);
         return QVariant(pLogger->GetCount(addr&MASK_8KB));
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
      }
   }
   return QVariant();
}

int CDebuggerCodeProfilerModel::rowCount(const QModelIndex&) const
{
   return m_symbols.count();
}

int CDebuggerCodeProfilerModel::columnCount(const QModelIndex&) const
{
   return 3;
}

void CDebuggerCodeProfilerModel::update()
{
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile(""); // CPTODO: File doesn't matter (yet).
   CCodeDataLogger* pLogger;
   unsigned int addr;
   unsigned int absAddr;

   m_symbols.clear();
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
            m_symbols.append(symbol);
         }
      }
   }

   emit layoutChanged();
}
