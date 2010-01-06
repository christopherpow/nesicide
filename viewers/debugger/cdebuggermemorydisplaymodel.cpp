#include "cdebuggermemorydisplaymodel.h"

#include "cnesmappers.h"

CDebuggerMemoryDisplayModel::CDebuggerMemoryDisplayModel(QObject* parent, eMemoryType display)
{
   m_display = display;
   m_tblRegisters = NULL;
   switch ( m_display )
   {
      case eMemory_CPU:
         m_offset = 0;
      break;
      case eMemory_CPUregs:
         m_offset = 0;
      break;
      case eMemory_PPUregs:
         m_offset = 0x2000;
      break;
      case eMemory_IOregs:
         m_offset = 0x4000;
      break;
      case eMemory_cartSRAM:
         m_offset = 0x6000;
      break;
      case eMemory_cartEXRAM:
         m_offset = 0x5C00;
      break;
      case eMemory_cartCHRMEM:
         m_offset = 0;
      break;
      case eMemory_cartMapper:         
         m_offset = 0;
      break;
      case eMemory_PPU:
         m_offset = 0x2000;
      break;
      case eMemory_PPUpalette:
         m_offset = 0x3F00;
      break;
      case eMemory_PPUoam:
         m_offset = 0;
      break;
   }
}

CDebuggerMemoryDisplayModel::~CDebuggerMemoryDisplayModel()
{
}

QVariant CDebuggerMemoryDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   sprintf ( data, "%02X", (int)index.internalPointer() );
   return data;
}

Qt::ItemFlags CDebuggerMemoryDisplayModel::flags(const QModelIndex &index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( (m_display != eMemory_cartCHRMEM) ||
        (!(CROM::IsWriteProtected())) )
   {
      flags |= Qt::ItemIsEditable;
   }
   return flags;
}

QVariant CDebuggerMemoryDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
      return QVariant();
   char buffer [ 64 ] = { 0, };
   if ( orientation == Qt::Horizontal )
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:
            switch ( section )
            {
               case 0:
                  sprintf ( buffer, "PC" );
               break;
               case 1:
                  sprintf ( buffer, "A" );
               break;
               case 2:
                  sprintf ( buffer, "X" );
               break;
               case 3:
                  sprintf ( buffer, "Y" );
               break;
               case 4:
                  sprintf ( buffer, "SP" );
               break;
               case 5:
                  sprintf ( buffer, "Flags" );
               break;
            }
         break;
         default:
            sprintf ( buffer, "x%1X", section );
         break;
      }
   }
   else
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:
            sprintf ( buffer, "CPU" );
         break;
         case eMemory_cartMapper:
            if ( m_tblRegisters )
            {
               sprintf ( buffer, m_tblRegisters[section]->GetName());
            }
         break;
         case eMemory_CPU:
         case eMemory_cartSRAM:
         case eMemory_cartEXRAM:
         case eMemory_cartCHRMEM:
         case eMemory_PPU:
         case eMemory_PPUregs:
            sprintf ( buffer, "$%04X:", m_offset+(section<<4) );
         break;
         case eMemory_PPUoam:
            sprintf ( buffer, "%d", section );
         break;
         case eMemory_PPUpalette:
         case eMemory_IOregs:
            sprintf ( buffer, "$%04X:", m_offset+(section<<2) );
         break;
      }
   }

   return  QString(buffer);
}

bool CDebuggerMemoryDisplayModel::setData ( const QModelIndex & index, const QVariant & value, int )
{
   unsigned int data;
   bool ok = false;

   data = value.toString().toInt(&ok,16);
   if ( ok )
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:
            switch ( index.column() )
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
         case eMemory_CPU:
            C6502::_MEM(m_offset+(index.row()<<4)+index.column(), data);
         break;
         case eMemory_PPUregs:
            CPPU::_PPU(m_offset+index.column(), data);
         break;
         case eMemory_IOregs:
            CAPU::_APU(m_offset+(index.row()<<2)+index.column(), data);
         break;
         case eMemory_cartSRAM:
            CROM::SRAM(m_offset+(index.row()<<4)+index.column(), data);
         break;
         case eMemory_cartEXRAM:
            CROM::EXRAM(m_offset+(index.row()<<4)+index.column(), data);
         break;
         case eMemory_cartCHRMEM:
            CROM::CHRMEM(m_offset+(index.row()<<4)+index.column(), data);
         break;
         case eMemory_cartMapper:
            mapperfunc[CROM::MAPPER()].highwrite(m_tblRegisters[index.row()]->GetAddr(), data);
         break;
         case eMemory_PPU:
            CPPU::_MEM(m_offset+(index.row()<<4)+index.column(), data);
         break;
         case eMemory_PPUpalette:
            CPPU::_MEM(m_offset+(index.row()<<2)+index.column(), data);
         break;
         case eMemory_PPUoam:
            CPPU::OAM(index.column(), index.row(), data);
         break;
      }
      emit dataChanged(index,index);
   }
   return ok;
}

QModelIndex CDebuggerMemoryDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   switch ( m_display )
   {
      case eMemory_CPUregs:
         switch ( column )
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
      case eMemory_CPU:
         return createIndex(row, column, (int)C6502::_MEM(m_offset+(row<<4)+column));
      break;
      case eMemory_PPUregs:
         return createIndex(row, column, (int)CPPU::_PPU(m_offset+column));
      break;
      case eMemory_IOregs:
         return createIndex(row, column, (int)CAPU::_APU(m_offset+(row<<2)+column));
      break;
      case eMemory_cartSRAM:
         return createIndex(row, column, (int)CROM::SRAM(m_offset+(row<<4)+column));
      break;
      case eMemory_cartEXRAM:
         return createIndex(row, column, (int)CROM::EXRAM(m_offset+(row<<4)+column));
      break;
      case eMemory_cartCHRMEM:
         return createIndex(row, column, (int)CROM::CHRMEM(m_offset+(row<<4)+column));
      break;
      case eMemory_cartMapper:
         if ( m_tblRegisters )
         {
            return createIndex(row, column, (int)mapperfunc[CROM::MAPPER()].highread(m_tblRegisters[row]->GetAddr()));
         }
         else
         {
            return QModelIndex();
         }
      break;
      case eMemory_PPU:
         return createIndex(row, column, (int)CPPU::_MEM(m_offset+(row<<4)+column));
      break;
      case eMemory_PPUpalette:
         return createIndex(row, column, (int)CPPU::_MEM(m_offset+(row<<2)+column));
      break;
      case eMemory_PPUoam:
         return createIndex(row, column, (int)CPPU::OAM(column,row));
      break;
   }
   return QModelIndex();
}

int CDebuggerMemoryDisplayModel::rowCount(const QModelIndex &parent) const
{
   switch ( m_display )
   {
      case eMemory_CPUregs:
         return 1;
      break;
      case eMemory_CPU:
         return (MEM_2KB>>4);
      break;
      case eMemory_PPUregs:
         return 1;
      break;
      case eMemory_IOregs:
         return 6;
      break;
      case eMemory_cartSRAM:
         return (MEM_8KB>>4);
      break;
      case eMemory_cartEXRAM:
         return (MEM_1KB>>4);
      break;
      case eMemory_cartCHRMEM:
         return (MEM_8KB>>4);
      break;
      case eMemory_cartMapper:
         return CROM::NUMREGISTERS();
      break;
      case eMemory_PPU:
         return (MEM_4KB>>4);
      break;
      case eMemory_PPUpalette:
         return (MEM_32B>>2);
      break;
      case eMemory_PPUoam:
         return (MEM_256B>>2);
      break;
   }
   return 0;
}

int CDebuggerMemoryDisplayModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   switch ( m_display )
   {
      case eMemory_CPUregs:
         return 6;
      break;
      case eMemory_cartMapper:
         if ( CROM::MAPPER() == 0 )
         {
            // Nothing to display here...
            return 0;
         }
         return 1;
      break;
      case eMemory_CPU:
      case eMemory_cartSRAM:
      case eMemory_cartEXRAM:
      case eMemory_cartCHRMEM:
      case eMemory_PPU:
         return 16;
      case eMemory_PPUregs:
         return 8;
      break;
      case eMemory_IOregs:
      case eMemory_PPUoam:
      case eMemory_PPUpalette:
         return 4;
      break;
   }
   return 0;
}

void CDebuggerMemoryDisplayModel::layoutChangedEvent()
{
   if ( m_display == eMemory_cartMapper )
   {
      // get the registers from the mapper just incase a cart has been loaded...
      m_tblRegisters = CROM::REGISTERS();
   }
   this->layoutChanged();
}
