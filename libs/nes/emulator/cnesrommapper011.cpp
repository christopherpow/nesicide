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

#include "cnesrommapper011.h"
#include "cnesppu.h"

// Mapper 011 Registers
static CBitfieldData* tbl8000_FFFFBitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 2, "%X", 0),
   new CBitfieldData("Lockout defeat", 2, 2, "%X", 0),
   new CBitfieldData("CHR Bank", 4, 4, "%X", 0),
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", nesMapperHighRead, nesMapperHighWrite, 3, tbl8000_FFFFBitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,1,1,tblRegisters,rowHeadings,columnHeadings);

CROMMapper011::CROMMapper011()
   : CROM(11)
{
   m_reg = 0x00;
}

CROMMapper011::~CROMMapper011()
{
}

void CROMMapper011::RESET ( bool soft )
{
   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper011::DEBUGINFO ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper011::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   m_reg = data;

   bank = ((data&0x03)<<2);

   m_PRGROMmemory.REMAP(0,bank);
   m_PRGROMmemory.REMAP(1,bank+1);
   m_PRGROMmemory.REMAP(2,bank+2);
   m_PRGROMmemory.REMAP(3,bank+3);

   m_CHRmemory.REMAP(0,((data>>1)&0x78)+0);
   m_CHRmemory.REMAP(1,((data>>1)&0x78)+1);
   m_CHRmemory.REMAP(2,((data>>1)&0x78)+2);
   m_CHRmemory.REMAP(3,((data>>1)&0x78)+3);
   m_CHRmemory.REMAP(4,((data>>1)&0x78)+4);
   m_CHRmemory.REMAP(5,((data>>1)&0x78)+5);
   m_CHRmemory.REMAP(6,((data>>1)&0x78)+6);
   m_CHRmemory.REMAP(7,((data>>1)&0x78)+7);

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}
