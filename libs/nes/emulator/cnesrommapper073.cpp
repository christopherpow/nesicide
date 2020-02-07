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

#include "cnesrommapper073.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 073 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 0:3", 0, 4, "%X", 0)
};

static CBitfieldData* tbl9000Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 4:7", 0, 4, "%X", 0)
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 8:11", 0, 4, "%X", 0)
};

static CBitfieldData* tblB000Bitfields [] =
{
   new CBitfieldData("IRQ Reload bits 12:15", 0, 4, "%X", 0)
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("IRQ Enable after ack", 0, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Enable", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Mode", 2, 1, "%X", 2, "16-bit counter", "8-bit counter")
};

static CBitfieldData* tblD000Bitfields [] =
{
   new CBitfieldData("IRQ Acknowledge", 0, 8, "%X", 0)
};

static CBitfieldData* tblF000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tbl9000Bitfields),
   new CRegisterData(0xA000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xB000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC000, "IRQ Control", nesMapperHighRead, nesMapperHighWrite, 3, tblC000Bitfields),
   new CRegisterData(0xD000, "IRQ Acknowledge", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xF000, "PRG Control", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","9000","A000","B000","C000","D000","F000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,7,7,tblRegisters,rowHeadings,columnHeadings);

CROMMapper073::CROMMapper073()
   : CROM(73)
{
   memset(m_reg,0,sizeof(m_reg));
   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqEnabled = false;
}

CROMMapper073::~CROMMapper073()
{
}

void CROMMapper073::RESET ( bool soft )
{
   m_mapper = 73;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqEnabled = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper073::SYNCCPU ( void )
{
   uint16_t counterMask;
   uint16_t counter;

   if ( m_irqEnabled )
   {
      // Get relevant counter bits.
      if ( (m_reg[4]&0x04) == 0 )
      {
         // 16 bits
         counterMask = 0xFFFF;
      }
      else
      {
         // 8 bits
         counterMask = 0x00FF;
      }

      counter = m_irqCounter&counterMask;
      counter++;
      m_irqCounter &= (~counterMask);
      m_irqCounter |= (counter&counterMask);

      if ( (m_irqCounter&counterMask) == 0 )
      {
         // Reload!
         m_irqCounter &= (~counterMask);
         m_irqCounter |= m_irqReload&counterMask;

         CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

         if ( nesIsDebuggable )
         {
            // Check for IRQ breakpoint...
            CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
         }
      }
   }
}

uint32_t CROMMapper073::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x8000:
      return m_reg[0];
      break;
   case 0x9000:
      return m_reg[1];
      break;
   case 0xA000:
      return m_reg[2];
      break;
   case 0xB000:
      return m_reg[3];
      break;
   case 0xC000:
      return m_reg[4];
      break;
   case 0xD000:
      return m_reg[5];
      break;
   case 0xF000:
      return m_reg[6];
      break;
   }
   return 0xA1; // garbage
}

void CROMMapper073::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr )
   {
   case 0x8000:
      reg = 0;
      m_reg[0] = data;
      m_irqReload &= 0xFFF0;
      m_irqReload |= (data&0x0F);
      break;
   case 0x9000:
      reg = 1;
      m_reg[1] = data;
      m_irqReload &= 0xFF0F;
      m_irqReload |= ((data&0x0F)<<4);
      break;
   case 0xA000:
      reg = 2;
      m_reg[2] = data;
      m_irqReload &= 0xF0FF;
      m_irqReload |= ((data&0x0F)<<8);
      break;
   case 0xB000:
      reg = 3;
      m_reg[3] = data;
      m_irqReload &= 0x0FFF;
      m_irqReload |= ((data&0x0F)<<12);
      break;
   case 0xC000:
      reg = 4;
      m_reg[4] = data;
      m_irqCounter = m_irqReload;
      m_irqEnabled = data&0x02;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0xD000:
      reg = 5;
      m_reg[5] = data;
      m_reg[4] &= 0xFD;
      m_reg[4] |= ((m_reg[4]&0x01)<<1);
      if ( m_reg[4]&0x02 )
      {
         m_irqEnabled = true;
      }
      else
      {
         m_irqEnabled = false;
      }
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0xF000:
      reg = 6;
      m_reg[6] = data;
      m_PRGROMmemory.REMAP(0,((data<<1)&(m_numPrgBanks-1))+0);
      m_PRGROMmemory.REMAP(1,((data<<1)&(m_numPrgBanks-1))+1);
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
