#include "ccodebrowserdisplaymodel.h"

#include "cnes.h"
#include "cnesrom.h"
#include "cnes6502.h"
#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

#include <QIcon>

CCodeBrowserDisplayModel::CCodeBrowserDisplayModel(QObject*)
{
}

CCodeBrowserDisplayModel::~CCodeBrowserDisplayModel()
{
}

QVariant CCodeBrowserDisplayModel::data(const QModelIndex &index, int role) const
{
   // FIXME: 64-bit support
    UINT addr = (long)index.internalPointer();
   //UINT addr = (UINT)index.internalPointer();
   char buffer [ 3 ];
   unsigned char opSize;
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
         if ( (pBreakpoint->enabled) &&
              (pBreakpoint->type == eBreakOnCPUExecution) &&
              ((UINT)pBreakpoint->item1 <= addr) &&
              ((UINT)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502::__PC() )
            {
               return QIcon(":/resources/22_execution_break.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint.png");
            }
         }
         else if ( (!pBreakpoint->enabled) &&
                   (pBreakpoint->type == eBreakOnCPUExecution) &&
                   ((UINT)pBreakpoint->item1 <= addr) &&
                   ((UINT)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502::__PC() )
            {
               return QIcon(":/resources/22_execution_break_disabled.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint_disabled.png");
            }
         }
      }
      if ( addr == C6502::__PC() )
      {
         return QIcon(":/resources/22_execution_pointer.png");
      }
   }

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         return "";
      break;
      case 1:
         if ( addr < MEM_2KB )
         {
            sprintf ( buffer, "%02X", C6502::_MEM(addr) );
         }
         else
         {
            sprintf ( buffer, "%02X", CROM::PRGROM(addr) );
         }
         return buffer;
      break;
      case 2:
         opSize = C6502::OpcodeSize ( CROM::PRGROM(addr) );
         if ( 1 < opSize )
         {
            if ( addr < MEM_2KB )
            {
               sprintf ( buffer, "%02X", C6502::_MEM(addr+1) );
            }
            else
            {
               sprintf ( buffer, "%02X", CROM::PRGROM(addr+1) );
            }
            return buffer;
         }
      break;
      case 3:
         opSize = C6502::OpcodeSize ( CROM::PRGROM(addr) );
         if ( 2 < opSize )
         {
            if ( addr < MEM_2KB )
            {
               sprintf ( buffer, "%02X", C6502::_MEM(addr+2) );
            }
            else
            {
               sprintf ( buffer, "%02X", CROM::PRGROM(addr+2) );
            }
            return buffer;
         }
      break;
      case 4:
         if ( addr < MEM_2KB )
         {
            return C6502::DISASSEMBLY(addr);
         }
         else
         {
            return CROM::DISASSEMBLY(addr);
         }
      break;
   }

   return QVariant();
}

Qt::ItemFlags CCodeBrowserDisplayModel::flags(const QModelIndex &) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

QVariant CCodeBrowserDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   char buffer [ 6 ];
   int addr;

   if (role != Qt::DisplayRole)
      return QVariant();

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case 0:
            return "!";
         break;
         case 1:
            return "Op";
         break;
         case 2:
            return "Lo";
         break;
         case 3:
            return "Hi";
         break;
         case 4:
            return "Disassembly";
         break;
      }
   }
   else
   {
      if ( C6502::__PC() < MEM_2KB )
      {
         addr = C6502::SLOC2ADDR(section);
      }
      else
      {
         addr = CROM::SLOC2ADDR(section);
      }
      sprintf ( buffer, "$%04X", addr );
      return buffer;
   }

   return  QVariant();
}

QModelIndex CCodeBrowserDisplayModel::index(int row, int column, const QModelIndex &) const
{
   int addr;

   if ( C6502::__PC() < MEM_2KB )
   {
      addr = C6502::SLOC2ADDR(row);
   }
   else
   {
      addr = CROM::SLOC2ADDR(row);
   }

   return createIndex(row, column, addr);
}

int CCodeBrowserDisplayModel::rowCount(const QModelIndex &) const
{
   int rows;

   if ( C6502::__PC() < MEM_2KB )
   {
      // Get the source-lines-of-code count from RAM that is currently visible to the CPU...
      rows = C6502::SLOC();
   }
   else
   {
      // Get the source-lines-of-code count from each disassembled ROM
      // bank that is currently visible to the CPU...
      rows = CROM::SLOC(0x8000)+CROM::SLOC(0xA000)+CROM::SLOC(0xC000)+CROM::SLOC(0xE000);
   }

   return rows;
}

int CCodeBrowserDisplayModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   return 5;
}

void CCodeBrowserDisplayModel::layoutChangedEvent()
{
    this->layoutChanged();
}
