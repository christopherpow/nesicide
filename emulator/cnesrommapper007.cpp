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

unsigned char  CROMMapper007::m_reg = 0x00;

CROMMapper007::CROMMapper007()
{

}

CROMMapper007::~CROMMapper007()
{

}

void CROMMapper007::RESET ()
{
   m_mapper = 7;

   CROM::RESET ( m_mapper );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_PRGROMbank [ 1 ] = 1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
   m_PRGROMbank [ 2 ] = 2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 3 ];
   m_PRGROMbank [ 3 ] = 3;

   CPPU::MIRROR ( 0 );

   // CHR ROM/RAM already set up in CROM::RESET()...
}

UINT CROMMapper007::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper007::MAPPER ( UINT addr, unsigned char data )
{
   unsigned char bank;

   m_reg = data;

   bank = (m_reg&0xF)<<2;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_PRGROMbank [ 0 ] = bank;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_PRGROMbank [ 1 ] = bank+1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
   m_PRGROMbank [ 2 ] = bank+2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];
   m_PRGROMbank [ 3 ] = bank+3;

   CPPU::MIRROR ( (m_reg&0x10)>>4 );

   // Check mapper state breakpoints...
   CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
}

void CROMMapper007::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper007::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

