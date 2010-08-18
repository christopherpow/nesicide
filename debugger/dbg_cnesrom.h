#if !defined ( ROM_H )
#define ROM_H

#include "emulator_core.h"

#include "cregisterdata.h"
#include "ccodedatalogger.h"
#include "cbreakpointinfo.h"

class CROMDBG
{
public:
   CROMDBG();
   virtual ~CROMDBG();

   static inline bool IsWriteProtected ( void ) { return nesIsCHRROM(); }
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

   static void RESET ( void );
   static void RESET ( uint32_t mapper );

   // Breakpoint support functions
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int32_t NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // Debug register display interfaces
   static inline CRegisterData** REGISTERS ( void ) { return m_tblRegisters; }
   static inline int32_t NUMREGISTERS ( void ) { return m_numRegisters; }

protected:
   // Other UI stuff...
   static CRegisterData** m_tblRegisters;
   static int32_t             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;
};

#endif
