#include "ccodebrowserdisplaymodel.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"
#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

#include "cmarker.h"

#include <QIcon>

static char modelStringBuffer [ 2048 ];

enum
{
   Column_Address = 0,
   Column_Decoration,
   Column_Opcode,
   Column_Operand1,
   Column_Operand2,
   Column_Disassembly,
   Column_Max
};
   
CCodeBrowserDisplayModel::CCodeBrowserDisplayModel(QObject*)
{
}

CCodeBrowserDisplayModel::~CCodeBrowserDisplayModel()
{
}

QVariant CCodeBrowserDisplayModel::data(const QModelIndex& index, int role) const
{
   // FIXME: 64-bit support
   uint32_t addr = (long)index.internalPointer();
   uint32_t absAddr;
   //uint32_t addr = (uint32_t)index.internalPointer();
   unsigned char opSize;
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int idx;

   absAddr = nesGetAbsoluteAddressFromAddress(addr);

   if ( role == Qt::ToolTipRole )
   {
      if ( addr != 0xFFFFFFFF )
      {
         if ( index.column() == Column_Disassembly )
         {
            CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_INFO,addr,modelStringBuffer);
            return QVariant(modelStringBuffer);
         }
         else if ( index.column() > Column_Decoration )
         {
            opSize = OPCODESIZE ( nesGetMemory(addr) );

            if ( opSize > (index.column()-Column_Opcode) )
            {
               CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_BYTES,addr+(index.column()-Column_Opcode),modelStringBuffer);
               return QVariant(modelStringBuffer);
            }
         }
      }
   }

   if ( (role == Qt::BackgroundRole) && (index.column() == Column_Decoration) )
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

   if ((role == Qt::DecorationRole) && (index.column() == Column_Decoration))
   {
      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

         if ( (pBreakpoint->enabled) &&
               (pBreakpoint->type == eBreakOnCPUExecution) &&
               ((uint32_t)pBreakpoint->item1 <= addr) &&
               ((uint32_t)pBreakpoint->item2 >= addr) )
         {
            if ( addr == nesGetCPUProgramCounterOfLastSync() )
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
            if ( addr == nesGetCPUProgramCounterOfLastSync() )
            {
               return QIcon(":/resources/22_execution_break_disabled.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint_disabled.png");
            }
         }
      }

      if ( addr == nesGetCPUProgramCounterOfLastSync() )
      {
         return QIcon(":/resources/22_execution_pointer.png");
      }
   }

   if (!index.isValid())
   {
      return QVariant();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   switch ( index.column() )
   {
      case Column_Address:
         sprintf ( modelStringBuffer, "$%04X", addr );
         return QVariant(modelStringBuffer);
         break;
      case Column_Decoration:
         return QVariant();
         break;
      case Column_Opcode:
         sprintf ( modelStringBuffer, "%02X", nesGetMemory(addr) );
         return QVariant(modelStringBuffer);
         break;
      case Column_Operand1:
         opSize = OPCODESIZE ( nesGetMemory(addr) );

         if ( 1 < opSize )
         {
            sprintf ( modelStringBuffer, "%02X", nesGetMemory(addr+1) );
            return QVariant(modelStringBuffer);
         }

         break;
      case Column_Operand2:
         opSize = OPCODESIZE ( nesGetMemory(addr) );

         if ( 2 < opSize )
         {
            sprintf ( modelStringBuffer, "%02X", nesGetMemory(addr+2) );
            return QVariant(modelStringBuffer);
         }

         break;
      case Column_Disassembly:
         return nesGetDisassemblyAtAddress(addr);
         break;
   }

   return QVariant();
}

Qt::ItemFlags CCodeBrowserDisplayModel::flags(const QModelIndex&) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return flags;
}

QVariant CCodeBrowserDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case Column_Address:
            return "@";
            break;
         case Column_Decoration:
            return "!";
            break;
         case Column_Opcode:
            return "Op";
            break;
         case Column_Operand1:
            return "Lo";
            break;
         case Column_Operand2:
            return "Hi";
            break;
         case Column_Disassembly:
            return "Disassembly";
            break;
      }
   }

   return  QVariant();
}

QModelIndex CCodeBrowserDisplayModel::index(int row, int column, const QModelIndex&) const
{
   int addr;

   if ( (row >= 0) && (column >= 0) )
   {
      addr = nesGetAddressFromSLOC(row);

      return createIndex(row, column, addr);
   }

   return QModelIndex();
}

int CCodeBrowserDisplayModel::rowCount(const QModelIndex&) const
{
   unsigned int rows;

   // Get the source-lines-of-code count from RAM/SRAM/EXRAM/PRG-ROM that is currently visible to the CPU...
   rows = nesGetSLOC(nesGetCPUProgramCounterOfLastSync());

   return rows;
}

int CCodeBrowserDisplayModel::columnCount(const QModelIndex& parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }

   return Column_Max;
}

void CCodeBrowserDisplayModel::update()
{
   // Update display...
   nesDisassemble();

   emit layoutChanged();
}
