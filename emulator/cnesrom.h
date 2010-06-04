// ROM.h: interface for the CROM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_H )
#define ROM_H

#include "cnesicidecommon.h"
#include "cnesmappers.h"
#include "cnes.h"

#include "cregisterdata.h"

#include "ccodedatalogger.h"

#include "cbreakpointinfo.h"

// Breakpoint event identifiers
enum
{
   MAPPER_EVENT_IRQ = 0,
   NUM_MAPPER_EVENTS
};

#define NUM_ROM_BANKS 64

#define NUM_SRAM_BANKS 8

// Resolve a 6502-address to one of 4 8KB PRG ROM banks [0:$8000-$9FFF, 1:$A000-$BFFF, 2:$C000-$DFFF, or 3:$E000-$FFFF]
#define PRGBANK_VIRT(addr) ( (addr&MASK_32KB)>>SHIFT_32KB_8KB )
// Retrieve the bank-offset address portion of a 6502-address for use within PRG ROM banks
#define PRGBANK_OFF(addr) ( addr&MASK_8KB )
// Resolve a 6502-address to one of the 8KB PRG ROM banks within a ROM file [the absolute physical address]
#define PRGBANK_PHYS(addr) ( *(m_PRGROMbank+PRGBANK_VIRT(addr)) )

#define CHRBANK_NUM(addr) ( addr>>SHIFT_8KB_1KB )
#define CHRBANK_OFF(addr) ( addr&MASK_1KB )

#define SRAMBANK_VIRT(addr) ( ((addr-SRAM_START)&MASK_64KB)>>SHIFT_64KB_8KB )
#define SRAMBANK_OFF(addr) ( addr&MASK_8KB )
// Resolve a 6502-address to one of the 8KB SRAM banks within a cartridge [the absolute physical address]
// NOTE: Mappers with SRAM typically have one 8KB bank at $6000-$7FFF but MMC5
// allows mapping of up to 64KB of SRAM anywhere in the upper 32KB ROM space.
#define SRAMBANK_PHYS(addr) ( *(m_SRAMbank+SRAMBANK_VIRT(addr)) )

class CROM  
{
public:
    CROM();
    virtual ~CROM();

   // Priming interfaces (data setup/initialization)
   static void Clear16KBanks ();
   static void Clear8KBanks () { m_numChrBanks = 0; }
   static void Set8KBank ( int bank, unsigned char* data );
   static void Set16KBank ( int bank, unsigned char* data );
   static void DoneLoadingBanks ( void );
   static void CHRRAMSET ( UINT, unsigned char* data, UINT length ) { memcpy(m_CHRRAMmemory,data,length); }
   static void CHRRAMCLR ( void ) { memset(m_CHRRAMmemory,0x00,MEM_8KB); }
   static void SRAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_SRAMmemory+addr,data,length); }
   static void SRAMCLR ( void ) { memset(m_SRAMmemory,0x00,MEM_64KB); }
   static void EXRAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_EXRAMmemory+addr,data,length); }
   static void EXRAMCLR ( void ) { memset(m_EXRAMmemory,0x00,MEM_1KB); }

   // Operations
   static bool IsWriteProtected ( void ) { return (m_numChrBanks>0); }
   static inline UINT ABSADDR ( UINT addr ) { return (PRGBANK_PHYS(addr)*MEM_8KB)+PRGBANK_OFF(addr); }
   static UINT CHRMEMABSADDR ( UINT addr ) { return ((*(m_CHRbank+CHRBANK_NUM(addr)))*MEM_1KB)+CHRBANK_OFF(addr); }
   static inline unsigned char* CHRRAMPTR ( UINT addr ) { return &(m_CHRRAMmemory[addr]); }
   static inline UINT PRGROM ( UINT addr ) { return *(*(m_pPRGROMmemory+PRGBANK_VIRT(addr))+(PRGBANK_OFF(addr))); }
   static inline void PRGROM ( UINT, unsigned char ) {}
   static inline void CHRMEM ( UINT addr, unsigned char data ) { *(*(m_pCHRmemory+CHRBANK_NUM(addr))+(CHRBANK_OFF(addr))) = data; }
   static inline UINT CHRMEM ( UINT addr ) { return *(*(m_pCHRmemory+CHRBANK_NUM(addr))+(CHRBANK_OFF(addr))); }
   static inline UINT SRAMABSADDR ( UINT addr ) { return (SRAMBANK_PHYS(addr)*MEM_8KB)+SRAMBANK_OFF(addr); }
   static inline UINT SRAM ( UINT addr ) { return *(*(m_pSRAMmemory+SRAMBANK_VIRT(addr))+SRAMBANK_OFF(addr)); }
   static inline void SRAM ( UINT addr, unsigned char data ) { *(*(m_pSRAMmemory+SRAMBANK_VIRT(addr))+SRAMBANK_OFF(addr)) = data; }
   static inline void REMAPSRAM ( UINT addr, unsigned char bank ) { *(m_pSRAMmemory+SRAMBANK_VIRT(addr)) = *(m_SRAMmemory+bank); }
   static inline UINT EXRAMABSADDR ( UINT addr ) { return (addr%MEM_1KB); }
   static inline UINT EXRAM ( UINT addr ) { return *(m_EXRAMmemory+(addr-EXRAM_START)); }
   static inline void EXRAM ( UINT addr, unsigned char data ) { *(m_EXRAMmemory+(addr-EXRAM_START)) = data; }
   static inline CRegisterData** REGISTERS ( void ) { return m_tblRegisters; }
   static inline int NUMREGISTERS ( void ) { return m_numRegisters; }

   // Support functions for inline disassembly in PRG-ROM, SRAM, and EXRAM
   static inline void PRGROMOPCODEMASK ( UINT addr, unsigned char mask ) { *(*(m_PRGROMopcodeMask+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr)) = mask; }
   static inline void PRGROMOPCODEMASKCLR ( void ) { memset(m_PRGROMopcodeMask,0,sizeof(m_PRGROMopcodeMask)); }
   static inline char* PRGROMDISASSEMBLY ( UINT addr ) { return *(*(m_PRGROMdisassembly+PRGBANK_PHYS(addr))+PRGBANK_OFF(addr)); }
   static UINT PRGROMSLOC2ADDR ( unsigned short sloc );
   static unsigned short PRGROMADDR2SLOC ( UINT addr );
   static inline unsigned int PRGROMSLOC ( UINT addr ) { return *(m_PRGROMsloc+PRGBANK_PHYS(addr)); }
   static inline void SRAMOPCODEMASK ( UINT addr, unsigned char mask ) { *(*(m_SRAMopcodeMask+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr)) = mask; }
   static inline void SRAMOPCODEMASKCLR ( void ) { memset(m_SRAMopcodeMask,0,sizeof(m_SRAMopcodeMask)); }
   static inline char* SRAMDISASSEMBLY ( UINT addr ) { return *(*(m_SRAMdisassembly+SRAMBANK_PHYS(addr))+SRAMBANK_OFF(addr)); }
   static UINT SRAMSLOC2ADDR ( unsigned short sloc );
   static unsigned short SRAMADDR2SLOC ( UINT addr );
   static inline unsigned int SRAMSLOC ( UINT addr) { return *(m_SRAMsloc+SRAMBANK_PHYS(addr)); }
   static inline void EXRAMOPCODEMASK ( UINT addr, unsigned char mask ) { *(m_EXRAMopcodeMask+(addr-0x5C00)) = mask; }
   static inline void EXRAMOPCODEMASKCLR ( void ) { memset(m_EXRAMopcodeMask,0,sizeof(m_EXRAMopcodeMask)); }
   static inline char* EXRAMDISASSEMBLY ( UINT addr ) { return *(m_EXRAMdisassembly+(addr-0x5C00)); }
   static UINT EXRAMSLOC2ADDR ( unsigned short sloc );
   static unsigned short EXRAMADDR2SLOC ( UINT addr );
   static inline unsigned int EXRAMSLOC () { return m_EXRAMsloc; }
   static void DISASSEMBLE ();

   // Code/Data logger support functions
   static inline CCodeDataLogger* LOGGER ( UINT addr ) { return *(m_pLogger+PRGBANK_PHYS(addr)); }
   static inline CCodeDataLogger* SRAMLOGGER ( UINT addr ) { return *(m_pSRAMLogger+SRAMBANK_PHYS(addr)); }
   static inline CCodeDataLogger* EXRAMLOGGER () { return m_pEXRAMLogger; }

   // Breakpoint support functions
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // Mapper interfaces [called by emulator through mapperfunc array]
   static void RESET ( void );
   static void RESET ( UINT mapper );
   static UINT MAPPER ( void ) { return m_mapper; }
   static UINT MAPPER ( UINT addr ) { return PRGROM(addr); }
   static void MAPPER ( UINT, unsigned char ) {}
   static UINT LMAPPER ( UINT ) { return 0x40; } // open bus?
   static void LMAPPER ( UINT, unsigned char ) {}
   static void SYNCH ( UINT, UINT ) {}
   static bool SYNCV ( void ) { return false; }
   static void LATCH ( UINT ) {}
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static unsigned char  m_PRGROMmemory [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned char  m_PRGROMopcodeMask [ NUM_ROM_BANKS ][ MEM_8KB ];
   static char*          m_PRGROMdisassembly [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned short m_PRGROMsloc2addr [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned short m_PRGROMaddr2sloc [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned int   m_PRGROMsloc [ NUM_ROM_BANKS ];

   static unsigned char  m_CHRROMmemory [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned char  m_CHRRAMmemory [ MEM_8KB ];

   static unsigned char  m_SRAMmemory [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static unsigned char  m_SRAMopcodeMask [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static char*          m_SRAMdisassembly [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static unsigned short m_SRAMsloc2addr [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static unsigned short m_SRAMaddr2sloc [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static unsigned int   m_SRAMsloc [ NUM_SRAM_BANKS ];
   static CCodeDataLogger* m_pSRAMLogger [ NUM_SRAM_BANKS ];

   static unsigned char  m_EXRAMmemory [ MEM_1KB ];
   static unsigned char  m_EXRAMopcodeMask [ MEM_1KB ];
   static char*          m_EXRAMdisassembly [ MEM_1KB ];
   static unsigned short m_EXRAMsloc2addr [ MEM_1KB ];
   static unsigned short m_EXRAMaddr2sloc [ MEM_1KB ];
   static unsigned int   m_EXRAMsloc;
   static CCodeDataLogger* m_pEXRAMLogger;

   // Mapper stuff...
   static UINT           m_mapper;
   static UINT           m_numPrgBanks;
   static UINT           m_numChrBanks;
   static UINT           m_PRGROMbank [ 4 ];
   static unsigned char* m_pPRGROMmemory [ 4 ];
   static UINT           m_CHRbank [ 8 ];
   static unsigned char* m_pCHRmemory [ 8 ];
   static UINT           m_SRAMbank [ 5 ];
   static unsigned char* m_pSRAMmemory [ 5 ];

   // Other UI stuff...
   static CCodeDataLogger* m_pLogger [ NUM_ROM_BANKS ];
   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int                    m_numBreakpointEvents;
};

#endif
