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

uint8_t  CROMMapper003::m_reg = 0x00;

CROMMapper003::CROMMapper003()
{
}

CROMMapper003::~CROMMapper003()
{
}

void CROMMapper003::RESET ()
{
   m_mapper = 3;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper003::MAPPER ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper003::MAPPER ( uint32_t addr, uint8_t data )
{
   m_reg = data;

   m_pCHRmemory [ 0 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 1 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 2 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 3 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (3<<UPSHIFT_1KB);
   m_pCHRmemory [ 4 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (4<<UPSHIFT_1KB);
   m_pCHRmemory [ 5 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (5<<UPSHIFT_1KB);
   m_pCHRmemory [ 6 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (6<<UPSHIFT_1KB);
   m_pCHRmemory [ 7 ] = m_CHRROMmemory [ m_reg%m_numChrBanks ] + (7<<UPSHIFT_1KB);

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}

void CROMMapper003::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper003::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}
