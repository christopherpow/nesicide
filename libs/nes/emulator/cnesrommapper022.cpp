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

#include "cnesrommapper022.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 022 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
};

static CBitfieldData* tbl9000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "Vertical", "Horizontal", "One-screen A", "One-screen B")
};

static CBitfieldData* tblB000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 4, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tbl9000Bitfields),
   new CRegisterData(0xA000, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB001, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB002, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB003, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC000, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC001, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC002, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC003, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD001, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD002, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD003, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE000, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE001, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE002, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE003, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","9000","A000",
   "B000","B001","B002","B003",
   "C000","C001","C002","C003",
   "D000","D001","D002","D003",
   "E000","E001","E002","E003"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,19,19,tblRegisters,rowHeadings,columnHeadings);

CROMMapper022::CROMMapper022()
   : CROM(22)
{
   memset(m_reg,0,sizeof(m_reg));
}

CROMMapper022::~CROMMapper022()
{
}

void CROMMapper022::RESET ( bool soft )
{
   m_mapper = 22;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper022::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x8000:
   case 0x8001:
   case 0x8002:
   case 0x8003:
      return m_reg[0];
      break;
   case 0x9000:
   case 0x9001:
   case 0x9002:
   case 0x9003:
      return m_reg[1];
      break;
   case 0xA000:
   case 0xA001:
   case 0xA002:
   case 0xA003:
      return m_reg[2];
      break;
   case 0xB000:
      return m_reg[3];
      break;
   case 0xB001:
      return m_reg[4];
      break;
   case 0xB002:
      return m_reg[5];
      break;
   case 0xB003:
      return m_reg[6];
      break;
   case 0xC000:
      return m_reg[7];
      break;
   case 0xC001:
      return m_reg[8];
      break;
   case 0xC002:
      return m_reg[9];
      break;
   case 0xC003:
      return m_reg[10];
      break;
   case 0xD000:
      return m_reg[11];
      break;
   case 0xD001:
      return m_reg[12];
      break;
   case 0xD002:
      return m_reg[13];
      break;
   case 0xD003:
      return m_reg[14];
      break;
   case 0xE000:
      return m_reg[15];
      break;
   case 0xE001:
      return m_reg[16];
      break;
   case 0xE002:
      return m_reg[17];
      break;
   case 0xE003:
      return m_reg[18];
      break;
   }
   return 0xA1; // garbage
}

void CROMMapper022::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr )
   {
   case 0x8000:
   case 0x8001:
   case 0x8002:
   case 0x8003:
      reg = 0;
      m_reg[0] = data;
      m_PRGROMmemory.REMAP(0,data&0xF);
      break;
   case 0x9000:
   case 0x9001:
   case 0x9002:
   case 0x9003:
      reg = 1;
      m_reg[1] = data;
      switch ( data&0x03 )
      {
      case 0:
         CNES::NES()->PPU()->MIRRORVERT();
         break;
      case 1:
         CNES::NES()->PPU()->MIRRORHORIZ();
         break;
      case 2:
         CNES::NES()->PPU()->MIRROR(0,0,0,0);
         break;
      case 3:
         CNES::NES()->PPU()->MIRROR(1,1,1,1);
         break;
      }
      break;
   case 0xA000:
   case 0xA001:
   case 0xA002:
   case 0xA003:
      reg = 2;
      m_reg[2] = data;
      m_PRGROMmemory.REMAP(1,data&0xF);
      break;
   case 0xB000:
      reg = 3;
      m_reg[3] = data;
      m_chr[0] &= 0xF0;
      m_chr[0] |= (data&0x0F);
      m_CHRmemory.REMAP(0,m_chr[0]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xB001:
      reg = 4;
      m_reg[4] = data;
      m_chr[1] &= 0xF0;        // NOTE: swapped VRC2a
      m_chr[1] |= (data&0x0F); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(1,m_chr[1]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xB002:
      reg = 5;
      m_reg[5] = data;
      m_chr[0] &= 0x0F;      // NOTE: swapped VRC2a
      m_chr[0] |= (data<<4); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(0,m_chr[0]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xB003:
      reg = 6;
      m_reg[6] = data;
      m_chr[1] &= 0x0F;
      m_chr[1] |= (data<<4);
      m_CHRmemory.REMAP(1,m_chr[1]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xC000:
      reg = 7;
      m_reg[7] = data;
      m_chr[2] &= 0xF0;
      m_chr[2] |= (data&0x0F);
      m_CHRmemory.REMAP(2,m_chr[2]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xC001:
      reg = 8;
      m_reg[8] = data;
      m_chr[3] &= 0xF0;        // NOTE: swapped VRC2a
      m_chr[3] |= (data&0x0F); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(3,m_chr[3]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xC002:
      reg = 9;
      m_reg[9] = data;
      m_chr[2] &= 0x0F;      // NOTE: swapped VRC2a
      m_chr[2] |= (data<<4); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(2,m_chr[2]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xC003:
      reg = 10;
      m_reg[10] = data;
      m_chr[3] &= 0x0F;
      m_chr[3] |= (data<<4);
      m_CHRmemory.REMAP(3,m_chr[3]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xD000:
      reg = 11;
      m_reg[11] = data;
      m_chr[4] &= 0xF0;
      m_chr[4] |= (data&0x0F);
      m_CHRmemory.REMAP(4,m_chr[4]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xD001:
      reg = 12;
      m_reg[12] = data;
      m_chr[5] &= 0xF0;        // NOTE: swapped VRC2a
      m_chr[5] |= (data&0x0F); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(5,m_chr[5]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xD002:
      reg = 13;
      m_reg[13] = data;
      m_chr[4] &= 0x0F;      // NOTE: swapped VRC2a
      m_chr[4] |= (data<<4); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(4,m_chr[4]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xD003:
      reg = 14;
      m_reg[14] = data;
      m_chr[5] &= 0x0F;
      m_chr[5] |= (data<<4);
      m_CHRmemory.REMAP(5,m_chr[5]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xE000:
      reg = 15;
      m_reg[15] = data;
      m_chr[6] &= 0xF0;
      m_chr[6] |= (data&0x0F);
      m_CHRmemory.REMAP(6,m_chr[6]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xE001:
      reg = 16;
      m_reg[16] = data;
      m_chr[7] &= 0xF0;        // NOTE: swapped VRC2a
      m_chr[7] |= (data&0x0F); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(7,m_chr[7]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xE002:
      reg = 17;
      m_reg[17] = data;
      m_chr[6] &= 0x0F;      // NOTE: swapped VRC2a
      m_chr[6] |= (data<<4); // NOTE: swapped VRC2a
      m_CHRmemory.REMAP(6,m_chr[6]>>1); // NOTE: >>1 VRC2a only
      break;
   case 0xE003:
      reg = 18;
      m_reg[18] = data;
      m_chr[7] &= 0x0F;
      m_chr[7] |= (data<<4);
      m_CHRmemory.REMAP(7,m_chr[7]>>1); // NOTE: >>1 VRC2a only
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
