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
    
#include "cnesrommapper002.h"

#include "cregisterdata.h"

// Mapper 002 Registers
static CBitfieldData* tbl8000_FFFFBitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Mapping", 1, tbl8000_FFFFBitfields),
};

unsigned char  CROMMapper002::m_reg = 0x00;

CROMMapper002::CROMMapper002()
{

}

CROMMapper002::~CROMMapper002()
{

}

void CROMMapper002::RESET ()
{
   m_mapper = 2;

   CROM::RESET ( m_mapper );

   m_tblRegisters = tblRegisters;
   m_numRegisters = sizeof(tblRegisters)/sizeof(tblRegisters[0]);

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_PRGROMbank [ 1 ] = 1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_PRGROMbank [ 2 ] = m_numPrgBanks-2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 3 ] = m_numPrgBanks-1;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

UINT CROMMapper002::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper002::MAPPER ( UINT addr, unsigned char data )
{
   unsigned char bank;

   m_reg = data%m_numPrgBanks;

   bank = m_reg<<1;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_PRGROMbank [ 0 ] = bank;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_PRGROMbank [ 1 ] = bank+1;

   // Check mapper state breakpoints...
   CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
}

void CROMMapper002::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper002::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}
