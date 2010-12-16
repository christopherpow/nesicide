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

#include "cnesio.h"
#include "cnesapu.h"
#include "cnesppu.h"
#include "cnes6502.h"

uint8_t  CIO::m_ioJoy [] = { 0x00, 0x00 };
uint8_t  CIO::m_ioJoyLatch [] = { 0x00, 0x00 };
uint8_t  CIO::m_last4016 = 0x00;

#if defined ( IDE_BUILD )
CJoypadLogger  CIO::m_logger [ NUM_JOY ];
#endif

CIO::CIO()
{

}

CIO::~CIO()
{

}

uint32_t CIO::IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

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
         *(m_ioJoyLatch+JOY1) |= 0x80;
         break;

      case IOJOY2:
         data = 0x40|((*(m_ioJoyLatch+JOY2))&0x01);
         *(m_ioJoyLatch+JOY2) >>= 1;
         *(m_ioJoyLatch+JOY2) |= 0x80;
         break;
   }

   return data;
}

void CIO::IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+JOY1) = *(m_ioJoy+JOY1);
            *(m_ioJoyLatch+JOY2) = *(m_ioJoy+JOY2);
         }

         m_last4016 = data;
         break;

      case IOSPRITEDMA:
         CPPU::PPU ( addr, data );
         break;

      default:
         CAPU::APU ( addr, data );
         break;
   }
}

uint32_t CIO::_IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

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

void CIO::_IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+JOY1) = *(m_ioJoy+JOY1);
            *(m_ioJoyLatch+JOY2) = *(m_ioJoy+JOY2);
         }

         m_last4016 = data;
         break;

      case IOSPRITEDMA:
         CPPU::PPU ( addr, data );
         break;

      default:
         CAPU::APU ( addr, data );
         break;
   }
}
