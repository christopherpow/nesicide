#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"
#include "ctracer.h"

#include "emulator_core.h"

// Identifiers of tooltip info to provide.
#define TOOLTIP_BYTES 0
#define TOOLTIP_INFO  1

// The CNESDBG class is a container for all debug elements
// relevant to the NES overall.
class CNESDBG
{
public:
   CNESDBG();
   virtual ~CNESDBG();

   // This method clears the runtime disassembly cache.  It is used
   // whenever a new ROM image is loaded into the machine so as not
   // to confuse the user with previously disassembled code appearing
   // where new code should replace it.  This has the 'unfortunate' side
   // effect of forcing the disassembly to show all ".DB" entries for
   // each byte, but as the ROM is emulated the disassembly of actual
   // executed code fills in.
   static void CLEAROPCODEMASKS ( void );

   // Accessor method to retrieve the execution tracer database.  Other
   // elements of the NES use this method to add entries to the database
   // during emulation.
   static inline CTracer* TRACER ( void ) { return &m_tracer; }

   // This method globally enables or disables breakpoints.  It is used
   // during an emulation hard-reset (which is caused whenever a new
   // ROM image is loaded) to prevent the emulation engine from getting
   // hung up on a set breakpoint during the ROM image switchover.
   static void BREAKPOINTS ( bool enable );

   // This method retrieves the database of currently active breakpoints.
   // It is used by the debugger inspectors to determine whether or not
   // they need to show themselves upon hitting a particular breakpoint type.
   // It is also used by the NES object in evaluating breakpoints for hits.
   static CBreakpointInfo* BREAKPOINTS ( void ) { return &m_breakpoints; }

   // This method is invoked by objects within the emulation engine (CNES,
   // C6502, CPPU, CAPU, CROM) to allow the emulation engine to halt itself
   // if a breakpoint is encountered.
   static void CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type = (eBreakpointType)-1, int32_t data = 0, int32_t event = 0 );

   // This method forces the emulation engine into breakpoint territory;
   // the emulation is halted, a breakpoint-watching thread is released,
   // and the UI is updated (by the newly released thread).
   static void FORCEBREAKPOINT ( void );

   // These methods get/set breakpoint flags within the NES object that indicate
   // whether or not the machine is currently at a breakpoint.
   static bool ATBREAKPOINT ( void ) { return m_bAtBreakpoint; }
   static void CLEARBREAKPOINT ( void ) { m_bAtBreakpoint = false; }

   // These methods set a flag within the NES object indicating that a
   // breakpoint should be taken either a)after execution of the next
   // instruction by the CPU, or b)after a PPU cycle has elapsed.
   static void STEPCPUBREAKPOINT ( void ) { m_bStepCPUBreakpoint = true; }
   static void STEPPPUBREAKPOINT ( void ) { m_bStepPPUBreakpoint = true; }

   // These methods are wrapper methods around the similar methods
   // declared in the CPU and ROM objects.  These wrapper
   // methods contain the logic to determine which of the sub-object
   // methods to invoke based on the passed parameters.
   static char* DISASSEMBLY ( uint32_t addr );
   static uint32_t SLOC2ADDR ( uint16_t sloc );
   static uint16_t ADDR2SLOC ( uint32_t addr );
   static uint32_t SLOC ( uint32_t addr );
   static uint8_t _MEM ( uint32_t addr );
   static void DISASSEMBLE ( void );
   static uint32_t ABSADDR ( uint32_t addr );

   // This method retrieves information to be displayed in ToolTips
   // for the Code Browser window.
   static void CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer );

protected:
   // This is the database of active breakpoints.
   static CBreakpointInfo m_breakpoints;

   // These flags determine the breakpoint state and behavior
   // of the emulation engine.
   static bool            m_bAtBreakpoint;
   static bool            m_bStepCPUBreakpoint;
   static bool            m_bStepPPUBreakpoint;

   // The execution tracer database.
   static CTracer         m_tracer;
};

#endif
