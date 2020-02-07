//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cnesrommapper001.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 001 Registers
static CBitfieldData* tbl8000_9FFFBitfields [] =
{
   new CBitfieldData("CHR Mode", 4, 1, "%X", 2, "8KB mapping", "4KB mapping"),
   new CBitfieldData("PRG Size", 3, 1, "%X", 2, "32KB", "16KB"),
   new CBitfieldData("Slot Select", 2, 1, "%X", 2, "C000 swappable, 8000 fixed to page 00", "8000 swappable, C000 fixed to page 0F"),
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "One-screen 2000", "One-screen 2400", "Vertical", "Horizontal")
};

static CBitfieldData* tblA000_BFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 0", 0, 5, "%02X", 0)
};

static CBitfieldData* tblC000_DFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 1", 0, 5, "%02X", 0)
};

static CBitfieldData* tblE000_FFFFBitfields [] =
{
   new CBitfieldData("WRAM State", 4, 1, "%X", 2, "Enabled", "Disabled"),
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", nesMapperHighRead, nesMapperHighWrite, 4, tbl8000_9FFFBitfields),
   new CRegisterData(0xA000, "CHR Mapping 1", nesMapperHighRead, nesMapperHighWrite, 1, tblA000_BFFFBitfields),
   new CRegisterData(0xC000, "CHR Mapping 2", nesMapperHighRead, nesMapperHighWrite, 1, tblC000_DFFFBitfields),
   new CRegisterData(0xE000, "PRG Mapping", nesMapperHighRead, nesMapperHighWrite, 2, tblE000_FFFFBitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","A000","C000","E000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,4,4,tblRegisters,rowHeadings,columnHeadings);

CROMMapper001::CROMMapper001()
   : CROM(1)
{
   memset(m_reg,0,sizeof(m_reg));
   memset(m_regdef,0,sizeof(m_regdef));
   m_reg[0] = 0x0C;
   m_regdef[0] = 0x0C;
   m_sr = 0x00;
   m_sel = 0x00;
   m_srCount = 0;
   m_cpuCycleOfLastWrite = 0xFFFFFFFF;
   m_cpuCycle = 0xFFFFFFFF;
}

CROMMapper001::~CROMMapper001()
{
}

void CROMMapper001::RESET ( bool soft )
{
   int32_t idx;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_sr = 0;
   m_srCount = 0;

   m_cpuCycleOfLastWrite = 0xFFFFFFFF;
   m_cpuCycle = 0xFFFFFFFF;

   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = m_regdef [ idx ];
   }

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper001::SYNCCPU()
{
   // This may not be the actual CPU cycle but it doesn't matter.
   m_cpuCycle++;
}

uint32_t CROMMapper001::DEBUGINFO ( uint32_t addr )
{
   return m_reg [ (addr-MEM_32KB)/MEM_8KB ];
}

void CROMMapper001::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank = 0;

   // Discard this write if it's immediately following another write.
   if ( m_cpuCycle == m_cpuCycleOfLastWrite+1 )
   {
      return;
   }

   // Keep track of when we last wrote.
   m_cpuCycleOfLastWrite = m_cpuCycle;

   // Shift bits into registers...
   if ( data&0x80 )
   {
      m_sr = 0x00;
      m_srCount = 0;
      m_reg [ 0 ] |= m_regdef [ 0 ];
      m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
      m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
   }
   else
   {
      m_sr>>=1;
      m_sr|=((data&0x01)<<4);
      m_srCount++;

      if ( m_srCount == 5 )
      {
         m_sel = (addr-0x8000)/0x2000;
         m_reg [ m_sel ] = m_sr;
         m_srCount = 0;

         // Act on new register content...
         switch ( m_sel )
         {
            case 0:

               if ( m_reg[0]&0x02 )
               {
                  if ( m_reg[0]&0x01 )
                  {
                     CNES::NES()->PPU()->MIRRORHORIZ ();
                  }
                  else
                  {
                     CNES::NES()->PPU()->MIRRORVERT ();
                  }
               }
               else
               {
                  CNES::NES()->PPU()->MIRROR ( m_reg[0]&0x01 );
               }

               break;
            case 1:

               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_CHRmemory.REMAP(0,((m_reg[1]&0x1F)<<2)+0);
                     m_CHRmemory.REMAP(1,((m_reg[1]&0x1F)<<2)+1);
                     m_CHRmemory.REMAP(2,((m_reg[1]&0x1F)<<2)+2);
                     m_CHRmemory.REMAP(3,((m_reg[1]&0x1F)<<2)+3);
                  }
                  else
                  {
                     m_CHRmemory.REMAP(0,((m_reg[1]&0x1F)<<2)+0);
                     m_CHRmemory.REMAP(1,((m_reg[1]&0x1F)<<2)+1);
                     m_CHRmemory.REMAP(2,((m_reg[1]&0x1F)<<2)+2);
                     m_CHRmemory.REMAP(3,((m_reg[1]&0x1F)<<2)+3);
                     m_CHRmemory.REMAP(4,((m_reg[1]&0x1F)<<2)+4);
                     m_CHRmemory.REMAP(5,((m_reg[1]&0x1F)<<2)+5);
                     m_CHRmemory.REMAP(6,((m_reg[1]&0x1F)<<2)+6);
                     m_CHRmemory.REMAP(7,((m_reg[1]&0x1F)<<2)+7);
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_CHRmemory.REMAP(0,0);
                     m_CHRmemory.REMAP(1,1);
                     m_CHRmemory.REMAP(2,2);
                     m_CHRmemory.REMAP(3,3);
                  }
               }

               break;
            case 2:

               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_CHRmemory.REMAP(4,((m_reg[2]&0x1F)<<2)+0);
                     m_CHRmemory.REMAP(5,((m_reg[2]&0x1F)<<2)+1);
                     m_CHRmemory.REMAP(6,((m_reg[2]&0x1F)<<2)+2);
                     m_CHRmemory.REMAP(7,((m_reg[2]&0x1F)<<2)+3);
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_CHRmemory.REMAP(4,4);
                     m_CHRmemory.REMAP(5,5);
                     m_CHRmemory.REMAP(6,6);
                     m_CHRmemory.REMAP(7,7);
                  }
               }

               break;
            case 3:

               if ( m_reg[0]&0x08 )
               {
                  bank = (m_reg[3]&0x0F)<<1;

                  if ( m_reg[0]&0x04 )
                  {
                     m_PRGROMmemory.REMAP(0,bank);
                     m_PRGROMmemory.REMAP(1,bank+1);
                  }
                  else
                  {
                     m_PRGROMmemory.REMAP(2,bank);
                     m_PRGROMmemory.REMAP(3,bank+1);
                  }
               }
               else
               {
                  bank = m_reg[3]&0x0F;
                  m_PRGROMmemory.REMAP(0,bank);
                  m_PRGROMmemory.REMAP(1,bank+1);
                  m_PRGROMmemory.REMAP(2,bank+2);
                  m_PRGROMmemory.REMAP(3,bank+3);
               }

               break;
         }

         if ( nesIsDebuggable )
         {
            // Check mapper state breakpoints...
            // m_sel is convenient register number...
            CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,m_sel);
         }
      }
   }
}
