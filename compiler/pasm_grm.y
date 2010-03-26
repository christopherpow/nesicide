%{

// This is the grammar generator for the 6502 assembler built
// into NESICIDE, an Integrated Development Environment for
// the 8-bit Nintendo Entertainment System.

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "pasm_types.h"

// Make yacc errors more meaningful
//#define YYERROR_VERBOSE

// **** 6502 information ****
// These are addressing modes for instructions
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

// This is the structure representing an opcode
// both by index [for reverse-lookup if necessary]
// and by name.  Also included are the addressing
// mode [used for intermediate representation
// emittance, and promotions], the number of cycles
// and whether or not the opcode is documented
// [for assembler output or diagnostics].
typedef struct _C6502_opcode
{
   int op;
   char* name;
   int amode;
   int cycles;
   char documented;
} C6502_opcode;

// This is the array of 6502 opcodes per the structure
// defined above.  Undocumented and illegal instructions
// are included.
static C6502_opcode m_6502opcode [ 256 ] =
{
   { 0x00, "BRK", AM_IMPLIED, 7, 1 }, // BRK
   { 0x01, "ORA", AM_PREINDEXED_INDIRECT, 6, 1 }, // ORA - (Indirect,X)
   { 0x02, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x03, "ASO", AM_PREINDEXED_INDIRECT, 8, 0 }, // ASO - (Indirect,X) (undocumented)
   { 0x04, "DOP", AM_ZEROPAGE, 3, 0 }, // DOP (undocumented)
   { 0x05, "ORA", AM_ZEROPAGE, 3, 1 }, // ORA - Zero Page
   { 0x06, "ASL", AM_ZEROPAGE, 5, 1 }, // ASL - Zero Page
   { 0x07, "ASO", AM_ZEROPAGE, 5, 0 }, // ASO - Zero Page (undocumented)
   { 0x08, "PHP", AM_IMPLIED, 3, 1 }, // PHP
   { 0x09, "ORA", AM_IMMEDIATE, 2, 1 }, // ORA - Immediate
   { 0x0A, "ASL", AM_ACCUMULATOR, 2, 1 }, // ASL - Accumulator
   { 0x0B, "ANC", AM_IMMEDIATE, 2, 0 }, // ANC - Immediate (undocumented)
   { 0x0C, "TOP", AM_ABSOLUTE, 4, 0 }, // TOP (undocumented)
   { 0x0D, "ORA", AM_ABSOLUTE, 4, 1 }, // ORA - Absolute
   { 0x0E, "ASL", AM_ABSOLUTE, 6, 1 }, // ASL - Absolute
   { 0x0F, "ASO", AM_ABSOLUTE, 6, 0 }, // ASO - Absolute (undocumented)
   { 0x10, "BPL", AM_RELATIVE, 2, 1 }, // BPL
   { 0x11, "ORA", AM_POSTINDEXED_INDIRECT, 5, 1 }, // ORA - (Indirect),Y
   { 0x12, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x13, "ASO", AM_POSTINDEXED_INDIRECT, 8, 0 }, // ASO - (Indirect),Y (undocumented)
   { 0x14, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0x15, "ORA", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // ORA - Zero Page,X
   { 0x16, "ASL", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // ASL - Zero Page,X
   { 0x17, "ASO", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // ASO - Zero Page,X (undocumented)
   { 0x18, "CLC", AM_IMPLIED, 2, 1 }, // CLC
   { 0x19, "ORA", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // ORA - Absolute,Y
   { 0x1A, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0x1B, "ASO", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // ASO - Absolute,Y (undocumented)
   { 0x1C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0x1D, "ORA", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // ORA - Absolute,X
   { 0x1E, "ASL", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // ASL - Absolute,X
   { 0x1F, "ASO", AM_ABSOLUTE_INDEXED_X, 7, 0 }, // ASO - Absolute,X (undocumented)
   { 0x20, "JSR", AM_ABSOLUTE, 6, 1 }, // JSR
   { 0x21, "AND", AM_PREINDEXED_INDIRECT, 6, 1 }, // AND - (Indirect,X)
   { 0x22, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x23, "RLA", AM_PREINDEXED_INDIRECT, 8, 0 }, // RLA - (Indirect,X) (undocumented)
   { 0x24, "BIT", AM_ZEROPAGE, 3, 1 }, // BIT - Zero Page
   { 0x25, "AND", AM_ZEROPAGE, 3, 1 }, // AND - Zero Page
   { 0x26, "ROL", AM_ZEROPAGE, 5, 1 }, // ROL - Zero Page
   { 0x27, "RLA", AM_ZEROPAGE, 5, 0 }, // RLA - Zero Page (undocumented)
   { 0x28, "PLP", AM_IMPLIED, 4, 1 }, // PLP
   { 0x29, "AND", AM_IMMEDIATE, 2, 1 }, // AND - Immediate
   { 0x2A, "ROL", AM_ACCUMULATOR, 2, 1 }, // ROL - Accumulator
   { 0x2B, "ANC", AM_IMMEDIATE, 2, 0 }, // ANC - Immediate (undocumented)
   { 0x2C, "BIT", AM_ABSOLUTE, 4, 1 }, // BIT - Absolute
   { 0x2D, "AND", AM_ABSOLUTE, 4, 1 }, // AND - Absolute
   { 0x2E, "ROL", AM_ABSOLUTE, 6, 1 }, // ROL - Absolute
   { 0x2F, "RLA", AM_ABSOLUTE, 6, 0 }, // RLA - Absolute (undocumented)
   { 0x30, "BMI", AM_RELATIVE, 2, 1 }, // BMI
   { 0x31, "AND", AM_POSTINDEXED_INDIRECT, 5, 1 }, // AND - (Indirect),Y
   { 0x32, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x33, "RLA", AM_POSTINDEXED_INDIRECT, 8, 0 }, // RLA - (Indirect),Y (undocumented)
   { 0x34, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0x35, "AND", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // AND - Zero Page,X
   { 0x36, "ROL", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // ROL - Zero Page,X
   { 0x37, "RLA", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // RLA - Zero Page,X (undocumented)
   { 0x38, "SEC", AM_IMPLIED, 2, 1 }, // SEC
   { 0x39, "AND", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // AND - Absolute,Y
   { 0x3A, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0x3B, "RLA", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // RLA - Absolute,Y (undocumented)
   { 0x3C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0x3D, "AND", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // AND - Absolute,X
   { 0x3E, "ROL", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // ROL - Absolute,X
   { 0x3F, "RLA", AM_ABSOLUTE_INDEXED_X, 7, 0 }, // RLA - Absolute,X (undocumented)
   { 0x40, "RTI", AM_IMPLIED, 6, 1 }, // RTI
   { 0x41, "EOR", AM_PREINDEXED_INDIRECT, 6, 1 }, // EOR - (Indirect,X)
   { 0x42, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x43, "LSE", AM_PREINDEXED_INDIRECT, 8, 0 }, // LSE - (Indirect,X) (undocumented)
   { 0x44, "DOP", AM_ZEROPAGE, 3, 0 }, // DOP (undocumented)
   { 0x45, "EOR", AM_ZEROPAGE, 3, 1 }, // EOR - Zero Page
   { 0x46, "LSR", AM_ZEROPAGE, 5, 1 }, // LSR - Zero Page
   { 0x47, "LSE", AM_ZEROPAGE, 5, 0 }, // LSE - Zero Page (undocumented)
   { 0x48, "PHA", AM_IMPLIED, 3, 1 }, // PHA
   { 0x49, "EOR", AM_IMMEDIATE, 2, 1 }, // EOR - Immediate
   { 0x4A, "LSR", AM_ACCUMULATOR, 2, 1 }, // LSR - Accumulator
   { 0x4B, "ALR", AM_IMMEDIATE, 2, 0 }, // ALR - Immediate (undocumented)
   { 0x4C, "JMP", AM_ABSOLUTE, 3, 1 }, // JMP - Absolute
   { 0x4D, "EOR", AM_ABSOLUTE, 4, 1 }, // EOR - Absolute
   { 0x4E, "LSR", AM_ABSOLUTE, 6, 1 }, // LSR - Absolute
   { 0x4F, "LSE", AM_ABSOLUTE, 6, 0 }, // LSE - Absolute (undocumented)
   { 0x50, "BVC", AM_RELATIVE, 2, 1 }, // BVC
   { 0x51, "EOR", AM_POSTINDEXED_INDIRECT, 5, 1 }, // EOR - (Indirect),Y
   { 0x52, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x53, "LSE", AM_POSTINDEXED_INDIRECT, 8, 0 }, // LSE - (Indirect),Y
   { 0x54, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0x55, "EOR", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // EOR - Zero Page,X
   { 0x56, "LSR", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // LSR - Zero Page,X
   { 0x57, "LSE", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // LSE - Zero Page,X (undocumented)
   { 0x58, "CLI", AM_IMPLIED, 2, 1 }, // CLI
   { 0x59, "EOR", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // EOR - Absolute,Y
   { 0x5A, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0x5B, "LSE", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // LSE - Absolute,Y (undocumented)
   { 0x5C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0x5D, "EOR", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // EOR - Absolute,X
   { 0x5E, "LSR", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // LSR - Absolute,X
   { 0x5F, "LSE", AM_ABSOLUTE_INDEXED_X, 7, 0 }, // LSE - Absolute,X (undocumented)
   { 0x60, "RTS", AM_IMPLIED, 6, 1 }, // RTS
   { 0x61, "ADC", AM_PREINDEXED_INDIRECT, 6, 1 }, // ADC - (Indirect,X)
   { 0x62, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x63, "RRA", AM_PREINDEXED_INDIRECT, 8, 0 }, // RRA - (Indirect,X) (undocumented)
   { 0x64, "DOP", AM_ZEROPAGE, 3, 0 }, // DOP (undocumented)
   { 0x65, "ADC", AM_ZEROPAGE, 3, 1 }, // ADC - Zero Page
   { 0x66, "ROR", AM_ZEROPAGE, 5, 1 }, // ROR - Zero Page
   { 0x67, "RRA", AM_ZEROPAGE, 5, 0 }, // RRA - Zero Page (undocumented)
   { 0x68, "PLA", AM_IMPLIED, 4, 1 }, // PLA
   { 0x69, "ADC", AM_IMMEDIATE, 2, 1 }, // ADC - Immediate
   { 0x6A, "ROR", AM_ACCUMULATOR, 2, 1 }, // ROR - Accumulator
   { 0x6B, "ARR", AM_IMMEDIATE, 2, 0 }, // ARR - Immediate (undocumented)
   { 0x6C, "JMP", AM_INDIRECT, 5, 1 }, // JMP - Indirect
   { 0x6D, "ADC", AM_ABSOLUTE, 4, 1 }, // ADC - Absolute
   { 0x6E, "ROR", AM_ABSOLUTE, 6, 1 }, // ROR - Absolute
   { 0x6F, "RRA", AM_ABSOLUTE, 6, 0 }, // RRA - Absolute (undocumented)
   { 0x70, "BVS", AM_RELATIVE, 2, 1 }, // BVS
   { 0x71, "ADC", AM_POSTINDEXED_INDIRECT, 5, 1 }, // ADC - (Indirect),Y
   { 0x72, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x73, "RRA", AM_POSTINDEXED_INDIRECT, 8, 0 }, // RRA - (Indirect),Y (undocumented)
   { 0x74, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0x75, "ADC", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // ADC - Zero Page,X
   { 0x76, "ROR", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // ROR - Zero Page,X
   { 0x77, "RRA", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // RRA - Zero Page,X (undocumented)
   { 0x78, "SEI", AM_IMPLIED, 2, 1 }, // SEI
   { 0x79, "ADC", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // ADC - Absolute,Y
   { 0x7A, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0x7B, "RRA", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // RRA - Absolute,Y (undocumented)
   { 0x7C, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0x7D, "ADC", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // ADC - Absolute,X
   { 0x7E, "ROR", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // ROR - Absolute,X
   { 0x7F, "RRA", AM_ABSOLUTE_INDEXED_X, 7, 0 }, // RRA - Absolute,X (undocumented)
   { 0x80, "DOP", AM_IMMEDIATE, 2, 0 }, // DOP (undocumented)
   { 0x81, "STA", AM_PREINDEXED_INDIRECT, 6, 1 }, // STA - (Indirect,X)
   { 0x82, "DOP", AM_IMMEDIATE, 2, 0 }, // DOP (undocumented)
   { 0x83, "AXS", AM_PREINDEXED_INDIRECT, 6, 0 }, // AXS - (Indirect,X) (undocumented)
   { 0x84, "STY", AM_ZEROPAGE, 3, 1 }, // STY - Zero Page
   { 0x85, "STA", AM_ZEROPAGE, 3, 1 }, // STA - Zero Page
   { 0x86, "STX", AM_ZEROPAGE, 3, 1 }, // STX - Zero Page
   { 0x87, "AXS", AM_ZEROPAGE, 3, 0 }, // AXS - Zero Page (undocumented)
   { 0x88, "DEY", AM_IMPLIED, 2, 1 }, // DEY
   { 0x89, "DOP", AM_IMMEDIATE, 2, 0 }, // DOP (undocumented)
   { 0x8A, "TXA", AM_IMPLIED, 2, 1 }, // TXA
   { 0x8B, "XAA", AM_IMMEDIATE, 2, 0 }, // XAA - Immediate (undocumented)
   { 0x8C, "STY", AM_ABSOLUTE, 4, 1 }, // STY - Absolute
   { 0x8D, "STA", AM_ABSOLUTE, 4, 1 }, // STA - Absolute
   { 0x8E, "STX", AM_ABSOLUTE, 4, 1 }, // STX - Absolute
   { 0x8F, "AXS", AM_ABSOLUTE, 4, 0 }, // AXS - Absolulte (undocumented)
   { 0x90, "BCC", AM_RELATIVE, 2, 1 }, // BCC
   { 0x91, "STA", AM_POSTINDEXED_INDIRECT, 6, 1 }, // STA - (Indirect),Y
   { 0x92, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0x93, "AXA", AM_POSTINDEXED_INDIRECT, 6, 0 }, // AXA - (Indirect),Y
   { 0x94, "STY", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // STY - Zero Page,X
   { 0x95, "STA", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // STA - Zero Page,X
   { 0x96, "STX", AM_ZEROPAGE_INDEXED_Y, 4, 1 }, // STX - Zero Page,Y
   { 0x97, "AXS", AM_ZEROPAGE_INDEXED_Y, 4, 0 }, // AXS - Zero Page,Y
   { 0x98, "TYA", AM_IMPLIED, 2, 1 }, // TYA
   { 0x99, "STA", AM_ABSOLUTE_INDEXED_Y, 5, 1 }, // STA - Absolute,Y
   { 0x9A, "TXS", AM_IMPLIED, 2, 1 }, // TXS
   { 0x9B, "TAS", AM_ABSOLUTE_INDEXED_Y, 5, 0 }, // TAS - Absolute,Y (undocumented)
   { 0x9C, "SAY", AM_ABSOLUTE_INDEXED_X, 5, 0 }, // SAY - Absolute,X (undocumented)
   { 0x9D, "STA", AM_ABSOLUTE_INDEXED_X, 5, 1 }, // STA - Absolute,X
   { 0x9E, "XAS", AM_ABSOLUTE_INDEXED_Y, 5, 0 }, // XAS - Absolute,Y (undocumented)
   { 0x9F, "AXA", AM_ABSOLUTE_INDEXED_Y, 5, 0 }, // AXA - Absolute,Y (undocumented)
   { 0xA0, "LDY", AM_IMMEDIATE, 2, 1 }, // LDY - Immediate
   { 0xA1, "LDA", AM_PREINDEXED_INDIRECT, 6, 1 }, // LDA - (Indirect,X)
   { 0xA2, "LDX", AM_IMMEDIATE, 2, 1 }, // LDX - Immediate
   { 0xA3, "LAX", AM_PREINDEXED_INDIRECT, 6, 0 }, // LAX - (Indirect,X) (undocumented)
   { 0xA4, "LDY", AM_ZEROPAGE, 3, 1 }, // LDY - Zero Page
   { 0xA5, "LDA", AM_ZEROPAGE, 3, 1 }, // LDA - Zero Page
   { 0xA6, "LDX", AM_ZEROPAGE, 3, 1 }, // LDX - Zero Page
   { 0xA7, "LAX", AM_ZEROPAGE, 3, 0 }, // LAX - Zero Page (undocumented)
   { 0xA8, "TAY", AM_IMPLIED, 2, 1 }, // TAY
   { 0xA9, "LDA", AM_IMMEDIATE, 2, 1 }, // LDA - Immediate
   { 0xAA, "TAX", AM_IMPLIED, 2, 1 }, // TAX
   { 0xAB, "OAL", AM_IMMEDIATE, 2, 0 }, // OAL - Immediate
   { 0xAC, "LDY", AM_ABSOLUTE, 4, 1 }, // LDY - Absolute
   { 0xAD, "LDA", AM_ABSOLUTE, 4, 1 }, // LDA - Absolute
   { 0xAE, "LDX", AM_ABSOLUTE, 4, 1 }, // LDX - Absolute
   { 0xAF, "LAX", AM_ABSOLUTE, 4, 0 }, // LAX - Absolute (undocumented)
   { 0xB0, "BCS", AM_RELATIVE, 2, 1 }, // BCS
   { 0xB1, "LDA", AM_POSTINDEXED_INDIRECT, 5, 1 }, // LDA - (Indirect),Y
   { 0xB2, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0xB3, "LAX", AM_POSTINDEXED_INDIRECT, 5, 0 }, // LAX - (Indirect),Y (undocumented)
   { 0xB4, "LDY", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // LDY - Zero Page,X
   { 0xB5, "LDA", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // LDA - Zero Page,X
   { 0xB6, "LDX", AM_ZEROPAGE_INDEXED_Y, 4, 1 }, // LDX - Zero Page,Y
   { 0xB7, "LAX", AM_ZEROPAGE_INDEXED_Y, 4, 0 }, // LAX - Zero Page,X (undocumented)
   { 0xB8, "CLV", AM_IMPLIED, 2, 1 }, // CLV
   { 0xB9, "LDA", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // LDA - Absolute,Y
   { 0xBA, "TSX", AM_IMPLIED, 2, 1 }, // TSX
   { 0xBB, "LAS", AM_ABSOLUTE_INDEXED_Y, 4, 0 }, // LAS - Absolute,Y (undocumented)
   { 0xBC, "LDY", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // LDY - Absolute,X
   { 0xBD, "LDA", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // LDA - Absolute,X
   { 0xBE, "LDX", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // LDX - Absolute,Y
   { 0xBF, "LAX", AM_ABSOLUTE_INDEXED_Y, 4, 0 }, // LAX - Absolute,Y (undocumented)
   { 0xC0, "CPY", AM_IMMEDIATE, 2, 1 }, // CPY - Immediate
   { 0xC1, "CMP", AM_PREINDEXED_INDIRECT, 6, 1 }, // CMP - (Indirect,X)
   { 0xC2, "DOP", AM_IMMEDIATE, 2, 0 }, // DOP (undocumented)
   { 0xC3, "DCM", AM_PREINDEXED_INDIRECT, 8, 0 }, // DCM - (Indirect,X) (undocumented)
   { 0xC4, "CPY", AM_ZEROPAGE, 3, 1 }, // CPY - Zero Page
   { 0xC5, "CMP", AM_ZEROPAGE, 3, 1 }, // CMP - Zero Page
   { 0xC6, "DEC", AM_ZEROPAGE, 5, 1 }, // DEC - Zero Page
   { 0xC7, "DCM", AM_ZEROPAGE, 5, 1 }, // DCM - Zero Page (undocumented)
   { 0xC8, "INY", AM_IMPLIED, 2, 1 }, // INY
   { 0xC9, "CMP", AM_IMMEDIATE, 2, 1 }, // CMP - Immediate
   { 0xCA, "DEX", AM_IMPLIED, 2, 1 }, // DEX
   { 0xCB, "SAX", AM_IMMEDIATE, 2, 0 }, // SAX - Immediate (undocumented)
   { 0xCC, "CPY", AM_ABSOLUTE, 4, 1 }, // CPY - Absolute
   { 0xCD, "CMP", AM_ABSOLUTE, 4, 1 }, // CMP - Absolute
   { 0xCE, "DEC", AM_ABSOLUTE, 6, 1 }, // DEC - Absolute
   { 0xCF, "DCM", AM_ABSOLUTE, 6, 0 }, // DCM - Absolute (undocumented)
   { 0xD0, "BNE", AM_RELATIVE, 2, 1 }, // BNE
   { 0xD1, "CMP", AM_POSTINDEXED_INDIRECT, 5, 1 }, // CMP   (Indirect),Y
   { 0xD2, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0xD3, "DCM", AM_POSTINDEXED_INDIRECT, 8, 0 }, // DCM - (Indirect),Y (undocumented)
   { 0xD4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0xD5, "CMP", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // CMP - Zero Page,X
   { 0xD6, "DEC", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // DEC - Zero Page,X
   { 0xD7, "DCM", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // DCM - Zero Page,X (undocumented)
   { 0xD8, "CLD", AM_IMPLIED, 2, 1 }, // CLD
   { 0xD9, "CMP", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // CMP - Absolute,Y
   { 0xDA, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0xDB, "DCM", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // DCM - Absolute,Y (undocumented)
   { 0xDC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0xDD, "CMP", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // CMP - Absolute,X
   { 0xDE, "DEC", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // DEC - Absolute,X
   { 0xDF, "DCM", AM_ABSOLUTE_INDEXED_X, 7, 0 }, // DCM - Absolute,X (undocumented)
   { 0xE0, "CPX", AM_IMMEDIATE, 2, 1 }, // CPX - Immediate
   { 0xE1, "SBC", AM_PREINDEXED_INDIRECT, 6, 1 }, // SBC - (Indirect,X)
   { 0xE2, "DOP", AM_IMMEDIATE, 2, 0 }, // DOP (undocumented)
   { 0xE3, "INS", AM_PREINDEXED_INDIRECT, 8, 0 }, // INS - (Indirect,X) (undocumented)
   { 0xE4, "CPX", AM_ZEROPAGE, 3, 1 }, // CPX - Zero Page
   { 0xE5, "SBC", AM_ZEROPAGE, 3, 1 }, // SBC - Zero Page
   { 0xE6, "INC", AM_ZEROPAGE, 5, 1 }, // INC - Zero Page
   { 0xE7, "INS", AM_ZEROPAGE, 5, 0 }, // INS - Zero Page (undocumented)
   { 0xE8, "INX", AM_IMPLIED, 2, 1 }, // INX
   { 0xE9, "SBC", AM_IMMEDIATE, 2, 1 }, // SBC - Immediate
   { 0xEA, "NOP", AM_IMPLIED, 2, 1 }, // NOP
   { 0xEB, "SBC", AM_IMMEDIATE, 2, 0 }, // SBC - Immediate (undocumented)
   { 0xEC, "CPX", AM_ABSOLUTE, 4, 1 }, // CPX - Absolute
   { 0xED, "SBC", AM_ABSOLUTE, 4, 1 }, // SBC - Absolute
   { 0xEE, "INC", AM_ABSOLUTE, 6, 1 }, // INC - Absolute
   { 0xEF, "INS", AM_ABSOLUTE, 6, 0 }, // INS - Absolute (undocumented)
   { 0xF0, "BEQ", AM_RELATIVE, 2, 1 }, // BEQ
   { 0xF1, "SBC", AM_POSTINDEXED_INDIRECT, 5, 1 }, // SBC - (Indirect),Y
   { 0xF2, "KIL", AM_IMPLIED, 0, 0 }, // KIL - Implied (processor lock up!)
   { 0xF3, "INS", AM_POSTINDEXED_INDIRECT, 8, 0 }, // INS - (Indirect),Y (undocumented)
   { 0xF4, "DOP", AM_ZEROPAGE_INDEXED_X, 4, 0 }, // DOP (undocumented)
   { 0xF5, "SBC", AM_ZEROPAGE_INDEXED_X, 4, 1 }, // SBC - Zero Page,X
   { 0xF6, "INC", AM_ZEROPAGE_INDEXED_X, 6, 1 }, // INC - Zero Page,X
   { 0xF7, "INS", AM_ZEROPAGE_INDEXED_X, 6, 0 }, // INS - Zero Page,X (undocumented)
   { 0xF8, "SED", AM_IMPLIED, 2, 1 }, // SED
   { 0xF9, "SBC", AM_ABSOLUTE_INDEXED_Y, 4, 1 }, // SBC - Absolute,Y
   { 0xFA, "NOP", AM_IMPLIED, 2, 0 }, // NOP (undocumented)
   { 0xFB, "INS", AM_ABSOLUTE_INDEXED_Y, 7, 0 }, // INS - Absolute,Y (undocumented)
   { 0xFC, "TOP", AM_ABSOLUTE_INDEXED_X, 4, 0 }, // TOP (undocumented)
   { 0xFD, "SBC", AM_ABSOLUTE_INDEXED_X, 4, 1 }, // SBC - Absolute,X
   { 0xFE, "INC", AM_ABSOLUTE_INDEXED_X, 7, 1 }, // INC - Absolute,X
   { 0xFF, "INS", AM_ABSOLUTE_INDEXED_X, 7, 0 }  // INS - Absolute,X (undocumented)
};

// A PRG-ROM bank in an iNES file is 16KB.  This could be
// made configurable to support other output file formats.
#define DEFAULT_BANK_SIZE 0x4000

// How many new banks to create when a new bank is specified
// in the assembly code we're parsing.
#define BTAB_ENT_INC 1

// How many new macros to create when a new macro is specified
// in the assembly code we're parsing.
#define MTAB_ENT_INC 1

// Default fill value to use for .space, .dsw, .dsb, etc.
#define DEFAULT_FILL 0x0000
int fillValue = DEFAULT_FILL;

// Hook function for retrieving data from NESICIDE.
incobj_callback_fn incobj_fn = NULL;

// Standard YACC functions.
int yyerror(char *s);
int yylex(void);
int yywrap(void);

// Current file being parsed for more meaningful error/warnings.
//char currentFile [ 256 ];

// Intermediate representation queue pointers.  The assembly
// code we're parsing is first built as a linked list of
// intermediate representation structures.  Once the assembly
// code has been fully and completely parsed we run some
// reduction/optimization algorithms to complete the
// intermediate representation and make it ready for binary
// output.
ir_table* ir_head = NULL;
ir_table* ir_tail = NULL;

// Repeat macro intermediate representation pointer stack.
// Allow for MAX_REPEATS nested REPT/ENDR macros, keeping
// track of the starting intermediate representation node
// for each nesting level.
#define MAX_REPEATS 10
ir_table* ir_repeat_head [ MAX_REPEATS ];
int       ir_repeat_count [ MAX_REPEATS ];
int repeat_level = 0;

// For enumeration mode, we're pretending we're in a
// .data segment to align ASM6 with our internals.  Thus,
// when an .enum directive is encountered, we need to save
// the fact that we're enumerating and the name of the segment
// we came from so we can switch back to the right segment
// on enumeration end.
int btab_ent_prior_to_enum = -1;

// Symbol table interface.  The symbol table is mostly
// built by the lexical analysis but there are some situations
// where a grammar rule needs to create a symbol, such as
// the rule for assigning a value to a global symbol.
// When a symbol that is a label [ie. not a global] is
// declared the intermediate representation link to where
// it was declared is stored in the symbol structure so
// the address of the symbol can be computed.
extern symbol_table* stab;
extern symbol_table* find_symbol ( char* symbol, int bank );
void update_symbol_ir ( symbol_table* stab, ir_table* ir );
extern int stab_ent;
extern int stab_max;
extern int current_label;

// Binary tables.  These represent the array of banks, and currently
// referenced bank into which assembled code should go.  These
// are controlled by the issuance of .text, .data, and .segment
// directives in the assembly code.
binary_table* btab = NULL;
binary_table* cur = NULL;
int btab_ent = 0;
int btab_max = 0;
unsigned char add_binary_bank ( segment_type type, char* symbol );
unsigned char set_binary_bank ( segment_type type, char* bankname );
void set_binary_addr ( unsigned int addr );

// Macros.  These represent the array of macros currently defined
// by the assembly code parsing.
macro_table* mtab = NULL;
int mtab_ent = 0;
int mtab_max = 0;
int start_macro ( char* symbol );
void finish_macro ( int macro );

// This function takes the completed intermediate representation
// and outputs it as a flat binary stream ready to be sent to a file.
void output_binary( char** buffer, int* size );

extern int yylineno;

// Storage space for error strings spit out by the assembler.
int errorCount = 0;
char* errorStorage = NULL;

// Turning this flag on may actually be somewhat dangerous.  Consider
// the fact that there are *several* undocumented variations of NOP
// opcode [no operation], each of which is actually keying a different
// addressing mode through the 6502 and will thus take more than the
// expected cycle count of the documented NOP opcode.  I might consider
// adding a piece of data to the opcode data structures indicating the
// *preferred* opcode to use which may, in cases like the above,
// override the use of an unexpected NOP, but for now I'll suggest leaving
// this flag set to 0 to prevent usage of undocumented opcodes.
int allowUndocumented = 0;

// Routines to create a node representing a pure number,
// a symbolic reference, or an expression part.
extern number_type* get_next_numtype ( void );
extern ref_type* get_next_reftype ( void );
extern expr_type* get_next_exprtype ( void );
extern text_type* get_next_texttype ( void );

unsigned char valid_instr_amode ( int instr, int amode );
char* instr_mnemonic ( unsigned char op );
int promote_instructions ( unsigned char flag );
void reduce_expressions ( void );
void check_fixup ( void );

unsigned int distance ( ir_table* from, ir_table* to );

// Expression handling routines.  These are recursive
// routines that reduce [change 5 + 5 to 10, for example],
// make a copy of, or evaluate the current value of an
// expression.  Expression reduction is done when an
// assembly code line is parsed that contains an expression,
// and also during the final intermediate representation
// processing phase after parsing is complete.  Expressions
// that reference label symbols or global symbols will evaluate
// correctly only when all symbols and their values are known.
// To complicate this a bit, instruction promotion
// [changing an absolute addressing mode to a zeropage
// addressing mode], can change the address of label symbols.
void reduce_expression ( expr_type* expr, symbol_table* hint );
expr_type* copy_expression ( expr_type* expr );
void evaluate_expression ( ir_table* ir, expr_type* expr, unsigned char* evaluated, unsigned char flag, char** symbol );
void destroy_expression ( expr_type* expr );

// Intermediate representation creation routines.
// These routines create an intermediate representation node
// for a given parsed assembly code line and add it to
// the list.  They also attempt rudimentary expression
// removal if the expression is a constant that is known
// not to change.
ir_table* emit_ir ( void );
ir_table* emit_fix ( int addr );
ir_table* emit_label ( int m, int fill );
ir_table* emit_bin ( expr_type* expr );
ir_table* emit_bin2 ( expr_type* expr );
ir_table* emit_bin_string ( expr_type* expr );
ir_table* emit_bin_space ( int m, int fill );
ir_table* emit_bin_align ( int m );
ir_table* emit_bin_data ( unsigned char data );
ir_table* emit_bin_implied ( C6502_opcode* opcode );
ir_table* emit_bin_accumulator ( C6502_opcode* opcode );
ir_table* emit_bin_immediate ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_absolute ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_zeropage ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_relative ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_indirect ( C6502_opcode* opcode, expr_type* num );
ir_table* emit_bin_abs_idx_x ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_abs_idx_y ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_zp_idx_x ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_zp_idx_y ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_pre_idx_ind ( C6502_opcode* opcode, expr_type* expr );
ir_table* emit_bin_post_idx_ind ( C6502_opcode* opcode, expr_type* expr );

// Flags indicating whether or not we are emitting intermediate
// representation.  These flags support directives such as:
// IF 0
// ... (this assembly code is not emitted)
// ELSEIF 1
// ... (this assembly code is emitted)
// ELSE
// ... (this assembly code is not emitted)
// ENDIF
#define MAX_IF_NEST 10
unsigned char emitting[MAX_IF_NEST] = { 1, 0, };
unsigned char emitted[MAX_IF_NEST] = { 0, };
unsigned char preproc_nest_level = 0;

// Function used to support ENUM and MACRO directives.
ir_table* reemit_ir ( ir_table* head, ir_table* tail );

// Error generation.  All parsing errors are constructed into
// the global buffer and then added to the list of errors.
char e [ 256 ];
void add_error ( char* s );
extern int errorCount;
extern char* errorStorage;
extern char* yytext;

// Diagnostic routines for dumping symbol table, binary table, immediate
// representation table, and an expression.
void dump_symbol_table ( void );
void dump_binary_table ( void );
void dump_ir_table ( void );
void dump_ir_expressions ( void );
void dump_expression ( expr_type* expr );
void dump_macro_table ( void );

%}

// Representation of possible types passed to the grammar parser
// from the lexical analyzer.
%union{
   struct _ref_type* ref;
   struct _number_type* num;
   struct _expr_type* expr;
   struct _text_type* text;
	int seg;
   int instr;
	unsigned char flag;
   char* directive;
}
// Terminal tokens passed to the grammar parser from
// the lexical analyzer.
%token <directive> DATAB DATAW DATABHEX
%token <directive> FILLSPACEB FILLSPACEW
%token <directive> VARSPACE
%token <directive> REPEAT ENDREPEAT
%token <directive> MACRO ENDMACRO
%token <directive> ENUMERATE ENDENUMERATE
%token <directive> ORIGIN BASE ADVANCE ALIGN
%token <directive> INCBIN INCOBJ INCLUDE
%token <directive> FILLVALUE
%token <directive> IFDEF IFNDEF
%token <directive> IF ELSEIF ELSE ENDIF
%token <text> QUOTEDSTRING
%token <instr> INSTR
%token <ref> LABEL LABELREF
%token <num> DIGITS
%token <seg> SEGMENT 

// Non-terminal type assignment for expressions.  The
// expression grammar is such that an expression binary
// tree is built as the expression is being parsed.  As
// such the expression's current root node must be passed
// up the parse tree via $$.
%type <expr> expr

// Precedence.  Note that some of this precedence is
// set up to try to avoid shift-reduce or reduce-reduce
// conflicts that are external to the parsing of an expression.
// The most notorious precedence-related problem is how
// to interpret ()'s.  Both an indirect jump,
// and a pre- or post-indexed indirect addressing mode
// expect ('s as part of the token stream for reducing,
// while an expression expects a ( as a shifted token.
%left LOGOR
%left LOGAND
%left '|'
%left '^'
%left '&'
%left EQEQ NOTEQ
%left '<' '>' LTEQ GTEQ
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/' '%'
%left UPLUS UMINUS '!' '~' ULEOF UBEOF
%left '(' ')'
%left ','

// Attempt to force precedence on line ending over anything else!
%left TERM

// Starting rule for grammar parser.
%start program
%%
// A program is a list of statements...
program: program statement
         | statement

// A statement is either:
// a) A lable on a line by itself,
// b) An instruction on a line by itself [instruction terminators follow in individual rules below],
// c) A label and instruction on the same line, or
// d) A blank line
statement: identifier TERM
           | instruction
           | identifier instruction
           | TERM
           | error {
   yyclearin;
}
			  ;

identifier: LABEL {
   ir_table* ptr;
   ptr = emit_label ( 0, fillValue );
   update_symbol_ir ( &(stab[$1->ref.stab_ent]), ptr );

   reduce_expressions ();
   //dump_symbol_table ();
}
			  ;

// An instruction follows a specific format, each of which has a rule or rules to reduce...
instruction: no_params
           | expression_param
           | indirect_param
           | index_reg_param
           | preindexed_indirect_param
           | postindexed_indirect_param
           | disambiguation_1
           | directive
           | disambiguation_2
           ;

// Directive .incbin "<file>" includes a binary file into the intermediate representation stream...
directive: INCBIN QUOTEDSTRING {
	int c;
   FILE* fp = fopen ( $2->string, "rb" );
	if ( fp != NULL )
	{
		for ( c=fgetc(fp); c!=EOF; c=fgetc(fp) )
		{
         emit_bin_data ( c );
		}
	}
	else
	{
      sprintf ( e, "%s: cannot open included file: %s", $1, $2->string );
		yyerror ( e );
	}
}
// Directive .incobj "<file>" includes a NESICIDE object into the parser stream...
           | INCOBJ QUOTEDSTRING TERM {
	char* data = NULL; 
	int c;
	int size = 0;
	int f;
	int buf, orig;
	if ( incobj_fn )
	{
      f = incobj_fn ( $2->string, &data, &size );
		if ( f != 0 )
		{
   //		strcpy ( currentFile, $2->string );
			
			orig = get_current_buffer ();

			buf = yy_scan_string ( data );

			yyparse();

			yy_flush_buffer ( buf );

			yy_switch_to_buffer ( orig );

			yy_delete_buffer ( buf );
			free ( data );

			yyparse();

			yy_flush_buffer ( orig );

	//		strcpy ( currentFile, "" );
		}
		else
		{
         sprintf ( e, "%s: object not found for .incobj: %s", $1, $2->string );
			yyerror ( e );
		}
	}
	else
	{
      sprintf ( e, "%s: directive not supported", $1 );
		yyerror ( e );
	}
}
// Directive .include "<file>" includes a source file into the parser stream...
           | INCLUDE QUOTEDSTRING TERM {
	char* buffer;
	int c;
	int bytes;
	int buf, orig;
   FILE* fp = fopen ( $2->string, "r" );
	if ( fp != NULL )
	{
//		strcpy ( currentFile, $2->string );

		fseek ( fp, 0, SEEK_END );
		bytes = ftell ( fp );
		if ( bytes > 0 )
		{
			fseek ( fp, 0, SEEK_SET );
			buffer = (char*) malloc ( bytes+1 );
			memset ( buffer, 0, bytes+1 );
			if ( buffer != NULL )
			{
				fread ( buffer, 1, bytes, fp );

				orig = get_current_buffer ();

				buf = yy_scan_string ( buffer );

				yyparse();

				yy_flush_buffer ( buf );

				yy_switch_to_buffer ( orig );

				yy_delete_buffer ( buf );
				free ( buffer );

				yyparse();

				yy_flush_buffer ( orig );
			}
		}

//		strcpy ( currentFile, "" );
	}
	else
	{
      sprintf ( e, "%s: cannot open included file: %s", $1, $2->string );
		yyerror ( e );
	}
}
// Directive .fillvalue <value> changes the default filler value.
           | FILLVALUE DIGITS {
   fillValue = $2->number;
}
// Directive <label-name> = <expression> reduces <expression> and assigns the
// expression's value to the global symbol <label-name>.  This is not a #define,
// it is equivalent to ASM6 syntax.  Expression evaluation occurs, rather than
// direct in-line replacement of <expression> wherever it occurs.
           | LABEL '=' expr TERM {
   // allow redefinition of globals...
   if ( stab[$1->ref.stab_ent].expr )
   {
      destroy_expression ( stab[$1->ref.stab_ent].expr );
   }

   // reduce expression
   reduce_expression ( $3, NULL );

   stab[$1->ref.stab_ent].expr = $3;
   stab[$1->ref.stab_ent].ir = NULL;

   //dump_symbol_table ();
}
// Directive byte/word lists.  The blist/wlist rules take care of outputting
// the intermediate representation of the list for us, so there's nothing more
// to do when these rules are reduced.
           | DATAB blist TERM
           | DATAW wlist TERM
           | DATABHEX hlist TERM
// Directive <.text|.data|.segment> "<name>" sets the current segment or creates a new one
// based on whether or not it has seen <name> segment declared yet.  Segments are
// equivalent to banks, which may be a bit of a misnomer that I need to work through.
// CPTODO: address ASM6 BASE directive.
           | SEGMENT QUOTEDSTRING TERM {
   if ( set_binary_bank($1,$2->string) == 0 )
   {
      add_binary_bank ( $1, $2->string );
   }
}
// Directive <.text|.data|.segment> <label> sets the current segment or creates a new one
// based on whether or not it has seen <label> segment declared yet.  Segments are
// equivalent to banks, which may be a bit of a misnomer that I need to work through.
// Note this is equivalent to the above rule but means the segment name does not need
// to be quoted.
// CPTODO: address ASM6 BASE directive.
           | SEGMENT LABELREF TERM {
   if ( set_binary_bank($1,$2->ref.symbol) == 0 )
   {
      add_binary_bank ( $1, $2->ref.symbol );
   }
}
// Directive <.text|.data> sets the current segment to the default segment.  One
// text and one data segment are created by default when parsing starts, so in some
// situations no segmenting is necessary to achieve a working program.
           | SEGMENT TERM {
   set_binary_bank ( $1, ANONYMOUS_BANK );
}
// Directive .org <addr> sets the current segment's address to <addr>.  The
// next emitted intermediate representation node will have <addr> as its address.
           | ORIGIN DIGITS TERM {
   if ( $2->number >= 0 )
   {
      set_binary_addr ( $2->number );
      emit_fix ( $2->number );
   }
   else
   {
      sprintf ( e, "%s: illegal negative value", $1 );
      yyerror ( e );
   }
}
// Directive .org <addr>, <value> sets the current segment's address to <addr> and
// pads the space with <value> if this is not the first .org.  The
// next emitted intermediate representation node will have <addr> as its address.
           | ORIGIN DIGITS ',' DIGITS TERM {
// CPTODO: implement
}
// Directive .base <expression> sets the current segment's address to <expression>.
// The next emitted intermediate representation node will have <addr> as its address.
           | BASE expr TERM {
   unsigned char evaluated;

   reduce_expression ( $2, NULL );

   evaluated = 1;
   evaluate_expression ( ir_tail, $2, &evaluated, FIX, NULL );
   if ( evaluated )
   {
      if ( $2->vtype == value_is_int )
      {
         set_binary_addr ( $2->value.ival );
         emit_fix ( $2->value.ival );
      }
      else
      {
         sprintf ( e, "%s: illegal string literal in expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      sprintf ( e, "%s: cannot fully evaluate expression", $1 );
      yyerror ( e );
   }
}
// Directive .space <label> <value> creates a space <value> bytes long for
// symbol <label> at the current position within the intermediate representation.
// Can be used to declare variables in .data segment.
           | VARSPACE LABELREF DIGITS TERM {
   symbol_table* symtab = NULL;
   ir_table* ptr;

   if ( $2->type == reference_symtab )
   {
      sprintf ( e, "%s: multiple declarations of symbol: %s", $1, stab[$2->ref.stab_ent].symbol );
      yyerror ( e );
   }
   else
   {
      add_symbol ( $2->ref.symbol, &symtab );
      ptr = emit_label ( $3->number, fillValue );
      update_symbol_ir ( symtab, ptr );
      //dump_symbol_table ();
   }
}
// Directive .dsb <length> creates a space <length> bytes long at the
// current position within the intermediate representation.
           | FILLSPACEB expr TERM {
   unsigned char evaluated;

   reduce_expression ( $2, NULL );

   // emit pure placeholder label...
   emit_label ( 0, 0 );

   // Check if this is a fixed-size block or a
   // variable-sized block.  If the expression contains
   // the special '$' symbol it is variable-sized since the
   // intent of the programmer is most likely equivalent to
   // a .pad directive [it may not be but this is the best
   // we can do given that our instruction stream might
   // shift due to promotions later on.
   evaluated = 1;
   evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
   if ( evaluated )
   {
      if ( $2->vtype == value_is_int )
      {
         // Fixed-size block, emit a size label...
         emit_label ( $2->value.ival, fillValue );
      }
      else
      {
         sprintf ( e, "%s: illegal string literal in expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      // Try to evaluate and fix the '$' label reference...
      evaluated = 1;
      evaluate_expression ( ir_tail, $2, &evaluated, FIX, NULL );

      if ( evaluated )
      {
         if ( $2->vtype == value_is_int )
         {
            // Variable-size block, do a .pad...
            emit_bin_space ( $2->value.ival, fillValue );
            emit_fix ( 0/* not used, anyway */ );
         }
         else
         {
            sprintf ( e, "%s: illegal string literal in expression", $1 );
            yyerror ( e );
         }
      }
   }
   if ( !evaluated )
   {
      sprintf ( e, "%s: cannot fully evaluate expression", $1 );
      yyerror ( e );
   }
}
// Directive .dsb <length>, <value> creates a space <length> bytes long filled
// with <value> at the current position within the intermediate representation.
           | FILLSPACEB expr ',' DIGITS TERM {
   unsigned char evaluated;

   reduce_expression ( $2, NULL );

   // emit pure placeholder label...
   emit_label ( 0, 0 );

   // Check if this is a fixed-size block or a
   // variable-sized block.  If the expression contains
   // the special '$' symbol it is variable-sized since the
   // intent of the programmer is most likely equivalent to
   // a .pad directive [it may not be but this is the best
   // we can do given that our instruction stream might
   // shift due to promotions later on.
   evaluated = 1;
   evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
   if ( evaluated )
   {
      if ( $2->vtype = value_is_int )
      {
         // Fixed-size block, emit a size label...
         emit_label ( $2->value.ival, $4->number );
      }
      else
      {
         sprintf ( e, "%s: illegal string literal in expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      // Try to evaluate and fix the '$' label reference...
      evaluated = 1;
      evaluate_expression ( ir_tail, $2, &evaluated, FIX, NULL );

      if ( evaluated )
      {
         if ( $2->vtype = value_is_int )
         {
            // Variable-size block, do a .pad...
            emit_bin_space ( $2->value.ival, $4->number );
            emit_fix ( 0/* not used, anyway */ );
         }
         else
         {
            sprintf ( e, "%s: illegal string literal in expression", $1 );
            yyerror ( e );
         }
      }
   }
   if ( !evaluated )
   {
      sprintf ( e, "%s: cannot fully evaluate expression", $1 );
      yyerror ( e );
   }
}
// Directive .dsw <value> creates a space <value> words long at the
// current position within the intermediate representation.
           | FILLSPACEW expr TERM {
   unsigned char evaluated;

   reduce_expression ( $2, NULL );

   // emit pure placeholder label...
   emit_label ( 0, 0 );

   // Check if this is a fixed-size block or a
   // variable-sized block.  If the expression contains
   // the special '$' symbol it is variable-sized since the
   // intent of the programmer is most likely equivalent to
   // a .pad directive [it may not be but this is the best
   // we can do given that our instruction stream might
   // shift due to promotions later on.
   evaluated = 1;
   evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
   if ( evaluated )
   {
      if ( $2->vtype == value_is_int )
      {
         // Fixed-size block, emit a size label...
         emit_label ( $2->value.ival<<1, fillValue );
      }
      else
      {
         sprintf ( e, "%s: illegal string literal in expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      // Try to evaluate and fix the '$' label reference...
      evaluated = 1;
      evaluate_expression ( ir_tail, $2, &evaluated, FIX, NULL );

      if ( evaluated )
      {
         if ( $2->vtype == value_is_int )
         {
            // Variable-size block, do a .pad...
            emit_bin_space ( $2->value.ival<<1, fillValue );
            emit_fix ( 0/* not used, anyway */ );
         }
         else
         {
            sprintf ( e, "%s: illegal string literal in expression", $1 );
            yyerror ( e );
         }
      }
   }
   if ( !evaluated )
   {
      sprintf ( e, "%s: cannot fully evaluate expression", $1 );
      yyerror ( e );
   }
}
// Directive .dsw <length>, <value> creates a space <length> words long filled
// with <value> at the current position within the intermediate representation.
           | FILLSPACEW expr ',' DIGITS TERM {
   unsigned char evaluated;

   reduce_expression ( $2, NULL );

   // emit pure placeholder label...
   emit_label ( 0, 0 );

   // Check if this is a fixed-size block or a
   // variable-sized block.  If the expression contains
   // the special '$' symbol it is variable-sized since the
   // intent of the programmer is most likely equivalent to
   // a .pad directive [it may not be but this is the best
   // we can do given that our instruction stream might
   // shift due to promotions later on.
   evaluated = 1;
   evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
   if ( evaluated )
   {
      if ( $2->vtype == value_is_int )
      {
         // Fixed-size block, emit a size label...
         emit_label ( $2->value.ival<<1, $4->number );
      }
      else
      {
         sprintf ( e, "%s: illegal string literal in expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      // Try to evaluate and fix the '$' label reference...
      evaluated = 1;
      evaluate_expression ( ir_tail, $2, &evaluated, FIX, NULL );

      if ( evaluated )
      {
         if ( $2->vtype == value_is_int )
         {
            // Variable-size block, do a .pad...
            emit_bin_space ( $2->value.ival<<1, $4->number );
            emit_fix ( 0/* not used, anyway */ );
         }
         else
         {
            sprintf ( e, "%s: illegal string literal in expression", $1 );
            yyerror ( e );
         }
      }
   }
   if ( !evaluated )
   {
      sprintf ( e, "%s: cannot fully evaluate expression", $1 );
      yyerror ( e );
   }
}
// Directive .align <alignment> aligns the address of the next emitted
// intermediate representation to <alignment>.  For example, .align 8 encountered
// when the current segment address is 0x804e will align the address to 0x8050.
// Bigger .align values can be used but are really probably better served
// using the .advance directive.
           | ALIGN DIGITS TERM {
   int j;
   if ( $2->number > 0 )
   {
      emit_bin_align ( $2->number );
   }
}
// Directive .advance <addr> fills the space from the current segment
// address to <addr> with the default fill value.  This has the same effect
// as .space directive but the addition of a fixed intermediate representation
// node makes the set-to address unchangeable by instruction promotions that
// occur prior in the intermediate representation stream.  Consider the somewhat
// meaningless example:
// .org $0
// DATA=$80
// lda DATA
// .advance 8
// ...
// In the above example, whether or not the lda DATA can be zeropage or
// absolute addressing mode is not known until the value of DATA can be
// completely evaluated.  Default behavior is to assume absolute and promote
// to zeropage if the expression evaluates to a zeropage value and the instruction
// has a zeropage addressing mode equivalent.  Thus the .advance directive above
// will emit a space of 5 bytes and a fix at address 8.  When DATA is completely
// evaluated the lda will be promoted to zeropage.  At that time all instructions
// up to the next fix point are shifted back a byte and the space emitted
// for the advance is bumped up a byte.
           | ADVANCE DIGITS TERM {
   int j;
   if ( cur->addr <= $2->number )
   {
      j = ($2->number-cur->addr);
      emit_bin_space ( j, fillValue );
      emit_fix ( $2->number );
   }
   else
   {
      sprintf ( e, "%s: illegal negative offset based on current address", $1 );
      yyerror ( e );
   }
}
// Directive .advance <addr>, <value> is equivalent to .advance <addr>
// except that it fills the space with <value> instead of the default
// fill value.
           | ADVANCE DIGITS ',' DIGITS TERM {
   int j;
   if ( cur->addr <= $2->number )
   {
      j = ($2->number-cur->addr);
      emit_bin_space ( j, $4->number );
      emit_fix ( $2->number );
   }
   else
   {
      sprintf ( e, "%s: illegal negative offset based on current address", $1 );
      yyerror ( e );
   }
}
// Directive .enum <addr> is equivalent to ASM6 ENUM directive.  I find this
// directive a bit hokey but am including it in an effort to be compatible
// with ASM6.  Note that I try to emulate it using my segment model.
         | ENUMERATE DIGITS TERM {
   // Emulate ASM6 by pretending we just saw a .data segment
   // directive followed by an .org directive...
   if ( $2->number >= 0 )
   {
      // Save segment we came from...
      btab_ent_prior_to_enum = cur->idx;

      set_binary_bank ( data_segment, ANONYMOUS_BANK );
      set_binary_addr ( $2->number );
      emit_fix ( $2->number );
   }
   else
   {
      sprintf ( e, "%s: illegal negative address", $1 );
      yyerror ( e );
   }
}
// Directive .ende is equivalent to ASM6 ENDE directive.  I find this
// directive a bit hokey but am including it in an effort to be compatible
// with ASM6.  Note that I try to emulate it using my segment model.
         | ENDENUMERATE TERM {
   // Emulate ASM6 by pretending we just saw a .text segment
   // directive...
   if ( btab_ent_prior_to_enum >= 0 )
   {
      set_binary_bank ( btab[btab_ent_prior_to_enum].type, btab[btab_ent_prior_to_enum].symbol );
      btab_ent_prior_to_enum = -1;
   }
   else
   {
      sprintf ( e, "%s: missing enum directive", $1 );
      yyerror ( e );
   }
}
         | MACRO LABELREF labellist {
   int macro;

   if ( $2->type == reference_symbol )
   {
      macro = start_macro ( $2->ref.symbol );
   }
   else
   {
   }

   dump_macro_table ();
}
         | ENDMACRO {
}
// Directive .rept <count> is equivalent to ASM6 REPT directive.  It
// repeats a section of intermediate representation nodes found between
// .rept and .endr directives.  Repeats can be nested up to MAX_REPEATS
// deep.  Currently this is fixed but certainly could be dynamic.  I just
// don't see much value in nesting repeated sections of stuff more than 10 deep.
         | REPEAT DIGITS TERM {
   ir_table* ptr;
   if ( $2->number > 1 )
   {
      if ( repeat_level < MAX_REPEATS )
      {
         // Create a dummy intermediate representation node
         // to cling to as the repeat begin point.
         ptr = emit_label ( 0, fillValue );

         // Store repeat begin point...
         ir_repeat_head [ repeat_level ] = ptr;
         ir_repeat_count [ repeat_level ] = $2->number;
         repeat_level++;
      }
      else
      {
         sprintf ( e, "%s: nesting exception, max is %d levels", $1, MAX_REPEATS );
         yyerror ( e );
      }
   }
   else
   {
      sprintf ( e, "%s: ignored, nothing to repeat", $1 );
      yyerror ( e );
   }
}
// Directive .endr is equivalent to ASM6 ENDR directive.  It
// causes the repeated emittance of instructions found between it
// and the .rept <count> directive that this .endr is tied to.
         | ENDREPEAT TERM {
   ir_table* ir_repeat_tail;

   // Create a dummy intermediate representation node
   // to cling to as the repeat end point.
   ir_repeat_tail = emit_label ( 0, fillValue );

   if ( repeat_level )
   {
      repeat_level--;
      while ( ir_repeat_count[repeat_level] > 1 )
      {
         // only repeat n-1 times because the code in the repeat
         // has been emitted once already by virtue of parsing
         // for the end marker.
         // Use pre-captured ir_tail here otherwise we'd be re-emitting
         // re-emitted stuff ad nauseum.
         reemit_ir ( ir_repeat_head[repeat_level], ir_repeat_tail );

         // decrement!
         ir_repeat_count[repeat_level]--;
      }
   }
   else
   {
      sprintf ( e, "%s: missing rept directive", $1 );
      yyerror ( e );
   }
}
// Directive .if <expression> emits the intermediate representation generated
// between it and its terminating .else, .elseif, or .endif directives if the condition
// evaluated from <expression> is true.
// Note: <expression> must fully evaluate.
         | IF expr TERM {
   unsigned char evaluated;
   if ( emitting[preproc_nest_level] )
   {
      preproc_nest_level++;
      emitting[preproc_nest_level] = 0;
      emitted[preproc_nest_level] = 0;
      if ( preproc_nest_level < MAX_IF_NEST )
      {
         reduce_expression ( $2, NULL );

         evaluated = 1;
         evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
         if ( evaluated )
         {
            if ( $2->vtype == value_is_int )
            {
               // Toggle intermediate representation emittance...
               emitting[preproc_nest_level] = !!$2->value.ival;
               emitted[preproc_nest_level] = !!$2->value.ival;
            }
            else
            {
               sprintf ( e, "%s: illegal string literal in expression", $1 );
               yyerror ( e );
            }
         }
         else
         {
            sprintf ( e, "%s: cannot fully evaluate expression", $1 );
            yyerror ( e );
         }
      }
      else
      {
         sprintf ( e, "%s: nesting exception, max is %d levels", $1, MAX_IF_NEST );
         yyerror ( e );
      }
   }
}
         | ELSEIF expr TERM {
   unsigned char evaluated;
   if ( !(emitted[preproc_nest_level]) )
   {
      emitting[preproc_nest_level] = 0;

      reduce_expression ( $2, NULL );

      evaluated = 1;
      evaluate_expression ( ir_tail, $2, &evaluated, 0, NULL );
      if ( evaluated )
      {
         if ( $2->vtype == value_is_int )
         {
            // Toggle intermediate representation emittance...
            emitting[preproc_nest_level] = !!$2->value.ival;
            emitted[preproc_nest_level] = !!$2->value.ival;
         }
         else
         {
            sprintf ( e, "%s: illegal string literal in expression", $1 );
            yyerror ( e );
         }
      }
      else
      {
         sprintf ( e, "%s: cannot fully evaluate expression", $1 );
         yyerror ( e );
      }
   }
   else
   {
      // Turn off intermediate representation emittance...
      emitting[preproc_nest_level] = 0;
   }
}
         | ELSE TERM {
   // If we haven't emitted yet, emit now...
   if ( !(emitted[preproc_nest_level]) )
   {
      // Turn on intermediate representation emittance...
      emitting[preproc_nest_level] = 1;
      emitted[preproc_nest_level] = 1;
   }
   else
   {
      // Turn off intermediate representation emittance...
      emitting[preproc_nest_level] = 0;
   }
}
         | ENDIF TERM {
   if ( preproc_nest_level > 0 )
   {
      preproc_nest_level--;
   }
   else
   {
      sprintf ( e, "%s: missing if, ifdef, or ifndef directive", $1 );
      yyerror ( e );
   }
}
         | IFDEF LABELREF TERM {
   if ( emitting[preproc_nest_level] )
   {
      preproc_nest_level++;
      emitting[preproc_nest_level] = 0;
      emitted[preproc_nest_level] = 0;
      if ( preproc_nest_level < MAX_IF_NEST )
      {
         // Declared symbols will be in the symbol table...
         if ( $2->type == reference_symtab )
         {
            // Toggle intermediate representation emittance...
            emitting[preproc_nest_level] = 1;
            emitted[preproc_nest_level] = 1;
         }
      }
      else
      {
         sprintf ( e, "%s: nesting exception, max is %d levels", $1, MAX_IF_NEST );
         yyerror ( e );
      }
   }
}
         | IFNDEF LABELREF TERM {
   if ( emitting[preproc_nest_level] )
   {
      preproc_nest_level++;
      emitting[preproc_nest_level] = 0;
      emitted[preproc_nest_level] = 0;
      if ( preproc_nest_level < MAX_IF_NEST )
      {
         // Undeclared symbols will NOT be in the symbol table...
         if ( $2->type == reference_symbol )
         {
            // Toggle intermediate representation emittance...
            emitting[preproc_nest_level] = 1;
            emitted[preproc_nest_level] = 1;
         }
      }
      else
      {
         sprintf ( e, "%s: nesting exception, max is %d levels", $1, MAX_IF_NEST );
         yyerror ( e );
      }
   }
}
;

// Parser for accumulator and implied addressing modes.
// There's no ambiguities here to note.  An instruction with
// no parameters can only be of one form or the other.
no_params: INSTR TERM {
	unsigned char f;
   if ( (f=valid_instr_amode($1,AM_IMPLIED)) != INVALID_INSTR )
	{
		emit_bin_implied ( &(m_6502opcode[f]) );
	}
	else if ( (f=valid_instr_amode($1,AM_ACCUMULATOR)) != INVALID_INSTR )
	{
		emit_bin_accumulator ( &(m_6502opcode[f]) );
	}
	else
	{
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
		yyerror ( e );
	}
}
;

// DISAMBIGUATION: These rules exist purely to catch single-character
// forward or backward autolabels coming from the lexer.  It is impossible
// for the lexer to determine whether a + or a - in the source stream is
// part of an expression or part (or, in this case whole) of an autolabel.
disambiguation_2: INSTR '+' TERM {
   unsigned char f;
   expr_type* expr;
   ref_type* ref;
   symbol_table* sym;
   char* label = malloc ( 2 );

   if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
   {
      // Create an expression that simply points to the relevant autolabel if it exists.
      expr = get_next_exprtype ();
      expr->type = expression_reference;
      ref = get_next_reftype ();

      // Forward reference symbol, it shouldn't exist yet...
      ref->type = reference_symbol;
      label[0] = '+';
      label[1] = 0;
      ref->ref.symbol = label;
      expr->node.ref = ref;

      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_relative ( &(m_6502opcode[f]), expr );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
               | INSTR '-' TERM {
   unsigned char f;
   expr_type* expr;
   ref_type* ref;
   symbol_table* sym;

   if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
   {
      // Create an expression that simply points to the relevant autolabel if it exists.
      expr = get_next_exprtype ();
      expr->type = expression_reference;
      ref = get_next_reftype ();

      ref->type = reference_symtab;
      sym = find_symbol ( "-", cur->idx );
      if ( sym )
      {
         ref->ref.stab_ent = sym->idx;
         expr->node.ref = ref;
      }
      else
      {
         sprintf ( e, "%s: reference to undeclared label: -", instr_mnemonic($1) );
         yyerror ( e );
      }

      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_relative ( &(m_6502opcode[f]), expr );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
;

// Parser for immediate addressing modes.
// There's no ambiguities here to note.  An instruction with
// '#' can only be of immediate form.
expression_param: INSTR '#' expr TERM {
	unsigned char f;

   // reduce expression...
   reduce_expression ( $3, NULL );

   if ( (f=valid_instr_amode($1,AM_IMMEDIATE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_immediate ( &(m_6502opcode[f]), $3 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
// Parser for absolute, zeropage, and relative addressing modes.
// Ambiguity creeps in here a bit.  First, we have the possibility that
// the instruction's expression may evaluate to an absolute (2-byte)
// value.  Second we have the opposing possibility that the instruction's
// expression evaluates to a zero-page (1-byte) value.  Thirdly, we
// have the possibility that the instruction is a branch and thus
// the expression should evaluate to an offset.
// For the first two cases we start by assuming the worst; the expression
// will evaluate to an absolute value, and emit an absolute addressing mode immediate
// representation if the instruction supports absolute addressing mode.
// If there's no absolute addressing mode then we emit a zero-page
// addressing mode [again, if it exists].  Finally we check for branches.
           | INSTR expr TERM {
   unsigned char f;

   // reduce expression...
   reduce_expression ( $2, NULL );

   if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_absolute ( &(m_6502opcode[f]), $2 );
   }
   else if ( (f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_zeropage ( &(m_6502opcode[f]), $2 );
   }
   else if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_relative ( &(m_6502opcode[f]), $2 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
;

// Parser for absolute indexed-by-x, and zeropage indexed-by-x addressing modes.
// Ambiguity creeps in here a bit.  First, we have the possibility that
// the instruction's expression may evaluate to an absolute (2-byte)
// value.  Second we have the opposing possibility that the instruction's
// expression evaluates to a zero-page (1-byte) value.
// For these two cases we start by assuming the worst; the expression
// will evaluate to an absolute value, and emit an absolute addressing mode immediate
// representation if the instruction supports absolute addressing mode.
// If there's no absolute addressing mode then we emit a zero-page
// addressing mode [again, if it exists].
index_reg_param: INSTR expr ',' 'x' TERM {
   unsigned char f;

   // reduce expression...
   reduce_expression ( $2, NULL );

   if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_abs_idx_x ( &(m_6502opcode[f]), $2 );
   }
   else if ( (f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_zp_idx_x ( &(m_6502opcode[f]), $2 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
// Parser for absolute indexed-by-y, and zeropage indexed-by-y addressing modes.
// Ambiguity creeps in here a bit.  First, we have the possibility that
// the instruction's expression may evaluate to an absolute (2-byte)
// value.  Second we have the opposing possibility that the instruction's
// expression evaluates to a zero-page (1-byte) value.
// For these two cases we start by assuming the worst; the expression
// will evaluate to an absolute value, and emit an absolute addressing mode immediate
// representation if the instruction supports absolute addressing mode.
// If there's no absolute addressing mode then we emit a zero-page
// addressing mode [again, if it exists].
           | INSTR expr ',' 'y' TERM {
	unsigned char f;

   // reduce expression...
   reduce_expression ( $2, NULL );

   if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_abs_idx_y ( &(m_6502opcode[f]), $2 );
   }
   else if ( (f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_zp_idx_y ( &(m_6502opcode[f]), $2 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
;

// Parser for indirect addressing modes.
// Ambiguity creeps in here a LOT.  First, we have the
// ambiguous ()'s.  Since we are evaluating a rule where
// the parser has decided the parenthesis are not part
// of the expression [expression rules also reduce ()'s]
// we might assume the instruction can only be an
// indirect addressing mode variant.  This turns out not
// to be the case given the precedence rules set up for ()'s.
// In reality any instruction of the form:
// opcode (value)
// will reduce to this rule.  This includes instructions
// that have absolute, zero-page and even relative addressing
// modes.  Because I don't want the parser to be too strict
// I want to allow parenthesis.  Thus to disambiguate the
// situation, we need to check for implied [there's what...
// one JMP?] first, and then also check for absolute, zero-page,
// and relative addressing modes.  The checking for the
// absolute, and zero-page addressing modes is done in the
// same way as is described for other instruction rules.
// Depending on the programmer's style this could be a busy rule.
indirect_param: INSTR '(' expr ')' TERM {
   unsigned char f;

   // reduce expression...
   reduce_expression ( $3, NULL );

   // special handling here...the ()'s could have been ripped off of a
   // parenthesized expression, so we need to check to see if this is really
   // an absolute or zero-page instruction format in disguise (due to the
   // stolen parenthesis...

   if ( (f=valid_instr_amode($1,AM_INDIRECT)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_indirect ( &(m_6502opcode[f]), $3 );
   }
   else if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_absolute ( &(m_6502opcode[f]), $3 );
   }
   else if ( (f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_zeropage ( &(m_6502opcode[f]), $3 );
   }
   else if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_relative ( &(m_6502opcode[f]), $3 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
;

// Parser for pre-indexed indirect addressing mode.
// There's no ambiguities here to note.  An instruction with
// this form can only be preindexed indirect addressing.
preindexed_indirect_param: INSTR '(' expr ',' 'x' ')' TERM {
	unsigned char f;

   // reduce expression...
   reduce_expression ( $3, NULL );

   if ( (f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_pre_idx_ind ( &(m_6502opcode[f]), $3 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
;

// Parser for post-indexed indirect addressing mode.
// There's no ambiguities here to note.  An instruction with
// this form can only be post-indexed indirect addressing.
postindexed_indirect_param: INSTR '(' expr ')' ',' 'y' TERM {
   unsigned char f;

   // reduce expression...
   reduce_expression ( $3, NULL );

   if ( (f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_post_idx_ind ( &(m_6502opcode[f]), $3 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}

// DISAMBIGUATION: This rule exists purely to catch cases of extreme
// parenthesizing by the programmer.  It is possible that the programmer
// could use ()'s to delineate an expression in what he *intends* to be
// an absolute or zero-page addressing mode instruction.  Without this
// disambiguation rule, the parser dies on the 'x' because it is trying
// to parse the instruction using the post-indexed indirect parameter rule
// above.  Once it gets to the 'y' of the rule and sees an 'x' it has a
// rule break and quits.  It was, after all, expecting a 'y' but didn't
// get a 'y' so it doesn't know what to do.  With the addition of this
// 'fake' rule the ambiguous situation becomes clear to the parser and
// it can go ahead and generate the appropriate absolute or zero-page
// addressing mode intermediate representation.
disambiguation_1: INSTR '(' expr ')' ',' 'x' TERM {
   unsigned char f;

   // note:  this rule exists purely to disambiguate the situations where
   // a parenthesized expression is followed by a ,x which is NOT meant to be
   // interpreted as a pre-indexed indirect addressing mode, but rather either
   // an absolute indexed or zeropage indexed addressing mode.

   // reduce expression...
   reduce_expression ( $3, NULL );

   if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_abs_idx_x ( &(m_6502opcode[f]), $3 );
   }
   else if ( (f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR )
   {
      // emit instruction with reference to expression for reduction when all symbols are known...
      emit_bin_zp_idx_x ( &(m_6502opcode[f]), $3 );
   }
   else
   {
      sprintf ( e, "%s: invalid addressing mode", instr_mnemonic($1) );
      yyerror ( e );
   }
}
         ;

labellist: /* empty */ {
}
         | labellist ',' LABELREF {
}
         | LABELREF {
}

blist: blist ',' expr {
   unsigned char evaluated;

   // reduce expression...
   reduce_expression ( $3, NULL );

   // Attempt to evaluate expression to see what type we'll get...
   evaluate_expression ( ir_tail, $3, &evaluated, FIX, NULL );

   // We can't care about whether or not the expression completely
   // evaluates here due to possible forward symbol references.  All
   // we're interested in is the *type* of expression which should
   // percolate up to the root node by evaluation...

   // Emit different stuff depending on whether expression
   // is string literal-based or numeric...
   if ( $3->vtype == value_is_int )
   {
      emit_bin ( $3 );
   }
   else
   {
      // String and length should have percolated to the
      // root node, just pass the expression...
      emit_bin_string ( $3 );
   }
}
     | expr {
   unsigned char evaluated;

   // reduce expression...
   reduce_expression ( $1, NULL );

   // Attempt to evaluate expression to see what type we'll get...
   evaluate_expression ( ir_tail, $1, &evaluated, FIX, NULL );

   // We can't care about whether or not the expression completely
   // evaluates here due to possible forward symbol references.  All
   // we're interested in is the *type* of expression which should
   // percolate up to the root node by evaluation...

   // Emit different stuff depending on whether expression
   // is string literal-based or numeric...
   if ( $1->vtype == value_is_int )
   {
      emit_bin ( $1 );
   }
   else
   {
      // String and length should have percolated to the
      // root node, just pass the expression...
      emit_bin_string ( $1 );
   }
}
    ;

wlist: wlist ',' expr {
   // reduce expression...
   reduce_expression ( $3, NULL );

   emit_bin2 ( $3 );
}
     | expr {
   // reduce expression...
   reduce_expression ( $1, NULL );

   emit_bin2 ( $1 );
}
    ;

hlist: hlist QUOTEDSTRING {
   int c;
   int l = $2->length;
   char d;
   char e;

   if ( l&1 ) l++; // catch and use the null to our advantage...
   for ( c = 0; c < l; c++ )
   {
      d = toupper($2->string[c]);
      e <<= 4;
      if ( (d >= '0') &&
           (d <= '9') )
      {
         e |= (d-'0');
      }
      else if ( (d >= 'A') &&
                (d <= 'F') )
      {
         e |= ((d-'A')+10);
      }
      if ( c&1 )
      {
         emit_bin_data ( e );
      }
   }
}
     | QUOTEDSTRING {
   int c;
   int l = $1->length;
   char d;
   char e;

   if ( l&1 ) l++; // catch and use the null to our advantage...
   for ( c = 0; c < l; c++ )
   {
      d = toupper($1->string[c]);
      e <<= 4;
      if ( (d >= '0') &&
           (d <= '9') )
      {
         e |= (d-'0');
      }
      else if ( (d >= 'A') &&
                (d <= 'F') )
      {
         e |= ((d-'A')+10);
      }
      if ( c&1 )
      {
         emit_bin_data ( e );
      }
   }
}
   ;
expr : QUOTEDSTRING {
   $$ = get_next_exprtype ();
   $$->type = expression_reference;
   $$->node.ref = get_next_reftype ();
   $$->node.ref->type = reference_const_string;
   $$->node.ref->ref.text = $1;
}
     | DIGITS {
   $$ = get_next_exprtype ();
   $$->type = expression_number;
   $$->node.num = $1;
}
     | LABELREF {
   // copy a declared global's expression directly into this
   // expression tree, to reduce expression parsing later on...
   // otherwise store a reference to the as-yet-unknown symbol
   // for later evaluation.
   if ( ($1->type == reference_symtab) &&
        (stab[$1->ref.stab_ent].expr) )
   {
      $$ = copy_expression ( stab[$1->ref.stab_ent].expr );
   }
   else
   {
      $$ = get_next_exprtype ();
      $$->type = expression_reference;
      $$->node.ref = $1;
   }
}
     | '(' expr ')' {
   // swallow useless parenthesis...
   $$ = $2;
}
     | '>' expr %prec UBEOF {
   // "big-end-of" operator...
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '>';
   $$->right = $2;
   $$->right->parent = $$;
}
     | '<' expr %prec ULEOF {
   // "little-end-of" operator...
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '<';
   $$->right = $2;
   $$->right->parent = $$;
}
     | '!' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '!';
   $$->right = $2;
   $$->right->parent = $$;
}
     | '-' expr %prec UMINUS {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '-';
   $$->right = $2;
   $$->right->parent = $$;
}
     | '+' expr %prec UPLUS {
   // swallow useless unary-plus...
   $$ = $2;
}
     | '~' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '~';
   $$->right = $2;
   $$->right->parent = $$;
}
     | expr '+' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '+';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '-' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '-';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '*' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '*';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '/' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '/';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '%' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '%';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '<' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '<';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '>' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '>';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr EQEQ expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = 'e'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr NOTEQ expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = 'n'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr LTEQ expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = 'l'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr GTEQ expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = 'g'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr LSHIFT expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = ','; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr RSHIFT expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '.'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr LOGAND expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '7'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr LOGOR expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '\\'; // i know...i know...
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '&' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '&';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '^' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '^';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}
     | expr '|' expr {
   $$ = get_next_exprtype ();
   $$->type = expression_operator;
   $$->node.op = '|';
   $$->left = $1;
   $$->right = $3;
   $$->left->parent = $$;
   $$->right->parent = $$;
}   ;
%%

int yyerror(char* s)
{
   char er [ 256 ];
   if ( yytext[0] != 0 )
   {
      sprintf ( er, "%s at '%s'", s, yytext );
   }
   else
   {
      sprintf ( er, "%s", s );
   }
   add_error ( er );
}

void initialize ( void )
{
	ir_table* ptr;
	ir_table* ptd = NULL;
	int idx;

   yylineno = 1;

	free ( errorStorage );
	errorStorage = NULL;
	errorCount = 0;

	for ( ptr = ir_head; ptr != NULL; ptr = ptr->next )
	{
      if ( ptd->expr != NULL ) destroy_expression ( ptd->expr );
		if ( ptd != NULL ) free ( ptd );
		ptd = ptr;
	}
	free ( ptd );
	ir_head = NULL;
	ir_tail = NULL;

	for ( idx = 0; idx < stab_ent; idx++ )
	{
      if ( stab[idx].expr != NULL )
      {
         destroy_expression ( stab[idx].expr );
      }
		free ( stab[idx].symbol );
	}
	free ( stab );
	stab = NULL;
	stab_ent = 0;
	stab_max = 0;

	for ( idx = 0; idx < btab_ent; idx++ )
	{
		free ( btab[idx].symbol );
	}
	free ( btab );
	btab = NULL;
	btab_ent = 0;
	btab_max = 0;
}

void add_error(char *s)
{
   static char error_buffer [ 2048 ] = { 0, };
   static char* ptr;

	errorCount++;
	if ( errorStorage == NULL )
	{
      if ( current_label == -1 )
		{
//			if ( strlen(currentFile) )
//			{
//				sprintf ( error_buffer, "error: %d: in included file %s: ", yylineno, currentFile );
//			}
//			else
//			{
				sprintf ( error_buffer, "error: %d: ", yylineno );
//			}
			errorStorage = (char*) malloc ( strlen(error_buffer)+1+strlen(s)+3 );
         ptr = errorStorage;
			strcpy ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
		}
		else
		{
//			if ( strlen(currentFile) )
//			{
//				sprintf ( error_buffer, "error: %d: in included file %s: ", yylineno, currentFile );
//			}
//			else
//			{
            sprintf ( error_buffer, "error: %d: after %s: ", yylineno, stab[current_label].symbol );
//			}
			errorStorage = (char*) malloc ( strlen(error_buffer)+1+strlen(s)+3 );
         ptr = errorStorage;
         strcpy ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
      }
	}
	else
	{
      if ( current_label == -1 )
		{
			sprintf ( error_buffer, "error: %d: ", yylineno );
			errorStorage = (char*) realloc ( errorStorage, strlen(errorStorage)+1+strlen(error_buffer)+1+strlen(s)+3 );
         ptr = errorStorage+strlen(errorStorage);
         strcat ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
      }
		else
		{
         sprintf ( error_buffer, "error: %d: after %s: ", yylineno, stab[current_label].symbol );
			errorStorage = (char*) realloc ( errorStorage, strlen(errorStorage)+1+strlen(error_buffer)+1+strlen(s)+3 );
         ptr = errorStorage+strlen(errorStorage);
         strcat ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
      }
	}
   fprintf ( stderr, ptr );
}

int yywrap(void)
{
  return -1;
}

unsigned char valid_instr ( char* instr )
{
   unsigned char a = INVALID_INSTR;
   char     upinstr [ 4 ];
   int i;

   for ( i = 0; i < 3; i++ )
   {
      upinstr[i] = toupper(instr[i]);
   }
   upinstr[3] = 0;

   if ( strlen(instr) == 3 )
   {
      for ( i = 0; i < 256; i++ )
      {
         if ( (strlen(m_6502opcode[i].name) > 0) && (strcmp(m_6502opcode[i].name,upinstr) == 0) )
         {
            a = i;
            return a;
         }
      }
   }

   return a;
}

int promote_instructions ( unsigned char flag )
{
	ir_table* ptr;
   expr_type* expr;
   ir_table* walk_ptr;
   unsigned int d;
   unsigned int dc;
   unsigned char f;
   unsigned int loc;
   int di;
   int j;
   int value;
   unsigned char evaluated = 1;
   unsigned char value_zp_ok;
   int promotions = 0;

   for ( ptr = ir_head; ptr != NULL; ptr = ptr->next )
   {
      expr = ptr->expr;

      if ( expr )
      {
         // try a symbol reduction to see if we can promote this to zeropage...
         evaluated = 1;
         evaluate_expression ( ptr, expr, &evaluated, flag, NULL );
         value = expr->value.ival;
         value_zp_ok = 0;
         if ( (value >= -128) &&
              (value < 256) )
         {
            value_zp_ok = 1;
         }
      }

      switch ( ptr->fixup )
      {
         case fixup_string:
            if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               // Emitting of the actual string is done at final binary generation...
               // done!
               ptr->fixup = fixup_fixed;
            }
         break;

         case fixup_datab:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[0] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
         break;

         case fixup_dataw:
            if ( (evaluated) && (expr->vtype == value_is_int) )
            {
               ptr->data[1] = (value>>8)&0xFF;
               ptr->data[0] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
         break;

         case fixup_abs_idx_x:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               // promote to zeropage if possible...
               if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR )
               {
                  // promote to zeropage fixup so we keep track of it!
                  ptr->fixup = fixup_zp_idx;

                  // indicate we're probably not done fixing yet...
                  promotions++;

                  // fix this instruction...
                  ptr->data[0] = f&0xFF;
                  ptr->data[1] = value&0xFF;
                  ptr->len = 2;

                  // adjust addresses of downstream stuff up to the first fixed wall...
                  walk_ptr = ptr;
                  for ( walk_ptr = walk_ptr->next; (walk_ptr != NULL) && (walk_ptr->fixed == 0); walk_ptr = walk_ptr->next )
                  {
                     walk_ptr->addr--;
                     if ( walk_ptr->multi == 1 ) walk_ptr->len += 1;
                  }

                  // adjust current bank address if necessary...
                  if ( ptr->btab_ent == cur->idx )
                  {
                     cur->addr--;
                  }
               }
               else
               {
                  ptr->data[1] = value&0xFF;
                  ptr->data[2] = (value>>8)&0xFF;
                  if ( flag == FIX )
                  {
                     // done!
                     ptr->fixup = fixup_fixed;
                  }
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_int) && (!value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               ptr->data[2] = (value>>8)&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
            else
            {
               // indicate we're probably not done fixing yet...
               promotions++;
            }
         break;

         case fixup_abs_idx_y:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               // promote to zeropage if possible...
               if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR )
               {
                  // promote to zeropage fixup so we keep track of it!
                  ptr->fixup = fixup_zp_idx;

                  // indicate we're probably not done fixing yet...
                  promotions++;

                  // fix this instruction...
                  ptr->data[0] = f&0xFF;
                  ptr->data[1] = value&0xFF;
                  ptr->len = 2;

                  // adjust addresses of downstream stuff up to the first fixed wall...
                  walk_ptr = ptr;
                  for ( walk_ptr = walk_ptr->next; (walk_ptr != NULL) && (walk_ptr->fixed == 0); walk_ptr = walk_ptr->next )
                  {
                     walk_ptr->addr--;
                     if ( walk_ptr->multi == 1 ) walk_ptr->len += 1;
                  }

                  // adjust current bank address if necessary...
                  if ( ptr->btab_ent == cur->idx )
                  {
                     cur->addr--;
                  }
               }
               else
               {
                  ptr->data[1] = value&0xFF;
                  ptr->data[2] = (value>>8)&0xFF;
                  if ( flag == FIX )
                  {
                     // done!
                     ptr->fixup = fixup_fixed;
                  }
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_int) && (!value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               ptr->data[2] = (value>>8)&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
            else
            {
               // indicate we're probably not done fixing yet...
               promotions++;
            }
         break;

         case fixup_absolute:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               // promote to zeropage if possible...
               if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ZEROPAGE)) != INVALID_INSTR )
               {
                  // promote to zeropage fixup so we keep track of it!
                  ptr->fixup = fixup_zeropage;

                  // indicate we're probably not done fixing yet...
                  promotions++;

                  // fix this instruction...
                  ptr->data[0] = f&0xFF;
                  ptr->data[1] = value&0xFF;
                  ptr->len = 2;

                  // adjust addresses of downstream stuff up to the first fixed wall...
                  walk_ptr = ptr;
                  for ( walk_ptr = walk_ptr->next; (walk_ptr != NULL) && (walk_ptr->fixed == 0); walk_ptr = walk_ptr->next )
                  {
                     walk_ptr->addr--;
                     if ( walk_ptr->multi == 1 ) walk_ptr->len += 1;
                  }

                  // adjust current bank address if necessary...
                  if ( ptr->btab_ent == cur->idx )
                  {
                     cur->addr--;
                  }
               }
               else
               {
                  ptr->data[1] = value&0xFF;
                  ptr->data[2] = (value>>8)&0xFF;
                  if ( flag == FIX )
                  {
                     // done!
                     ptr->fixup = fixup_fixed;
                  }
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_int) && (!value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               ptr->data[2] = (value>>8)&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
            else
            {
               // indicate we're probably not done fixing yet...
               promotions++;
            }
         break;

         case fixup_zeropage:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_zp_idx:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_pre_idx_ind:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_post_idx_ind:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_int) )
            {
               // disambiguation: if we've decided this instruction is a post-indexed indirect
               // addressing mode but the expression evaluates outside of zeropage, check to see if
               // the instruction is absolute indexed by y addressing mode capable and if so WARN
               // and demote...
               unsigned char f;
               if ( (f=valid_instr_amode(ptr->data[0],AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
               {
                  ptr->fixup = fixup_abs_idx_y;

                  // indicate we're probably not done fixing yet...
                  promotions++;

                  // fix this instruction...
                  ptr->data[0] = f&0xFF;
                  ptr->len = 3; // DEMOTION

                  // adjust addresses of downstream stuff up to the first fixed wall...
                  walk_ptr = ptr;
                  for ( walk_ptr = walk_ptr->next; (walk_ptr != NULL) && (walk_ptr->fixed == 0); walk_ptr = walk_ptr->next )
                  {
                     walk_ptr->addr++; // DEMOTION
                     if ( walk_ptr->multi == 1 ) walk_ptr->len -= 1; // DEMOTION
                  }

                  // adjust current bank address if necessary...
                  if ( ptr->btab_ent == cur->idx )
                  {
                     cur->addr++; // DEMOTION
                  }

                  sprintf ( e, "demotion of assumed post-indexed indirect instruction to absolute indexed instruction" );
                  yyerror ( e );
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_indirect:
            if ( (evaluated) && (expr->vtype == value_is_int) )
            {
               ptr->data[1] = value&0xFF;
               ptr->data[2] = (value>>8)&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_relative:
            if ( (flag == FIX) && (evaluated) && (expr->vtype == value_is_int) )
            {
               if ( (value > 255) ||
                    (value < -128) )
               {
                  // must be a branch to a label (calculate distance)...
                  di = -((ptr->addr+ptr->len) - value);
               }
               else
               {
                  // must be a direct branch to a numeric offset, directly use distance...
                  di = value;
               }
               if ( (di >= -128) && (di <= 127) )
               {
                  ptr->data[1] = di&0xFF;
                  if ( flag == FIX )
                  {
                     // done!
                     ptr->fixup = fixup_fixed;
                  }
               }
               else
               {
                  sprintf ( e, "branch to address out of range" );
                  yyerror ( e );
               }
               // done!
            }
            else if ( (flag == FIX) && (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;

         case fixup_immediate:
            if ( (evaluated) && (expr->vtype == value_is_int) && (value_zp_ok) )
            {
               ptr->data[1] = value&0xFF;
               if ( flag == FIX )
               {
                  // done!
                  ptr->fixup = fixup_fixed;
               }
            }
            else if ( (evaluated) && (expr->vtype == value_is_string) )
            {
               sprintf ( e, "illegal string constant: %s", expr->value.sval->string );
               yyerror ( e );
            }
         break;
      }
   }

   return promotions;
}

unsigned int distance ( ir_table* from, ir_table* to )
{
	ir_table* ptr;

	unsigned int d = 0;
	for ( ptr = from; ptr != to->next; ptr = ptr->next )
	{
		d += ptr->len;
	}
	return d;
}

void check_fixup ( void )
{
   ir_table* ptr;
   expr_type* expr;
   unsigned char evaluated = 1;
   unsigned char value_zp_ok;
   char* symbol;

   for ( ptr = ir_head; ptr != NULL; ptr = ptr->next )
   {
      expr = ptr->expr;

      if ( expr )
      {
         // check expression evaluates...
         symbol = NULL;
         evaluated = 1;
         evaluate_expression ( ptr, expr, &evaluated, FIX, &symbol );

         // if not, emit an error...
         if ( (!evaluated) && symbol )
         {
            sprintf ( e, "reference to undefined or unreachable symbol: %s", symbol );
            yyerror ( e );
         }
         else if ( !evaluated )
         {
            sprintf ( e, "unable to determine value of expression" );
            yyerror ( e );
         }
      }
   }
}

int start_macro ( char* symbol )
{
   unsigned int i;
   int idx = -1;

   if ( mtab == NULL )
   {
      mtab = (macro_table*)calloc ( MTAB_ENT_INC, sizeof(macro_table) );
      if ( mtab != NULL )
      {
         mtab_max += MTAB_ENT_INC;
         mtab[mtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( mtab[mtab_ent].symbol != NULL )
         {
            memset ( mtab[mtab_ent].symbol, 0, strlen(symbol)+1 );
            strncpy ( mtab[mtab_ent].symbol, symbol, strlen(symbol) );
            mtab[mtab_ent].idx = mtab_ent;
         }
      }
      else
      {
         sprintf ( e, "unable to allocate memory for tables" );
         yyerror ( e );
      }
   }
   else
   {
      if ( mtab_ent < mtab_max )
      {
         mtab[mtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( mtab[mtab_ent].symbol != NULL )
         {
            memset ( mtab[mtab_ent].symbol, 0, strlen(symbol)+1 );
            strncpy ( mtab[mtab_ent].symbol, symbol, strlen(symbol) );
            mtab[mtab_ent].idx = mtab_ent;
         }
      }
      else
      {
         mtab_max += MTAB_ENT_INC;
         mtab = (macro_table*)realloc ( mtab, mtab_max*sizeof(macro_table) );
         if ( mtab != NULL )
         {
            mtab[mtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
            if ( mtab[mtab_ent].symbol != NULL )
            {
               memset ( mtab[mtab_ent].symbol, 0, strlen(symbol)+1 );
               strncpy ( mtab[mtab_ent].symbol, symbol, strlen(symbol) );
               mtab[mtab_ent].idx = mtab_ent;
            }
         }
         else
         {
            sprintf ( e, "unable to allocate memory for tables" );
            yyerror ( e );
         }
      }
   }

   idx = mtab_ent;

   mtab_ent++;

   return idx;
}

void finish_macro ( int macro )
{
}

unsigned char add_binary_bank ( segment_type type, char* symbol )
{
	unsigned char a = 1;
	unsigned int i;

	if ( btab == NULL )
	{
		btab = (binary_table*)calloc ( BTAB_ENT_INC, sizeof(binary_table) );
		if ( btab != NULL )
		{
			btab_max += BTAB_ENT_INC;
			btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( btab[btab_ent].symbol != NULL )
			{
				memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
				btab[btab_ent].idx = btab_ent;
				btab[btab_ent].type = type;
				btab[btab_ent].addr = 0;
			}
		}
		else
		{
			sprintf ( e, "unable to allocate memory for tables" );
			yyerror ( e );
		}
	}
	else
	{
		if ( btab_ent < btab_max )
		{
			btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( btab[btab_ent].symbol != NULL )
			{
				memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
				btab[btab_ent].idx = btab_ent;
				btab[btab_ent].type = type;
				btab[btab_ent].addr = 0;
			}
		}
		else
		{
			btab_max += BTAB_ENT_INC;
			btab = (binary_table*)realloc ( btab, btab_max*sizeof(binary_table) );
			if ( btab != NULL )
			{
				btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
				if ( btab[btab_ent].symbol != NULL )
				{
					memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
					strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
					btab[btab_ent].idx = btab_ent;
					btab[btab_ent].type = type;
					btab[btab_ent].addr = 0;
				}
			}
			else
			{
				sprintf ( e, "unable to allocate memory for tables" );
				yyerror ( e );
			}
		}
	}

	cur = &(btab[btab_ent]);
	btab_ent++;

	//dump_binary_table ();

	return a;
}

unsigned char set_binary_bank ( segment_type type, char* bankname )
{
	unsigned char a = 0;
	unsigned int i;
	for ( i = 0; i < btab_ent; i++ )
	{
		if ( (type == btab[i].type) &&
		     (strcmp(bankname,btab[i].symbol) == 0) )
		{
			a = 1;
			cur = &(btab[i]);
			return a;
		}
	}
	return a;
}

void output_binary ( char** buffer, int* size )
{
	int pos = 0;
	ir_table* ptr1;
	ir_table* ptr2;
	ir_table* ptr3;
	ir_table* ptr4 = NULL;
	ir_table* ptrl;
   unsigned int addr = 0;
	int       i;
	int       bank;
	int       lowest_bank_addr = -1;
	int       lowest_bank;

   (*buffer) = (char*) malloc ( DEFAULT_BANK_SIZE );
   (*size) = DEFAULT_BANK_SIZE;
	if ( (*buffer) != NULL )
	{
		for ( bank = 0; bank < btab_ent; bank++ )
		{
			ptr4 = NULL;
			for ( ptr1 = ir_head; ptr1 != NULL; ptr1 = ptr1->next )
			{
            lowest_bank = ptr1->btab_ent;
            lowest_bank_addr = ptr1->addr;
				if ( (ptr1->btab_ent == lowest_bank) && (btab[ptr1->btab_ent].type == text_segment) )
				{
// CPTODO: don't need to reorder IR!!
               ptr3 = ptr1;
#if 0
               addr = 0xFFFFFFFF;
				
					// find lowest addr
					ptr3 = NULL;
					for ( ptr2 = ir_head; ptr2 != NULL; ptr2 = ptr2->next )
					{
						if ( (ptr2->btab_ent == lowest_bank) && (btab[ptr2->btab_ent].type == text_segment) )
						{
							if ( (ptr2->addr < addr) && (ptr2->emitted == 0) )
							{
								ptr3 = ptr2;
								addr = ptr3->addr;
							}
						}
						else if ( (ptr2->btab_ent == lowest_bank) && (btab[ptr2->btab_ent].type == data_segment) )
						{
							ptr2->emitted = 1;
						}
					}
#endif
               if ( (ptr3 != NULL) && (ptr3->emitted == 0) )
					{
						ptr3->emitted = 1;
						ptr4 = ptr3;
                  if ( (ptr3->multi == 0) && (ptr3->label == 0) && (ptr3->string == 0) )
						{
							for ( i = 0; i < ptr3->len; i++ )
							{
								(*buffer)[pos++] = (ptr3->data[i])&0xFF;
								if ( pos == (*size) )
								{
                           (*size) += DEFAULT_BANK_SIZE;
									(*buffer) = (char*) realloc ( (*buffer), (*size) );
								}
							}
						}
                  else if ( ptr3->string == 1 )
                  {
                     for ( i = 0; i < ptr3->len; i++ )
                     {
                        (*buffer)[pos++] = (ptr3->expr->value.sval->string[i])&0xFF;
                        if ( pos == (*size) )
                        {
                           (*size) += DEFAULT_BANK_SIZE;
                           (*buffer) = (char*) realloc ( (*buffer), (*size) );
                        }
                     }
                  }
                  else
						{
							for ( i = 0; i < ptr3->len; i++ )
							{
								(*buffer)[pos++] = (ptr3->data[0])&0xFF;
								if ( pos == (*size) )
								{
                           (*size) += DEFAULT_BANK_SIZE;
									(*buffer) = (char*) realloc ( (*buffer), (*size) );
								}
							}
						}
					}
				}
			}
		}
	}
	(*size) = pos;

   dump_ir_table ();
}

char* instr_mnemonic ( unsigned char op )
{
   return m_6502opcode[op].name;
}

unsigned char valid_instr_amode ( int instr, int amode )
{
	unsigned char a = INVALID_INSTR;
	int i;

	for ( i = 0; i < 256; i++ )
	{
      if ( ((allowUndocumented) ||
           (m_6502opcode[i].documented)) &&
           (strlen(m_6502opcode[i].name) > 0) &&
			  (strncmp(m_6502opcode[i].name,m_6502opcode[instr].name,3) == 0) &&
			  (m_6502opcode[i].amode == amode) )
		{
			a = i;
			return a;
		}
	}
	return a;
}

void update_symbol_ir ( symbol_table* stab, ir_table* ir )
{
   stab->ir = ir;
}

void dump_ir_expressions ( void )
{
   ir_table* ptr = ir_head;
   int       i;

   for ( ; ptr != NULL; ptr = ptr->next )
   {
      if ( ptr->expr )
      {
         printf ( "%08x %08x %04X: \n", ptr, ptr->expr, ptr->addr );
         dump_expression ( ptr->expr );
      }
   }
}

void dump_ir_table ( void )
{
	ir_table* ptr = ir_head;
	int       i;

	for ( ; ptr != NULL; ptr = ptr->next )
	{
      printf ( "%08x %d %04X: ", ptr, ptr->btab_ent, ptr->addr );

      if ( (ptr->multi == 0) && (ptr->label == 0) && (ptr->string == 0) )
		{
         // only dump out three bytes!
         for ( i = 0; i < ptr->len&3; i++ )
			{
				printf ( "%02X ", ptr->data[i] );
			}
			if ( ptr->fixed == 1 )
			{
				printf ( "(fixed)" );
			}
			printf ( "\n" );
		}
      else if ( ptr->string == 1 )
      {
         printf ( "'%s' (string, length %d)\n", ptr->expr->value.sval->string, ptr->expr->value.sval->length );
      }
		else
		{
         printf ( "%05X (%d) %02X\n", ptr->len, ptr->len, ptr->data[0] );
		}
	}
}

void dump_symbol_table ( void )
{
	int i;
	for ( i = 0; i < stab_ent; i++ )
	{
      // globals have expressions
      if ( stab[i].expr )
      {
         printf ( "%d: %s global: expression:\n", i, stab[i].symbol );
         dump_expression ( stab[i].expr );
      }
      else
      {
         if ( stab[i].ir )
         {
            printf ( "%d: %s value %04X (%08x)\n", i, stab[i].symbol, stab[i].ir->addr, stab[i].ir );
         }
         else
         {
            printf ( "%d: %s value UNKNOWN (%08x)\n", i, stab[i].symbol, stab[i].ir );
         }
      }
	}
}

void dump_macro_table ( void )
{
   int i;
   for ( i = 0; i < mtab_ent; i++ )
   {
      printf ( "%d: %s head-%08x tail-%08x\n", i, mtab[i].symbol, mtab[i].ir_head, mtab[i].ir_tail );
   }
}

void dump_binary_table ( void )
{
	int i;
	for ( i = 0; i < btab_ent; i++ )
	{
		printf ( "%c%d: %s %s addr-%04X\n", cur==&(btab[i])?'*':' ', i, btab[i].type==text_segment?"TEXT":"DATA", btab[i].symbol, btab[i].addr );
	}
}

void set_binary_addr ( unsigned int addr )
{
	cur->addr = addr;
	//dump_binary_table ();
}

void dump_expression ( expr_type* expr )
{
   if ( expr->left )
   {
      dump_expression ( expr->left );
   }
   if ( expr->right )
   {
      dump_expression ( expr->right );
   }
   if ( expr->type == expression_operator )
   {
      printf ( "m%08x l%08x r%08x: operator '%c'\n", expr, expr->left, expr->right, expr->node.op );
   }
   else if ( expr->type == expression_number )
   {
      printf ( "m%08x l%08x r%08x: number: %d zp: %d\n", expr, expr->left, expr->right, expr->node.num->number, expr->node.num->zp_ok );
   }
   else if ( expr->type == expression_reference )
   {
      if ( expr->node.ref->type == reference_symbol )
      {
         printf ( "m%08x l%08x r%08x: symbol: %s\n", expr, expr->left, expr->right, expr->node.ref->ref.symbol );
      }
      else if ( expr->node.ref->type == reference_symtab )
      {
         printf ( "m%08x l%08x r%08x: symtab: %s\n", expr, expr->left, expr->right, stab[expr->node.ref->ref.stab_ent].symbol );
      }
      else if ( expr->node.ref->type == reference_const_string )
      {
         printf ( "m%08x l%08x r%08x: %d string: %s\n", expr, expr->left, expr->right, expr->node.ref->ref.text->length, expr->node.ref->ref.text->string );
      }
   }
}

void destroy_expression ( expr_type* expr )
{
   if ( expr->left )
   {
      destroy_expression ( expr->left );
   }
   if ( expr->right )
   {
      destroy_expression ( expr->right );
   }

   switch ( expr->type )
   {
      case expression_number:
         free ( expr->node.num );
      break;
      case expression_reference:
         if ( expr->node.ref->type == reference_symbol )
         {
            free ( expr->node.ref->ref.symbol );
         }
         if ( expr->node.ref->type == reference_const_string )
         {
            free ( expr->node.ref->ref.text->string );
            free ( expr->node.ref->ref.text );
         }
         free ( expr->node.ref );
      break;
   }
   free ( expr );

   return;
}

expr_type* copy_expression ( expr_type* expr )
{
   expr_type* templ = NULL;
   expr_type* tempr = NULL;
   expr_type* temp;

   if ( expr->left )
   {
      templ = copy_expression ( expr->left );
   }
   if ( expr->right )
   {
      tempr = copy_expression ( expr->right );
   }

   temp = get_next_exprtype ();
   memcpy ( temp, expr, sizeof(expr_type) );
   temp->parent = NULL;
   switch ( expr->type )
   {
      case expression_number:
         temp->node.num = get_next_numtype ();
         memcpy ( temp->node.num, expr->node.num, sizeof(number_type) );
      break;
      case expression_reference:
         temp->node.ref = get_next_reftype ();
         memcpy ( temp->node.ref, expr->node.ref, sizeof(ref_type) );
         if ( temp->node.ref->type == reference_symbol )
         {
            temp->node.ref->ref.symbol = strdup ( expr->node.ref->ref.symbol );
         }
         if ( temp->node.ref->type == reference_const_string )
         {
            temp->node.ref->ref.text = get_next_texttype ();
            temp->node.ref->ref.text->string = strdup ( expr->node.ref->ref.text->string );
            temp->node.ref->ref.text->length = expr->node.ref->ref.text->length;
         }
      break;
   }
   temp->left = templ;
   temp->right = tempr;
   if ( templ )
   {
      temp->left->parent = temp;
   }
   if ( tempr )
   {
      temp->right->parent = temp;
   }

   return temp;
}

void reduce_expressions ( void )
{
   ir_table* ptr;
   int i, j;

   // go through each symbol, reducing its expression [if it has one]
   // with the expressions of other symbols, then go through the
   // intermediate assembly representation reducing expressions
   // found there with the newly-reduced symbol
   for ( i = 0; i < stab_ent; i++ )
   {
      for ( j = 0; j < stab_ent; j++ )
      {
         if ( (i != j) &&
              (stab[i].expr) )
         {
            reduce_expression ( stab[i].expr, &(stab[j]) );
         }
      }

      for ( ptr = ir_head; ptr != NULL; ptr = ptr->next )
      {
         if ( ptr->expr )
         {
            reduce_expression ( ptr->expr, &(stab[i]) );
         }
      }
   }
}

void evaluate_expression ( ir_table* ir, expr_type* expr, unsigned char* evaluated, unsigned char flag, char** symbol )
{
   int b;
   unsigned char ok = 1;

   if ( expr->left )
   {
      evaluate_expression ( ir, expr->left, evaluated, flag, symbol );
   }
   if ( expr->right )
   {
      evaluate_expression ( ir, expr->right, evaluated, flag, symbol );
   }
   if ( expr->type == expression_number )
   {
      expr->vtype = value_is_int;
      expr->value.ival = expr->node.num->number;
   }
   else if ( (flag == FIX) &&
             (expr->type == expression_reference) &&
             (expr->node.ref->type == reference_symbol) &&
             (expr->node.ref->ref.symbol[0] == '$') )
   {
      // '$' symbol is special 'PC' tracker
      expr->vtype = value_is_int;
      expr->value.ival = ir->addr;
   }
   else if ( (expr->type == expression_reference) &&
             (expr->node.ref->type == reference_symtab) )
   {
      if ( stab[expr->node.ref->ref.stab_ent].ir )
      {
         expr->vtype = value_is_int;
         expr->value.ival = stab[expr->node.ref->ref.stab_ent].ir->addr;
      }
   }
   else if ( (expr->type == expression_reference) &&
             (expr->node.ref->type == reference_const_string) )
   {
      expr->vtype = value_is_string;
      expr->value.sval = expr->node.ref->ref.text;
   }
   else if ( expr->type == expression_operator )
   {
      if ( (!expr->left) &&
           (expr->right->vtype == value_is_int) )
      {
         expr->vtype = value_is_int;
         if ( expr->node.op == '>' )
         {
            expr->value.ival = (expr->right->value.ival>>8)&0xFF;
         }
         else if ( expr->node.op == '<' )
         {
            expr->value.ival = expr->right->value.ival&0xFF;
         }
         else if ( expr->node.op == '!' )
         {
            expr->value.ival = !expr->right->value.ival;
         }
         else if ( expr->node.op == '-' )
         {
            expr->value.ival = -expr->right->value.ival;
         }
         // unary-plus is swallowed during expression parsing...
         else if ( expr->node.op == '~' )
         {
            expr->value.ival = ~expr->right->value.ival;
         }
         else
         {
            // shouldn't get here but just incase...
            ok = 0;
         }
      }
      else if ( (!expr->left) &&
                (expr->right->vtype == value_is_string) )
      {
         expr->vtype = value_is_string;
         expr->value.sval = expr->right->value.sval;
         if ( expr->node.op == '!' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] = !expr->value.sval->string[b];
            }
         }
         else if ( expr->node.op == '~' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] = ~expr->value.sval->string[b];
            }
         }
         else
         {
            // shouldn't get here but just incase...
            ok = 0;
         }
      }
      else if ( (expr->left) &&
                (expr->left->vtype == value_is_int) &&
                (expr->right) &&
                (expr->right->vtype == value_is_int) )
      {
         expr->vtype = value_is_int;
         if ( expr->node.op == '+' )
         {
            expr->value.ival = expr->left->value.ival+expr->right->value.ival;
         }
         else if ( expr->node.op == '-' )
         {
            expr->value.ival = expr->left->value.ival-expr->right->value.ival;
         }
         else if ( expr->node.op == '*' )
         {
            expr->value.ival = expr->left->value.ival*expr->right->value.ival;
         }
         else if ( expr->node.op == '/' )
         {
            expr->value.ival = expr->left->value.ival/expr->right->value.ival;
         }
         else if ( expr->node.op == '%' )
         {
            expr->value.ival = expr->left->value.ival%expr->right->value.ival;
         }
         else if ( expr->node.op == '<' )
         {
            expr->value.ival = (expr->left->value.ival < expr->right->value.ival);
         }
         else if ( expr->node.op == '>' )
         {
            expr->value.ival = (expr->left->value.ival > expr->right->value.ival);
         }
         else if ( expr->node.op == 'e' ) // equivalent
         {
            expr->value.ival = (expr->left->value.ival == expr->right->value.ival);
         }
         else if ( expr->node.op == 'n' ) // not-equivalent
         {
            expr->value.ival = (expr->left->value.ival != expr->right->value.ival);
         }
         else if ( expr->node.op == 'l' ) // less-than-or-equal-to
         {
            expr->value.ival = (expr->left->value.ival <= expr->right->value.ival);
         }
         else if ( expr->node.op == 'g' ) // greater-than-or-equal-to
         {
            expr->value.ival = (expr->left->value.ival >= expr->right->value.ival);
         }
         else if ( expr->node.op == ',' ) // left-shift
         {
            expr->value.ival = (expr->left->value.ival << expr->right->value.ival);
         }
         else if ( expr->node.op == '.' ) // right-shift
         {
            expr->value.ival = (expr->left->value.ival >> expr->right->value.ival);
         }
         else if ( expr->node.op == '7' ) // logical AND
         {
            expr->value.ival = (expr->left->value.ival && expr->right->value.ival);
         }
         else if ( expr->node.op == '\\' ) // logical OR
         {
            expr->value.ival = (expr->left->value.ival || expr->right->value.ival);
         }
         else if ( expr->node.op == '&' )
         {
            expr->value.ival = (expr->left->value.ival & expr->right->value.ival);
         }
         else if ( expr->node.op == '^' )
         {
            expr->value.ival = (expr->left->value.ival ^ expr->right->value.ival);
         }
         else if ( expr->node.op == '|' )
         {
            expr->value.ival = (expr->left->value.ival | expr->right->value.ival);
         }
         else
         {
            // shouldn't get here but just incase...
            ok = 0;
         }
      }
      else if ( (expr->left) &&
                (expr->left->vtype == value_is_string) &&
                (expr->right) &&
                (expr->right->vtype == value_is_int) )
      {
         expr->vtype = value_is_string;
         expr->value.sval = expr->left->value.sval;
         if ( expr->node.op == '+' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] += expr->right->value.ival;
            }
         }
         else if ( expr->node.op == '-' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] -= expr->right->value.ival;
            }
         }
         else if ( expr->node.op == '*' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] *= expr->right->value.ival;
            }
         }
         else if ( expr->node.op == '/' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] /= expr->right->value.ival;
            }
         }
         else if ( expr->node.op == '%' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] %= expr->right->value.ival;
            }
         }
         else
         {
            // shouldn't get here but just incase...
            ok = 0;
         }
      }
      else if ( (expr->left) &&
                (expr->left->vtype == value_is_string) &&
                (expr->right) &&
                (expr->right->vtype == value_is_string) &&
                (expr->right->value.sval->length == 1) ) // only allow "ABCD"+"A" for example...
      {
         expr->vtype = value_is_string;
         expr->value.sval = expr->left->value.sval;
         if ( expr->node.op == '+' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] += expr->right->value.sval->string[0];
            }
         }
         else if ( expr->node.op == '-' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] -= expr->right->value.sval->string[0];
            }
         }
         else if ( expr->node.op == '*' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] *= expr->right->value.sval->string[0];
            }
         }
         else if ( expr->node.op == '/' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] /= expr->right->value.sval->string[0];
            }
         }
         else if ( expr->node.op == '%' )
         {
            for ( b = 0; b < expr->value.sval->length; b++ )
            {
               expr->value.sval->string[b] %= expr->right->value.sval->string[0];
            }
         }
         else
         {
            // shouldn't get here but just incase...
            ok = 0;
         }
      }
      else
      {
         // shouldn't get here but just incase...
         ok = 0;
      }
   }
   else
   {
      // shouldn't get here but just incase...
      ok = 0;
   }

   if ( !ok )
   {
      (*evaluated) = 0;
      if ( (symbol) &&
           (expr->type == expression_reference) &&
           (expr->node.ref->type == reference_symbol) )
      {
         (*symbol) = expr->node.ref->ref.symbol;
      }
   }
   return;
}

void reduce_expression ( expr_type* expr, symbol_table* hint )
{
   expr_type* temp;
   expr_type* temp2;
   expr_type** temp3;
   int b;

   if ( expr->left )
   {
      reduce_expression ( expr->left, hint );
   }
   if ( expr->right )
   {
      reduce_expression ( expr->right, hint );
   }
   // check for global symbols that have become defined if one is passed in...
   if ( hint )
   {
      if ( (expr->type == expression_reference) &&
           (expr->node.ref->type == reference_symbol) &&
           (hint->expr) &&
           (strcmp(expr->node.ref->ref.symbol,hint->symbol) == 0) )
      {
         // glue global's expression to this tree...
         temp = expr->parent;
         if ( (temp) && (temp->left == expr) )
         {
            temp3 = &(temp->left);
         }
         else if ( (temp) && (temp->right == expr) )
         {
            temp3 = &(temp->right);
         }
         else
         {
            temp3 = NULL;
         }
         free ( expr->node.ref );
         free ( expr );
         expr = copy_expression ( hint->expr );
         expr->parent = temp;
         if ( temp3 )
         {
            (*temp3) = expr;
         }
      }
      if ( (expr->type == expression_reference) &&
           (expr->node.ref->type == reference_symbol) &&
           (!(hint->expr)) &&
           (strcmp(expr->node.ref->ref.symbol,hint->symbol) == 0) )
      {
         // glue global's reference node to this tree...
         free ( expr->node.ref->ref.symbol );
         expr->node.ref->type = reference_symtab;
         expr->node.ref->ref.stab_ent = hint->idx;
      }
   }

   // check for reducible operators...
   if ( expr->type == expression_operator )
   {
      // check for unary...
      if ( (!expr->left) &&
           (expr->right) && (expr->right->type == expression_number) )
      {
         // node and children are collapsible...adopt right node and morph it...
         if ( expr->node.op == '>' )
         {
            expr->node.num = expr->right->node.num;
            expr->node.num->number = (expr->node.num->number>>8)&0xFF;
            expr->node.num->zp_ok = 1;
         }
         else if ( expr->node.op == '<' )
         {
            expr->node.num = expr->right->node.num;
            expr->node.num->number &= 0xFF;
            expr->node.num->zp_ok = 1;
         }
         else if ( expr->node.op == '!' )
         {
            expr->node.num = expr->right->node.num;
            expr->node.num->number = !expr->node.num->number;
            // result should be 0 or 1
            expr->node.num->zp_ok = 1;
         }
         else if ( expr->node.op == '-' )
         {
            expr->node.num = expr->right->node.num;
            expr->node.num->number = -(expr->node.num->number);
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         // unary-plus is swallowed during expression parsing...
         else if ( expr->node.op == '~' )
         {
            expr->node.num = expr->right->node.num;
            expr->node.num->number = ~(expr->node.num->number);
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }

         // promote root node...
         expr->type = expression_number;

         // destroy reduced node...
         // we adopted the right node so don't destroy its node content...
         free ( expr->right );
         expr->right = NULL;
      }
      // check for unary...
      else if ( (!expr->left) &&
                (expr->right) && (expr->right->type == expression_reference) &&
                (expr->right->node.ref->type == reference_const_string) )
      {
         // node and children are collapsible...adopt right node and morph it...
         if ( expr->node.op == '!' )
         {
            expr->node.ref = expr->right->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] = !expr->node.ref->ref.text->string[b];
            }
         }
         // unary-plus is swallowed during expression parsing...
         else if ( expr->node.op == '~' )
         {
            expr->node.ref = expr->right->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] = ~expr->node.ref->ref.text->string[b];
            }
         }

         // promote root node...
         expr->type = expression_reference;

         // destroy reduced node...
         // we adopted the right node so don't destroy its node content...
         free ( expr->right );
         expr->right = NULL;
      }
      else if ( (expr->left) &&
                (expr->left->type == expression_number) &&
                (expr->right) &&
                (expr->right->type == expression_number) )
      {
         // node and children are collapsible...adopt one and morph it with the other...
         if ( expr->node.op == '+' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number += expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '-' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number -= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '*' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number *= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '/' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number /= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '%' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number %= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '<' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number < expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == '>' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number > expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == 'e' ) // equivalent
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number == expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == 'n' ) // not equivalent
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number != expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == 'l' ) // less-than-or-equal-to
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number <= expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == 'g' ) // greater-than-or-equal-to
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number >= expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == ',' ) // left-shift
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number <<= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '.' ) // right-shift
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number >>= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '7' ) // logical AND
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number && expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == '\\' ) // logical OR
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number = (expr->left->node.num->number || expr->right->node.num->number);
            expr->node.num->zp_ok = 1; // answer will be 0 or 1
         }
         else if ( expr->node.op == '&' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number &= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '^' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number ^= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }
         else if ( expr->node.op == '|' )
         {
            expr->node.num = expr->left->node.num;
            expr->node.num->number |= expr->right->node.num->number;
            expr->node.num->zp_ok = 0;
            if ( (expr->node.num->number >= -128) &&
                 (expr->node.num->number < 256) )
            {
               expr->node.num->zp_ok = 1;
            }
         }

         // promote root node...
         expr->type = expression_number;

         // destroy reduced nodes...
         // we adopted the left node so don't destroy its node content...
         free ( expr->right->node.num );
         free ( expr->right );
         free ( expr->left );
         expr->left = NULL;
         expr->right = NULL;
      }
      else if ( (expr->left) &&
                (expr->left->type == expression_reference) &&
                (expr->left->node.ref->type == reference_const_string) &&
                (expr->right) &&
                (expr->right->type == expression_number) )
      {
         // node and children are collapsible...adopt one and morph it with the other...
         if ( expr->node.op == '+' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] += expr->right->node.num->number;
            }
         }
         else if ( expr->node.op == '-' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] -= expr->right->node.num->number;
            }
         }
         else if ( expr->node.op == '*' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] *= expr->right->node.num->number;
            }
         }
         else if ( expr->node.op == '/' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] /= expr->right->node.num->number;
            }
         }
         else if ( expr->node.op == '%' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] %= expr->right->node.num->number;
            }
         }

         // promote root node...
         expr->type = expression_reference;

         // destroy reduced nodes...
         // we adopted the left node so don't destroy its node content...
         free ( expr->right->node.num );
         free ( expr->right );
         free ( expr->left );
         expr->left = NULL;
         expr->right = NULL;
      }
      else if ( (expr->left) &&
                (expr->left->type == expression_reference) &&
                (expr->left->node.ref->type == reference_const_string) &&
                (expr->right) &&
                (expr->right->type == expression_reference) &&
                (expr->right->node.ref->type == reference_const_string) &&
                (expr->right->node.ref->ref.text->length == 1) ) // only allow for "AB"+"A" for example...
      {
         // node and children are collapsible...adopt one and morph it with the other...
         if ( expr->node.op == '+' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] += expr->right->node.ref->ref.text->string[0];
            }
         }
         else if ( expr->node.op == '-' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] -= expr->right->node.ref->ref.text->string[0];
            }
         }
         else if ( expr->node.op == '*' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] *= expr->right->node.ref->ref.text->string[0];
            }
         }
         else if ( expr->node.op == '/' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] /= expr->right->node.ref->ref.text->string[0];
            }
         }
         else if ( expr->node.op == '%' )
         {
            expr->node.ref = expr->left->node.ref;
            for ( b = 0; b < expr->node.ref->ref.text->length; b++ )
            {
               expr->node.ref->ref.text->string[b] %= expr->right->node.ref->ref.text->string[0];
            }
         }

         // promote root node...
         expr->type = expression_reference;

         // destroy reduced nodes...
         // we adopted the left node so don't destroy its node content...
         free ( expr->right->node.ref->ref.text->string );
         free ( expr->right->node.ref );
         free ( expr->right );
         free ( expr->left );
         expr->left = NULL;
         expr->right = NULL;
      }
   }

   //dump_expression ( expr );
}

ir_table* emit_ir ( void )
{
	ir_table* ptr;

   // If we're supposed to be emitting, do so...
   if ( emitting[preproc_nest_level] )
   {
      if ( ir_tail == NULL )
      {
         ir_head = (ir_table*) malloc ( sizeof(ir_table) );
         if ( ir_head != NULL )
         {
            ir_tail = ir_head;
            ir_tail->btab_ent = cur->idx;
            ir_tail->addr = 0;
            ir_tail->emitted = 0;
            ir_tail->multi = 0;
            ir_tail->align = 0;
            ir_tail->label = 0;
            ir_tail->fixed = 0;
            ir_tail->string = 0;
            ir_tail->source_linenum = yylineno;
            ir_tail->next = NULL;
            ir_tail->prev = NULL;
            ir_tail->expr = NULL;
         }
         else
         {
            yyerror ( "cannot allocate memory" );
         }
      }
      else
      {
         ptr = (ir_table*) malloc ( sizeof(ir_table) );
         if ( ptr != NULL )
         {
            ir_tail->next = ptr;
            ptr->prev = ir_tail;
            ptr->next = NULL;
            ir_tail = ptr;
            ir_tail->btab_ent = cur->idx;
            ir_tail->addr = 0;
            ir_tail->emitted = 0;
            ir_tail->multi = 0;
            ir_tail->align = 0;
            ir_tail->label = 0;
            ir_tail->fixed = 0;
            ir_tail->string = 0;
            ir_tail->source_linenum = yylineno;
            ir_tail->expr = NULL;
         }
         else
         {
            yyerror ( "cannot allocate memory" );
         }
      }
      return ir_tail;
   }
   // We're not supposed to be emitting, so don't...
   return NULL;
}

ir_table* reemit_ir ( ir_table* head, ir_table* tail )
{
   ir_table* ptr;

   // skip past first node, it is a dummy label node
   // to keep us from re-emitting the instruction that
   // is immediately prior to the REPT directive.
   // [Directives typically don't emit their own nodes.]
   for ( head = head->next; head != tail; head = head->next )
   {
      // create a new node...
      ptr = emit_ir ();
      if ( ptr )
      {
         // copy node contents...
         ptr->data[0] = head->data[0];
         ptr->data[1] = head->data[1];
         ptr->data[2] = head->data[2];
         ptr->btab_ent = head->btab_ent;
         ptr->addr = cur->addr; // address not copied!
         ptr->len = head->len;
         cur->addr += ptr->len;
         ptr->emitted = head->emitted;
         ptr->multi = head->multi;
         ptr->align = head->align;
         ptr->fixed = head->fixed;
         ptr->label = head->label;
         ptr->string = head->string;
         ptr->fixup = head->fixup;
         ptr->source_linenum = head->source_linenum;
         if ( head->expr )
         {
            ptr->expr = copy_expression ( head->expr );
         }
         else
         {
            ptr->expr = NULL;
         }
      }
   }
}

ir_table* emit_fix ( int addr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->len = 0;
      ptr->fixed = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_label ( int m, int fill )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->len = m;
      ptr->data[0] = fill;
      ptr->label = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_space ( int m, int fill )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->len = m;
      ptr->data[0] = fill;
      ptr->multi = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin ( expr_type* expr )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_datab;

      ptr->expr = expr;

      ptr->len = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin2 ( expr_type* expr )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_dataw;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_data ( unsigned char data )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->data[0] = data;
      ptr->len = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_string ( expr_type* expr )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_string;
      ptr->len = expr->value.sval->length;
      ptr->string = 1;

      ptr->expr = expr;

      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_align ( int m )
{
   ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_align;
      ptr->data[0] = m;
      if ( m )
      {
         if ( ((unsigned int)cur->addr%m) )
         {
            ptr->len = (m-((unsigned int)cur->addr%m));
         }
         else
         {
            m = 0;
         }
      }
      else
      {
         m = 0;
      }
      ptr->len = m;
      ptr->multi = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_implied ( C6502_opcode* opcode )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->data[0] = opcode->op;
      ptr->len = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_accumulator ( C6502_opcode* opcode )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_fixed;
      ptr->data[0] = opcode->op;
      ptr->len = 1;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_immediate ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_immediate;
      ptr->data[0] = opcode->op;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_absolute ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_absolute;
      ptr->data[0] = opcode->op;

      // don't attempt expression reduction here, let the fixup
      // engine have a whack at morphing this emitted thing into
      // zeropage if possible...
      ptr->expr = expr;

      ptr->len = 3;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_zeropage ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_zeropage;
      ptr->data[0] = opcode->op;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_relative ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->fixup = fixup_relative;
      ptr->data[0] = opcode->op;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_indirect ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_indirect;

      ptr->expr = expr;

      ptr->len = 3;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_abs_idx_x ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_abs_idx_x;

      // don't attempt expression reduction here, let the fixup
      // engine have a whack at morphing this emitted thing into
      // zeropage if possible...
      ptr->expr = expr;

      ptr->len = 3;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_abs_idx_y ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_abs_idx_y;

      // don't attempt expression reduction here, let the fixup
      // engine have a whack at morphing this emitted thing into
      // zeropage if possible...
      ptr->expr = expr;

      ptr->len = 3;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_zp_idx_x ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_zp_idx;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_zp_idx_y ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_zp_idx;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_pre_idx_ind ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_pre_idx_ind;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}

ir_table* emit_bin_post_idx_ind ( C6502_opcode* opcode, expr_type* expr )
{
	ir_table* ptr = emit_ir ();
   if ( ptr )
   {
      ptr->data[0] = opcode->op;
      ptr->fixup = fixup_post_idx_ind;

      ptr->expr = expr;

      ptr->len = 2;
      ptr->addr = cur->addr;
      cur->addr += ptr->len;
   }
   return ptr;
}
