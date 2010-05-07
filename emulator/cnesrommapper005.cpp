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
    
// ROMMapper005.cpp: implementation of the CROMMapper005 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrommapper005.h"

#include "cnes6502.h"
#include "cnesppu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CROMMapper005::m_prgMode = 0;
unsigned char  CROMMapper005::m_chrMode = 0;
unsigned char  CROMMapper005::m_irqScanline = 0;
unsigned char  CROMMapper005::m_irqEnabled = 0;
unsigned char  CROMMapper005::m_irqStatus = 0;
bool           CROMMapper005::m_prgRAM [] = { false, false, false, false };
bool           CROMMapper005::m_wp = false;
unsigned char  CROMMapper005::m_wp1 = 0;
unsigned char  CROMMapper005::m_wp2 = 0;
unsigned char  CROMMapper005::m_mult1 = 0;
unsigned char  CROMMapper005::m_mult2 = 0;
unsigned short CROMMapper005::m_prod = 0;
unsigned char  CROMMapper005::m_fillTile = 0;
unsigned char  CROMMapper005::m_fillAttr = 0;

CROMMapper005::CROMMapper005()
{

}

CROMMapper005::~CROMMapper005()
{

}

void CROMMapper005::RESET ()
{
   CROM::RESET ();

   m_mapper = 5;

   m_prgMode = 0;
   m_chrMode = 0;
   m_irqScanline = 0;
   m_irqStatus = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 0 ] = m_numPrgBanks-1;
   m_prgRAM [ 0 ] = false;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 1 ] = m_numPrgBanks-1;
   m_prgRAM [ 1 ] = false;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 2 ] = m_numPrgBanks-1;
   m_prgRAM [ 2 ] = false;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
   m_PRGROMbank [ 3 ] = m_numPrgBanks-1;
   m_prgRAM [ 3 ] = false;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper005::SYNCH ( int scanline )
{
   if ( scanline == 0 )
   {
      m_irqStatus = 0x40;
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
      C6502::ASSERTIRQ ( eSource_Mapper );
   }
}

void CROMMapper005::SETCPU ( void )
{
}

void CROMMapper005::SETPPU ( void )
{
}

void CROMMapper005::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper005::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

UINT CROMMapper005::MAPPER ( UINT addr )
{
   // CPTODO: return MMC5 registers here...
   return 0xA1;
}

void CROMMapper005::MAPPER ( UINT addr, unsigned char data )
{
   // if PRG RAM has been mapped in it will already be there...
   // first check to ensure writes allowed...
   if ( !m_wp )
   {
      CROM::PRGROM ( addr, data );
   }
}

UINT CROMMapper005::LMAPPER ( UINT addr )
{
   unsigned char data = 0xff;

   if ( addr >= 0x5C00 )
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

void CROMMapper005::LMAPPER ( UINT addr, unsigned char data )
{
   if ( addr >= 0x5C00 )
   {
      EXRAM ( addr, data );
   }
   else
   {
      switch ( addr )
      {
         case 0x5100:
            m_prgMode = data;
         break;
         case 0x5101:
            m_chrMode = data;
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
            m_pSRAMmemory = m_SRAMmemory + (data<<UPSHIFT_8KB);
         break;
         case 0x5114:
            m_prgRAM [ 0 ] = !(data&0x80);
            data &= 0x7F;
            if ( m_prgRAM[0] ) data &= 0x7;
            if ( m_prgMode == 3 )
            {
               if ( m_prgRAM[0] )
               {
                  m_pPRGROMmemory [ 0 ] = m_SRAMmemory + (data<<UPSHIFT_8KB);
                  m_PRGROMbank [ 0 ] = data;
               }
               else
               {
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
                  m_PRGROMbank [ 0 ] = data;
               }
            }
         break;
         case 0x5115:
            m_prgRAM [ 1 ] = !(data&0x80);
            data &= 0x7F;
            if ( m_prgRAM[1] ) data &= 0x7;
            if ( (m_prgMode == 1) || (m_prgMode == 2) )
            {
               if ( m_prgRAM[1] )
               {
                  m_pPRGROMmemory [ 0 ] = m_SRAMmemory + (data<<UPSHIFT_8KB) + (0<<UPSHIFT_8KB);
                  m_PRGROMbank [ 0 ] = data;
                  m_pPRGROMmemory [ 1 ] = m_SRAMmemory + (data<<UPSHIFT_8KB) + (1<<UPSHIFT_8KB);
                  m_PRGROMbank [ 1 ] = data+1;
               }
               else
               {
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
                  m_PRGROMbank [ 0 ] = data;
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ data+1 ];
                  m_PRGROMbank [ 1 ] = data+1;
               }
            }
            else if ( m_prgMode == 3 )
            {
               if ( m_prgRAM[0] )
               {
                  m_pPRGROMmemory [ 1 ] = m_SRAMmemory + (data<<UPSHIFT_8KB);
                  m_PRGROMbank [ 1 ] = data;
               }
               else
               {
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ data ];
                  m_PRGROMbank [ 1 ] = data;
               }
            }
         break;
         case 0x5116:
            m_prgRAM [ 2 ] = !(data&0x80);
            data &= 0x7F;
            if ( m_prgRAM[2] ) data &= 0x7;
            if ( (m_prgMode == 2) || (m_prgMode == 3) )
            {
               if ( m_prgRAM[0] )
               {
                  m_pPRGROMmemory [ 2 ] = m_SRAMmemory + (data<<UPSHIFT_8KB);
                  m_PRGROMbank [ 2 ] = data;
               }
               else
               {
                  m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ data ];
                  m_PRGROMbank [ 2 ] = data;
               }
            }
         break;
         case 0x5117:
            m_prgRAM [ 3 ] = !(data&0x80);
            data &= 0x7F;
            if ( m_prgRAM[3] ) data &= 0x7;
            if ( m_prgMode == 0 )
            {
               m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ data ];
               m_PRGROMbank [ 0 ] = data;
               m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ data+1 ];
               m_PRGROMbank [ 1 ] = data+1;
               m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ data+2 ];
               m_PRGROMbank [ 2 ] = data+2;
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ data+3 ];
               m_PRGROMbank [ 3 ] = data+3;
            }
            else if ( m_prgMode == 1 )
            {
               m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ data ];
               m_PRGROMbank [ 2 ] = data;
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ data+1 ];
               m_PRGROMbank [ 3 ] = data+1;
            }
            else if ( (m_prgMode == 2) || (m_prgMode == 3) )
            {
               m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ data ];
               m_PRGROMbank [ 3 ] = data;
            }
         break;
         case 0x5120:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 0 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 0 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5121:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 1 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 1 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5122:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 2 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 2 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5123:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 3 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 3 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5124:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 4 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 4 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5125:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 5 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 5 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5126:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 6 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 6 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5127:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 7 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 7 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5128:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 4 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 4 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x5129:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 5 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 5 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x512A:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 6 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 6 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
         break;         
         case 0x512B:
            if ( m_chrMode == 3 )
            {
               if ( m_numChrBanks > 0 )
               {
                  m_pCHRmemory [ 7 ] = m_CHRROMmemory [ data>>3 ] + ((data&0x7)<<UPSHIFT_1KB);
               }
               else
               {
                  m_pCHRmemory [ 7 ] = CROM::CHRRAMPTR ( (data&0x7)<<UPSHIFT_1KB );
               }
            }
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
}
