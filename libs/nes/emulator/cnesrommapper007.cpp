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

#include "cnesrommapper007.h"

#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 007 Registers
static CBitfieldData* tbl8000_FFFFBitfields [] =
{
   new CBitfieldData("Mirroring", 4, 1, "%X", 2, "One-screen at 2000", "One-screen at 2400"),
   new CBitfieldData("PRG Bank", 0, 3, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", nesMapperHighRead, nesMapperHighWrite, 2, tbl8000_FFFFBitfields),
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

uint8_t  CROMMapper007::m_reg = 0x00;

CROMMapper007::CROMMapper007()
{
}

CROMMapper007::~CROMMapper007()
{
}

void CROMMapper007::RESET ( bool soft )
{
   m_mapper = 7;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   CPPU::MIRROR ( 0 );

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper007::MAPPER ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper007::MAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   m_reg = data;

   bank = (m_reg&0xF)<<2;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];

   CPPU::MIRROR ( (m_reg&0x10)>>4 );

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}

void CROMMapper007::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper007::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

