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

#include "cnesrommapper003.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 003 Registers
static CBitfieldData* tbl8000_FFFFBitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "CHR Mapping", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000_FFFFBitfields)
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

CROMMapper003::CROMMapper003()
   : CROM(3)
{
   m_reg = 0x00;
   m_prgRemappable = false;
   m_chrRemappable = true;
}

CROMMapper003::~CROMMapper003()
{
}

void CROMMapper003::RESET ( bool soft )
{
   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper003::DEBUGINFO ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper003::HMAPPER ( uint32_t addr, uint8_t data )
{
   m_reg = data&0x3; // avoid latch diode protection bits
                     // https://wiki.nesdev.com/w/index.php/CNROM

   m_CHRmemory.REMAP(0,(m_reg<<3)+0);
   m_CHRmemory.REMAP(1,(m_reg<<3)+1);
   m_CHRmemory.REMAP(2,(m_reg<<3)+2);
   m_CHRmemory.REMAP(3,(m_reg<<3)+3);
   m_CHRmemory.REMAP(4,(m_reg<<3)+4);
   m_CHRmemory.REMAP(5,(m_reg<<3)+5);
   m_CHRmemory.REMAP(6,(m_reg<<3)+6);
   m_CHRmemory.REMAP(7,(m_reg<<3)+7);

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}
