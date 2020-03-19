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

#include "cnesrommapper069.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 069 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("Address", 0, 4, "%X", 16, "CHR Register 0", "CHR Register 1", "CHR Register 2", "CHR Register 3", "CHR Register 4", "CHR Register 5", "CHR Register 6", "CHR Register 7", "PRG Register 0", "PRG Register 1", "PRG Register 2", "PRG Register 3", "Mirroring", "IRQ Control", "IRQ Counter Low", "IRQ Counter High")
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("Expansion Sound Address", 0, 4, "%X", 16, "Channel 0 Frequency Low", "Channel 0 Frequency High", "Channel 1 Frequency Low", "Channel 1 Frequency High", "Channel 2 Frequency Low", "Channel 2 Frequency High", "Channel Disables", "Channel 0 Volume", "Channel 1 Volume", "Channel 2 Volume", "", "", "", "", "", "")
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("Expansion Sound Data", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Address", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xA000, "Data", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xC000, "Expansion Sound Address", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xE000, "Expansion Sound Data", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","A000","C000","E000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,4,4,tblRegisters,rowHeadings,columnHeadings);

CROMMapper069::CROMMapper069()
   : CROM(69)
{
   delete m_pSRAMmemory; // Remove open-bus default
   m_pSRAMmemory = new CMEMORY(0x6000,MEM_8KB);
   m_prgRemappable = true;
   m_chrRemappable = true;
   memset(m_reg,0,sizeof(m_reg));
   m_irqAsserted = false;
   m_irqCounter = 0x0000;
   m_irqEnable = false;
   m_irqCountEnable = false;
   memset(m_prg,0,sizeof(m_prg));
   memset(m_chr,0,sizeof(m_chr));
   m_sramAreaIsSram = false;
   m_sramAreaEnabled = false;
}

CROMMapper069::~CROMMapper069()
{
}

void CROMMapper069::RESET ( bool soft )
{
   int32_t idx;

   m_mapper = 69;

   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = 0x00;
      m_prg [ idx ] = 0;
   }

   for ( idx = 0; idx < 8; idx++ )
   {
      m_chr [ idx ] = 0;
   }

   m_irqCounter = 0x0000;
   m_irqEnable = false;
   m_irqCountEnable = false;
   m_irqAsserted = false;

   m_PRGROMmemory.REMAP(0,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(1,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(2,m_numPrgBanks-1);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   m_sramAreaIsSram = false;
   m_sramAreaEnabled = false;

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper069::SYNCCPU ( bool write, uint16_t addr, uint8_t data )
{
   uint32_t prevCounter = m_irqCounter;

   if ( m_irqCountEnable )
   {
      m_irqCounter--;
   }

   if ( m_irqEnable && (!prevCounter) )
   {
      CNES::NES()->CPU()->ASSERTIRQ ( eNESSource_Mapper );
      m_irqAsserted = true;

      if ( nesIsDebuggable )
      {
         // Check for IRQ breakpoint...
         CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
      }
   }
}

void CROMMapper069::SETCPU ( void )
{
   m_PRGROMmemory.REMAP(0,m_prg[1]);
   m_PRGROMmemory.REMAP(1,m_prg[2]);
   m_PRGROMmemory.REMAP(2,m_prg[3]);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
}

void CROMMapper069::SETPPU ( void )
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

uint32_t CROMMapper069::DEBUGINFO ( uint32_t addr )
{
   return m_reg [ ((addr-MEM_32KB)/MEM_8KB) ];
}

void CROMMapper069::HMAPPER ( uint32_t addr, uint8_t data )
{
   int32_t reg = ((addr-0x8000)/MEM_8KB);
   m_reg [ reg ] = data;

   switch ( addr&0xE000 )
   {
      case 0xA000:
         m_subReg [ m_reg[0]&0xF ] = data;
         switch ( m_reg[0]&0xF )
         {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
               m_chr [ m_reg[0]&0xF ] = data;
               SETPPU ();
               break;
            case 0x08:
               m_sramAreaIsSram = (data&0x40);
               m_sramAreaEnabled = (data&0x80);
               if ( m_sramAreaIsSram )
               {
                  m_prg[0] = (data&0x3F)%NUM_SRAM_BANKS;
                  REMAPSRAM(0x6000,m_prg[0]);
               }
               else
               {
                  m_prg[0] = (data&0x3F)%m_numPrgBanks;
               }
               break;
            case 0x09:
            case 0x0A:
            case 0x0B:
               m_prg [ (m_reg[0]&0xF)-8 ] = data%m_numPrgBanks;
               SETCPU ();
               break;
            case 0x0C:
               switch ( data )
               {
               case 0x00:
                  CNES::NES()->PPU()->MIRRORVERT();
                  break;
               case 0x01:
                  CNES::NES()->PPU()->MIRRORHORIZ();
                  break;
               case 0x02:
                  CNES::NES()->PPU()->MIRROR(0,0,0,0);
                  break;
               case 0x03:
                  CNES::NES()->PPU()->MIRROR(1,1,1,1);
                  break;
               }
               break;
            case 0x0D:
               m_irqEnable = (data&0x01);
               if ( !m_irqEnable )
               {
                  m_irqAsserted = false;
                  CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
               }
               m_irqCountEnable = (data&0x80);
               break;
            case 0x0E:
               m_irqCounter &= 0xFF00;
               m_irqCounter |= data;
               break;
            case 0x0F:
               m_irqCounter &= 0x00FF;
               m_irqCounter |= (data<<8);
               break;
         }

         break;
      case 0xE000:
         // CPTODO: sound
         break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}

uint32_t CROMMapper069::LMAPPER ( uint32_t addr )
{
   uint8_t data = CNES::NES()->CPU()->OPENBUS();

   if ( addr >= 0x6000 )
   {
      if ( m_sramAreaIsSram && m_sramAreaEnabled )
      {
         data = CROM::SRAMVIRT(addr);
      }
      else if ( !m_sramAreaIsSram )
      {
         data = CROM::PRGROMPHYS((m_prg[0]*MEM_8KB)+(addr-0x6000));
      }
   }

   return data;
}

void CROMMapper069::LMAPPER ( uint32_t addr, uint8_t data )
{
   if ( addr >= 0x6000 )
   {
      if ( m_sramAreaIsSram && m_sramAreaEnabled )
      {
         CROM::SRAMVIRT(addr,data);
      }
   }
}
