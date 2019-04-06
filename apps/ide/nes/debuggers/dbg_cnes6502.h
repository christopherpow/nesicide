#if !defined ( NESCPU_H )
#define NESCPU_H

#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cbreakpointinfo.h"

#include "nes_emulator_core.h"

// Routines to retrieve the ToolTip information for a particular opcode.
const char* OPCODEINFO ( uint8_t op );
const char* OPCODEINFO ( const char* op );
const char* OPCODECHECK ( uint8_t idx, const char* op );

// Routine to retrieve the size of the specified opcode.
uint8_t OPCODESIZE ( uint8_t op );

void RENDERCPUCODEDATALOGGER ();
void CLEARCPUCODEDATALOGGER ();
int8_t* CPUCODEDATALOGGERTV ();

void RENDERCPUEXECUTIONVISUALIZER ();
void CLEARCPUEXECUTIONVISUALIZER ();
int8_t* CPUEXECUTIONVISUALIZERTV ();

// Structure representing each instruction and
// addressing mode possible within the CPU core.
typedef struct _C6502_opcode
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
} C6502_opcode;

#endif
