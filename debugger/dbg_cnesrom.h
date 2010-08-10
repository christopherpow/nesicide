#if !defined ( ROM_H )
#define ROM_H

#include "emulator_core.h"

#include "cregisterdata.h"
#include "ccodedatalogger.h"
#include "cbreakpointinfo.h"

// Breakpoint event identifiers
enum
{
   MAPPER_EVENT_IRQ = 0,
   NUM_MAPPER_EVENTS
};

class CROMDBG
{
public:
   CROMDBG();
   virtual ~CROMDBG();

   static uint32_t ABSADDR ( uint32_t addr ) { return nesGetPRGROMAbsoluteAddress(addr); }
   static uint32_t CHRMEMABSADDR ( uint32_t addr ) { return nesGetCHRMEMAbsoluteAddress(addr); }
   static uint32_t PRGROM ( uint32_t addr ) { return nesGetPRGROMData(addr); }
   static void PRGROM ( uint32_t, uint8_t ) {};
   static uint32_t CHRMEM ( uint32_t addr ) { return nesGetCHRMEMData(addr); }
   static void CHRMEM ( uint32_t addr, uint8_t data ) { nesSetCHRMEMData(addr,data); }
   static uint32_t SRAMABSADDR ( uint32_t addr ) { return nesGetSRAMAbsoluteAddress(addr); }
   static uint32_t SRAM ( uint32_t addr ) { return nesGetSRAMData(addr); }
   static void SRAM ( uint32_t addr, uint8_t data ) { nesSetSRAMData(addr,data); }
   static uint32_t EXRAMABSADDR ( uint32_t addr ) { return nesGetEXRAMAbsoluteAddress(addr); }
   static uint32_t EXRAM ( uint32_t addr ) { return nesGetEXRAMData(addr); }
   static void EXRAM ( uint32_t addr, uint8_t data ) { nesSetEXRAMData(addr,data); }
   static uint8_t MAPPER ( void ) { return nesGetMapper(); }
   static uint8_t LOWREAD ( uint32_t addr ) { return nesMapperLowRead(addr); }
   static uint8_t HIGHREAD ( uint32_t addr ) { return nesMapperHighRead(addr); }
   static void LOWWRITE ( uint32_t addr, uint8_t data ) { nesMapperLowWrite(addr,data); }
   static void HIGHWRITE ( uint32_t addr, uint8_t data ) { nesMapperHighWrite(addr,data); }
   static bool PRGREMAPABLE ( void ) { return nesMapperRemapsPRGROM(); }
   static bool CHRREMAPABLE ( void ) { return nesMapperRemapsCHRMEM(); }

   // Support functions for inline disassembly in PRG-ROM, SRAM, and EXRAM
   static inline bool IsWriteProtected ( void ) { return nesIsCHRROM(); }
   static inline void PRGROMOPCODEMASK ( uint32_t addr, uint8_t mask ) { *(*(m_PRGROMopcodeMask+nesGetPhysicalPRGROMBank(addr))+PRGBANK_OFF(addr)) = mask; }
   static inline void PRGROMOPCODEMASKCLR ( void ) { memset(m_PRGROMopcodeMask,0,sizeof(m_PRGROMopcodeMask)); }
   static inline char* PRGROMDISASSEMBLY ( uint32_t addr ) { return *(*(m_PRGROMdisassembly+nesGetPhysicalPRGROMBank(addr))+PRGBANK_OFF(addr)); }
   static uint32_t PRGROMSLOC2ADDR ( uint16_t sloc );
   static uint16_t PRGROMADDR2SLOC ( uint32_t addr );
   static inline uint32_t PRGROMSLOC ( uint32_t addr ) { return *(m_PRGROMsloc+nesGetPhysicalPRGROMBank(addr)); }
   static inline void SRAMOPCODEMASK ( uint32_t addr, uint8_t mask ) { *(*(m_SRAMopcodeMask+nesGetPhysicalSRAMBank(addr))+SRAMBANK_OFF(addr)) = mask; }
   static inline void SRAMOPCODEMASKCLR ( void ) { memset(m_SRAMopcodeMask,0,sizeof(m_SRAMopcodeMask)); }
   static inline char* SRAMDISASSEMBLY ( uint32_t addr ) { return *(*(m_SRAMdisassembly+nesGetPhysicalPRGROMBank(addr))+SRAMBANK_OFF(addr)); }
   static uint32_t SRAMSLOC2ADDR ( uint16_t sloc );
   static uint16_t SRAMADDR2SLOC ( uint32_t addr );
   static inline uint32_t SRAMSLOC ( uint32_t addr) { return *(m_SRAMsloc+nesGetPhysicalPRGROMBank(addr)); }
   static inline void EXRAMOPCODEMASK ( uint32_t addr, uint8_t mask ) { *(m_EXRAMopcodeMask+(addr-0x5C00)) = mask; }
   static inline void EXRAMOPCODEMASKCLR ( void ) { memset(m_EXRAMopcodeMask,0,sizeof(m_EXRAMopcodeMask)); }
   static inline char* EXRAMDISASSEMBLY ( uint32_t addr ) { return *(m_EXRAMdisassembly+(addr-0x5C00)); }
   static uint32_t EXRAMSLOC2ADDR ( uint16_t sloc );
   static uint16_t EXRAMADDR2SLOC ( uint32_t addr );
   static inline uint32_t EXRAMSLOC () { return m_EXRAMsloc; }
   static void DISASSEMBLE ();

   static void RESET ( void );
   static void RESET ( uint32_t mapper );

   // Code/Data logger support functions
   static inline CCodeDataLogger* LOGGER ( uint32_t addr ) { return *(m_pLogger+nesGetPhysicalPRGROMBank(addr)); }
   static inline CCodeDataLogger* SRAMLOGGER ( uint32_t addr ) { return *(m_pSRAMLogger+nesGetPhysicalSRAMBank(addr)); }
   static inline CCodeDataLogger* EXRAMLOGGER () { return m_pEXRAMLogger; }

   // Breakpoint support functions
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int32_t NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // Debug register display interfaces
   static inline CRegisterData** REGISTERS ( void ) { return m_tblRegisters; }
   static inline int32_t NUMREGISTERS ( void ) { return m_numRegisters; }

protected:
   static uint8_t  m_PRGROMopcodeMask [ NUM_ROM_BANKS ][ MEM_8KB ];
   static char*          m_PRGROMdisassembly [ NUM_ROM_BANKS ][ MEM_8KB ];
   static uint16_t m_PRGROMsloc2addr [ NUM_ROM_BANKS ][ MEM_8KB ];
   static uint16_t m_PRGROMaddr2sloc [ NUM_ROM_BANKS ][ MEM_8KB ];
   static uint32_t   m_PRGROMsloc [ NUM_ROM_BANKS ];

   static uint8_t  m_SRAMopcodeMask [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static char*          m_SRAMdisassembly [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static uint16_t m_SRAMsloc2addr [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static uint16_t m_SRAMaddr2sloc [ NUM_SRAM_BANKS ][ MEM_8KB ];
   static uint32_t   m_SRAMsloc [ NUM_SRAM_BANKS ];
   static CCodeDataLogger* m_pSRAMLogger [ NUM_SRAM_BANKS ];

   static uint8_t  m_EXRAMopcodeMask [ MEM_1KB ];
   static char*          m_EXRAMdisassembly [ MEM_1KB ];
   static uint16_t m_EXRAMsloc2addr [ MEM_1KB ];
   static uint16_t m_EXRAMaddr2sloc [ MEM_1KB ];
   static uint32_t   m_EXRAMsloc;
   static CCodeDataLogger* m_pEXRAMLogger;

   // Other UI stuff...
   static CCodeDataLogger* m_pLogger [ NUM_ROM_BANKS ];
   static CRegisterData** m_tblRegisters;
   static int32_t             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;
};

#endif
