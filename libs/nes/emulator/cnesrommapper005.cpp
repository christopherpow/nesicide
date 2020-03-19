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

#include "cregisterdata.h"

// Mapper 005 Registers
static CBitfieldData* tbl5000Bitfields [] =
{
   new CBitfieldData("Duty Cycle", 6, 2, "%X", 4, "25%", "50%", "75%", "12.5%"),
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Disabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

static CBitfieldData* tbl5001Bitfields [] =
{
   new CBitfieldData("Sweep Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sweep Divider", 4, 3, "%X", 0),
   new CBitfieldData("Sweep Direction", 3, 1, "%X", 2, "Down", "Up"),
   new CBitfieldData("Sweep Shift", 0, 3, "%X", 0)
};

static CBitfieldData* tbl5002Bitfields [] =
{
   new CBitfieldData("Period Low-bits", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5003Bitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "0A","FE","14","02","28","04","50","06","A0","08","3C","0A","0E","0C","1A","0E","0C","10","18","12","30","14","60","16","C0","18","48","1A","10","1C","20","1E"),
   new CBitfieldData("Period High-bits", 0, 3, "%X", 0)
};

static CBitfieldData* tbl5010Bitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Loop", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "1AC","17C","154","140","11E","0FE","0E2","0D6","0BE","0A0","08E","080","06A","054","048","036")
};

static CBitfieldData* tbl5011Bitfields [] =
{
   new CBitfieldData("Volume", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5015Bitfields [] =
{
   new CBitfieldData("Square2 Channel", 1, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Square1 Channel", 0, 1, "%X", 2, "Disabled", "Enabled"),
};

static CBitfieldData* tbl5100Bitfields [] =
{
   new CBitfieldData("PRG Mode", 0, 2, "%X", 4, "32KB", "16KB", "16KB+8KB", "8KB")
};

static CBitfieldData* tbl5101Bitfields [] =
{
   new CBitfieldData("CHR Mode", 0, 2, "%X", 4, "8KB", "4KB", "2KB", "1KB")
};

static CBitfieldData* tbl5102Bitfields [] =
{
   new CBitfieldData("PRG-RAM Protect A", 0, 2, "%X", 0)
};

static CBitfieldData* tbl5103Bitfields [] =
{
   new CBitfieldData("PRG-RAM Protect B", 0, 2, "%X", 0)
};

static CBitfieldData* tbl5104Bitfields [] =
{
   new CBitfieldData("EXRAM Mode", 0, 2, "%X", 4, "Extra Nametable mode", "Extended Attribute mode", "CPU access mode", "CPU read-only mode")
};

static CBitfieldData* tbl5105Bitfields [] =
{
   new CBitfieldData("$2000 mapping", 0, 2, "%X", 4, "$2000", "$2400", "EXRAM", "Fill mode"),
   new CBitfieldData("$2400 mapping", 2, 2, "%X", 4, "$2000", "$2400", "EXRAM", "Fill mode"),
   new CBitfieldData("$2800 mapping", 4, 2, "%X", 4, "$2000", "$2400", "EXRAM", "Fill mode"),
   new CBitfieldData("$2C00 mapping", 6, 2, "%X", 4, "$2000", "$2400", "EXRAM", "Fill mode")
};

static CBitfieldData* tbl5106Bitfields [] =
{
   new CBitfieldData("Fill Tile", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5107Bitfields [] =
{
   new CBitfieldData("Fill Attribute bits", 0, 2, "%X", 0)
};

static CBitfieldData* tbl5113Bitfields [] =
{
   new CBitfieldData("$6000 mapping", 0, 3, "%X", 0)
};

static CBitfieldData* tbl5114Bitfields [] =
{
   new CBitfieldData("PRG mapping", 0, 7, "%02X", 0),
   new CBitfieldData("ROM select", 7, 1, "%X", 2, "RAM", "ROM")
};

static CBitfieldData* tbl5115Bitfields [] =
{
   new CBitfieldData("PRG mapping", 0, 7, "%02X", 0),
   new CBitfieldData("ROM select", 7, 1, "%X", 2, "RAM", "ROM")
};

static CBitfieldData* tbl5116Bitfields [] =
{
   new CBitfieldData("PRG mapping", 0, 7, "%02X", 0),
   new CBitfieldData("ROM select", 7, 1, "%X", 2, "RAM", "ROM")
};

static CBitfieldData* tbl5117Bitfields [] =
{
   new CBitfieldData("PRG mapping", 0, 7, "%02X", 0)
};

static CBitfieldData* tbl5120Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5121Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5122Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5123Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5124Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5125Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5126Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5127Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5128Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5129Bitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl512ABitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl512BBitfields [] =
{
   new CBitfieldData("CHR mapping", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5130Bitfields [] =
{
   new CBitfieldData("CHR high bits", 0, 2, "%X", 0)
};

static CBitfieldData* tbl5200Bitfields [] =
{
   new CBitfieldData("Tile", 0, 5, "%X", 0),
   new CBitfieldData("Side", 6, 1, "%X", 2, "Left", "Right"),
   new CBitfieldData("Enabled", 7, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tbl5201Bitfields [] =
{
   new CBitfieldData("Split Y scroll", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5202Bitfields [] =
{
   new CBitfieldData("Split 4KB CHR page", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5203Bitfields [] =
{
   new CBitfieldData("IRQ Target", 0, 8, "%02X", 0),
};

static CBitfieldData* tbl5204Bitfields [] =
{
   new CBitfieldData("In Frame", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Enabled/Pending", 7, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tbl5205Bitfields [] =
{
   new CBitfieldData("Multiplicand & Result LSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5206Bitfields [] =
{
   new CBitfieldData("Multiplier & Result MSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5209Bitfields [] =
{
   new CBitfieldData("Timer IRQ", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Timer LSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl520ABitfields [] =
{
   new CBitfieldData("Timer MSB", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x5000, "Square1 Control", nesMapperLowRead, nesMapperLowWrite, 4, tbl5000Bitfields),
   new CRegisterData(0x5001, "Square1 Sweep", nesMapperLowRead, nesMapperLowWrite, 4, tbl5001Bitfields),
   new CRegisterData(0x5002, "Square1 Period LSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl5002Bitfields),
   new CRegisterData(0x5003, "Square1 Length", nesMapperLowRead, nesMapperLowWrite, 2, tbl5003Bitfields),
   new CRegisterData(0x5004, "Square2 Control", nesMapperLowRead, nesMapperLowWrite, 4, tbl5000Bitfields),
   new CRegisterData(0x5005, "Square2 Sweep", nesMapperLowRead, nesMapperLowWrite, 4, tbl5001Bitfields),
   new CRegisterData(0x5006, "Square2 Period LSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl5002Bitfields),
   new CRegisterData(0x5007, "Square2 Length", nesMapperLowRead, nesMapperLowWrite, 2, tbl5003Bitfields),
   new CRegisterData(0x5010, "DMC Control", nesMapperLowRead, nesMapperLowWrite, 3, tbl5010Bitfields),
   new CRegisterData(0x5011, "DMC DAC", nesMapperLowRead, nesMapperLowWrite, 1, tbl5011Bitfields),
   new CRegisterData(0x5015, "APU Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5015Bitfields),
   new CRegisterData(0x5100, "PRG Mode Select", nesMapperLowRead, nesMapperLowWrite, 1, tbl5100Bitfields),
   new CRegisterData(0x5101, "CHR Mode Select", nesMapperLowRead, nesMapperLowWrite, 1, tbl5101Bitfields),
   new CRegisterData(0x5102, "PRG-RAM Write Protect A", nesMapperLowRead, nesMapperLowWrite, 1, tbl5102Bitfields),
   new CRegisterData(0x5103, "PRG-RAM Write Protect B", nesMapperLowRead, nesMapperLowWrite, 1, tbl5103Bitfields),
   new CRegisterData(0x5104, "EXRAM Mode Select", nesMapperLowRead, nesMapperLowWrite, 1, tbl5104Bitfields),
   new CRegisterData(0x5105, "Mirroring Mode Select", nesMapperLowRead, nesMapperLowWrite, 4, tbl5105Bitfields),
   new CRegisterData(0x5106, "Fill Tile", nesMapperLowRead, nesMapperLowWrite, 1, tbl5106Bitfields),
   new CRegisterData(0x5107, "Fill Attribute", nesMapperLowRead, nesMapperLowWrite, 1, tbl5107Bitfields),
   new CRegisterData(0x5113, "$6000 Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5113Bitfields),
   new CRegisterData(0x5114, "PRG Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5114Bitfields),
   new CRegisterData(0x5115, "PRG Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5115Bitfields),
   new CRegisterData(0x5116, "PRG Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5116Bitfields),
   new CRegisterData(0x5117, "PRG Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5117Bitfields),
   new CRegisterData(0x5120, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5120Bitfields),
   new CRegisterData(0x5121, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5121Bitfields),
   new CRegisterData(0x5122, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5122Bitfields),
   new CRegisterData(0x5123, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5123Bitfields),
   new CRegisterData(0x5124, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5124Bitfields),
   new CRegisterData(0x5125, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5125Bitfields),
   new CRegisterData(0x5126, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5126Bitfields),
   new CRegisterData(0x5127, "CHR A Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5127Bitfields),
   new CRegisterData(0x5128, "CHR B Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5128Bitfields),
   new CRegisterData(0x5129, "CHR B Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl5129Bitfields),
   new CRegisterData(0x512A, "CHR B Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl512ABitfields),
   new CRegisterData(0x512B, "CHR B Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl512BBitfields),
   new CRegisterData(0x5130, "CHR High bits", nesMapperLowRead, nesMapperLowWrite, 1, tbl5130Bitfields),
   new CRegisterData(0x5200, "Split Screen Control", nesMapperLowRead, nesMapperLowWrite, 3, tbl5200Bitfields),
   new CRegisterData(0x5201, "Split Screen Vert Scroll", nesMapperLowRead, nesMapperLowWrite, 1, tbl5201Bitfields),
   new CRegisterData(0x5202, "Split Screen CHR Page", nesMapperLowRead, nesMapperLowWrite, 1, tbl5202Bitfields),
   new CRegisterData(0x5203, "IRQ Trigger", nesMapperLowRead, nesMapperLowWrite, 1, tbl5203Bitfields),
   new CRegisterData(0x5204, "IRQ Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5204Bitfields),
   new CRegisterData(0x5205, "8*8 Multiplier", nesMapperLowRead, nesMapperLowWrite, 1, tbl5205Bitfields),
   new CRegisterData(0x5206, "8*8 Multiplier", nesMapperLowRead, nesMapperLowWrite, 1, tbl5206Bitfields),
   new CRegisterData(0x5209, "Timer LSB", nesMapperLowRead, nesMapperLowWrite, 2, tbl5209Bitfields),
   new CRegisterData(0x520A, "Timer MSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl520ABitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "5000", "5001", "5002", "5003", "5004", "5005", "5006", "5007", "5010", "5011", "5015",
   "5100", "5101", "5102", "5103", "5104", "5105", "5106", "5107",
   "5113", "5114", "5115", "5116", "5117",
   "5120", "5121", "5122", "5123", "5124", "5125", "5126", "5127", "5128", "5129", "512A", "512B",
   "5130",
   "5200", "5201", "5202", "5203", "5204", "5205", "5206",
   "5209", "520A"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,46,46,tblRegisters,rowHeadings,columnHeadings);

uint32_t CROMMapper005::m_soundEnableMask = 0xffffffff;

CCodeDataLogger* CNAMETABLEFILLER::LOGGER (uint32_t virtAddr)
{
   return m_bank[0].LOGGER();
}

CCodeDataLogger* CNAMETABLEFILLER::LOGGERATPHYSADDR (uint32_t physAddr)
{
   return m_bank[0].LOGGER();
}

uint8_t CNAMETABLEFILLER::MEM (uint32_t addr)
{
   if ( (addr&MEM_1KB) < 0x3C0 )
   {
      return m_tileFill;
   }
   else
   {
      return m_attrFill;
   }
}

void CNAMETABLEFILLER::MEM (uint32_t addr, uint8_t data)
{
   if ( (addr&MEM_1KB) < 0x3C0 )
   {
      m_tileFill = data;
   }
   else
   {
      m_attrFill = data;
   }
}

uint8_t CNAMETABLEFILLER::MEMATPHYSADDR (uint32_t absAddr)
{
   if ( (absAddr&MEM_1KB) < 0x3C0 )
   {
      return m_tileFill;
   }
   else
   {
      return m_attrFill;
   }
}

void CNAMETABLEFILLER::MEMATPHYSADDR (uint32_t absAddr, uint8_t data)
{
   if ( (absAddr&MEM_1KB) < 0x3C0 )
   {
      m_tileFill = data;
   }
   else
   {
      m_attrFill = data;
   }
}

CROMMapper005::CROMMapper005()
   : CROM(5)
{
   delete m_pEXRAMmemory; // Remove open-bus default
   m_pEXRAMmemory = new CMEMORY(0x5C00,MEM_1KB);
   delete m_pSRAMmemory; // Remove open-bus default
   m_pSRAMmemory = new CMEMORY(0x6000,MEM_8KB,8,4);
   m_pFILLmemory = new CNAMETABLEFILLER();
   memset(m_prgRAM,false,sizeof(m_prgRAM));
   memset(m_reg,0,sizeof(m_reg));
   memset(m_chrReg_a,0,sizeof(m_chrReg_a));
   memset(m_chrReg_b,0,sizeof(m_chrReg_b));
   m_prgRemappable = true;
   m_chrRemappable = true;
}

CROMMapper005::~CROMMapper005()
{
}

void CROMMapper005::RESET ( bool soft )
{   
   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   m_square[0].PARENT(CNES::NES()->CPU()->APU());
   m_square[1].PARENT(CNES::NES()->CPU()->APU());
   m_dmc.PARENT(CNES::NES()->CPU()->APU());

   m_square[0].RESET();
   m_square[1].RESET();
   m_dmc.RESET();

   m_prgMode = 3;
   m_chrMode = 0;
   m_chrHigh = 0;
   m_lastChr = 0;
   m_irqScanline = 0;
   m_irqStatus = 0;

   m_exRamMode = 0;

   m_8x16e = 0;
   m_8x16z = 0;

   m_mult1 = 0xFF;
   m_mult2 = 0xFF;
   m_prod = 0xFE01;

   m_timer = 0x0000;
   m_timerIrq = false;

   m_reg[23] = 0xFF;

   m_prgRAM [ 0 ] = false;
   m_prgRAM [ 1 ] = false;
   m_prgRAM [ 2 ] = false;
   m_prg[0] = 0x80;
   m_prg[1] = 0x80;
   m_prg[2] = 0x80;
   m_prg[3] = 0xFF;
   SETCPU();
   SETPPU();
}

void CROMMapper005::SYNCCPU ( bool write, uint16_t addr, uint8_t data )
{
   if ( write )
   {
      if ( addr == 0x2000 )
      {
         m_8x16z = !!(data&0x20);
      }
      else if ( addr == 0x2001 )
      {
         m_8x16e = (data>>3)&0x3;
      }
   }

   if ( m_timer )
   {
      m_timer--;
      if ( !m_timer )
      {
         CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);
      }
   }

   m_square[0].TIMERTICK();
   m_square[1].TIMERTICK();
   m_dmc.TIMERTICK();
}

void CROMMapper005::SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr )
{
   int32_t scanline = CYCLE_TO_VISY(ppuCycle);
   int32_t rasterX = CYCLE_TO_VISX(ppuCycle);

   m_ppuCycle = ppuCycle;
   m_ppuAddr = ppuAddr;

   SETPPU();

   if ( scanline < 239 )
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

   if ( m_irqEnabled && (m_irqStatus&0x80) && rasterX == 0 )
   {
      CNES::NES()->CPU()->ASSERTIRQ ( eNESSource_Mapper );

      if ( nesIsDebuggable )
      {
         // Check for IRQ breakpoint...
         CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
      }
   }
}

void CROMMapper005::SETCPU ( void )
{
   uint8_t bank;

   switch ( m_prgMode )
   {
   case 3:
      bank = m_prg[0]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[0]&0x80 )
      {
         m_PRGROMmemory.REMAP(0,bank);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0x8000, bank );
      }
      bank = m_prg[1]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[1]&0x80 )
      {
         m_PRGROMmemory.REMAP(1,bank);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0xA000, bank );
      }
      bank = m_prg[2]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[2]&0x80 )
      {
         m_PRGROMmemory.REMAP(2,bank);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0xC000, bank );
      }
      bank = m_prg[3]&0x7f;
      bank %= m_numPrgBanks;
      m_PRGROMmemory.REMAP(3,bank);
      break;
   case 2:
      bank = m_prg[1]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[1]&0x80 )
      {
         m_PRGROMmemory.REMAP(0,(bank&0xFE)+0);
         m_PRGROMmemory.REMAP(1,(bank&0xFE)+1);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0x8000, (bank&0xFE)+0 );
         REMAPSRAM ( 0xA000, (bank&0xFE)+1 );
      }
      bank = m_prg[2]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[2]&0x80 )
      {
         m_PRGROMmemory.REMAP(2,bank);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0xC000, bank );
      }
      bank = m_prg[3]&0x7f;
      bank %= m_numPrgBanks;
      m_PRGROMmemory.REMAP(3,bank);
      break;
   case 1:
      bank = m_prg[1]&0x7f;
      bank %= m_numPrgBanks;
      if ( m_prg[1]&0x80 )
      {
         m_PRGROMmemory.REMAP(0,(bank&0xFE)+0);
         m_PRGROMmemory.REMAP(1,(bank&0xFE)+1);
      }
      else
      {
         bank &= 0x7;
         REMAPSRAM ( 0x8000, (bank&0xFE)+0 );
         REMAPSRAM ( 0xA000, (bank&0xFE)+1 );
      }
      bank = m_prg[3]&0x7f;
      bank %= m_numPrgBanks;
      m_PRGROMmemory.REMAP(2,(bank&0xFE)+0);
      m_PRGROMmemory.REMAP(3,(bank&0xFE)+1);
      break;
   case 0:
      bank = m_prg[3]&0x7f;
      bank %= m_numPrgBanks;
      m_PRGROMmemory.REMAP(0,(bank&0xFC)+0);
      m_PRGROMmemory.REMAP(1,(bank&0xFC)+1);
      m_PRGROMmemory.REMAP(2,(bank&0xFC)+2);
      m_PRGROMmemory.REMAP(3,(bank&0xFC)+3);
      break;
   }
}

void CROMMapper005::SETPPU ( void )
{
   switch ( m_chrMode )
   {
   case 3:
      m_chrBank_a[0] = m_chrReg_a[0];
      m_chrBank_a[1] = m_chrReg_a[1];
      m_chrBank_a[2] = m_chrReg_a[2];
      m_chrBank_a[3] = m_chrReg_a[3];
      m_chrBank_a[4] = m_chrReg_a[4];
      m_chrBank_a[5] = m_chrReg_a[5];
      m_chrBank_a[6] = m_chrReg_a[6];
      m_chrBank_a[7] = m_chrReg_a[7];
      m_chrBank_b[0] = m_chrReg_b[0];
      m_chrBank_b[1] = m_chrReg_b[1];
      m_chrBank_b[2] = m_chrReg_b[2];
      m_chrBank_b[3] = m_chrReg_b[3];
      m_chrBank_b[4] = m_chrReg_b[0];
      m_chrBank_b[5] = m_chrReg_b[1];
      m_chrBank_b[6] = m_chrReg_b[2];
      m_chrBank_b[7] = m_chrReg_b[3];
      break;
   case 2:
      m_chrBank_a[0] = (m_chrReg_a[1]<<1)+0;
      m_chrBank_a[1] = (m_chrReg_a[1]<<1)+1;
      m_chrBank_a[2] = (m_chrReg_a[3]<<1)+0;
      m_chrBank_a[3] = (m_chrReg_a[3]<<1)+1;
      m_chrBank_a[4] = (m_chrReg_a[5]<<1)+0;
      m_chrBank_a[5] = (m_chrReg_a[5]<<1)+1;
      m_chrBank_a[6] = (m_chrReg_a[7]<<1)+0;
      m_chrBank_a[7] = (m_chrReg_a[7]<<1)+1;
      m_chrBank_b[0] = (m_chrReg_b[1]<<1)+0;
      m_chrBank_b[1] = (m_chrReg_b[1]<<1)+1;
      m_chrBank_b[2] = (m_chrReg_b[3]<<1)+0;
      m_chrBank_b[3] = (m_chrReg_b[3]<<1)+1;
      m_chrBank_b[4] = (m_chrReg_b[1]<<1)+0;
      m_chrBank_b[5] = (m_chrReg_b[1]<<1)+1;
      m_chrBank_b[6] = (m_chrReg_b[3]<<1)+0;
      m_chrBank_b[7] = (m_chrReg_b[3]<<1)+1;
      break;
   case 1:
      m_chrBank_a[0] = (m_chrReg_a[3]<<2)+0;
      m_chrBank_a[1] = (m_chrReg_a[3]<<2)+1;
      m_chrBank_a[2] = (m_chrReg_a[3]<<2)+2;
      m_chrBank_a[3] = (m_chrReg_a[3]<<2)+3;
      m_chrBank_a[4] = (m_chrReg_a[7]<<2)+0;
      m_chrBank_a[5] = (m_chrReg_a[7]<<2)+1;
      m_chrBank_a[6] = (m_chrReg_a[7]<<2)+2;
      m_chrBank_a[7] = (m_chrReg_a[7]<<2)+3;
      m_chrBank_b[0] = (m_chrReg_b[3]<<2)+0;
      m_chrBank_b[1] = (m_chrReg_b[3]<<2)+1;
      m_chrBank_b[2] = (m_chrReg_b[3]<<2)+2;
      m_chrBank_b[3] = (m_chrReg_b[3]<<2)+3;
      m_chrBank_b[4] = (m_chrReg_b[3]<<2)+0;
      m_chrBank_b[5] = (m_chrReg_b[3]<<2)+1;
      m_chrBank_b[6] = (m_chrReg_b[3]<<2)+2;
      m_chrBank_b[7] = (m_chrReg_b[3]<<2)+3;
      break;
   case 0:
      m_chrBank_a[0] = (m_chrReg_a[7]<<3)+0;
      m_chrBank_a[1] = (m_chrReg_a[7]<<3)+1;
      m_chrBank_a[2] = (m_chrReg_a[7]<<3)+2;
      m_chrBank_a[3] = (m_chrReg_a[7]<<3)+3;
      m_chrBank_a[4] = (m_chrReg_a[7]<<3)+4;
      m_chrBank_a[5] = (m_chrReg_a[7]<<3)+5;
      m_chrBank_a[6] = (m_chrReg_a[7]<<3)+6;
      m_chrBank_a[7] = (m_chrReg_a[7]<<3)+7;
      m_chrBank_b[0] = (m_chrReg_b[3]<<3)+0;
      m_chrBank_b[1] = (m_chrReg_b[3]<<3)+1;
      m_chrBank_b[2] = (m_chrReg_b[3]<<3)+2;
      m_chrBank_b[3] = (m_chrReg_b[3]<<3)+3;
      m_chrBank_b[4] = (m_chrReg_b[3]<<3)+4;
      m_chrBank_b[5] = (m_chrReg_b[3]<<3)+5;
      m_chrBank_b[6] = (m_chrReg_b[3]<<3)+6;
      m_chrBank_b[7] = (m_chrReg_b[3]<<3)+7;
      break;
   }

   int32_t rasterX = CYCLE_TO_VISX(m_ppuCycle);

   // Sprite fetches
   if ( (rasterX >= 256) &&
        (rasterX < 320) )
   {
      if ( m_8x16z && m_8x16e )
      {
         m_CHRmemory.REMAP(0,m_chrBank_a[0]);
         m_CHRmemory.REMAP(1,m_chrBank_a[1]);
         m_CHRmemory.REMAP(2,m_chrBank_a[2]);
         m_CHRmemory.REMAP(3,m_chrBank_a[3]);
         m_CHRmemory.REMAP(4,m_chrBank_a[4]);
         m_CHRmemory.REMAP(5,m_chrBank_a[5]);
         m_CHRmemory.REMAP(6,m_chrBank_a[6]);
         m_CHRmemory.REMAP(7,m_chrBank_a[7]);
      }
      else if ( m_8x16e )
      {
         m_CHRmemory.REMAP(0,m_chrBank_a[0]);
         m_CHRmemory.REMAP(1,m_chrBank_a[1]);
         m_CHRmemory.REMAP(2,m_chrBank_a[2]);
         m_CHRmemory.REMAP(3,m_chrBank_a[3]);
         m_CHRmemory.REMAP(4,m_chrBank_a[4]);
         m_CHRmemory.REMAP(5,m_chrBank_a[5]);
         m_CHRmemory.REMAP(6,m_chrBank_a[6]);
         m_CHRmemory.REMAP(7,m_chrBank_a[7]);
      }
   }
   // Background fetches
   else
   {
      if ( m_8x16z && m_8x16e )
      {
         m_CHRmemory.REMAP(0,m_chrBank_b[0]);
         m_CHRmemory.REMAP(1,m_chrBank_b[1]);
         m_CHRmemory.REMAP(2,m_chrBank_b[2]);
         m_CHRmemory.REMAP(3,m_chrBank_b[3]);
         m_CHRmemory.REMAP(4,m_chrBank_b[4]);
         m_CHRmemory.REMAP(5,m_chrBank_b[5]);
         m_CHRmemory.REMAP(6,m_chrBank_b[6]);
         m_CHRmemory.REMAP(7,m_chrBank_b[7]);
      }
      else if ( m_8x16e )
      {
         m_CHRmemory.REMAP(0,m_chrBank_a[0]);
         m_CHRmemory.REMAP(1,m_chrBank_a[1]);
         m_CHRmemory.REMAP(2,m_chrBank_a[2]);
         m_CHRmemory.REMAP(3,m_chrBank_a[3]);
         m_CHRmemory.REMAP(4,m_chrBank_a[4]);
         m_CHRmemory.REMAP(5,m_chrBank_a[5]);
         m_CHRmemory.REMAP(6,m_chrBank_a[6]);
         m_CHRmemory.REMAP(7,m_chrBank_a[7]);
      }
   }
}

uint32_t CROMMapper005::HMAPPER ( uint32_t addr )
{
   if ( (m_prgRAM[0]) && (addr < 0xA000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else if ( (m_prgRAM[1]) && (addr >= 0xA000) && (addr < 0xC000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else if ( (m_prgRAM[2]) && (addr >= 0xC000) && (addr < 0xE000) )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else
   {
      return CROM::PRGROM(addr);
   }
}

void CROMMapper005::HMAPPER ( uint32_t addr, uint8_t data )
{
   // if PRG RAM has been mapped in it will already be there...
   // first check to ensure writes allowed...
   if ( !m_wp )
   {
      CROM::SRAMVIRT ( addr, data );
   }
}

uint32_t CROMMapper005::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x5000:
      return m_reg[0];
      break;
   case 0x5001:
      return m_reg[1];
      break;
   case 0x5002:
      return m_reg[2];
      break;
   case 0x5003:
      return m_reg[3];
      break;
   case 0x5004:
      return m_reg[4];
      break;
   case 0x5005:
      return m_reg[5];
      break;
   case 0x5006:
      return m_reg[6];
      break;
   case 0x5007:
      return m_reg[7];
      break;
   case 0x5010:
      return m_reg[8];
      break;
   case 0x5011:
      return m_reg[9];
      break;
   case 0x5015:
      return m_reg[10];
      break;
   case 0x5100:
      return m_reg[11];
      break;
   case 0x5101:
      return m_reg[12];
      break;
   case 0x5102:
      return m_reg[13];
      break;
   case 0x5103:
      return m_reg[14];
      break;
   case 0x5104:
      return m_reg[15];
      break;
   case 0x5105:
      return m_reg[16];
      break;
   case 0x5106:
      return m_reg[17];
      break;
   case 0x5107:
      return m_reg[18];
      break;
   case 0x5113:
      return m_reg[19];
      break;
   case 0x5114:
      return m_reg[20];
      break;
   case 0x5115:
      return m_reg[21];
      break;
   case 0x5116:
      return m_reg[22];
      break;
   case 0x5117:
      return m_reg[23];
      break;
   case 0x5120:
      return m_reg[24];
      break;
   case 0x5121:
      return m_reg[25];
      break;
   case 0x5122:
      return m_reg[26];
      break;
   case 0x5123:
      return m_reg[27];
      break;
   case 0x5124:
      return m_reg[28];
      break;
   case 0x5125:
      return m_reg[29];
      break;
   case 0x5126:
      return m_reg[30];
      break;
   case 0x5127:
      return m_reg[31];
      break;
   case 0x5128:
      return m_reg[32];
      break;
   case 0x5129:
      return m_reg[33];
      break;
   case 0x512A:
      return m_reg[34];
      break;
   case 0x512B:
      return m_reg[35];
      break;
   case 0x5130:
      return m_reg[36];
      break;
   case 0x5200:
      return m_reg[37];
      break;
   case 0x5201:
      return m_reg[38];
      break;
   case 0x5202:
      return m_reg[39];
      break;
   case 0x5203:
      return m_reg[40];
      break;
   case 0x5204:
      return m_reg[41];
      break;
   case 0x5205:
      return m_reg[42];
      break;
   case 0x5206:
      return m_reg[43];
      break;
   case 0x5209:
      return m_reg[44];
      break;
   case 0x520A:
      return m_reg[45];
      break;
   }
   return 0;
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
         CNES::NES()->CPU()->RELEASEIRQ ( eNESSource_Mapper );
         break;
      case 0x5205:
         data = m_prod&0xFF;
         break;
      case 0x5206:
         data = (m_prod>>8)&0xFF;
         break;
      case 0x5209:
         data = m_timerIrq?0x80:0x00;
         CNES::NES()->CPU()->RELEASEIRQ ( eNESSource_Mapper );
         break;
      }
   }

   return data;
}

void CROMMapper005::LMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t prgRAM;
   int32_t sc1 = -1, sc2 = -1, sc3 = -1, sc4 = -1;

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
      case 0x5000:
         m_reg[0] = data;
         m_square[0].APU(0,data);
         break;
      case 0x5001:
         m_reg[1] = data;
         m_square[0].APU(1,data);
         break;
      case 0x5002:
         m_reg[2] = data;
         m_square[0].APU(2,data);
         break;
      case 0x5003:
         m_reg[3] = data;
         m_square[0].APU(3,data);
         break;
      case 0x5004:
         m_reg[4] = data;
         m_square[1].APU(0,data);
         break;
      case 0x5005:
         m_reg[5] = data;
         m_square[1].APU(1,data);
         break;
      case 0x5006:
         m_reg[6] = data;
         m_square[1].APU(2,data);
         break;
      case 0x5007:
         m_reg[7] = data;
         m_square[1].APU(3,data);
         break;
      case 0x5010:
         m_reg[8] = data;
         m_dmc.APU(0,data);
         break;
      case 0x5011:
         m_reg[9] = data;
         m_dmc.APU(1,data);
         break;
      case 0x5015:
         m_reg[10] = data;
         m_square[0].ENABLE(!!(data&0x01));
         m_square[1].ENABLE(!!(data&0x02));
         break;
      case 0x5100:
         m_reg[11] = data;
         m_prgMode = data&0x3;
         SETCPU();
         break;
      case 0x5101:
         m_reg[12] = data;
         m_chrMode = data&0x3;
         SETPPU();
         break;
      case 0x5102:
         m_reg[13] = data;
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
      case 0x5103:
         m_reg[14] = data;
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
      case 0x5104:
         m_reg[15] = data;
         m_exRamMode = data&0x3;
         break;
      case 0x5105:
         m_reg[16] = data;
         m_sc1 = sc1 = data&0x03;
         m_sc2 = sc2 = (data&0x0C)>>2;
         m_sc3 = sc3 = (data&0x30)>>4;
         m_sc4 = sc4 = (data&0xC0)>>6;
         if ( sc1 == 0x2 )
         {
            sc1 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x0, m_pEXRAMmemory->PHYSBANK(0) );
         }
         else if ( sc1 == 0x3 )
         {
            sc1 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x0, m_pFILLmemory->PHYSBANK(0) );
         }
         if ( sc2 == 0x2 )
         {
            sc2 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x1, m_pEXRAMmemory->PHYSBANK(0) );
         }
         else if ( sc2 == 0x3 )
         {
            sc2 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x1, m_pFILLmemory->PHYSBANK(0) );
         }
         if ( sc3 == 0x2 )
         {
            sc3 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x2, m_pEXRAMmemory->PHYSBANK(0) );
         }
         else if ( sc3 == 0x3 )
         {
            sc3 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x2, m_pFILLmemory->PHYSBANK(0) );
         }
         if ( sc4 == 0x2 )
         {
            sc4 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x3, m_pEXRAMmemory->PHYSBANK(0) );
         }
         else if ( sc4 == 0x3 )
         {
            sc4 = -1;
            CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x3, m_pFILLmemory->PHYSBANK(0) );
         }
         CNES::NES()->PPU()->MIRROR ( sc1, sc2, sc3, sc4 );
         break;
      case 0x5106:
         m_reg[17] = data;
         // store tile filler
         m_pFILLmemory->MEM(0x000,data);
         break;
      case 0x5107:
         m_reg[18] = data;
         // store attribute filler
         m_pFILLmemory->MEM(0x3C0,data);
         break;
      case 0x5113:
         m_reg[19] = data;
         data &= 0x7;
         REMAPSRAM ( 0x6000, data );
         break;
      case 0x5114:
         m_reg[20] = data;
         m_prg[0] = data;
         SETCPU();
         break;
      case 0x5115:
         m_reg[21] = data;
         m_prg[1] = data;
         SETCPU();
         break;
      case 0x5116:
         m_reg[22] = data;
         m_prg[2] = data;
         SETCPU();
         break;
      case 0x5117:
         data |= 0x80;
         m_reg[23] = data;
         m_prg[3] = data;
         SETCPU();
         break;
      case 0x5120:
         m_reg[24] = data;
         m_lastChr = 0;
         m_chrReg_a[0] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5121:
         m_reg[25] = data;
         m_lastChr = 0;
         m_chrReg_a[1] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5122:
         m_reg[26] = data;
         m_lastChr = 0;
         m_chrReg_a[2] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5123:
         m_reg[27] = data;
         m_lastChr = 0;
         m_chrReg_a[3] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5124:
         m_reg[28] = data;
         m_lastChr = 0;
         m_chrReg_a[4] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5125:
         m_reg[29] = data;
         m_lastChr = 0;
         m_chrReg_a[5] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5126:
         m_reg[30] = data;
         m_lastChr = 0;
         m_chrReg_a[6] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5127:
         m_reg[31] = data;
         m_lastChr = 0;
         m_chrReg_a[7] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5128:
         m_reg[32] = data;
         m_lastChr = 1;
         m_chrReg_b[0] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5129:
         m_reg[33] = data;
         m_lastChr = 1;
         m_chrReg_b[1] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x512A:
         m_reg[34] = data;
         m_lastChr = 1;
         m_chrReg_b[2] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x512B:
         m_reg[35] = data;
         m_lastChr = 1;
         m_chrReg_b[3] = (m_chrHigh<<8)|data;
         SETPPU();
         break;
      case 0x5130:
         m_reg[36] = data;
         m_chrHigh = data&0x3;
         break;
      case 0x5200:
         m_reg[37] = data;
         break;
      case 0x5201:
         m_reg[38] = data;
         break;
      case 0x5202:
         m_reg[39] = data;
         break;
      case 0x5203:
         m_reg[40] = data;
         m_irqScanline = data;
         break;
      case 0x5204:
         m_reg[41] = data;
         m_irqEnabled = data&0x80;
         break;
      case 0x5205:
         m_reg[42] = data;
         m_mult1 = data;
         m_prod = m_mult1*m_mult2;
         break;
      case 0x5206:
         m_reg[43] = data;
         m_mult2 = data;
         m_prod = m_mult1*m_mult2;
         break;
      case 0x5209:
         m_reg[44] = data;
         m_timer &= 0x00FF;
         m_timer |= data;
         break;
      case 0x520A:
         m_reg[45] = data;
         m_timer &= 0xFF00;
         m_timer |= (data<<8);
         break;
      }
   }
}

uint32_t CROMMapper005::CHRMEM ( uint32_t addr )
{
   if ( m_exRamMode == 1 )
   {
      if ( m_sc1 == 2 || m_sc2 == 2 || m_sc3 == 2 || m_sc4 == 2 )
      {
         return VRAM(addr&MASK_4KB);
      }
   }
   return m_CHRmemory.MEM(addr);
}

uint32_t CROMMapper005::VRAM ( uint32_t addr )
{
   if ( m_exRamMode == 0 )
   {
      if ( m_sc1 == 2 || m_sc2 == 2 || m_sc3 == 2 || m_sc4 == 2 )
      {
         return m_pEXRAMmemory->MEM(addr&MASK_1KB);
      }
      else if ( m_sc1 == 3 || m_sc2 == 3 || m_sc3 == 3 || m_sc4 == 3 )
      {
         return m_pFILLmemory->MEM(addr&MASK_1KB);
      }
   }
   else if ( m_exRamMode == 1 )
   {
      if ( m_sc1 == 2 || m_sc2 == 2 || m_sc3 == 2 || m_sc4 == 2 )
      {
         if ( (addr&0x3C0) < 0x3C0 )
         {
            return CART_UNCLAIMED;
         }
         else
         {
            uint8_t data = m_pEXRAMmemory->MEM(addr&0x3C0);
            return (data&0xC0)|((data&0xC0)>>2)|((data&0xC0)>>4)|((data&0xC0)>>6);
         }
      }
      else if ( m_sc1 == 3 || m_sc2 == 3 || m_sc3 == 3 || m_sc4 == 3 )
      {
         return m_pFILLmemory->MEM(addr&MASK_1KB);
      }
   }
   return CART_UNCLAIMED;
}

uint16_t CROMMapper005::AMPLITUDE ( void )
{
   float famp;
   int16_t amp;
   int16_t delta;
   int16_t out[100] = { 0, };
   static int16_t outLast = 0;
   uint8_t sample;
   uint8_t* sq1dacSamples = m_square[0].GETDACSAMPLES();
   uint8_t* sq2dacSamples = m_square[1].GETDACSAMPLES();
   uint8_t* dmcDacSamples = m_dmc.GETDACSAMPLES();
   static int32_t outDownsampled = 0;

   m_square[0].MUTE(!(m_soundEnableMask&0x01));
   m_square[1].MUTE(!(m_soundEnableMask&0x02));
   m_dmc.MUTE(!(m_soundEnableMask&0x04));

   for ( sample = 0; sample < m_square[0].GETDACSAMPLECOUNT(); sample++ )
   {
//      output = square_out + tnd_out
//
//
//                            95.88
//      square_out = -----------------------
//                          8128
//                   ----------------- + 100
//                   square1 + square2
//
//
//                            159.79
//      tnd_out = ------------------------------
//                            1
//                ------------------------ + 100
//                triangle   noise    dmc
//                -------- + ----- + -----
//                  8227     12241   22638
      famp = 0.0;
      if ( (*(sq1dacSamples+sample))+(*(sq2dacSamples+sample)) )
      {
         famp = (95.88/((8128.0/((*(sq1dacSamples+sample))+(*(sq2dacSamples+sample))))+100.0));
      }
      if ( (*(dmcDacSamples+sample)) )
      {
         famp += (159.79/((1.0/((((*(dmcDacSamples+sample))/22638.0))))+100.0));
      }
      amp = (int16_t)(float)(65535.0*famp*0.50);

      (*(out+sample)) = amp;

      outDownsampled += (*(out+sample));
   }

   outDownsampled = (int32_t)((float)outDownsampled/((float)m_square[0].GETDACSAMPLECOUNT()));

   delta = outDownsampled - outLast;
   outDownsampled = outLast+((delta*65371)/65536); // 65371/65536 is 0.9975 adjusted to 16-bit fixed point.

   outLast = outDownsampled;

   // Reset DAC averaging...
   m_square[0].CLEARDACAVG();
   m_square[1].CLEARDACAVG();
   m_dmc.CLEARDACAVG();

   return outDownsampled;
}
