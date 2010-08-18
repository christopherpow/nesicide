#include "ccodebrowserdisplaymodel.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"
#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

#include "cmarker.h"

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
    uint32_t addr = (long)index.internalPointer();
    uint32_t absAddr;
   //uint32_t addr = (uint32_t)index.internalPointer();
   char buffer [ 3 ];
   unsigned char opSize;
   CBreakpointInfo* pBreakpoints = CNESDBG::BREAKPOINTS();
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int idx;
   char tooltipBuffer [ 2048 ];

   absAddr = nesGetAbsoluteAddressFromAddress(addr);

   if ( role == Qt::ToolTipRole )
   {
      if ( addr != 0xFFFFFFFF )
      {
         if ( index.column() == 4 )
         {
            CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_INFO,addr,tooltipBuffer);
            return tooltipBuffer;
         }
         else if ( index.column() > 0 )
         {
            opSize = OPCODESIZE ( nesGetMemory(addr) );
            if ( opSize > (index.column()-1) )
            {
               CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_BYTES,addr+(index.column()-1),tooltipBuffer);
               return tooltipBuffer;
            }
         }
      }
   }

   if ( (role == Qt::BackgroundRole) && (index.column() == 0) )
   {
      for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
      {
         pMarker = markers->GetMarker(idx);
         if ( (pMarker->state == eMarkerSet_Started) ||
              (pMarker->state == eMarkerSet_Complete) )
         {
            if ( (absAddr >= pMarker->startAbsAddr) &&
                 (absAddr <= pMarker->endAbsAddr) )
            {
               return QColor(pMarker->red,pMarker->green,pMarker->blue);
            }
         }
      }
      return QVariant();
   }

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
         if ( (pBreakpoint->enabled) &&
              (pBreakpoint->type == eBreakOnCPUExecution) &&
              ((uint32_t)pBreakpoint->item1 <= addr) &&
              ((uint32_t)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502DBG::__PC() )
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
                   ((uint32_t)pBreakpoint->item1 <= addr) &&
                   ((uint32_t)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502DBG::__PC() )
            {
               return QIcon(":/resources/22_execution_break_disabled.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint_disabled.png");
            }
         }
      }
      if ( addr == C6502DBG::__PC() )
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
         return QVariant();
      break;
      case 1:
         sprintf ( buffer, "%02X", nesGetMemory(addr) );
         return buffer;
      break;
      case 2:
         opSize = OPCODESIZE ( nesGetMemory(addr) );
         if ( 1 < opSize )
         {
            sprintf ( buffer, "%02X", nesGetMemory(addr+1) );
            return buffer;
         }
      break;
      case 3:
         opSize = OPCODESIZE ( nesGetMemory(addr) );
         if ( 2 < opSize )
         {
            sprintf ( buffer, "%02X", nesGetMemory(addr+2) );
            return buffer;
         }
      break;
      case 4:
         return nesGetDisassemblyAtAddress(addr);
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
      addr = nesGetAddressFromSLOC(section);
      sprintf ( buffer, "$%04X", addr );
      return buffer;
   }

   return  QVariant();
}

QModelIndex CCodeBrowserDisplayModel::index(int row, int column, const QModelIndex &) const
{
   int addr;

   if ( (row >= 0) && (column >= 0) )
   {
      addr = nesGetAddressFromSLOC(row);

      return createIndex(row, column, addr);
   }

   return QModelIndex();
}

int CCodeBrowserDisplayModel::rowCount(const QModelIndex &) const
{
   unsigned int rows;

   // Get the source-lines-of-code count from RAM/SRAM/EXRAM/PRG-ROM that is currently visible to the CPU...
   rows = nesGetSLOC(C6502DBG::__PC());

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
