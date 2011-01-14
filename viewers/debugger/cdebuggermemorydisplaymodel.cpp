#include "cdebuggermemorydisplaymodel.h"

#include "emulator_core.h"

#include "dbg_cnes.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesrom.h"
#include "dbg_cnesapu.h"
#include "dbg_cnesppu.h"

#include "cnessystempalette.h"

#include <QColor>
#include <QBrush>
#include <QRgb>

static char modelStringBuffer [ 2048 ];
   
CDebuggerMemoryDisplayModel::CDebuggerMemoryDisplayModel(QObject*, eMemoryType display)
{
   m_display = display;
   m_tblRegisters = NULL;

   switch ( m_display )
   {
      case eMemory_CPU:
         m_offset = 0x0000;
         m_length = MEM_2KB;
         break;
      case eMemory_CPUregs:
         m_offset = 0; // bogus...
         m_length = 6; // bogus...
         break;
      case eMemory_PPUregs:
         m_offset = 0x2000;
         m_length = MEM_8KB; // it's really 8 bytes mirrored to 0x4000...
         break;
      case eMemory_IOregs:
         m_offset = 0x4000;
         m_length = 0x18; // this should perhaps be MEM_4KB or something else...mirroring?
         break;
      case eMemory_cartSRAM:
         m_offset = 0x6000;
         m_length = MEM_8KB;
         break;
      case eMemory_cartROM:
         m_offset = 0x8000;
         m_length = MEM_32KB;
         break;
      case eMemory_cartEXRAM:
         m_offset = 0x5C00;
         m_length = MEM_1KB;
         break;
      case eMemory_cartCHRMEM:
         m_offset = 0;
         m_length = MEM_8KB;
         break;
      case eMemory_cartMapper:
         m_offset = 0; // bogus...
         m_length = 0; // bogus...
         break;
      case eMemory_PPU:
         m_offset = 0x2000;
         m_length = MEM_8KB-MEM_256B; // mirroring but last 256 bytes are palette...
         break;
      case eMemory_PPUpalette:
         m_offset = 0x3F00;
         m_length = MEM_256B; // it's really 32 bytes mirrored to 0x4000...
         break;
      case eMemory_PPUoam:
         m_offset = 0; // bogus...
         m_length = MEM_256B; // bogus...
         break;
   }
}

CDebuggerMemoryDisplayModel::~CDebuggerMemoryDisplayModel()
{
}

QVariant CDebuggerMemoryDisplayModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if (m_display==eMemory_PPUpalette && role == Qt::BackgroundRole)
   {
      return QBrush(QColor(nesGetPaletteRedComponent((long)index.internalPointer()),nesGetPaletteGreenComponent((long)index.internalPointer()),nesGetPaletteBlueComponent((long)index.internalPointer())));
   }

   if (m_display==eMemory_PPUpalette && role == Qt::ForegroundRole)
   {
      QColor col = QColor(nesGetPaletteRedComponent((long)index.internalPointer()),nesGetPaletteGreenComponent((long)index.internalPointer()),nesGetPaletteBlueComponent((long)index.internalPointer()));

      if ((((double)col.red() +
            (double)col.green() +
            (double)col.blue()) / (double)3) < 200)
      {
         return QBrush(QColor(255, 255, 255));
      }
      else
      {
         return QBrush(QColor(0, 0, 0));
      }
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

#if __WORDSIZE == 64
   sprintf ( modelStringBuffer, "%02X", (long)index.internalPointer() );
#else
   sprintf ( modelStringBuffer, "%02X", (int)index.internalPointer() );
#endif
   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CDebuggerMemoryDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   if ( (m_display != eMemory_cartCHRMEM) ||
         (!(CROMDBG::IsWriteProtected())) )
   {
      flags |= Qt::ItemIsEditable;
   }

   return flags;
}

QVariant CDebuggerMemoryDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:

            switch ( section )
            {
               case 0:
                  sprintf ( modelStringBuffer, "PC" );
                  break;
               case 1:
                  sprintf ( modelStringBuffer, "A" );
                  break;
               case 2:
                  sprintf ( modelStringBuffer, "X" );
                  break;
               case 3:
                  sprintf ( modelStringBuffer, "Y" );
                  break;
               case 4:
                  sprintf ( modelStringBuffer, "SP" );
                  break;
               case 5:
                  sprintf ( modelStringBuffer, "Flags" );
                  break;
            }

            break;
         default:
            sprintf ( modelStringBuffer, "x%1X", section );
            break;
      }
   }
   else
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:
            sprintf ( modelStringBuffer, "CPU" );
            break;
         case eMemory_cartMapper:

            if ( m_tblRegisters )
            {
               sprintf ( modelStringBuffer, "$%04X", m_tblRegisters[section]->GetAddr());
            }

            break;
         case eMemory_CPU:
         case eMemory_cartROM:
         case eMemory_cartSRAM:
         case eMemory_cartEXRAM:
         case eMemory_cartCHRMEM:
         case eMemory_PPU:
         case eMemory_PPUregs:
            sprintf ( modelStringBuffer, "$%04X", m_offset+(section<<4) );
            break;
         case eMemory_PPUoam:
            sprintf ( modelStringBuffer, "%d", section );
            break;
         case eMemory_PPUpalette:
         case eMemory_IOregs:
            sprintf ( modelStringBuffer, "$%04X", m_offset+(section<<2) );
            break;
      }
   }

   return QVariant(modelStringBuffer);
}

bool CDebuggerMemoryDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
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
         case eMemory_CPU:
            C6502DBG::_MEM(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_PPUregs:
            CPPUDBG::_PPU(m_offset+index.column(), data);
            break;
         case eMemory_IOregs:
            CAPUDBG::_APU(m_offset+(index.row()<<2)+index.column(), data);
            break;
         case eMemory_cartROM:
            CROMDBG::HIGHWRITE(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_cartSRAM:
            CROMDBG::SRAM(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_cartEXRAM:
            CROMDBG::EXRAM(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_cartCHRMEM:
            CROMDBG::CHRMEM(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_cartMapper:

            if ( m_tblRegisters[index.row()]->GetAddr() < MEM_32KB )
            {
               CROMDBG::LOWWRITE(m_tblRegisters[index.row()]->GetAddr(), data);
            }
            else
            {
               CROMDBG::HIGHWRITE(m_tblRegisters[index.row()]->GetAddr(), data);
            }

            break;
         case eMemory_PPU:
            CPPUDBG::_MEM(m_offset+(index.row()<<4)+index.column(), data);
            break;
         case eMemory_PPUpalette:
            CPPUDBG::_MEM(m_offset+(index.row()<<2)+index.column(), data);
            break;
         case eMemory_PPUoam:
            CPPUDBG::_OAM(index.column(), index.row(), data);
            break;
      }

      emit dataChanged(index,index);
   }

   return ok;
}

QModelIndex CDebuggerMemoryDisplayModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      switch ( m_display )
      {
         case eMemory_CPUregs:

            switch ( column )
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
         case eMemory_CPU:
            return createIndex(row, column, (int)C6502DBG::_MEM(m_offset+(row<<4)+column));
            break;
         case eMemory_PPUregs:
            return createIndex(row, column, (int)CPPUDBG::_PPU(m_offset+column));
            break;
         case eMemory_IOregs:
            return createIndex(row, column, (int)CAPUDBG::_APU(m_offset+(row<<2)+column));
            break;
         case eMemory_cartROM:
            return createIndex(row, column, (int)CROMDBG::PRGROM(m_offset+(row<<4)+column));
            break;
         case eMemory_cartSRAM:
            return createIndex(row, column, (int)CROMDBG::SRAM(m_offset+(row<<4)+column));
            break;
         case eMemory_cartEXRAM:
            return createIndex(row, column, (int)CROMDBG::EXRAM(m_offset+(row<<4)+column));
            break;
         case eMemory_cartCHRMEM:
            return createIndex(row, column, (int)CROMDBG::CHRMEM(m_offset+(row<<4)+column));
            break;
         case eMemory_cartMapper:

            if ( m_tblRegisters )
            {
               if ( m_tblRegisters[row]->GetAddr() < MEM_32KB )
               {
                  return createIndex(row, column, (int)CROMDBG::LOWREAD(m_tblRegisters[row]->GetAddr()));
               }
               else
               {
                  return createIndex(row, column, (int)CROMDBG::HIGHREAD(m_tblRegisters[row]->GetAddr()));
               }
            }
            else
            {
               return QModelIndex();
            }

            break;
         case eMemory_PPU:
            return createIndex(row, column, (int)CPPUDBG::_MEM(m_offset+(row<<4)+column));
            break;
         case eMemory_PPUpalette:
            return createIndex(row, column, (int)CPPUDBG::_MEM(m_offset+(row<<2)+column));
            break;
         case eMemory_PPUoam:
            return createIndex(row, column, (int)CPPUDBG::_OAM(column,row));
            break;
      }
   }

   return QModelIndex();
}

int CDebuggerMemoryDisplayModel::rowCount(const QModelIndex&) const
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
      case eMemory_cartROM:
         return (MEM_32KB>>4);
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
         return nesGetSizeOfCartridgeRegisterDatabase();
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

int CDebuggerMemoryDisplayModel::columnCount(const QModelIndex& parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }

   switch ( m_display )
   {
      case eMemory_CPUregs:
         return NUM_CPU_REGISTERS;
         break;
      case eMemory_cartMapper:

         if ( m_tblRegisters )
         {
            return 1;
         }

         // Nothing to display here...
         return 0;
         break;
      case eMemory_CPU:
      case eMemory_cartROM:
      case eMemory_cartSRAM:
      case eMemory_cartEXRAM:
      case eMemory_cartCHRMEM:
      case eMemory_PPU:
         return 16;
      case eMemory_PPUregs:
         return NUM_PPU_REGISTERS;
         break;
      case eMemory_IOregs:
      case eMemory_PPUoam:
      case eMemory_PPUpalette:
         return 4;
         break;
   }

   return 0;
}

void CDebuggerMemoryDisplayModel::update()
{
   if ( m_display == eMemory_cartMapper )
   {
      // get the registers from the mapper just incase a cart has been loaded...
      m_tblRegisters = nesGetCartridgeRegisterDatabase();
   }

   emit layoutChanged();
}
