#if !defined ( NESCPU_H )
#define NESCPU_H

#include "nes_emulator_core.h"

#include "cnes.h"

#include "cmarker.h"
#include "ctracer.h"
#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cmemorydata.h"
#include "cbreakpointinfo.h"

// CPU flags register bit definitions.
#define FLAG_C    0x01
#define FLAG_Z    0x02
#define FLAG_I    0x04
#define FLAG_D    0x08
#define FLAG_B    0x10
#define FLAG_MISC 0x20
#define FLAG_V    0x40
#define FLAG_N    0x80
// CPU flags register shifting for flag register manipulations.
#define FLAG_C_SHIFT 0
#define FLAG_Z_SHIFT 1
#define FLAG_I_SHIFT 2
#define FLAG_D_SHIFT 3
#define FLAG_B_SHIFT 4
#define FLAG_V_SHIFT 6
#define FLAG_N_SHIFT 7

// Macros for accessing bytes of multi-byte entities or
// creating multi-byte entities from byte combinations.
#define GETSIGNED8(data,op) ((int8_t)((*((data)+(op)))&0xFF))
#define GETUNSIGNED8(data,op) ((uint8_t)((*((data)+(op)))&0xFF))
#define GETHI8(wd) (((wd)>>8)&0xFF)
#define GETLO8(wd) ((wd)&0xFF)

// CPU stack manipuation macros.
#define GETSTACKADDR() (MAKE16(rSP(),0x01))
#define GETSTACKDATA() (MEM(GETSTACKADDR()))

// CPU program counter manipulation macros.
#define rPC() (uint32_t)(m_pc)
#define wPC(pc) { m_pc = (pc); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_PC); }
#define INCPC() { m_pc++; CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_PC); }

// CPU stack pointer manipulation macros.
#define rSP() (m_sp)
#define wSP(sp) { m_sp = (sp); }
#define DECSP() { m_sp--;  CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_SP); }
#define INCSP() { m_sp++;  CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_SP); }
#define PUSH(data) { MEM(GETSTACKADDR(),(data)); DECSP(); }

// The effective address is the calculated address for a
// memory-affecting CPU operation based on the addressing
// mode of the instruction.  It is kept track of by the CPU
// core so that the debugging inspectors can display the
// absolute physical address being manipulated by the CPU
// for any given instruction executed.
// NOTE: INTERNAL MACROS
#define rEA() (m_ea)
#define wEA(ea) { m_ea = (ea); }

// CPU accumulator, X, and Y register manipulation macros.
// NOTE: INTERNAL MACROS
#define rA() (m_a)
#define rX() (m_x)
#define rY() (m_y)
#define wA(a) { m_a = (a); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_A); }
#define wX(x) { m_x = (x); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_X); }
#define wY(y) { m_y = (y); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_Y); }

// CPU flags register manipulation macros.  Used by instructions
// that manipulate the flags register as a complete set rather than
// as individual flag bits.
// NOTE: INTERNAL MACROS
#define rF() (m_f|FLAG_MISC)
#define wF(f) { m_f = ((f)|FLAG_MISC); }

// CPU flag bit manipulation macros.  Used by instructions that
// manipulate individual flag bits within the CPU flags register.
// NOTE: INTERNAL MACROS
#define rN() (!!(m_f&FLAG_N))
#define rV() (!!(m_f&FLAG_V))
#define rB() (!!(m_f&FLAG_B))
#define rD() (!!(m_f&FLAG_D))
#define rI() (!!(m_f&FLAG_I))
#define rZ() (!!(m_f&FLAG_Z))
#define rC() (!!(m_f&FLAG_C))
#define sN() { m_f|=FLAG_N; }
#define cN() { m_f&=~(FLAG_N); }
#define sV() { m_f|=FLAG_V; }
#define cV() { m_f&=~(FLAG_V); }
#define sB() { m_f|=FLAG_B; }
#define cB() { m_f&=~(FLAG_B); }
#define sD() { m_f|=FLAG_D; }
#define cD() { m_f&=~(FLAG_D); }
#define sI() { m_f|=FLAG_I; }
#define cI() { m_f&=~(FLAG_I); }
#define sZ() { m_f|=FLAG_Z; }
#define cZ() { m_f&=~(FLAG_Z); }
#define sC() { m_f|=FLAG_C; }
#define cC() { m_f&=~(FLAG_C); }
#define wN(set) { m_f&=(~(FLAG_N)); m_f|=((!!(set))<<FLAG_N_SHIFT); }
#define wV(set) { m_f&=(~(FLAG_V)); m_f|=((!!(set))<<FLAG_V_SHIFT); }
#define wB(set) { m_f&=(~(FLAG_B)); m_f|=((!!(set))<<FLAG_B_SHIFT); }
#define wD(set) { m_f&=(~(FLAG_D)); m_f|=((!!(set))<<FLAG_D_SHIFT); }
#define wI(set) { m_f&=(~(FLAG_I)); m_f|=((!!(set))<<FLAG_I_SHIFT); }
#define wZ(set) { m_f&=(~(FLAG_Z)); m_f|=((!!(set))<<FLAG_Z_SHIFT); }
#define wC(set) { m_f&=(~(FLAG_C)); m_f|=((!!(set))<<FLAG_C_SHIFT); }

// The C6502 class is the implementation of the core CPU of the NES.
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
class C6502
{
public:
   C6502();
   ~C6502();

   // Emulation routines.
   static void EMULATE ( int32_t cycles );
   static void GOTO ( uint32_t pcGoto )
   {
      m_pcGoto = pcGoto;
   }
   static void GOTO ()
   {
      m_pcGoto = 0xFFFFFFFF;
   }

   // CPU reset vector routine.
   static void RESET ( bool soft );

   // Routines to manipulate the IRQ/NMI inputs to the CPU core.
   static void ASSERTIRQ ( int8_t source );
   static void RELEASEIRQ ( int8_t source );
   static void ASSERTNMI ();

   // NMI may be choked by the PPU if the CPU interacts with the
   // PPU in a specific fashion on a PPU cycle too close to the
   // assertion of NMI to the CPU.
   static void CHOKENMI ()
   {
      m_nmiAsserted = false;
   }

   // The CPU BRK instruction and also the handler routines for
   // NMI and IRQ interrupts, since the behavior of BRK, IRQ, and NMI
   // is very similar.  (A NMI or IRQ is actually treated like an
   // instruction [BRK] in the instruction stream.)
   static void BRK ( void );

   // CPU instruction execution routines.  Each routine
   // contains the logic to execute all addressing mode
   // variants of the particular instruction.
   // The documented opcodes:
   static void ADC ( void );
   static void AND ( void );
   static void ASL ( void );
   static void BCC ( void );
   static void BCS ( void );
   static void BEQ ( void );
   static void BIT ( void );
   static void BMI ( void );
   static void BNE ( void );
   static void BPL ( void );
   static void BVC ( void );
   static void BVS ( void );
   static void CLC ( void );
   static void CLD ( void );
   static void CLI ( void );
   static void CLV ( void );
   static void CMP ( void );
   static void CPX ( void );
   static void CPY ( void );
   static void DEC ( void );
   static void DEX ( void );
   static void DEY ( void );
   static void EOR ( void );
   static void INC ( void );
   static void INX ( void );
   static void INY ( void );
   static void JMP ( void );
   static void JSR ( void );
   static void LDA ( void );
   static void LDY ( void );
   static void LDX ( void );
   static void LSR ( void );
   static void NOP ( void );
   static void ORA ( void );
   static void PHA ( void );
   static void PHP ( void );
   static void PLA ( void );
   static void PLP ( void );
   static void ROL ( void );
   static void ROR ( void );
   static void RTI ( void );
   static void RTS ( void );
   static void SBC ( void );
   static void SEC ( void );
   static void SED ( void );
   static void SEI ( void );
   static void STA ( void );
   static void STX ( void );
   static void STY ( void );
   static void TAX ( void );
   static void TAY ( void );
   static void TSX ( void );
   static void TXA ( void );
   static void TXS ( void );
   static void TYA ( void );

   // The undocumented opcodes:
   static void ASO ( void );
   static void AXS ( void );
   static void ANC ( void );
   static void ALR ( void );
   static void ARR ( void );
   static void DCM ( void );
   static void INS ( void );
   static void LAX ( void );
   static void LSE ( void );
   static void DOP ( void );
   static void TOP ( void );
   static void RLA ( void );
   static void RRA ( void );
   static void XAA ( void );
   static void AXA ( void );
   static void TAS ( void );
   static void SAY ( void );
   static void XAS ( void );
   static void OAL ( void );
   static void LAS ( void );
   static void SAX ( void );

   // The illegal opcodes:
   static void KIL ( void );

   // Accessor methods for retrieving information from or about
   // the managed entities within the CPU core object.
   // These routines are used by the debugger, not the emulator core.
   // Return the currently calculated effective address.
   static uint32_t _EA ( void )
   {
      return m_ea;
   }

   // Return the contents of a memory location visible to the CPU.
   static uint8_t _MEM ( uint32_t addr );

   // Modify the contents of a memory location visible to the CPU.
   static void _MEM ( uint32_t addr, uint8_t data );

   // Retrieve a pointer to the whole memory.
   static uint8_t* _MEMPTR ( void )
   {
      return m_6502memory;
   }

   // Return whether or not the CPU is currently in the middle of
   // the first cycle of an instruction fetch (the opcode fetch).
   static bool _SYNC ( void )
   {
      return (m_instrCycle==0);
   }

   // Return whether or not the CPU is currently in the middle of
   // a write memory cycle.
   static bool _WRITING ( void )
   {
      return m_write;
   }

   // Return the current cycle index of the CPU core.
   // The cycle index is a free-running counter of executed CPU cycles.
   // It will roll-over after approximately 40 minutes of emulation.
   // But, roll-over of this counter is not a significant event.
   static inline uint32_t _CYCLES ( void )
   {
      return m_cycles;
   }

   // Accessor methods to set up or clear the state of the RAM
   // maintained internally by the CPU core object.
   static void MEMSET ( uint32_t addr, uint8_t* data, uint32_t length )
   {
      memcpy(m_6502memory+addr,data,length);
   };
   static void MEMCLR ( void )
   {
      memset(m_6502memory,0,MEM_2KB);
   }

   // Method to return the current open bus data.
   static uint8_t OPENBUS () { return m_openBusData; }

   // DMA driver method.
   static bool DMA ( void );

   // Accessor methods for supporting DMA between the CPU and APU/PPU.
   static uint8_t DMA ( uint32_t addr );
   static void DMA ( uint32_t srcAddr, uint32_t dstAddr, uint8_t data );

   // The APU can request a DMA.
   static void APUDMAREQ ( uint16_t addr );

   // Accessor method for stack popping.
   static inline uint32_t POP ( void )
   {
      INCSP();
      return GETSTACKDATA();
   }

   // Accessor methods for manipulating CPU core registers.
   // These routines are used by the debugger, not by the
   // emulator core.
   static uint32_t __PC ( void )
   {
      return m_pc;
   }
   static uint32_t __PCSYNC ( void )
   {
      if ( m_pcSyncSet )
      {
         return m_pcSync;
      }
      else
      {
         return m_pc;
      }
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
   static uint32_t _N ( void )
   {
      return (!!(m_f&FLAG_N));   // Negative
   }
   static uint32_t _V ( void )
   {
      return (!!(m_f&FLAG_V));   // Overflow
   }
   static uint32_t _B ( void )
   {
      return (!!(m_f&FLAG_B));   // Break command
   }
   static uint32_t _D ( void )
   {
      return (!!(m_f&FLAG_D));   // Decimal mode
   }
   static uint32_t _I ( void )
   {
      return (!!(m_f&FLAG_I));   // Interrupt disable
   }
   static uint32_t _Z ( void )
   {
      return (!!(m_f&FLAG_Z));   // Zero
   }
   static uint32_t _C ( void )
   {
      return (!!(m_f&FLAG_C));   // Carry
   }
   static void _N ( uint32_t set )
   {
      m_f&=~(FLAG_N);
      m_f|=((!!set)<<FLAG_N_SHIFT);
   }
   static void _V ( uint32_t set )
   {
      m_f&=~(FLAG_V);
      m_f|=((!!set)<<FLAG_V_SHIFT);
   }
   static void _B ( uint32_t set )
   {
      m_f&=~(FLAG_B);
      m_f|=((!!set)<<FLAG_B_SHIFT);
   }
   static void _D ( uint32_t set )
   {
      m_f&=~(FLAG_D);
      m_f|=((!!set)<<FLAG_D_SHIFT);
   }
   static void _I ( uint32_t set )
   {
      m_f&=~(FLAG_I);
      m_f|=((!!set)<<FLAG_I_SHIFT);
   }
   static void _Z ( uint32_t set )
   {
      m_f&=~(FLAG_Z);
      m_f|=((!!set)<<FLAG_Z_SHIFT);
   }
   static void _C ( uint32_t set )
   {
      m_f&=~(FLAG_C);
      m_f|=((!!set)<<FLAG_C_SHIFT);
   }

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

   // Disassembly routines for display.
   static void DISASSEMBLE ();
   static void DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint16_t* addr2sloc, uint32_t* sourceLength );
   static char* Disassemble ( uint8_t* pOpcode, char* buffer );

   static inline CCodeDataLogger* LOGGER ( void )
   {
      return m_logger;
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

   // This API tells the emulator core whether or not to force a
   // breakpoint if a KIL opcode is executed.
   static void BREAKONKIL(bool breakOnKIL)
   {
      m_breakOnKIL = breakOnKIL;
   }

   // Interface to retrieve the database of CPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the CPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  CPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS()
   {
      return m_tblBreakpointEvents;
   }
   static int32_t NUMBREAKPOINTEVENTS()
   {
      return m_numBreakpointEvents;
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
      *(m_RAMopcodeMask+(addr&MEM_2KB)) = mask;
   }
   static inline void OPCODEMASKCLR ( void )
   {
      int32_t idx;
      for ( idx = 0; idx < MEM_2KB; idx++ )
      {
         m_RAMopcodeMask[idx] = 0;
      }
   }
   static inline char* DISASSEMBLY ( uint32_t addr )
   {
      return *(m_RAMdisassembly+addr);
   }
   static uint32_t SLOC2ADDR ( uint16_t sloc )
   {
      return *(m_RAMsloc2addr+sloc);
   }
   static uint16_t ADDR2SLOC ( uint32_t addr )
   {
      return *(m_RAMaddr2sloc+addr);
   }
   static inline uint16_t SLOC ()
   {
      return m_RAMsloc;
   }

   static inline uint32_t WRITEDMAADDR()
   {
      return (512-m_writeDmaCounter)>>1;
   }

protected:
   // Routine to calculate the effective address of a particular
   // instruction addressing mode based on the internal state of the CPU.
   static inline uint32_t MAKEADDR ( int32_t amode, uint8_t* data );

   // Routine to drive APU and CPU synchronization by cycles.
   static void ADVANCE ( bool stealing = false );

   // Routines to access the RAM maintained by the CPU core object.
   // These are used internally by the CPU core during emulation.
   static uint8_t MEM ( uint32_t addr );
   static void MEM ( uint32_t addr, uint8_t data );
   static uint8_t FETCH ();
   static uint8_t EXTRAFETCH ();
   static uint8_t STEAL ( uint32_t addr, uint8_t source );
   static uint8_t LOAD ( uint32_t addr, int8_t* pTarget );
   static void STORE ( uint32_t addr, uint8_t data, int8_t* pTarget );

   // Is the CPU currently locked due to execution of an
   // illegal instruction?  Illegal instructions are all KIL opcodes.
   static bool            m_killed;

   // Has an IRQ been asserted to the CPU core?
   static bool            m_irqAsserted;

   // Was IRQ asserted when checked?
   static bool            m_irqPending;

   // Has NMI been asserted to the CPU core?
   static bool            m_nmiAsserted;

   // Was NMI asserted when checked?
   static bool            m_nmiPending;

   // The CPU core maintains the 2KB of RAM visible to the CPU.
   static uint8_t*  m_6502memory;

   // The CPU core registers.
   static uint8_t   m_a;
   static uint8_t   m_x;
   static uint8_t   m_y;
   static uint8_t   m_f;
   static uint16_t  m_pc;
   static uint16_t  m_pcSync;
   static bool      m_pcSyncSet;
   static uint8_t   m_sp;

   // The effective address calculated by the CPU core.
   static uint32_t    m_ea;

   // The address to break at on a "run to here" go.
   static uint32_t            m_pcGoto;

   // Running counter of CPU cycles executed.  Will roll over in
   // approximately 40 minutes of emulation.
   static uint32_t    m_cycles;
   static int32_t     m_instrCycle;

   // The current number of CPU cycles ready to be executed by
   // the CPU core.
   static int32_t             m_curCycles; // must be allowed to go negative!

   // The following data is used internally by the CPU core
   // during instruction execution.  As opcodes are fetched and
   // decoded, relevant information about the opcode is stored
   // in these variables to avoid passing all of this information
   // on the stack frame during instruction execution via
   // function-pointer invocation.
   // The current opcode's addressing mode.
   static int32_t             amode;

   // DMC DMA request active flag.
   static int32_t m_dmaRequest;

   // DMA address for DMA write transfers.  The CPU sets this on a DMA
   // request from a write to $4014, then begins its DMA transfer at the
   // appropriate time.  When not DMAing the counter will be 0.
   static uint16_t m_writeDmaAddr;
   static int32_t m_writeDmaCounter;

   // DMA address for DMA read transfers.  The APU sets this on a DMA
   // request for a DMC channel sample, then begins its DMA transfer at the
   // appropriate time.  When not DMAing the counter will be 0.
   static uint16_t m_readDmaAddr;
   static int32_t m_readDmaCounter;

   // The current opcode's full 1-, 2-, or 3-byte instruction data.
   static uint8_t*  data;
   static uint8_t   opcodeData [ 4 ]; // 3 opcode bytes and 1 byte for operand return data [extra cycle]

   // The current opcode's table entry (see struct _CNES6502_opcode below).
   static struct _CNES6502_opcode* pOpcodeStruct;

   // The size of the current opcode in bytes (1, 2, or 3).
   static int32_t             opcodeSize;

   // Whether or not the CPU is in a write memory cycle.
   static bool            m_write;

   // Open bus data to be returned if reading an unconnected memory region.
   static uint8_t m_openBusData;

   // Which phase of instruction fetching is the CPU core in?
   // m_phase will be 0 during the opcode fetch.  m_phase will go
   // up to 1 if the fetched opcode is 1-byte (extra fetch cycle) or 2-bytes.
   // m_phase will go up to 3 if the fetched opcode is 3-byte.
   // Then m_phase goes to -1 for the instruction execution.
   static int8_t            m_phase;

   // This points to the last execution tracer tag that
   // is where the disassembly of the instruction should
   // be placed.
   static TracerInfo* pDisassemblySample;

   // Database used by the Execution Visualizer debugger inspector.
   // The data structure is maintained by the CPU core as it executes
   // instructions that are marked.
   static CMarker*         m_marker;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the CPU core as it performs fetches, reads,
   // writes, and DMA transfers to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger* m_logger;

   // The database for CPU core registers.  Declaration
   // is in source file.
   static CRegisterDatabase* m_dbRegisters;

   // The database for CPU RAM.  Declaration is in source file.
   static CMemoryDatabase* m_dbMemory;

   // Configuration from EmulatorPrefs.
   static bool m_breakOnKIL;

   // The database for CPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;

   // The data structures that support runtime disassembly of executed code.
   static uint8_t*   m_RAMopcodeMask;
   static char**           m_RAMdisassembly;
   static uint16_t*  m_RAMsloc2addr;
   static uint16_t*  m_RAMaddr2sloc;
   static uint32_t    m_RAMsloc;
};

// Structure representing each instruction and
// addressing mode possible within the CPU core.
typedef struct _CNES6502_opcode
{
   // Self-referential index.
   uint8_t op;

   // Instruction printable name.
   const char* name;

   // Instruction execution function.
   void (*pFn)(void);

   // Addressing mode of this particular entry.
   uint8_t amode;

   // Number of CPU cycles required to execute this particular entry.
   uint8_t cycles;

   // Is the instruction part of the documented 6502 ISA?
   uint8_t documented;

   // Do we force an extra cycle for this instruction variant?
   uint8_t forceExtraCycle;

   // Which cycles of the instruction do interrupts get checked on?
   // NOTE: This is a bitmap to save space since instructions are never > 8 cycles.
   uint8_t checkInterruptCycleMap;
} CNES6502_opcode;

#endif
