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

uint8_t  CROMMapper001::m_reg [] = { 0x0C, 0x00, 0x00, 0x00 };
uint8_t  CROMMapper001::m_regdef [] = { 0x0C, 0x00, 0x00, 0x00 };
uint8_t  CROMMapper001::m_sr = 0x00;
uint8_t  CROMMapper001::m_sel = 0x00;
uint8_t  CROMMapper001::m_srCount = 0;
uint32_t CROMMapper001::m_cpuCycleOfLastWrite = 0xFFFFFFFF;
uint32_t CROMMapper001::m_cpuCycle = 0xFFFFFFFF;

CROMMapper001::CROMMapper001()
{
}

CROMMapper001::~CROMMapper001()
{
}

void CROMMapper001::RESET ( bool soft )
{
   int32_t idx;

   m_mapper = 1;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_sr = 0;
   m_srCount = 0;

   m_cpuCycleOfLastWrite = 0xFFFFFFFF;
   m_cpuCycle = 0xFFFFFFFF;

   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = m_regdef [ idx ];
   }

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper001::LOAD ( MapperState* data )
{
   m_reg [ 0 ] = data->data.mapper001.reg [ 0 ];
   m_reg [ 1 ] = data->data.mapper001.reg [ 1 ];
   m_reg [ 2 ] = data->data.mapper001.reg [ 2 ];
   m_reg [ 3 ] = data->data.mapper001.reg [ 3 ];
   m_sel = data->data.mapper001.sel;
   m_sr = data->data.mapper001.sr;
   m_srCount = data->data.mapper001.srCount;

   if ( m_reg[0]&0x02 )
   {
      if ( m_reg[0]&0x01 )
      {
         CPPU::MIRRORHORIZ ();
      }
      else
      {
         CPPU::MIRRORVERT ();
      }
   }
   else
   {
      CPPU::MIRROR ( m_reg[0]&0x01 );
   }

   CROM::LOAD ( data );
}

void CROMMapper001::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );

   data->data.mapper001.reg [ 0 ] = m_reg [ 0 ];
   data->data.mapper001.reg [ 1 ] = m_reg [ 1 ];
   data->data.mapper001.reg [ 2 ] = m_reg [ 2 ];
   data->data.mapper001.reg [ 3 ] = m_reg [ 3 ];
   data->data.mapper001.sel = m_sel;
   data->data.mapper001.sr = m_sr;
   data->data.mapper001.srCount = m_srCount;
}

void CROMMapper001::SYNCCPU()
{
   // This may not be the actual CPU cycle but it doesn't matter.
   m_cpuCycle++;
}

uint32_t CROMMapper001::MAPPER ( uint32_t addr )
{
   return m_reg [ (addr-MEM_32KB)/MEM_8KB ];
}

void CROMMapper001::MAPPER ( uint32_t addr, uint8_t data )
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
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
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
                     CPPU::MIRRORHORIZ ();
                  }
                  else
                  {
                     CPPU::MIRRORVERT ();
                  }
               }
               else
               {
                  CPPU::MIRROR ( m_reg[0]&0x01 );
               }

               break;
            case 1:

               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 0 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+0 ];
                     m_pCHRmemory [ 1 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+1 ];
                     m_pCHRmemory [ 2 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+2 ];
                     m_pCHRmemory [ 3 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+3 ];
                  }
                  else
                  {
                     m_pCHRmemory [ 0 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+0 ];
                     m_pCHRmemory [ 1 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+1 ];
                     m_pCHRmemory [ 2 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+2 ];
                     m_pCHRmemory [ 3 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+3 ];
                     m_pCHRmemory [ 4 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+4 ];
                     m_pCHRmemory [ 5 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+5 ];
                     m_pCHRmemory [ 6 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+6 ];
                     m_pCHRmemory [ 7 ] = m_CHRmemory [ ((m_reg[1]&0x1F)<<2)+7 ];
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 0 ] = m_CHRmemory [ 0 ];
                     m_pCHRmemory [ 1 ] = m_CHRmemory [ 1 ];
                     m_pCHRmemory [ 2 ] = m_CHRmemory [ 2 ];
                     m_pCHRmemory [ 3 ] = m_CHRmemory [ 3 ];
                  }
               }

               break;
            case 2:

               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 4 ] = m_CHRmemory [ ((m_reg[2]&0x1F)<<2)+0 ];
                     m_pCHRmemory [ 5 ] = m_CHRmemory [ ((m_reg[2]&0x1F)<<2)+1 ];
                     m_pCHRmemory [ 6 ] = m_CHRmemory [ ((m_reg[2]&0x1F)<<2)+2 ];
                     m_pCHRmemory [ 7 ] = m_CHRmemory [ ((m_reg[2]&0x1F)<<2)+3 ];
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 4 ] = m_CHRmemory [ 4 ];
                     m_pCHRmemory [ 5 ] = m_CHRmemory [ 5 ];
                     m_pCHRmemory [ 6 ] = m_CHRmemory [ 6 ];
                     m_pCHRmemory [ 7 ] = m_CHRmemory [ 7 ];
                  }
               }

               break;
            case 3:

               if ( m_reg[0]&0x08 )
               {
                  bank = (m_reg[3]&0x0F)<<1;

                  if ( m_reg[0]&0x04 )
                  {
                     m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
                     m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
                  }
                  else
                  {
                     m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank ];
                     m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+1 ];
                  }
               }
               else
               {
                  bank = m_reg[3]&0x0F;
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
                  m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
                  m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];
               }

               break;
         }

         if ( nesIsDebuggable() )
         {
            // Check mapper state breakpoints...
            // m_sel is convenient register number...
            CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,m_sel);
         }
      }
   }
}
