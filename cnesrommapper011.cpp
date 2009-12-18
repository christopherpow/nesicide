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
    
// ROMMapper011.cpp: implementation of the CROMMapper011 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrommapper011.h"

#include "cnesppu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CROMMapper011::m_reg = 0x00;

CROMMapper011::CROMMapper011()
{

}

CROMMapper011::~CROMMapper011()
{

}

void CROMMapper011::RESET ()
{
   CROM::RESET ();

   m_mapper = 11;

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

void CROMMapper011::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper011::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

void CROMMapper011::DISPLAY ( char* sz )
{
   CROM::DISPLAY ( sz );
}

UINT CROMMapper011::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper011::MAPPER ( UINT addr, unsigned char data )
{
   m_reg = data;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ ((data&0x03)<<1) ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = ((data&0x03)<<1);
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ ((data&0x03)<<1) ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = ((data&0x03)<<1);
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ (((data&0x03)<<1)+1) ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = (((data&0x03)<<1)+1);
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ (((data&0x03)<<1)+1) ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = (((data&0x03)<<1)+1);
   
   m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (0<<UPSHIFT_1KB);
   m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (1<<UPSHIFT_1KB);
   m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (2<<UPSHIFT_1KB);
   m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (3<<UPSHIFT_1KB);
   m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (4<<UPSHIFT_1KB);
   m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (5<<UPSHIFT_1KB);
   m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (6<<UPSHIFT_1KB);
   m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (data>>4)&0x0F ] + (7<<UPSHIFT_1KB);
}
