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

#include <stdint.h>

#include "cnesrommapper068.h"

#include "cnesppu.h"

// Sunsoft Mapper #4 stuff
uint8_t  CROMMapper068::m_reg [] = { 0x00, 0x00, 0x00, 0x00 };

CROMMapper068::CROMMapper068()
{

}

CROMMapper068::~CROMMapper068()
{

}

void CROMMapper068::RESET ()
{
   int32_t idx;

   m_mapper = 68;

   CROM::RESET ( m_mapper );

   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = 0x00;
   }

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

void CROMMapper068::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper068::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

uint32_t CROMMapper068::MAPPER ( uint32_t addr )
{
   // TODO: Implement mapper 68 fully.
   return 0xFF;
}

void CROMMapper068::MAPPER ( uint32_t addr, uint8_t data )
{
   switch ( addr&0xF000 )
   {
      case 0x8000:
         m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (data>>2) ] + (((data<<1)&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (data>>2) ] + ((((data<<1)&0x7)+1)<<UPSHIFT_1KB);
         break;
      case 0x9000:
         m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (data>>2) ] + (((data<<1)&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (data>>2) ] + ((((data<<1)&0x7)+1)<<UPSHIFT_1KB);
         break;
      case 0xA000:
         m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (data>>2) ] + (((data<<1)&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (data>>2) ] + ((((data<<1)&0x7)+1)<<UPSHIFT_1KB);
         break;
      case 0xB000:
         m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (data>>2) ] + (((data<<1)&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (data>>2) ] + ((((data<<1)&0x7)+1)<<UPSHIFT_1KB);
         break;
      case 0xC000:
         m_reg [ 2 ] = data|0x80;

         if ( m_reg[0] )
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 1:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 2:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 3:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
            }
         }
         else
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::MIRRORVERT ();
                  break;
               case 1:
                  CPPU::MIRRORHORIZ ();
                  break;
               case 2:
                  CPPU::MIRROR ( 0 );
                  break;
               case 3:
                  CPPU::MIRROR ( 1 );
                  break;
            }
         }

         break;
      case 0xD000:
         m_reg [ 3 ] = data|0x80;

         if ( m_reg[0] )
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 1:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 2:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 3:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
            }
         }
         else
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::MIRRORVERT ();
                  break;
               case 1:
                  CPPU::MIRRORHORIZ ();
                  break;
               case 2:
                  CPPU::MIRROR ( 0 );
                  break;
               case 3:
                  CPPU::MIRROR ( 1 );
                  break;
            }
         }

         break;
      case 0xE000:
         m_reg [ 0 ] = (data>>4)&0x01;
         m_reg [ 1 ] = data&0x03;

         if ( m_reg[0] )
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 1:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 2:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[2]>>3) ] + (((m_reg[2])&0x7)<<UPSHIFT_1KB) );
                  break;
               case 3:
                  CPPU::Move1KBank ( 0x8, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0x9, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xA, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  CPPU::Move1KBank ( 0xB, m_CHRROMmemory [ (m_reg[3]>>3) ] + (((m_reg[3])&0x7)<<UPSHIFT_1KB) );
                  break;
            }
         }
         else
         {
            switch ( m_reg[1] )
            {
               case 0:
                  CPPU::MIRRORVERT ();
                  break;
               case 1:
                  CPPU::MIRRORHORIZ ();
                  break;
               case 2:
                  CPPU::MIRROR ( 0 );
                  break;
               case 3:
                  CPPU::MIRROR ( 1 );
                  break;
            }
         }

         break;
      case 0xF000:
         m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
         m_PRGROMbank [ 0 ] = data;
         m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ data+1 ];
         m_PRGROMbank [ 1 ] = data+1;
         break;
   }
}
