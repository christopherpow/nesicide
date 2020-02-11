#if !defined ( C64CPU_H )
#define C64CPU_H

#include "c64_emulator_core.h"

//#include "cmarker.h"
//#include "ctracer.h"
//#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cmemorydata.h"
#include "cc64breakpointinfo.h"

// The CC646502 class is the implementation of the core CPU of the NES.
// It provides CPU-fetch-cycle granular emulation of the CPU core, including
// undocumented and illegal instructions.  It handles vectoring to
// the three interrupt sources (reset, NMI, IRQ) at the appropriate
// times.  It is implemented as a static object so that accesses to its
// internal data via accessor functions does not require a class object.
//
// The class internally maintains the state of the CPU's registers
// and of the 2KB of RAM that is on the NES mainboard, dedicated to the CPU.
// The class also maintains several data structures used by debugger inspectors.
//
// CPU instruction dispatch is via a main execution method that is
// invoked by other elements of the NES architecture (see PPU) whenever
// there is at least one CPU cycle time that needs to be
// executed.  This main execution method invokes a step method which
// executes CPU instructions until there are no CPU cycles remaining.
//
// The execution step method performs the required number of instruction
// fetch cycles (1, 2, or 3 depending on addressing mode of fetched opcode),
// and then uses a table-of-function-pointers dispatch method
// to execute instructions once the instruction fetch cycles are completed.
// The step method returns control to the caller after each phase of the
// instruction fetch to provide CPU-cycle granular accuracy with respect
// to the calling element's cycle.  For example, if the PPU invokes the
// CPU's execute method with only one CPU cycle needing to be executed, the
// CPU will execute one fetch cycle and immediately return.  However,
// if the CPU is ready to execute a fetched instruction it will finish the
// execution of the instruction even though it was asked to only execute
// one actual CPU cycle.  This is a limitation that could be removed in
// future updates to the CPU core, but currently adequate accuracy is obtained
// with this method.
class CC646502
{
public:
   CC646502();
   virtual ~CC646502();

#if 0
   // Interface to retrieve the database of execution markers.
   // Execution markers maintain PPU-cycle/frame counts for the
   // start/finish of marked sections of code.  Whenever the
   // CPU executes an opcode marked as the start or finish of
   // a marked section of code, it updates the database of markers.
   // The Execution Visualizer debugger inspector displays this
   // database visually.
   static CMarker* MARKERS()
   {
      return m_marker;
   }
#endif

   // Disassembly routines for display.
   static void DISASSEMBLE ();
   static void DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint32_t* addr2sloc, uint32_t* sourceLength );
   static char* Disassemble ( uint8_t* pOpcode, char* buffer );

#if 0
   static inline CCodeDataLogger* LOGGER ( void )
   {
      return m_logger;
   }
#endif

   static void GOTO ( uint32_t pcGoto )
   {
      m_pcGoto = pcGoto;
   }
   static uint32_t GOTO ()
   {
      uint32_t _goto = m_pcGoto;
      m_pcGoto = 0xFFFFFFFF;
      return _goto;
   }

   // Accessor methods for manipulating CPU core registers.
   // These routines are used by the debugger, not by the
   // emulator core.
   static uint32_t __PC ( void )
   {
      return m_pc;
   }
   static void __PC ( uint16_t pc )
   {
      m_pc = pc;
   }
   static uint32_t _SP ( void )
   {
      return m_sp;
   }
   static void _SP ( uint8_t sp )
   {
      m_sp = sp;
   }
   static uint32_t _A ( void )
   {
      return m_a;
   }
   static void _A ( uint8_t a )
   {
      m_a = a;
   }
   static uint32_t _X ( void )
   {
      return m_x;
   }
   static void _X ( uint8_t x )
   {
      m_x = x;
   }
   static uint32_t _Y ( void )
   {
      return m_y;
   }
   static void _Y ( uint8_t y )
   {
      m_y = y;
   }
   static uint32_t _F ( void )
   {
      return m_f;
   }
   static void _F ( uint8_t f )
   {
      m_f = f;
   }

   // Return the contents of a memory location visible to the CPU.
   static uint8_t _MEM ( uint32_t addr )
   {
      return *(m_6502memory+addr);
   }

   // Modify the contents of a memory location visible to the CPU.
   static void _MEM ( uint32_t addr, uint8_t data )
   {
      *(m_6502memory+addr) = data;
   }

   // Interface to retrieve the database defining the registers
   // of the CPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the CPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // CPU registers are declared in the source file.
   static CRegisterDatabase* REGISTERS()
   {
      return m_dbRegisters;
   }

   // Interface to retrieve the database defining the RAM
   // of the CPU core in a "human readable" form that is used by
   // the Memory-type debugger inspector.
   static CMemoryDatabase* MEMORY()
   {
      return m_dbMemory;
   }

   // This method retrieves the database of currently active breakpoints.
   // It is used by the debugger inspectors to determine whether or not
   // they need to show themselves upon hitting a particular breakpoint type.
   // It is also used by the NES object in evaluating breakpoints for hits.
   static CBreakpointInfo* BREAKPOINTS ( void )
   {
      return m_breakpoints;
   }

   // The following routines are support for the runtime
   // disassembly of RAM if it is executed by the CPU core.
   // An "opcode mask" is tracked for each byte of accessible
   // RAM.  If a memory location is fetched by the CPU as an
   // opcode (see _SYNC), the memory location is marked
   // as having been executed.  Disassembly of the executed
   // instruction is generated real-time.  If the debuggers
   // need to display disassembly of a memory location they
   // retrieve it from the database generated by the runtime
   // disassembler.
   static inline void OPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      *(m_RAMopcodeMask+addr) = mask;
   }
   static inline void OPCODEMASKCLR ( void )
   {
      int32_t idx;
      for ( idx = 0; idx < MEM_64KB; idx++ )
      {
         m_RAMopcodeMask[idx] = 0;
      }
   }
   static inline char* DISASSEMBLY ( uint32_t addr )
   {
      return *(m_RAMdisassembly+addr);
   }
   static inline char* DISASSEMBLYATPHYSADDR ( uint32_t absAddr, char* buffer )
   {
      return CC646502::Disassemble(m_6502memory+absAddr,buffer);
   }
   static uint16_t SLOC2VIRTADDR ( uint32_t sloc )
   {
      return *(m_RAMsloc2addr+sloc);
   }
   static uint32_t ADDR2SLOC ( uint16_t addr )
   {
      return *(m_RAMaddr2sloc+addr);
   }
   static inline uint32_t SLOC ()
   {
      return m_RAMsloc;
   }

protected:
   // The CPU core maintains the 64KB of RAM visible to the CPU.
   static uint8_t*  m_6502memory;

   // The CPU core registers.
   static uint8_t   m_a;
   static uint8_t   m_x;
   static uint8_t   m_y;
   static uint8_t   m_f;
   static uint16_t  m_pc;
   static uint8_t   m_sp;

   // The address to break at on a "run to here" go.
   static uint32_t            m_pcGoto;

#if 0
   // Database used by the Execution Visualizer debugger inspector.
   // The data structure is maintained by the CPU core as it executes
   // instructions that are marked.
   static CMarker*         m_marker;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the CPU core as it performs fetches, reads,
   // writes, and DMA transfers to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger* m_logger;
#endif

   // The database for CPU core registers.  Declaration
   // is in source file.
   static CRegisterDatabase* m_dbRegisters;

   // The database for CPU RAM.  Declaration is in source file.
   static CMemoryDatabase* m_dbMemory;

   // This is the database of active breakpoints.
   static CBreakpointInfo* m_breakpoints;

   // The data structures that support runtime disassembly of executed code.
   static uint8_t*   m_RAMopcodeMask;
   static char**           m_RAMdisassembly;
   static uint16_t*  m_RAMsloc2addr;
   static uint32_t*  m_RAMaddr2sloc;
   static uint32_t    m_RAMsloc;
};

// Structure representing each instruction and
// addressing mode possible within the CPU core.
typedef struct _CC646502_opcode
{
   // Self-referential index.
   int32_t op;

   // Instruction printable name.
   const char* name;

   // Addressing mode of this particular entry.
   int32_t amode;

   // Number of CPU cycles required to execute this particular entry.
   int32_t cycles;

   // Is the instruction part of the documented 6502 ISA?
   bool documented;

   // Do we force an extra cycle for this instruction variant?
   bool forceExtraCycle;

   // Which cycles of the instruction do interrupts get checked on?
   // NOTE: This is a bitmap to save space since instructions are never > 8 cycles.
   uint8_t checkInterruptCycleMap;
} CC646502_opcode;

#endif
