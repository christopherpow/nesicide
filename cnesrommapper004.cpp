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
    
// ROMMapper004.cpp: implementation of the CROMMapper004 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrommapper004.h"

#include "cnesppu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// MMC3 stuff
unsigned char  CROMMapper004::m_reg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  CROMMapper004::m_irqCounter = 0x00;
unsigned char  CROMMapper004::m_irqLatch = 0x00;
bool           CROMMapper004::m_irqEnable = false;
bool           CROMMapper004::m_irqReload = false;
unsigned char  CROMMapper004::m_prg [ 2 ] = { 0, 0 };
unsigned char  CROMMapper004::m_chr [ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };

CROMMapper004::CROMMapper004()
{

}

CROMMapper004::~CROMMapper004()
{

}

void CROMMapper004::RESET ()
{
   int idx;

   CROM::RESET ();

   m_mapper = 4;

   for ( idx = 0; idx < 8; idx++ )
   {
      m_reg [ idx ] = 0x00;
      m_chr [ idx ] = 0;
   }
   m_prg [ 0 ] = 0;
   m_prg [ 1 ] = 0;
   m_irqCounter = 0x00;
   m_irqLatch = 0x00;
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

bool CROMMapper004::SYNCH ( int scanline )
{
   bool fire = false;
   bool zero = false;

   if ( (scanline == -1) || ((scanline >= 0) && (scanline < 240)) )
   {
      if ( m_irqReload )
      {
         m_irqCounter = m_irqLatch;
         m_irqReload = false;
      }
      if ( (scanline == -1) || (CPPU::_PPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) )
      {
         if ( m_irqCounter )
         {
            m_irqCounter--;
         }
         if ( !m_irqCounter )
         {
            m_irqCounter = m_irqLatch;
            zero = true;
         }
      }
   }
   if ( m_irqEnable && zero )
   {
      fire = true;
   }

   return fire;
}

void CROMMapper004::SETCPU ( void )
{
   if ( m_reg[0]&0x40 )
   {
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (0<<UPSHIFT_8KB);
      m_PRGROMbank [ 0 ] = m_numPrgBanks-1;
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ m_prg[1]>>1 ] + ((m_prg[1]&0x1)<<UPSHIFT_8KB);
      m_PRGROMbank [ 1 ] = m_prg[1]>>1;
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_prg[0]>>1 ] + ((m_prg[0]&0x1)<<UPSHIFT_8KB);
      m_PRGROMbank [ 2 ] = m_prg[0]>>1;
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (1<<UPSHIFT_8KB);
      m_PRGROMbank [ 3 ] = m_numPrgBanks-1;
   }
   else
   {
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (m_prg[0])>>1 ] + ((m_prg[0]&0x1)<<UPSHIFT_8KB);
      m_PRGROMbank [ 0 ] = (m_prg[0])>>1;
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (m_prg[1])>>1 ] + ((m_prg[1]&0x1)<<UPSHIFT_8KB);
      m_PRGROMbank [ 1 ] = (m_prg[1])>>1;
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (0<<UPSHIFT_8KB);
      m_PRGROMbank [ 2 ] = m_numPrgBanks-1;
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ] + (1<<UPSHIFT_8KB);
      m_PRGROMbank [ 3 ] = m_numPrgBanks-1;
   }
}

void CROMMapper004::SETPPU ( void )
{
   if ( m_numChrBanks > 0 )
   {
      if ( m_reg[0]&0x80 )
      {
         m_pCHRmemory [ 0 ] = m_CHRROMmemory [ m_chr[4]>>3 ] + ((m_chr[4]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 1 ] = m_CHRROMmemory [ m_chr[5]>>3 ] + ((m_chr[5]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 2 ] = m_CHRROMmemory [ m_chr[6]>>3 ] + ((m_chr[6]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 3 ] = m_CHRROMmemory [ m_chr[7]>>3 ] + ((m_chr[7]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 4 ] = m_CHRROMmemory [ m_chr[0]>>3 ] + ((m_chr[0]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 5 ] = m_CHRROMmemory [ m_chr[1]>>3 ] + ((m_chr[1]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 6 ] = m_CHRROMmemory [ m_chr[2]>>3 ] + ((m_chr[2]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 7 ] = m_CHRROMmemory [ m_chr[3]>>3 ] + ((m_chr[3]&0x7)<<UPSHIFT_1KB);
      }
      else
      {
         m_pCHRmemory [ 0 ] = m_CHRROMmemory [ m_chr[0]>>3 ] + ((m_chr[0]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 1 ] = m_CHRROMmemory [ m_chr[1]>>3 ] + ((m_chr[1]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 2 ] = m_CHRROMmemory [ m_chr[2]>>3 ] + ((m_chr[2]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 3 ] = m_CHRROMmemory [ m_chr[3]>>3 ] + ((m_chr[3]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 4 ] = m_CHRROMmemory [ m_chr[4]>>3 ] + ((m_chr[4]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 5 ] = m_CHRROMmemory [ m_chr[5]>>3 ] + ((m_chr[5]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 6 ] = m_CHRROMmemory [ m_chr[6]>>3 ] + ((m_chr[6]&0x7)<<UPSHIFT_1KB);
         m_pCHRmemory [ 7 ] = m_CHRROMmemory [ m_chr[7]>>3 ] + ((m_chr[7]&0x7)<<UPSHIFT_1KB);
      }
   }
   else
   {
      if ( m_reg[0]&0x80 )
      {
         m_pCHRmemory [ 0 ] = CROM::CHRRAMPTR ( (m_chr[4]<<UPSHIFT_1KB)+(0<<UPSHIFT_1KB) );
         m_pCHRmemory [ 1 ] = CROM::CHRRAMPTR ( (m_chr[5]<<UPSHIFT_1KB)+(1<<UPSHIFT_1KB) );
         m_pCHRmemory [ 2 ] = CROM::CHRRAMPTR ( (m_chr[6]<<UPSHIFT_1KB)+(2<<UPSHIFT_1KB) );
         m_pCHRmemory [ 3 ] = CROM::CHRRAMPTR ( (m_chr[7]<<UPSHIFT_1KB)+(3<<UPSHIFT_1KB) );
         m_pCHRmemory [ 4 ] = CROM::CHRRAMPTR ( (m_chr[0]<<UPSHIFT_1KB)+(4<<UPSHIFT_1KB) );
         m_pCHRmemory [ 5 ] = CROM::CHRRAMPTR ( (m_chr[1]<<UPSHIFT_1KB)+(5<<UPSHIFT_1KB) );
         m_pCHRmemory [ 6 ] = CROM::CHRRAMPTR ( (m_chr[2]<<UPSHIFT_1KB)+(6<<UPSHIFT_1KB) );
         m_pCHRmemory [ 7 ] = CROM::CHRRAMPTR ( (m_chr[3]<<UPSHIFT_1KB)+(7<<UPSHIFT_1KB) );
      }
      else
      {
         m_pCHRmemory [ 0 ] = CROM::CHRRAMPTR ( (m_chr[0]<<UPSHIFT_1KB)+(0<<UPSHIFT_1KB) );
         m_pCHRmemory [ 1 ] = CROM::CHRRAMPTR ( (m_chr[1]<<UPSHIFT_1KB)+(1<<UPSHIFT_1KB) );
         m_pCHRmemory [ 2 ] = CROM::CHRRAMPTR ( (m_chr[2]<<UPSHIFT_1KB)+(2<<UPSHIFT_1KB) );
         m_pCHRmemory [ 3 ] = CROM::CHRRAMPTR ( (m_chr[3]<<UPSHIFT_1KB)+(3<<UPSHIFT_1KB) );
         m_pCHRmemory [ 4 ] = CROM::CHRRAMPTR ( (m_chr[4]<<UPSHIFT_1KB)+(4<<UPSHIFT_1KB) );
         m_pCHRmemory [ 5 ] = CROM::CHRRAMPTR ( (m_chr[5]<<UPSHIFT_1KB)+(5<<UPSHIFT_1KB) );
         m_pCHRmemory [ 6 ] = CROM::CHRRAMPTR ( (m_chr[6]<<UPSHIFT_1KB)+(6<<UPSHIFT_1KB) );
         m_pCHRmemory [ 7 ] = CROM::CHRRAMPTR ( (m_chr[7]<<UPSHIFT_1KB)+(7<<UPSHIFT_1KB) );
      }
   }
}

void CROMMapper004::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );

   int idx; 

   for ( idx = 0; idx < 8; idx++ )
   {
      m_reg [ idx ] = data->data.mapper004.reg [ idx ];
      m_chr [ idx ] = data->data.mapper004.chr [ idx ];
   }
   m_prg [ 0 ] = data->data.mapper004.prg [ 0 ];
   m_prg [ 1 ] = data->data.mapper004.prg [ 1 ];
   m_irqCounter = data->data.mapper004.irqCounter;
   m_irqEnable = data->data.mapper004.irqEnable;
   m_irqLatch = data->data.mapper004.irqLatch;
   SETCPU ();
   SETPPU ();
}

void CROMMapper004::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );

   int idx; 

   for ( idx = 0; idx < 8; idx++ )
   {
      data->data.mapper004.reg [ idx ] = m_reg [ idx ];
      data->data.mapper004.chr [ idx ] = m_chr [ idx ];
   }
   data->data.mapper004.prg [ 0 ] = m_prg [ 0 ];
   data->data.mapper004.prg [ 1 ] = m_prg [ 1 ];
   data->data.mapper004.irqCounter = m_irqCounter;
   data->data.mapper004.irqEnable = m_irqEnable;
   data->data.mapper004.irqLatch = m_irqLatch;
}

void CROMMapper004::DISPLAY ( char* sz )
{
   static const char* fmt = "IRQC:%02X\r\nIRQE:%02X\r\nIRQL:%02X";
   int bytes;

   bytes = sprintf ( sz, fmt, m_irqCounter, m_irqEnable, m_irqLatch );

   CROM::DISPLAY ( sz+bytes );
}

UINT CROMMapper004::MAPPER ( UINT addr )
{
   return m_reg [ (((addr-MEM_32KB)/MEM_8KB)*2)+(addr&0x0001) ];
}

void CROMMapper004::MAPPER ( UINT addr, unsigned char data )
{
   m_reg [ (((addr-0x8000)/0x2000)*2)+(addr&0x0001) ] = data;

   switch ( addr&0xE001 )
   {
      case 0x8000:
         SETCPU ();
         SETPPU ();
      break;
      case 0x8001:
         switch ( m_reg[0]&0x7 )
         {
            case 0x00:
               m_chr [ 0 ] = data&0xFE;
               m_chr [ 1 ] = (data&0xFE)+1;
               SETPPU ();
            break;
            case 0x01:
               m_chr [ 2 ] = data&0xFE;
               m_chr [ 3 ] = (data&0xFE)+1;
               SETPPU ();
            break;
            case 0x02:
               m_chr [ 4 ] = data;
               SETPPU ();
            break;
            case 0x03:
               m_chr [ 5 ] = data;
               SETPPU ();
            break;
            case 0x04:
               m_chr [ 6 ] = data;
               SETPPU ();
            break;
            case 0x05:
               m_chr [ 7 ] = data;
               SETPPU ();
            break;
            case 0x06:
               m_prg [ 0 ] = data;
               SETCPU ();
            break;
            case 0x07:
               m_prg [ 1 ] = data;
               SETCPU ();
            break;
         }
      break;
      case 0xA000:
         if ( !(CPPU::FOURSCREEN()) )
         {
            if ( data&0x01 )
            {
               CPPU::MIRRORHORIZ ();
            }
            else
            {
               CPPU::MIRRORVERT ();
            }
         }
      break;
      case 0xA001:
      break;
      case 0xC000:
         m_irqLatch = data;
      break;
      case 0xC001:
         m_irqReload = true;
      break;
      case 0xE000:
         m_irqEnable = false;
      break;
      case 0xE001:
         m_irqEnable = true;
      break;
   }
}
