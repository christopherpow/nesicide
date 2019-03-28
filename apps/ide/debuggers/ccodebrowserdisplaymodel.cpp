#include "ccodebrowserdisplaymodel.h"

#include "environmentsettingsdialog.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"

#include "dbg_cc64.h"

#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

#include "cmarker.h"

#include "main.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include <QIcon>

static char modelStringBuffer [ 2048 ];

CCodeBrowserDisplayModel::CCodeBrowserDisplayModel(CBreakpointInfo* pBreakpoints,QObject*)
{
   m_pBreakpoints = pBreakpoints;
}

CCodeBrowserDisplayModel::~CCodeBrowserDisplayModel()
{
}

QVariant CCodeBrowserDisplayModel::data(const QModelIndex& index, int role) const
{
   // FIXME: 64-bit support
   int32_t addr = (long)index.internalPointer();
   int32_t absAddr;
   //uint32_t addr = (uint32_t)index.internalPointer();
   unsigned char opSize;
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int idx;
   uint32_t opcode;
   uint32_t operand1;
   uint32_t operand2;
   uint32_t pc;

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
      opcode = nesGetMemory(addr);
      operand1 = nesGetMemory(addr+1);
      operand2 = nesGetMemory(addr+2);
      pc = nesGetCPUProgramCounterOfLastSync();
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      absAddr = c64GetAbsoluteAddressFromAddress(addr);
      opcode = c64GetMemory(addr);
      operand1 = c64GetMemory(addr+1);
      operand2 = c64GetMemory(addr+2);
      pc = c64GetCPURegister(CPU_PC);
   }
   qDebug("%04X %08X: %02X",addr,absAddr,opcode);

   if ( role == Qt::ToolTipRole )
   {
      if ( EnvironmentSettingsDialog::showOpcodeTips() )
      {
         if ( addr != -1 )
         {
            if ( index.column() == CodeBrowserCol_Disassembly )
            {
               if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
               {
                  CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_INFO,addr,modelStringBuffer);
               }
               else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
               {
                  CC64DBG::CODEBROWSERTOOLTIP(TOOLTIP_INFO,addr,modelStringBuffer);
               }
               return QVariant(modelStringBuffer);
            }
            else if ( index.column() > CodeBrowserCol_Address )
            {
               opSize = OPCODESIZE ( opcode );

               if ( opSize > (index.column()-CodeBrowserCol_Opcode) )
               {
                  if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
                  {
                     CNESDBG::CODEBROWSERTOOLTIP(TOOLTIP_BYTES,addr+(index.column()-CodeBrowserCol_Opcode),modelStringBuffer);
                  }
                  else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
                  {
                     CC64DBG::CODEBROWSERTOOLTIP(TOOLTIP_BYTES,addr+(index.column()-CodeBrowserCol_Opcode),modelStringBuffer);
                  }
                  return QVariant(modelStringBuffer);
               }
            }
         }
      }
   }

   if ( (role == Qt::BackgroundRole) && (index.column() == CodeBrowserCol_Decoration) )
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

   if ((role == Qt::DecorationRole) && (index.column() == CodeBrowserCol_Decoration))
   {
      for ( idx = 0; idx < m_pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(idx);

         if ( (pBreakpoint->enabled) &&
               (pBreakpoint->type == eBreakOnCPUExecution) &&
               (pBreakpoint->item1 <= addr) &&
               ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
               (pBreakpoint->item2 >= addr) )
         {
            if ( addr == pc )
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
                   (pBreakpoint->item1 <= addr) &&
                   ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                   (pBreakpoint->item2 >= addr) )
         {
            if ( addr == pc )
            {
               return QIcon(":/resources/22_execution_break_disabled.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint_disabled.png");
            }
         }
      }

      if ( addr == pc )
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
      case CodeBrowserCol_Address:
         if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
         {
            nesGetPrintableAddress(modelStringBuffer,addr);
         }
         else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
         {
            c64GetPrintableAddress(modelStringBuffer,addr);
         }
         return QVariant(modelStringBuffer);
         break;
      case CodeBrowserCol_Decoration:
         return QVariant();
         break;
      case CodeBrowserCol_Opcode:
         sprintf ( modelStringBuffer, "%02X", opcode );
         return QVariant(modelStringBuffer);
         break;
      case CodeBrowserCol_Operand1:
         opSize = OPCODESIZE ( opcode );

         if ( 1 < opSize )
         {
            sprintf ( modelStringBuffer, "%02X", operand1 );
            return QVariant(modelStringBuffer);
         }

         break;
      case CodeBrowserCol_Operand2:
         opSize = OPCODESIZE ( opcode );

         if ( 2 < opSize )
         {
            sprintf ( modelStringBuffer, "%02X", operand2 );
            return QVariant(modelStringBuffer);
         }

         break;
      case CodeBrowserCol_Disassembly:
         if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
         {
            return nesGetDisassemblyAtAddress(addr);
         }
         else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
         {
            return c64GetDisassemblyAtAddress(addr);
         }
         break;
   }

   return QVariant();
}

Qt::ItemFlags CCodeBrowserDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled;
   if ( index.column() > 0 )
   {
      flags |= Qt::ItemIsSelectable;
   }
   return flags;
}

QVariant CCodeBrowserDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role == Qt::ToolTipRole)
   {
      switch ( section )
      {
         case CodeBrowserCol_Address:
            if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
            {
               return "Address in 6502 memory space and (Mapper Bank:Offset)";
            }
            else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
            {
               return "Address in 6502 memory space";
            }
            break;
         case CodeBrowserCol_Decoration:
            return "Information such as Breakpoint status, Marker status, etc.";
            break;
         case CodeBrowserCol_Opcode:
            return "Opcode";
            break;
         case CodeBrowserCol_Operand1:
            return "Low byte";
            break;
         case CodeBrowserCol_Operand2:
            return "High byte";
            break;
         case CodeBrowserCol_Disassembly:
            return "Disassembly";
            break;
      }
   }
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case CodeBrowserCol_Address:
            return "@";
            break;
         case CodeBrowserCol_Decoration:
            return "!";
            break;
         case CodeBrowserCol_Opcode:
            return "Op";
            break;
         case CodeBrowserCol_Operand1:
            return "Lo";
            break;
         case CodeBrowserCol_Operand2:
            return "Hi";
            break;
         case CodeBrowserCol_Disassembly:
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
      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         addr = nesGetAddressFromSLOC(row);
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         addr = c64GetAddressFromSLOC(row);
      }

      return createIndex(row, column, addr);
   }

   return QModelIndex();
}

int CCodeBrowserDisplayModel::rowCount(const QModelIndex&) const
{
   unsigned int rows = 0;

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      // Get the source-lines-of-code count from RAM/SRAM/EXRAM/PRG-ROM that is currently visible to the CPU...
      rows = nesGetSLOC(nesGetCPUProgramCounterOfLastSync());
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      // Get the source-lines-of-code count from RAM/SRAM/EXRAM/PRG-ROM that is currently visible to the CPU...
      rows = c64GetSLOC(c64GetCPURegister(CPU_PC));
   }

   return rows;
}

int CCodeBrowserDisplayModel::columnCount(const QModelIndex& parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }

   return CodeBrowserCol_Max;
}

void CCodeBrowserDisplayModel::update()
{
   // Update display...
   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      nesDisassemble();
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      c64Disassemble();
   }

   emit layoutChanged();
}
