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

#include "cnesrom.h"
#include "cnesppu.h"
#include "cnes6502.h"

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

CRegisterDatabase*  CROM::m_dbRegisters = NULL;

static CMemoryDatabase* dbSRAMMemory = new CMemoryDatabase(eMemory_cartSRAM,
                                                           SRAM_START,
                                                           MEM_8KB,
                                                           16,
                                                           "Cartridge SRAM Memory",
                                                           nesGetSRAMDataVirtual,
                                                           nesSetSRAMDataVirtual,
                                                           nesGetPrintableAddress,
                                                           true);

CMemoryDatabase* CROM::m_dbSRAMMemory = dbSRAMMemory;

static CMemoryDatabase* dbEXRAMMemory = new CMemoryDatabase(eMemory_cartEXRAM,
                                                            EXRAM_START,
                                                            MEM_1KB,
                                                            16,
                                                            "Cartridge EXRAM Memory",
                                                            nesGetEXRAMData,
                                                            nesSetEXRAMData,
                                                            nesGetPrintableAddress,
                                                            true);

CMemoryDatabase* CROM::m_dbEXRAMMemory = dbEXRAMMemory;

static CMemoryDatabase* dbVRAMMemory = new CMemoryDatabase(eMemory_cartVRAM,
                                                            VRAM_START,
                                                            MEM_16KB,
                                                            16,
                                                            "Cartridge VRAM Memory",
                                                            nesGetVRAMData,
                                                            nesSetVRAMData,
                                                            nesGetPrintableAddress,
                                                            true);

CMemoryDatabase* CROM::m_dbVRAMMemory = dbVRAMMemory;

bool returnFalse() { return false; }

static CMemoryDatabase* dbPRGROMMemory = new CMemoryDatabase(eMemory_cartROM,
                                                             MEM_32KB,
                                                             MEM_32KB,
                                                             16,
                                                             "Cartridge PRGROM Memory",
                                                             nesGetPRGROMData,
                                                             NULL,
                                                             nesGetPrintableAddress,
                                                             true,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             returnFalse);

CMemoryDatabase* CROM::m_dbPRGROMMemory = dbPRGROMMemory;

static CMemoryDatabase* dbCHRMemory = new CMemoryDatabase(eMemory_cartCHRMEM,
                                                          0,
                                                          MEM_8KB,
                                                          16,
                                                          "Cartridge CHR Memory",
                                                          nesGetCHRMEMData,
                                                          nesSetCHRMEMData,
                                                          nesGetPrintableAddress,
                                                          true,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          nesIsCHRRAM);

CMemoryDatabase* CROM::m_dbCHRMemory = dbCHRMemory;

CBreakpointEventInfo** CROM::m_tblBreakpointEvents = tblMapperEvents;
int32_t                CROM::m_numBreakpointEvents = NUM_MAPPER_EVENTS;

uint8_t** CROM::m_PRGROMmemory = NULL;
uint8_t** CROM::m_CHRmemory = NULL;
uint8_t*  CROM::m_VRAMmemory = NULL;
uint8_t*  CROM::m_pPRGROMmemory [] = { NULL, NULL, NULL, NULL };
uint8_t*  CROM::m_pCHRmemory [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
uint8_t*  CROM::m_pVRAMmemory [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
uint8_t** CROM::m_SRAMmemory = NULL;
uint8_t*  CROM::m_pSRAMmemory [] = { NULL, NULL, NULL, NULL, NULL };
uint8_t*  CROM::m_EXRAMmemory = NULL;

uint32_t           CROM::m_mapper = 0;
uint32_t           CROM::m_numPrgBanks = 0;
uint32_t           CROM::m_numChrBanks = 0;

CCodeDataLogger* CROM::m_pLogger [] = { NULL, };
CCodeDataLogger* CROM::m_pEXRAMLogger = NULL;
CCodeDataLogger* CROM::m_pSRAMLogger [] = { NULL, };

uint8_t**  CROM::m_PRGROMopcodeMask = NULL;
bool*      CROM::m_PRGROMopcodeMaskDirty  = NULL;
char***    CROM::m_PRGROMdisassembly = NULL;
uint16_t** CROM::m_PRGROMsloc2addr = NULL;
uint16_t** CROM::m_PRGROMaddr2sloc = NULL;
uint32_t*  CROM::m_PRGROMsloc = NULL;
uint8_t**  CROM::m_SRAMopcodeMask = NULL;
bool*      CROM::m_SRAMopcodeMaskDirty = NULL;
char***    CROM::m_SRAMdisassembly = NULL;
uint16_t** CROM::m_SRAMsloc2addr = NULL;
uint16_t** CROM::m_SRAMaddr2sloc = NULL;
uint32_t*  CROM::m_SRAMsloc = NULL;
bool       CROM::m_SRAMdirty = false;
uint8_t*   CROM::m_EXRAMopcodeMask = NULL;
bool       CROM::m_EXRAMopcodeMaskDirty = false;
char**     CROM::m_EXRAMdisassembly = NULL;
uint16_t*  CROM::m_EXRAMsloc2addr = NULL;
uint16_t*  CROM::m_EXRAMaddr2sloc = NULL;
uint32_t   CROM::m_EXRAMsloc = 0;

static CROM __init __attribute__((unused));

CROM::CROM()
{
   int32_t bank;
   int32_t addr;

   m_PRGROMmemory = new uint8_t*[NUM_ROM_BANKS];
   m_PRGROMdisassembly = new char**[NUM_ROM_BANKS];
   m_PRGROMopcodeMaskDirty = new bool[NUM_ROM_BANKS];
   m_PRGROMopcodeMask = new uint8_t*[NUM_ROM_BANKS];
   m_PRGROMsloc2addr = new uint16_t*[NUM_ROM_BANKS];
   m_PRGROMaddr2sloc = new uint16_t*[NUM_ROM_BANKS];
   m_PRGROMsloc = new uint32_t[NUM_ROM_BANKS];
   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      m_PRGROMmemory[bank] = new uint8_t[MEM_8KB+1]; // Leave room for bank ID.
      m_PRGROMdisassembly[bank] = new char*[MEM_8KB];
      m_PRGROMopcodeMaskDirty[bank] = true;
      m_PRGROMopcodeMask[bank] = new uint8_t[MEM_8KB];
      m_PRGROMsloc2addr[bank] = new uint16_t[MEM_8KB];
      m_PRGROMaddr2sloc[bank] = new uint16_t[MEM_8KB];
      m_PRGROMsloc[bank] = 0;
      m_pLogger [ bank ] = new CCodeDataLogger ( MEM_8KB, MASK_8KB );

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         m_PRGROMdisassembly[bank][addr] = new char [ 16 ];
         m_PRGROMopcodeMask[bank][addr] = 0;
         m_PRGROMsloc2addr[bank][addr] = 0;
         m_PRGROMaddr2sloc[bank][addr] = 0;
      }

      // Store bank ID in bank data at the end.  This is used only
      // by code that needs to calculate absolute address stuff.
      // Since the banks are stored non-contiguously this is a cheap
      // way to get the bank ID without having to implement a structure.
      m_PRGROMmemory[bank][MEM_8KB] = bank;
   }

   m_SRAMmemory = new uint8_t*[NUM_SRAM_BANKS];
   m_SRAMdisassembly = new char**[NUM_SRAM_BANKS];
   m_SRAMopcodeMaskDirty = new bool[NUM_SRAM_BANKS];
   m_SRAMopcodeMask = new uint8_t*[NUM_SRAM_BANKS];
   m_SRAMsloc2addr = new uint16_t*[NUM_SRAM_BANKS];
   m_SRAMaddr2sloc = new uint16_t*[NUM_SRAM_BANKS];
   m_SRAMsloc = new uint32_t[NUM_SRAM_BANKS];
   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      m_SRAMmemory[bank] = new uint8_t[MEM_8KB+1]; // Leave room for bank ID.
      m_SRAMdisassembly[bank] = new char*[MEM_8KB];
      m_SRAMopcodeMaskDirty[bank] = true;
      m_SRAMopcodeMask[bank] = new uint8_t[MEM_8KB];
      m_SRAMsloc2addr[bank] = new uint16_t[MEM_8KB];
      m_SRAMaddr2sloc[bank] = new uint16_t[MEM_8KB];
      m_SRAMsloc[bank] = 0;
      m_pSRAMLogger [ bank ] = new CCodeDataLogger ( MEM_8KB, MASK_8KB );

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         m_SRAMdisassembly[bank][addr] = new char [ 16 ];
         m_SRAMopcodeMask[bank][addr] = 0;
         m_SRAMsloc2addr[bank][addr] = 0;
         m_SRAMaddr2sloc[bank][addr] = 0;
      }

      // Store bank ID in bank data at the end.  This is used only
      // by code that needs to calculate absolute address stuff.
      // Since the banks are stored non-contiguously this is a cheap
      // way to get the bank ID without having to implement a structure.
      m_SRAMmemory[bank][MEM_8KB] = bank;
   }

   m_EXRAMmemory = new uint8_t[MEM_1KB];
   m_EXRAMdisassembly = new char*[MEM_1KB];
   m_EXRAMopcodeMask = new uint8_t[MEM_1KB];
   m_EXRAMsloc2addr = new uint16_t[MEM_1KB];
   m_EXRAMaddr2sloc = new uint16_t[MEM_1KB];
   m_pEXRAMLogger = new CCodeDataLogger ( MEM_1KB, MASK_1KB );
   for ( addr = 0; addr < MEM_1KB; addr++ )
   {
      m_EXRAMdisassembly[addr] = new char [ 16 ];
      m_EXRAMopcodeMask[addr] = 0;
      m_EXRAMsloc2addr[addr] = 0;
      m_EXRAMaddr2sloc[addr] = 0;
   }

   m_VRAMmemory = new uint8_t[MEM_16KB]; // GTROM mapper 111 has 16KB remappable here

   m_CHRmemory = new uint8_t*[NUM_CHR_BANKS];
   for ( bank = 0; bank < NUM_CHR_BANKS; bank++ )
   {
      m_CHRmemory[bank] = new uint8_t[MEM_1KB+1]; // Leave room for bank ID.

      // Store bank ID in bank data at the end.  This is used only
      // by code that needs to calculate absolute address stuff.
      // Since the banks are stored non-contiguously this is a cheap
      // way to get the bank ID without having to implement a structure.
      m_CHRmemory[bank][MEM_1KB] = bank;
   }

   // Assume identity-mapped SRAM...
   // There are five possible concurrently-visible 8KB
   // SRAM banks in MMC5: (0x6000 - 0xFFFF).  Other
   // cartridges with 8KB of SRAM will leave this mapping alone.
   for ( bank = 0; bank < 5; bank++ )
   {
      m_pSRAMmemory [ bank ] = *(m_SRAMmemory+bank);
   }

   CROM::RESET ( false );
}

CROM::~CROM()
{
   int32_t bank;
   int32_t addr;

   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      delete m_pLogger [ bank ];

      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         delete m_PRGROMdisassembly[bank][addr];
      }
      delete [] m_PRGROMmemory[bank];
      delete [] m_PRGROMopcodeMask[bank];
      delete [] m_PRGROMsloc2addr[bank];
      delete [] m_PRGROMaddr2sloc[bank];
   }
   delete [] m_PRGROMopcodeMaskDirty;
   delete [] m_PRGROMmemory;
   delete [] m_PRGROMopcodeMask;
   delete [] m_PRGROMsloc2addr;
   delete [] m_PRGROMaddr2sloc;
   delete [] m_PRGROMsloc;

   for ( bank = 0; bank < NUM_CHR_BANKS; bank++ )
   {
      delete [] m_CHRmemory[bank];
   }
   delete [] m_CHRmemory;

   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      for ( addr = 0; addr < MEM_8KB; addr++ )
      {
         delete m_SRAMdisassembly[bank][addr];
      }
      delete [] m_SRAMmemory[bank];
      delete [] m_SRAMopcodeMask[bank];
      delete [] m_SRAMsloc2addr[bank];
      delete [] m_SRAMaddr2sloc[bank];
   }
   delete [] m_SRAMopcodeMaskDirty;
   delete [] m_SRAMmemory;
   delete [] m_SRAMopcodeMask;
   delete [] m_SRAMsloc2addr;
   delete [] m_SRAMaddr2sloc;
   delete [] m_SRAMsloc;

   delete [] m_VRAMmemory;

   for ( addr = 0; addr < MEM_1KB; addr++ )
   {
      delete m_EXRAMdisassembly[addr];
   }
   delete [] m_EXRAMmemory;
   delete [] m_EXRAMopcodeMask;
   delete [] m_EXRAMsloc2addr;
   delete [] m_EXRAMaddr2sloc;
}

void CROM::SetPRGBank ( int32_t bank, uint8_t* data )
{
   memcpy ( m_PRGROMmemory[m_numPrgBanks], data, MEM_8KB );
   m_numPrgBanks++;
}

void CROM::SetCHRBank ( int32_t bank, uint8_t* data )
{
   uint8_t ibank;
   for ( ibank = 0; ibank < 8; ibank++ )
   {
      memcpy ( m_CHRmemory[(bank<<3)+ibank], data+(ibank*MEM_1KB), MEM_1KB );
   }
   m_numChrBanks = bank + 1;
}

void CROM::DoneLoadingBanks ()
{
   // This is called when the ROM loader is done so that fixup can be done...
   if ( m_numPrgBanks == 2 )
   {
      // If the ROM contains only one 16KB PRG-ROM bank then it needs to be replicated
      // to the second PRG-ROM bank slot...
      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 0 ];
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 1 ];
   }

   if ( nesIsDebuggable() )
   {
      // Initial disassembly will be 'crap' but do it anyway...
      DISASSEMBLE();
   }

   // We just put a cartridge in...the SRAM isn't dirty yet.
   m_SRAMdirty = false;
}

void CROM::RESET ( bool soft )
{
   RESET ( 0, soft );
}

void CROM::RESET ( uint32_t mapper, bool soft )
{
   int32_t bank;

   m_mapper = mapper;

   if ( mapper == 0 )
   {
      m_dbRegisters = NULL;
   }

   if ( nesIsDebuggable() )
   {
      // Clear Code/Data Logger info...
      for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
      {
         m_pLogger [ bank ]->ClearData ();
      }
   }

   // Support for NROM-368 for Shiru and crew.
   if ( (m_mapper == 0) && (m_numPrgBanks > 4) )
   {
      // A NROM-368 file is laid out linearly so the six 8KB PRG-ROM banks will be
      // loaded into the following memory addresses:
      // 0: $4000-$5fff
      // 1: $6000-$7fff
      // 2: $8000-$9fff
      // 3: $a000-$bfff
      // 4: $c000-$dfff
      // 5: $e000-$ffff
      // The code here takes care of shifting the PRG-ROM visible window from $8000-$ffff
      // so that the CPU sees the upper end of the linear memory there.  The lower end
      // from $4000-$7fff will be mapped in by the existing mapper "low read" APIs called
      // whenever the CPU reads from $4018-$7fff.
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 2 ];
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 3 ];

      m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 4 ];
      m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 5 ];
   }
   else
   {
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];

      // If the ROM contains only one 16KB PRG-ROM bank then it needs to be replicated
      // to the second PRG-ROM bank slot...
      if ( m_numPrgBanks == 2 )
      {
         m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 0 ];
         m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 1 ];
      }
      else
      {
         m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
         m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 3 ];
      }
   }

   // Default CHR map...
   for ( bank = 0; bank < 8; bank++ )
   {
      m_pCHRmemory [ bank ] = m_CHRmemory [ bank ];
   }

   // Default VRAM map...
   for ( bank = 0; bank < 8; bank++ )
   {
      m_pVRAMmemory [ bank ] = m_VRAMmemory+(bank*MEM_1KB);
   }

   // Assume identity-mapped SRAM...
   // There are five possible concurrently-visible 8KB
   // SRAM banks in MMC5: (0x6000 - 0xFFFF).  Other
   // cartridges with 8KB of SRAM will leave this mapping alone.
   for ( bank = 0; bank < 5; bank++ )
   {
      m_pSRAMmemory [ bank ] = *(m_SRAMmemory+bank);
   }
}

uint32_t CROM::LMAPPER ( uint32_t addr )
{
   uint8_t data = C6502::OPENBUS();

   if ( (m_mapper == 0) && (m_numPrgBanks > 4) )
   {
      addr -= MEM_16KB;
      return *(*(m_PRGROMmemory+(PRGBANK_VIRT(addr)))+(PRGBANK_OFF(addr)));
   }
   else
   {
      if ( addr >= 0x6000 )
      {
         data = CROM::SRAMVIRT(addr);
      }
   }

   return data;
}

void CROM::LMAPPER ( uint32_t addr, uint8_t data )
{
   if ( addr >= 0x6000 )
   {
      CROM::SRAMVIRT(addr,data);
   }
}

void CROM::DISASSEMBLE ()
{
   uint32_t bank;

   // Disassemble PRG-ROM banks...
   for ( bank = 0; bank < m_numPrgBanks; bank++ )
   {
      if ( *(m_PRGROMopcodeMaskDirty+bank) )
      {
         C6502::DISASSEMBLE ( m_PRGROMdisassembly[bank],
                              m_PRGROMmemory[bank],
                              MEM_8KB,
                              m_PRGROMopcodeMask[bank],
                              m_PRGROMsloc2addr[bank],
                              m_PRGROMaddr2sloc[bank],
                              &(m_PRGROMsloc[bank]) );

         *(m_PRGROMopcodeMaskDirty+bank) = false;
      }
   }

   // Disassemble SRAM...
   for ( bank = 0; bank < NUM_SRAM_BANKS; bank++ )
   {
      if ( *(m_SRAMopcodeMaskDirty+bank) )
      {
         C6502::DISASSEMBLE ( m_SRAMdisassembly[bank],
                              m_SRAMmemory[bank],
                              MEM_8KB,
                              m_SRAMopcodeMask[bank],
                              m_SRAMsloc2addr[bank],
                              m_SRAMaddr2sloc[bank],
                              &(m_SRAMsloc[bank]) );

         *(m_SRAMopcodeMaskDirty+bank) = false;
      }
   }

   // Disassemble EXRAM...
   if ( m_EXRAMopcodeMaskDirty )
   {
      C6502::DISASSEMBLE ( m_EXRAMdisassembly,
                           m_EXRAMmemory,
                           MEM_1KB,
                           m_EXRAMopcodeMask,
                           m_EXRAMsloc2addr,
                           m_EXRAMaddr2sloc,
                           &(m_EXRAMsloc) );

      m_EXRAMopcodeMaskDirty = false;
   }
}

uint32_t CROM::PRGROMSLOC2ADDR ( uint16_t sloc )
{
   int32_t sloc8000 = PRGROMSLOC(0x8000);
   int32_t slocA000 = sloc8000+PRGROMSLOC(0xA000);
   int32_t slocC000 = slocA000+PRGROMSLOC(0xC000);
   int32_t slocE000 = slocC000+PRGROMSLOC(0xE000);
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

   return addr+(*(*(m_PRGROMsloc2addr+PRGBANK_PHYS(addr))+sloc));
}

uint32_t CROM::SRAMSLOC2ADDR ( uint16_t sloc )
{
   int32_t addr = 0x6000;

   return addr+(*(*(m_SRAMsloc2addr+SRAMBANK_PHYS(addr))+sloc));
}

uint32_t CROM::EXRAMSLOC2ADDR ( uint16_t sloc )
{
   return 0x5C00+(*(m_EXRAMsloc2addr+sloc));
}

uint16_t CROM::PRGROMADDR2SLOC ( uint32_t addr )
{
   int32_t sloc8000 = PRGROMSLOC(0x8000);
   int32_t slocA000 = sloc8000+PRGROMSLOC(0xA000);
   int32_t slocC000 = slocA000+PRGROMSLOC(0xC000);
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

   return sloc+(*(*(m_PRGROMaddr2sloc+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr)));
}

uint16_t CROM::SRAMADDR2SLOC ( uint32_t addr )
{
   int32_t sloc = 0;

   return sloc+(*(*(m_SRAMaddr2sloc+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr)));
}

uint16_t CROM::EXRAMADDR2SLOC ( uint32_t addr )
{
   return *(m_EXRAMaddr2sloc+(addr-0x5C00));
}
