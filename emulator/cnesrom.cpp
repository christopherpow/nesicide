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
    
// ROM.cpp: implementation of the CROM class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesrom.h"
#include "cnesppu.h"
#include "cnes6502.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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

unsigned char  CROM::m_PRGROMmemory [][ MEM_8KB ] = { { 0, }, };
unsigned char  CROM::m_PRGROMopcodeMask [][ MEM_8KB ] = { { 0, }, };
char*          CROM::m_PRGROMdisassembly [][ MEM_8KB ] = { { 0, }, };
unsigned short CROM::m_PRGROMsloc2addr [][ MEM_8KB ] = { { 0, }, };
unsigned short CROM::m_PRGROMaddr2sloc [][ MEM_8KB ] = { { 0, }, };
unsigned int   CROM::m_PRGROMsloc [] = { 0, };
unsigned char  CROM::m_CHRROMmemory [][ MEM_8KB ] = { { 0, }, };
unsigned char  CROM::m_CHRRAMmemory [ MEM_8KB ] = { 0, };
UINT           CROM::m_PRGROMbank [] = { 0, 0, 0, 0 };
unsigned char* CROM::m_pPRGROMmemory [] = { NULL, NULL, NULL, NULL };
UINT           CROM::m_CHRbank [] = { 0, 1, 2, 3, 4, 5, 6, 7 };
unsigned char* CROM::m_pCHRmemory [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
unsigned char  CROM::m_SRAMmemory [][ MEM_8KB ] = { { 0, }, };
unsigned char  CROM::m_SRAMopcodeMask [][ MEM_8KB ] = { { 0, }, };
char*          CROM::m_SRAMdisassembly [][ MEM_8KB ] = { { 0, }, };
unsigned short CROM::m_SRAMsloc2addr [][ MEM_8KB ] = { { 0, }, };
unsigned short CROM::m_SRAMaddr2sloc [][ MEM_8KB ] = { { 0, }, };
unsigned int   CROM::m_SRAMsloc [] = { 0, };
unsigned char* CROM::m_pSRAMmemory [] = { NULL, NULL, NULL, NULL, NULL };
UINT           CROM::m_SRAMbank [] = { 0, 1, 2, 3, 4 };
CCodeDataLogger* CROM::m_pSRAMLogger [] = { NULL, };
unsigned char  CROM::m_EXRAMmemory [] = { 0, };
unsigned char  CROM::m_EXRAMopcodeMask [ MEM_1KB ] = { 0, };
char*          CROM::m_EXRAMdisassembly [ MEM_1KB ] = { 0, };
unsigned short CROM::m_EXRAMsloc2addr [ MEM_1KB ] = { 0, };
unsigned short CROM::m_EXRAMaddr2sloc [ MEM_1KB ] = { 0, };
unsigned int   CROM::m_EXRAMsloc = 0;
CCodeDataLogger* CROM::m_pEXRAMLogger = NULL;

UINT           CROM::m_mapper = 0; 
UINT           CROM::m_numPrgBanks = 0;
UINT           CROM::m_numChrBanks = 0;

CCodeDataLogger* CROM::m_pLogger [] = { NULL, };
CRegisterData**  CROM::m_tblRegisters = NULL;
int              CROM::m_numRegisters = 0;

CBreakpointEventInfo** CROM::m_tblBreakpointEvents = tblMapperEvents;
int                    CROM::m_numBreakpointEvents = NUM_MAPPER_EVENTS;

static CROM __init __attribute((unused));

CROM::CROM()
{
   int bank;
   int addr;

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

   // Assume identity-mapped SRAM...
   // There are five possible concurrently-visible 8KB
   // SRAM banks in MMC5: (0x6000 - 0xFFFF).  Other
   // cartridges with 8KB of SRAM will leave this mapping alone.
   for ( bank = 0; bank < 5; bank++ )
   {
      m_pSRAMmemory [ bank ] = *(m_SRAMmemory+bank);
   }

   m_pEXRAMLogger = new CCodeDataLogger ( MEM_1KB, MASK_1KB );

   for ( addr = 0; addr < MEM_1KB; addr++ )
   {
      m_EXRAMdisassembly[addr] = new char [ 16 ];
   }

   CROM::RESET ();
}

CROM::~CROM()
{
   int bank;
   int addr;

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

void CROM::Clear16KBanks ()
{
   m_numPrgBanks = 0;
}

void CROM::Set16KBank ( int bank, unsigned char* data )
{
   memcpy ( m_PRGROMmemory[m_numPrgBanks], data, MEM_8KB );
   m_numPrgBanks++;
   memcpy ( m_PRGROMmemory[m_numPrgBanks], data+MEM_8KB, MEM_8KB );
   m_numPrgBanks++;
}

void CROM::Set8KBank ( int bank, unsigned char* data )
{
   memcpy ( m_CHRROMmemory[bank], data, MEM_8KB );
   m_numChrBanks = bank + 1;
}

void CROM::DoneLoadingBanks ()
{
   // This is called when the ROM loader is done so that fixup can be done...
   if ( m_numPrgBanks == 2 )
   {
      // If the ROM contains only one 16KB PRG-ROM bank then it needs to be replicated
      // to the second PRG-ROM bank slot...
      memcpy ( m_PRGROMmemory[2], m_PRGROMmemory[0], MEM_8KB );
      m_numPrgBanks++;
      memcpy ( m_PRGROMmemory[3], m_PRGROMmemory[1], MEM_8KB );
      m_numPrgBanks++;
   }

   // Initial disassembly will be 'crap' but do it anyway...
   DISASSEMBLE();
}

void CROM::DISASSEMBLE ()
{
   unsigned int bank;

   // Disassemble PRG-ROM banks...
   for ( bank = 0; bank < m_numPrgBanks; bank++ )
   {
      C6502::DISASSEMBLE ( m_PRGROMdisassembly[bank],
                           m_PRGROMmemory[bank],
                           MEM_8KB,
                           m_PRGROMopcodeMask[bank],
                           m_PRGROMsloc2addr[bank],
                           m_PRGROMaddr2sloc[bank],
                           &(m_PRGROMsloc[bank]) );
   }
   // Disassemble SRAM...
   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      C6502::DISASSEMBLE ( m_SRAMdisassembly[bank],
                           m_SRAMmemory[bank],
                           MEM_8KB,
                           m_SRAMopcodeMask[bank],
                           m_SRAMsloc2addr[bank],
                           m_SRAMaddr2sloc[bank],
                           &(m_SRAMsloc[bank]) );
   }
   // Disassemble EXRAM...
   C6502::DISASSEMBLE ( m_EXRAMdisassembly,
                        m_EXRAMmemory,
                        MEM_1KB,
                        m_EXRAMopcodeMask,
                        m_EXRAMsloc2addr,
                        m_EXRAMaddr2sloc,
                        &(m_EXRAMsloc) );
}

UINT CROM::PRGROMSLOC2ADDR ( unsigned short sloc )
{
   int sloc8000 = CROM::PRGROMSLOC(0x8000);
   int slocA000 = sloc8000+CROM::PRGROMSLOC(0xA000);
   int slocC000 = slocA000+CROM::PRGROMSLOC(0xC000);
   int slocE000 = slocC000+CROM::PRGROMSLOC(0xE000);
   int addr = 0;

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

   return addr+(*(*(m_PRGROMsloc2addr+PRGBANK_PHYS(addr))+sloc));
}

UINT CROM::SRAMSLOC2ADDR ( unsigned short sloc )
{
   int addr = 0x6000;

   return addr+(*(*(m_SRAMsloc2addr+SRAMBANK_PHYS(addr))+sloc));
}

UINT CROM::EXRAMSLOC2ADDR ( unsigned short sloc )
{
   return 0x5C00+(*(m_EXRAMsloc2addr+sloc));
}

unsigned short CROM::PRGROMADDR2SLOC ( UINT addr )
{
   int sloc8000 = CROM::PRGROMSLOC(0x8000);
   int slocA000 = sloc8000+CROM::PRGROMSLOC(0xA000);
   int slocC000 = slocA000+CROM::PRGROMSLOC(0xC000);
   int sloc;

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

   return sloc+(*(*(m_PRGROMaddr2sloc+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr)));
}

unsigned short CROM::SRAMADDR2SLOC ( UINT addr )
{
   int sloc = 0;

   return sloc+(*(*(m_SRAMaddr2sloc+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr)));
}

unsigned short CROM::EXRAMADDR2SLOC ( UINT addr )
{
   return *(m_EXRAMaddr2sloc+(addr-0x5C00));
}

void CROM::RESET ( void )
{
   RESET ( 0 );
}

void CROM::RESET ( UINT mapper )
{
   int bank;

   m_mapper = mapper;
   m_tblRegisters = NULL;
   m_numRegisters = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_PRGROMbank [ 1 ] = 1;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
   m_PRGROMbank [ 2 ] = 2;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 3 ];
   m_PRGROMbank [ 3 ] = 3;

   // Assume no VROM...point to VRAM...
   for ( bank = 0; bank < 8; bank++ )
   {
      m_pCHRmemory [ bank ] = m_CHRRAMmemory + (bank<<UPSHIFT_1KB);
      m_CHRbank [ bank ] = bank;
   }

   // If the cartridge has VROM, map it instead...
   if ( m_numChrBanks > 0 )
   {
      for ( bank = 0; bank < 8; bank++ )
      {
         m_pCHRmemory [ bank ] = m_CHRROMmemory [ 0 ] + (bank<<UPSHIFT_1KB);
         m_CHRbank [ bank ] = bank;
      }
   }

   // Assume identity-mapped SRAM...
   // There are five possible concurrently-visible 8KB
   // SRAM banks in MMC5: (0x6000 - 0xFFFF).  Other
   // cartridges with 8KB of SRAM will leave this mapping alone.
   for ( bank = 0; bank < 5; bank++ )
   {
      m_pSRAMmemory [ bank ] = *(m_SRAMmemory+bank);
   }

   // Clear Code/Data Logger info...
   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      m_pLogger [ bank ]->ClearData ();
   }
}

void CROM::LOAD ( MapperState* data )
{
   int idx;

   for ( idx = 0; idx < 4; idx++ )
   {
      m_pPRGROMmemory [ idx ] = (unsigned char*)m_PRGROMmemory + data->PRGROMOffset [ idx ];
   }
   for ( idx = 0; idx < 8; idx++ )
   {
      if ( m_numChrBanks )
      {
         m_pCHRmemory [ idx ] = (unsigned char*)m_CHRROMmemory + data->CHRMEMOffset [ idx ];
      }
      else
      {
         m_pCHRmemory [ idx ] = (unsigned char*)m_CHRRAMmemory + data->CHRMEMOffset [ idx ];
      }
   }
}

void CROM::SAVE ( MapperState* data )
{
   int idx;

   for ( idx = 0; idx < 4; idx++ )
   {
      data->PRGROMOffset [ idx ] = m_pPRGROMmemory [ idx ] - (unsigned char*)m_PRGROMmemory;
   }
   for ( idx = 0; idx < 8; idx++ )
   {
      if ( m_numChrBanks )
      {
         data->CHRMEMOffset [ idx ] = m_pCHRmemory [ idx ] - (unsigned char*)m_CHRROMmemory;
      }
      else
      {
         data->CHRMEMOffset [ idx ] = m_pCHRmemory [ idx ] - (unsigned char*)m_CHRRAMmemory;
      }
   }
}
