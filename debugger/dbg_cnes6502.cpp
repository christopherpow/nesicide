//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "dbg_cnes6502.h"
#include "dbg_cnesrom.h"
#include "dbg_cnesppu.h"

#include "emulator_core.h"

#include <QColor>

// CPU Registers
static CBitfieldData* tblCPUPCBitfields [] =
{
   new CBitfieldData("Instruction Pointer", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPUABitfields [] =
{
   new CBitfieldData("Accumulator", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUXBitfields [] =
{
   new CBitfieldData("X Index", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUYBitfields [] =
{
   new CBitfieldData("Y Index", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUSPBitfields [] =
{
   new CBitfieldData("Stack Pointer", 0, 12, "%03X", 0),
};

static CBitfieldData* tblCPUFBitfields [] =
{
   new CBitfieldData("Negative", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Overflow", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Break", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Decimal Mode", 3, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Interrupt", 2, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Zero", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Carry", 0, 1, "%X", 2, "No", "Yes")
};

static CRegisterData* tblCPURegisters [] =
{
   new CRegisterData(0, "CPUPC", 1, tblCPUPCBitfields),
   new CRegisterData(1, "CPUA", 1, tblCPUABitfields),
   new CRegisterData(2, "CPUX", 1, tblCPUXBitfields),
   new CRegisterData(3, "CPUY", 1, tblCPUYBitfields),
   new CRegisterData(4, "CPUSP", 1, tblCPUSPBitfields),
   new CRegisterData(5, "CPUF", 7, tblCPUFBitfields)
};

// CPU Event breakpoints
bool cpuUndocumentedEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool cpuUndocumentedExactEvent(BreakpointInfo* pBreakpoint,int data)
{
   // If opcode executing is one specified, break...
   if ( pBreakpoint->item1 == data )
   {
      return true;
   }
   return false;
}

bool cpuExecuteExactEvent(BreakpointInfo* pBreakpoint,int data)
{
   // If opcode executing is one specified, break...
   if ( pBreakpoint->item1 == data )
   {
      return true;
   }
   return false;
}

bool cpuResetEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool cpuIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool cpuNMIEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* tblCPUEvents [] =
{
   new CBreakpointEventInfo("Specific Instruction Execution", cpuExecuteExactEvent, 1, "Break if opcode $%02X is executed", 16, "Opcode:"),
   new CBreakpointEventInfo("Any Undocumented Instruction Execution", cpuUndocumentedEvent, 0, "Break if any undocumented opcode is executed", 16),
   new CBreakpointEventInfo("Specific Undocumented Instruction Execution", cpuUndocumentedExactEvent, 1, "Break if undocumented opcode $%02X is executed", 16, "Opcode:"),
   new CBreakpointEventInfo("Reset", cpuResetEvent, 0, "Break if CPU is reset", 10),
   new CBreakpointEventInfo("IRQ", cpuIRQEvent, 0, "Break if CPU IRQ fires", 10),
   new CBreakpointEventInfo("NMI", cpuNMIEvent, 0, "Break if CPU NMI fires", 10)
};

uint8_t   C6502DBG::m_RAMopcodeMask [ MEM_2KB ] = { 0, };
char*     C6502DBG::m_RAMdisassembly [ MEM_2KB ] = { 0, };
uint16_t  C6502DBG::m_RAMsloc2addr [ MEM_2KB ] = { 0, };
uint16_t  C6502DBG::m_RAMaddr2sloc [ MEM_2KB ] = { 0, };
uint32_t  C6502DBG::m_RAMsloc = 0;

CMarker         C6502DBG::m_marker;

CCodeDataLogger C6502DBG::m_logger ( MEM_32KB, MASK_32KB );
TracerInfo*     C6502DBG::pDisassemblySample = NULL;

CRegisterData** C6502DBG::m_tblRegisters = tblCPURegisters;
int             C6502DBG::m_numRegisters = NUM_CPU_REGISTERS;

CBreakpointEventInfo** C6502DBG::m_tblBreakpointEvents = tblCPUEvents;
int                    C6502DBG::m_numBreakpointEvents = NUM_CPU_EVENTS;

int8_t*          C6502DBG::m_pCodeDataLoggerInspectorTV = NULL;

int8_t*          C6502DBG::m_pExecutionVisualizerInspectorTV = NULL;

static int32_t opcode_size [ NUM_ADDRESSING_MODES ] =
{
   1, // AM_IMPLIED
   2, // AM_IMMEDIATE
   3, // AM_ABSOLUTE
   2, // AM_ZEROPAGE
   1, // AM_ACCUMULATOR
   3, // AM_ABSOLUTE_INDEXED_X
   3, // AM_ABSOLUTE_INDEXED_Y
   2, // AM_ZEROPAGE_INDEXED_X
   2, // AM_ZEROPAGE_INDEXED_Y
   3, // AM_INDIRECT
   2, // AM_PREINDEXED_INDIRECT
   2, // AM_POSTINDEXED_INDIRECT
   2  // AM_RELATIVE
};

static const char* operandFmt [ NUM_ADDRESSING_MODES ] =
{
   "", // AM_IMPLIED
   " #$%02X", // AM_IMMEDIATE
   " $%02X%02X", // AM_ABSOLUTE
   " $%02X", // AM_ZEROPAGE
   "", // AM_ACCUMULATOR
   " $%02X%02X, X", // AM_ABSOLUTE_INDEXED_X
   " $%02X%02X, Y", // AM_ABSOLUTE_INDEXED_Y
   " $%02X, X", // AM_ZEROPAGE_INDEXED_X
   " $%02X, Y", // AM_ZEROPAGE_INDEXED_Y
   " ($%02X%02X)", // AM_INDIRECT
   " ($%02X, X)", // AM_PREINDEXED_INDIRECT
   " ($%02X), Y", // AM_POSTINDEXED_INDIRECT
   " $%02X"  // AM_RELATIVE
};

static C6502_opcode m_6502opcode [ 256 ] =
{
   { 0x00, "BRK", AM_IMPLIED, 7, true, true }, // BRK
   { 0x01, "ORA", AM_PREINDEXED_INDIRECT, 6, true, true }, // ORA - (Indirect,X)
   { 0x02, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x03, "ASO", AM_PREINDEXED_INDIRECT, 8, false, true }, // ASO - (Indirect,X) (undocumented)
   { 0x04, "DOP", AM_ZEROPAGE, 3, false, true }, // DOP (undocumented)
   { 0x05, "ORA", AM_ZEROPAGE, 3, true, true }, // ORA - Zero Page
   { 0x06, "ASL", AM_ZEROPAGE, 5, true, true }, // ASL - Zero Page
   { 0x07, "ASO", AM_ZEROPAGE, 5, false, true }, // ASO - Zero Page (undocumented)
   { 0x08, "PHP", AM_IMPLIED, 3, true, true }, // PHP
   { 0x09, "ORA", AM_IMMEDIATE, 2, true, true }, // ORA - Immediate
   { 0x0A, "ASL", AM_ACCUMULATOR, 2, true, true }, // ASL - Accumulator
   { 0x0B, "ANC", AM_IMMEDIATE, 2, false, true }, // ANC - Immediate (undocumented)
   { 0x0C, "TOP", AM_ABSOLUTE, 4, false, true }, // TOP (undocumented)
   { 0x0D, "ORA", AM_ABSOLUTE, 4, true, true }, // ORA - Absolute
   { 0x0E, "ASL", AM_ABSOLUTE, 6, true, true }, // ASL - Absolute
   { 0x0F, "ASO", AM_ABSOLUTE, 6, false, true }, // ASO - Absolute (undocumented)
   { 0x10, "BPL", AM_RELATIVE, 2, true, true }, // BPL
   { 0x11, "ORA", AM_POSTINDEXED_INDIRECT, 5, true, false }, // ORA - (Indirect),Y
   { 0x12, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x13, "ASO", AM_POSTINDEXED_INDIRECT, 8, false, true }, // ASO - (Indirect),Y (undocumented)
   { 0x14, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0x15, "ORA", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // ORA - Zero Page,X
   { 0x16, "ASL", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // ASL - Zero Page,X
   { 0x17, "ASO", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // ASO - Zero Page,X (undocumented)
   { 0x18, "CLC", AM_IMPLIED, 2, true, true }, // CLC
   { 0x19, "ORA", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // ORA - Absolute,Y
   { 0x1A, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0x1B, "ASO", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // ASO - Absolute,Y (undocumented)
   { 0x1C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0x1D, "ORA", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // ORA - Absolute,X
   { 0x1E, "ASL", AM_ABSOLUTE_INDEXED_X, 7, true, true }, // ASL - Absolute,X
   { 0x1F, "ASO", AM_ABSOLUTE_INDEXED_X, 7, false, true }, // ASO - Absolute,X (undocumented)
   { 0x20, "JSR", AM_ABSOLUTE, 6, true, true }, // JSR
   { 0x21, "AND", AM_PREINDEXED_INDIRECT, 6, true, true }, // AND - (Indirect,X)
   { 0x22, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x23, "RLA", AM_PREINDEXED_INDIRECT, 8, false, true }, // RLA - (Indirect,X) (undocumented)
   { 0x24, "BIT", AM_ZEROPAGE, 3, true, true }, // BIT - Zero Page
   { 0x25, "AND", AM_ZEROPAGE, 3, true, true }, // AND - Zero Page
   { 0x26, "ROL", AM_ZEROPAGE, 5, true, true }, // ROL - Zero Page
   { 0x27, "RLA", AM_ZEROPAGE, 5, false, true }, // RLA - Zero Page (undocumented)
   { 0x28, "PLP", AM_IMPLIED, 4, true, true }, // PLP
   { 0x29, "AND", AM_IMMEDIATE, 2, true, true }, // AND - Immediate
   { 0x2A, "ROL", AM_ACCUMULATOR, 2, true, true }, // ROL - Accumulator
   { 0x2B, "ANC", AM_IMMEDIATE, 2, false, true }, // ANC - Immediate (undocumented)
   { 0x2C, "BIT", AM_ABSOLUTE, 4, true, true }, // BIT - Absolute
   { 0x2D, "AND", AM_ABSOLUTE, 4, true, true }, // AND - Absolute
   { 0x2E, "ROL", AM_ABSOLUTE, 6, true, true }, // ROL - Absolute
   { 0x2F, "RLA", AM_ABSOLUTE, 6, false, true }, // RLA - Absolute (undocumented)
   { 0x30, "BMI", AM_RELATIVE, 2, true, true }, // BMI
   { 0x31, "AND", AM_POSTINDEXED_INDIRECT, 5, true, false }, // AND - (Indirect),Y
   { 0x32, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x33, "RLA", AM_POSTINDEXED_INDIRECT, 8, false, true }, // RLA - (Indirect),Y (undocumented)
   { 0x34, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0x35, "AND", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // AND - Zero Page,X
   { 0x36, "ROL", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // ROL - Zero Page,X
   { 0x37, "RLA", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // RLA - Zero Page,X (undocumented)
   { 0x38, "SEC", AM_IMPLIED, 2, true, true }, // SEC
   { 0x39, "AND", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // AND - Absolute,Y
   { 0x3A, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0x3B, "RLA", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // RLA - Absolute,Y (undocumented)
   { 0x3C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0x3D, "AND", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // AND - Absolute,X
   { 0x3E, "ROL", AM_ABSOLUTE_INDEXED_X, 7, true, false }, // ROL - Absolute,X
   { 0x3F, "RLA", AM_ABSOLUTE_INDEXED_X, 7, false, true }, // RLA - Absolute,X (undocumented)
   { 0x40, "RTI", AM_IMPLIED, 6, true, true }, // RTI
   { 0x41, "EOR", AM_PREINDEXED_INDIRECT, 6, true, true }, // EOR - (Indirect,X)
   { 0x42, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x43, "LSE", AM_PREINDEXED_INDIRECT, 8, false, true }, // LSE - (Indirect,X) (undocumented)
   { 0x44, "DOP", AM_ZEROPAGE, 3, false, true }, // DOP (undocumented)
   { 0x45, "EOR", AM_ZEROPAGE, 3, true, true }, // EOR - Zero Page
   { 0x46, "LSR", AM_ZEROPAGE, 5, true, true }, // LSR - Zero Page
   { 0x47, "LSE", AM_ZEROPAGE, 5, false, true }, // LSE - Zero Page (undocumented)
   { 0x48, "PHA", AM_IMPLIED, 3, true, true }, // PHA
   { 0x49, "EOR", AM_IMMEDIATE, 2, true, true }, // EOR - Immediate
   { 0x4A, "LSR", AM_ACCUMULATOR, 2, true, true }, // LSR - Accumulator
   { 0x4B, "ALR", AM_IMMEDIATE, 2, false, true }, // ALR - Immediate (undocumented)
   { 0x4C, "JMP", AM_ABSOLUTE, 3, true, true }, // JMP - Absolute
   { 0x4D, "EOR", AM_ABSOLUTE, 4, true, true }, // EOR - Absolute
   { 0x4E, "LSR", AM_ABSOLUTE, 6, true, true }, // LSR - Absolute
   { 0x4F, "LSE", AM_ABSOLUTE, 6, false, true }, // LSE - Absolute (undocumented)
   { 0x50, "BVC", AM_RELATIVE, 2, true, true }, // BVC
   { 0x51, "EOR", AM_POSTINDEXED_INDIRECT, 5, true, false }, // EOR - (Indirect),Y
   { 0x52, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x53, "LSE", AM_POSTINDEXED_INDIRECT, 8, false, true }, // LSE - (Indirect),Y
   { 0x54, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0x55, "EOR", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // EOR - Zero Page,X
   { 0x56, "LSR", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // LSR - Zero Page,X
   { 0x57, "LSE", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // LSE - Zero Page,X (undocumented)
   { 0x58, "CLI", AM_IMPLIED, 2, true, true }, // CLI
   { 0x59, "EOR", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // EOR - Absolute,Y
   { 0x5A, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0x5B, "LSE", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // LSE - Absolute,Y (undocumented)
   { 0x5C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0x5D, "EOR", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // EOR - Absolute,X
   { 0x5E, "LSR", AM_ABSOLUTE_INDEXED_X, 7, true, true }, // LSR - Absolute,X
   { 0x5F, "LSE", AM_ABSOLUTE_INDEXED_X, 7, false, true }, // LSE - Absolute,X (undocumented)
   { 0x60, "RTS", AM_IMPLIED, 6, true, true }, // RTS
   { 0x61, "ADC", AM_PREINDEXED_INDIRECT, 6, true, true }, // ADC - (Indirect,X)
   { 0x62, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x63, "RRA", AM_PREINDEXED_INDIRECT, 8, false, true }, // RRA - (Indirect,X) (undocumented)
   { 0x64, "DOP", AM_ZEROPAGE, 3, false, true }, // DOP (undocumented)
   { 0x65, "ADC", AM_ZEROPAGE, 3, true, true }, // ADC - Zero Page
   { 0x66, "ROR", AM_ZEROPAGE, 5, true, true }, // ROR - Zero Page
   { 0x67, "RRA", AM_ZEROPAGE, 5, false, true }, // RRA - Zero Page (undocumented)
   { 0x68, "PLA", AM_IMPLIED, 4, true, true }, // PLA
   { 0x69, "ADC", AM_IMMEDIATE, 2, true, true }, // ADC - Immediate
   { 0x6A, "ROR", AM_ACCUMULATOR, 2, true, true }, // ROR - Accumulator
   { 0x6B, "ARR", AM_IMMEDIATE, 2, false, true }, // ARR - Immediate (undocumented)
   { 0x6C, "JMP", AM_INDIRECT, 5, true, true }, // JMP - Indirect
   { 0x6D, "ADC", AM_ABSOLUTE, 4, true, true }, // ADC - Absolute
   { 0x6E, "ROR", AM_ABSOLUTE, 6, true, true }, // ROR - Absolute
   { 0x6F, "RRA", AM_ABSOLUTE, 6, false, true }, // RRA - Absolute (undocumented)
   { 0x70, "BVS", AM_RELATIVE, 2, true, true }, // BVS
   { 0x71, "ADC", AM_POSTINDEXED_INDIRECT, 5, true, false }, // ADC - (Indirect),Y
   { 0x72, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x73, "RRA", AM_POSTINDEXED_INDIRECT, 8, false, true }, // RRA - (Indirect),Y (undocumented)
   { 0x74, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0x75, "ADC", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // ADC - Zero Page,X
   { 0x76, "ROR", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // ROR - Zero Page,X
   { 0x77, "RRA", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // RRA - Zero Page,X (undocumented)
   { 0x78, "SEI", AM_IMPLIED, 2, true, true }, // SEI
   { 0x79, "ADC", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // ADC - Absolute,Y
   { 0x7A, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0x7B, "RRA", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // RRA - Absolute,Y (undocumented)
   { 0x7C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0x7D, "ADC", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // ADC - Absolute,X
   { 0x7E, "ROR", AM_ABSOLUTE_INDEXED_X, 7, true, true }, // ROR - Absolute,X
   { 0x7F, "RRA", AM_ABSOLUTE_INDEXED_X, 7, false, true }, // RRA - Absolute,X (undocumented)
   { 0x80, "DOP", AM_IMMEDIATE, 2, false, false }, // DOP (undocumented)
   { 0x81, "STA", AM_PREINDEXED_INDIRECT, 6, true, true }, // STA - (Indirect,X)
   { 0x82, "DOP", AM_IMMEDIATE, 2, false, true }, // DOP (undocumented)
   { 0x83, "AXS", AM_PREINDEXED_INDIRECT, 6, false, true }, // AXS - (Indirect,X) (undocumented)
   { 0x84, "STY", AM_ZEROPAGE, 3, true, true }, // STY - Zero Page
   { 0x85, "STA", AM_ZEROPAGE, 3, true, true }, // STA - Zero Page
   { 0x86, "STX", AM_ZEROPAGE, 3, true, true }, // STX - Zero Page
   { 0x87, "AXS", AM_ZEROPAGE, 3, false, true }, // AXS - Zero Page (undocumented)
   { 0x88, "DEY", AM_IMPLIED, 2, true, true }, // DEY
   { 0x89, "DOP", AM_IMMEDIATE, 2, false, true }, // DOP (undocumented)
   { 0x8A, "TXA", AM_IMPLIED, 2, true, true }, // TXA
   { 0x8B, "XAA", AM_IMMEDIATE, 2, false, true }, // XAA - Immediate (undocumented)
   { 0x8C, "STY", AM_ABSOLUTE, 4, true, true }, // STY - Absolute
   { 0x8D, "STA", AM_ABSOLUTE, 4, true, true }, // STA - Absolute
   { 0x8E, "STX", AM_ABSOLUTE, 4, true, true }, // STX - Absolute
   { 0x8F, "AXS", AM_ABSOLUTE, 4, false, true }, // AXS - Absolulte (undocumented)
   { 0x90, "BCC", AM_RELATIVE, 2, true, true }, // BCC
   { 0x91, "STA", AM_POSTINDEXED_INDIRECT, 6, true, true }, // STA - (Indirect),Y
   { 0x92, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0x93, "AXA", AM_POSTINDEXED_INDIRECT, 6, false, false }, // AXA - (Indirect),Y
   { 0x94, "STY", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // STY - Zero Page,X
   { 0x95, "STA", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // STA - Zero Page,X
   { 0x96, "STX", AM_ZEROPAGE_INDEXED_Y, 4, true, true }, // STX - Zero Page,Y
   { 0x97, "AXS", AM_ZEROPAGE_INDEXED_Y, 4, false, true }, // AXS - Zero Page,Y
   { 0x98, "TYA", AM_IMPLIED, 2, true, true }, // TYA
   { 0x99, "STA", AM_ABSOLUTE_INDEXED_Y, 5, true, true }, // STA - Absolute,Y
   { 0x9A, "TXS", AM_IMPLIED, 2, true, true }, // TXS
   { 0x9B, "TAS", AM_ABSOLUTE_INDEXED_Y, 5, false, true }, // TAS - Absolute,Y (undocumented)
   { 0x9C, "SAY", AM_ABSOLUTE_INDEXED_X, 5, false, true }, // SAY - Absolute,X (undocumented)
   { 0x9D, "STA", AM_ABSOLUTE_INDEXED_X, 5, true, true }, // STA - Absolute,X
   { 0x9E, "XAS", AM_ABSOLUTE_INDEXED_Y, 5, false, true }, // XAS - Absolute,Y (undocumented)
   { 0x9F, "AXA", AM_ABSOLUTE_INDEXED_Y, 5, false, true }, // AXA - Absolute,Y (undocumented)
   { 0xA0, "LDY", AM_IMMEDIATE, 2, true, true }, // LDY - Immediate
   { 0xA1, "LDA", AM_PREINDEXED_INDIRECT, 6, true, true }, // LDA - (Indirect,X)
   { 0xA2, "LDX", AM_IMMEDIATE, 2, true, true }, // LDX - Immediate
   { 0xA3, "LAX", AM_PREINDEXED_INDIRECT, 6, false, true }, // LAX - (Indirect,X) (undocumented)
   { 0xA4, "LDY", AM_ZEROPAGE, 3, true, true }, // LDY - Zero Page
   { 0xA5, "LDA", AM_ZEROPAGE, 3, true, true }, // LDA - Zero Page
   { 0xA6, "LDX", AM_ZEROPAGE, 3, true, true }, // LDX - Zero Page
   { 0xA7, "LAX", AM_ZEROPAGE, 3, false, true }, // LAX - Zero Page (undocumented)
   { 0xA8, "TAY", AM_IMPLIED, 2, true, true }, // TAY
   { 0xA9, "LDA", AM_IMMEDIATE, 2, true, true }, // LDA - Immediate
   { 0xAA, "TAX", AM_IMPLIED, 2, true, true }, // TAX
   { 0xAB, "OAL", AM_IMMEDIATE, 2, false, true }, // OAL - Immediate
   { 0xAC, "LDY", AM_ABSOLUTE, 4, true, true }, // LDY - Absolute
   { 0xAD, "LDA", AM_ABSOLUTE, 4, true, true }, // LDA - Absolute
   { 0xAE, "LDX", AM_ABSOLUTE, 4, true, true }, // LDX - Absolute
   { 0xAF, "LAX", AM_ABSOLUTE, 4, false, true }, // LAX - Absolute (undocumented)
   { 0xB0, "BCS", AM_RELATIVE, 2, true, true }, // BCS
   { 0xB1, "LDA", AM_POSTINDEXED_INDIRECT, 5, true, false }, // LDA - (Indirect),Y
   { 0xB2, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0xB3, "LAX", AM_POSTINDEXED_INDIRECT, 5, false, false }, // LAX - (Indirect),Y (undocumented)
   { 0xB4, "LDY", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // LDY - Zero Page,X
   { 0xB5, "LDA", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // LDA - Zero Page,X
   { 0xB6, "LDX", AM_ZEROPAGE_INDEXED_Y, 4, true, true }, // LDX - Zero Page,Y
   { 0xB7, "LAX", AM_ZEROPAGE_INDEXED_Y, 4, false, true }, // LAX - Zero Page,X (undocumented)
   { 0xB8, "CLV", AM_IMPLIED, 2, true, true }, // CLV
   { 0xB9, "LDA", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // LDA - Absolute,Y
   { 0xBA, "TSX", AM_IMPLIED, 2, true, true }, // TSX
   { 0xBB, "LAS", AM_ABSOLUTE_INDEXED_Y, 4, false, false }, // LAS - Absolute,Y (undocumented)
   { 0xBC, "LDY", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // LDY - Absolute,X
   { 0xBD, "LDA", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // LDA - Absolute,X
   { 0xBE, "LDX", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // LDX - Absolute,Y
   { 0xBF, "LAX", AM_ABSOLUTE_INDEXED_Y, 4, false, false }, // LAX - Absolute,Y (undocumented)
   { 0xC0, "CPY", AM_IMMEDIATE, 2, true, true }, // CPY - Immediate
   { 0xC1, "CMP", AM_PREINDEXED_INDIRECT, 6, true, true }, // CMP - (Indirect,X)
   { 0xC2, "DOP", AM_IMMEDIATE, 2, false, true }, // DOP (undocumented)
   { 0xC3, "DCM", AM_PREINDEXED_INDIRECT, 8, false, true }, // DCM - (Indirect,X) (undocumented)
   { 0xC4, "CPY", AM_ZEROPAGE, 3, true, true }, // CPY - Zero Page
   { 0xC5, "CMP", AM_ZEROPAGE, 3, true, true }, // CMP - Zero Page
   { 0xC6, "DEC", AM_ZEROPAGE, 5, true, true }, // DEC - Zero Page
   { 0xC7, "DCM", AM_ZEROPAGE, 5, true, true }, // DCM - Zero Page (undocumented)
   { 0xC8, "INY", AM_IMPLIED, 2, true, true }, // INY
   { 0xC9, "CMP", AM_IMMEDIATE, 2, true, true }, // CMP - Immediate
   { 0xCA, "DEX", AM_IMPLIED, 2, true, true }, // DEX
   { 0xCB, "SAX", AM_IMMEDIATE, 2, false, true }, // SAX - Immediate (undocumented)
   { 0xCC, "CPY", AM_ABSOLUTE, 4, true, true }, // CPY - Absolute
   { 0xCD, "CMP", AM_ABSOLUTE, 4, true, true }, // CMP - Absolute
   { 0xCE, "DEC", AM_ABSOLUTE, 6, true, true }, // DEC - Absolute
   { 0xCF, "DCM", AM_ABSOLUTE, 6, false, true }, // DCM - Absolute (undocumented)
   { 0xD0, "BNE", AM_RELATIVE, 2, true, true }, // BNE
   { 0xD1, "CMP", AM_POSTINDEXED_INDIRECT, 5, true, false }, // CMP   (Indirect),Y
   { 0xD2, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0xD3, "DCM", AM_POSTINDEXED_INDIRECT, 8, false, true }, // DCM - (Indirect),Y (undocumented)
   { 0xD4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0xD5, "CMP", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // CMP - Zero Page,X
   { 0xD6, "DEC", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // DEC - Zero Page,X
   { 0xD7, "DCM", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // DCM - Zero Page,X (undocumented)
   { 0xD8, "CLD", AM_IMPLIED, 2, true, true }, // CLD
   { 0xD9, "CMP", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // CMP - Absolute,Y
   { 0xDA, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0xDB, "DCM", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // DCM - Absolute,Y (undocumented)
   { 0xDC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0xDD, "CMP", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // CMP - Absolute,X
   { 0xDE, "DEC", AM_ABSOLUTE_INDEXED_X, 7, true, true }, // DEC - Absolute,X
   { 0xDF, "DCM", AM_ABSOLUTE_INDEXED_X, 7, false, true }, // DCM - Absolute,X (undocumented)
   { 0xE0, "CPX", AM_IMMEDIATE, 2, true, true }, // CPX - Immediate
   { 0xE1, "SBC", AM_PREINDEXED_INDIRECT, 6, true, true }, // SBC - (Indirect,X)
   { 0xE2, "DOP", AM_IMMEDIATE, 2, false, true }, // DOP (undocumented)
   { 0xE3, "INS", AM_PREINDEXED_INDIRECT, 8, false, true }, // INS - (Indirect,X) (undocumented)
   { 0xE4, "CPX", AM_ZEROPAGE, 3, true, true }, // CPX - Zero Page
   { 0xE5, "SBC", AM_ZEROPAGE, 3, true, true }, // SBC - Zero Page
   { 0xE6, "INC", AM_ZEROPAGE, 5, true, true }, // INC - Zero Page
   { 0xE7, "INS", AM_ZEROPAGE, 5, false, true }, // INS - Zero Page (undocumented)
   { 0xE8, "INX", AM_IMPLIED, 2, true, true }, // INX
   { 0xE9, "SBC", AM_IMMEDIATE, 2, true, true }, // SBC - Immediate
   { 0xEA, "NOP", AM_IMPLIED, 2, true, true }, // NOP
   { 0xEB, "SBC", AM_IMMEDIATE, 2, false, true }, // SBC - Immediate (undocumented)
   { 0xEC, "CPX", AM_ABSOLUTE, 4, true, true }, // CPX - Absolute
   { 0xED, "SBC", AM_ABSOLUTE, 4, true, true }, // SBC - Absolute
   { 0xEE, "INC", AM_ABSOLUTE, 6, true, true }, // INC - Absolute
   { 0xEF, "INS", AM_ABSOLUTE, 6, false, true }, // INS - Absolute (undocumented)
   { 0xF0, "BEQ", AM_RELATIVE, 2, true, true }, // BEQ
   { 0xF1, "SBC", AM_POSTINDEXED_INDIRECT, 5, true, false }, // SBC - (Indirect),Y
   { 0xF2, "KIL", AM_IMPLIED, 0, false, true }, // KIL - Implied (processor lock up!)
   { 0xF3, "INS", AM_POSTINDEXED_INDIRECT, 8, false, true }, // INS - (Indirect),Y (undocumented)
   { 0xF4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, true }, // DOP (undocumented)
   { 0xF5, "SBC", AM_ZEROPAGE_INDEXED_X, 4, true, true }, // SBC - Zero Page,X
   { 0xF6, "INC", AM_ZEROPAGE_INDEXED_X, 6, true, true }, // INC - Zero Page,X
   { 0xF7, "INS", AM_ZEROPAGE_INDEXED_X, 6, false, true }, // INS - Zero Page,X (undocumented)
   { 0xF8, "SED", AM_IMPLIED, 2, true, true }, // SED
   { 0xF9, "SBC", AM_ABSOLUTE_INDEXED_Y, 4, true, false }, // SBC - Absolute,Y
   { 0xFA, "NOP", AM_IMPLIED, 2, false, true }, // NOP (undocumented)
   { 0xFB, "INS", AM_ABSOLUTE_INDEXED_Y, 7, false, true }, // INS - Absolute,Y (undocumented)
   { 0xFC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false }, // TOP (undocumented)
   { 0xFD, "SBC", AM_ABSOLUTE_INDEXED_X, 4, true, false }, // SBC - Absolute,X
   { 0xFE, "INC", AM_ABSOLUTE_INDEXED_X, 7, true, true }, // INC - Absolute,X
   { 0xFF, "INS", AM_ABSOLUTE_INDEXED_X, 7, false, true }  // INS - Absolute,X (undocumented)
};

#define FLAG_EFFECT(__n,__z,__c,__i,__d,__v) \
   "Flag effect:" \
   "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">" \
   "<tr bgcolor=\"#B0B0B0\"><th>N</th><th>Z</th><th>C</th><th>I</th><th>D</th><th>V</th></tr>" \
   "<tr><td>" __n \
   "</td><td>" __z \
   "</td><td>" __c \
   "</td><td>" __i \
   "</td><td>" __d \
   "</td><td>" __v \
   "</td></tr>" \
   "</table><br>"

#define VARIANT_HEADER \
   "Variants:" \
   "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">" \
   "<tr bgcolor=\"#B0B0B0\"><th></th><th>Addressing Mode</th><th>Assembly Language Form</th><th>Opcode</th><th># Bytes</th><th># Cycles</th></tr>"

#define VARIANT_SELECTED \
   "<tr bgcolor=\"#E8E8E8\"><td>&gt;</td>"

#define VARIANT \
   "<tr><td></td>"

#define VARIANT_INFO(__amode,__mnemonic,__langform,__opcode,__bytes,__cycles) \
   "<td>" __amode \
   "</td><td>" __mnemonic " " __langform \
   "</td><td>" __opcode \
   "</td><td>" __bytes \
   "</td><td>" __cycles \
   "</td></tr>"

#define VARIANT_IMPLIED(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Implied",__mnemonic,"",__opcode,__bytes,__cycles)

#define VARIANT_IMMEDIATE(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Immediate",__mnemonic,"#Operand",__opcode,__bytes,__cycles)

#define VARIANT_ACCUMULATOR(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Accumulator",__mnemonic,"A",__opcode,__bytes,__cycles)

#define VARIANT_ZEROPAGE(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Zero Page",__mnemonic,"Operand",__opcode,__bytes,__cycles)

#define VARIANT_ZEROPAGE_X(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Zero Page, indexed by X",__mnemonic,"Operand,X",__opcode,__bytes,__cycles)

#define VARIANT_ZEROPAGE_Y(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Zero Page, indexed by Y",__mnemonic,"Operand,Y",__opcode,__bytes,__cycles)

#define VARIANT_ABSOLUTE(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Absolute",__mnemonic,"Operand",__opcode,__bytes,__cycles)

#define VARIANT_ABSOLUTE_X(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Absolute, indexed by X",__mnemonic,"Operand,X",__opcode,__bytes,__cycles)

#define VARIANT_ABSOLUTE_Y(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Absolute, indexed by Y",__mnemonic,"Operand,Y",__opcode,__bytes,__cycles)

#define VARIANT_INDIRECT(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Indirect",__mnemonic,"(Operand)",__opcode,__bytes,__cycles)

#define VARIANT_INDIRECT_X(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Pre-indexed Indirect",__mnemonic,"(Operand,X)",__opcode,__bytes,__cycles)

#define VARIANT_INDIRECT_Y(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("Post-indexed Indirect",__mnemonic,"(Operand),Y",__opcode,__bytes,__cycles)

#define VARIANT_RELATIVE(__mnemonic,__opcode,__bytes,__cycles) \
   VARIANT_INFO("PC relative",__mnemonic,"Operand",__opcode,__bytes,__cycles)

#define VARIANT_FOOTER \
   "</table>"

static const char* m_6502opcodeInfo [ 256 ] =
{
   // BRK
   "BRK: Force Break<br>"
   "Operation:  Forced Interrupt, PC + 2 to S, P to S<br>"
   FLAG_EFFECT("","","","1","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("BRK","00","1","7")
   VARIANT_FOOTER
   "1. A BRK command cannot be masked by setting I.",

   // ORA - (Indirect,X)
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   "", // KIL - Implied (processor lock up!)

   "", // ASO - (Indirect,X) (undocumented)

   "", // DOP (undocumented)

   // ORA - Zero Page
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   // ASL - Zero Page
   "ASL: Shift Left One Bit (Memory or Accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  C &lt;- |7|6|5|4|3|2|1|0| &lt;- 0<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ASL","0A","1","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("ASL","06","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ASL","16","2","6")
   VARIANT VARIANT_ABSOLUTE("ASL","0E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ASL","1E","3","7")
   VARIANT_FOOTER,

   "", // ASO - Zero Page (undocumented)

   // PHP
   "PHP: Push processor status on stack<br>"
   "Operation:  P to S<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("PHP","08","1","3")
   VARIANT_FOOTER,

   // ORA - Immediate
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   // ASL - Accumulator
   "ASL: Shift Left One Bit (Memory or Accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  C &lt;- |7|6|5|4|3|2|1|0| &lt;- 0<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ACCUMULATOR("ASL","0A","1","2")
   VARIANT VARIANT_ZEROPAGE("ASL","06","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ASL","16","2","6")
   VARIANT VARIANT_ABSOLUTE("ASL","0E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ASL","1E","3","7")
   VARIANT_FOOTER,

   "", // ANC - Immediate (undocumented)

   "", // TOP (undocumented)

   // ORA - Absolute
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   // ASL - Absolute
   "ASL: Shift Left One Bit (Memory or Accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  C &lt;- |7|6|5|4|3|2|1|0| &lt;- 0<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ASL","0A","1","2")
   VARIANT VARIANT_ZEROPAGE("ASL","06","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ASL","16","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("ASL","0E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ASL","1E","3","7")
   VARIANT_FOOTER,

   "", // ASO - Absolute (undocumented)

   // BPL
   "BPL: Branch on result plus<br>"
   "Operation:  Branch on N = 0<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BPL","10","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // ORA - (Indirect),Y
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   "", // KIL - Implied (processor lock up!)

   "", // ASO - (Indirect),Y (undocumented)

   "", // DOP (undocumented)

   // ORA - Zero Page,X
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   // ASL - Zero Page,X
   "ASL: Shift Left One Bit (Memory or Accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  C &lt;- |7|6|5|4|3|2|1|0| &lt;- 0<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ASL","0A","1","2")
   VARIANT VARIANT_ZEROPAGE("ASL","06","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("ASL","16","2","6")
   VARIANT VARIANT_ABSOLUTE("ASL","0E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ASL","1E","3","7")
   VARIANT_FOOTER,

   "", // ASO - Zero Page,X (undocumented)

   // CLC
   "CLC: Clear carry flag<br>"
   "Operation:  0 -&gt; C<br>"
   FLAG_EFFECT("","","0","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("CLC","18","1","2")
   VARIANT_FOOTER,

   // ORA - Absolute,Y
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   "", // NOP (undocumented)

   "", // ASO - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // ORA - Absolute,X
   "ORA: \"OR\" memory with accumulator<br>"
   "Operation: A \\/ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ORA","09","2","2")
   VARIANT VARIANT_ZEROPAGE("ORA","05","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ORA","15","2","4")
   VARIANT VARIANT_ABSOLUTE("ORA","0D","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("ORA","1D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ORA","19","3","4*")
   VARIANT VARIANT_INDIRECT_X("ORA","01","2","6")
   VARIANT VARIANT_INDIRECT_Y("ORA","11","2","5*")
   VARIANT_FOOTER
   "* Add 1 on page crossing",

   // ASL - Absolute,X
   "ASL: Shift Left One Bit (Memory or Accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  C &lt;- |7|6|5|4|3|2|1|0| &lt;- 0<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ASL","0A","1","2")
   VARIANT VARIANT_ZEROPAGE("ASL","06","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ASL","16","2","6")
   VARIANT VARIANT_ABSOLUTE("ASL","0E","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("ASL","1E","3","7")
   VARIANT_FOOTER,

   "", // ASO - Absolute,X (undocumented)

   // JSR
   "JSR: Jump to new location saving return address<br>"
   "Operation:  PC + 2 toS, (PC + 1) -&gt; PCL<br>"
   "                        (PC + 2) -&gt; PCH<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ABSOLUTE("JSR","20","3","6")
   VARIANT_FOOTER,

   // AND - (Indirect,X)
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // RLA - (Indirect,X) (undocumented)

   // BIT - Zero Page
   "BIT: Test bits in memory with accumulator<br>"
   "Operation:  A /\\ M, M7 -&gt; N, M6 -&gt; V<br>"
   "Bit 6 and 7 are transferred to the status register.<br>"
   "If the result of A /\\ M is zero then Z = 1, otherwise<br>"
   "Z = 0<br>"
   FLAG_EFFECT("M7","/","","","","M6")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("BIT","24","2","3")
   VARIANT VARIANT_ABSOLUTE("BIT","24","3","4")
   VARIANT_FOOTER,

   // AND - Zero Page
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROL - Zero Page
   "ROL: Rotate one bit left (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |         M or A               |<br>"
   "             |   +-+-+-+-+-+-+-+-+    +-+   |<br>"
   "Operation:   +-&lt; |7|6|5|4|3|2|1|0| &lt;- |C| &lt;-+<br>"
   "                 +-+-+-+-+-+-+-+-+    +-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROL","2A","1","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("ROL","26","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROL","36","2","6")
   VARIANT VARIANT_ABSOLUTE("ROL","2E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROL","3E","3","7")
   VARIANT_FOOTER,

   "", // RLA - Zero Page (undocumented)

   // PLP
   "PLP: Pull processor status from stack<br>"
   "Operation:  P from S<br>"
   FLAG_EFFECT("from S","from S","from S","from S","from S","from S")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("PLP","28","1","4")
   VARIANT_FOOTER,

   // AND - Immediate
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROL - Accumulator
   "ROL: Rotate one bit left (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |         M or A               |<br>"
   "             |   +-+-+-+-+-+-+-+-+    +-+   |<br>"
   "Operation:   +-&lt; |7|6|5|4|3|2|1|0| &lt;- |C| &lt;-+<br>"
   "                 +-+-+-+-+-+-+-+-+    +-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ACCUMULATOR("ROL","2A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROL","26","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROL","36","2","6")
   VARIANT VARIANT_ABSOLUTE("ROL","2E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROL","3E","3","7")
   VARIANT_FOOTER,

   "", // ANC - Immediate (undocumented)

   // BIT - Absolute
   "BIT: Test bits in memory with accumulator<br>"
   "Operation:  A /\\ M, M7 -&gt; N, M6 -&gt; V<br>"
   "Bit 6 and 7 are transferred to the status register.<br>"
   "If the result of A /\\ M is zero then Z = 1, otherwise<br>"
   "Z = 0<br>"
   FLAG_EFFECT("M7","/","","","","M6")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("BIT","24","2","3")
   VARIANT_SELECTED VARIANT_ABSOLUTE("BIT","24","3","4")
   VARIANT_FOOTER,

   // AND - Absolute
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROL - Absolute
   "ROL: Rotate one bit left (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |         M or A               |<br>"
   "             |   +-+-+-+-+-+-+-+-+    +-+   |<br>"
   "Operation:   +-&lt; |7|6|5|4|3|2|1|0| &lt;- |C| &lt;-+<br>"
   "                 +-+-+-+-+-+-+-+-+    +-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROL","2A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROL","26","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROL","36","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("ROL","2E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROL","3E","3","7")
   VARIANT_FOOTER,

   "", // RLA - Absolute (undocumented)

   // BMI
   "BMI: Branch on result minus<br>"
   "Operation:  Branch on N = 1<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BMI","30","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // AND - (Indirect),Y
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // RLA - (Indirect),Y (undocumented)

   "", // DOP (undocumented)

   // AND - Zero Page,X
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROL - Zero Page,X
   "ROL: Rotate one bit left (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |         M or A               |<br>"
   "             |   +-+-+-+-+-+-+-+-+    +-+   |<br>"
   "Operation:   +-&lt; |7|6|5|4|3|2|1|0| &lt;- |C| &lt;-+<br>"
   "                 +-+-+-+-+-+-+-+-+    +-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROL","2A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROL","26","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("ROL","36","2","6")
   VARIANT VARIANT_ABSOLUTE("ROL","2E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROL","3E","3","7")
   VARIANT_FOOTER,

   "", // RLA - Zero Page,X (undocumented)

   // SEC
   "SEC: Set carry flag<br>"
   "Operation:  1 -&gt; C<br>"
   FLAG_EFFECT("","","1","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("SEC","38","1","2")
   VARIANT_FOOTER,

   // AND - Absolute,Y
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP (undocumented)

   "", // RLA - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // AND - Absolute,X
   "AND: \"AND\" memory with accumulator<br>"
   "Operation:  A /\\ M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("AND","29","2","2")
   VARIANT VARIANT_ZEROPAGE("AND","25","2","3")
   VARIANT VARIANT_ZEROPAGE_X("AND","35","2","4")
   VARIANT VARIANT_ABSOLUTE("AND","2D","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("AND","3D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("AND","39","3","4*")
   VARIANT VARIANT_INDIRECT_X("AND","21","2","6")
   VARIANT VARIANT_INDIRECT_Y("AND","31","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROL - Absolute,X
   "ROL: Rotate one bit left (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |         M or A               |<br>"
   "             |   +-+-+-+-+-+-+-+-+    +-+   |<br>"
   "Operation:   +-&lt; |7|6|5|4|3|2|1|0| &lt;- |C| &lt;-+<br>"
   "                 +-+-+-+-+-+-+-+-+    +-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROL","2A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROL","26","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROL","36","2","6")
   VARIANT VARIANT_ABSOLUTE("ROL","2E","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("ROL","3E","3","7")
   VARIANT_FOOTER,

   "", // RLA - Absolute,X (undocumented)

   // RTI
   "RTI: Return from interrupt<br>"
   "Operation:  P from S, PC from S<br>"
   FLAG_EFFECT("from S","from S","from S","from S","from S","from S")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("RTI","4D","1","6")
   VARIANT_FOOTER,

   // EOR - (Indirect,X)
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // LSE - (Indirect,X) (undocumented)

   "", // DOP (undocumented)

   // EOR - Zero Page
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LSR - Zero Page
   "LSR: Shift right one bit (memory or accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  0 -&gt; |7|6|5|4|3|2|1|0| -&gt; C<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("0","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("LSR","4A","1","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("LSR","46","2","5")
   VARIANT VARIANT_ZEROPAGE_X("LSR","56","2","6")
   VARIANT VARIANT_ABSOLUTE("LSR","4E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("LSR","5E","3","7")
   VARIANT_FOOTER,

   "", // LSE - Zero Page (undocumented)

   // PHA
   "PHA: Push accumulator on stack<br>"
   "Operation:  A to S<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("PHA","48","1","3")
   VARIANT_FOOTER,

   // EOR - Immediate
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LSR - Accumulator
   "LSR: Shift right one bit (memory or accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  0 -&gt; |7|6|5|4|3|2|1|0| -&gt; C<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("0","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ACCUMULATOR("LSR","4A","1","2")
   VARIANT VARIANT_ZEROPAGE("LSR","46","2","5")
   VARIANT VARIANT_ZEROPAGE_X("LSR","56","2","6")
   VARIANT VARIANT_ABSOLUTE("LSR","4E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("LSR","5E","3","7")
   VARIANT_FOOTER,

   "", // ALR - Immediate (undocumented)

   // JMP - Absolute
   "JMP: Jump to new location<br>"
   "Operation:  (PC + 1) -&gt; PCL<br>"
   "            (PC + 2) -&gt; PCH<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ABSOLUTE("JMP","4C","3","3")
   VARIANT VARIANT_INDIRECT("JMP","6C","3","5")
   VARIANT_FOOTER,

   // EOR - Absolute
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LSR - Absolute
   "LSR: Shift right one bit (memory or accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  0 -&gt; |7|6|5|4|3|2|1|0| -&gt; C<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("0","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("LSR","4A","1","2")
   VARIANT VARIANT_ZEROPAGE("LSR","46","2","5")
   VARIANT VARIANT_ZEROPAGE_X("LSR","56","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("LSR","4E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("LSR","5E","3","7")
   VARIANT_FOOTER,

   "", // LSE - Absolute (undocumented)

   // BVC
   "BVC: Branch on overflow clear<br>"
   "Operation:  Branch on V = 0<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BVC","50","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // EOR - (Indirect),Y
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // LSE - (Indirect),Y

   "", // DOP (undocumented)

   // EOR - Zero Page,X
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LSR - Zero Page,X
   "LSR: Shift right one bit (memory or accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  0 -&gt; |7|6|5|4|3|2|1|0| -&gt; C<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("0","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("LSR","4A","1","2")
   VARIANT VARIANT_ZEROPAGE("LSR","46","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("LSR","56","2","6")
   VARIANT VARIANT_ABSOLUTE("LSR","4E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("LSR","5E","3","7")
   VARIANT_FOOTER,

   "", // LSE - Zero Page,X (undocumented)

   // CLI
   "CLI: Clear interrupt disable bit<br>"
   "Operation:  0 -&gt; I<br>"
   FLAG_EFFECT("","","","0","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("CLI","58","1","2")
   VARIANT_FOOTER,

   // EOR - Absolute,Y
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP (undocumented)

   "", // LSE - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // EOR - Absolute,X
   "EOR: \"Exclusive-Or\" memory with accumulator<br>"
   "Operation:  A EOR M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("EOR","49","2","2")
   VARIANT VARIANT_ZEROPAGE("EOR","45","2","3")
   VARIANT VARIANT_ZEROPAGE_X("EOR","55","2","4")
   VARIANT VARIANT_ABSOLUTE("EOR","4D","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("EOR","5D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("EOR","59","3","4*")
   VARIANT VARIANT_INDIRECT_X("EOR","41","2","6")
   VARIANT VARIANT_INDIRECT_Y("EOR","51","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LSR - Absolute,X
   "LSR: Shift right one bit (memory or accumulator)<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   "Operation:  0 -&gt; |7|6|5|4|3|2|1|0| -&gt; C<br>"
   "                 +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("0","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("LSR","4A","1","2")
   VARIANT VARIANT_ZEROPAGE("LSR","46","2","5")
   VARIANT VARIANT_ZEROPAGE_X("LSR","56","2","6")
   VARIANT VARIANT_ABSOLUTE("LSR","4E","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("LSR","5E","3","7")
   VARIANT_FOOTER,

   "", // LSE - Absolute,X (undocumented)

   // RTS
   "RTS: Return from subroutine<br>"
   "Operation:  PC from S, PC + 1 -&gt; PC<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("RTS","60","1","6")
   VARIANT_FOOTER,

   // ADC - (Indirect,X)
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // RRA - (Indirect,X) (undocumented)

   "", // DOP (undocumented)

   // ADC - Zero Page
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROR - Zero Page
   "ROR: Rotate one bit right (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |                              |<br>"
   "             |   +-+    +-+-+-+-+-+-+-+-+   |<br>"
   "Operation:   +-&gt; |C| -&gt; |7|6|5|4|3|2|1|0| &gt;-+<br>"
   "                 +-+    +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROR","6A","1","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("ROR","66","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROR","76","2","6")
   VARIANT VARIANT_ABSOLUTE("ROR","6E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROR","7E","3","7")
   VARIANT_FOOTER,

   "", // RRA - Zero Page (undocumented)

   // PLA
   "PLA: Pull accumulator from stack<br>"
   "Operation:  A from S<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("PLA","68","1","4")
   VARIANT_FOOTER,

   // ADC - Immediate
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROR - Accumulator
   "ROR: Rotate one bit right (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |                              |<br>"
   "             |   +-+    +-+-+-+-+-+-+-+-+   |<br>"
   "Operation:   +-&gt; |C| -&gt; |7|6|5|4|3|2|1|0| &gt;-+<br>"
   "                 +-+    +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ACCUMULATOR("ROR","6A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROR","66","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROR","76","2","6")
   VARIANT VARIANT_ABSOLUTE("ROR","6E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROR","7E","3","7")
   VARIANT_FOOTER,

   "", // ARR - Immediate (undocumented)

   // JMP - Indirect
   "JMP: Jump to new location<br>"
   "Operation:  (PC + 1) -&gt; PCL<br>"
   "            (PC + 2) -&gt; PCH<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ABSOLUTE("JMP","4C","3","3")
   VARIANT_SELECTED VARIANT_INDIRECT("JMP","6C","3","5")
   VARIANT_FOOTER,

   // ADC - Absolute
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROR - Absolute
   "ROR: Rotate one bit right (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |                              |<br>"
   "             |   +-+    +-+-+-+-+-+-+-+-+   |<br>"
   "Operation:   +-&gt; |C| -&gt; |7|6|5|4|3|2|1|0| &gt;-+<br>"
   "                 +-+    +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROR","6A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROR","66","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROR","76","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("ROR","6E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROR","7E","3","7")
   VARIANT_FOOTER,

   "", // RRA - Absolute (undocumented)

   // BVS
   "BVS: Branch on overflow set<br>"
   "Operation:  Branch on V = 1<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BVS","70","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // ADC - (Indirect),Y
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // RRA - (Indirect),Y (undocumented)

   "", // DOP (undocumented)

   // ADC - Zero Page,X
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROR - Zero Page,X
   "ROR: Rotate one bit right (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |                              |<br>"
   "             |   +-+    +-+-+-+-+-+-+-+-+   |<br>"
   "Operation:   +-&gt; |C| -&gt; |7|6|5|4|3|2|1|0| &gt;-+<br>"
   "                 +-+    +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROR","6A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROR","66","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("ROR","76","2","6")
   VARIANT VARIANT_ABSOLUTE("ROR","6E","3","6")
   VARIANT VARIANT_ABSOLUTE_X("ROR","7E","3","7")
   VARIANT_FOOTER,

   "", // RRA - Zero Page,X (undocumented)

   // SEI
   "SEI: Set interrupt disable status<br>"
   "Operation:  1 -&gt; I<br>"
   FLAG_EFFECT("","","","1","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("SEI","78","1","2")
   VARIANT_FOOTER,

   // ADC - Absolute,Y
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP (undocumented)

   "", // RRA - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // ADC - Absolute,X
   "ADC: Add memory to accumulator with carry<br>"
   "Operation:  A + M + C -&gt; A, C<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("ADC","69","2","2")
   VARIANT VARIANT_ZEROPAGE("ADC","65","2","3")
   VARIANT VARIANT_ZEROPAGE_X("ADC","75","2","4")
   VARIANT VARIANT_ABSOLUTE("ADC","6D","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("ADC","7D","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("ADC","79","3","4*")
   VARIANT VARIANT_INDIRECT_X("ADC","61","2","6")
   VARIANT VARIANT_INDIRECT_Y("ADC","71","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // ROR - Absolute,X
   "ROR: Rotate one bit right (memory or accumulator)<br>"
   "             +------------------------------+<br>"
   "             |                              |<br>"
   "             |   +-+    +-+-+-+-+-+-+-+-+   |<br>"
   "Operation:   +-&gt; |C| -&gt; |7|6|5|4|3|2|1|0| &gt;-+<br>"
   "                 +-+    +-+-+-+-+-+-+-+-+<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ACCUMULATOR("ROR","6A","1","2")
   VARIANT VARIANT_ZEROPAGE("ROR","66","2","5")
   VARIANT VARIANT_ZEROPAGE_X("ROR","76","2","6")
   VARIANT VARIANT_ABSOLUTE("ROR","6E","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("ROR","7E","3","7")
   VARIANT_FOOTER,

   "", // RRA - Absolute,X (undocumented)

   "", // DOP (undocumented)

   // STA - (Indirect,X)
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT_SELECTED VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   "", // DOP (undocumented)

   "", // AXS - (Indirect,X) (undocumented)

   // STY - Zero Page
   "STY: Store index Y in memory<br>"
   "Operation: Y -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("STY","84","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STY","94","2","4")
   VARIANT VARIANT_ABSOLUTE("STY","8C","3","4")
   VARIANT_FOOTER,

   // STA - Zero Page
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   // STX - Zero Page
   "STX: Store index X in memory<br>"
   "Operation: X -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("STX","86","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STX","96","2","4")
   VARIANT VARIANT_ABSOLUTE("STX","8E","3","4")
   VARIANT_FOOTER,

   "", // AXS - Zero Page (undocumented)

   // DEY
   "DEY: Decrement index Y by one<br>"
   "Operation:  Y - 1 -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("DEY","88","1","2")
   VARIANT_FOOTER,

   "", // DOP (undocumented)

   // TXA
   "TXA: Transfer index X to accumulator<br>"
   "Operation:  X -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TXA","8A","1","2")
   VARIANT_FOOTER,

   "", // XAA - Immediate (undocumented)

   // STY - Absolute
   "STY: Store index Y in memory<br>"
   "Operation: Y -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STY","84","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STY","94","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("STY","8C","3","4")
   VARIANT_FOOTER,

   // STA - Absolute
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   // STX - Absolute
   "STX: Store index X in memory<br>"
   "Operation: X -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STX","86","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STX","96","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("STX","8E","3","4")
   VARIANT_FOOTER,

   "", // AXS - Absolulte (undocumented)

   // BCC
   "BCC: Branch on carry clear<br>"
   "Operation:  Branch on C = 0<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BCC","90","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // STA - (Indirect),Y
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   "", // KIL - Implied (processor lock up!)

   "", // AXA - (Indirect),Y

   // STY - Zero Page,X
   "STY: Store index Y in memory<br>"
   "Operation: Y -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STY","84","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("STY","94","2","4")
   VARIANT VARIANT_ABSOLUTE("STY","8C","3","4")
   VARIANT_FOOTER,

   // STA - Zero Page,X
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   // STX - Zero Page,Y
   "STX: Store index X in memory<br>"
   "Operation: X -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STX","86","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("STX","96","2","4")
   VARIANT VARIANT_ABSOLUTE("STX","8E","3","4")
   VARIANT_FOOTER,

   "", // AXS - Zero Page,Y

   // TYA
   "TYA: Transfer index Y to accumulator<br>"
   "Operation:  Y -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TYA","98","1","2")
   VARIANT_FOOTER,

   // STA - Absolute,Y
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   // TXS
   "TXS: Transfer index X to stack pointer<br>"
   "Operation:  X -&gt; S<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TXS","9A","1","2")
   VARIANT_FOOTER,

   "", // TAS - Absolute,Y (undocumented)

   "", // SAY - Absolute,X (undocumented)

   // STA - Absolute,X
   "STA: Store accumulator in memory<br>"
   "Operation:  A -&gt; M<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("STA","85","2","3")
   VARIANT VARIANT_ZEROPAGE_X("STA","95","2","4")
   VARIANT VARIANT_ABSOLUTE("STA","8D","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("STA","9D","3","5")
   VARIANT VARIANT_ABSOLUTE_Y("STA","99","3","5")
   VARIANT VARIANT_INDIRECT_X("STA","81","2","6")
   VARIANT VARIANT_INDIRECT_Y("STA","91","2","6")
   VARIANT_FOOTER,

   "", // XAS - Absolute,Y (undocumented)

   "", // AXA - Absolute,Y (undocumented)

   // LDY - Immediate
   "LDY: Load index Y with memory<br>"
   "Operation:  M -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("LDY","A0","2","2")
   VARIANT VARIANT_ZEROPAGE("LDY","A4","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDY","B4","2","4")
   VARIANT VARIANT_ABSOLUTE("LDY","AC","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDY","BC","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDA - (Indirect,X)
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDX - Immediate
   "LDX: Load index X with memory<br>"
   "Operation:  M -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("LDX","A2","2","2")
   VARIANT VARIANT_ZEROPAGE("LDX","A6","2","3")
   VARIANT VARIANT_ZEROPAGE_Y("LDX","B6","2","4")
   VARIANT VARIANT_ABSOLUTE("LDX","AE","3","4")
   VARIANT VARIANT_ABSOLUTE_Y("LDX","BE","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // LAX - (Indirect,X) (undocumented)

   // LDY - Zero Page
   "LDY: Load index Y with memory<br>"
   "Operation:  M -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDY","A0","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("LDY","A4","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDY","B4","2","4")
   VARIANT VARIANT_ABSOLUTE("LDY","AC","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDY","BC","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDA - Zero Page
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDX - Zero Page
   "LDX: Load index X with memory<br>"
   "Operation:  M -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDX","A2","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("LDX","A6","2","3")
   VARIANT VARIANT_ZEROPAGE_Y("LDX","B6","2","4")
   VARIANT VARIANT_ABSOLUTE("LDX","AE","3","4")
   VARIANT VARIANT_ABSOLUTE_Y("LDX","BE","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // LAX - Zero Page (undocumented)

   // TAY
   "TAY: Transfer accumulator to index Y<br>"
   "Operation:  A -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TAY","A8","1","2")
   VARIANT_FOOTER,

   // LDA - Immediate
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // TAX
   "TAX: Transfer accumulator to index X<br>"
   "Operation:  A -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TAX","AA","1","2")
   VARIANT_FOOTER,

   "", // OAL - Immediate (undocumented)

   // LDY - Absolute
   "LDY: Load index Y with memory<br>"
   "Operation:  M -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDY","A0","2","2")
   VARIANT VARIANT_ZEROPAGE("LDY","A4","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDY","B4","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("LDY","AC","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDY","BC","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDA - Absolute
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDX - Absolute
   "LDX: Load index X with memory<br>"
   "Operation:  M -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDX","A2","2","2")
   VARIANT VARIANT_ZEROPAGE("LDX","A6","2","3")
   VARIANT VARIANT_ZEROPAGE_Y("LDX","B6","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("LDX","AE","3","4")
   VARIANT VARIANT_ABSOLUTE_Y("LDX","BE","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // LAX - Absolute (undocumented)

   // BCS
   "BCS: Branch on carry set<br>"
   "Operation:  Branch on C = 1<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BCS","B0","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // LDA - (Indirect),Y
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // LAX - (Indirect),Y (undocumented)

   // LDY - Zero Page,X
   "LDY: Load index Y with memory<br>"
   "Operation:  M -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDY","A0","2","2")
   VARIANT VARIANT_ZEROPAGE("LDY","A4","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("LDY","B4","2","4")
   VARIANT VARIANT_ABSOLUTE("LDY","AC","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDY","BC","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDA - Zero Page,X
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDX - Zero Page,Y
   "LDX: Load index X with memory<br>"
   "Operation:  M -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDX","A2","2","2")
   VARIANT VARIANT_ZEROPAGE("LDX","A6","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_Y("LDX","B6","2","4")
   VARIANT VARIANT_ABSOLUTE("LDX","AE","3","4")
   VARIANT VARIANT_ABSOLUTE_Y("LDX","BE","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // LAX - Zero Page,X (undocumented)

   // CLV
   "CLV: Clear overflow flag<br>"
   "Operation:  0 -&gt; V<br>"
   FLAG_EFFECT("","","","","","0")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("CLV","B8","1","2")
   VARIANT_FOOTER,

   // LDA - Absolute,Y
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // TSX
   "TSX: Transfer stack pointer to index X<br>"
   "Operation:  S -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("TSX","BA","1","2")
   VARIANT_FOOTER,

   "", // LAS - Absolute,Y (undocumented)

   // LDY - Absolute,X
   "LDY: Load index Y with memory<br>"
   "Operation:  M -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDY","A0","2","2")
   VARIANT VARIANT_ZEROPAGE("LDY","A4","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDY","B4","2","4")
   VARIANT VARIANT_ABSOLUTE("LDY","AC","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("LDY","BC","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDA - Absolute,X
   "LDA: Load accumulator with memory<br>"
   "Operation:  M -&gt; A<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDA","A9","2","2")
   VARIANT VARIANT_ZEROPAGE("LDA","A5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("LDA","B5","2","4")
   VARIANT VARIANT_ABSOLUTE("LDA","AD","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("LDA","BD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("LDA","B9","3","4*")
   VARIANT VARIANT_INDIRECT_X("LDA","A1","2","6")
   VARIANT VARIANT_INDIRECT_Y("LDA","B1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // LDX - Absolute,Y
   "LDX: Load index X with memory<br>"
   "Operation:  M -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("LDX","A2","2","2")
   VARIANT VARIANT_ZEROPAGE("LDX","A6","2","3")
   VARIANT VARIANT_ZEROPAGE_Y("LDX","B6","2","4")
   VARIANT VARIANT_ABSOLUTE("LDX","AE","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("LDX","BE","3","4*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // LAX - Absolute,Y (undocumented)

   // CPY - Immediate
   "CPY: Compare memory and index Y<br>"
   "Operation:  Y - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("CPY","C0","2","2")
   VARIANT VARIANT_ZEROPAGE("CPY","C4","2","3")
   VARIANT VARIANT_ABSOLUTE("CPY","CC","3","4")
   VARIANT_FOOTER,

   // CMP - (Indirect,X)
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // DOP (undocumented)

   "", // DCM - (Indirect,X) (undocumented)

   // CPY - Zero Page
   "CPY: Compare memory and index Y<br>"
   "Operation:  Y - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CPY","C0","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("CPY","C4","2","3")
   VARIANT VARIANT_ABSOLUTE("CPY","CC","3","4")
   VARIANT_FOOTER,

   // CMP - Zero Page
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // DEC - Zero Page
   "DEC: Decrement memory by one<br>"
   "Operation:  M - 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("DEC","C6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("DEC","D6","2","6")
   VARIANT VARIANT_ABSOLUTE("DEC","CE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("DEC","DE","3","7")
   VARIANT_FOOTER,

   "", // DCM - Zero Page (undocumented)

   // INY
   "INY: Increment Index Y by one<br>"
   "Operation:  Y + 1 -&gt; Y<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("INY","C8","1","2")
   VARIANT_FOOTER,

   // CMP - Immediate
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // DEX
   "DEX: DEX Decrement index X by one<br>"
   "Operation:  X - 1 -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("DEX","CA","1","2")
   VARIANT_FOOTER,

   "", // SAX - Immediate (undocumented)

   // CPY - Absolute
   "CPY: Compare memory and index Y<br>"
   "Operation:  Y - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CPY","C0","2","2")
   VARIANT VARIANT_ZEROPAGE("CPY","C4","2","3")
   VARIANT_SELECTED VARIANT_ABSOLUTE("CPY","CC","3","4")
   VARIANT_FOOTER,

   // CMP - Absolute
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // DEC - Absolute
   "DEC: Decrement memory by one<br>"
   "Operation:  M - 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("DEC","C6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("DEC","D6","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("DEC","CE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("DEC","DE","3","7")
   VARIANT_FOOTER,

   "", // DCM - Absolute (undocumented)

   // BNE
   "BNE: Branch on result not zero<br>"
   "Operation:  Branch on Z = 0<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BNE","D0","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // CMP   (Indirect),Y
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // DCM - (Indirect),Y (undocumented)

   "", // DOP (undocumented)

   // CMP - Zero Page,X
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // DEC - Zero Page,X
   "DEC: Decrement memory by one<br>"
   "Operation:  M - 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("DEC","C6","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("DEC","D6","2","6")
   VARIANT VARIANT_ABSOLUTE("DEC","CE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("DEC","DE","3","7")
   VARIANT_FOOTER,

   "", // DCM - Zero Page,X (undocumented)

   // CLD
   "CLD: Clear decimal mode<br>"
   "Operation:  0 -&gt; D<br>"
   FLAG_EFFECT("","","","","0","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("CLD","D8","1","2")
   VARIANT_FOOTER,

   // CMP - Absolute,Y
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP (undocumented)

   "", // DCM - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // CMP - Absolute,X
   "CMP: Compare memory and accumulator<br>"
   "Operation:  A - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CMP","C9","2","2")
   VARIANT VARIANT_ZEROPAGE("CMP","C5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("CMP","D5","2","4")
   VARIANT VARIANT_ABSOLUTE("CMP","CD","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("CMP","DD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("CMP","D9","3","4*")
   VARIANT VARIANT_INDIRECT_X("CMP","C1","2","6")
   VARIANT VARIANT_INDIRECT_Y("CMP","D1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // DEC - Absolute,X
   "DEC: Decrement memory by one<br>"
   "Operation:  M - 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("DEC","C6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("DEC","D6","2","6")
   VARIANT VARIANT_ABSOLUTE("DEC","CE","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("DEC","DE","3","7")
   VARIANT_FOOTER,

   "", // DCM - Absolute,X (undocumented)

   // CPX - Immediate
   "CPX: Compare memory and index X<br>"
   "Operation:  X - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("CPX","E0","2","2")
   VARIANT VARIANT_ZEROPAGE("CPX","E4","2","3")
   VARIANT VARIANT_ABSOLUTE("CPX","EC","3","4")
   VARIANT_FOOTER,

   // SBC - (Indirect,X)
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT_SELECTED VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // DOP (undocumented)

   "", // INS - (Indirect,X) (undocumented)

   // CPX - Zero Page
   "CPX: Compare memory and index X<br>"
   "Operation:  X - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CPX","E0","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("CPX","E4","2","3")
   VARIANT VARIANT_ABSOLUTE("CPX","EC","3","4")
   VARIANT_FOOTER,

   // SBC - Zero Page
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT_SELECTED VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // INC - Zero Page
   "INC: Increment memory by one<br>"
   "Operation:  M + 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_ZEROPAGE("INC","E6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("INC","F6","2","6")
   VARIANT VARIANT_ABSOLUTE("INC","EE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("INC","FE","3","7")
   VARIANT_FOOTER,

   "", // INS - Zero Page (undocumented)

   // INX
   "INX: Increment Index X by one<br>"
   "Operation:  X + 1 -&gt; X<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("INX","E8","1","2")
   VARIANT_FOOTER,

   // SBC - Immediate
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP

   // SBC - Immediate (undocumented)
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // CPX - Absolute
   "CPX: Compare memory and index X<br>"
   "Operation:  X - M<br>"
   FLAG_EFFECT("/","/","/","","","")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("CPX","E0","2","2")
   VARIANT VARIANT_ZEROPAGE("CPX","E4","2","3")
   VARIANT_SELECTED VARIANT_ABSOLUTE("CPX","EC","3","4")
   VARIANT_FOOTER,

   // SBC - Absolute
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // INC - Absolute
   "INC: Increment memory by one<br>"
   "Operation:  M + 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("INC","E6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("INC","F6","2","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE("INC","EE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("INC","FE","3","7")
   VARIANT_FOOTER,

   "", // INS - Absolute (undocumented)

   // BEQ
   "BEQ: Branch on result zero<br>"
   "Operation:  Branch on Z = 1<br>"
   FLAG_EFFECT("","","","","","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_RELATIVE("BEQ","F0","2","2*")
   VARIANT_FOOTER
   "* Add 1 if branch occurs to same page.<br>"
   "* Add 2 if branch occurs to different page.",

   // SBC - (Indirect),Y
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT_SELECTED VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // KIL - Implied (processor lock up!)

   "", // INS - (Indirect),Y (undocumented)

   "", // DOP (undocumented)

   // SBC - Zero Page,X
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // INC - Zero Page,X
   "INC: Increment memory by one<br>"
   "Operation:  M + 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("INC","E6","2","5")
   VARIANT_SELECTED VARIANT_ZEROPAGE_X("INC","F6","2","6")
   VARIANT VARIANT_ABSOLUTE("INC","EE","3","6")
   VARIANT VARIANT_ABSOLUTE_X("INC","FE","3","7")
   VARIANT_FOOTER,

   "", // INS - Zero Page,X (undocumented)

   // SED
   "SED: Set decimal mode<br>"
   "Operation:  1 -&gt; D<br>"
   FLAG_EFFECT("","","","","1","")
   VARIANT_HEADER
   VARIANT_SELECTED VARIANT_IMPLIED("SED","F8","1","2")
   VARIANT_FOOTER,

   // SBC - Absolute,Y
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT_SELECTED VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   "", // NOP (undocumented)

   "", // INS - Absolute,Y (undocumented)

   "", // TOP (undocumented)

   // SBC - Absolute,X
   "SBC: Subtract memory from accumulator with borrow<br>"
   "Operation:  A - M - (1-C) -&gt; A<br>"
   FLAG_EFFECT("/","/","/","","","/")
   VARIANT_HEADER
   VARIANT VARIANT_IMMEDIATE("SBC","E9","2","2")
   VARIANT VARIANT_ZEROPAGE("SBC","E5","2","3")
   VARIANT VARIANT_ZEROPAGE_X("SBC","F5","2","4")
   VARIANT VARIANT_ABSOLUTE("SBC","ED","3","4")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("SBC","FD","3","4*")
   VARIANT VARIANT_ABSOLUTE_Y("SBC","F9","3","4*")
   VARIANT VARIANT_INDIRECT_X("SBC","E1","2","6")
   VARIANT VARIANT_INDIRECT_Y("SBC","F1","2","5*")
   VARIANT_FOOTER
   "* Add 1 if page boundary is crossed.",

   // INC - Absolute,X
   "INC: Increment memory by one<br>"
   "Operation:  M + 1 -&gt; M<br>"
   FLAG_EFFECT("/","/","","","","")
   VARIANT_HEADER
   VARIANT VARIANT_ZEROPAGE("INC","E6","2","5")
   VARIANT VARIANT_ZEROPAGE_X("INC","F6","2","6")
   VARIANT VARIANT_ABSOLUTE("INC","EE","3","6")
   VARIANT_SELECTED VARIANT_ABSOLUTE_X("INC","FE","3","7")
   VARIANT_FOOTER,

   ""  // INS - Absolute,X (undocumented)
};

const char* OPCODEINFO(uint8_t op)
{
   return *(m_6502opcodeInfo+op);
}

uint8_t OPCODESIZE ( uint8_t op )
{
   C6502_opcode* pOp;

   pOp = m_6502opcode+op;
   return *(opcode_size+pOp->amode);
}

static C6502DBG __init __attribute((unused));

C6502DBG::C6502DBG()
{
   int32_t addr;
   for ( addr = 0; addr < MEM_2KB; addr++ )
   {
      m_RAMdisassembly[addr] = new char [ 16 ];
   }
}

C6502DBG::~C6502DBG()
{
   int32_t addr;
   for ( addr = 0; addr < MEM_2KB; addr++ )
   {
      delete [] m_RAMdisassembly[addr];
   }
}

static QColor color [] =
{
   QColor(255,0,0),
   QColor(0,255,0),
   QColor(0,0,255),
   QColor(255,255,0)
};

static QColor dmaColor [] =
{
   QColor(0,0,0),
   QColor(255,0,255),
   QColor(0,255,255),
   QColor(0,0,0)
};

void C6502DBG::RENDERCODEDATALOGGER ( void )
{
   uint32_t idxx, idxy;
   uint32_t cycleDiff;
   uint32_t curCycle = CCodeDataLogger::GetCurCycle ();
   QColor lcolor;
   CCodeDataLogger* pLogger;
   LoggerInfo* pLogEntry;
   int8_t* pTV;

   // Clearly...
   memset ( m_pCodeDataLoggerInspectorTV, 255, 196608 );

   pTV = (int8_t*)m_pCodeDataLoggerInspectorTV;

   // Show CPU RAM...
   pLogger = &m_logger;
   for ( idxx = 0; idxx < MEM_2KB; idxx++ )
   {
      pLogEntry = pLogger->GetLogEntry(idxx);
      cycleDiff = (curCycle-pLogEntry->cycle)/17800;
      if ( cycleDiff > 199 ) cycleDiff = 199;

      if ( pLogEntry->count )
      {
         if ( pLogEntry->type == eLogger_DMA )
         {
            lcolor = dmaColor[(int)pLogEntry->source];
         }
         else
         {
            lcolor = color[(int)pLogEntry->type];
         }
         if ( !lcolor.red() )
         {
            lcolor.setRed(cycleDiff);
         }
         if ( !lcolor.green() )
         {
            lcolor.setGreen(cycleDiff);
         }
         if ( !lcolor.blue() )
         {
            lcolor.setBlue(cycleDiff);
         }
         *pTV = lcolor.red();
         *(pTV+1) = lcolor.green();
         *(pTV+2) = lcolor.blue();
      }

      pTV += 3;
   }

   // Show I/O region...
   pLogger = &m_logger;
   pTV = (int8_t*)(m_pCodeDataLoggerInspectorTV+(MEM_8KB*3));

   for ( idxx = MEM_8KB; idxx < 0x5C00; idxx++ )
   {
      pLogEntry = pLogger->GetLogEntry(idxx);
      cycleDiff = (curCycle-pLogEntry->cycle)/17800;
      if ( cycleDiff > 199 ) cycleDiff = 199;

      if ( pLogEntry->count )
      {
         if ( pLogEntry->type == eLogger_DMA )
         {
            lcolor = dmaColor[(int)pLogEntry->source];
         }
         else
         {
            lcolor = color[(int)pLogEntry->type];
         }
         if ( !lcolor.red() )
         {
            lcolor.setRed(cycleDiff);
         }
         if ( !lcolor.green() )
         {
            lcolor.setGreen(cycleDiff);
         }
         if ( !lcolor.blue() )
         {
            lcolor.setBlue(cycleDiff);
         }
         *pTV = lcolor.red();
         *(pTV+1) = lcolor.green();
         *(pTV+2) = lcolor.blue();
      }

      pTV += 3;
   }

   // Show cartrige EXRAM memory...
   pLogger = CROMDBG::EXRAMLOGGER();
   pTV = (int8_t*)(m_pCodeDataLoggerInspectorTV+(0x5C00*3));

   for ( idxx = 0; idxx < MEM_1KB; idxx++ )
   {
      pLogEntry = pLogger->GetLogEntry(idxx);
      cycleDiff = (curCycle-pLogEntry->cycle)/17800;
      if ( cycleDiff > 199 ) cycleDiff = 199;

      if ( pLogEntry->count )
      {
         if ( pLogEntry->type == eLogger_DMA )
         {
            lcolor = dmaColor[(int)pLogEntry->source];
         }
         else
         {
            lcolor = color[(int)pLogEntry->type];
         }
         if ( !lcolor.red() )
         {
            lcolor.setRed(cycleDiff);
         }
         if ( !lcolor.green() )
         {
            lcolor.setGreen(cycleDiff);
         }
         if ( !lcolor.blue() )
         {
            lcolor.setBlue(cycleDiff);
         }
         *pTV = lcolor.red();
         *(pTV+1) = lcolor.green();
         *(pTV+2) = lcolor.blue();
      }

      pTV += 3;
   }

   // Show cartrige SRAM memory...
   pTV = (int8_t*)(m_pCodeDataLoggerInspectorTV+(0x6000*3));

   for ( idxx = 0; idxx < MEM_8KB; idxx++ )
   {
      pLogger = CROMDBG::SRAMLOGGER(0x6000);

      pLogEntry = pLogger->GetLogEntry(idxx);
      cycleDiff = (curCycle-pLogEntry->cycle)/17800;
      if ( cycleDiff > 199 ) cycleDiff = 199;

      if ( pLogEntry->count )
      {
         if ( pLogEntry->type == eLogger_DMA )
         {
            lcolor = dmaColor[(int)pLogEntry->source];
         }
         else
         {
            lcolor = color[(int)pLogEntry->type];
         }
         if ( !lcolor.red() )
         {
            lcolor.setRed(cycleDiff);
         }
         if ( !lcolor.green() )
         {
            lcolor.setGreen(cycleDiff);
         }
         if ( !lcolor.blue() )
         {
            lcolor.setBlue(cycleDiff);
         }
         *pTV = lcolor.red();
         *(pTV+1) = lcolor.green();
         *(pTV+2) = lcolor.blue();
      }

      pTV += 3;
   }

   // Show cartrige PRG-ROM memory...
   pTV = (int8_t*)(m_pCodeDataLoggerInspectorTV+(MEM_32KB*3));

   for ( idxy = 0; idxy < 4; idxy++ )
   {
      for ( idxx = 0; idxx < MEM_8KB; idxx++ )
      {
         pLogger = CROMDBG::LOGGER(MEM_32KB+(idxy*MEM_8KB)+idxx);

         pLogEntry = pLogger->GetLogEntry(idxx);
         cycleDiff = (curCycle-pLogEntry->cycle)/17800;
         if ( cycleDiff > 199 ) cycleDiff = 199;

         if ( pLogEntry->count )
         {
            if ( pLogEntry->type == eLogger_DMA )
            {
               lcolor = dmaColor[(int)pLogEntry->source];
            }
            else
            {
               lcolor = color[(int)pLogEntry->type];
            }
            if ( !lcolor.red() )
            {
               lcolor.setRed(cycleDiff);
            }
            if ( !lcolor.green() )
            {
               lcolor.setGreen(cycleDiff);
            }
            if ( !lcolor.blue() )
            {
               lcolor.setBlue(cycleDiff);
            }
            *pTV = lcolor.red();
            *(pTV+1) = lcolor.green();
            *(pTV+2) = lcolor.blue();
         }

         pTV += 3;
      }
   }
}

void C6502DBG::RENDEREXECUTIONVISUALIZER ( void )
{
   uint32_t idxx, idxy;
   MarkerSetInfo* pMarker;
   int32_t marker;
   int32_t frameDiff;
   int8_t marked;
   uint32_t numScanlines = CPPUDBG::SCANLINES();
   int8_t* pTV = (int8_t*)m_pExecutionVisualizerInspectorTV;

   for ( idxy = 0; idxy < 512; idxy++ )
   {
      for ( idxx = 0; idxx < 512; idxx++ )
      {
         if ( (idxx >= PPU_CYCLES_PER_SCANLINE) || (idxy >= numScanlines) )
         {
            // Black otherwise...
            *pTV = 0;
            *(pTV+1) = 0;
            *(pTV+2) = 0;
         }

         if ( (idxx < PPU_CYCLES_PER_SCANLINE) && (idxy < numScanlines) )
         {
            marked = 0;
            for ( marker = 0; marker < m_marker.GetNumMarkers(); marker++ )
            {
               pMarker = m_marker.GetMarker(marker);
               frameDiff = pMarker->endFrame-pMarker->startFrame;
               if ( ((pMarker->state == eMarkerSet_Started) ||
                    (pMarker->state == eMarkerSet_Complete)) &&
                    (pMarker->endCycle != MARKER_NOT_MARKED) &&
                    (((frameDiff == 0) &&
                    (VISY_VISX_TO_CYCLE(idxy,idxx) >= pMarker->startCycle) &&
                    (VISY_VISX_TO_CYCLE(idxy,idxx) <= pMarker->endCycle)) ||
                    ((frameDiff == 1) &&
                    ((pMarker->startCycle > pMarker->endCycle) &&
                    ((VISY_VISX_TO_CYCLE(idxy,idxx) >= pMarker->startCycle) ||
                    (VISY_VISX_TO_CYCLE(idxy,idxx) <= pMarker->endCycle)))) ||
                    (frameDiff > 1)) )
               {
                  // Marker color!
                  *pTV = pMarker->red;
                  *(pTV+1) = pMarker->green;
                  *(pTV+2) = pMarker->blue;
                  marked++;
               }
            }
            if ( !marked  )
            {
               if ( (idxx < 256) && (idxy < 240) )
               {
                  // Screen...
                  *pTV = nesGetTVOut()[((idxy<<8) * 3) + (idxx * 3) + 0];
                  *(pTV+1) = nesGetTVOut()[((idxy<<8) * 3) + (idxx * 3) + 1];
                  *(pTV+2) = nesGetTVOut()[((idxy<<8) * 3) + (idxx * 3) + 2];
               }
               else
               {
                  // Darker gray backdrop PPU-off time outline...
                  *pTV = 105;
                  *(pTV+1) = 105;
                  *(pTV+2) = 105;
               }
            }
         }

         pTV += 3;
      }
   }
}

void C6502DBG::DISASSEMBLE ()
{
   if ( C6502DBG::__PC() < 0x800 )
   {
      DISASSEMBLE ( m_RAMdisassembly,
                    C6502DBG::_MEMPTR(),
                    MEM_2KB,
                    m_RAMopcodeMask,
                    m_RAMsloc2addr,
                    m_RAMaddr2sloc,
                    &(m_RAMsloc) );
   }
}

void C6502DBG::DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint16_t* addr2sloc, uint32_t* sourceLength )
{
   C6502_opcode* pOp;
   int32_t opSize;
   uint8_t op;
   char* ptr;
   uint8_t mask;

   (*sourceLength) = 0;

   for ( int32_t i = 0; i < binaryLength; )
   {
      op = *(binary+i);
      pOp = m_6502opcode+op;
      opSize = *(opcode_size+pOp->amode);

      mask = *(opcodeMask+i);

      ptr = (*disassembly);

      // save sloc
      (*sloc2addr) = i;
      sloc2addr++;
      (*addr2sloc) = (*sourceLength);
      addr2sloc++;

      // If we've discovered this address has been executed by the 6502 we'll
      // attempt to provide disassembly for it...
      if ( (mask) && ((i+opSize) < binaryLength) )
      {
         ptr += sprintf ( ptr, "%s", pOp->name );

         switch ( pOp->amode )
         {
            // Single byte operands
            case AM_IMMEDIATE:
            case AM_ZEROPAGE_INDEXED_X:
            case AM_ZEROPAGE_INDEXED_Y:
            case AM_ZEROPAGE:
            case AM_PREINDEXED_INDIRECT:
            case AM_POSTINDEXED_INDIRECT:
            case AM_RELATIVE:
               ptr += sprintf ( ptr, operandFmt[pOp->amode], binary[i+1] );
            break;

            // Two byte operands
            case AM_ABSOLUTE:
            case AM_ABSOLUTE_INDEXED_X:
            case AM_ABSOLUTE_INDEXED_Y:
            case AM_INDIRECT:
               ptr += sprintf ( ptr, operandFmt[pOp->amode], binary[i+2], binary[i+1] );
            break;
         }

         if ( opSize > 2 )
         {
            strcpy((*(disassembly+2)), (*disassembly));
            (*addr2sloc) = (*sourceLength);
            addr2sloc++;
         }
         if ( opSize > 1 )
         {
            strcpy((*(disassembly+1)), (*disassembly));
            (*addr2sloc) = (*sourceLength);
            addr2sloc++;
         }

         i += opSize;
         disassembly += opSize;
      }
      else
      {
         ptr += sprintf ( ptr, ".DB $%02X", binary[i] );

         i++;
         disassembly++;
      }

      (*sourceLength)++;
   }
}

char* C6502DBG::Disassemble ( uint8_t* pOpcode, char* buffer )
{
   char* lbuffer = buffer;
   C6502_opcode* pOp = m_6502opcode+(*pOpcode);

   if ( pOp->documented )
   {
      buffer += sprintf ( buffer, "%s", pOp->name );
   }
   else
   {
      buffer += sprintf ( buffer, "*%s", pOp->name );
   }

   switch ( pOp->amode )
   {
      // Single byte operands
      case AM_IMMEDIATE:
      case AM_ZEROPAGE_INDEXED_X:
      case AM_ZEROPAGE_INDEXED_Y:
      case AM_ZEROPAGE:
      case AM_PREINDEXED_INDIRECT:
      case AM_POSTINDEXED_INDIRECT:
      case AM_RELATIVE:
         buffer += sprintf ( buffer, operandFmt[pOp->amode],
                             (*(pOpcode+1)) );
      break;

      // Two byte operands
      case AM_ABSOLUTE:
      case AM_ABSOLUTE_INDEXED_X:
      case AM_ABSOLUTE_INDEXED_Y:
      case AM_INDIRECT:
         buffer += sprintf ( buffer, operandFmt[pOp->amode],
                             (*(pOpcode+2)),
                             (*(pOpcode+1)) );
      break;
   }
   (*buffer) = 0;

   return lbuffer;
}
