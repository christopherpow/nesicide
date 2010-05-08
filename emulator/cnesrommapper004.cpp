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

#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Mapper 004 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("CHR Mode", 7, 1, "%X", 2, "2KBx2,1KBx4", "1KBx4,2KBx2"),
   new CBitfieldData("PRG Mode", 6, 1, "%X", 2, "$8000 and $A000 swappable, $C000 and $E000 fixed to last bank", "$8000 and $E000 swappable, $A000 and $C000 fixed to last bank"),
   new CBitfieldData("Address", 0, 3, "%X", 8, "CHR Register 0", "CHR Register 1", "CHR Register 2", "CHR Register 3", "CHR Register 4", "CHR Register 5", "PRG Register 0", "PRG Register 1")
};

static CBitfieldData* tbl8001Bitfields [] =
{
   new CBitfieldData("Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 1, "%X", 2, "Vertical", "Horizontal")
};

static CBitfieldData* tblA001Bitfields [] =
{
   new CBitfieldData("WRAM", 7, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("WRAM Write-protect", 6, 1, "%X", 2, "Off", "On")
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("IRQ Reload", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC001Bitfields [] =
{
   new CBitfieldData("IRQ Clear", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("IRQ Ack/Disable", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE001Bitfields [] =
{
   new CBitfieldData("IRQ Enable", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", 3, tbl8000Bitfields),
   new CRegisterData(0x8001, "Data", 1, tbl8001Bitfields),
   new CRegisterData(0xA000, "Mirroring", 1, tblA000Bitfields),
   new CRegisterData(0xA001, "WRAM", 2, tblA001Bitfields),
   new CRegisterData(0xC000, "IRQ Reload", 1, tblC000Bitfields),
   new CRegisterData(0xC001, "IRQ Clear", 1, tblC001Bitfields),
   new CRegisterData(0xE000, "IRQ Ack/Disable", 1, tblE000Bitfields),
   new CRegisterData(0xE001, "IRQ Enable", 1, tblE001Bitfields),
};

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
   m_tblRegisters = tblRegisters;
   m_numRegisters = sizeof(tblRegisters)/sizeof(tblRegisters[0]);

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

void CROMMapper004::SYNCH ( int scanline )
{
   bool zero = false;

   if ( m_irqReload )
   {
      m_irqCounter = m_irqLatch;
      m_irqReload = false;
      if ( !m_irqCounter )
      {
         zero = true;
      }
      C6502::RELEASEIRQ ( eSource_Mapper );
   }
   else
   {
      m_irqCounter--;
      if ( m_irqCounter <= 0 )
      {
         m_irqCounter = m_irqLatch;
         zero = true;
      }
   }
   if ( m_irqEnable && zero )
   {
      C6502::ASSERTIRQ ( eSource_Mapper );

      // Check for IRQ breakpoint...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
   }
}

void CROMMapper004::SETCPU ( void )
{   
   if ( m_reg[0]&0x40 )
   {
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
      m_PRGROMbank [ 0 ] = m_numPrgBanks-2;
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ m_prg[1] ];
      m_PRGROMbank [ 1 ] = m_prg[1];
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_prg[0] ];
      m_PRGROMbank [ 2 ] = m_prg[0];
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
      m_PRGROMbank [ 3 ] = m_numPrgBanks-1;
   }
   else
   {
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ m_prg[0] ];
      m_PRGROMbank [ 0 ] = m_prg[0];
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ m_prg[1] ];
      m_PRGROMbank [ 1 ] = m_prg[1];
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
      m_PRGROMbank [ 2 ] = m_numPrgBanks-2;
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
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

UINT CROMMapper004::MAPPER ( UINT addr )
{
   return m_reg [ (((addr-MEM_32KB)/MEM_8KB)*2)+(addr&0x0001) ];
}

void CROMMapper004::MAPPER ( UINT addr, unsigned char data )
{
   int reg = (((addr-0x8000)/0x2000)*2)+(addr&0x0001);
   m_reg [ reg ] = data;

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
         C6502::RELEASEIRQ ( eSource_Mapper );
      break;
      case 0xE001:
         m_irqEnable = true;
      break;
   }

   // Check mapper state breakpoints...
   CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
}
