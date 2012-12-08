#if !defined ( NES_H )
#define NES_H

#include "ctracer.h"
#include "cjoypadlogger.h"
#include "cnesbreakpointinfo.h"

#include "nes_emulator_core.h"

// The CNES class is the implementation of the NES as a complete
// emulatable machine.  It contains a RUN method which is used
// to drive the other objects (CPPU, C6502, CAPU, CROM) through
// the paces of emulating a NES as accurately as humanly possible.
//
// The CNES::RUN method drives each frame down through the PPU in
// the following order:
//
// Render visible scanlines, one at a time.
// Update scanline-dependent visual inspectors.
// Run the 'quiet' or resting scanline.
// Run the VBLANK scanlines.
// Run the 'pre-render' scanline.
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
// mode, which is used to record input sequences for testing.
class CNES
{
public:
   CNES();
   ~CNES();

   // Accessor methods to get/set the current video mode.
   static inline void VIDEOMODE ( int32_t mode )
   {
      m_videoMode = mode;
   }
   static inline int32_t VIDEOMODE ( void )
   {
      return m_videoMode;
   }

   // Accessor methods to get/set the controller type.
   static inline void CONTROLLER ( int32_t port, int32_t type )
   {
      m_controllerType[port] = type;
   }
   static inline int32_t CONTROLLER ( int32_t port )
   {
      return m_controllerType[port];
   }

   // Accessor method to set the controller's screen coordinates
   // for controllers that are screen-relative such as Zapper.
   static inline void CONTROLLERPOSITION ( int32_t port, int32_t px, int32_t py, int32_t wx1, int32_t wy1, int32_t wx2, int32_t wy2 )
   {
      m_controllerPositionX[port] = px;
      m_controllerPositionY[port] = py;
      m_windowX1 = wx1;
      m_windowY1 = wy1;
      m_windowX2 = wx2;
      m_windowY2 = wy2;
   }
   static inline void CONTROLLERPOSITION ( int32_t port, int32_t* px, int32_t* py, int32_t* wx1, int32_t* wy1, int32_t* wx2, int32_t* wy2 )
   {
      (*px) = m_controllerPositionX[port];
      (*py) = m_controllerPositionY[port];
      (*wx1) = m_windowX1;
      (*wy1) = m_windowY1;
      (*wx2) = m_windowX2;
      (*wy2) = m_windowY2;
   }

   // This method performs a full NES reset and initializes
   // the ROM object with the appropriate mapper index.
   static void RESET ( uint32_t mapper, bool soft );

   // This method emulates a NES video frame and passes the
   // current state of the joypad to the emulation engine.
   // The current state of the joypad is constructed from
   // intercepted keypress/keyrelease events in the UI.
   static void RUN ( uint32_t* joy );

   // Accessor methods to get/set whether or not the emulation
   // engine is in replay mode.  In replay mode the emulation runs
   // as normal but the joypad inputs are fed in from previously
   // recorded emulation runs.  At present this only works "sort of well"
   // for emulation runs that start from NES reset.
   static void REPLAY ( bool enable )
   {
      m_bReplay = enable;
   }
   static bool REPLAY ()
   {
      return m_bReplay;
   }

   // Accessor methods to get/set whether or not the emulation
   // engine is in input record mode.  If input is being recorded,
   // it can be played back on a subsequent emulation run by using
   // the REPLAY API.
   static void RECORD ( bool enable )
   {
      m_bRecord = enable;
   }
   static bool RECORD ()
   {
      return m_bRecord;
   }

   // Accessor method to retrieve the NES object's frame counter.
   // This is used by some debugger inspectors.
   static uint32_t FRAME ()
   {
      return m_frame;
   }

   // Accessor method to retrieve the execution tracer database.  Other
   // elements of the NES use this method to add entries to the database
   // during emulation.
   static inline CTracer* TRACER ( void )
   {
      return m_tracer;
   }

   // This method globally enables or disables breakpoints.  It is used
   // during an emulation hard-reset (which is caused whenever a new
   // ROM image is loaded) to prevent the emulation engine from getting
   // hung up on a set breakpoint during the ROM image switchover.
   static void BREAKPOINTS ( bool enable ) { m_bBreakpointsEnabled = enable; }

   // This method retrieves the database of currently active breakpoints.
   // It is used by the debugger inspectors to determine whether or not
   // they need to show themselves upon hitting a particular breakpoint type.
   // It is also used by the NES object in evaluating breakpoints for hits.
   static CBreakpointInfo* BREAKPOINTS ( void )
   {
      return m_breakpoints;
   }

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
   static bool ATBREAKPOINT ( void )
   {
      return m_bAtBreakpoint;
   }
   static void CLEARBREAKPOINT ( void )
   {
      m_bAtBreakpoint = false;
   }

   // These methods set a flag within the NES object indicating that a
   // breakpoint should be taken either a)after execution of the next
   // instruction by the CPU, or b)after a PPU cycle has elapsed.
   static void STEPCPUBREAKPOINT ( void );
   static void STEPPPUBREAKPOINT ( bool goFrame = false );

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

   // This method turns a 6502-based address into a printable representation
   // based on the underlying addressing scheme of the targeted memory/device.
   static void PRINTABLEADDR ( char* buffer, uint32_t addr );
   static void PRINTABLEADDR ( char* buffer, uint32_t addr, uint32_t absAddr );

protected:
   // Whether or not joypad input is being fed from the user or from
   // previously recorded emulation runs.
   static bool         m_bReplay;

   // Whether or not joypad input is being recorded during this emulation run.
   static bool         m_bRecord;

   // NTSC, or PAL?
   static int32_t             m_videoMode;

   // Controller type information
   static int32_t  m_controllerType [ NUM_CONTROLLERS ];

   // Controller screen position information (for things like zapper)
   static int32_t  m_controllerPositionX [ NUM_CONTROLLERS ];
   static int32_t  m_controllerPositionY [ NUM_CONTROLLERS ];
   static int32_t  m_windowX1;
   static int32_t  m_windowY1;
   static int32_t  m_windowX2;
   static int32_t  m_windowY2;

   // The execution tracer database.
   static CTracer*         m_tracer;

   // This is the database of active breakpoints.
   static CBreakpointInfo* m_breakpoints;
   static bool m_bBreakpointsEnabled;

   // These flags determine the breakpoint state and behavior
   // of the emulation engine.
   static bool            m_bAtBreakpoint;
   static bool            m_bStepCPUBreakpoint;
   static bool            m_bStepPPUBreakpoint;
   static int32_t         m_ppuCycleToStepTo;
   static uint32_t        m_ppuFrameToStepTo;

   // Emulation frame counter...a copy of CPPU::m_frame;
   static uint32_t m_frame;
};

#endif
