#include "cdebuggerregisterdisplaymodel.h"

#include "emulator_core.h"

static char modelStringBuffer [ 2048 ];

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

QVariant CDebuggerRegisterDisplayModel::data(const QModelIndex& index, int role) const
{
   char* pValues = modelStringBuffer;
   int value;

   if (!index.isValid())
   {
      return QVariant();
   }

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
            return QVariant(modelStringBuffer);
         }
      }

      if ( role == Qt::DisplayRole )
      {
         if ( pBitfield->GetNumValues() )
         {
            sprintf ( modelStringBuffer, "%s", pBitfield->GetValue(regData) );
         }
         else
         {
            sprintf ( modelStringBuffer, pBitfield->GetDisplayFormat(), pBitfield->GetValueRaw(regData) );
         }

         return QVariant(modelStringBuffer);
      }
      else if ( role == Qt::EditRole )
      {
         sprintf ( modelStringBuffer, pBitfield->GetDisplayFormat(), regData );
         return QVariant(modelStringBuffer);
      }
   }

   return QVariant();
}

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex&) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CDebuggerRegisterDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      sprintf ( modelStringBuffer, "Value" );
   }
   else
   {
      if ( m_tblRegisters )
      {
         CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( section );

         if ( pBitfield->GetWidth() == 1 )
         {
            sprintf ( modelStringBuffer, "[%d] %s", pBitfield->GetLsb(), pBitfield->GetName() );
         }
         else
         {
            sprintf ( modelStringBuffer, "[%d:%d] %s", pBitfield->GetMsb(), pBitfield->GetLsb(), pBitfield->GetName() );
         }
      }
   }

   return QVariant(modelStringBuffer);
}

bool CDebuggerRegisterDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
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
                     nesSetCPUProgramCounter(data);
                     break;
                  case CPU_A:
                     nesSetCPUAccumulator(data);
                     break;
                  case CPU_X:
                     nesSetCPUIndexX(data);
                     break;
                  case CPU_Y:
                     nesSetCPUIndexY(data);
                     break;
                  case CPU_SP:
                     nesSetCPUStackPointer(data);
                     break;
                  case CPU_F:
                     nesSetCPUFlags(data);
                     break;
               }

               break;
            case eMemory_PPUregs:
               nesSetPPURegister(addr, data);
               break;
            case eMemory_IOregs:
               nesSetAPURegister(addr, data);
               break;
            case eMemory_PPUoam:
               nesSetPPUOAM(addr%OAM_SIZE,addr/OAM_SIZE, data);
               break;
            case eMemory_cartMapper:
               if ( addr < MEM_32KB )
               {
                  nesMapperLowWrite(addr,data);
               }
               else
               {
                  nesMapperHighWrite(addr, data);
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

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex&) const
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
                     return createIndex(row, column, (int)nesGetCPUProgramCounter());
                     break;
                  case CPU_A:
                     return createIndex(row, column, (int)nesGetCPUAccumulator());
                     break;
                  case CPU_X:
                     return createIndex(row, column, (int)nesGetCPUIndexX());
                     break;
                  case CPU_Y:
                     return createIndex(row, column, (int)nesGetCPUIndexY());
                     break;
                  case CPU_SP:
                     return createIndex(row, column, (int)0x100|nesGetCPUStackPointer());
                     break;
                  case CPU_F:
                     return createIndex(row, column, (int)nesGetCPUFlags());
                     break;
               }

               break;
            case eMemory_PPUregs:
               return createIndex(row, column, (int)nesGetPPURegister(addr));
               break;
            case eMemory_IOregs:
               return createIndex(row, column, (int)nesGetAPURegister(addr));
               break;
            case eMemory_PPUoam:
               return createIndex(row, column, (int)nesGetPPUOAM(addr%OAM_SIZE,addr/OAM_SIZE));
               break;
            case eMemory_cartMapper:
               if ( m_tblRegisters )
               {
                  if ( addr < MEM_32KB )
                  {
                     return createIndex(row, column, (int)nesMapperLowRead(addr));
                  }
                  else
                  {
                     return createIndex(row, column, (int)nesMapperHighRead(addr));
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

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex&) const
{
   if ( m_tblRegisters )
   {
      return m_tblRegisters[m_register]->GetNumBitfields();
   }

   // Nothing to display here...
   return 0;
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex&) const
{
   if ( m_tblRegisters )
   {
      return 1;
   }

   // Nothing to display here...
   return 0;
}

void CDebuggerRegisterDisplayModel::update()
{
   if ( m_display == eMemory_cartMapper )
   {
      // get the registers from the mapper just incase a cart has been loaded...
      m_tblRegisters = nesGetCartridgeRegisterDatabase();
   }

   emit layoutChanged();
}
