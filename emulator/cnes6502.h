#if !defined ( NESCPU_H )
#define NESCPU_H

#include "cnesicidecommon.h"

#include "ctracer.h"

#include "cmarker.h"

#include "ccodedatalogger.h"
#include "cregisterdata.h"

#include "cnes.h"

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   CPU_EVENT_EXECUTE_EXACT = 0,
   CPU_EVENT_UNDOCUMENTED,
   CPU_EVENT_UNDOCUMENTED_EXACT,
   CPU_EVENT_RESET,
   CPU_EVENT_IRQ,
   CPU_EVENT_NMI,
   NUM_CPU_EVENTS
};

// CPU interrupt vector memory addresses.
#define VECTOR_NMI   0xFFFA
#define VECTOR_RESET 0xFFFC
#define VECTOR_IRQ   0xFFFE

// Cycles to claim as executed for NMI/IRQ execution.
#define NMI_CYCLES 2
#define IRQ_CYCLES 2

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

// CPU instruction addressing modes.
#define AM_IMPLIED              0
#define AM_IMMEDIATE            1
#define AM_ABSOLUTE             2
#define AM_ZEROPAGE             3
#define AM_ACCUMULATOR          4
#define AM_ABSOLUTE_INDEXED_X   5
#define AM_ABSOLUTE_INDEXED_Y   6
#define AM_ZEROPAGE_INDEXED_X   7
#define AM_ZEROPAGE_INDEXED_Y   8
#define AM_INDIRECT             9
#define AM_PREINDEXED_INDIRECT  10
#define AM_POSTINDEXED_INDIRECT 11
#define AM_RELATIVE             12
#define NUM_ADDRESSING_MODES    13

// Macros for accessing bytes of multi-byte entities or
// creating multi-byte entities from byte combinations.
#define MAKE16(lo,hi) ((((lo)&0xFF)|(((hi)&0xFF)<<8)))
#define GETSIGNED8(data,op) ((char)((*((data)+(op)))&0xFF))
#define GETUNSIGNED8(data,op) ((unsigned char)((*((data)+(op)))&0xFF))
#define GETHI8(wd) (((wd)>>8)&0xFF)
#define GETLO8(wd) ((wd)&0xFF)

// CPU stack manipuation macros.
#define GETSTACKADDR() (MAKE16(rSP(),0x01))
#define GETSTACKDATA() (MEM(GETSTACKADDR()))

// CPU program counter manipulation macros.
#define rPC() (UINT)(m_pc)
#define wPC(pc) { m_pc = (pc); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,0); }
#define INCPC() { m_pc++; }
#define DECPC() { m_pc--; }

// CPU stack pointer manipulation macros.
#define rSP() (m_sp)
#define wSP(sp) { m_sp = (sp); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,4); }
#define DECSP() m_sp--
#define INCSP() m_sp++
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
#define wA(a) { m_a = (a); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,1); }
#define rX() (m_x)
#define wX(x) { m_x = (x); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,2); }
#define rY() (m_y)
#define wY(y) { m_y = (y); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,3); }

// CPU flags register manipulation macros.  Used by instructions
// that manipulate the flags register as a complete set rather than
// as individual flag bits.
// NOTE: INTERNAL MACROS
#define rF() (m_f|FLAG_MISC)
#define wF(f) { m_f = ((f)|FLAG_MISC); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }

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
#define wN(set) { m_f&=(~(FLAG_N)); m_f|=((!!(set))<<FLAG_N_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sV() { m_f|=FLAG_V; }
#define cV() { m_f&=~(FLAG_V); }
#define wV(set) { m_f&=(~(FLAG_V)); m_f|=((!!(set))<<FLAG_V_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sB() { m_f|=FLAG_B; }
#define cB() { m_f&=~(FLAG_B); }
#define wB(set) { m_f&=(~(FLAG_B)); m_f|=((!!(set))<<FLAG_B_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sD() { m_f|=FLAG_D; }
#define cD() { m_f&=~(FLAG_D); }
#define wD(set) { m_f&=(~(FLAG_D)); m_f|=((!!(set))<<FLAG_D_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sI() { m_f|=FLAG_I; }
#define cI() { m_f&=~(FLAG_I); }
#define wI(set) { m_f&=(~(FLAG_I)); m_f|=((!!(set))<<FLAG_I_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sZ() { m_f|=FLAG_Z; }
#define cZ() { m_f&=~(FLAG_Z); }
#define wZ(set) { m_f&=(~(FLAG_Z)); m_f|=((!!(set))<<FLAG_Z_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }
#define sC() { m_f|=FLAG_C; }
#define cC() { m_f&=~(FLAG_C); }
#define wC(set) { m_f&=(~(FLAG_C)); m_f|=((!!(set))<<FLAG_C_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,5); }

// Routine to retrieve the ToolTip information for a particular opcode.
const char* OPCODEINFO ( unsigned char op );

// Routine to retrieve the size of the specified opcode.
unsigned char OPCODESIZE ( unsigned char op );

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
   virtual ~C6502();

   // Disassembly routines for display.
   static void DISASSEMBLE ();
   static void DISASSEMBLE ( char** disassembly, unsigned char* binary, int binaryLength, unsigned char* opcodeMask, unsigned short* sloc2addr, unsigned short* addr2sloc, unsigned int* sourceLength );
   static char* Disassemble ( unsigned char* pOpcode, char* buffer );

   // Emulation routines.
   static void EMULATE ( int cycles );
   static void GOTO ( UINT pcGoto ) { m_pcGoto = pcGoto; }
   static void GOTO () { m_pcGoto = 0xFFFFFFFF; }
   static void STEP ( void );

   // CPU reset vector routine.
   static void RESET ( void );

   // Routines to manipulate the IRQ/NMI inputs to the CPU core.
   static void ASSERTIRQ ( char source );
   static void RELEASEIRQ ( char source );
   static void ASSERTNMI ();

   // NMI may be choked by the PPU if the CPU interacts with the
   // PPU in a specific fashion on a PPU cycle too close to the
   // assertion of NMI to the CPU.
   static void CHOKENMI () { m_nmiAsserted = false; }

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
   static void BRK ( void );
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

   // Accessor methods for manipulating CPU core registers.
   // These routines are used by the debugger, not by the
   // emulator core.
   static UINT __PC ( void ) { return m_pc; }
   static void __PC ( unsigned short pc ) { m_pc = pc; }
   static UINT _SP ( void ) { return m_sp; }
   static void _SP ( unsigned char sp ) { m_sp = sp; }
   static inline UINT POP ( void ) { INCSP(); return GETSTACKDATA(); }
   static UINT _A ( void ) { return m_a; }
   static void _A ( unsigned char a ) { m_a = a; }
   static UINT _X ( void ) { return m_x; }
   static void _X ( unsigned char x ) { m_x = x; }
   static UINT _Y ( void ) { return m_y; }
   static void _Y ( unsigned char y ) { m_y = y; }
   static UINT _F ( void ) { return m_f; }
   static void _F ( unsigned char f ) { m_f = f; }
   static UINT _N ( void ) { return (!!(m_f&FLAG_N)); } // Negative
   static UINT _V ( void ) { return (!!(m_f&FLAG_V)); } // Overflow
   static UINT _B ( void ) { return (!!(m_f&FLAG_B)); } // Break command
   static UINT _D ( void ) { return (!!(m_f&FLAG_D)); } // Decimal mode
   static UINT _I ( void ) { return (!!(m_f&FLAG_I)); } // Interrupt disable
   static UINT _Z ( void ) { return (!!(m_f&FLAG_Z)); } // Zero
   static UINT _C ( void ) { return (!!(m_f&FLAG_C)); } // Carry
   static void _N ( UINT set ) { m_f&=~(FLAG_N); m_f|=((!!set)<<FLAG_N_SHIFT); }
   static void _V ( UINT set ) { m_f&=~(FLAG_V); m_f|=((!!set)<<FLAG_V_SHIFT); }
   static void _B ( UINT set ) { m_f&=~(FLAG_B); m_f|=((!!set)<<FLAG_B_SHIFT); }
   static void _D ( UINT set ) { m_f&=~(FLAG_D); m_f|=((!!set)<<FLAG_D_SHIFT); }
   static void _I ( UINT set ) { m_f&=~(FLAG_I); m_f|=((!!set)<<FLAG_I_SHIFT); }
   static void _Z ( UINT set ) { m_f&=~(FLAG_Z); m_f|=((!!set)<<FLAG_Z_SHIFT); }
   static void _C ( UINT set ) { m_f&=~(FLAG_C); m_f|=((!!set)<<FLAG_C_SHIFT); }

   // Accessor methods for retrieving information from or about
   // the managed entities within the CPU core object.
   // These routines are used by the debugger, not the emulator core.
   // Return the currently calculated effective address.
   static UINT _EA ( void ) { return m_ea; }

   // Return the contents of a memory location visible to the CPU.
   static unsigned char _MEM ( UINT addr );

   // Modify the contents of a memory location visible to the CPU.
   static void _MEM ( UINT addr, unsigned char data );

   // Return whether or not the CPU is currently in the middle of
   // the first cycle of an instruction fetch (the opcode fetch).
   static bool _SYNC ( void ) { return m_sync; }

   // Return the current cycle index of the CPU core.
   // The cycle index is a free-running counter of executed CPU cycles.
   // It will roll-over after approximately 40 minutes of emulation.
   // But, roll-over of this counter is not a significant event.
   static inline unsigned int _CYCLES ( void ) { return m_cycles; }

   // Accessor methods to set up or clear the state of the RAM
   // maintained internally by the CPU core object.
   static void MEMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_6502memory+addr,data,length); };
   static void MEMCLR ( void ) { memset(m_6502memory,0,MEM_2KB); }

   // Accessor method for supporting DMA from the RAM maintained
   // internally by the CPU core object.  The APU and the
   // PPU both use this method to transfer data from the CPU memory space.
   static unsigned char DMA ( UINT addr, char source );

   // The PPU and APU can steal cycles from the CPU in addition to
   // doing DMA transfers (actually, the DMA engine steals the cycles
   // but the DMA engine is invoked by the PPU or APU).
   static void STEALCYCLES ( int cycles ) { for ( int i = 0; i < cycles; i++ ) ADVANCE(); }

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
   static inline void OPCODEMASK ( UINT addr, unsigned char mask ) { *(m_RAMopcodeMask+addr) = mask; }
   static inline void OPCODEMASKCLR ( void ) { memset(m_RAMopcodeMask,0,sizeof(m_RAMopcodeMask)); }
   static inline char* DISASSEMBLY ( UINT addr ) { return *(m_RAMdisassembly+addr); }
   static UINT SLOC2ADDR ( unsigned short sloc ) { return *(m_RAMsloc2addr+sloc); }
   static unsigned short ADDR2SLOC ( UINT addr ) { return *(m_RAMaddr2sloc+addr); }
   static inline unsigned short SLOC () { return m_RAMsloc; }

   // Interface to retrieve the database defining the registers
   // of the CPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the CPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // CPU registers are declared in the source file.
   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static inline int NUMREGISTERS ( void ) { return m_numRegisters; }

   // Interface to retrieve the database of CPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the CPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  CPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // Interface to retrieve the database of execution markers.
   // Execution markers maintain PPU-cycle/frame counts for the
   // start/finish of marked sections of code.  Whenever the
   // CPU executes an opcode marked as the start or finish of
   // a marked section of code, it updates the database of markers.
   // The Execution Visualizer debugger inspector displays this
   // database visually.
   static CMarker& MARKERS() { return m_marker; }

   // The CPU's Code/Data Logger display is generated by the CPU core
   // because the CPU core maintains all of the information necessary
   // to generate it.
   static inline void CodeDataLoggerInspectorTV ( char* pTV ) { m_pCodeDataLoggerInspectorTV = pTV; }
   static void RENDERCODEDATALOGGER ( void );

   // The Execution Visualizer display is generated by the CPU core
   // because the CPU core maintains all of the information necessary
   // to generate it.
   static inline void ExecutionVisualizerInspectorTV ( char* pTV ) { m_pExecutionVisualizerInspectorTV = pTV; }
   static void RENDEREXECUTIONVISUALIZER ( void );

protected:
   // Routine to calculate the effective address of a particular
   // instruction addressing mode based on the internal state of the CPU.
   static inline UINT MAKEADDR ( int amode, unsigned char* data );

   // Routine to drive APU and CPU synchronization by cycles.
   static void ADVANCE ( void );

   // CPU interrupt vector routines.
   static void IRQ ();
   static void NMI ();

   // Routines to access the RAM maintained by the CPU core object.
   // These are used internally by the CPU core during emulation.
   static unsigned char MEM ( UINT addr );
   static void MEM ( UINT addr, unsigned char data );
   static unsigned char FETCH ( UINT addr );
   static unsigned char EXTRAFETCH ( UINT addr );
   static unsigned char LOAD ( UINT addr, char* pTarget );
   static void STORE ( UINT addr, unsigned char data, char* pTarget );

protected:
   // Is the CPU currently locked due to execution of an
   // illegal instruction?  Illegal instructions are all KIL opcodes.
   static bool            m_killed;

   // Has an IRQ been asserted to the CPU core?
   static bool            m_irqAsserted;

   // Has NMI been asserted to the CPU core?
   static bool            m_nmiAsserted;

   // The CPU core maintains the 2KB of RAM visible to the CPU.
   static unsigned char   m_6502memory [ MEM_2KB ];

   // The data structures that support runtime disassembly of executed code.
   static unsigned char   m_RAMopcodeMask [ MEM_2KB ];
   static char*           m_RAMdisassembly [ MEM_2KB ];
   static unsigned short  m_RAMsloc2addr [ MEM_2KB ];
   static unsigned short  m_RAMaddr2sloc [ MEM_2KB ];
   static unsigned int    m_RAMsloc;

   // The CPU core registers.
   static unsigned char   m_a;
   static unsigned char   m_x;
   static unsigned char   m_y;
   static unsigned char   m_f;
   static unsigned short  m_pc;
   static unsigned char   m_sp;

   // The effective address calculated by the CPU core.
   static unsigned int    m_ea;

   // The address to break at on a "run to here" go.
   static UINT            m_pcGoto;

   // Database used by the Execution Visualizer debugger inspector.
   // The data structure is maintained by the CPU core as it executes
   // instructions that are marked.
   static CMarker         m_marker;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the CPU core as it performs fetches, reads,
   // writes, and DMA transfers to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger m_logger;

   // Running counter of CPU cycles executed.  Will roll over in
   // approximately 40 minutes of emulation.
   static unsigned int    m_cycles;

   // The current number of CPU cycles ready to be executed by
   // the CPU core.
   static int             m_curCycles; // must be allowed to go negative!

   // The following data is used internally by the CPU core
   // during instruction execution.  As opcodes are fetched and
   // decoded, relevant information about the opcode is stored
   // in these variables to avoid passing all of this information
   // on the stack frame during instruction execution via
   // function-pointer invocation.
   // The current opcode's addressing mode.
   static int             amode;

   // The current opcode's full 1-, 2-, or 3-byte instruction data.
   static unsigned char*  data;
   static unsigned char   opcodeData [ 4 ]; // 3 opcode bytes and 1 byte for operand return data [extra cycle]

   // This points to the last execution tracer tag that
   // is where the disassembly of the instruction should
   // be placed.
   static TracerInfo* pDisassemblySample;

   // The current opcode's table entry (see struct _C6502_opcode below).
   static struct _C6502_opcode* pOpcodeStruct;

   // The size of the current opcode in bytes (1, 2, or 3).
   static int             opcodeSize;

   // Whether or not the CPU core is in an opcode fetch cycle.
   static bool            m_sync;

   // Which phase of instruction fetching is the CPU core in?
   // Note, this is slightly different than m_sync.  m_sync is
   // *true* only during the opcode fetch.  m_phase will be 1
   // during the opcode fetch.  m_phase will go up to 2 if the fetched
   // opcode is 1-byte (extra fetch cycle) or 2-bytes.  m_phase
   // will go up to 3 if the fetched opcode is 3-byte.
   static char            m_phase;

   // The database for CPU core registers.  Declaration
   // is in source file.
   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;

   // The database for CPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int                    m_numBreakpointEvents;

   // The memory for the Code/Data Logger display.  It is allocated
   // by the debugger inspector and passed to the CPU core for use
   // during emulation.
   static char*          m_pCodeDataLoggerInspectorTV;

   // The memory for the Execution Visualizer display.  It is allocated
   // by the debugger inspector and passed to the CPU core for use
   // during emulation.
   static char*          m_pExecutionVisualizerInspectorTV;
};

// Structure representing each instruction and
// addressing mode possible within the CPU core.
typedef struct _C6502_opcode
{
   // Self-referential index.
   int op;

   // Instruction printable name.
   const char* name;

   // Instruction execution function.
   void (*pFn)(void);

   // Addressing mode of this particular entry.
   int amode;

   // Number of CPU cycles required to execute this particular entry.
   int cycles;

   // Is the instruction part of the documented 6502 ISA?
   bool documented;

   // Do we force an extra cycle for this instruction variant?
   bool forceExtraCycle;
} C6502_opcode;

#endif
