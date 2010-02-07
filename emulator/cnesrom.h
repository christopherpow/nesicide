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

#define NUM_ROM_BANKS 32

#define PRGBANK_NUM(addr) ( (addr&MASK_32KB)>>SHIFT_32KB_8KB )
#define PRGBANK_OFF(addr) ( addr&MASK_8KB )

#define CHRBANK_NUM(addr) ( addr>>SHIFT_8KB_1KB )
#define CHRBANK_OFF(addr) ( addr&MASK_1KB )

#define SRAMBANK_NUM(addr) ( ((addr-SRAM_START)&MASK_64KB)>>SHIFT_64KB_8KB )
#define SRAMBANK_OFF(addr) ( addr&MASK_8KB )

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
   static void CHRRAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_CHRRAMmemory,data,length); }
   static void CHRRAMCLR ( void ) { memset(m_CHRRAMmemory,0x00,MEM_8KB); }
   static void SRAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_SRAMmemory+addr,data,length); }
   static void SRAMCLR ( void ) { memset(m_SRAMmemory,0x00,MEM_64KB); }
   static void EXRAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_EXRAMmemory+addr,data,length); }
   static void EXRAMCLR ( void ) { memset(m_EXRAMmemory,0x00,MEM_1KB); }

   // Operations
   static bool IsWriteProtected ( void ) { return (m_numChrBanks>0); }
   static inline UINT PRGROMBANK ( UINT addr ) { return *(m_PRGROMbank+PRGBANK_NUM(addr)); }
   static inline UINT ABSADDR ( UINT addr ) { return (PRGROMBANK(addr)*MEM_8KB)+PRGBANK_OFF(addr); }
//   static UINT CHRMEMBANK ( UINT addr ) { return m_CHRbank[CHRBANK_NUM(addr)]; }
   static inline UINT PRGROM ( UINT addr ) { return *(*(m_pPRGROMmemory+PRGBANK_NUM(addr))+(PRGBANK_OFF(addr))); }
   static inline void PRGROM ( UINT addr, unsigned char data ) {}
   static inline void CHRMEM ( UINT addr, unsigned char data ) { *(*(m_pCHRmemory+CHRBANK_NUM(addr))+(CHRBANK_OFF(addr))) = data; }
   static inline UINT CHRMEM ( UINT addr ) { return *(*(m_pCHRmemory+CHRBANK_NUM(addr))+(CHRBANK_OFF(addr))); }
   static inline UINT SRAM ( UINT addr ) { return *(m_pSRAMmemory+SRAMBANK_OFF(addr)); }
   static inline void SRAM ( UINT addr, unsigned char data ) { *(m_pSRAMmemory+SRAMBANK_OFF(addr)) = data; }
   static inline UINT EXRAM ( UINT addr ) { return *(m_EXRAMmemory+(addr-EXRAM_START)); }
   static inline void EXRAM ( UINT addr, unsigned char data ) { *(m_EXRAMmemory+(addr-EXRAM_START)) = data; }
   static inline CCodeDataLogger* LOGGER ( UINT addr ) { return *(m_pLogger+PRGROMBANK(addr)); }
   static inline unsigned char* CHRRAMPTR ( UINT addr ) { return &(m_CHRRAMmemory[addr]); }
   static inline CRegisterData** REGISTERS ( void ) { return m_tblRegisters; }
   static inline int NUMREGISTERS ( void ) { return m_numRegisters; }
   static inline char* DISASSEMBLY ( int addr ) { return m_PRGROMdisassembly[PRGROMBANK(addr)][PRGBANK_OFF(addr)]; }
   static inline unsigned short SLOC2ADDR ( unsigned short addr, int sloc ) { return m_PRGROMsloc2addr[PRGROMBANK(addr)][sloc]; }
   static inline int SLOC ( int addr ) { return m_PRGROMsloc[PRGROMBANK(addr)]; }

   // Mapper interfaces [not usually called directly, rather through mapperfunc array]
   static void RESET ();
   static UINT MAPPER ( void ) { return m_mapper; }
   static UINT MAPPER ( UINT addr ) { return PRGROM(addr); }
   static void MAPPER ( UINT addr, unsigned char data ) {};
   static UINT LMAPPER ( UINT addr ) { return 0x40; } // open bus?
   static void LMAPPER ( UINT addr, unsigned char data ) {};
   static bool SYNCH ( int scanline ) { return false; }
   static bool SYNCV ( void ) { return false; }
   static void LATCH ( UINT addr ) {}
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static unsigned char  m_SRAMmemory [ MEM_64KB ];
   static unsigned char  m_EXRAMmemory [ MEM_1KB ];
   static unsigned char  m_PRGROMmemory [ NUM_ROM_BANKS ][ MEM_8KB ];
   static char*          m_PRGROMdisassembly [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned short m_PRGROMsloc2addr [ NUM_ROM_BANKS ][ MEM_8KB ];
   static int            m_PRGROMsloc [ NUM_ROM_BANKS ];
   static unsigned char  m_CHRROMmemory [ NUM_ROM_BANKS ][ MEM_8KB ];
   static unsigned char  m_CHRRAMmemory [ MEM_8KB ];

   // Mapper stuff...
   static UINT           m_mapper;
   static UINT           m_numPrgBanks;
   static UINT           m_numChrBanks;
   static UINT           m_PRGROMbank [ 4 ];
   static unsigned char* m_pPRGROMmemory [ 4 ];
   static unsigned char* m_pCHRmemory [ 8 ];
   static unsigned char* m_pSRAMmemory;

   // Other UI stuff...
   static CCodeDataLogger* m_pLogger [ NUM_ROM_BANKS ];
   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;
};

#endif
