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

#include "cnesrommapper021.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 021 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 5, "%X", 0)
};

static CBitfieldData* tbl9000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "Vertical", "Horizontal", "One-screen A", "One-screen B")
};

static CBitfieldData* tbl9004Bitfields [] =
{
   new CBitfieldData("PRG Swap Mode", 1, 1, "%X", 2, "Swap $8000", "Swap $C000")
};

static CBitfieldData* tblB000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 4, "%X", 0)
};

static CBitfieldData* tblF000Bitfields [] =
{
   new CBitfieldData("IRQ Latch LSN", 0, 4, "%X", 0)
};

static CBitfieldData* tblF002Bitfields [] =
{
   new CBitfieldData("IRQ Latch HSN", 0, 4, "%X", 0)
};

static CBitfieldData* tblF004Bitfields [] =
{
   new CBitfieldData("IRQ Enable after ack", 0, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Enable", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Mode", 2, 1, "%X", 2, "Scanline mode", "Cycle mode")
};

static CBitfieldData* tblF006Bitfields [] =
{
   new CBitfieldData("IRQ Acknowledge", 0, 8, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tbl9000Bitfields),
   new CRegisterData(0x9004, "PRG Swap Mode", nesMapperHighRead, nesMapperHighWrite, 1, tbl9004Bitfields),
   new CRegisterData(0xA000, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB002, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB004, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB006, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC000, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC002, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC004, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC006, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD002, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD004, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD006, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE000, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE002, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE004, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE006, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xF000, "IRQ Latch LSN", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields),
   new CRegisterData(0xF002, "IRQ Latch HSN", nesMapperHighRead, nesMapperHighWrite, 1, tblF002Bitfields),
   new CRegisterData(0xF004, "IRQ Control", nesMapperHighRead, nesMapperHighWrite, 3, tblF004Bitfields),
   new CRegisterData(0xF006, "IRQ Acknowledge", nesMapperHighRead, nesMapperHighWrite, 1, tblF006Bitfields),
   new CRegisterData(0x8000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tbl9000Bitfields),
   new CRegisterData(0x9080, "PRG Swap Mode", nesMapperHighRead, nesMapperHighWrite, 1, tbl9004Bitfields),
   new CRegisterData(0xA000, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB040, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB080, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB0C0, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC000, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC040, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC080, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC0C0, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD040, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD080, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xD0C0, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE000, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE040, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE080, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xE0C0, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xF000, "IRQ Latch LSN", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields),
   new CRegisterData(0xF040, "IRQ Latch HSN", nesMapperHighRead, nesMapperHighWrite, 1, tblF002Bitfields),
   new CRegisterData(0xF080, "IRQ Control", nesMapperHighRead, nesMapperHighWrite, 3, tblF004Bitfields),
   new CRegisterData(0xF0C0, "IRQ Acknowledge", nesMapperHighRead, nesMapperHighWrite, 1, tblF006Bitfields)
};

static const char* rowHeadings [] =
{
   "VRC4a",
   "VRC4c"
};

static const char* columnHeadings [] =
{
   "8000",
   "9000","9004\n9080",
   "A000",
   "B000","B002\nB040","B004\nB080","B006\nB0C0",
   "C000","C002\nC040","C004\nC080","C006\nC0C0",
   "D000","D002\nD040","D004\nD080","D006\nD0C0",
   "E000","E002\nE040","E004\nE080","E006\nE0C0",
   "F000","F002\nF040","F004\nF080","F006\nF0C0",
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,2,24,48,tblRegisters,rowHeadings,columnHeadings);

CROMMapper021::CROMMapper021()
   : CROM(21)
{
   memset(m_reg,0,sizeof(m_reg));
   memset(m_chr,0,sizeof(m_chr));
   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqPrescaler = 0;
   m_irqPrescalerPhase = 0;
   m_irqEnabled = false;
}

CROMMapper021::~CROMMapper021()
{
}

void CROMMapper021::RESET ( bool soft )
{
   int32_t idx;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqPrescaler = 0;
   m_irqPrescalerPhase = 0;
   m_irqEnabled = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper021::SYNCCPU ( void )
{
   uint8_t phases[3] = { 114, 114, 113 };

   if ( m_reg[22]&0x02 )
   {
      if ( m_reg[22]&0x04 )
      {
         // Cycle mode counter
         if ( m_irqCounter == 0xFF )
         {
            m_irqCounter = m_irqReload;
            CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

            if ( nesIsDebuggable() )
            {
               // Check for IRQ breakpoint...
               CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
            }
         }
         else
         {
            m_irqCounter++;
         }
      }
      else
      {
         // Scanline mode counter
         m_irqPrescaler++;
         if ( m_irqPrescaler >= phases[m_irqPrescalerPhase] )
         {
            m_irqPrescaler = 0;
            m_irqPrescalerPhase++;
            m_irqPrescalerPhase %= 3;

            if ( m_irqCounter == 0xFF )
            {
               m_irqCounter = m_irqReload;
               CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

               if ( nesIsDebuggable() )
               {
                  // Check for IRQ breakpoint...
                  CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
               }
            }
            else
            {
               m_irqCounter++;
            }
         }
      }
   }
}

uint32_t CROMMapper021::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x8000:
   case 0x8002:
   case 0x8040:
   case 0x8004:
   case 0x8080:
   case 0x8006:
   case 0x80C0:
      return m_reg[0];
      break;
   case 0x9000:
   case 0x9002:
   case 0x9040:
      return m_reg[1];
      break;
   case 0x9004:
   case 0x9080:
   case 0x9006:
   case 0x90C0:
      return m_reg[2];
      break;
   case 0xA000:
   case 0xA002:
   case 0xA040:
   case 0xA004:
   case 0xA080:
   case 0xA006:
   case 0xA0C0:
      return m_reg[3];
      break;
   case 0xB000:
      return m_reg[4];
      break;
   case 0xB002:
   case 0xB040:
      return m_reg[5];
      break;
   case 0xB004:
   case 0xB080:
      return m_reg[6];
      break;
   case 0xB006:
   case 0xB0C0:
      return m_reg[7];
      break;
   case 0xC000:
      return m_reg[8];
      break;
   case 0xC002:
   case 0xC040:
      return m_reg[9];
      break;
   case 0xC004:
   case 0xC080:
      return m_reg[10];
      break;
   case 0xC006:
   case 0xC0C0:
      return m_reg[11];
      break;
   case 0xD000:
      return m_reg[12];
      break;
   case 0xD002:
   case 0xD040:
      return m_reg[13];
      break;
   case 0xD004:
   case 0xD080:
      return m_reg[14];
      break;
   case 0xD006:
   case 0xD0C0:
      return m_reg[15];
      break;
   case 0xE000:
      return m_reg[16];
      break;
   case 0xE002:
   case 0xE040:
      return m_reg[17];
      break;
   case 0xE004:
   case 0xE080:
      return m_reg[18];
      break;
   case 0xE006:
   case 0xE0C0:
      return m_reg[19];
      break;
   case 0xF000:
      return m_reg[20];
      break;
   case 0xF002:
   case 0xF040:
      return m_reg[21];
      break;
   case 0xF004:
   case 0xF080:
      return m_reg[22];
      break;
   case 0xF006:
   case 0xF0C0:
      return m_reg[23];
      break;
   }
}

void CROMMapper021::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg;

   switch ( addr )
   {
   case 0x8000:
   case 0x8002:
   case 0x8040:
   case 0x8004:
   case 0x8080:
   case 0x8006:
   case 0x80C0:
      reg = 0;
      m_reg[0] = data;
      if ( m_reg[2]&0x02 )
      {
         m_PRGROMmemory.REMAP(0,m_numPrgBanks-2);
         m_PRGROMmemory.REMAP(2,data&0x1F);
      }
      else
      {
         m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
         m_PRGROMmemory.REMAP(0,data&0x1F);
      }
      break;
   case 0x9000:
   case 0x9002:
   case 0x9040:
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
   case 0x9004:
   case 0x9080:
   case 0x9006:
   case 0x90C0:
      reg = 2;
      m_reg[2] = data;
      if ( m_reg[2]&0x02 )
      {
         m_PRGROMmemory.REMAP(0,m_numPrgBanks-2);
      }
      else
      {
         m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
      }
      break;
   case 0xA000:
   case 0xA002:
   case 0xA040:
   case 0xA004:
   case 0xA080:
   case 0xA006:
   case 0xA0C0:
      reg = 3;
      m_reg[3] = data;
      m_PRGROMmemory.REMAP(1,data&0x1F);
      break;
   case 0xB000:
      reg = 4;
      m_reg[4] = data;
      m_chr[0] &= 0xF0;
      m_chr[0] |= (data&0x0F);
      m_CHRmemory.REMAP(0,m_chr[0]);
      break;
   case 0xB002:
   case 0xB040:
      reg = 5;
      m_reg[5] = data;
      m_chr[0] &= 0x0F;
      m_chr[0] |= (data<<4);
      m_CHRmemory.REMAP(0,m_chr[0]);
      break;
   case 0xB004:
   case 0xB080:
      reg = 6;
      m_reg[6] = data;
      m_chr[1] &= 0xF0;
      m_chr[1] |= (data&0x0F);
      m_CHRmemory.REMAP(1,m_chr[1]);
      break;
   case 0xB006:
   case 0xB0C0:
      reg = 7;
      m_reg[7] = data;
      m_chr[1] &= 0x0F;
      m_chr[1] |= (data<<4);
      m_CHRmemory.REMAP(1,m_chr[1]);
      break;
   case 0xC000:
      reg = 8;
      m_reg[8] = data;
      m_chr[2] &= 0xF0;
      m_chr[2] |= (data&0x0F);
      m_CHRmemory.REMAP(2,m_chr[2]);
      break;
   case 0xC002:
   case 0xC040:
      reg = 9;
      m_reg[9] = data;
      m_chr[2] &= 0x0F;
      m_chr[2] |= (data<<4);
      m_CHRmemory.REMAP(2,m_chr[2]);
      break;
   case 0xC004:
   case 0xC080:
      reg = 10;
      m_reg[10] = data;
      m_chr[3] &= 0xF0;
      m_chr[3] |= (data&0x0F);
      m_CHRmemory.REMAP(3,m_chr[3]);
      break;
   case 0xC006:
   case 0xC0C0:
      reg = 11;
      m_reg[11] = data;
      m_chr[3] &= 0x0F;
      m_chr[3] |= (data<<4);
      m_CHRmemory.REMAP(3,m_chr[3]);
      break;
   case 0xD000:
      reg = 12;
      m_reg[12] = data;
      m_chr[4] &= 0xF0;
      m_chr[4] |= (data&0x0F);
      m_CHRmemory.REMAP(4,m_chr[4]);
      break;
   case 0xD002:
   case 0xD040:
      reg = 13;
      m_reg[13] = data;
      m_chr[4] &= 0x0F;
      m_chr[4] |= (data<<4);
      m_CHRmemory.REMAP(4,m_chr[4]);
      break;
   case 0xD004:
   case 0xD080:
      reg = 14;
      m_reg[14] = data;
      m_chr[5] &= 0xF0;
      m_chr[5] |= (data&0x0F);
      m_CHRmemory.REMAP(5,m_chr[5]);
      break;
   case 0xD006:
   case 0xD0C0:
      reg = 15;
      m_reg[15] = data;
      m_chr[5] &= 0x0F;
      m_chr[5] |= (data<<4);
      m_CHRmemory.REMAP(5,m_chr[5]);
      break;
   case 0xE000:
      reg = 16;
      m_reg[16] = data;
      m_chr[6] &= 0xF0;
      m_chr[6] |= (data&0x0F);
      m_CHRmemory.REMAP(6,m_chr[6]);
      break;
   case 0xE002:
   case 0xE040:
      reg = 17;
      m_reg[17] = data;
      m_chr[6] &= 0x0F;
      m_chr[6] |= (data<<4);
      m_CHRmemory.REMAP(6,m_chr[6]);
      break;
   case 0xE004:
   case 0xE080:
      reg = 18;
      m_reg[18] = data;
      m_chr[7] &= 0xF0;
      m_chr[7] |= (data&0x0F);
      m_CHRmemory.REMAP(7,m_chr[7]);
      break;
   case 0xE006:
   case 0xE0C0:
      reg = 19;
      m_reg[19] = data;
      m_chr[7] &= 0x0F;
      m_chr[7] |= (data<<4);
      m_CHRmemory.REMAP(7,m_chr[7]);
      break;
   case 0xF000:
      reg = 20;
      m_reg[20] = data;
      m_irqReload &= 0xF0;
      m_irqReload |= (data&0x0F);
      break;
   case 0xF002:
   case 0xF040:
      reg = 21;
      m_reg[21] = data;
      m_irqReload &= 0x0F;
      m_irqReload |= (data<<4);
      break;
   case 0xF004:
   case 0xF080:
      reg = 22;
      m_reg[22] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      if ( m_reg[22]&0x02 )
      {
         m_irqCounter = m_irqReload;
         m_irqPrescaler = 0;
         m_irqPrescalerPhase = 0;
         m_irqEnabled = true;
      }
      else
      {
         m_irqEnabled = false;
      }
      break;
   case 0xF006:
   case 0xF0C0:
      reg = 23;
      m_reg[23] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      m_reg[22] &= 0xFD;
      m_reg[22] |= ((m_reg[22]&0x01)<<1);
      if ( m_reg[22]&0x02 )
      {
         m_irqEnabled = true;
      }
      else
      {
         m_irqEnabled = false;
      }
      break;
   }

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
