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

#include "cnesrommapper075.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 075 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("Address", 0, 4, "%X", 16, "CHR Register 0", "CHR Register 1", "CHR Register 2", "CHR Register 3", "CHR Register 4", "CHR Register 5", "CHR Register 6", "CHR Register 7", "PRG Register 0", "PRG Register 1", "PRG Register 2", "PRG Register 3", "Mirroring", "IRQ Control", "IRQ Counter Low", "IRQ Counter High")
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("Expansion Sound Address", 0, 4, "%X", 16, "Channel 0 Frequency Low", "Channel 0 Frequency High", "Channel 1 Frequency Low", "Channel 1 Frequency High", "Channel 2 Frequency Low", "Channel 2 Frequency High", "Channel Disables", "Channel 0 Volume", "Channel 1 Volume", "Channel 2 Volume", "", "", "", "", "", "")
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("Expansion Sound Data", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Address", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xA000, "Data", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xC000, "Expansion Sound Address", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xE000, "Expansion Sound Data", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
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

CROMMapper075::CROMMapper075()
   : CROM(75)
{
   memset(m_reg,0,sizeof(m_reg));
   memset(m_prg,0,sizeof(m_prg));
   memset(m_chr,0,sizeof(m_chr));
}

CROMMapper075::~CROMMapper075()
{
}

void CROMMapper075::RESET ( bool soft )
{
   int32_t idx;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   for ( idx = 0; idx < 4; idx++ )
   {
      m_prg [ idx ] = 0;
   }

   for ( idx = 0; idx < 6; idx++ )
   {
      m_reg [ idx ] = 0x00;
   }

   for ( idx = 0; idx < 8; idx++ )
   {
      m_chr [ idx ] = 0;
   }

   m_PRGROMmemory.REMAP(0,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(1,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(2,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper075::SETCPU ( void )
{
   m_PRGROMmemory.REMAP(0,m_prg[0]);
   m_PRGROMmemory.REMAP(1,m_prg[1]);
   m_PRGROMmemory.REMAP(2,m_prg[2]);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
}

void CROMMapper075::SETPPU ( void )
{
   m_CHRmemory.REMAP(0,(m_chr[0]<<2)+0);
   m_CHRmemory.REMAP(1,(m_chr[1]<<2)+1);
   m_CHRmemory.REMAP(2,(m_chr[2]<<2)+2);
   m_CHRmemory.REMAP(3,(m_chr[3]<<2)+3);
   m_CHRmemory.REMAP(4,(m_chr[4]<<2)+0);
   m_CHRmemory.REMAP(5,(m_chr[5]<<2)+1);
   m_CHRmemory.REMAP(6,(m_chr[6]<<2)+2);
   m_CHRmemory.REMAP(7,(m_chr[7]<<2)+3);
}

uint32_t CROMMapper075::DEBUGINFO ( uint32_t addr )
{
   return m_reg [ ((addr-MEM_32KB)/MEM_8KB) ];
}

void CROMMapper075::HMAPPER ( uint32_t addr, uint8_t data )
{
   int reg = 0;

   switch ( addr&0xF000 )
   {
   case 0x8000:
      reg = 0;
      m_reg[reg] = data;
      m_prg [ 0 ] = data%m_numPrgBanks;
      SETCPU ();
      break;
   case 0x9000:
      reg = 1;
      m_reg[reg] = data;
      m_chr[0] &= 0x0F;
      m_chr[1] &= 0x0F;
      m_chr[2] &= 0x0F;
      m_chr[3] &= 0x0F;
      m_chr[4] &= 0x0F;
      m_chr[5] &= 0x0F;
      m_chr[6] &= 0x0F;
      m_chr[7] &= 0x0F;
      m_chr[0] |= ((data&0x02)<<3);
      m_chr[1] |= ((data&0x02)<<3);
      m_chr[2] |= ((data&0x02)<<3);
      m_chr[3] |= ((data&0x02)<<3);
      m_chr[4] |= ((data&0x04)<<2);
      m_chr[5] |= ((data&0x04)<<2);
      m_chr[6] |= ((data&0x04)<<2);
      m_chr[7] |= ((data&0x04)<<2);
      SETPPU ();
      switch ( data&0x01 )
      {
      case 0:
         CNES::NES()->PPU()->MIRRORVERT();
         break;
      case 1:
         CNES::NES()->PPU()->MIRRORHORIZ();
         break;
      }
      break;
   case 0xA000:
      reg = 2;
      m_reg[reg] = data;
      m_prg [ 1 ] = data%m_numPrgBanks;
      SETCPU ();
      break;
   case 0xC000:
      reg = 3;
      m_reg[reg] = data;
      m_prg [ 2 ] = data%m_numPrgBanks;
      SETCPU ();
      break;
   case 0xE000:
      reg = 4;
      m_reg[reg] = data;
      m_chr[0] &= (0x10);
      m_chr[1] &= (0x10);
      m_chr[2] &= (0x10);
      m_chr[3] &= (0x10);
      m_chr[0] |= (data&0x0F);
      m_chr[1] |= (data&0x0F);
      m_chr[2] |= (data&0x0F);
      m_chr[3] |= (data&0x0F);
      SETPPU ();
      break;
   case 0xF000:
      reg = 5;
      m_reg[reg] = data;
      m_chr[4] &= (0x10);
      m_chr[5] &= (0x10);
      m_chr[6] &= (0x10);
      m_chr[7] &= (0x10);
      m_chr[4] |= (data&0x0F);
      m_chr[5] |= (data&0x0F);
      m_chr[6] |= (data&0x0F);
      m_chr[7] |= (data&0x0F);
      SETPPU ();
      break;
   }

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
