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
    
// ROMMapper007.cpp: implementation of the CROMMapper007 class.
//
//////////////////////////////////////////////////////////////////////

// CPTODO: include for ?
//#include "nesicide.h"
#include "ROMMapper007.h"

#include "PPU.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CROMMapper007::m_reg = 0x00;

CROMMapper007::CROMMapper007()
{

}

CROMMapper007::~CROMMapper007()
{

}

void CROMMapper007::RESET ()
{
   CROM::RESET ();

   m_mapper = 7;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 0 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = 0;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = 1;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = 1;

   CPPU::MIRROR ( 0 );

   // CHR ROM/RAM already set up in CROM::RESET()...
}

UINT CROMMapper007::MAPPER ( UINT addr )
{
   return m_reg;
}

void CROMMapper007::MAPPER ( UINT addr, unsigned char data )
{
   m_reg = data;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (m_reg&0xF)<<1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = (m_reg&0xF)<<1;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (m_reg&0xF)<<1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = (m_reg&0xF)<<1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ ((m_reg&0xF)<<1)+1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = ((m_reg&0xF)<<1)+1;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ ((m_reg&0xF)<<1)+1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = ((m_reg&0xF)<<1)+1;

   CPPU::MIRROR ( (m_reg&0x10)>>4 );
}

void CROMMapper007::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper007::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

void CROMMapper007::DISPLAY ( char* sz )
{
   static const char* fmt = "8000-FFFF:%02X\r\n";
   int bytes;
   
   bytes = sprintf ( sz, fmt, m_reg );

   CROM::DISPLAY ( sz+bytes );
}

