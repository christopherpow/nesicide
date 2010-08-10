#if !defined ( APU_H )
#define APU_H

#include "emulator_core.h"

#include "cbreakpointinfo.h"
#include "cregisterdata.h"

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   APU_EVENT_IRQ = 0,
   APU_EVENT_DMC_DMA,
   APU_EVENT_LENGTH_COUNTER_CLOCKED,
   NUM_APU_EVENTS
};

class CAPUDBG
{
public:
   CAPUDBG();
   virtual ~CAPUDBG();

   static uint32_t _APU ( uint32_t addr ) { return nesGetAPURegister(addr); }
   static void _APU ( uint32_t addr, uint8_t data ) { nesSetAPURegister(addr,data); }

   static inline uint32_t CYCLES ( void ) { return nesGetAPUCycle(); }

   static int32_t SEQUENCERMODE ( void ) { return nesGetAPUSequencerMode(); }

   // INTERNAL ACCESSOR FUNCTIONS
   // These are called directly.
   static void LENGTHCOUNTERS ( uint16_t* sq1, uint16_t* sq2, uint16_t* triangle, uint16_t* noise, uint16_t* dmc )
   {
      return nesGetAPULengthCounters(sq1,sq2,triangle,noise,dmc);
   }
   static void LINEARCOUNTER ( uint8_t* triangle ) { return nesGetAPUTriangleLinearCounter(triangle); }
   static void GETDACS ( uint8_t* sq1,
                         uint8_t* sq2,
                         uint8_t* triangle,
                         uint8_t* noise,
                         uint8_t* dmc ) { return nesGetAPUDACs(sq1,sq2,triangle,noise,dmc); }
   static void DMCIRQ ( bool* enabled, bool* asserted )
   {
      return nesGetAPUDMCIRQ(enabled,asserted);
   }
   static void SAMPLEINFO ( uint16_t* addr, uint16_t* length, uint16_t* pos )
   {
      return nesGetAPUDMCSampleInfo(addr,length,pos);
   }
   static void DMAINFO ( uint8_t* buffer, bool* full )
   {
      return nesGetAPUDMCDMAInfo(buffer,full);
   }

   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static int32_t NUMREGISTERS() { return m_numRegisters; }

   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int32_t NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

protected:
   static CRegisterData** m_tblRegisters;
   static int32_t             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;
};

#endif
