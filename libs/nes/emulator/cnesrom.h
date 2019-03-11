#if !defined ( ROM_H )
#define ROM_H

#include "nes_emulator_core.h"

#include "cnesmappers.h"
#include "cnes.h"
#include "cnes6502.h"

#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cmemorydata.h"

// Resolve a 6502-address to one of 4 8KB PRG ROM banks [0:$8000-$9FFF, 1:$A000-$BFFF, 2:$C000-$DFFF, or 3:$E000-$FFFF]
#define PRGBANK_VIRT(addr) ( (addr&MASK_32KB)>>SHIFT_32KB_8KB )
// Retrieve the bank-offset address portion of a 6502-address for use within PRG ROM banks
#define PRGBANK_OFF(addr) ( addr&MASK_8KB )
// Resolve a 6502-address to one of the 8KB PRG ROM banks within a ROM file [the absolute physical address]
#define PRGBANK_PHYS(addr) ( *((*(m_pPRGROMmemory+PRGBANK_VIRT(addr)))+MEM_8KB) )

// Resolve an absolute address to a PRG-ROM bank
#define PRGBANK_ABSBANK(absAddr) ( absAddr>>SHIFT_32KB_8KB )

// Resolve an address to one of 8 1KB CHR memory banks [0:$0000-$03FF, 1:$0400-$07FF, 2:$0800-$0BFF, 3:$0C00-$0FFF, 4:$1000-$13FF, 5:$1400-$17FF, 6:$1800-$1BFF, or 7:$1C00-$1FFF]
#define CHRBANK_VIRT(addr) ( (addr&MASK_8KB)>>SHIFT_8KB_1KB )
// Retrieve the bank-offset address portion of an address for use within CHR memory banks
#define CHRBANK_OFF(addr) ( addr&MASK_1KB )
// Resolve an address to one of the 8KB CHR memory banks [the absolute physical address]
#define CHRBANK_PHYS(addr) ( *((*(m_pCHRmemory+CHRBANK_VIRT(addr)))+MEM_1KB) )

#define SRAMBANK_VIRT(addr) ( ((addr-SRAM_START)&MASK_64KB)>>SHIFT_64KB_8KB )
#define SRAMBANK_OFF(addr) ( addr&MASK_8KB )
// Resolve a 6502-address to one of the 8KB SRAM banks within a cartridge [the absolute physical address]
// NOTE: Mappers with SRAM typically have one 8KB bank at $6000-$7FFF but MMC5
// allows mapping of up to 64KB of SRAM anywhere in the upper 32KB ROM space.
//#define SRAMBANK_PHYS(addr) ( ((uint8_t*)(*(m_pSRAMmemory+SRAMBANK_VIRT(addr)))-(uint8_t*)m_SRAMmemory)>>SHIFT_64KB_8KB )
#define SRAMBANK_PHYS(addr) ( *((*(m_pSRAMmemory+SRAMBANK_VIRT(addr)))+MEM_8KB) )

#define SRAMBANK_ABSBANK(absAddr) ( absAddr>>SHIFT_64KB_8KB )

class CROM
{
public:
   CROM();
   ~CROM();

   // Priming interfaces (data setup/initialization)
   static void ClearPRGBanks ()
   {
      m_numPrgBanks = 0;
   }
   static void ClearCHRBanks ()
   {
      m_numChrBanks = 0;
   }
   static void SetCHRBank ( int32_t bank, uint8_t* data );
   static void SetPRGBank ( int32_t bank, uint8_t* data );
   static void DoneLoadingBanks ( void );
   static uint32_t NUMPRGROMBANKS ( void )
   {
      return m_numPrgBanks;
   }
   static uint32_t NUMCHRROMBANKS ( void )
   {
      return m_numChrBanks;
   }

   // Operations
   static bool IsWriteProtected ( void )
   {
      return (m_numChrBanks>0);
   }
   static uint32_t CHRMEMABSADDR ( uint32_t addr )
   {
      return (CHRBANK_PHYS(addr)*MEM_1KB)+CHRBANK_OFF(addr);
   }
   static inline uint32_t PRGROMABSADDR ( uint32_t addr )
   {
      return (PRGBANK_PHYS(addr)*MEM_8KB)+PRGBANK_OFF(addr);
   }
   static inline uint32_t PRGROM ( uint32_t addr )
   {
      return *(*(m_pPRGROMmemory+PRGBANK_VIRT(addr))+(PRGBANK_OFF(addr)));
   }
   static inline void PRGROM ( uint32_t, uint8_t ) {}
   static inline uint32_t PRGROMPHYS ( uint32_t addr )
   {
      return *(*(m_PRGROMmemory+PRGBANK_ABSBANK(addr))+PRGBANK_OFF(addr));
   }
   static inline void CHRMEM ( uint32_t addr, uint8_t data )
   {
      *(*(m_pCHRmemory+CHRBANK_VIRT(addr))+CHRBANK_OFF(addr)) = data;
   }
   static inline uint32_t CHRMEM ( uint32_t addr )
   {
      return *(*(m_pCHRmemory+CHRBANK_VIRT(addr))+CHRBANK_OFF(addr));
   }
   static inline uint32_t SRAMABSADDR ( uint32_t addr )
   {
      return (SRAMBANK_PHYS(addr)*MEM_8KB)+SRAMBANK_OFF(addr);
   }
   static inline uint32_t SRAMVIRT ( uint32_t addr )
   {
      return *(*(m_pSRAMmemory+SRAMBANK_VIRT(addr))+SRAMBANK_OFF(addr));
   }
   static inline void SRAMVIRT ( uint32_t addr, uint8_t data )
   {
      *(*(m_pSRAMmemory+SRAMBANK_VIRT(addr))+SRAMBANK_OFF(addr)) = data;
      m_SRAMdirty = true;
   }
   static inline uint32_t SRAMPHYS ( uint32_t addr )
   {
      return *(*(m_SRAMmemory+SRAMBANK_ABSBANK(addr))+SRAMBANK_OFF(addr));
   }
   static inline void SRAMPHYS ( uint32_t addr, uint8_t data, bool setDirty = true )
   {
      *(*(m_SRAMmemory+SRAMBANK_ABSBANK(addr))+SRAMBANK_OFF(addr)) = data;
      if ( setDirty )
      {
         m_SRAMdirty = true;
      }
   }
   static inline void REMAPSRAM ( uint32_t addr, uint8_t bank )
   {
      *(m_pSRAMmemory+SRAMBANK_VIRT(addr)) = *(m_SRAMmemory+bank);
   }
   static inline uint32_t EXRAMABSADDR ( uint32_t addr )
   {
      return (addr%MEM_1KB);
   }
   static inline uint32_t EXRAM ( uint32_t addr )
   {
      return *(m_EXRAMmemory+(addr-EXRAM_START));
   }
   static inline void EXRAM ( uint32_t addr, uint8_t data )
   {
      *(m_EXRAMmemory+(addr-EXRAM_START)) = data;
   }
   static inline uint32_t VRAM ( uint32_t addr )
   {
      return *(m_VRAMmemory+(addr-VRAM_START));
   }
   static inline void VRAM ( uint32_t addr, uint8_t data )
   {
      *(m_VRAMmemory+(addr-VRAM_START)) = data;
   }
   static inline void REMAPVRAM ( uint32_t bank, uint32_t newBank )
   {
      m_pVRAMmemory[bank] = m_VRAMmemory+(newBank*MEM_1KB);
   }

   // Mapper interfaces [called by emulator through mapperfunc array]
   static void RESET ( bool soft );
   static void RESET ( uint32_t mapper, bool soft );
   static uint32_t MAPPER ( void )
   {
      return m_mapper;
   }
   static uint32_t HMAPPER ( uint32_t addr )
   {
      return PRGROM(addr);
   }
   static void HMAPPER ( uint32_t, uint8_t ) {}
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data);
   static void SYNCPPU ( uint32_t, uint32_t ) {}
   static void SYNCCPU ( void ) {}
   static uint32_t DEBUGINFO ( uint32_t addr )
   {
      return PRGROM(addr);
   }
   static uint16_t AMPLITUDE ( void )
   {
      return 0; // soundless...
   }
   static void SOUNDENABLE ( uint32_t mask ) {}

   // Code/Data logger support functions
   static inline CCodeDataLogger* LOGGERVIRT ( uint32_t addr )
   {
      return *(m_pLogger+PRGBANK_PHYS(addr));
   }
   static inline CCodeDataLogger* SRAMLOGGERVIRT ( uint32_t addr )
   {
      return *(m_pSRAMLogger+SRAMBANK_PHYS(addr));
   }
   static inline CCodeDataLogger* LOGGERPHYS ( uint32_t addr )
   {
      return *(m_pLogger+PRGBANK_ABSBANK(addr));
   }
   static inline CCodeDataLogger* SRAMLOGGERPHYS ( uint32_t addr )
   {
      return *(m_pSRAMLogger+SRAMBANK_ABSBANK(addr));
   }
   static inline CCodeDataLogger* EXRAMLOGGER ()
   {
      return m_pEXRAMLogger;
   }

   // Support functions for inline disassembly in PRG-ROM, SRAM, and EXRAM
   static inline void PRGROMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      if ( (*(*(m_PRGROMopcodeMask+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr))) != mask )
      {
         *(m_PRGROMopcodeMaskDirty+PRGBANK_PHYS(addr)) = true;
      }
      *(*(m_PRGROMopcodeMask+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr)) = mask;
   }
   static inline void PRGROMOPCODEMASKATABSADDR ( uint32_t absAddr, uint8_t mask )
   {
      *(*(m_PRGROMopcodeMask+PRGBANK_ABSBANK(absAddr))+PRGBANK_OFF(absAddr)) = mask;
   }
   static inline void PRGROMOPCODEMASKCLR ( void )
   {
      int32_t idx1;
      int32_t idx2;
      for ( idx1 = 0; idx1 < NUM_ROM_BANKS; idx1++ )
      {
         for ( idx2 = 0; idx2 < MEM_8KB; idx2++ )
         {
            m_PRGROMopcodeMask[idx1][idx2] = 0;
         }
         m_PRGROMopcodeMaskDirty[idx1] = true;
      }
   }
   static inline char* PRGROMDISASSEMBLY ( uint32_t addr )
   {
      return *(*(m_PRGROMdisassembly+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr));
   }
   static inline char* PRGROMDISASSEMBLYATABSADDR ( uint32_t absAddr, char* buffer )
   {
      return C6502::Disassemble((*(m_PRGROMmemory+PRGBANK_ABSBANK(absAddr))+PRGBANK_OFF(absAddr)),buffer);
   }
   static uint32_t PRGROMSLOC2ADDR ( uint16_t sloc );
   static uint16_t PRGROMADDR2SLOC ( uint32_t addr );
   static inline uint32_t PRGROMSLOC ( uint32_t addr )
   {
      return *(m_PRGROMsloc+PRGBANK_PHYS(addr));
   }
   static inline void SRAMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      if ( (*(*(m_SRAMopcodeMask+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr))) != mask )
      {
         *(m_SRAMopcodeMaskDirty+SRAMBANK_PHYS(addr)) = true;
      }
      *(*(m_SRAMopcodeMask+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr)) = mask;
   }
   static inline void SRAMOPCODEMASKCLR ( void )
   {
      int32_t idx1;
      int32_t idx2;
      for ( idx1 = 0; idx1 < NUM_SRAM_BANKS; idx1++ )
      {
         for ( idx2 = 0; idx2 < MEM_8KB; idx2++ )
         {
            m_SRAMopcodeMask[idx1][idx2] = 0;
         }
         m_SRAMopcodeMaskDirty[idx1] = true;
      }
   }
   static inline char* SRAMDISASSEMBLY ( uint32_t addr )
   {
      return *(*(m_SRAMdisassembly+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr));
   }
   static uint32_t SRAMSLOC2ADDR ( uint16_t sloc );
   static uint16_t SRAMADDR2SLOC ( uint32_t addr );
   static inline uint32_t SRAMSLOC ( uint32_t addr)
   {
      return *(m_SRAMsloc+SRAMBANK_PHYS(addr));
   }
   static bool SRAMDIRTY() { return m_SRAMdirty; }
   static inline void EXRAMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      if ( (*(m_EXRAMopcodeMask+(addr-0x5C00))) != mask )
      {
         m_EXRAMopcodeMaskDirty = true;
      }
      *(m_EXRAMopcodeMask+(addr-0x5C00)) = mask;
   }
   static inline void EXRAMOPCODEMASKCLR ( void )
   {
      int32_t idx2;
      for ( idx2 = 0; idx2 < MEM_1KB; idx2++ )
      {
         m_EXRAMopcodeMask[idx2] = 0;
      }
      m_EXRAMopcodeMaskDirty = true;
   }
   static inline char* EXRAMDISASSEMBLY ( uint32_t addr )
   {
      return *(m_EXRAMdisassembly+(addr-0x5C00));
   }
   static uint32_t EXRAMSLOC2ADDR ( uint16_t sloc );
   static uint16_t EXRAMADDR2SLOC ( uint32_t addr );
   static inline uint32_t EXRAMSLOC ()
   {
      return m_EXRAMsloc;
   }
   static void DISASSEMBLE ();

   // Breakpoint support functions
   static CBreakpointEventInfo** BREAKPOINTEVENTS()
   {
      return m_tblBreakpointEvents;
   }
   static int32_t NUMBREAKPOINTEVENTS()
   {
      return m_numBreakpointEvents;
   }

   // Debug register display interfaces
   static inline CRegisterDatabase* REGISTERS ( void )
   {
      return m_dbRegisters;
   }

   static inline CMemoryDatabase* PRGROMMEMORY ( void )
   {
      return m_dbPRGROMMemory;
   }

   static inline CMemoryDatabase* SRAMMEMORY ( void )
   {
      return m_dbSRAMMemory;
   }

   static inline CMemoryDatabase* EXRAMMEMORY ( void )
   {
      return m_dbEXRAMMemory;
   }

   static inline CMemoryDatabase* VRAMMEMORY ( void )
   {
      return m_dbVRAMMemory;
   }

   static inline CMemoryDatabase* CHRMEMORY ( void )
   {
      return m_dbCHRMemory;
   }

protected:
   static uint8_t**  m_PRGROMmemory;
   static uint8_t**  m_CHRmemory;
   static uint8_t**  m_SRAMmemory;
   static uint8_t*   m_EXRAMmemory;
   static uint8_t*   m_VRAMmemory;

   // Mapper stuff...
   static uint32_t           m_mapper;
   static uint32_t           m_numPrgBanks;
   static uint32_t           m_numChrBanks;
   static uint8_t* m_pPRGROMmemory [ 4 ];
   static uint8_t* m_pCHRmemory [ 8 ];
   static uint8_t* m_pSRAMmemory [ 5 ];
   static uint8_t* m_pVRAMmemory [ 8 ];

   static CCodeDataLogger* m_pLogger [ NUM_ROM_BANKS ];
   static CCodeDataLogger* m_pEXRAMLogger;
   static CCodeDataLogger* m_pSRAMLogger [ NUM_SRAM_BANKS ];

   static CRegisterDatabase* m_dbRegisters;

   static CMemoryDatabase* m_dbPRGROMMemory;
   static CMemoryDatabase* m_dbSRAMMemory;
   static CMemoryDatabase* m_dbEXRAMMemory;
   static CMemoryDatabase* m_dbVRAMMemory;
   static CMemoryDatabase* m_dbCHRMemory;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;

   static uint8_t**  m_PRGROMopcodeMask;
   static bool*      m_PRGROMopcodeMaskDirty;
   static char***    m_PRGROMdisassembly;
   static uint16_t** m_PRGROMsloc2addr;
   static uint16_t** m_PRGROMaddr2sloc;
   static uint32_t*  m_PRGROMsloc;

   static uint8_t**  m_SRAMopcodeMask;
   static bool*      m_SRAMopcodeMaskDirty;
   static char***    m_SRAMdisassembly;
   static uint16_t** m_SRAMsloc2addr;
   static uint16_t** m_SRAMaddr2sloc;
   static uint32_t*  m_SRAMsloc;
   static bool       m_SRAMdirty;

   static uint8_t*  m_EXRAMopcodeMask;
   static bool     m_EXRAMopcodeMaskDirty;
   static char**          m_EXRAMdisassembly;
   static uint16_t* m_EXRAMsloc2addr;
   static uint16_t* m_EXRAMaddr2sloc;
   static uint32_t   m_EXRAMsloc;
};

#endif
