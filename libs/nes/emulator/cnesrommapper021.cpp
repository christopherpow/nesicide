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
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
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
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","9000","9004","A000",
   "B000","B002","B004","B006",
   "C000","C002","C004","C006",
   "D000","D002","D004","D006",
   "E000","E002","E004","E006",
   "F000","F002","F004","F006"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,24,24,tblRegisters,rowHeadings,columnHeadings);

uint8_t  CROMMapper021::m_reg [] = { 0x00, };
uint8_t  CROMMapper021::m_chr [] = { 0, };
uint8_t  CROMMapper021::m_irqReload = 0;
uint8_t  CROMMapper021::m_irqCounter = 0;
uint8_t  CROMMapper021::m_irqPrescaler = 0;
uint8_t  CROMMapper021::m_irqPrescalerPhase = 0;
bool     CROMMapper021::m_irqEnabled = false;

CROMMapper021::CROMMapper021()
{
}

CROMMapper021::~CROMMapper021()
{
}

void CROMMapper021::RESET ( bool soft )
{
   int32_t idx;

   m_mapper = 21;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqPrescaler = 0;
   m_irqPrescalerPhase = 0;
   m_irqEnabled = false;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper021::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper021::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
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
            C6502::ASSERTIRQ(eNESSource_Mapper);
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
               C6502::ASSERTIRQ(eNESSource_Mapper);
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
   case 0x8004:
   case 0x8006:
      return m_reg[0];
      break;
   case 0x9000:
   case 0x9002:
      return m_reg[1];
      break;
   case 0x9004:
   case 0x9006:
      return m_reg[2];
      break;
   case 0xA000:
   case 0xA002:
   case 0xA004:
   case 0xA006:
      return m_reg[3];
      break;
   case 0xB000:
      return m_reg[4];
      break;
   case 0xB002:
      return m_reg[5];
      break;
   case 0xB004:
      return m_reg[6];
      break;
   case 0xB006:
      return m_reg[7];
      break;
   case 0xC000:
      return m_reg[8];
      break;
   case 0xC002:
      return m_reg[9];
      break;
   case 0xC004:
      return m_reg[10];
      break;
   case 0xC006:
      return m_reg[11];
      break;
   case 0xD000:
      return m_reg[12];
      break;
   case 0xD002:
      return m_reg[13];
      break;
   case 0xD004:
      return m_reg[14];
      break;
   case 0xD006:
      return m_reg[15];
      break;
   case 0xE000:
      return m_reg[16];
      break;
   case 0xE002:
      return m_reg[17];
      break;
   case 0xE004:
      return m_reg[18];
      break;
   case 0xE006:
      return m_reg[19];
      break;
   case 0xF000:
      return m_reg[20];
      break;
   case 0xF002:
      return m_reg[21];
      break;
   case 0xF004:
      return m_reg[22];
      break;
   case 0xF006:
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
   case 0x8004:
   case 0x8006:
      reg = 0;
      m_reg[0] = data;
      if ( m_reg[2]&0x02 )
      {
         m_pPRGROMmemory[0] = m_PRGROMmemory[m_numPrgBanks-2];
         m_pPRGROMmemory[2] = m_PRGROMmemory[data&0x1F];
      }
      else
      {
         m_pPRGROMmemory[0] = m_PRGROMmemory[data&0x1F];
         m_pPRGROMmemory[2] = m_PRGROMmemory[m_numPrgBanks-2];
      }
      break;
   case 0x9000:
   case 0x9002:
      reg = 1;
      m_reg[1] = data;
      switch ( data )
      {
      case 0:
         CPPU::MIRRORVERT();
         break;
      case 1:
         CPPU::MIRRORHORIZ();
         break;
      case 2:
         CPPU::MIRROR(0,0,0,0);
         break;
      case 3:
         CPPU::MIRROR(1,1,1,1);
         break;
      }
      break;
   case 0x9004:
   case 0x9006:
      reg = 2;
      m_reg[2] = data;
      if ( m_reg[2]&0x02 )
      {
         m_pPRGROMmemory[0] = m_PRGROMmemory[m_numPrgBanks-2];
      }
      else
      {
         m_pPRGROMmemory[2] = m_PRGROMmemory[m_numPrgBanks-2];
      }
      break;
   case 0xA000:
   case 0xA002:
   case 0xA004:
   case 0xA006:
      reg = 3;
      m_reg[3] = data;
      m_pPRGROMmemory[1] = m_PRGROMmemory[data&0x1F];
      break;
   case 0xB000:
      reg = 4;
      m_reg[4] = data;
      m_chr[0] &= 0xF0;
      m_chr[0] |= (data&0x0F);
      m_pCHRmemory[0] = m_CHRmemory[m_chr[0]];
      break;
   case 0xB002:
      reg = 5;
      m_reg[5] = data;
      m_chr[0] &= 0x0F;
      m_chr[0] |= (data<<4);
      m_pCHRmemory[0] = m_CHRmemory[m_chr[0]];
      break;
   case 0xB004:
      reg = 6;
      m_reg[6] = data;
      m_chr[1] &= 0xF0;
      m_chr[1] |= (data&0x0F);
      m_pCHRmemory[1] = m_CHRmemory[m_chr[1]];
      break;
   case 0xB006:
      reg = 7;
      m_reg[7] = data;
      m_chr[1] &= 0x0F;
      m_chr[1] |= (data<<4);
      m_pCHRmemory[1] = m_CHRmemory[m_chr[1]];
      break;
   case 0xC000:
      reg = 8;
      m_reg[8] = data;
      m_chr[2] &= 0xF0;
      m_chr[2] |= (data&0x0F);
      m_pCHRmemory[2] = m_CHRmemory[m_chr[2]];
      break;
   case 0xC002:
      reg = 9;
      m_reg[9] = data;
      m_chr[2] &= 0x0F;
      m_chr[2] |= (data<<4);
      m_pCHRmemory[2] = m_CHRmemory[m_chr[2]];
      break;
   case 0xC004:
      reg = 10;
      m_reg[10] = data;
      m_chr[3] &= 0xF0;
      m_chr[3] |= (data&0x0F);
      m_pCHRmemory[3] = m_CHRmemory[m_chr[3]];
      break;
   case 0xC006:
      reg = 11;
      m_reg[11] = data;
      m_chr[3] &= 0x0F;
      m_chr[3] |= (data<<4);
      m_pCHRmemory[3] = m_CHRmemory[m_chr[3]];
      break;
   case 0xD000:
      reg = 12;
      m_reg[12] = data;
      m_chr[4] &= 0xF0;
      m_chr[4] |= (data&0x0F);
      m_pCHRmemory[4] = m_CHRmemory[m_chr[4]];
      break;
   case 0xD002:
      reg = 13;
      m_reg[13] = data;
      m_chr[4] &= 0x0F;
      m_chr[4] |= (data<<4);
      m_pCHRmemory[4] = m_CHRmemory[m_chr[4]];
      break;
   case 0xD004:
      reg = 14;
      m_reg[14] = data;
      m_chr[5] &= 0xF0;
      m_chr[5] |= (data&0x0F);
      m_pCHRmemory[5] = m_CHRmemory[m_chr[5]];
      break;
   case 0xD006:
      reg = 15;
      m_reg[15] = data;
      m_chr[5] &= 0x0F;
      m_chr[5] |= (data<<4);
      m_pCHRmemory[5] = m_CHRmemory[m_chr[5]];
      break;
   case 0xE000:
      reg = 16;
      m_reg[16] = data;
      m_chr[6] &= 0xF0;
      m_chr[6] |= (data&0x0F);
      m_pCHRmemory[6] = m_CHRmemory[m_chr[6]];
      break;
   case 0xE002:
      reg = 17;
      m_reg[17] = data;
      m_chr[6] &= 0x0F;
      m_chr[6] |= (data<<4);
      m_pCHRmemory[6] = m_CHRmemory[m_chr[6]];
      break;
   case 0xE004:
      reg = 18;
      m_reg[18] = data;
      m_chr[7] &= 0xF0;
      m_chr[7] |= (data&0x0F);
      m_pCHRmemory[7] = m_CHRmemory[m_chr[7]];
      break;
   case 0xE006:
      reg = 19;
      m_reg[19] = data;
      m_chr[7] &= 0x0F;
      m_chr[7] |= (data<<4);
      m_pCHRmemory[7] = m_CHRmemory[m_chr[7]];
      break;
   case 0xF000:
      reg = 20;
      m_reg[20] = data;
      m_irqReload &= 0xF0;
      m_irqReload |= (data&0x0F);
      break;
   case 0xF002:
      reg = 21;
      m_reg[21] = data;
      m_irqReload &= 0x0F;
      m_irqReload |= (data<<4);
      break;
   case 0xF004:
      reg = 22;
      m_reg[22] = data;
      C6502::RELEASEIRQ(eNESSource_Mapper);
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
      reg = 23;
      m_reg[23] = data;
      C6502::RELEASEIRQ(eNESSource_Mapper);
      if ( m_reg[22]&0x01 )
      {
         m_reg[22] |= 0x02;
         m_reg[22] &= 0xFE;
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
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
