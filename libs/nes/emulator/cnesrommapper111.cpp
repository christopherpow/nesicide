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

#include "cnesrommapper111.h"

#include "cregisterdata.h"

// Mapper 111 Registers
static CBitfieldData* tbl5000_5FFFBitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0),
   new CBitfieldData("CHR Bank", 4, 1, "%X", 0),
   new CBitfieldData("Nametable Bank", 5, 1, "%X", 0),
   new CBitfieldData("Red LED", 6, 1, "%X", 2, "On", "Off"),
   new CBitfieldData("Green LED", 7, 1, "%X", 2, "On", "Off")
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x5000, "Bank Select", nesMapperHighRead, nesMapperHighWrite, 5, tbl5000_5FFFBitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "5000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,1,1,tblRegisters,rowHeadings,columnHeadings);


CROMMapper111::CROMMapper111()
   : CROM(111)
{
   m_prgRemappable = true;
   m_chrRemappable = true;
   m_reg = 0x00;
}

CROMMapper111::~CROMMapper111()
{
}

void CROMMapper111::RESET ( bool soft )
{
   m_mapper = 111;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper111::DEBUGINFO ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper111::LMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   m_reg = data;

   bank = (m_reg&0x0f)<<2;

   m_PRGROMmemory.REMAP(0,bank+0);
   m_PRGROMmemory.REMAP(1,bank+1);
   m_PRGROMmemory.REMAP(2,bank+2);
   m_PRGROMmemory.REMAP(3,bank+3);

   bank = (m_reg&0x10)>>1;

   m_CHRmemory.REMAP(0,bank+0);
   m_CHRmemory.REMAP(1,bank+1);
   m_CHRmemory.REMAP(2,bank+2);
   m_CHRmemory.REMAP(3,bank+3);
   m_CHRmemory.REMAP(4,bank+4);
   m_CHRmemory.REMAP(5,bank+5);
   m_CHRmemory.REMAP(6,bank+6);
   m_CHRmemory.REMAP(7,bank+7);

   bank = (m_reg&0x20)>>2;

   CNES::NES()->PPU()->VRAM()->REMAP(0,bank+0);
   CNES::NES()->PPU()->VRAM()->REMAP(1,bank+1);
   CNES::NES()->PPU()->VRAM()->REMAP(2,bank+2);
   CNES::NES()->PPU()->VRAM()->REMAP(3,bank+3);
   CNES::NES()->PPU()->VRAM()->REMAP(4,bank+4);
   CNES::NES()->PPU()->VRAM()->REMAP(5,bank+5);
   CNES::NES()->PPU()->VRAM()->REMAP(6,bank+6);
   CNES::NES()->PPU()->VRAM()->REMAP(7,bank+7);

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}
