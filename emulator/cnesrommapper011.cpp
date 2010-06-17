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

#include "cregisterdata.h"

// Mapper 011 Registers
static CBitfieldData* tbl8000_FFFFBitfields [] =
{
   new CBitfieldData("CHR Bank", 4, 4, "%X", 0),
   new CBitfieldData("Lockout", 2, 2, "%X", 0),
   new CBitfieldData("PRG Bank", 0, 2, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG/CHR Mapping", 3, tbl8000_FFFFBitfields)
};

unsigned char  CROMMapper011::m_reg = 0x00;

CROMMapper011::CROMMapper011()
{

}

CROMMapper011::~CROMMapper011()
{

}

void CROMMapper011::RESET ()
{
   m_mapper = 11;

   CROM::RESET ( m_mapper );

   m_tblRegisters = tblRegisters;
   m_numRegisters = sizeof(tblRegisters)/sizeof(tblRegisters[0]);

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_PRGROMbank [ 1 ] = 1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
   m_PRGROMbank [ 2 ] = 2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 3 ];
   m_PRGROMbank [ 3 ] = 3;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper011::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper011::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

UINT CROMMapper011::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper011::MAPPER ( UINT addr, unsigned char data )
{
   unsigned char bank;
   
   m_reg = data;
   
   bank = ((data&0x03)<<2);

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_PRGROMbank [ 0 ] = bank;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_PRGROMbank [ 1 ] = bank+1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
   m_PRGROMbank [ 2 ] = bank+2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];
   m_PRGROMbank [ 3 ] = bank+3;
   
   m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (3<<UPSHIFT_1KB);
   m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (4<<UPSHIFT_1KB);
   m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (5<<UPSHIFT_1KB);
   m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (6<<UPSHIFT_1KB);
   m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (7<<UPSHIFT_1KB);

   // Check mapper state breakpoints...
   CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
}
