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

#include "cnesrommapper065.h"

#include "cnes6502.h"
#include "cnesppu.h"

// Mapper 068 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl9001Bitfields [] =
{
   new CBitfieldData("Mirroring", 7, 1, "%X", 2, "Vertical", "Horizontal")
};

static CBitfieldData* tbl9003Bitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tbl9004Bitfields [] =
{
   new CBitfieldData("Reload Trigger", 0, 8, "%X", 0)
};

static CBitfieldData* tbl9005Bitfields [] =
{
   new CBitfieldData("IRQ Reload MSB", 0, 8, "%X", 0)
};

static CBitfieldData* tbl9006Bitfields [] =
{
   new CBitfieldData("IRQ Reload LSB", 0, 8, "%X", 0)
};

static CBitfieldData* tblB000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Reg 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9001, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tbl9001Bitfields),
   new CRegisterData(0x9003, "IRQ Enable", nesMapperHighRead, nesMapperHighWrite, 1, tbl9003Bitfields),
   new CRegisterData(0x9004, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tbl9004Bitfields),
   new CRegisterData(0x9005, "IRQ Reload MSB", nesMapperHighRead, nesMapperHighWrite, 1, tbl9005Bitfields),
   new CRegisterData(0x9006, "IRQ Reload LSB", nesMapperHighRead, nesMapperHighWrite, 1, tbl9006Bitfields),
   new CRegisterData(0xA000, "PRG Reg 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Reg 0", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB001, "CHR Reg 1", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB002, "CHR Reg 2", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB003, "CHR Reg 3", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB004, "CHR Reg 4", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB005, "CHR Reg 5", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB006, "CHR Reg 6", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB007, "CHR Reg 7", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xC000, "PRG Reg 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
};

static const char* rowHeadings [] =
{
 ""
};

static const char* columnHeadings [] =
{
   "8000","9001","9003","9004","9005","9006","A000","B000","B001","B002","B003","B004","B005","B006","B007","C000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,16,16,tblRegisters,rowHeadings,columnHeadings);

CROMMapper065::CROMMapper065()
   : CROM(65)
{
   // Irem H-3001 stuff
   memset(m_reg,0,sizeof(m_reg));
   m_irqCounter = 0x00;
   m_irqEnable = false;
   m_irqReload = 0;
}

CROMMapper065::~CROMMapper065()
{
}

void CROMMapper065::RESET ( bool soft )
{
   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_irqCounter = 0x00;
   m_irqEnable = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper065::SYNCCPU ( void )
{
   if ( m_irqEnable )
   {
      if ( m_irqCounter == 0 )
      {
         m_irqEnable = false;
         CNES::NES()->CPU()->ASSERTIRQ ( eNESSource_Mapper );

         if ( nesIsDebuggable )
         {
            // Check for IRQ breakpoint...
            CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
         }
      }
      else
      {
         m_irqCounter--;
      }
   }
}

uint32_t CROMMapper065::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
      case 0x8000:
         return m_reg [ 0 ];
         break;
      case 0x9001:
         return m_reg [ 1 ];
         break;
      case 0x9003:
         return m_reg [ 2 ];
         break;
      case 0x9004:
         return m_reg [ 3 ];
         break;
      case 0x9005:
         return m_reg [ 4 ];
         break;
      case 0x9006:
         return m_reg [ 5 ];
         break;
      case 0xA000:
         return m_reg [ 6 ];
         break;
      case 0xB000:
         return m_reg [ 7 ];
         break;
      case 0xB001:
         return m_reg [ 8 ];
         break;
      case 0xB002:
         return m_reg [ 9 ];
         break;
      case 0xB003:
         return m_reg [ 10 ];
         break;
      case 0xB004:
         return m_reg [ 11 ];
         break;
      case 0xB005:
         return m_reg [ 12 ];
         break;
      case 0xB006:
         return m_reg [ 13 ];
         break;
      case 0xB007:
         return m_reg [ 14 ];
         break;
      case 0xC000:
         return m_reg [ 15 ];
         break;
   }

   return 0xFF;
}

void CROMMapper065::HMAPPER ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case 0x8000:
         m_reg [ 0 ] = data;
         m_PRGROMmemory.REMAP(0,data);
         break;
      case 0x9001:
         m_reg [ 1 ] = data;
         if ( data&0x80 )
         {
            CNES::NES()->PPU()->MIRRORHORIZ();
         }
         else
         {
            CNES::NES()->PPU()->MIRRORVERT();
         }
         break;
      case 0x9003:
         m_reg [ 2 ] = data;
         m_irqEnable = (data&0x80);
         break;
      case 0x9004:
         m_reg [ 3 ] = data;
         m_irqCounter = m_irqReload;
         break;
      case 0x9005:
         m_reg [ 4 ] = data;
         m_irqReload &= 0x00FF;
         m_irqReload |= (data<<8);
         break;
      case 0x9006:
         m_reg [ 5 ] = data;
         m_irqReload &= 0xFF00;
         m_irqReload |= data;
         break;
      case 0xA000:
         m_reg [ 6 ] = data;
         m_PRGROMmemory.REMAP(1,data);
         break;
      case 0xB000:
         m_reg [ 7 ] = data;
         m_CHRmemory.REMAP(0,data);
         break;
      case 0xB001:
         m_reg [ 8 ] = data;
         m_CHRmemory.REMAP(1,data);
         break;
      case 0xB002:
         m_reg [ 9 ] = data;
         m_CHRmemory.REMAP(2,data);
         break;
      case 0xB003:
         m_reg [ 10 ] = data;
         m_CHRmemory.REMAP(3,data);
         break;
      case 0xB004:
         m_reg [ 11 ] = data;
         m_CHRmemory.REMAP(4,data);
         break;
      case 0xB005:
         m_reg [ 12 ] = data;
         m_CHRmemory.REMAP(5,data);
         break;
      case 0xB006:
         m_reg [ 13 ] = data;
         m_CHRmemory.REMAP(6,data);
         break;
      case 0xB007:
         m_reg [ 14 ] = data;
         m_CHRmemory.REMAP(7,data);
         break;
      case 0xC000:
         m_reg [ 15 ] = data;
         m_PRGROMmemory.REMAP(2,data);
         break;
   }
}
