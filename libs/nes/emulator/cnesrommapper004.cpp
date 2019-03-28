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

#include "cnesrommapper004.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 004 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("CHR Mode", 7, 1, "%X", 2, "2KBx2,1KBx4", "1KBx4,2KBx2"),
   new CBitfieldData("PRG Mode", 6, 1, "%X", 2, "8000 and A000 swappable, C000 and E000 fixed to last bank", "8000 and E000 swappable, A000 and C000 fixed to last bank"),
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
   new CRegisterData(0x8000, "Control", nesMapperHighRead, nesMapperHighWrite, 3, tbl8000Bitfields),
   new CRegisterData(0x8001, "Data", nesMapperHighRead, nesMapperHighWrite, 1, tbl8001Bitfields),
   new CRegisterData(0xA000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xA001, "WRAM", nesMapperHighRead, nesMapperHighWrite, 2, tblA001Bitfields),
   new CRegisterData(0xC000, "IRQ Reload", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xC001, "IRQ Clear", nesMapperHighRead, nesMapperHighWrite, 1, tblC001Bitfields),
   new CRegisterData(0xE000, "IRQ Ack/Disable", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
   new CRegisterData(0xE001, "IRQ Enable", nesMapperHighRead, nesMapperHighWrite, 1, tblE001Bitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","8001","A000","A001","C000","C001","E000","E001"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,8,8,tblRegisters,rowHeadings,columnHeadings);

CROMMapper004::CROMMapper004()
   : CROM(4)
{
   memset(m_reg,0,sizeof(m_reg));
   m_irqAsserted = false;
   m_irqCounter = 0x00;
   m_irqLatch = 0x00;
   m_irqEnable = false;
   m_irqReload = false;
   memset(m_prg,0,sizeof(m_prg));
   memset(m_chr,0,sizeof(m_chr));

   m_lastPPUAddrA12 = 0;
   m_lastPPUCycle = 0;
}

CROMMapper004::~CROMMapper004()
{
}

void CROMMapper004::RESET ( bool soft )
{
   int32_t idx;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

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
   m_irqAsserted = false;

   m_lastPPUCycle = 0;
   m_lastPPUAddrA12 = 0;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper004::SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr )
{
   bool zero = false;
   uint16_t ppuAddrA12 = ppuAddr&0x1000;
   bool clockIt = false;

   // Determine if clocked...rising edge more than 11 cycles
   // after the last one.
   if ( (m_lastPPUAddrA12 == 0x0000) &&
         (ppuAddrA12 == 0x1000) )
   {
      clockIt = true;
   }

   // Update PPU watching variables on rising edge...
   m_lastPPUAddrA12 = ppuAddrA12;

   if ( clockIt )
   {
      if ( (ppuCycle-m_lastPPUCycle) < 12 )
      {
         clockIt = false;
      }

      m_lastPPUCycle = ppuCycle;
   }

   // Clock the IRQ counter if needed...
   if ( clockIt )
   {
      if ( m_irqReload )
      {
         m_irqCounter = m_irqLatch;
         m_irqReload = false;

         if ( m_irqCounter == 0 )
         {
            zero = true;
         }
      }
      else
      {
         m_irqCounter--;

         if ( m_irqCounter == 0 )
         {
            m_irqReload = true;
            zero = true;
         }
      }

      if ( m_irqEnable && zero )
      {
         CNES::NES()->CPU()->ASSERTIRQ ( eNESSource_Mapper );
         m_irqAsserted = true;

         if ( nesIsDebuggable() )
         {
            // Check for IRQ breakpoint...
            CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
         }
      }
   }
}

void CROMMapper004::SETCPU ( void )
{
   if ( m_reg[0]&0x40 )
   {
      m_PRGROMmemory.REMAP(0,m_numPrgBanks-2);
      m_PRGROMmemory.REMAP(1,m_prg[1]);
      m_PRGROMmemory.REMAP(2,m_prg[0]);
      m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
   }
   else
   {
      m_PRGROMmemory.REMAP(0,m_prg[0]);
      m_PRGROMmemory.REMAP(1,m_prg[0]);
      m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
      m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
   }
}

void CROMMapper004::SETPPU ( void )
{
   if ( m_reg[0]&0x80 )
   {
      m_CHRmemory.REMAP(0,m_chr[4]);
      m_CHRmemory.REMAP(1,m_chr[5]);
      m_CHRmemory.REMAP(2,m_chr[6]);
      m_CHRmemory.REMAP(3,m_chr[7]);
      m_CHRmemory.REMAP(4,m_chr[0]);
      m_CHRmemory.REMAP(5,m_chr[1]);
      m_CHRmemory.REMAP(6,m_chr[2]);
      m_CHRmemory.REMAP(7,m_chr[3]);
   }
   else
   {
      m_CHRmemory.REMAP(0,m_chr[0]);
      m_CHRmemory.REMAP(1,m_chr[1]);
      m_CHRmemory.REMAP(2,m_chr[2]);
      m_CHRmemory.REMAP(3,m_chr[3]);
      m_CHRmemory.REMAP(4,m_chr[4]);
      m_CHRmemory.REMAP(5,m_chr[5]);
      m_CHRmemory.REMAP(6,m_chr[6]);
      m_CHRmemory.REMAP(7,m_chr[7]);
   }
}

uint32_t CROMMapper004::DEBUGINFO ( uint32_t addr )
{
   return m_reg [ (((addr-MEM_32KB)/MEM_8KB)*2)+(addr&0x0001) ];
}

void CROMMapper004::HMAPPER ( uint32_t addr, uint8_t data )
{
   int32_t reg = (((addr-0x8000)/0x2000)*2)+(addr&0x0001);
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
               m_prg [ 0 ] = data%m_numPrgBanks;
               SETCPU ();
               break;
            case 0x07:
               m_prg [ 1 ] = data%m_numPrgBanks;
               SETCPU ();
               break;
         }

         break;
      case 0xA000:
         if ( data&0x01 )
         {
            CNES::NES()->PPU()->MIRRORHORIZ ();
         }
         else
         {
            CNES::NES()->PPU()->MIRRORVERT ();
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
         CNES::NES()->CPU()->RELEASEIRQ ( eNESSource_Mapper );
         m_irqAsserted = false;
         break;
      case 0xE001:
         m_irqEnable = true;
         break;
   }

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
