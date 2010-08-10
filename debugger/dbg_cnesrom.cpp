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

#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"

#include "emulator_core.h"

// Mapper Event breakpoints
bool mapperIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* tblMapperEvents [] =
{
   new CBreakpointEventInfo("IRQ", mapperIRQEvent, 0, "Break if mapper asserts IRQ", 10),
};

uint8_t  CROMDBG::m_PRGROMopcodeMask [][ MEM_8KB ] = { { 0, }, };
char*          CROMDBG::m_PRGROMdisassembly [][ MEM_8KB ] = { { 0, }, };
uint16_t CROMDBG::m_PRGROMsloc2addr [][ MEM_8KB ] = { { 0, }, };
uint16_t CROMDBG::m_PRGROMaddr2sloc [][ MEM_8KB ] = { { 0, }, };
uint32_t   CROMDBG::m_PRGROMsloc [] = { 0, };
uint8_t  CROMDBG::m_SRAMopcodeMask [][ MEM_8KB ] = { { 0, }, };
char*          CROMDBG::m_SRAMdisassembly [][ MEM_8KB ] = { { 0, }, };
uint16_t CROMDBG::m_SRAMsloc2addr [][ MEM_8KB ] = { { 0, }, };
uint16_t CROMDBG::m_SRAMaddr2sloc [][ MEM_8KB ] = { { 0, }, };
uint32_t   CROMDBG::m_SRAMsloc [] = { 0, };
CCodeDataLogger* CROMDBG::m_pSRAMLogger [] = { NULL, };
uint8_t  CROMDBG::m_EXRAMopcodeMask [ MEM_1KB ] = { 0, };
char*          CROMDBG::m_EXRAMdisassembly [ MEM_1KB ] = { 0, };
uint16_t CROMDBG::m_EXRAMsloc2addr [ MEM_1KB ] = { 0, };
uint16_t CROMDBG::m_EXRAMaddr2sloc [ MEM_1KB ] = { 0, };
uint32_t   CROMDBG::m_EXRAMsloc = 0;
CCodeDataLogger* CROMDBG::m_pEXRAMLogger = NULL;

CCodeDataLogger* CROMDBG::m_pLogger [] = { NULL, };
CRegisterData**  CROMDBG::m_tblRegisters = NULL;
int              CROMDBG::m_numRegisters = 0;

CBreakpointEventInfo** CROMDBG::m_tblBreakpointEvents = tblMapperEvents;
int                    CROMDBG::m_numBreakpointEvents = NUM_MAPPER_EVENTS;

static CROMDBG __init __attribute((unused));

CROMDBG::CROMDBG()
{
   int32_t bank;
   int32_t addr;

   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      m_pLogger [ bank ] = new CCodeDataLogger ( MEM_8KB, MASK_8KB );

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         m_PRGROMdisassembly[bank][addr] = new char [ 16 ];
      }
   }

   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      m_pSRAMLogger [ bank ] = new CCodeDataLogger ( MEM_8KB, MASK_8KB );

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         m_SRAMdisassembly[bank][addr] = new char [ 16 ];
      }
   }

   m_pEXRAMLogger = new CCodeDataLogger ( MEM_1KB, MASK_1KB );

   for ( addr = 0; addr < MEM_1KB; addr++ )
   {
      m_EXRAMdisassembly[addr] = new char [ 16 ];
   }

   CROMDBG::RESET ();
}

CROMDBG::~CROMDBG()
{
   int32_t bank;
   int32_t addr;

   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      delete m_pLogger [ bank ];

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         delete [] m_PRGROMdisassembly[bank][addr];
      }
   }

   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         delete [] m_SRAMdisassembly[bank][addr];
      }
   }

   for ( addr = 0; addr < MEM_1KB; addr++ )
   {
      delete [] m_EXRAMdisassembly[addr];
   }
}

void CROMDBG::DISASSEMBLE ()
{
   uint32_t bank;

   // Disassemble PRG-ROM banks...
   for ( bank = 0; bank < nesGetNumPRGROMBanks(); bank++ )
   {
      C6502DBG::DISASSEMBLE ( m_PRGROMdisassembly[bank],
                           nesGetPRGROMMemory(bank*MEM_8KB),
                           MEM_8KB,
                           m_PRGROMopcodeMask[bank],
                           m_PRGROMsloc2addr[bank],
                           m_PRGROMaddr2sloc[bank],
                           &(m_PRGROMsloc[bank]) );
   }
   // Disassemble SRAM...
   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      C6502DBG::DISASSEMBLE ( m_SRAMdisassembly[bank],
                           nesGetSRAMMemory(bank*MEM_8KB),
                           MEM_8KB,
                           m_SRAMopcodeMask[bank],
                           m_SRAMsloc2addr[bank],
                           m_SRAMaddr2sloc[bank],
                           &(m_SRAMsloc[bank]) );
   }
   // Disassemble EXRAM...
   C6502DBG::DISASSEMBLE ( m_EXRAMdisassembly,
                        nesGetEXRAMMemory(),
                        MEM_1KB,
                        m_EXRAMopcodeMask,
                        m_EXRAMsloc2addr,
                        m_EXRAMaddr2sloc,
                        &(m_EXRAMsloc) );
}

uint32_t CROMDBG::PRGROMSLOC2ADDR ( uint16_t sloc )
{
   int32_t sloc8000 = CROMDBG::PRGROMSLOC(0x8000);
   int32_t slocA000 = sloc8000+CROMDBG::PRGROMSLOC(0xA000);
   int32_t slocC000 = slocA000+CROMDBG::PRGROMSLOC(0xC000);
   int32_t slocE000 = slocC000+CROMDBG::PRGROMSLOC(0xE000);
   int32_t addr = 0;

   if ( sloc < sloc8000 )
   {
      addr = 0x8000;
   }
   else if ( sloc < slocA000 )
   {
      addr = 0xA000;
      sloc -= sloc8000;
   }
   else if ( sloc < slocC000 )
   {
      addr = 0xC000;
      sloc -= slocA000;
   }
   else if ( sloc < slocE000 )
   {
      addr = 0xE000;
      sloc -= slocC000;
   }

   return addr+(*(*(m_PRGROMsloc2addr+nesGetPhysicalPRGROMBank(addr))+sloc));
}

uint32_t CROMDBG::SRAMSLOC2ADDR ( uint16_t sloc )
{
   int32_t addr = 0x6000;

   return addr+(*(*(m_SRAMsloc2addr+nesGetPhysicalSRAMBank(addr))+sloc));
}

uint32_t CROMDBG::EXRAMSLOC2ADDR ( uint16_t sloc )
{
   return 0x5C00+(*(m_EXRAMsloc2addr+sloc));
}

uint16_t CROMDBG::PRGROMADDR2SLOC ( uint32_t addr )
{
   int32_t sloc8000 = CROMDBG::PRGROMSLOC(0x8000);
   int32_t slocA000 = sloc8000+CROMDBG::PRGROMSLOC(0xA000);
   int32_t slocC000 = slocA000+CROMDBG::PRGROMSLOC(0xC000);
   int32_t sloc;

   if ( addr < 0xA000 )
   {
      sloc = 0;
   }
   else if ( addr < 0xC000 )
   {
      sloc = sloc8000;
   }
   else if ( addr < 0xE000 )
   {
      sloc = slocA000;
   }
   else
   {
      sloc = slocC000;
   }

   return sloc+(*(*(m_PRGROMaddr2sloc+nesGetPhysicalPRGROMBank(addr))+PRGBANK_OFF(addr)));
}

uint16_t CROMDBG::SRAMADDR2SLOC ( uint32_t addr )
{
   int32_t sloc = 0;

   return sloc+(*(*(m_SRAMaddr2sloc+nesGetPhysicalSRAMBank(addr))+SRAMBANK_OFF(addr)));
}

uint16_t CROMDBG::EXRAMADDR2SLOC ( uint32_t addr )
{
   return *(m_EXRAMaddr2sloc+(addr-0x5C00));
}

void CROMDBG::RESET ( void )
{
   RESET ( 0 );
}

void CROMDBG::RESET ( uint32_t mapper )
{
}
