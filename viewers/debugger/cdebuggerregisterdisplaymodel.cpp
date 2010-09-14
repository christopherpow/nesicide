#include "cdebuggerregisterdisplaymodel.h"

#include "dbg_cnesmappers.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"
#include "dbg_cnesapu.h"
#include "dbg_cnesrom.h"

CDebuggerRegisterDisplayModel::CDebuggerRegisterDisplayModel(QObject*, eMemoryType display)
{
   m_display = display;
   m_register = 0;
   switch ( m_display )
   {
      case eMemory_CPUregs:
         m_tblRegisters = nesGetCpuRegisterDatabase();
      break;
      case eMemory_PPUregs:
         m_tblRegisters = nesGetPpuRegisterDatabase();
      break;
      case eMemory_IOregs:
         m_tblRegisters = nesGetApuRegisterDatabase();
      break;
      case eMemory_PPUoam:
         m_tblRegisters = nesGetPpuOamRegisterDatabase();
      break;
      case eMemory_cartMapper:
         m_tblRegisters = nesGetCartridgeRegisterDatabase();
      break;
      default:
         m_tblRegisters = NULL;
      break;
   }
}

CDebuggerRegisterDisplayModel::~CDebuggerRegisterDisplayModel()
{
}

QVariant CDebuggerRegisterDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];
   char tooltipBuffer [ 512 ];
   char* pValues = tooltipBuffer;
   int value;

   if (!index.isValid())
      return QVariant();

   // FIXME: 64-bit support
   int regData = (long)index.internalPointer();
   //int regData = (int)index.internalPointer();

   if ( m_tblRegisters )
   {
      CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( index.row() );

      if ( role == Qt::ToolTipRole )
      {
         if ( pBitfield->GetNumValues() )
         {
            pValues += sprintf ( pValues, "<pre>" );
            for ( value = 0; value < pBitfield->GetNumValues(); value++ )
            {
               if ( value == pBitfield->GetValueRaw(regData) )
               {
                  pValues += sprintf ( pValues, "<b>%s</b>", pBitfield->GetValueByIndex(value) );
               }
               else
               {
                  pValues += sprintf ( pValues, "%s", pBitfield->GetValueByIndex(value) );
               }
               if ( value < pBitfield->GetNumValues()-1 )
               {
                  pValues += sprintf ( pValues, "\n" );
               }
            }
            pValues += sprintf ( pValues, "</pre>" );
            return tooltipBuffer;
         }
      }

      if ( role == Qt::DisplayRole )
      {
         if ( pBitfield->GetNumValues() )
         {
            sprintf ( data, "%s", pBitfield->GetValue(regData) );
         }
         else
         {
            sprintf ( data, pBitfield->GetDisplayFormat(), pBitfield->GetValueRaw(regData) );
         }
         return QVariant(data);
      }
      else if ( role == Qt::EditRole )
      {
         sprintf ( data, pBitfield->GetDisplayFormat(), regData );
         return QVariant(data);
      }
   }
   return QVariant();
}

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex &) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CDebuggerRegisterDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
      return QVariant();
   char buffer [ 64 ] = { 0, };
   if ( orientation == Qt::Horizontal )
   {
      sprintf ( buffer, "Value" );
   }
   else
   {
      if ( m_tblRegisters )
      {
         CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( section );
         if ( pBitfield->GetWidth() == 1 )
         {
            sprintf ( buffer, "[%d] %s", pBitfield->GetLsb(), pBitfield->GetName() );
         }
         else
         {
            sprintf ( buffer, "[%d:%d] %s", pBitfield->GetMsb(), pBitfield->GetLsb(), pBitfield->GetName() );
         }
      }
   }

   return  QString(buffer);
}

bool CDebuggerRegisterDisplayModel::setData ( const QModelIndex & index, const QVariant & value, int )
{
   int data;
   bool ok = false;
   int addr;

   if ( m_tblRegisters )
   {
      addr = m_tblRegisters[m_register]->GetAddr();
      data = value.toString().toInt(&ok,16);

      if ( ok )
      {
         switch ( m_display )
         {
            case eMemory_CPUregs:
               switch ( m_register )
               {
                  case CPU_PC:
                     C6502DBG::__PC(data);
                  break;
                  case CPU_A:
                     C6502DBG::_A(data);
                  break;
                  case CPU_X:
                     C6502DBG::_X(data);
                  break;
                  case CPU_Y:
                     C6502DBG::_Y(data);
                  break;
                  case CPU_SP:
                     C6502DBG::_SP(data);
                  break;
                  case CPU_F:
                     C6502DBG::_F(data);
                  break;
               }
            break;
            case eMemory_PPUregs:
               CPPUDBG::_PPU(addr, data);
            break;
            case eMemory_IOregs:
               CAPUDBG::_APU(addr, data);
            break;
            case eMemory_PPUoam:
               CPPUDBG::_OAM(addr%OAM_SIZE,addr/OAM_SIZE, data);
            break;
            case eMemory_cartMapper:
               if ( addr < MEM_32KB )
               {
                  CROMDBG::LOWWRITE(addr,data);
               }
               else
               {
                  CROMDBG::HIGHWRITE(addr, data);
               }
            break;
            default:
            break;
         }
         emit dataChanged(index,index);
      }
   }
   return ok;
}

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex &) const
{
   int addr;

   if ( (row >= 0) && (column >= 0) )
   {
      if ( m_tblRegisters )
      {
         addr = m_tblRegisters[m_register]->GetAddr();

         switch ( m_display )
         {
            case eMemory_CPUregs:
               switch ( m_register )
               {
                  case CPU_PC:
                     return createIndex(row, column, (int)C6502DBG::__PC());
                  break;
                  case CPU_A:
                     return createIndex(row, column, (int)C6502DBG::_A());
                  break;
                  case CPU_X:
                     return createIndex(row, column, (int)C6502DBG::_X());
                  break;
                  case CPU_Y:
                     return createIndex(row, column, (int)C6502DBG::_Y());
                  break;
                  case CPU_SP:
                     return createIndex(row, column, (int)0x100|C6502DBG::_SP());
                  break;
                  case CPU_F:
                     return createIndex(row, column, (int)C6502DBG::_F());
                  break;
               }
            break;
            case eMemory_PPUregs:
               return createIndex(row, column, (int)CPPUDBG::_PPU(addr));
            break;
            case eMemory_IOregs:
               return createIndex(row, column, (int)CAPUDBG::_APU(addr));
            break;
            case eMemory_PPUoam:
               return createIndex(row, column, (int)CPPUDBG::_OAM(addr%OAM_SIZE,addr/OAM_SIZE));
            break;
            case eMemory_cartMapper:
               if ( m_tblRegisters )
               {
                  if ( addr < MEM_32KB )
                  {
                     return createIndex(row, column, (int)CROMDBG::LOWREAD(addr));
                  }
                  else
                  {
                     return createIndex(row, column, (int)CROMDBG::HIGHREAD(addr));
                  }
               }
               else
               {
                  return QModelIndex();
               }
            break;
            default:
               return QModelIndex();
            break;
         }
      }
   }
   return QModelIndex();
}

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex &) const
{
   if ( m_tblRegisters )
   {
      return m_tblRegisters[m_register]->GetNumBitfields();
   }
   // Nothing to display here...
   return 0;
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex &) const
{
   if ( m_tblRegisters )
   {
      return 1;
   }
   // Nothing to display here...
   return 0;
}

void CDebuggerRegisterDisplayModel::layoutChangedEvent()
{
   if ( m_display == eMemory_cartMapper )
   {
      // get the registers from the mapper just incase a cart has been loaded...
      m_tblRegisters = nesGetCartridgeRegisterDatabase();
   }
   this->layoutChanged();
}
