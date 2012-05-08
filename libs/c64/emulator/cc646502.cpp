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

#include "cc646502.h"

#include "c64_emulator_core.h"

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

static CBitfieldData* tblCPUNMIVectorBitfields [] =
{
   new CBitfieldData("NMI Vector", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPUIRQVectorBitfields [] =
{
   new CBitfieldData("IRQ Vector", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPURESETVectorBitfields [] =
{
   new CBitfieldData("RESET Vector", 0, 16, "%04X", 0),
};

static CRegisterData* tblCPURegisters [] =
{
   new CRegisterData(CPU_PC, "Program Counter", c64GetCPURegister, c64SetCPURegister, 1, tblCPUPCBitfields),
   new CRegisterData(CPU_A, "Accumulator", c64GetCPURegister, c64SetCPURegister, 1, tblCPUABitfields),
   new CRegisterData(CPU_X, "X Index", c64GetCPURegister, c64SetCPURegister, 1, tblCPUXBitfields),
   new CRegisterData(CPU_Y, "Y Index", c64GetCPURegister, c64SetCPURegister, 1, tblCPUYBitfields),
   new CRegisterData(CPU_SP, "Stack Pointer", c64GetCPURegister, c64SetCPURegister, 1, tblCPUSPBitfields),
   new CRegisterData(CPU_F, "Flags", c64GetCPURegister, c64SetCPURegister, 7, tblCPUFBitfields),
   new CRegisterData(VECTOR_NMI, "NMI Vector", c64GetCPURegister, c64SetCPURegister, 1, tblCPUNMIVectorBitfields),
   new CRegisterData(VECTOR_RESET, "RESET Vector", c64GetCPURegister, c64SetCPURegister, 1, tblCPURESETVectorBitfields),
   new CRegisterData(VECTOR_IRQ, "IRQ Vector", c64GetCPURegister, c64SetCPURegister, 1, tblCPUIRQVectorBitfields)
};

static const char* rowHeadings [] =
{
   "CPU"
};

static const char* columnHeadings [] =
{
   "PC","A","X","Y","SP","F","NMI","RESET","IRQ"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_CPUregs,1,9,9,tblCPURegisters,rowHeadings,columnHeadings);

CRegisterDatabase* CC646502::m_dbRegisters = dbRegisters;
CBreakpointInfo* CC646502::m_breakpoints;

static CMemoryDatabase* dbMemory = new CMemoryDatabase(eMemory_CPU,
                                                       0x0000,
                                                       MEM_64KB,
                                                       16,
                                                       "CPU RAM",
                                                       c64GetMemory,
                                                       c64SetMemory,
                                                       c64GetPrintableAddress,
                                                       true);

CMemoryDatabase* CC646502::m_dbMemory = dbMemory;

uint8_t*  CC646502::m_6502memory = NULL;
uint8_t   CC646502::m_a;
uint8_t   CC646502::m_x;
uint8_t   CC646502::m_y;
uint8_t   CC646502::m_f;
uint16_t  CC646502::m_pc;
uint8_t   CC646502::m_sp;
uint32_t  CC646502::m_pcGoto = 0xFFFFFFFF;

#if 0
CMarker*         CC646502::m_marker = NULL;

CCodeDataLogger* CC646502::m_logger = NULL;
#endif

uint8_t*   CC646502::m_RAMopcodeMask = NULL;
char**     CC646502::m_RAMdisassembly = NULL;
uint16_t*  CC646502::m_RAMsloc2addr = NULL;
uint32_t*  CC646502::m_RAMaddr2sloc = NULL;
uint32_t   CC646502::m_RAMsloc = 0;

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

static CC646502_opcode m_6502opcode [ 256 ] =
{
   { 0x00, "BRK", AM_IMPLIED, 7, true, false, 0x0 }, // BRK
   { 0x01, "ORA", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // ORA - (Indirect,X)
   { 0x02, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x03, "ASO", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // ASO - (Indirect,X) (undocumented)
   { 0x04, "DOP", AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x05, "ORA", AM_ZEROPAGE, 3, true, false, 0x4 }, // ORA - Zero Page
   { 0x06, "ASL", AM_ZEROPAGE, 5, true, false, 0x10 }, // ASL - Zero Page
   { 0x07, "ASO", AM_ZEROPAGE, 5, false, false, 0x10 }, // ASO - Zero Page (undocumented)
   { 0x08, "PHP", AM_IMPLIED, 3, true, false, 0x4 }, // PHP
   { 0x09, "ORA", AM_IMMEDIATE, 2, true, false, 0x2 }, // ORA - Immediate
   { 0x0A, "ASL", AM_ACCUMULATOR, 2, true, false, 0x2 }, // ASL - Accumulator
   { 0x0B, "ANC", AM_IMMEDIATE, 2, false, false, 0x2 }, // ANC - Immediate (undocumented)
   { 0x0C, "TOP", AM_ABSOLUTE, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x0D, "ORA", AM_ABSOLUTE, 4, true, false, 0x8 }, // ORA - Absolute
   { 0x0E, "ASL", AM_ABSOLUTE, 6, true, false, 0x20 }, // ASL - Absolute
   { 0x0F, "ASO", AM_ABSOLUTE, 6, false, false, 0x20 }, // ASO - Absolute (undocumented)
   { 0x10, "BPL", AM_RELATIVE, 2, true, false, 0xA }, // BPL
   { 0x11, "ORA", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // ORA - (Indirect),Y
   { 0x12, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x13, "ASO", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // ASO - (Indirect),Y (undocumented)
   { 0x14, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x15, "ORA", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // ORA - Zero Page,X
   { 0x16, "ASL", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ASL - Zero Page,X
   { 0x17, "ASO", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // ASO - Zero Page,X (undocumented)
   { 0x18, "CLC", AM_IMPLIED, 2, true, false, 0x2 }, // CLC
   { 0x19, "ORA", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // ORA - Absolute,Y
   { 0x1A, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x1B, "ASO", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // ASO - Absolute,Y (undocumented)
   { 0x1C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x1D, "ORA", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // ORA - Absolute,X
   { 0x1E, "ASL", AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // ASL - Absolute,X
   { 0x1F, "ASO", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // ASO - Absolute,X (undocumented)
   { 0x20, "JSR", AM_ABSOLUTE, 6, true, false, 0x20 }, // JSR
   { 0x21, "AND", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // AND - (Indirect,X)
   { 0x22, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x23, "RLA", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // RLA - (Indirect,X) (undocumented)
   { 0x24, "BIT", AM_ZEROPAGE, 3, true, false, 0x4 }, // BIT - Zero Page
   { 0x25, "AND", AM_ZEROPAGE, 3, true, false, 0x4 }, // AND - Zero Page
   { 0x26, "ROL", AM_ZEROPAGE, 5, true, false, 0x10 }, // ROL - Zero Page
   { 0x27, "RLA", AM_ZEROPAGE, 5, false, false, 0x10 }, // RLA - Zero Page (undocumented)
   { 0x28, "PLP", AM_IMPLIED, 4, true, false, 0x8 }, // PLP
   { 0x29, "AND", AM_IMMEDIATE, 2, true, false, 0x2 }, // AND - Immediate
   { 0x2A, "ROL", AM_ACCUMULATOR, 2, true, false, 0x2 }, // ROL - Accumulator
   { 0x2B, "ANC", AM_IMMEDIATE, 2, false, false, 0x2 }, // ANC - Immediate (undocumented)
   { 0x2C, "BIT", AM_ABSOLUTE, 4, true, false, 0x8 }, // BIT - Absolute
   { 0x2D, "AND", AM_ABSOLUTE, 4, true, false, 0x8 }, // AND - Absolute
   { 0x2E, "ROL", AM_ABSOLUTE, 6, true, false, 0x20 }, // ROL - Absolute
   { 0x2F, "RLA", AM_ABSOLUTE, 6, false, false, 0x20 }, // RLA - Absolute (undocumented)
   { 0x30, "BMI", AM_RELATIVE, 2, true, false, 0x2 }, // BMI
   { 0x31, "AND", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // AND - (Indirect),Y
   { 0x32, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x33, "RLA", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // RLA - (Indirect),Y (undocumented)
   { 0x34, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x35, "AND", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // AND - Zero Page,X
   { 0x36, "ROL", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ROL - Zero Page,X
   { 0x37, "RLA", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // RLA - Zero Page,X (undocumented)
   { 0x38, "SEC", AM_IMPLIED, 2, true, false, 0x2 }, // SEC
   { 0x39, "AND", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // AND - Absolute,Y
   { 0x3A, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x3B, "RLA", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // RLA - Absolute,Y (undocumented)
   { 0x3C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x3D, "AND", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // AND - Absolute,X
   { 0x3E, "ROL", AM_ABSOLUTE_INDEXED_X, 7, true, false, 0x40 }, // ROL - Absolute,X
   { 0x3F, "RLA", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // RLA - Absolute,X (undocumented)
   { 0x40, "RTI", AM_IMPLIED, 6, true, false, 0x20 }, // RTI
   { 0x41, "EOR", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // EOR - (Indirect,X)
   { 0x42, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x43, "LSE", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // LSE - (Indirect,X) (undocumented)
   { 0x44, "DOP", AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x45, "EOR", AM_ZEROPAGE, 3, true, false, 0x4 }, // EOR - Zero Page
   { 0x46, "LSR", AM_ZEROPAGE, 5, true, false, 0x10 }, // LSR - Zero Page
   { 0x47, "LSE", AM_ZEROPAGE, 5, false, false, 0x10 }, // LSE - Zero Page (undocumented)
   { 0x48, "PHA", AM_IMPLIED, 3, true, false, 0x4 }, // PHA
   { 0x49, "EOR", AM_IMMEDIATE, 2, true, false, 0x2 }, // EOR - Immediate
   { 0x4A, "LSR", AM_ACCUMULATOR, 2, true, false, 0x2 }, // LSR - Accumulator
   { 0x4B, "ALR", AM_IMMEDIATE, 2, false, false, 0x2 }, // ALR - Immediate (undocumented)
   { 0x4C, "JMP", AM_ABSOLUTE, 3, true, false, 0x4 }, // JMP - Absolute
   { 0x4D, "EOR", AM_ABSOLUTE, 4, true, false, 0x8 }, // EOR - Absolute
   { 0x4E, "LSR", AM_ABSOLUTE, 6, true, false, 0x20 }, // LSR - Absolute
   { 0x4F, "LSE", AM_ABSOLUTE, 6, false, false, 0x20 }, // LSE - Absolute (undocumented)
   { 0x50, "BVC", AM_RELATIVE, 2, true, false, 0xA }, // BVC
   { 0x51, "EOR", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // EOR - (Indirect),Y
   { 0x52, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x53, "LSE", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // LSE - (Indirect),Y
   { 0x54, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x55, "EOR", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // EOR - Zero Page,X
   { 0x56, "LSR", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // LSR - Zero Page,X
   { 0x57, "LSE", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // LSE - Zero Page,X (undocumented)
   { 0x58, "CLI", AM_IMPLIED, 2, true, false, 0x2 }, // CLI
   { 0x59, "EOR", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // EOR - Absolute,Y
   { 0x5A, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x5B, "LSE", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // LSE - Absolute,Y (undocumented)
   { 0x5C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x5D, "EOR", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // EOR - Absolute,X
   { 0x5E, "LSR", AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // LSR - Absolute,X
   { 0x5F, "LSE", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // LSE - Absolute,X (undocumented)
   { 0x60, "RTS", AM_IMPLIED, 6, true, false, 0x20 }, // RTS
   { 0x61, "ADC", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // ADC - (Indirect,X)
   { 0x62, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x63, "RRA", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // RRA - (Indirect,X) (undocumented)
   { 0x64, "DOP", AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x65, "ADC", AM_ZEROPAGE, 3, true, false, 0x4 }, // ADC - Zero Page
   { 0x66, "ROR", AM_ZEROPAGE, 5, true, false, 0x10 }, // ROR - Zero Page
   { 0x67, "RRA", AM_ZEROPAGE, 5, false, false, 0x10 }, // RRA - Zero Page (undocumented)
   { 0x68, "PLA", AM_IMPLIED, 4, true, false, 0x8 }, // PLA
   { 0x69, "ADC", AM_IMMEDIATE, 2, true, false, 0x2 }, // ADC - Immediate
   { 0x6A, "ROR", AM_ACCUMULATOR, 2, true, false, 0x2 }, // ROR - Accumulator
   { 0x6B, "ARR", AM_IMMEDIATE, 2, false, false, 0x2 }, // ARR - Immediate (undocumented)
   { 0x6C, "JMP", AM_INDIRECT, 5, true, false, 0x10 }, // JMP - Indirect
   { 0x6D, "ADC", AM_ABSOLUTE, 4, true, false, 0x8 }, // ADC - Absolute
   { 0x6E, "ROR", AM_ABSOLUTE, 6, true, false, 0x20 }, // ROR - Absolute
   { 0x6F, "RRA", AM_ABSOLUTE, 6, false, false, 0x20 }, // RRA - Absolute (undocumented)
   { 0x70, "BVS", AM_RELATIVE, 2, true, false, 0xA }, // BVS
   { 0x71, "ADC", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // ADC - (Indirect),Y
   { 0x72, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x73, "RRA", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // RRA - (Indirect),Y (undocumented)
   { 0x74, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x75, "ADC", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // ADC - Zero Page,X
   { 0x76, "ROR", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ROR - Zero Page,X
   { 0x77, "RRA", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // RRA - Zero Page,X (undocumented)
   { 0x78, "SEI", AM_IMPLIED, 2, true, false, 0x2 }, // SEI
   { 0x79, "ADC", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // ADC - Absolute,Y
   { 0x7A, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x7B, "RRA", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // RRA - Absolute,Y (undocumented)
   { 0x7C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x7D, "ADC", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // ADC - Absolute,X
   { 0x7E, "ROR", AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // ROR - Absolute,X
   { 0x7F, "RRA", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // RRA - Absolute,X (undocumented)
   { 0x80, "DOP", AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x81, "STA", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // STA - (Indirect,X)
   { 0x82, "DOP", AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x83, "AXS", AM_PREINDEXED_INDIRECT, 6, false, false, 0x20 }, // AXS - (Indirect,X) (undocumented)
   { 0x84, "STY", AM_ZEROPAGE, 3, true, false, 0x4 }, // STY - Zero Page
   { 0x85, "STA", AM_ZEROPAGE, 3, true, false, 0x4 }, // STA - Zero Page
   { 0x86, "STX", AM_ZEROPAGE, 3, true, false, 0x4 }, // STX - Zero Page
   { 0x87, "AXS", AM_ZEROPAGE, 3, false, false, 0x4 }, // AXS - Zero Page (undocumented)
   { 0x88, "DEY", AM_IMPLIED, 2, true, false, 0x2 }, // DEY
   { 0x89, "DOP", AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x8A, "TXA", AM_IMPLIED, 2, true, false, 0x2 }, // TXA
   { 0x8B, "XAA", AM_IMMEDIATE, 2, false, false, 0x2 }, // XAA - Immediate (undocumented)
   { 0x8C, "STY", AM_ABSOLUTE, 4, true, false, 0x8 }, // STY - Absolute
   { 0x8D, "STA", AM_ABSOLUTE, 4, true, false, 0x8 }, // STA - Absolute
   { 0x8E, "STX", AM_ABSOLUTE, 4, true, false, 0x8 }, // STX - Absolute
   { 0x8F, "AXS", AM_ABSOLUTE, 4, false, false, 0x8 }, // AXS - Absolulte (undocumented)
   { 0x90, "BCC", AM_RELATIVE, 2, true, false, 0xA }, // BCC
   { 0x91, "STA", AM_POSTINDEXED_INDIRECT, 6, true, true, 0x20 }, // STA - (Indirect),Y
   { 0x92, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x93, "AXA", AM_POSTINDEXED_INDIRECT, 6, false, true, 0x20 }, // AXA - (Indirect),Y
   { 0x94, "STY", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // STY - Zero Page,X
   { 0x95, "STA", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // STA - Zero Page,X
   { 0x96, "STX", AM_ZEROPAGE_INDEXED_Y, 4, true, false, 0x8 }, // STX - Zero Page,Y
   { 0x97, "AXS", AM_ZEROPAGE_INDEXED_Y, 4, false, false, 0x8 }, // AXS - Zero Page,Y
   { 0x98, "TYA", AM_IMPLIED, 2, true, false, 0x2 }, // TYA
   { 0x99, "STA", AM_ABSOLUTE_INDEXED_Y, 5, true, true, 0x10 }, // STA - Absolute,Y
   { 0x9A, "TXS", AM_IMPLIED, 2, true, false, 0x2 }, // TXS
   { 0x9B, "TAS", AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // TAS - Absolute,Y (undocumented)
   { 0x9C, "SAY", AM_ABSOLUTE_INDEXED_X, 5, false, true, 0x10 }, // SAY - Absolute,X (undocumented)
   { 0x9D, "STA", AM_ABSOLUTE_INDEXED_X, 5, true, true, 0x10 }, // STA - Absolute,X
   { 0x9E, "XAS", AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // XAS - Absolute,Y (undocumented)
   { 0x9F, "AXA", AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // AXA - Absolute,Y (undocumented)
   { 0xA0, "LDY", AM_IMMEDIATE, 2, true, false, 0x2 }, // LDY - Immediate
   { 0xA1, "LDA", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // LDA - (Indirect,X)
   { 0xA2, "LDX", AM_IMMEDIATE, 2, true, false, 0x2 }, // LDX - Immediate
   { 0xA3, "LAX", AM_PREINDEXED_INDIRECT, 6, false, false, 0x20 }, // LAX - (Indirect,X) (undocumented)
   { 0xA4, "LDY", AM_ZEROPAGE, 3, true, false, 0x4 }, // LDY - Zero Page
   { 0xA5, "LDA", AM_ZEROPAGE, 3, true, false, 0x4 }, // LDA - Zero Page
   { 0xA6, "LDX", AM_ZEROPAGE, 3, true, false, 0x4 }, // LDX - Zero Page
   { 0xA7, "LAX", AM_ZEROPAGE, 3, false, false, 0x4 }, // LAX - Zero Page (undocumented)
   { 0xA8, "TAY", AM_IMPLIED, 2, true, false, 0x2 }, // TAY
   { 0xA9, "LDA", AM_IMMEDIATE, 2, true, false, 0x2 }, // LDA - Immediate
   { 0xAA, "TAX", AM_IMPLIED, 2, true, false, 0x2 }, // TAX
   { 0xAB, "OAL", AM_IMMEDIATE, 2, false, false, 0x2 }, // OAL - Immediate
   { 0xAC, "LDY", AM_ABSOLUTE, 4, true, false, 0x8 }, // LDY - Absolute
   { 0xAD, "LDA", AM_ABSOLUTE, 4, true, false, 0x8 }, // LDA - Absolute
   { 0xAE, "LDX", AM_ABSOLUTE, 4, true, false, 0x8 }, // LDX - Absolute
   { 0xAF, "LAX", AM_ABSOLUTE, 4, false, false, 0x8 }, // LAX - Absolute (undocumented)
   { 0xB0, "BCS", AM_RELATIVE, 2, true, false, 0xA }, // BCS
   { 0xB1, "LDA", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // LDA - (Indirect),Y
   { 0xB2, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xB3, "LAX", AM_POSTINDEXED_INDIRECT, 5, false, false, 0x10 }, // LAX - (Indirect),Y (undocumented)
   { 0xB4, "LDY", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // LDY - Zero Page,X
   { 0xB5, "LDA", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // LDA - Zero Page,X
   { 0xB6, "LDX", AM_ZEROPAGE_INDEXED_Y, 4, true, false, 0x8 }, // LDX - Zero Page,Y
   { 0xB7, "LAX", AM_ZEROPAGE_INDEXED_Y, 4, false, false, 0x8 }, // LAX - Zero Page,X (undocumented)
   { 0xB8, "CLV", AM_IMPLIED, 2, true, false, 0x2 }, // CLV
   { 0xB9, "LDA", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // LDA - Absolute,Y
   { 0xBA, "TSX", AM_IMPLIED, 2, true, false, 0x2 }, // TSX
   { 0xBB, "LAS", AM_ABSOLUTE_INDEXED_Y, 4, false, false, 0x8 }, // LAS - Absolute,Y (undocumented)
   { 0xBC, "LDY", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // LDY - Absolute,X
   { 0xBD, "LDA", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // LDA - Absolute,X
   { 0xBE, "LDX", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // LDX - Absolute,Y
   { 0xBF, "LAX", AM_ABSOLUTE_INDEXED_Y, 4, false, false, 0x8 }, // LAX - Absolute,Y (undocumented)
   { 0xC0, "CPY", AM_IMMEDIATE, 2, true, false, 0x2 }, // CPY - Immediate
   { 0xC1, "CMP", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // CMP - (Indirect,X)
   { 0xC2, "DOP", AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0xC3, "DCM", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // DCM - (Indirect,X) (undocumented)
   { 0xC4, "CPY", AM_ZEROPAGE, 3, true, false, 0x4 }, // CPY - Zero Page
   { 0xC5, "CMP", AM_ZEROPAGE, 3, true, false, 0x4 }, // CMP - Zero Page
   { 0xC6, "DEC", AM_ZEROPAGE, 5, true, false, 0x10 }, // DEC - Zero Page
   { 0xC7, "DCM", AM_ZEROPAGE, 5, true, false, 0x10 }, // DCM - Zero Page (undocumented)
   { 0xC8, "INY", AM_IMPLIED, 2, true, false, 0x2 }, // INY
   { 0xC9, "CMP", AM_IMMEDIATE, 2, true, false, 0x2 }, // CMP - Immediate
   { 0xCA, "DEX", AM_IMPLIED, 2, true, false, 0x2 }, // DEX
   { 0xCB, "SAX", AM_IMMEDIATE, 2, false, false, 0x2 }, // SAX - Immediate (undocumented)
   { 0xCC, "CPY", AM_ABSOLUTE, 4, true, false, 0x8 }, // CPY - Absolute
   { 0xCD, "CMP", AM_ABSOLUTE, 4, true, false, 0x8 }, // CMP - Absolute
   { 0xCE, "DEC", AM_ABSOLUTE, 6, true, false, 0x20 }, // DEC - Absolute
   { 0xCF, "DCM", AM_ABSOLUTE, 6, false, false, 0x20 }, // DCM - Absolute (undocumented)
   { 0xD0, "BNE", AM_RELATIVE, 2, true, false, 0xA }, // BNE
   { 0xD1, "CMP", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // CMP   (Indirect),Y
   { 0xD2, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xD3, "DCM", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // DCM - (Indirect),Y (undocumented)
   { 0xD4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0xD5, "CMP", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // CMP - Zero Page,X
   { 0xD6, "DEC", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // DEC - Zero Page,X
   { 0xD7, "DCM", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // DCM - Zero Page,X (undocumented)
   { 0xD8, "CLD", AM_IMPLIED, 2, true, false, 0x2 }, // CLD
   { 0xD9, "CMP", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // CMP - Absolute,Y
   { 0xDA, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0xDB, "DCM", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // DCM - Absolute,Y (undocumented)
   { 0xDC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0xDD, "CMP", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // CMP - Absolute,X
   { 0xDE, "DEC", AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // DEC - Absolute,X
   { 0xDF, "DCM", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // DCM - Absolute,X (undocumented)
   { 0xE0, "CPX", AM_IMMEDIATE, 2, true, false, 0x2 }, // CPX - Immediate
   { 0xE1, "SBC", AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // SBC - (Indirect,X)
   { 0xE2, "DOP", AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0xE3, "INS", AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // INS - (Indirect,X) (undocumented)
   { 0xE4, "CPX", AM_ZEROPAGE, 3, true, false, 0x4 }, // CPX - Zero Page
   { 0xE5, "SBC", AM_ZEROPAGE, 3, true, false, 0x4 }, // SBC - Zero Page
   { 0xE6, "INC", AM_ZEROPAGE, 5, true, false, 0x10 }, // INC - Zero Page
   { 0xE7, "INS", AM_ZEROPAGE, 5, false, false, 0x10 }, // INS - Zero Page (undocumented)
   { 0xE8, "INX", AM_IMPLIED, 2, true, false, 0x2 }, // INX
   { 0xE9, "SBC", AM_IMMEDIATE, 2, true, false, 0x2 }, // SBC - Immediate
   { 0xEA, "NOP", AM_IMPLIED, 2, true, false, 0x2 }, // NOP
   { 0xEB, "SBC", AM_IMMEDIATE, 2, false, false, 0x2 }, // SBC - Immediate (undocumented)
   { 0xEC, "CPX", AM_ABSOLUTE, 4, true, false, 0x8 }, // CPX - Absolute
   { 0xED, "SBC", AM_ABSOLUTE, 4, true, false, 0x8 }, // SBC - Absolute
   { 0xEE, "INC", AM_ABSOLUTE, 6, true, false, 0x20 }, // INC - Absolute
   { 0xEF, "INS", AM_ABSOLUTE, 6, false, false, 0x20 }, // INS - Absolute (undocumented)
   { 0xF0, "BEQ", AM_RELATIVE, 2, true, false, 0xA }, // BEQ
   { 0xF1, "SBC", AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // SBC - (Indirect),Y
   { 0xF2, "KIL", AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xF3, "INS", AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // INS - (Indirect),Y (undocumented)
   { 0xF4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0xF5, "SBC", AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // SBC - Zero Page,X
   { 0xF6, "INC", AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // INC - Zero Page,X
   { 0xF7, "INS", AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // INS - Zero Page,X (undocumented)
   { 0xF8, "SED", AM_IMPLIED, 2, true, false, 0x2 }, // SED
   { 0xF9, "SBC", AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // SBC - Absolute,Y
   { 0xFA, "NOP", AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0xFB, "INS", AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // INS - Absolute,Y (undocumented)
   { 0xFC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0xFD, "SBC", AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // SBC - Absolute,X
   { 0xFE, "INC", AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // INC - Absolute,X
   { 0xFF, "INS", AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }  // INS - Absolute,X (undocumented)
};

static CC646502 __init __attribute((unused));

CC646502::CC646502()
{
   int32_t addr;

   m_breakpoints = new CC64BreakpointInfo();

   m_RAMdisassembly = new char*[MEM_64KB];
   for ( addr = 0; addr < MEM_64KB; addr++ )
   {
      m_RAMdisassembly[addr] = new char [ 16 ];
   }
   m_RAMopcodeMask = new uint8_t[MEM_64KB];
   m_RAMsloc2addr = new uint16_t[MEM_64KB];
   m_RAMaddr2sloc = new uint32_t[MEM_64KB];

   m_6502memory = new uint8_t[MEM_64KB];

#if 0
   m_logger = new CCodeDataLogger ( MEM_32KB, MASK_32KB );

   m_marker = new CMarker;
#endif
}

CC646502::~CC646502()
{
   int32_t addr;

   delete m_breakpoints;

   for ( addr = 0; addr < MEM_64KB; addr++ )
   {
      delete m_RAMdisassembly[addr];
   }
   delete [] m_RAMdisassembly;
   delete [] m_RAMopcodeMask;
   delete [] m_RAMsloc2addr;
   delete [] m_RAMaddr2sloc;

   delete [] m_6502memory;

#if 0
   delete m_logger;

   delete m_marker;
#endif
}

char* CC646502::Disassemble ( uint8_t* pOpcode, char* buffer )
{
   char* lbuffer = buffer;
   CC646502_opcode* pOp = m_6502opcode+(*pOpcode);

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
void CC646502::DISASSEMBLE ()
{
   DISASSEMBLE ( m_RAMdisassembly,
                 m_6502memory,
                 MEM_64KB,
                 m_RAMopcodeMask,
                 m_RAMsloc2addr,
                 m_RAMaddr2sloc,
                 &(m_RAMsloc) );
}

void CC646502::DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint32_t* addr2sloc, uint32_t* sourceLength )
{
   CC646502_opcode* pOp;
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
         sprintf_opcode ( ptr, pOp->name );

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
         sprintf_db(ptr);
         sprintf_02x(ptr,binary[i]);

         i++;
         disassembly++;
      }

      (*sourceLength)++;
   }
}
