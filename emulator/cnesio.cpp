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
    
// IO.cpp: implementation of the CIO class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesio.h"
#include "cnesapu.h"
#include "cnesppu.h"
#include "cnes6502.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CIO::m_ioJoy [] = { 0x00, 0x00 };
unsigned char  CIO::m_ioJoyLatch [] = { 0x00, 0x00 };

CJoypadLogger  CIO::m_logger [ NUM_JOY ];

CIO::CIO()
{

}

CIO::~CIO()
{

}

UINT CIO::IO ( UINT addr )
{
   UINT data = 0xFF;

   switch ( addr )
   {
      case APUCTRL:
         data = CAPU::APU ( addr );
      break;

      case IOSPRITEDMA:
         data = CPPU::PPU ( addr );
      break;

      case IOJOY1:
         data = 0x40|((*(m_ioJoyLatch+JOY1))&0x01);
         *(m_ioJoyLatch+JOY1) >>= 1;
      break;

      case IOJOY2:
         data = 0x40|((*(m_ioJoyLatch+JOY2))&0x01);
         *(m_ioJoyLatch+JOY2) >>= 1;
      break;
   }

   return data;
}

void CIO::IO ( UINT addr, unsigned char data )
{
   switch ( addr )
   {
      case IOJOY1:
         if ( (data&1) == 0 ) // latch on negative edge
         {
            *(m_ioJoyLatch+JOY1) = *(m_ioJoy+JOY1);
            *(m_ioJoyLatch+JOY2) = *(m_ioJoy+JOY2);
         }
      break;

      case IOSPRITEDMA:
         CPPU::PPU ( addr, data );
      break;

      default:
         CAPU::APU ( addr, data );
      break;
   }
}

UINT CIO::_IO ( UINT addr )
{
   UINT data = 0xFF;

   switch ( addr )
   {
      case IOSPRITEDMA:
         data = CPPU::PPU ( addr );
      break;

      case IOJOY1:
         data = 0x40|(m_ioJoyLatch[JOY1]&0x01);
      break;

      case IOJOY2:
         data = 0x40|(m_ioJoyLatch[JOY2]&0x01);
      break;

      default:
         data = CAPU::_APU ( addr );
      break;
   }

   return data;
}

void CIO::_IO ( UINT addr, unsigned char data )
{
   switch ( addr )
   {
      case IOJOY1:
         if ( (data&1) == 0 ) // latch on negative edge
         {
            *(m_ioJoyLatch+JOY1) = *(m_ioJoy+JOY1);
            *(m_ioJoyLatch+JOY2) = *(m_ioJoy+JOY2);
         }
      break;

      case IOSPRITEDMA:
         CPPU::PPU ( addr, data );
      break;

      default:
         CAPU::APU ( addr, data );
      break;
   }
}
