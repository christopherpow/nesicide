#include "cdebuggerregisterdisplaymodel.h"

#include "cnesmappers.h"
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

CDebuggerRegisterDisplayModel::CDebuggerRegisterDisplayModel(QObject* parent, eMemoryType display)
{
   m_display = display;
   m_register = 0;
   switch ( m_display )
   {
      case eMemory_CPUregs:
         m_tblRegisters = C6502::REGISTERS();
      break;
      case eMemory_PPUregs:
         m_tblRegisters = CPPU::REGISTERS();
      break;
      case eMemory_IOregs:
         m_tblRegisters = CAPU::REGISTERS();
      break;
      case eMemory_PPUoam:
         m_tblRegisters = tblOAMRegisters;
      break;
      case eMemory_cartMapper:
         m_tblRegisters = CROM::REGISTERS();
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

   if (!index.isValid())
      return QVariant();

   int regData = (int)index.internalPointer();

   if ( m_tblRegisters )
   {
      CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( index.row() );
      if ( role == Qt::DisplayRole )
      {
         if ( pBitfield->GetNumValues() )
         {
            sprintf ( data, "%X: %s", pBitfield->GetValueRaw(regData), pBitfield->GetValue(regData) );
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

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex &index) const
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
         sprintf ( buffer, pBitfield->GetName() );
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
                  case 0:
                     C6502::__PC(data);
                  break;
                  case 1:
                     C6502::_A(data);
                  break;
                  case 2:
                     C6502::_X(data);
                  break;
                  case 3:
                     C6502::_Y(data);
                  break;
                  case 4:
                     C6502::_SP(data);
                  break;
                  case 5:
                     C6502::_F(data);
                  break;
               }
            break;
            case eMemory_PPUregs:
               CPPU::_PPU(addr, data);
            break;
            case eMemory_IOregs:
               CAPU::_APU(addr, data);
            break;
            case eMemory_PPUoam:
               CPPU::OAM(addr%NUM_OAM_REGS,addr/NUM_OAM_REGS, data);
            break;
            case eMemory_cartMapper:
               if ( addr < MEM_32KB )
               {
                  mapperfunc[CROM::MAPPER()].lowwrite(addr, data);
               }
               else
               {
                  mapperfunc[CROM::MAPPER()].highwrite(addr, data);
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

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   int addr;

   if ( m_tblRegisters )
   {
      addr = m_tblRegisters[m_register]->GetAddr();

      switch ( m_display )
      {
         case eMemory_CPUregs:
            switch ( m_register )
            {
               case 0:
                  return createIndex(row, column, (int)C6502::__PC());
               break;
               case 1:
                  return createIndex(row, column, (int)C6502::_A());
               break;
               case 2:
                  return createIndex(row, column, (int)C6502::_X());
               break;
               case 3:
                  return createIndex(row, column, (int)C6502::_Y());
               break;
               case 4:
                  return createIndex(row, column, (int)0x100|C6502::_SP());
               break;
               case 5:
                  return createIndex(row, column, (int)C6502::_F());
               break;
            }
         break;
         case eMemory_PPUregs:
            return createIndex(row, column, (int)CPPU::_PPU(addr));
         break;
         case eMemory_IOregs:
            return createIndex(row, column, (int)CAPU::_APU(addr));
         break;
         case eMemory_PPUoam:
            return createIndex(row, column, (int)CPPU::OAM(addr%NUM_OAM_REGS,addr/NUM_OAM_REGS));
         break;
         case eMemory_cartMapper:
            if ( m_tblRegisters )
            {
               if ( addr < MEM_32KB )
               {
                  return createIndex(row, column, (int)mapperfunc[CROM::MAPPER()].lowread(addr));
               }
               else
               {
                  return createIndex(row, column, (int)mapperfunc[CROM::MAPPER()].highread(addr));
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
   return QModelIndex();
}

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex &parent) const
{
   if ( m_tblRegisters )
   {
      return m_tblRegisters[m_register]->GetNumBitfields();
   }
   return 0;
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex &parent) const
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
      m_tblRegisters = CROM::REGISTERS();
   }
   this->layoutChanged();
}
