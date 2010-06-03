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
    
// ROMMapper001.cpp: implementation of the CROMMapper001 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrommapper001.h"

#include "cnesppu.h"

#include "cregisterdata.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Mapper 001 Registers
static CBitfieldData* tbl8000_9FFFBitfields [] =
{
   new CBitfieldData("CHR Mode", 4, 1, "%X", 2, "8KB mapping", "4KB mapping"),
   new CBitfieldData("PRG Size", 3, 1, "%X", 2, "32KB", "16KB"),
   new CBitfieldData("Slot Select", 2, 1, "%X", 2, "$C000 swappable, $8000 fixed to page $00", "$8000 swappable, $C000 fixed to page $0F"),
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "One-screen $2000", "One-screen $2400", "Vertical", "Horizontal")
};

static CBitfieldData* tblA000_BFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 0", 0, 5, "%02X", 0)
};

static CBitfieldData* tblC000_DFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 1", 0, 5, "%02X", 0)
};

static CBitfieldData* tblE000_FFFFBitfields [] =
{
   new CBitfieldData("WRAM State", 4, 1, "%X", 2, "Enabled", "Disabled"),
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", 4, tbl8000_9FFFBitfields),
   new CRegisterData(0xA000, "CHR Mapping 1", 1, tblA000_BFFFBitfields),
   new CRegisterData(0xC000, "CHR Mapping 2", 1, tblC000_DFFFBitfields),
   new CRegisterData(0xE000, "PRG Mapping", 2, tblE000_FFFFBitfields)
};

// MMC1 stuff
unsigned char  CROMMapper001::m_reg [] = { 0x0C, 0x00, 0x00, 0x00 };
unsigned char  CROMMapper001::m_regdef [] = { 0x0C, 0x00, 0x00, 0x00 };
unsigned char  CROMMapper001::m_sr = 0x00;
unsigned char  CROMMapper001::m_sel = 0x00;
unsigned char  CROMMapper001::m_srCount = 0;
UINT           CROMMapper001::m_lastWriteAddr = 0x0000;

CROMMapper001::CROMMapper001()
{

}

CROMMapper001::~CROMMapper001()
{

}

void CROMMapper001::RESET ()
{
   int idx;

   m_mapper = 1;

   CROM::RESET ( m_mapper );

   m_tblRegisters = tblRegisters;
   m_numRegisters = sizeof(tblRegisters)/sizeof(tblRegisters[0]);

   m_sr = 0;
   m_srCount = 0;
   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = m_regdef [ idx ];
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

void CROMMapper001::LOAD ( MapperState* data )
{
   m_reg [ 0 ] = data->data.mapper001.reg [ 0 ];
   m_reg [ 1 ] = data->data.mapper001.reg [ 1 ];
   m_reg [ 2 ] = data->data.mapper001.reg [ 2 ];
   m_reg [ 3 ] = data->data.mapper001.reg [ 3 ];
   m_sel = data->data.mapper001.sel;
   m_sr = data->data.mapper001.sr;
   m_srCount = data->data.mapper001.srCount;
   m_lastWriteAddr = data->data.mapper001.lastWriteAddr;

   if ( m_reg[0]&0x02 )
   {
      if ( m_reg[0]&0x01 )
      {
         CPPU::MIRRORHORIZ ();
      }
      else
      {
         CPPU::MIRRORVERT ();
      }
   }
   else
   {
      CPPU::MIRROR ( m_reg[0]&0x01 );
   }

   CROM::LOAD ( data );   
}

void CROMMapper001::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );

   data->data.mapper001.reg [ 0 ] = m_reg [ 0 ];
   data->data.mapper001.reg [ 1 ] = m_reg [ 1 ];
   data->data.mapper001.reg [ 2 ] = m_reg [ 2 ];
   data->data.mapper001.reg [ 3 ] = m_reg [ 3 ];
   data->data.mapper001.sel = m_sel;
   data->data.mapper001.sr = m_sr;
   data->data.mapper001.srCount = m_srCount;
   data->data.mapper001.lastWriteAddr = m_lastWriteAddr;
}

UINT CROMMapper001::MAPPER ( UINT addr )
{
   return m_reg [ (addr-MEM_32KB)/MEM_8KB ];
}

void CROMMapper001::MAPPER ( UINT addr, unsigned char data )
{
   unsigned char bank = 0;

   if ( (addr&0x6000) != (m_lastWriteAddr&0x6000) )
   {
      m_sr = 0x00;
      m_srCount = 0;
   }
   m_lastWriteAddr = addr;

   // Shift bits into registers...
   if ( data&0x80 )
   {
      m_sr = 0x00;
      m_srCount = 0;
      m_reg [ 0 ] |= m_regdef [ 0 ];
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
      m_PRGROMbank [ 2 ] = m_numPrgBanks-2;
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];
      m_PRGROMbank [ 3 ] = m_numPrgBanks-1;
   }
   else
   {
      m_sr>>=1;
      m_sr|=((data&0x01)<<4);
      m_srCount++;
      if ( m_srCount == 5 )
      {
         m_sel = (addr-0x8000)/0x2000;
         m_reg [ m_sel ] = m_sr;
         m_srCount = 0;

         // Act on new register content...
         switch ( m_sel )
         {
            case 0:
               if ( m_reg[0]&0x02 )
               {
                  if ( m_reg[0]&0x01 )
                  {
                     CPPU::MIRRORHORIZ ();
                  }
                  else
                  {
                     CPPU::MIRRORVERT ();
                  }
               }
               else
               {
                  CPPU::MIRROR ( m_reg[0]&0x01 );
               }
            break;
            case 1:
               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 0 ] = m_CHRROMmemory [ ((m_reg[1]&0x1F)>>1) ] + ((m_reg[1]&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
                     m_pCHRmemory [ 1 ] = m_CHRROMmemory [ ((m_reg[1]&0x1F)>>1) ] + ((m_reg[1]&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
                     m_pCHRmemory [ 2 ] = m_CHRROMmemory [ ((m_reg[1]&0x1F)>>1) ] + ((m_reg[1]&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
                     m_pCHRmemory [ 3 ] = m_CHRROMmemory [ ((m_reg[1]&0x1F)>>1) ] + ((m_reg[1]&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
                  }
                  else
                  {
                     m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (0<<UPSHIFT_1KB);
                     m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (1<<UPSHIFT_1KB);
                     m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (2<<UPSHIFT_1KB);
                     m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (3<<UPSHIFT_1KB);
                     m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (4<<UPSHIFT_1KB);
                     m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (5<<UPSHIFT_1KB);
                     m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (6<<UPSHIFT_1KB);
                     m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_reg[1]&0x1F)>>1 ] + (7<<UPSHIFT_1KB);
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 0 ] = CROM::CHRRAMPTR ( (((m_reg[1]&0x1F)>>1)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 1 ] = CROM::CHRRAMPTR ( (((m_reg[1]&0x1F)>>1)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 2 ] = CROM::CHRRAMPTR ( (((m_reg[1]&0x1F)>>1)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 3 ] = CROM::CHRRAMPTR ( (((m_reg[1]&0x1F)>>1)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB) );
                  }
               }
            break;
            case 2:
               if ( m_numChrBanks > 0 )
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 4 ] = m_CHRROMmemory [ ((m_reg[2]&0x1F)>>1) ] + ((m_reg[2]&0x01)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB);
                     m_pCHRmemory [ 5 ] = m_CHRROMmemory [ ((m_reg[2]&0x1F)>>1) ] + ((m_reg[2]&0x01)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB);
                     m_pCHRmemory [ 6 ] = m_CHRROMmemory [ ((m_reg[2]&0x1F)>>1) ] + ((m_reg[2]&0x01)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB);
                     m_pCHRmemory [ 7 ] = m_CHRROMmemory [ ((m_reg[2]&0x1F)>>1) ] + ((m_reg[2]&0x01)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB);
                  }
               }
               else
               {
                  if ( m_reg[0]&0x10 )
                  {
                     m_pCHRmemory [ 4 ] = CROM::CHRRAMPTR ( (((m_reg[2]&0x1F)>>1)<<UPSHIFT_4KB) + (0<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 5 ] = CROM::CHRRAMPTR ( (((m_reg[2]&0x1F)>>1)<<UPSHIFT_4KB) + (1<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 6 ] = CROM::CHRRAMPTR ( (((m_reg[2]&0x1F)>>1)<<UPSHIFT_4KB) + (2<<UPSHIFT_1KB) );
                     m_pCHRmemory [ 7 ] = CROM::CHRRAMPTR ( (((m_reg[2]&0x1F)>>1)<<UPSHIFT_4KB) + (3<<UPSHIFT_1KB) );
                  }
               }
            break;
            case 3:
               if ( m_reg[0]&0x08 )
               {
                  bank = (m_reg[3]&0x0F)<<1;
                  if ( m_reg[0]&0x04 )
                  {
                     m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
                     m_PRGROMbank [ 0 ] = bank;
                     m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
                     m_PRGROMbank [ 1 ] = bank+1;
                  }
                  else
                  {
                     m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank ];
                     m_PRGROMbank [ 2 ] = bank;
                     m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+1 ];
                     m_PRGROMbank [ 3 ] = bank+1;
                  }
               }
               else
               {
                  bank = m_reg[3]&0x0F;
                  m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
                  m_PRGROMbank [ 0 ] = bank;
                  m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
                  m_PRGROMbank [ 1 ] = bank+1;
                  m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
                  m_PRGROMbank [ 2 ] = bank+2;
                  m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];
                  m_PRGROMbank [ 3 ] = bank+3;
               }
            break;
         }

         // Check mapper state breakpoints...
         // m_sel is convenient register number...
         CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,m_sel);
      }
   }
}
