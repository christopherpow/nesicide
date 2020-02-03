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

// Default IO implementation stuff
uint32_t   CIO::m_ioJoy [] = { 0x00, 0x00 };

// Standard joypad stuff
uint8_t   CIOStandardJoypad::m_ioJoyLatch [] = { 0x00, 0x00 };
uint8_t   CIOStandardJoypad::m_last4016 = 0x00;
CJoypadLogger  CIOStandardJoypad::m_logger [ NUM_CONTROLLERS ];

// Turbo joypad stuff
uint8_t   CIOTurboJoypad::m_alternator [][2] = { { 0, 0 }, { 0, 0 } };
uint32_t  CIOTurboJoypad::m_lastFrame = 0;

// Vaus Arkanoid pad stuff
uint8_t   CIOVaus::m_ioPotLatch [] = { 0x00, 0x00 };
uint8_t   CIOVaus::m_last4016 = 0x00;
uint8_t   CIOVaus::m_trimPot [] = { 0x54, 0x54 };

uint32_t CIO::IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x00; // return 'unconnected'
         break;

      case IOJOY2:
         data = 0x00; // return 'unconnected'
         break;
   }

   return data;
}

void CIO::IO ( uint32_t addr, uint8_t data )
{
   // No effect.
   return;
}

uint32_t CIO::_IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x00; // return 'unconnected'
         break;

      case IOJOY2:
         data = 0x00; // return 'unconnected'
         break;
   }

   return data;
}

void CIO::_IO ( uint32_t addr, uint8_t data )
{
   // No effect.
   return;
}

uint32_t CIOStandardJoypad::IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x40|((*(m_ioJoyLatch+CONTROLLER1))&0x01);
         *(m_ioJoyLatch+CONTROLLER1) >>= 1;
         *(m_ioJoyLatch+CONTROLLER1) |= 0x80;
         break;

      case IOJOY2:
         data = 0x40|((*(m_ioJoyLatch+CONTROLLER2))&0x01);
         *(m_ioJoyLatch+CONTROLLER2) >>= 1;
         *(m_ioJoyLatch+CONTROLLER2) |= 0x80;
         break;
   }

   return data;
}

void CIOStandardJoypad::IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+CONTROLLER1) = *(m_ioJoy+CONTROLLER1);
            *(m_ioJoyLatch+CONTROLLER2) = *(m_ioJoy+CONTROLLER2);
         }

         m_last4016 = data;
         break;
   }
}

uint32_t CIOStandardJoypad::_IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x40|(m_ioJoyLatch[CONTROLLER1]&0x01);
         break;

      case IOJOY2:
         data = 0x40|(m_ioJoyLatch[CONTROLLER2]&0x01);
         break;
   }

   return data;
}

void CIOStandardJoypad::_IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+CONTROLLER1) = *(m_ioJoy+CONTROLLER1);
            *(m_ioJoyLatch+CONTROLLER2) = *(m_ioJoy+CONTROLLER2);
         }

         m_last4016 = data;
         break;
   }
}

void CIOTurboJoypad::IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+CONTROLLER1) = (uint8_t)(*(m_ioJoy+CONTROLLER1))&0xFF;
            *(m_ioJoyLatch+CONTROLLER2) = (uint8_t)(*(m_ioJoy+CONTROLLER2))&0xFF;

            // Alternate for turbos if necessary.
            if ( m_lastFrame != CNES::NES()->FRAME() )
            {
               m_alternator[CONTROLLER1][0] = !m_alternator[CONTROLLER1][0];
               m_alternator[CONTROLLER1][1] = !m_alternator[CONTROLLER1][1];
               m_alternator[CONTROLLER2][0] = !m_alternator[CONTROLLER2][0];
               m_alternator[CONTROLLER2][1] = !m_alternator[CONTROLLER2][1];
            }
            m_lastFrame = CNES::NES()->FRAME();

            if ( m_ioJoy[CONTROLLER1]&JOY_ATURBO )
            {
               *(m_ioJoyLatch+CONTROLLER1) &= (~JOY_A);
               if ( m_alternator[CONTROLLER1][0] )
               {
                  *(m_ioJoyLatch+CONTROLLER1) |= JOY_A;
               }
            }
            if ( m_ioJoy[CONTROLLER1]&JOY_BTURBO )
            {
               *(m_ioJoyLatch+CONTROLLER1) &= (~JOY_B);
               if ( m_alternator[CONTROLLER1][1] )
               {
                  *(m_ioJoyLatch+CONTROLLER1) |= JOY_B;
               }
            }
            if ( m_ioJoy[CONTROLLER2]&JOY_ATURBO )
            {
               *(m_ioJoyLatch+CONTROLLER2) &= (~JOY_A);
               if ( m_alternator[CONTROLLER2][0] )
               {
                  *(m_ioJoyLatch+CONTROLLER2) |= JOY_A;
               }
            }
            if ( m_ioJoy[CONTROLLER2]&JOY_BTURBO )
            {
               *(m_ioJoyLatch+CONTROLLER2) &= (~JOY_B);
               if ( m_alternator[CONTROLLER2][1] )
               {
                  *(m_ioJoyLatch+CONTROLLER2) |= JOY_B;
               }
            }
         }

         m_last4016 = data;
         break;
   }
}

uint32_t CIOTurboJoypad::_IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x40|(m_ioJoyLatch[CONTROLLER1]&0x01);
         break;

      case IOJOY2:
         data = 0x40|(m_ioJoyLatch[CONTROLLER2]&0x01);
         break;
   }

   return data;
}

void CIOTurboJoypad::_IO ( uint32_t addr, uint8_t data )
{
   switch ( addr )
   {
      case IOJOY1:

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            *(m_ioJoyLatch+CONTROLLER1) = *(m_ioJoy+CONTROLLER1);
            *(m_ioJoyLatch+CONTROLLER2) = *(m_ioJoy+CONTROLLER2);
         }

         m_last4016 = data;
         break;
   }
}

uint32_t CIOZapper::IO ( uint32_t addr )
{
   uint32_t data = 0xFF;
   int32_t px, py, wx1, wy1, wx2, wy2;
   int32_t pxs, pys;
   uint8_t r = 0, g = 0, b = 0;
   int32_t nonBlacks = 0;

   // Evaluate light detection based on position most recently
   // passed into the emulator core by the UI.
   switch ( addr )
   {
      case IOJOY1:
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER1,&px,&py,&wx1,&wy1,&wx2,&wy2);
         if ( (py > wy1) && (py < wy2) &&
              (px > wx1) && (px < wx2) )
         {
            px = (((px-wx1)*256)/(wx2-wx1));
            py = (((py-wy1)*240)/(wy2-wy1));
            px -= 4;
            py -= 4;
            if ( px < 0 ) px = 0;
            if ( py < 0 ) py = 0;
            if ( px > 248 ) px = 248;
            if ( py > 232 ) py = 232;
            nonBlacks = 0;
            for ( pxs = px; pxs < px+8; pxs++ )
            {
               for ( pys = py; pys < py+8; pys++ )
               {
                  CNES::NES()->PPU()->PIXELRGB(pxs,pys,&r,&g,&b);
                  if ( (r>0) || (g>0) || (b>0) )
                  {
                     nonBlacks++;
                  }
               }
            }
         }

         // grab trigger state...
         data = m_ioJoy [ CONTROLLER1 ];

         if ( nonBlacks > 0 )
         {
            // Light detection is 0
            data &= (~(ZAPPER_DETECT));
         }
         else
         {
            data |= ZAPPER_DETECT;
         }
         break;

      case IOJOY2:
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER2,&px,&py,&wx1,&wy1,&wx2,&wy2);
         if ( (py > wy1) && (py < wy2) &&
              (px > wx1) && (px < wx2) )
         {
            px = (((px-wx1)*256)/(wx2-wx1));
            py = (((py-wy1)*240)/(wy2-wy1));
            px -= 4;
            py -= 4;
            if ( px < 0 ) px = 0;
            if ( py < 0 ) py = 0;
            if ( px > 248 ) px = 248;
            if ( py > 232 ) py = 232;
            nonBlacks = 0;
            for ( pxs = px; pxs < px+8; pxs++ )
            {
               for ( pys = py; pys < py+8; pys++ )
               {
                  CNES::NES()->PPU()->PIXELRGB(pxs,pys,&r,&g,&b);
                  if ( (r>0) || (g>0) || (b>0) )
                  {
                     nonBlacks++;
                  }
               }
            }
         }

         // grab trigger state...
         data = m_ioJoy [ CONTROLLER2 ];

         if ( nonBlacks > 0 )
         {
            // Light detection is 0
            data &= (~(ZAPPER_DETECT));
         }
         else
         {
            data |= ZAPPER_DETECT;
         }
         break;
   }

   return data;
}

void CIOZapper::IO ( uint32_t addr, uint8_t data )
{
   // Writes to zapper have no effect...
}

uint32_t CIOZapper::_IO ( uint32_t addr )
{
   uint32_t data = CIO::_IO(addr);

   switch ( addr )
   {
      case IOJOY1:
         break;

      case IOJOY2:
         break;
   }

   return data;
}

void CIOZapper::_IO ( uint32_t addr, uint8_t data )
{
   // Writes to zapper have no effect...
}

uint32_t CIOVaus::IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x40|(*(m_ioJoy+CONTROLLER1))|(((*(m_ioPotLatch+CONTROLLER1))&0x80)>>3);
         *(m_ioPotLatch+CONTROLLER1) <<= 1;
         break;

      case IOJOY2:
         data = 0x40|(*(m_ioJoy+CONTROLLER2))|(((*(m_ioPotLatch+CONTROLLER2))&0x80)>>3);
         *(m_ioPotLatch+CONTROLLER2) <<= 1;
         break;
   }

   return data;
}

void CIOVaus::IO ( uint32_t addr, uint8_t data )
{
   int32_t px1, py1;
   int32_t px2, py2;
   int32_t wx1, wy1, wx2, wy2;

   switch ( addr )
   {
      case IOJOY1:
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER1,&px1,&py1,&wx1,&wy1,&wx2,&wy2);
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER2,&px2,&py2,&wx1,&wy1,&wx2,&wy2);

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            if ( (py1 > wy1) && (py1 < wy2) &&
                 (px1 > wx1) && (px1 < wx2) )
            {
               *(m_ioPotLatch+CONTROLLER1) = ~((uint8_t)((((px1-wx1)*VAUS_POT_RANGE)/(wx2-wx1)))+(*(m_trimPot+CONTROLLER1)));
            }
            if ( (py2 > wy1) && (py2 < wy2) &&
                 (px2 > wx1) && (px2 < wx2) )
            {
               *(m_ioPotLatch+CONTROLLER2) = ~((uint8_t)((((px2-wx1)*VAUS_POT_RANGE)/(wx2-wx1)))+(*(m_trimPot+CONTROLLER2)));
            }
         }

         m_last4016 = data;
         break;
   }
}

uint32_t CIOVaus::_IO ( uint32_t addr )
{
   uint32_t data = 0xFF;

   switch ( addr )
   {
      case IOJOY1:
         data = 0x40|(*(m_ioJoy+CONTROLLER1))|(((*(m_ioPotLatch+CONTROLLER1))&0x80)>>3);
         break;

      case IOJOY2:
         data = 0x40|(*(m_ioJoy+CONTROLLER2))|(((*(m_ioPotLatch+CONTROLLER2))&0x80)>>3);
         break;
   }

   return data;
}

void CIOVaus::_IO ( uint32_t addr, uint8_t data )
{
   int32_t px1, py1;
   int32_t px2, py2;
   int32_t wx1, wy1, wx2, wy2;

   switch ( addr )
   {
      case IOJOY1:
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER1,&px1,&py1,&wx1,&wy1,&wx2,&wy2);
         CNES::NES()->CONTROLLERPOSITION(CONTROLLER2,&px2,&py2,&wx1,&wy1,&wx2,&wy2);

         if ( (m_last4016&1) && (!(data&1)) ) // latch on negative edge
         {
            if ( (py1 > wy1) && (py1 < wy2) &&
                 (px1 > wx1) && (px1 < wx2) )
            {
               *(m_ioPotLatch+CONTROLLER1) = ~((uint8_t)((((px1-wx1)*VAUS_POT_RANGE)/(wx2-wx1)))+(*(m_trimPot+CONTROLLER1)));
            }
            if ( (py2 > wy1) && (py2 < wy2) &&
                 (px2 > wx1) && (px2 < wx2) )
            {
               *(m_ioPotLatch+CONTROLLER2) = ~((uint8_t)((((px2-wx1)*VAUS_POT_RANGE)/(wx2-wx1)))+(*(m_trimPot+CONTROLLER2)));
            }
         }

         m_last4016 = data;
         break;
   }
}

void CIOVaus::SPECIAL(int32_t port,int32_t special)
{
   m_trimPot[port] = special;
}
