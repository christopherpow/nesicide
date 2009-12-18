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
    
// ROMMapper003.cpp: implementation of the CROMMapper003 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrommapper003.h"

#include "cnesppu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CROMMapper003::m_reg = 0x00;

CROMMapper003::CROMMapper003()
{

}

CROMMapper003::~CROMMapper003()
{

}

void CROMMapper003::RESET ()
{
   CROM::RESET ();

   m_mapper = 3;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 0 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = 0;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = 1;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = 1;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

UINT CROMMapper003::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper003::MAPPER ( UINT addr, unsigned char data )
{
   m_reg = data;

   m_pCHRmemory [ 0 ] = m_CHRROMmemory [ m_reg ] + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 1 ] = m_CHRROMmemory [ m_reg ] + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 2 ] = m_CHRROMmemory [ m_reg ] + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 3 ] = m_CHRROMmemory [ m_reg ] + (3<<UPSHIFT_1KB);
   m_pCHRmemory [ 4 ] = m_CHRROMmemory [ m_reg ] + (4<<UPSHIFT_1KB);
   m_pCHRmemory [ 5 ] = m_CHRROMmemory [ m_reg ] + (5<<UPSHIFT_1KB);
   m_pCHRmemory [ 6 ] = m_CHRROMmemory [ m_reg ] + (6<<UPSHIFT_1KB);
   m_pCHRmemory [ 7 ] = m_CHRROMmemory [ m_reg ] + (7<<UPSHIFT_1KB);
}

void CROMMapper003::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper003::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

void CROMMapper003::DISPLAY ( char* sz )
{
   CROM::DISPLAY ( sz );
}
