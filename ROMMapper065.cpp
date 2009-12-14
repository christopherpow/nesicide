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
    
// ROMMapper065.cpp: implementation of the CROMMapper065 class.
//
//////////////////////////////////////////////////////////////////////

#include "ROMMapper065.h"

#include "PPU.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Irem H-3001 stuff
unsigned char  CROMMapper065::m_reg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  CROMMapper065::m_irqCounter = 0x00;
bool           CROMMapper065::m_irqEnable = false;

CROMMapper065::CROMMapper065()
{

}

CROMMapper065::~CROMMapper065()
{

}

void CROMMapper065::RESET ()
{
   CROM::RESET ();

   m_mapper = 65;

   m_irqCounter = 0x00;
   m_irqEnable = false;
   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 0 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = 0;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = m_numPrgBanks-1;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = m_numPrgBanks-1;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

bool CROMMapper065::SYNCH ( int scanline )
{
   bool fire = false;

   if ( m_irqEnable )
   {
      if ( m_irqCounter == 0 )
      {
         m_irqEnable = false;
         fire = true;
      }
      else
      {
         m_irqCounter--;
      }
   }

   return fire;
}

void CROMMapper065::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper065::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

void CROMMapper065::DISPLAY ( char* sz )
{
   CROM::DISPLAY ( sz );
}

UINT CROMMapper065::MAPPER ( UINT addr )
{
   switch ( addr )
   {
      case 0x8000:
         return m_reg [ 0 ];
      break;
      case 0x9000:
         return m_reg [ 1 ];
      break;
      case 0x9001:
         return m_reg [ 2 ];
      break;
      case 0x9003:
         return m_reg [ 3 ];
      break;
      case 0x9004:
         return m_reg [ 4 ];
      break;
      case 0x9005:
         return m_reg [ 5 ];
      break;
      case 0x9006:
         return m_reg [ 6 ];
      break;
      case 0xA000:
         return m_reg [ 7 ];
      break;
      case 0xB000:
         return m_reg [ 8 ];
      break;
      case 0xB001:
         return m_reg [ 9 ];
      break;
      case 0xB002:
         return m_reg [ 10 ];
      break;
      case 0xB003:
         return m_reg [ 11 ];
      break;
      case 0xB004:
         return m_reg [ 12 ];
      break;
      case 0xB005:
         return m_reg [ 13 ];
      break;
      case 0xB006:
         return m_reg [ 14 ];
      break;
      case 0xB007:
         return m_reg [ 15 ];
      break;
      case 0xC000:
         return m_reg [ 16 ];
      break;
   }
   return 0xFF;
}

void CROMMapper065::MAPPER ( UINT addr, unsigned char data )
{
   switch ( addr )
   {
      case 0x8000:
         m_reg [ 0 ] = data;
         m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (data>>1)%m_numPrgBanks ] + (0<<UPSHIFT_8KB);
         m_PRGROMbank [ 0 ] = (data>>1)%m_numPrgBanks;
      break;
      case 0x9000:
         m_reg [ 1 ] = data;
      break;
      case 0x9001:
         m_reg [ 2 ] = data;
      break;
      case 0x9003:
         m_reg [ 3 ] = data;
      break;
      case 0x9004:
         m_reg [ 4 ] = data;
      break;
      case 0x9005:
         m_reg [ 5 ] = data;
      break;
      case 0x9006:
         m_reg [ 6 ] = data;
      break;
      case 0xA000:
         m_reg [ 7 ] = data;
         m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (data>>1)%m_numPrgBanks ] + (1<<UPSHIFT_8KB);
         m_PRGROMbank [ 1 ] = (data>>1)%m_numPrgBanks;
      break;
      case 0xB000:
         m_reg [ 8 ] = data;
         m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB001:
         m_reg [ 9 ] = data;
         m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB002:
         m_reg [ 10 ] = data;
         m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB003:
         m_reg [ 11 ] = data;
         m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB004:
         m_reg [ 12 ] = data;
         m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB005:
         m_reg [ 13 ] = data;
         m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB006:
         m_reg [ 14 ] = data;
         m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xB007:
         m_reg [ 15 ] = data;
         m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (data>>3)%m_numChrBanks ] + ((data&0x7)<<UPSHIFT_1KB);
      break;
      case 0xC000:
         m_reg [ 16 ] = data;
         m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ (data>>1)%m_numPrgBanks ] + (0<<UPSHIFT_8KB);
         m_PRGROMbank [ 2 ] = (data>>1)%m_numPrgBanks;
      break;
   }
}
