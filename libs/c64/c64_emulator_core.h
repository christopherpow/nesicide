#ifndef C64_EMULATOR_CORE_H
#define C64_EMULATOR_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // for standard base types...
#include <string.h> // for memcpy...
#include <stdio.h> // for sprintf...

#include "cmemorydata.h"
#include "cregisterdata.h"
#include "cbreakpointinfo.h"

// Common enumerations for emulated items.
typedef enum
{
   eC64Source_CPU
} eC64SourceType;

typedef enum
{
   eC64Memory_CPU = 0,
   eC64Memory_CPUregs
} eC64MemoryType;

#define MAKE16(lo,hi) ((((lo)&0xFF)|(((hi)&0xFF)<<8)))

// CPU interrupt vector memory addresses.
#define VECTOR_NMI   0xFFFA
#define VECTOR_RESET 0xFFFC
#define VECTOR_IRQ   0xFFFE

// CPU register 'numbers' (correspond only to UI elements as there's no
// physical memory address associated with them).
#define CPU_PC    0
#define CPU_A     1
#define CPU_X     2
#define CPU_Y     3
#define CPU_SP    4
#define CPU_F     5
#define CPU_IRQ   6
#define CPU_NMI   7
#define CPU_RESET 8

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

// The Execution Visualizer debugger inspector converts PPU cycles to
// (x,y) coordinates for visualization of the execution of code in the
// PPU frame.
#define CYCLE_TO_VISX(c) (c%PPU_CYCLES_PER_SCANLINE)
#define CYCLE_TO_VISY(c) (c/PPU_CYCLES_PER_SCANLINE)
#define VISY_VISX_TO_CYCLE(y,x) ((y*PPU_CYCLES_PER_SCANLINE)+x)

#define MEM_0B 0x0
#define MEM_8B 0x8
#define MASK_8B 0x7
#define MEM_32B 0x20
#define MASK_32B 0x1F
#define MEM_256B 0x100
#define MASK_256B 0xFF
#define MEM_512B 0x200
#define MASK_512B 0x1FF
#define MEM_1KB 0x400
#define MASK_1KB 0x3FF
#define UPSHIFT_1KB 10
#define MEM_2KB 0x800
#define MASK_2KB 0x7FF
#define UPSHIFT_2KB 11
#define MEM_4KB 0x1000
#define MASK_4KB 0xFFF
#define UPSHIFT_4KB 12
#define MEM_8KB 0x2000
#define SHIFT_8KB_1KB 10
#define MASK_8KB 0x1FFF
#define UPSHIFT_8KB 13
#define MEM_16KB 0x4000
#define MASK_16KB 0x3FFF
#define UPSHIFT_16KB 14
#define MEM_32KB 0x8000
#define MASK_32KB 0x7FFF
#define SHIFT_32KB_8KB 13
#define SHIFT_32KB_4KB 14
#define UPSHIFT_32KB 15
#define MEM_64KB 0x10000
#define MASK_64KB 0xFFFF
#define SHIFT_64KB_8KB 13

#define SID_BASE 0xd400
#define SID_END  0xd420

// sprintf() replacement macros for speed.
extern const char* hex_char;
#define sprintf_opcode(b,v) \
{ \
   (*(b++)) = (*(v++)); \
   (*(b++)) = (*(v++)); \
   (*(b++)) = (*(v++)); \
   (*(b)) = 0; \
   v-=3; \
}
#define sprintf_db(b) \
{ \
   (*(b++)) = '.'; \
   (*(b++)) = 'D'; \
   (*(b++)) = 'B'; \
   (*(b++)) = ' '; \
   (*(b++)) = '$'; \
   (*(b)) = 0; \
}
#define sprintf_02x(b,v) \
{ \
   (*(b++)) = *(hex_char+((v)>>4)); \
   (*(b++)) = *(hex_char+((v)&0xF)); \
   (*(b)) = 0; \
}

// Version control interfaces.
char* c64GetVersion();

// Internal hook interfaces.
void c64LockCoreMutex ( void );
void c64UnlockCoreMutex ( void );

// Exported interfaces.
// The following interfaces are to be used by a UI to interact with the emulation
// core and perform the necessary steps to interact with the VICE C=64 remote
// monitor.

// Internal debug interfaces.
extern bool __c64debug;
#define c64IsDebuggable() ( __c64debug )
void c64Break ( void );

CBreakpointInfo* c64GetBreakpointDatabase ( void );

CRegisterDatabase* c64GetCpuRegisterDatabase ( void );

CMemoryDatabase* c64GetCpuMemoryDatabase ( void );

// General debug interfaces.
void c64EnableDebug ( void );
void c64DisableDebug ( void );
uint32_t c64GetNumColors ( void );
uint32_t c64GetPaletteRedComponent(uint32_t idx);
uint32_t c64GetPaletteGreenComponent(uint32_t idx);
uint32_t c64GetPaletteBlueComponent(uint32_t idx);
void c64Disassemble ();
void c64DisassembleSingle ( uint8_t* pOpcode, char* buffer );
char* c64GetDisassemblyAtAddress ( uint32_t addr );
void c64GetDisassemblyAtAbsoluteAddress ( uint32_t absAddr, char* buffer );
uint32_t c64GetAddressFromSLOC ( uint16_t sloc );
uint16_t c64GetSLOCFromAddress ( uint32_t addr );
uint32_t c64GetSLOC ( uint32_t addr );
uint32_t c64GetAbsoluteAddressFromAddress ( uint32_t addr );
void c64ClearOpcodeMasks ( void );
void c64SetOpcodeMask ( uint32_t addr, uint8_t mask );
void c64SetBreakpointHook ( void (*hook)(void) );
void c64EnableBreakpoints ( bool enable );
void c64StepCpu ( void );
void c64GetPrintableAddress ( char* buffer, uint32_t addr );
void c64GetPrintableAddressWithAbsolute ( char* buffer, uint32_t addr, uint32_t absAddr );

// 6502 debug interfaces.
void c64SetGotoAddress ( uint32_t addr );
uint32_t c64GetGotoAddress ( void );
uint32_t c64GetMemory ( uint32_t addr );
void c64SetMemory ( uint32_t addr, uint32_t data );
uint32_t c64GetCPURegister ( uint32_t addr );
void c64SetCPURegister ( uint32_t addr, uint32_t data );
uint32_t c64GetCPUFlagNegative ( void );
uint32_t c64GetCPUFlagOverflow ( void );
uint32_t c64GetCPUFlagBreak ( void );
uint32_t c64GetCPUFlagDecimal ( void );
uint32_t c64GetCPUFlagInterrupt ( void );
uint32_t c64GetCPUFlagZero ( void );
uint32_t c64GetCPUFlagCarry ( void );
void c64SetCPUFlagNegative ( uint32_t set );
void c64SetCPUFlagOverflow ( uint32_t set );
void c64SetCPUFlagBreak ( uint32_t set );
void c64SetCPUFlagDecimal ( uint32_t set );
void c64SetCPUFlagInterrupt ( uint32_t set );
void c64SetCPUFlagZero ( uint32_t set );
void c64SetCPUFlagCarry ( uint32_t set );

// SID debug interfaces.
CRegisterDatabase* c64GetSidRegisterDatabase ( void );
uint32_t c64GetSID8Register ( uint32_t addr );
uint32_t c64GetSID16Register ( uint32_t addr );
void c64SetSID8Register ( uint32_t addr, uint32_t data );
void c64SetSID16Register ( uint32_t addr, uint32_t data );

typedef struct
{
   uint8_t memory[MEM_64KB];
   uint16_t pc;
   uint8_t sp;
   uint8_t a;
   uint8_t x;
   uint8_t y;
   uint8_t f;
} C64CpuStateSnapshot;

void c64GetCpuSnapshot(C64CpuStateSnapshot* pSnapshot);

#ifdef __cplusplus
}
#endif

#endif // EMULATOR_CORE_H
