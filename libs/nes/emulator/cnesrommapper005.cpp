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

#include "cnesrommapper005.h"

#include "cnes6502.h"
#include "cnesppu.h"

uint8_t  CROMMapper005::m_prgMode = 0;
uint8_t  CROMMapper005::m_chrMode = 0;
uint8_t  CROMMapper005::m_chrHigh = 0;
uint8_t  CROMMapper005::m_irqScanline = 0;
uint8_t  CROMMapper005::m_irqEnabled = 0;
uint8_t  CROMMapper005::m_irqStatus = 0;
bool           CROMMapper005::m_prgRAM [] = { false, false, false };
bool           CROMMapper005::m_wp = false;
uint8_t  CROMMapper005::m_wp1 = 0;
uint8_t  CROMMapper005::m_wp2 = 0;
uint8_t  CROMMapper005::m_mult1 = 0;
uint8_t  CROMMapper005::m_mult2 = 0;
uint16_t CROMMapper005::m_prod = 0;
uint8_t  CROMMapper005::m_fillTile = 0;
uint8_t  CROMMapper005::m_fillAttr = 0;
uint32_t CROMMapper005::m_ppuCycle = 0;
uint8_t  CROMMapper005::m_chr[] = { 0, };

CROMMapper005::CROMMapper005()
{
}

CROMMapper005::~CROMMapper005()
{
}

void CROMMapper005::RESET ( bool soft )
{
   m_mapper = 5;

   CROM::RESET ( m_mapper, soft );

   m_prgMode = 0;
   m_chrMode = 0;
   m_chrHigh = 0;
   m_irqScanline = 0;
   m_irqStatus = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_prgRAM [ 0 ] = false;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_prgRAM [ 1 ] = false;
   if ( m_numPrgBanks == 2 )
   {
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 0 ];
      m_prgRAM [ 2 ] = false;
   }
   else
   {
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
      m_prgRAM [ 2 ] = false;
   }
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper005::SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr )
{
   int32_t scanline = CYCLE_TO_VISY(ppuCycle);

   m_ppuCycle = ppuCycle;

   SETPPU();

   if ( scanline == 0 )
   {
      m_irqStatus = 0x40;
      C6502::RELEASEIRQ(eNESSource_Mapper);
   }

   if ( scanline == 239 )
   {
      m_irqStatus &= ~(0x40);
   }

   if ( (m_irqScanline > 0) && (scanline == m_irqScanline) )
   {
      m_irqStatus |= 0x80;
   }

   if ( m_irqEnabled && (m_irqStatus&0x80) )
   {
      C6502::ASSERTIRQ ( eNESSource_Mapper );
   }
}

void CROMMapper005::SETCPU ( void )
{
}

void CROMMapper005::SETPPU ( void )
{
   int32_t rasterX = CYCLE_TO_VISX(m_ppuCycle);

   if ( (rasterX >= 256) &&
        (rasterX < 320) )
   {
      // Sprite fetches
      m_pCHRmemory [ 0 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[0] ];
      m_pCHRmemory [ 1 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[1] ];
      m_pCHRmemory [ 2 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[2] ];
      m_pCHRmemory [ 3 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[3] ];
      m_pCHRmemory [ 4 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[4] ];
      m_pCHRmemory [ 5 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[5] ];
      m_pCHRmemory [ 6 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[6] ];
      m_pCHRmemory [ 7 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[7] ];
   }
   else
   {
      // Background fetches
      m_pCHRmemory [ 0 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[8] ];
      m_pCHRmemory [ 1 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[9] ];
      m_pCHRmemory [ 2 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[10] ];
      m_pCHRmemory [ 3 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[11] ];
      m_pCHRmemory [ 4 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[8] ];
      m_pCHRmemory [ 5 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[9] ];
      m_pCHRmemory [ 6 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[10] ];
      m_pCHRmemory [ 7 ] = m_CHRmemory [ (m_chrHigh<<8)|m_chr[11] ];
   }
}

void CROMMapper005::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper005::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

uint32_t CROMMapper005::MAPPER ( uint32_t addr )
{
   if ( (m_prgRAM[0]) && (addr < 0xA000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else if ( (m_prgRAM[1]) && (addr >= 0xA000) && (addr < 0xC000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else if ( (m_prgRAM[1]) && (addr >= 0xC000) && (addr < 0xE000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else
   {
      return CROM::PRGROM(addr);
   }
}

void CROMMapper005::MAPPER ( uint32_t addr, uint8_t data )
{
   // if PRG RAM has been mapped in it will already be there...
   // first check to ensure writes allowed...
   if ( !m_wp )
   {
      CROM::SRAMVIRT ( addr, data );
   }
}

uint32_t CROMMapper005::LMAPPER ( uint32_t addr )
{
   uint8_t data = 0xff;

   if ( addr >= 0x6000 )
   {
      data = CROM::SRAMVIRT(addr);
   }
   else if ( addr >= 0x5C00 )
   {
      data = EXRAM ( addr );
   }
   else
   {
      switch ( addr )
      {
         case 0x5204:
            data = m_irqStatus;
            m_irqStatus &= ~(0x80);
            C6502::RELEASEIRQ ( eNESSource_Mapper );
            break;
         case 0x5205:
            data = m_prod&0xFF;
            break;
         case 0x5206:
            data = (m_prod>>8)&0xFF;
            break;
      }
   }

   return data;
}

void CROMMapper005::LMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t prgRAM;

   if ( addr >= 0x6000 )
   {
      CROM::SRAMVIRT(addr,data);
   }
   else if ( addr >= 0x5C00 )
   {
      EXRAM ( addr, data );
   }
   else
   {
      switch ( addr )
      {
         case 0x5100:
            m_prgMode = data&0x3;
            break;
         case 0x5101:
            m_chrMode = data&0x3;
            break;
         case 0x5103:
            m_wp1 = data&0x3;

            if ( (m_wp1 == 0x2) && (m_wp2 == 0x1) )
            {
               m_wp = false;
            }
            else
            {
               m_wp = true;
            }

            break;
         case 0x5104:
            m_wp2 = data&0x3;

            if ( (m_wp1 == 0x2) && (m_wp2 == 0x1) )
            {
               m_wp = false;
            }
            else
            {
               m_wp = true;
            }

            break;
         case 0x5105:
            CPPU::MIRROR ( data&0x03, (data&0x0C)>>2, (data&0x30)>>4, (data&0xC0)>>6 );
            break;
         case 0x5106:
            m_fillTile = data;
            break;
         case 0x5107:
            m_fillAttr = data&0x3;
            break;
         case 0x5113:
            data &= 0x7;
            CROM::REMAPSRAM ( 0x6000, data );
            break;
         case 0x5114:
            prgRAM = !(data&0x80);
            data &= 0x7F;

            if ( prgRAM )
            {
               data &= 0x7;
            }

            if ( m_prgMode == 3 )
            {
               m_prgRAM[0] = prgRAM;
               if ( m_prgRAM[0] )
               {
                  CROM::REMAPSRAM ( 0x8000, data );
               }
               else
               {
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
               }
            }

            break;
         case 0x5115:
            prgRAM = !(data&0x80);
            data &= 0x7F;

            if ( prgRAM )
            {
               data &= 0x7;
            }

            if ( (m_prgMode == 1) || (m_prgMode == 2) )
            {
               m_prgRAM[0] = prgRAM;
               m_prgRAM[1] = prgRAM;
               if ( prgRAM )
               {
                  CROM::REMAPSRAM ( 0x8000, (data&0xFE)+0 );
                  CROM::REMAPSRAM ( 0xA000, (data&0xFE)+1 );
               }
               else
               {
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (data&0xFE)+0 ];
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (data&0xFE)+1 ];
               }
            }
            else if ( m_prgMode == 3 )
            {
               m_prgRAM[1] = prgRAM;
               if ( prgRAM )
               {
                  CROM::REMAPSRAM ( 0x8000, data );
               }
               else
               {
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ data ];
               }
            }

            break;
         case 0x5116:
            prgRAM = !(data&0x80);
            data &= 0x7F;

            if ( prgRAM )
            {
               data &= 0x7;
            }

            if ( (m_prgMode == 2) || (m_prgMode == 3) )
            {
               m_prgRAM[2] = prgRAM;
               if ( prgRAM )
               {
                  CROM::REMAPSRAM ( 0xC000, data );
               }
               else
               {
                  m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ data ];
               }
            }

            break;
         case 0x5117:
            if ( m_prgMode == 0 )
            {
               m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (data&0xFC)+0 ];
               m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (data&0xFC)+1 ];
               m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ (data&0xFC)+2 ];
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ (data&0xFC)+3 ];
            }
            else if ( m_prgMode == 1 )
            {
               m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ (data&0xFE)+0 ];
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ (data&0xFE)+1 ];
            }
            else if ( (m_prgMode == 2) || (m_prgMode == 3) )
            {
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ data ];
            }

            break;
         case 0x5120:

            if ( m_chrMode == 3 )
            {
               m_chr[0] = data;
            }

            break;
         case 0x5121:

            if ( m_chrMode == 3 )
            {
               m_chr[1] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[0] = (data<<1)+0;
               m_chr[1] = (data<<1)+1;
            }

            break;
         case 0x5122:

            if ( m_chrMode == 3 )
            {
               m_chr[2] = data;
            }

            break;
         case 0x5123:

            if ( m_chrMode == 3 )
            {
               m_chr[3] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[2] = (data<<1)+0;
               m_chr[3] = (data<<1)+1;
            }
            else if ( m_chrMode == 1 )
            {
               m_chr[0] = (data<<2)+0;
               m_chr[1] = (data<<2)+1;
               m_chr[2] = (data<<2)+2;
               m_chr[3] = (data<<2)+3;
            }

            break;
         case 0x5124:

            if ( m_chrMode == 3 )
            {
               m_chr[4] = data;
            }

            break;
         case 0x5125:

            if ( m_chrMode == 3 )
            {
               m_chr[5] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[4] = (data<<1)+0;
               m_chr[5] = (data<<1)+1;
            }

            break;
         case 0x5126:

            if ( m_chrMode == 3 )
            {
               m_chr[6] = data;
            }

            break;
         case 0x5127:

            if ( m_chrMode == 3 )
            {
               m_chr[7] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[6] = (data<<1)+0;
               m_chr[7] = (data<<1)+1;
            }
            else if ( m_chrMode == 1 )
            {
               m_chr[4] = (data<<2)+0;
               m_chr[5] = (data<<2)+1;
               m_chr[6] = (data<<2)+2;
               m_chr[7] = (data<<2)+3;
            }
            else if ( m_chrMode == 0 )
            {
               m_chr[0] = (data<<3)+0;
               m_chr[1] = (data<<3)+1;
               m_chr[2] = (data<<3)+2;
               m_chr[3] = (data<<3)+3;
               m_chr[4] = (data<<3)+4;
               m_chr[5] = (data<<3)+5;
               m_chr[6] = (data<<3)+6;
               m_chr[7] = (data<<3)+7;
            }

            break;
         case 0x5128:

            if ( m_chrMode == 3 )
            {
               m_chr[8] = data;
            }

            break;
         case 0x5129:

            if ( m_chrMode == 3 )
            {
               m_chr[9] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[8] = (data<<1)+0;
               m_chr[9] = (data<<1)+1;
            }

            break;
         case 0x512A:

            if ( m_chrMode == 3 )
            {
               m_chr[10] = data;
            }

            break;
         case 0x512B:

            if ( m_chrMode == 3 )
            {
               m_chr[11] = data;
            }
            else if ( m_chrMode == 2 )
            {
               m_chr[10] = (data<<1)+0;
               m_chr[11] = (data<<1)+1;
            }
            else if ( m_chrMode == 1 || m_chrMode == 0 )
            {
               m_chr[8] = (data<<2)+0;
               m_chr[9] = (data<<2)+1;
               m_chr[10] = (data<<2)+2;
               m_chr[11] = (data<<2)+3;
            }

            break;

         case 0x5130:
            m_chrHigh = data&0x3;
            break;
         case 0x5203:
            m_irqScanline = data;
            break;
         case 0x5204:
            m_irqEnabled = data;
            break;
         case 0x5205:
            m_mult1 = data;
            m_prod = m_mult1*m_mult2;
            break;
         case 0x5206:
            m_mult2 = data;
            m_prod = m_mult1*m_mult2;
            break;
      }
   }
   SETPPU();
}
