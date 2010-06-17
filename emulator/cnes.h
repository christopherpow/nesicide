#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"
#include "ctracer.h"

#include "cnesicidecommon.h"

// Identifiers of tooltip info to provide.
#define TOOLTIP_BYTES 0
#define TOOLTIP_INFO  1

// Video modes.  The emulator supports both NTSC and PAL.
#define MODE_NTSC 0
#define MODE_PAL  1

// The CNES class is the implementation of the NES as a complete
// emulatable machine.  It contains a RUN method which is used
// to drive the other objects (CPPU, C6502, CAPU, CROM) through
// the paces of emulating a NES as accurately as humanly possible.
//
// The NES object also is a container for globally useful stuff
// that is needed by the other objects.  Examples of this are:
// the current video mode; the current list of active breakpoints
// from the debugger; logic to perform breakpoint checks during
// emulation; the execution tracer database; and interfaces to
// provide a one-stop-shop for the debugger to retrieve disassembled
// code for a particular memory address.  The disassembled code
// interfaces are important otherwise each debugger inspector that
// needed to display disassembled code would need to replicate the
// logic to determine whether the address was in CPU RAM, or cartridge
// EXRAM, SRAM, or PRG-ROM.
//
// The NES object also contains information regarding the replay
// mode which is not yet fully implemented.
class CNES
{
public:
	CNES();
	virtual ~CNES();

   // Accessor methods to get/set the current video mode.
   static inline void VIDEOMODE ( int mode ) { m_videoMode = mode; }
   static inline int VIDEOMODE ( void ) { return m_videoMode; }

   // This method performs a full NES reset and initializes
   // the ROM object with the appropriate mapper index.
   static void RESET ( UINT mapper );

   // This method emulates a NES video frame and passes the
   // current state of the joypad to the emulation engine.
   // The current state of the joypad is constructed from
   // intercepted keypress/keyrelease events in the UI.
   static void RUN ( unsigned char* joy );

   // Accessor methods to get/set whether or not the emulation
   // engine is in replay mode.  In replay mode the emulation runs
   // as normal but the joypad inputs are fed in from previously
   // recorded emulation runs.  At present this only works "sort of well"
   // for emulation runs that start from NES reset.
   static void REPLAY ( bool enable ) { m_bReplay = enable; }
   static bool REPLAY () { return m_bReplay; }

   // This method clears the runtime disassembly cache.  It is used
   // whenever a new ROM image is loaded into the machine so as not
   // to confuse the user with previously disassembled code appearing
   // where new code should replace it.  This has the 'unfortunate' side
   // effect of forcing the disassembly to show all ".DB" entries for
   // each byte, but as the ROM is emulated the disassembly of actual
   // executed code fills in.
   static void CLEAROPCODEMASKS ( void );

   // Accessor method to retrieve the NES object's frame counter.
   // This is used by some debugger inspectors.
   static unsigned int FRAME () { return m_frame; }

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
   static void CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type = (eBreakpointType)-1, int data = 0, int event = 0 );

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
   // declared in the CPU, PPU, APU, and ROM objects.  These wrapper
   // methods contain the logic to determine which of the sub-object
   // methods to invoke based on the passed parameters.
   static char* DISASSEMBLY ( UINT addr );
   static UINT SLOC2ADDR ( unsigned short sloc );
   static unsigned short ADDR2SLOC ( UINT addr );
   static unsigned int SLOC ( UINT addr );
   static unsigned char _MEM ( UINT addr );
   static void DISASSEMBLE ( void );

   // This method retrieves information to be displayed in ToolTips
   // for the Code Browser window.
   static void CODEBROWSERTOOLTIP ( int tipType, UINT addr, char* tooltipBuffer );

protected:
   // Whether or not joypad input is being fed from the user or from
   // previously recorded emulation runs.
   static bool         m_bReplay;

   // This is a running counter of the number of frames emulated.
   static unsigned int m_frame;

   // This is the database of active breakpoints.
   static CBreakpointInfo m_breakpoints;

   // These flags determine the breakpoint state and behavior
   // of the emulation engine.
   static bool            m_bAtBreakpoint;
   static bool            m_bStepCPUBreakpoint;
   static bool            m_bStepPPUBreakpoint;

   // NTSC, or PAL?
   static int             m_videoMode;

   // The execution tracer database.
   static CTracer         m_tracer;
};

#endif
