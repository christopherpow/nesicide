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

#include "cnesrommapper018.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 018 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank LSN", 0, 4, "%X", 0)
};

static CBitfieldData* tbl8001Bitfields [] =
{
   new CBitfieldData("PRG Bank MSN", 0, 4, "%X", 0)
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("CHR Bank LSN", 0, 4, "%X", 0)
};

static CBitfieldData* tblA001Bitfields [] =
{
   new CBitfieldData("CHR Bank MSN", 0, 4, "%X", 0)
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 0:3", 0, 4, "%X", 0)
};

static CBitfieldData* tblE001Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 4:7", 0, 4, "%X", 0)
};

static CBitfieldData* tblE002Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 8:11", 0, 4, "%X", 0)
};

static CBitfieldData* tblE003Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 12:15", 0, 4, "%X", 0)
};

static CBitfieldData* tblF000Bitfields [] =
{
   new CBitfieldData("IRQ Reset", 0, 8, "%X", 0)
};

static CBitfieldData* tblF001Bitfields [] =
{
   new CBitfieldData("IRQ Enabled", 0, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ counter size", 1, 3, "%X", 8, "16 bits", "12 bits", "8 bits", "8 bits", "4 bits", "4 bits", "4 bits", "4 bits")
};

static CBitfieldData* tblF002Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "Horizontal", "Vertical", "One-screen A", "One-screen B")
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x8001, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8001Bitfields),
   new CRegisterData(0x8002, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x8003, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8001Bitfields),
   new CRegisterData(0x9000, "PRG Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9001, "PRG Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8001Bitfields),
   new CRegisterData(0xA000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xA001, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xA002, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xA003, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xB000, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xB001, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xB002, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xB003, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xC000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xC001, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xC002, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xC003, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xD000, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xD001, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xD002, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xD003, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xE000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
   new CRegisterData(0xE001, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblE001Bitfields),
   new CRegisterData(0xE002, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblE002Bitfields),
   new CRegisterData(0xE003, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblE003Bitfields),
   new CRegisterData(0xF000, "IRQ Reset", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields),
   new CRegisterData(0xF001, "IRQ Control", nesMapperHighRead, nesMapperHighWrite, 2, tblF001Bitfields),
   new CRegisterData(0xF002, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblF002Bitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","8001","8002","8003",
   "9000","9001",
   "A000","A001","A002","A003",
   "B000","B001","B002","B003",
   "C000","C001","C002","C003",
   "D000","D001","D002","D003",
   "E000","E001","E002","E003",
   "F000","F001","F002"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,29,29,tblRegisters,rowHeadings,columnHeadings);

CROMMapper018::CROMMapper018()
   : CROM(18)
{
   memset(m_reg,0,sizeof(m_reg));
   memset(m_prg,0,sizeof(m_prg));
   memset(m_chr,0,sizeof(m_chr));
   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqEnabled = false;
}

CROMMapper018::~CROMMapper018()
{
}

void CROMMapper018::RESET ( bool soft )
{
   m_mapper = 18;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqEnabled = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper018::SYNCCPU ( void )
{
   uint16_t counterMask;
   uint16_t counter;
   uint8_t  size;

   size = ((m_reg[27]&0x0E)>>1);

   if ( m_irqEnabled )
   {
      // Get relevant counter bits.
      if ( size == 0 )
      {
         // 16 bits
         counterMask = 0xFFFF;
      }
      else if ( size == 1 )
      {
         // 12 bits
         counterMask = 0x0FFF;
      }
      else if ( size < 4 )
      {
         // 8 bits
         counterMask = 0x00FF;
      }
      else
      {
         // 4 bits
         counterMask = 0x000F;
      }

      counter = m_irqCounter&counterMask;
      counter--;
      m_irqCounter &= (~counterMask);
      m_irqCounter |= (counter&counterMask);

      if ( (m_irqCounter&counterMask) == counterMask )
      {
         CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

         if ( nesIsDebuggable )
         {
            // Check for IRQ breakpoint...
            CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
         }
      }
   }
}

uint32_t CROMMapper018::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x8000:
      return m_reg[0];
      break;
   case 0x8001:
      return m_reg[1];
      break;
   case 0x8002:
      return m_reg[2];
      break;
   case 0x8003:
      return m_reg[3];
      break;
   case 0x9000:
      return m_reg[4];
      break;
   case 0x9001:
      return m_reg[5];
      break;
   case 0xA000:
      return m_reg[6];
      break;
   case 0xA001:
      return m_reg[7];
      break;
   case 0xA002:
      return m_reg[8];
      break;
   case 0xA003:
      return m_reg[9];
      break;
   case 0xB000:
      return m_reg[10];
      break;
   case 0xB001:
      return m_reg[11];
      break;
   case 0xB002:
      return m_reg[12];
      break;
   case 0xB003:
      return m_reg[13];
      break;
   case 0xC000:
      return m_reg[14];
      break;
   case 0xC001:
      return m_reg[15];
      break;
   case 0xC002:
      return m_reg[16];
      break;
   case 0xC003:
      return m_reg[17];
      break;
   case 0xD000:
      return m_reg[18];
      break;
   case 0xD001:
      return m_reg[19];
      break;
   case 0xD002:
      return m_reg[20];
      break;
   case 0xD003:
      return m_reg[21];
      break;
   case 0xE000:
      return m_reg[22];
      break;
   case 0xE001:
      return m_reg[23];
      break;
   case 0xE002:
      return m_reg[24];
      break;
   case 0xE003:
      return m_reg[25];
      break;
   case 0xF000:
      return m_reg[26];
      break;
   case 0xF001:
      return m_reg[27];
      break;
   case 0xF002:
      return m_reg[28];
      break;
   }
   return 0xA1; // garbage
}

void CROMMapper018::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr )
   {
   case 0x8000:
      reg = 0;
      m_reg[0] = data;
      m_prg[0] &= 0xF0;
      m_prg[0] |= (data&0x0F);
      m_PRGROMmemory.REMAP(0,m_prg[0]&(m_numPrgBanks-1));
      break;
   case 0x8001:
      reg = 1;
      m_reg[1] = data;
      m_prg[0] &= 0x0F;
      m_prg[0] |= (data<<4);
      m_PRGROMmemory.REMAP(0,m_prg[0]&(m_numPrgBanks-1));
      break;
   case 0x8002:
      reg = 2;
      m_reg[2] = data;
      m_prg[1] &= 0xF0;
      m_prg[1] |= (data&0x0F);
      m_PRGROMmemory.REMAP(1,m_prg[1]&(m_numPrgBanks-1));
      break;
   case 0x8003:
      reg = 3;
      m_reg[3] = data;
      m_prg[1] &= 0x0F;
      m_prg[1] |= (data<<4);
      m_PRGROMmemory.REMAP(1,m_prg[1]&(m_numPrgBanks-1));
      break;
   case 0x9000:
      reg = 4;
      m_reg[4] = data;
      m_prg[2] &= 0xF0;
      m_prg[2] |= (data&0x0F);
      m_PRGROMmemory.REMAP(2,m_prg[2]&(m_numPrgBanks-1));
      break;
   case 0x9001:
      reg = 5;
      m_reg[5] = data;
      m_prg[2] &= 0x0F;
      m_prg[2] |= (data<<4);
      m_PRGROMmemory.REMAP(2,m_prg[2]&(m_numPrgBanks-1));
      break;
   case 0xA000:
      reg = 6;
      m_reg[6] = data;
      m_chr[0] &= 0xF0;
      m_chr[0] |= (data&0x0F);
      m_CHRmemory.REMAP(0,m_chr[0]);
      break;
   case 0xA001:
      reg = 7;
      m_reg[7] = data;
      m_chr[0] &= 0x0F;
      m_chr[0] |= (data<<4);
      m_CHRmemory.REMAP(0,m_chr[0]);
      break;
   case 0xA002:
      reg = 8;
      m_reg[8] = data;
      m_chr[1] &= 0xF0;
      m_chr[1] |= (data&0x0F);
      m_CHRmemory.REMAP(1,m_chr[1]);
      break;
   case 0xA003:
      reg = 9;
      m_reg[9] = data;
      m_chr[1] &= 0x0F;
      m_chr[1] |= (data<<4);
      m_CHRmemory.REMAP(1,m_chr[1]);
      break;
   case 0xB000:
      reg = 10;
      m_reg[10] = data;
      m_chr[2] &= 0xF0;
      m_chr[2] |= (data&0x0F);
      m_CHRmemory.REMAP(2,m_chr[2]);
      break;
   case 0xB001:
      reg = 11;
      m_reg[11] = data;
      m_chr[2] &= 0x0F;
      m_chr[2] |= (data<<4);
      m_CHRmemory.REMAP(2,m_chr[2]);
      break;
   case 0xB002:
      reg = 12;
      m_reg[12] = data;
      m_chr[3] &= 0xF0;
      m_chr[3] |= (data&0x0F);
      m_CHRmemory.REMAP(3,m_chr[3]);
      break;
   case 0xB003:
      reg = 13;
      m_reg[13] = data;
      m_chr[3] &= 0x0F;
      m_chr[3] |= (data<<4);
      m_CHRmemory.REMAP(3,m_chr[3]);
      break;
   case 0xC000:
      reg = 14;
      m_reg[14] = data;
      m_chr[4] &= 0xF0;
      m_chr[4] |= (data&0x0F);
      m_CHRmemory.REMAP(4,m_chr[4]);
      break;
   case 0xC001:
      reg = 15;
      m_reg[15] = data;
      m_chr[4] &= 0x0F;
      m_chr[4] |= (data<<4);
      m_CHRmemory.REMAP(4,m_chr[4]);
      break;
   case 0xC002:
      reg = 16;
      m_reg[16] = data;
      m_chr[5] &= 0xF0;
      m_chr[5] |= (data&0x0F);
      m_CHRmemory.REMAP(5,m_chr[5]);
      break;
   case 0xC003:
      reg = 17;
      m_reg[17] = data;
      m_chr[5] &= 0x0F;
      m_chr[5] |= (data<<4);
      m_CHRmemory.REMAP(5,m_chr[5]);
      break;
   case 0xD000:
      reg = 18;
      m_reg[18] = data;
      m_chr[6] &= 0xF0;
      m_chr[6] |= (data&0x0F);
      m_CHRmemory.REMAP(6,m_chr[6]);
      break;
   case 0xD001:
      reg = 19;
      m_reg[19] = data;
      m_chr[6] &= 0x0F;
      m_chr[6] |= (data<<4);
      m_CHRmemory.REMAP(6,m_chr[6]);
      break;
   case 0xD002:
      reg = 20;
      m_reg[20] = data;
      m_chr[7] &= 0xF0;
      m_chr[7] |= (data&0x0F);
      m_CHRmemory.REMAP(7,m_chr[7]);
      break;
   case 0xD003:
      reg = 21;
      m_reg[21] = data;
      m_chr[7] &= 0x0F;
      m_chr[7] |= (data<<4);
      m_CHRmemory.REMAP(7,m_chr[7]);
      break;
   case 0xE000:
      reg = 22;
      m_reg[22] = data;
      m_irqReload &= 0xFFF0;
      m_irqReload |= (data&0x0F);
      break;
   case 0xE001:
      reg = 23;
      m_reg[23] = data;
      m_irqReload &= 0xFF0F;
      m_irqReload |= ((data&0x0F)<<4);
      break;
   case 0xE002:
      reg = 24;
      m_reg[24] = data;
      m_irqReload &= 0xF0FF;
      m_irqReload |= ((data&0x0F)<<8);
      break;
   case 0xE003:
      reg = 25;
      m_reg[25] = data;
      m_irqReload &= 0x0FFF;
      m_irqReload |= ((data&0x0F)<<12);
      break;
   case 0xF000:
      reg = 26;
      m_reg[26] = data;
      m_irqCounter = m_irqReload;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0xF001:
      reg = 27;
      m_reg[27] = data;
      m_irqEnabled = data&0x01;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0xF002:
      reg = 28;
      m_reg[28] = data;
      switch ( data&0x03 )
      {
      case 0:
         CNES::NES()->PPU()->MIRRORHORIZ();
         break;
      case 1:
         CNES::NES()->PPU()->MIRRORVERT();
         break;
      case 2:
         CNES::NES()->PPU()->MIRROR(0,0,0,0);
         break;
      case 3:
         CNES::NES()->PPU()->MIRROR(1,1,1,1);
         break;
      }
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
