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

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesrom.h"
#include "cnesapu.h"
#include "cnesio.h"
#include "cnesmappers.h"
#include "cnesios.h"

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

CNES6502_opcode C6502::m_6502opcode [ 256 ] =
{
   { 0x00, "BRK", &C6502::BRK, AM_IMPLIED, 7, true, false, 0x0 }, // BRK
   { 0x01, "ORA", &C6502::ORA, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // ORA - (Indirect,X)
   { 0x02, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x03, "ASO", &C6502::ASO, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // ASO - (Indirect,X) (undocumented)
   { 0x04, "DOP", &C6502::DOP, AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x05, "ORA", &C6502::ORA, AM_ZEROPAGE, 3, true, false, 0x4 }, // ORA - Zero Page
   { 0x06, "ASL", &C6502::ASL, AM_ZEROPAGE, 5, true, false, 0x10 }, // ASL - Zero Page
   { 0x07, "ASO", &C6502::ASO, AM_ZEROPAGE, 5, false, false, 0x10 }, // ASO - Zero Page (undocumented)
   { 0x08, "PHP", &C6502::PHP, AM_IMPLIED, 3, true, false, 0x4 }, // PHP
   { 0x09, "ORA", &C6502::ORA, AM_IMMEDIATE, 2, true, false, 0x2 }, // ORA - Immediate
   { 0x0A, "ASL", &C6502::ASL, AM_ACCUMULATOR, 2, true, false, 0x2 }, // ASL - Accumulator
   { 0x0B, "ANC", &C6502::ANC, AM_IMMEDIATE, 2, false, false, 0x2 }, // ANC - Immediate (undocumented)
   { 0x0C, "TOP", &C6502::TOP, AM_ABSOLUTE, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x0D, "ORA", &C6502::ORA, AM_ABSOLUTE, 4, true, false, 0x8 }, // ORA - Absolute
   { 0x0E, "ASL", &C6502::ASL, AM_ABSOLUTE, 6, true, false, 0x20 }, // ASL - Absolute
   { 0x0F, "ASO", &C6502::ASO, AM_ABSOLUTE, 6, false, false, 0x20 }, // ASO - Absolute (undocumented)
   { 0x10, "BPL", &C6502::BPL, AM_RELATIVE, 2, true, false, 0xA }, // BPL
   { 0x11, "ORA", &C6502::ORA, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // ORA - (Indirect),Y
   { 0x12, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x13, "ASO", &C6502::ASO, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // ASO - (Indirect),Y (undocumented)
   { 0x14, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x15, "ORA", &C6502::ORA, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // ORA - Zero Page,X
   { 0x16, "ASL", &C6502::ASL, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ASL - Zero Page,X
   { 0x17, "ASO", &C6502::ASO, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // ASO - Zero Page,X (undocumented)
   { 0x18, "CLC", &C6502::CLC, AM_IMPLIED, 2, true, false, 0x2 }, // CLC
   { 0x19, "ORA", &C6502::ORA, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // ORA - Absolute,Y
   { 0x1A, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x1B, "ASO", &C6502::ASO, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // ASO - Absolute,Y (undocumented)
   { 0x1C, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x1D, "ORA", &C6502::ORA, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // ORA - Absolute,X
   { 0x1E, "ASL", &C6502::ASL, AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // ASL - Absolute,X
   { 0x1F, "ASO", &C6502::ASO, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // ASO - Absolute,X (undocumented)
   { 0x20, "JSR", &C6502::JSR, AM_ABSOLUTE, 6, true, false, 0x20 }, // JSR
   { 0x21, "AND", &C6502::AND, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // AND - (Indirect,X)
   { 0x22, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x23, "RLA", &C6502::RLA, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // RLA - (Indirect,X) (undocumented)
   { 0x24, "BIT", &C6502::BIT, AM_ZEROPAGE, 3, true, false, 0x4 }, // BIT - Zero Page
   { 0x25, "AND", &C6502::AND, AM_ZEROPAGE, 3, true, false, 0x4 }, // AND - Zero Page
   { 0x26, "ROL", &C6502::ROL, AM_ZEROPAGE, 5, true, false, 0x10 }, // ROL - Zero Page
   { 0x27, "RLA", &C6502::RLA, AM_ZEROPAGE, 5, false, false, 0x10 }, // RLA - Zero Page (undocumented)
   { 0x28, "PLP", &C6502::PLP, AM_IMPLIED, 4, true, false, 0x8 }, // PLP
   { 0x29, "AND", &C6502::AND, AM_IMMEDIATE, 2, true, false, 0x2 }, // AND - Immediate
   { 0x2A, "ROL", &C6502::ROL, AM_ACCUMULATOR, 2, true, false, 0x2 }, // ROL - Accumulator
   { 0x2B, "ANC", &C6502::ANC, AM_IMMEDIATE, 2, false, false, 0x2 }, // ANC - Immediate (undocumented)
   { 0x2C, "BIT", &C6502::BIT, AM_ABSOLUTE, 4, true, false, 0x8 }, // BIT - Absolute
   { 0x2D, "AND", &C6502::AND, AM_ABSOLUTE, 4, true, false, 0x8 }, // AND - Absolute
   { 0x2E, "ROL", &C6502::ROL, AM_ABSOLUTE, 6, true, false, 0x20 }, // ROL - Absolute
   { 0x2F, "RLA", &C6502::RLA, AM_ABSOLUTE, 6, false, false, 0x20 }, // RLA - Absolute (undocumented)
   { 0x30, "BMI", &C6502::BMI, AM_RELATIVE, 2, true, false, 0x2 }, // BMI
   { 0x31, "AND", &C6502::AND, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // AND - (Indirect),Y
   { 0x32, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x33, "RLA", &C6502::RLA, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // RLA - (Indirect),Y (undocumented)
   { 0x34, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x35, "AND", &C6502::AND, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // AND - Zero Page,X
   { 0x36, "ROL", &C6502::ROL, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ROL - Zero Page,X
   { 0x37, "RLA", &C6502::RLA, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // RLA - Zero Page,X (undocumented)
   { 0x38, "SEC", &C6502::SEC, AM_IMPLIED, 2, true, false, 0x2 }, // SEC
   { 0x39, "AND", &C6502::AND, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // AND - Absolute,Y
   { 0x3A, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x3B, "RLA", &C6502::RLA, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // RLA - Absolute,Y (undocumented)
   { 0x3C, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x3D, "AND", &C6502::AND, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // AND - Absolute,X
   { 0x3E, "ROL", &C6502::ROL, AM_ABSOLUTE_INDEXED_X, 7, true, false, 0x40 }, // ROL - Absolute,X
   { 0x3F, "RLA", &C6502::RLA, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // RLA - Absolute,X (undocumented)
   { 0x40, "RTI", &C6502::RTI, AM_IMPLIED, 6, true, false, 0x20 }, // RTI
   { 0x41, "EOR", &C6502::EOR, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // EOR - (Indirect,X)
   { 0x42, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x43, "LSE", &C6502::LSE, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // LSE - (Indirect,X) (undocumented)
   { 0x44, "DOP", &C6502::DOP, AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x45, "EOR", &C6502::EOR, AM_ZEROPAGE, 3, true, false, 0x4 }, // EOR - Zero Page
   { 0x46, "LSR", &C6502::LSR, AM_ZEROPAGE, 5, true, false, 0x10 }, // LSR - Zero Page
   { 0x47, "LSE", &C6502::LSE, AM_ZEROPAGE, 5, false, false, 0x10 }, // LSE - Zero Page (undocumented)
   { 0x48, "PHA", &C6502::PHA, AM_IMPLIED, 3, true, false, 0x4 }, // PHA
   { 0x49, "EOR", &C6502::EOR, AM_IMMEDIATE, 2, true, false, 0x2 }, // EOR - Immediate
   { 0x4A, "LSR", &C6502::LSR, AM_ACCUMULATOR, 2, true, false, 0x2 }, // LSR - Accumulator
   { 0x4B, "ALR", &C6502::ALR, AM_IMMEDIATE, 2, false, false, 0x2 }, // ALR - Immediate (undocumented)
   { 0x4C, "JMP", &C6502::JMP, AM_ABSOLUTE, 3, true, false, 0x4 }, // JMP - Absolute
   { 0x4D, "EOR", &C6502::EOR, AM_ABSOLUTE, 4, true, false, 0x8 }, // EOR - Absolute
   { 0x4E, "LSR", &C6502::LSR, AM_ABSOLUTE, 6, true, false, 0x20 }, // LSR - Absolute
   { 0x4F, "LSE", &C6502::LSE, AM_ABSOLUTE, 6, false, false, 0x20 }, // LSE - Absolute (undocumented)
   { 0x50, "BVC", &C6502::BVC, AM_RELATIVE, 2, true, false, 0xA }, // BVC
   { 0x51, "EOR", &C6502::EOR, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // EOR - (Indirect),Y
   { 0x52, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x53, "LSE", &C6502::LSE, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // LSE - (Indirect),Y
   { 0x54, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x55, "EOR", &C6502::EOR, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // EOR - Zero Page,X
   { 0x56, "LSR", &C6502::LSR, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // LSR - Zero Page,X
   { 0x57, "LSE", &C6502::LSE, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // LSE - Zero Page,X (undocumented)
   { 0x58, "CLI", &C6502::CLI, AM_IMPLIED, 2, true, false, 0x2 }, // CLI
   { 0x59, "EOR", &C6502::EOR, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // EOR - Absolute,Y
   { 0x5A, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x5B, "LSE", &C6502::LSE, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // LSE - Absolute,Y (undocumented)
   { 0x5C, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x5D, "EOR", &C6502::EOR, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // EOR - Absolute,X
   { 0x5E, "LSR", &C6502::LSR, AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // LSR - Absolute,X
   { 0x5F, "LSE", &C6502::LSE, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // LSE - Absolute,X (undocumented)
   { 0x60, "RTS", &C6502::RTS, AM_IMPLIED, 6, true, false, 0x20 }, // RTS
   { 0x61, "ADC", &C6502::ADC, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // ADC - (Indirect,X)
   { 0x62, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x63, "RRA", &C6502::RRA, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // RRA - (Indirect,X) (undocumented)
   { 0x64, "DOP", &C6502::DOP, AM_ZEROPAGE, 3, false, false, 0x4 }, // DOP (undocumented)
   { 0x65, "ADC", &C6502::ADC, AM_ZEROPAGE, 3, true, false, 0x4 }, // ADC - Zero Page
   { 0x66, "ROR", &C6502::ROR, AM_ZEROPAGE, 5, true, false, 0x10 }, // ROR - Zero Page
   { 0x67, "RRA", &C6502::RRA, AM_ZEROPAGE, 5, false, false, 0x10 }, // RRA - Zero Page (undocumented)
   { 0x68, "PLA", &C6502::PLA, AM_IMPLIED, 4, true, false, 0x8 }, // PLA
   { 0x69, "ADC", &C6502::ADC, AM_IMMEDIATE, 2, true, false, 0x2 }, // ADC - Immediate
   { 0x6A, "ROR", &C6502::ROR, AM_ACCUMULATOR, 2, true, false, 0x2 }, // ROR - Accumulator
   { 0x6B, "ARR", &C6502::ARR, AM_IMMEDIATE, 2, false, false, 0x2 }, // ARR - Immediate (undocumented)
   { 0x6C, "JMP", &C6502::JMP, AM_INDIRECT, 5, true, false, 0x10 }, // JMP - Indirect
   { 0x6D, "ADC", &C6502::ADC, AM_ABSOLUTE, 4, true, false, 0x8 }, // ADC - Absolute
   { 0x6E, "ROR", &C6502::ROR, AM_ABSOLUTE, 6, true, false, 0x20 }, // ROR - Absolute
   { 0x6F, "RRA", &C6502::RRA, AM_ABSOLUTE, 6, false, false, 0x20 }, // RRA - Absolute (undocumented)
   { 0x70, "BVS", &C6502::BVS, AM_RELATIVE, 2, true, false, 0xA }, // BVS
   { 0x71, "ADC", &C6502::ADC, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // ADC - (Indirect),Y
   { 0x72, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x73, "RRA", &C6502::RRA, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // RRA - (Indirect),Y (undocumented)
   { 0x74, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0x75, "ADC", &C6502::ADC, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // ADC - Zero Page,X
   { 0x76, "ROR", &C6502::ROR, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // ROR - Zero Page,X
   { 0x77, "RRA", &C6502::RRA, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // RRA - Zero Page,X (undocumented)
   { 0x78, "SEI", &C6502::SEI, AM_IMPLIED, 2, true, false, 0x2 }, // SEI
   { 0x79, "ADC", &C6502::ADC, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // ADC - Absolute,Y
   { 0x7A, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0x7B, "RRA", &C6502::RRA, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // RRA - Absolute,Y (undocumented)
   { 0x7C, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0x7D, "ADC", &C6502::ADC, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // ADC - Absolute,X
   { 0x7E, "ROR", &C6502::ROR, AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // ROR - Absolute,X
   { 0x7F, "RRA", &C6502::RRA, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // RRA - Absolute,X (undocumented)
   { 0x80, "DOP", &C6502::DOP, AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x81, "STA", &C6502::STA, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // STA - (Indirect,X)
   { 0x82, "DOP", &C6502::DOP, AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x83, "AXS", &C6502::AXS, AM_PREINDEXED_INDIRECT, 6, false, false, 0x20 }, // AXS - (Indirect,X) (undocumented)
   { 0x84, "STY", &C6502::STY, AM_ZEROPAGE, 3, true, false, 0x4 }, // STY - Zero Page
   { 0x85, "STA", &C6502::STA, AM_ZEROPAGE, 3, true, false, 0x4 }, // STA - Zero Page
   { 0x86, "STX", &C6502::STX, AM_ZEROPAGE, 3, true, false, 0x4 }, // STX - Zero Page
   { 0x87, "AXS", &C6502::AXS, AM_ZEROPAGE, 3, false, false, 0x4 }, // AXS - Zero Page (undocumented)
   { 0x88, "DEY", &C6502::DEY, AM_IMPLIED, 2, true, false, 0x2 }, // DEY
   { 0x89, "DOP", &C6502::DOP, AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0x8A, "TXA", &C6502::TXA, AM_IMPLIED, 2, true, false, 0x2 }, // TXA
   { 0x8B, "XAA", &C6502::XAA, AM_IMMEDIATE, 2, false, false, 0x2 }, // XAA - Immediate (undocumented)
   { 0x8C, "STY", &C6502::STY, AM_ABSOLUTE, 4, true, false, 0x8 }, // STY - Absolute
   { 0x8D, "STA", &C6502::STA, AM_ABSOLUTE, 4, true, false, 0x8 }, // STA - Absolute
   { 0x8E, "STX", &C6502::STX, AM_ABSOLUTE, 4, true, false, 0x8 }, // STX - Absolute
   { 0x8F, "AXS", &C6502::AXS, AM_ABSOLUTE, 4, false, false, 0x8 }, // AXS - Absolulte (undocumented)
   { 0x90, "BCC", &C6502::BCC, AM_RELATIVE, 2, true, false, 0xA }, // BCC
   { 0x91, "STA", &C6502::STA, AM_POSTINDEXED_INDIRECT, 6, true, true, 0x20 }, // STA - (Indirect),Y
   { 0x92, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0x93, "AXA", &C6502::AXA, AM_POSTINDEXED_INDIRECT, 6, false, true, 0x20 }, // AXA - (Indirect),Y
   { 0x94, "STY", &C6502::STY, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // STY - Zero Page,X
   { 0x95, "STA", &C6502::STA, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // STA - Zero Page,X
   { 0x96, "STX", &C6502::STX, AM_ZEROPAGE_INDEXED_Y, 4, true, false, 0x8 }, // STX - Zero Page,Y
   { 0x97, "AXS", &C6502::AXS, AM_ZEROPAGE_INDEXED_Y, 4, false, false, 0x8 }, // AXS - Zero Page,Y
   { 0x98, "TYA", &C6502::TYA, AM_IMPLIED, 2, true, false, 0x2 }, // TYA
   { 0x99, "STA", &C6502::STA, AM_ABSOLUTE_INDEXED_Y, 5, true, true, 0x10 }, // STA - Absolute,Y
   { 0x9A, "TXS", &C6502::TXS, AM_IMPLIED, 2, true, false, 0x2 }, // TXS
   { 0x9B, "TAS", &C6502::TAS, AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // TAS - Absolute,Y (undocumented)
   { 0x9C, "SAY", &C6502::SAY, AM_ABSOLUTE_INDEXED_X, 5, false, true, 0x10 }, // SAY - Absolute,X (undocumented)
   { 0x9D, "STA", &C6502::STA, AM_ABSOLUTE_INDEXED_X, 5, true, true, 0x10 }, // STA - Absolute,X
   { 0x9E, "XAS", &C6502::XAS, AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // XAS - Absolute,Y (undocumented)
   { 0x9F, "AXA", &C6502::AXA, AM_ABSOLUTE_INDEXED_Y, 5, false, true, 0x10 }, // AXA - Absolute,Y (undocumented)
   { 0xA0, "LDY", &C6502::LDY, AM_IMMEDIATE, 2, true, false, 0x2 }, // LDY - Immediate
   { 0xA1, "LDA", &C6502::LDA, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // LDA - (Indirect,X)
   { 0xA2, "LDX", &C6502::LDX, AM_IMMEDIATE, 2, true, false, 0x2 }, // LDX - Immediate
   { 0xA3, "LAX", &C6502::LAX, AM_PREINDEXED_INDIRECT, 6, false, false, 0x20 }, // LAX - (Indirect,X) (undocumented)
   { 0xA4, "LDY", &C6502::LDY, AM_ZEROPAGE, 3, true, false, 0x4 }, // LDY - Zero Page
   { 0xA5, "LDA", &C6502::LDA, AM_ZEROPAGE, 3, true, false, 0x4 }, // LDA - Zero Page
   { 0xA6, "LDX", &C6502::LDX, AM_ZEROPAGE, 3, true, false, 0x4 }, // LDX - Zero Page
   { 0xA7, "LAX", &C6502::LAX, AM_ZEROPAGE, 3, false, false, 0x4 }, // LAX - Zero Page (undocumented)
   { 0xA8, "TAY", &C6502::TAY, AM_IMPLIED, 2, true, false, 0x2 }, // TAY
   { 0xA9, "LDA", &C6502::LDA, AM_IMMEDIATE, 2, true, false, 0x2 }, // LDA - Immediate
   { 0xAA, "TAX", &C6502::TAX, AM_IMPLIED, 2, true, false, 0x2 }, // TAX
   { 0xAB, "OAL", &C6502::OAL, AM_IMMEDIATE, 2, false, false, 0x2 }, // OAL - Immediate
   { 0xAC, "LDY", &C6502::LDY, AM_ABSOLUTE, 4, true, false, 0x8 }, // LDY - Absolute
   { 0xAD, "LDA", &C6502::LDA, AM_ABSOLUTE, 4, true, false, 0x8 }, // LDA - Absolute
   { 0xAE, "LDX", &C6502::LDX, AM_ABSOLUTE, 4, true, false, 0x8 }, // LDX - Absolute
   { 0xAF, "LAX", &C6502::LAX, AM_ABSOLUTE, 4, false, false, 0x8 }, // LAX - Absolute (undocumented)
   { 0xB0, "BCS", &C6502::BCS, AM_RELATIVE, 2, true, false, 0xA }, // BCS
   { 0xB1, "LDA", &C6502::LDA, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // LDA - (Indirect),Y
   { 0xB2, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xB3, "LAX", &C6502::LAX, AM_POSTINDEXED_INDIRECT, 5, false, false, 0x10 }, // LAX - (Indirect),Y (undocumented)
   { 0xB4, "LDY", &C6502::LDY, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // LDY - Zero Page,X
   { 0xB5, "LDA", &C6502::LDA, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // LDA - Zero Page,X
   { 0xB6, "LDX", &C6502::LDX, AM_ZEROPAGE_INDEXED_Y, 4, true, false, 0x8 }, // LDX - Zero Page,Y
   { 0xB7, "LAX", &C6502::LAX, AM_ZEROPAGE_INDEXED_Y, 4, false, false, 0x8 }, // LAX - Zero Page,X (undocumented)
   { 0xB8, "CLV", &C6502::CLV, AM_IMPLIED, 2, true, false, 0x2 }, // CLV
   { 0xB9, "LDA", &C6502::LDA, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // LDA - Absolute,Y
   { 0xBA, "TSX", &C6502::TSX, AM_IMPLIED, 2, true, false, 0x2 }, // TSX
   { 0xBB, "LAS", &C6502::LAS, AM_ABSOLUTE_INDEXED_Y, 4, false, false, 0x8 }, // LAS - Absolute,Y (undocumented)
   { 0xBC, "LDY", &C6502::LDY, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // LDY - Absolute,X
   { 0xBD, "LDA", &C6502::LDA, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // LDA - Absolute,X
   { 0xBE, "LDX", &C6502::LDX, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // LDX - Absolute,Y
   { 0xBF, "LAX", &C6502::LAX, AM_ABSOLUTE_INDEXED_Y, 4, false, false, 0x8 }, // LAX - Absolute,Y (undocumented)
   { 0xC0, "CPY", &C6502::CPY, AM_IMMEDIATE, 2, true, false, 0x2 }, // CPY - Immediate
   { 0xC1, "CMP", &C6502::CMP, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // CMP - (Indirect,X)
   { 0xC2, "DOP", &C6502::DOP, AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0xC3, "DCM", &C6502::DCM, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // DCM - (Indirect,X) (undocumented)
   { 0xC4, "CPY", &C6502::CPY, AM_ZEROPAGE, 3, true, false, 0x4 }, // CPY - Zero Page
   { 0xC5, "CMP", &C6502::CMP, AM_ZEROPAGE, 3, true, false, 0x4 }, // CMP - Zero Page
   { 0xC6, "DEC", &C6502::DEC, AM_ZEROPAGE, 5, true, false, 0x10 }, // DEC - Zero Page
   { 0xC7, "DCM", &C6502::DCM, AM_ZEROPAGE, 5, true, false, 0x10 }, // DCM - Zero Page (undocumented)
   { 0xC8, "INY", &C6502::INY, AM_IMPLIED, 2, true, false, 0x2 }, // INY
   { 0xC9, "CMP", &C6502::CMP, AM_IMMEDIATE, 2, true, false, 0x2 }, // CMP - Immediate
   { 0xCA, "DEX", &C6502::DEX, AM_IMPLIED, 2, true, false, 0x2 }, // DEX
   { 0xCB, "SAX", &C6502::SAX, AM_IMMEDIATE, 2, false, false, 0x2 }, // SAX - Immediate (undocumented)
   { 0xCC, "CPY", &C6502::CPY, AM_ABSOLUTE, 4, true, false, 0x8 }, // CPY - Absolute
   { 0xCD, "CMP", &C6502::CMP, AM_ABSOLUTE, 4, true, false, 0x8 }, // CMP - Absolute
   { 0xCE, "DEC", &C6502::DEC, AM_ABSOLUTE, 6, true, false, 0x20 }, // DEC - Absolute
   { 0xCF, "DCM", &C6502::DCM, AM_ABSOLUTE, 6, false, false, 0x20 }, // DCM - Absolute (undocumented)
   { 0xD0, "BNE", &C6502::BNE, AM_RELATIVE, 2, true, false, 0xA }, // BNE
   { 0xD1, "CMP", &C6502::CMP, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // CMP   (Indirect),Y
   { 0xD2, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xD3, "DCM", &C6502::DCM, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // DCM - (Indirect),Y (undocumented)
   { 0xD4, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0xD5, "CMP", &C6502::CMP, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // CMP - Zero Page,X
   { 0xD6, "DEC", &C6502::DEC, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // DEC - Zero Page,X
   { 0xD7, "DCM", &C6502::DCM, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // DCM - Zero Page,X (undocumented)
   { 0xD8, "CLD", &C6502::CLD, AM_IMPLIED, 2, true, false, 0x2 }, // CLD
   { 0xD9, "CMP", &C6502::CMP, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // CMP - Absolute,Y
   { 0xDA, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0xDB, "DCM", &C6502::DCM, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // DCM - Absolute,Y (undocumented)
   { 0xDC, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0xDD, "CMP", &C6502::CMP, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // CMP - Absolute,X
   { 0xDE, "DEC", &C6502::DEC, AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // DEC - Absolute,X
   { 0xDF, "DCM", &C6502::DCM, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }, // DCM - Absolute,X (undocumented)
   { 0xE0, "CPX", &C6502::CPX, AM_IMMEDIATE, 2, true, false, 0x2 }, // CPX - Immediate
   { 0xE1, "SBC", &C6502::SBC, AM_PREINDEXED_INDIRECT, 6, true, false, 0x20 }, // SBC - (Indirect,X)
   { 0xE2, "DOP", &C6502::DOP, AM_IMMEDIATE, 2, false, false, 0x2 }, // DOP (undocumented)
   { 0xE3, "INS", &C6502::INS, AM_PREINDEXED_INDIRECT, 8, false, false, 0x80 }, // INS - (Indirect,X) (undocumented)
   { 0xE4, "CPX", &C6502::CPX, AM_ZEROPAGE, 3, true, false, 0x4 }, // CPX - Zero Page
   { 0xE5, "SBC", &C6502::SBC, AM_ZEROPAGE, 3, true, false, 0x4 }, // SBC - Zero Page
   { 0xE6, "INC", &C6502::INC, AM_ZEROPAGE, 5, true, false, 0x10 }, // INC - Zero Page
   { 0xE7, "INS", &C6502::INS, AM_ZEROPAGE, 5, false, false, 0x10 }, // INS - Zero Page (undocumented)
   { 0xE8, "INX", &C6502::INX, AM_IMPLIED, 2, true, false, 0x2 }, // INX
   { 0xE9, "SBC", &C6502::SBC, AM_IMMEDIATE, 2, true, false, 0x2 }, // SBC - Immediate
   { 0xEA, "NOP", &C6502::NOP, AM_IMPLIED, 2, true, false, 0x2 }, // NOP
   { 0xEB, "SBC", &C6502::SBC, AM_IMMEDIATE, 2, false, false, 0x2 }, // SBC - Immediate (undocumented)
   { 0xEC, "CPX", &C6502::CPX, AM_ABSOLUTE, 4, true, false, 0x8 }, // CPX - Absolute
   { 0xED, "SBC", &C6502::SBC, AM_ABSOLUTE, 4, true, false, 0x8 }, // SBC - Absolute
   { 0xEE, "INC", &C6502::INC, AM_ABSOLUTE, 6, true, false, 0x20 }, // INC - Absolute
   { 0xEF, "INS", &C6502::INS, AM_ABSOLUTE, 6, false, false, 0x20 }, // INS - Absolute (undocumented)
   { 0xF0, "BEQ", &C6502::BEQ, AM_RELATIVE, 2, true, false, 0xA }, // BEQ
   { 0xF1, "SBC", &C6502::SBC, AM_POSTINDEXED_INDIRECT, 5, true, false, 0x10 }, // SBC - (Indirect),Y
   { 0xF2, "KIL", &C6502::KIL, AM_IMPLIED, 0, false, false, 0x0 }, // KIL - Implied (processor lock up!)
   { 0xF3, "INS", &C6502::INS, AM_POSTINDEXED_INDIRECT, 8, false, true, 0x80 }, // INS - (Indirect),Y (undocumented)
   { 0xF4, "DOP", &C6502::DOP, AM_ZEROPAGE_INDEXED_X, 4, false, false, 0x8 }, // DOP (undocumented)
   { 0xF5, "SBC", &C6502::SBC, AM_ZEROPAGE_INDEXED_X, 4, true, false, 0x8 }, // SBC - Zero Page,X
   { 0xF6, "INC", &C6502::INC, AM_ZEROPAGE_INDEXED_X, 6, true, false, 0x20 }, // INC - Zero Page,X
   { 0xF7, "INS", &C6502::INS, AM_ZEROPAGE_INDEXED_X, 6, false, false, 0x20 }, // INS - Zero Page,X (undocumented)
   { 0xF8, "SED", &C6502::SED, AM_IMPLIED, 2, true, false, 0x2 }, // SED
   { 0xF9, "SBC", &C6502::SBC, AM_ABSOLUTE_INDEXED_Y, 4, true, false, 0x8 }, // SBC - Absolute,Y
   { 0xFA, "NOP", &C6502::NOP, AM_IMPLIED, 2, false, false, 0x2 }, // NOP (undocumented)
   { 0xFB, "INS", &C6502::INS, AM_ABSOLUTE_INDEXED_Y, 7, false, true, 0x40 }, // INS - Absolute,Y (undocumented)
   { 0xFC, "TOP", &C6502::TOP, AM_ABSOLUTE_INDEXED_X, 4, false, false, 0x8 }, // TOP (undocumented)
   { 0xFD, "SBC", &C6502::SBC, AM_ABSOLUTE_INDEXED_X, 4, true, false, 0x8 }, // SBC - Absolute,X
   { 0xFE, "INC", &C6502::INC, AM_ABSOLUTE_INDEXED_X, 7, true, true, 0x40 }, // INC - Absolute,X
   { 0xFF, "INS", &C6502::INS, AM_ABSOLUTE_INDEXED_X, 7, false, true, 0x40 }  // INS - Absolute,X (undocumented)
};

C6502::C6502(CNES *pNES)
   : m_apu(new CAPU()),
     m_nes(pNES),
     m_6502memory(CMEMORY(0x0000, MEM_2KB, 1, 4))
{
   m_killed = false;              // KIL opcode not executed.
   m_breakOnKIL = false;          // IDE sets this for us.
   m_irqAsserted = false;
   m_instrCycle = 0;
   m_irqPending = false;
   m_nmiAsserted = false;
   m_nmiPending = false;
   m_openBusData = 0x00;
   m_a = 0x00;
   m_x = 0x00;
   m_y = 0x00;
   m_f = FLAG_MISC;
   m_pc = VECTOR_RESET;
   m_pcSync = VECTOR_RESET;
   m_pcSyncSet = true;
   m_ea = 0;
   m_pcGoto = 0xFFFFFFFF;
   m_sp = 0x00;

   m_cycles = 0;
   m_curCycles = 0;

   m_writeDmaAddr = 0x0000;
   m_writeDmaCounter = 0;
   m_readDmaAddr = 0x0000;
   m_dmaRequest = -1;
   m_readDmaCounter = 0;

   data = NULL;
   pOpcodeStruct = NULL;
   m_write = false;
   m_phase = 0;

   pDisassemblySample = NULL;

   m_marker = new CMarker;
}

C6502::~C6502()
{
   delete m_marker;
}

void C6502::EMULATE ( int32_t cycles )
{
   bool doCycle;
   bool nmiPending = false;

   m_curCycles += cycles;

   if ( !m_killed )
   {
      do
      {
         if ( m_curCycles > 0 )
         {
            doCycle = DMA();
            if ( doCycle )
            {
               if ( m_phase == 0 )
               {
                  // Indicate opcode fetch...
                  m_instrCycle = 0;

                  // Keep track of synchronization points so IDE can display properly.
                  m_pcSyncSet = true;
                  m_pcSync = m_pc;

                  // Break 'flag' is cleared after BRK finishes.
                  cB();

                  // Fetch
                  nmiPending = m_nmiPending;
                  (*opcodeData) = FETCH ();

                  NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUExecution, (*opcodeData) );

                  // Save the pointer to where to put the disassembly of
                  // the current opcode now.  This might be the last fetch
                  // for an instruction and the disassembly should be placed there.
                  pDisassemblySample = NES()->TRACER()->GetLastCPUSample ();

                  // Check flags breakpoint.  Do it here instead of everywhere flags are
                  // changed so as to limit the number of calls to check the breakpoint.
                  NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUState,CPU_F);

                  // Check for KIL opcodes...
                  // Get information about current opcode...
                  pOpcodeStruct = m_6502opcode+(*opcodeData);
                  if ( pOpcodeStruct->pFn == &C6502::KIL )
                  {
                     // KIL opcodes halt PC dead!  Force break if desired...
                     if ( m_breakOnKIL )
                     {
                        NES()->FORCEBREAKPOINT ();
                     }
                  }

                  if ( rPC() == m_pcGoto )
                  {
                     NES()->STEPCPUBREAKPOINT();
                     m_pcGoto = 0xFFFFFFFF;
                  }

                  // If we fetched a BRK we might need to do some special stuff...
                  if ( (!(nmiPending || m_irqPending)) && (*opcodeData) == BRK_IMPLIED )
                  {
                     INCPC ();
                     sB ();
                  }

                  // Inject BRK if IRQ or NMI are pending...
                  if ( (m_irqPending && (!rI())) || nmiPending )
                  {
                     (*opcodeData) = BRK_IMPLIED;
                  }

                  if ( (*opcodeData) != BRK_IMPLIED )
                  {
                     INCPC ();
                  }

                  // Get information about current opcode...
                  pOpcodeStruct = m_6502opcode+(*opcodeData);
                  opcodeSize = (*(opcode_size+(pOpcodeStruct->amode)));

                  // Set up class data so we don't need to pass it down to each func...
                  amode = pOpcodeStruct->amode;
                  data = opcodeData+1;

                  // Go to next phase...
                  m_phase++;
               }
               else if ( m_phase == 1 )
               {
                  // Check for dummy-read needed for single-byte instructions...
                  if ( opcodeSize == 1 )
                  {
                     // Perform additional fetch...
                     (*(opcodeData+1)) = EXTRAFETCH ();

                     if ( rPC() == m_pcGoto )
                     {
                        NES()->STEPCPUBREAKPOINT();
                        m_pcGoto = 0xFFFFFFFF;
                     }

                     if ( rB() )
                     {
                        INCPC();
                     }

                     // Cause instruction execution...
                     m_phase = -1;
                  }
                  else
                  {
                     (*(opcodeData+1)) = FETCH ();

                     if ( rPC() == m_pcGoto )
                     {
                        NES()->STEPCPUBREAKPOINT();
                        m_pcGoto = 0xFFFFFFFF;
                     }

                     INCPC ();

                     if ( opcodeSize == 2 )
                     {
                        // Cause instruction execution...
                        m_phase = -1;
                     }
                     else
                     {
                        // JSR doesn't do the third byte fetch yet.
                        if ( (*opcodeData) == JSR_ABSOLUTE )
                        {
                           // Cause instruction execution...
                           m_phase = -1;
                        }
                        else
                        {
                           // Go to next phase...
                           m_phase++;
                        }
                     }
                  }
               }
               else if ( m_phase == 2 )
               {
                  (*(opcodeData+2)) = FETCH ();

                  if ( rPC() == m_pcGoto )
                  {
                     NES()->STEPCPUBREAKPOINT();
                     m_pcGoto = 0xFFFFFFFF;
                  }

                  INCPC ();

                  // Cause instruction execution...
                  m_phase = -1;
               }
               else if (  m_phase == -1 )
               {
                  if ( nesIsDebuggable )
                  {
                     // Update Tracer
                     NES()->TRACER()->SetRegisters ( pDisassemblySample, rA(), rX(), rY(), rSP(), rF() );
                  }

                  if ( rPC() == m_pcGoto )
                  {
                     NES()->STEPCPUBREAKPOINT();
                     m_pcGoto = 0xFFFFFFFF;
                  }

                  // Execute
                  (this->*pOpcodeStruct->pFn)();

                  if ( nesIsDebuggable )
                  {
                     // Update Tracer
                     NES()->TRACER()->SetDisassembly ( pDisassemblySample, opcodeData );

                     // Check for undocumented breakpoint...
                     if ( pOpcodeStruct->documented )
                     {
                        NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUEvent, (*opcodeData), CPU_EVENT_EXECUTE_EXACT );
                     }
                     else
                     {
                        NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUEvent, 0, CPU_EVENT_UNDOCUMENTED );
                        NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUEvent, (*opcodeData), CPU_EVENT_UNDOCUMENTED_EXACT );
                     }
                  }

                  // Go back to fetch phases...
                  if ( ((*opcodeData) != BRK_IMPLIED) ||
                       (((*opcodeData) == BRK_IMPLIED) && (m_instrCycle == 7)) )
                  {
                     m_phase = 0;
                  }
               }
            }
         }
      }
      while ( (!m_killed) && (m_curCycles > 0) );
   }
   else
   {
      // Run APU for cycles...
      while ( cycles )
      {
         APU()->EMULATE ();
         cycles--;
      }
   }
}

void C6502::APUDMAREQ ( uint16_t addr )
{
   m_dmaRequest = 3;
   m_readDmaAddr = addr;
}

void C6502::ADVANCE ( bool stealing )
{
   // If this cycle is being stolen, don't check whether IRQ/NMI needs to happen.
   if ( !stealing )
   {
      // NMI is ignored if it is asserted in the 6th or 7th cycles of a BRK.
      if ( ((*opcodeData) != BRK_IMPLIED) ||
           (((*opcodeData) == BRK_IMPLIED) &&
           (m_instrCycle >= 0) &&
           (m_instrCycle < 5)) )
      {
         // NMI is edge-sensitive thus it will only be marked as pending
         // if an assertion occurred recently.  Once marked as pending it
         // is cleared as not asserted to prevent multiple NMIs on a single
         // edge.
         if ( m_nmiAsserted )
         {
            // NMI is now pending to the CPU.
            m_nmiPending = true;
            m_nmiAsserted = false;
         }
      }

      if ( pOpcodeStruct && (pOpcodeStruct->checkInterruptCycleMap&(1<<m_instrCycle)) )
      {
         // IRQ is level-sensitive thus it will always be marked as
         // "pending" if it has been asserted and not negated by the peripheral (and the
         // special cases described below are considered).
         if ( m_irqAsserted && !rI() )
         {
            // IRQ is now pending to the CPU.
            m_irqPending = true;
         }
      }
   }

   // Tell mappers that look at CPU cycles that a CPU cycle has whisked by...
   NES()->CART()->SYNCCPU();

   // Run APU for one cycle...
   APU()->EMULATE ();

   // Increment running cycle counters...
   m_cycles++;

   // If stealing, don't move instruction cycle forward.
   if ( !stealing )
   {
      m_instrCycle++;
   }

   // Decrement cycles available counter...
   m_curCycles--;
}

bool C6502::DMA( void )
{
   bool doCycle = true;
   static uint8_t databuf;

   // If the DMC DMA request is active it means the CPU was writing when
   // the DMC DMA controller went active.  We need to assert RDY on the next
   // CPU read cycle.  Check if that is now.
   if ( !m_dmaRequest )
   {
      if ( !m_write )
      {
         if ( m_writeDmaCounter )
         {
            m_readDmaCounter = 2;
         }
         else
         {
            m_readDmaCounter = 4;
         }
         if ( !(_CYCLES()&1) )
         {
            m_readDmaCounter++;
         }
         m_dmaRequest--;
      }
   }
   if ( m_dmaRequest > 0 )
   {
      m_dmaRequest--;
   }

   // Run the DMA controller cycles if necessary.
   // Is this a read-beat?
   if ( (_CYCLES()&1) )
   {
      // APU DMC DMA happens even if sprite DMA is occurring.
      // If we're in the waiting period before DMA, wait.
      if ( m_readDmaCounter > 4 )
      {
         m_readDmaCounter--;
         goto done;
      }
      // If we're in the DMC DMA RDY-phase, just steal a cycle if there's
      // no sprite DMA already stealing them.
      if ( m_readDmaCounter > 2 )
      {
         m_readDmaCounter--;
         if ( !m_writeDmaCounter )
         {
            STEAL ( 1, eNESSource_APU );
            doCycle = false;
            goto done;
         }
         else
         {
            doCycle = false;
            goto done;
         }
      }
      // If we're ready to do the DMC DMA read, do it.
      if ( m_readDmaCounter == 2 )
      {
         APU()->DMASAMPLE ( DMA(m_readDmaAddr) );
         m_readDmaCounter--;
         doCycle = false;

         if ( nesIsDebuggable )
         {
            // Check for APU DMC channel DMA breakpoint event...
            NES()->CHECKBREAKPOINT(eBreakInAPU,eBreakOnAPUEvent,0,APU_EVENT_DMC_DMA);
         }

         goto done;
      }
      // If we're in the sprite DMA RDY-phase, just steal a cycle.
      if ( m_writeDmaCounter > 512 )
      {
         STEAL ( 1, eNESSource_PPU );
         m_writeDmaCounter--;
         doCycle = false;
         goto done;
      }
      // If we're ready to do the sprite DMA read, do it.
      if ( m_writeDmaCounter )
      {
         databuf = DMA(m_writeDmaAddr|(((512-m_writeDmaCounter)>>1)&0xFF));

         if ( nesIsDebuggable )
         {
            // Check for PPU cycle breakpoint...
            NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, (512-m_writeDmaCounter)>>1, PPU_EVENT_SPRITE_DMA );
         }

         m_writeDmaCounter--;
         doCycle = false;
         goto done;
      }
   }
   else
   {
      // If we're in the waiting period before DMA, wait.
      if ( m_readDmaCounter > 4 )
      {
         m_readDmaCounter--;
         goto done;
      }
      // If we're in the DMC DMA RDY-phase, just steal a cycle if there's
      // no sprite DMA already stealing them.
      if ( m_readDmaCounter > 2 )
      {
         m_readDmaCounter--;
         if ( !m_writeDmaCounter )
         {
            STEAL ( 1, eNESSource_APU );
            doCycle = false;
            goto done;
         }
         else
         {
            doCycle = false;
            goto done;
         }
      }
      // If APU DMC DMA occurred on the read-beat, skip this
      // write-beat if sprite DMA is in progress.
      if ( m_readDmaCounter == 1 )
      {
         if ( m_writeDmaCounter )
         {
            m_readDmaCounter--;
            STEAL(rPC(),eNESSource_APU); // Put CPU on bus.
            doCycle = false;
            goto done;
         }
         else
         {
            m_readDmaCounter--;
            goto done;
         }
      }
      // If we're in the sprite DMA RDY-phase, just steal a cycle.
      if ( m_writeDmaCounter > 512 )
      {
         STEAL ( 1, eNESSource_PPU );
         m_writeDmaCounter--;
         doCycle = false;
         goto done;
      }
      // If we're ready to do the sprite DMA write, do it.
      if ( m_writeDmaCounter )
      {
         DMA ( (m_writeDmaAddr)|(((512-m_writeDmaCounter)>>1)&0xFF),
               OAMDATA,
               databuf );
         m_writeDmaCounter--;
         doCycle = false;
         goto done;
      }
   }
   done:
#if 0

   if ( (m_readDmaCounter > 1) && (!m_writeDmaCounter) )
   {
      STEAL ( 1, eNESSource_APU );
      m_readDmaCounter--;
      doCycle = false;
   }
   else if ( m_readDmaCounter > 1 )
   {
      m_readDmaCounter--;
      doCycle = false;
   }
   else if ( m_writeDmaCounter > 512 )
   {
      STEAL ( 1, eNESSource_PPU );
      m_writeDmaCounter--;
      doCycle = false;
   }
   else if ( m_readDmaCounter == 1 && (!m_writeDmaCounter) )
   {
      APU()->DMASAMPLE ( DMA(m_readDmaAddr) );
      m_readDmaCounter--;
      doCycle = false;
   }
   else if ( m_readDmaCounter == 1 )
   {
      if ( _CYCLES()&1 )
      {
         APU()->DMASAMPLE ( DMA(m_readDmaAddr) );
         doCycle = false;
      }
      else
      {
         STEAL ( 1, eNESSource_APU );
         m_readDmaCounter--;
         doCycle = false;
      }
   }
   else if ( m_writeDmaCounter && (m_readDmaCounter != 1) )
   {
      // If this is a read-beat, do the read.
      if ( !(m_writeDmaCounter&0x01) )
      {
         databuf = DMA(m_writeDmaAddr|(((512-m_writeDmaCounter)>>1)&0xFF));
         doCycle = false;
      }
      // If this is a write-beat, do the write.
      else
      {
         DMA ( (m_writeDmaAddr)|(((512-m_writeDmaCounter)>>1)&0xFF),
               OAMDATA,
               databuf );
         doCycle = false;
      }
      m_writeDmaCounter--;
   }
#endif

#if 0
   // Steal a CPU cycle if it is the appropriate time to do so...
   // If the CPU is currently writing, we don't bother it unless we're
   // already bothering it.
   if ( (m_readDmaCounter > 1) && ((m_writeDmaCounter == 0) || (!(m_writeDmaCounter&1))) )
   {
      if ( m_write )
      {
         m_readDmaCounter--;
         return true;
      }
      else
      {
         STEAL ( 1, eNESSource_APU );
         m_readDmaCounter--;
         doCycle = false;
      }
   }
   if ( m_writeDmaCounter > 512 )
   {
      STEAL ( 1, eNESSource_PPU );
      m_writeDmaCounter--;
      doCycle = false;
   }

   // Perform DMA if necessary.
   if ( doCycle )
   {
      if ( (m_readDmaCounter == 1) && ((m_writeDmaCounter == 0) || (!(m_writeDmaCounter&1))) )
      {
         APU()->DMASAMPLE ( DMA(m_readDmaAddr) );
         m_readDmaCounter = 0;
         doCycle = false;
      }
      if ( m_writeDmaCounter && doCycle )
      {
         if ( !(m_writeDmaCounter&1) )
         {
            databuf = DMA(m_writeDmaAddr|(((512-m_writeDmaCounter)>>1)&0xFF));
            m_writeDmaCounter--;
            doCycle = false;
         }
         // If we are on a DMA cycle, do the DMA...
         else
         {
            DMA ( (m_writeDmaAddr)|(((512-m_writeDmaCounter)>>1)&0xFF),
                  OAMDATA,
                  databuf );
            m_writeDmaCounter--;
            doCycle = false;
         }
      }
   }
#endif
   return doCycle;
}

// START OPCODE HANDLERS

// ILLEGAL/UNDOCUMENTED OPCODES

void C6502::KIL ( void )
{
   m_killed = true;
   return;
}

// AAC (ANC) [ANC]
// ~~~~~~~~~~~~~~~
// AND byte with accumulator. If result is negative then carry is
// set. Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |AAC #arg   |$0B| 2 | 2
// Immediate   |AAC #arg   |$2B| 2 | 2
void C6502::ANC ( void )
{
   wA ( rA()&(*data) );
   wN ( rA()&0x80 );
   wC ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

// ASR (ASR) [ALR]
// ~~~~~~~~~~~~~~~
// AND byte with accumulator, then shift right one bit in accumu-
// lator. Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |ASR #arg   |$4B| 2 | 2
void C6502::ALR ( void )
{
   wA ( (rA()&(*data)) );
   wC ( rA()&0x01 );
   wA ( rA()>>1 );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

// ARR (ARR) [ARR]
// ~~~~~~~~~~~~~~~
// AND byte with accumulator, then rotate one bit right in accu-
// mulator and check bit 5 and 6:
// If both bits are 1: set C, clear V.
// If both bits are 0: clear C and V.
// If only bit 5 is 1: set V, clear C.
// If only bit 6 is 1: set C and V.
// Status flags: N,V,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |ARR #arg   |$6B| 2 | 2
void C6502::ARR ( void )
{
   wA ( (rC()<<7)|((rA()>>1)&((*data)>>1)) );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   if ( (rA()&0x60) == 0x60 )
   {
      sC();
      cV();
   }
   else if ( (rA()&0x60) == 0x00 )
   {
      cC();
      cV();
   }
   else if ( (rA()&0x60) == 0x20 )
   {
      cC();
      sV();
   }
   else if ( (rA()&0x60) == 0x40 )
   {
      sC();
      sV();
   }

   return;
}

// XAA (ANE) [XAA]
// ~~~~~~~~~~~~~~~
// Exact operation unknown. Read the referenced documents for
// more information and observations.
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |XAA #arg   |$8B| 2 | 2
void C6502::XAA ( void )
{
   // From kevtris on #nesdev:
   // 8'h8b : Q <= ({Reg_X[7:5], (Reg_A[4] & Reg_X[4]), Reg_X[3:1], (Reg_A[0] & Reg_X[0])} & DB); // 8B ANE #

   return;
}

// AXA (SHA) [AXA]
// ~~~~~~~~~~~~~~~
// AND X register with accumulator then AND result with 7 and
// store in memory. Status flags: -
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Absolute,Y  |AXA arg,Y  |$9F| 3 | 5
// (Indirect),Y|AXA arg    |$93| 2 | 6
void C6502::AXA ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = (rX()&rA())&7;
   MEM ( addr, val );

   return;
}

// XAS (SHS) [TAS]
// ~~~~~~~~~~~~~~~
// AND X register with accumulator and store result in stack
// pointer, then AND stack pointer with the high byte of the
// target address of the argument + 1. Store result in memory.
//
// S = X AND A, M = S AND HIGH(arg) + 1
//
// Status flags: -
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Absolute,Y  |XAS arg,Y  |$9B| 3 | 5
void C6502::TAS ( void )
{
   uint16_t addr;
   uint8_t  val;

   wSP ( rX()&rA() );
   addr = MAKEADDR ( amode, data );
   val = (rSP()&((*(data+1))+1));
   MEM ( addr, val );

   return;
}

// SYA (SHY) [SAY]
// ~~~~~~~~~~~~~~~
// AND Y register with the high byte of the target address of the
// argument + 1. Store the result in memory.
//
// M = Y AND HIGH(arg) + 1
//
// Status flags: -
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Absolute,X  |SYA arg,X  |$9C| 3 | 5
void C6502::SAY ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = (rY()&((addr>>8)+1));
   addr &= 0x00FF;
   addr |= (val<<8);
   MEM ( addr, val );

   return;
}

// SXA (SHX) [XAS]
// ~~~~~~~~~~~~~~~
// AND X register with the high byte of the target address of the
// argument + 1. Store the result in memory.
//
// M = X AND HIGH(arg) + 1
//
// Status flags: -
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Absolute,Y  |SXA arg,Y  |$9E| 3 | 5
void C6502::XAS ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = (rX()&((addr>>8)+1));
   addr &= 0x00FF;
   addr |= (val<<8);
   MEM ( addr, val );

   return;
}

// ATX (LXA) [OAL]
// ~~~~~~~~~~~~~~~
// AND byte with accumulator, then transfer accumulator to X
// register. Status flags: N,Z
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |ATX #arg   |$AB| 2 | 2
void C6502::OAL ( void )
{
   wA ( (*data) );
   wX ( rA() );
   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

// LAR (LAE) [LAS]
// ~~~~~~~~~~~~~~~
// AND memory with stack pointer, transfer result to accumulator,
// X register and stack pointer.
// Status flags: N,Z
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Absolute,Y  |LAR arg,Y  |$BB| 3 | 4 *
void C6502::LAS ( void )
{
   uint16_t addr;

   addr = MAKEADDR ( amode, data );
   wA ( rSP()&MEM(addr) );
   wX ( rA() );
   wSP ( rA() );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

// AXS (SBX) [SAX]
// ~~~~~~~~~~~~~~~
// AND X register with accumulator and store result in X regis-
// ter, then subtract byte from X register (without borrow).
// Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Immediate   |AXS #arg   |$CB| 2 | 2
void C6502::SAX ( void )
{
   int16_t val;

   wX ( rA()&rX() );
   val = rX()-(*data);
   wX ( val&0xFF );
   wN ( rX()&0x80 );
   wZ ( !rX() );
   wC ( !(val<0) );

   return;
}

// SLO (SLO) [ASO]
// ~~~~~~~~~~~~~~~
// Shift left one bit in memory, then OR accumulator with memory.
//
// Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |SLO arg    |$07| 2 | 5
// Zero Page,X |SLO arg,X  |$17| 2 | 6
// Absolute    |SLO arg    |$0F| 3 | 6
// Absolute,X  |SLO arg,X  |$1F| 3 | 7
// Absolute,Y  |SLO arg,Y  |$1B| 3 | 7
// (Indirect,X)|SLO (arg,X)|$03| 2 | 8
// (Indirect),Y|SLO (arg),Y|$13| 2 | 8
void C6502::ASO ( void )
{
   uint16_t addr;
   uint16_t val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val <<= 1;
   MEM ( addr, (uint8_t)val );
   wC ( val&0x100 );
   val &= 0xFF;
   wA ( rA()|val );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE();

   return;
}

// AAX (SAX) [AXS]
// ~~~~~~~~~~~~~~~
// AND X register with accumulator and store result in memory.
// Status flags: N,Z
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |AAX arg    |$87| 2 | 3
// Zero Page,Y |AAX arg,Y  |$97| 2 | 4
// (Indirect,X)|AAX (arg,X)|$83| 2 | 6
// Absolute    |AAX arg    |$8F| 3 | 4
void C6502::AXS ( void )
{
   uint16_t addr;

   addr = MAKEADDR ( amode, data );

   MEM ( addr, rA()&rX() );

   return;
}

// LEGAL/DOCUMENTED OPCODES

//  ORA                 ORA "OR" memory with accumulator                  ORA
//
//  Operation: A V M -> A                                 N Z C I D V
//                                                        / / _ _ _ _
//                               (Ref: 2.2.3.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   ORA #Oper           |    09   |    2    |    2     |
//  |  Zero Page     |   ORA Oper            |    05   |    2    |    3     |
//  |  Zero Page,X   |   ORA Oper,X          |    15   |    2    |    4     |
//  |  Absolute      |   ORA Oper            |    0D   |    3    |    4     |
//  |  Absolute,X    |   ORA Oper,X          |    1D   |    3    |    4*    |
//  |  Absolute,Y    |   ORA Oper,Y          |    19   |    3    |    4*    |
//  |  (Indirect,X)  |   ORA (Oper,X)        |    01   |    2    |    6     |
//  |  (Indirect),Y  |   ORA (Oper),Y        |    11   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 on page crossing
void C6502::ORA ( void )
{
   uint16_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wA ( rA()|(*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wA ( rA()|MEM(addr) );
   }

   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  ASL          ASL Shift Left One Bit (Memory or Accumulator)           ASL
//                   +-+-+-+-+-+-+-+-+
//  Operation:  C <- |7|6|5|4|3|2|1|0| <- 0
//                   +-+-+-+-+-+-+-+-+                    N Z C I D V
//                                                        / / / _ _ _
//                                 (Ref: 10.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Accumulator   |   ASL A               |    0A   |    1    |    2     |
//  |  Zero Page     |   ASL Oper            |    06   |    2    |    5     |
//  |  Zero Page,X   |   ASL Oper,X          |    16   |    2    |    6     |
//  |  Absolute      |   ASL Oper            |    0E   |    3    |    6     |
//  |  Absolute, X   |   ASL Oper,X          |    1E   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::ASL ( void )
{
   uint16_t addr = 0x0000;
   uint16_t val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );

      // dummy write
      MEM ( addr, val );
   }

   val <<= 1;

   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (uint8_t)val );
   }
   else
   {
      MEM ( addr, (uint8_t)val );
   }

   wC ( val&0x100 );
   wN ( val&0x80 );
   wZ ( !(val&0xFF) );

   return;
}

//  PHP                 PHP Push processor status on stack                PHP
//
//  Operation:  P toS                                     N Z C I D V
//                                                        _ _ _ _ _ _
//                                 (Ref: 8.11)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   PHP                 |    08   |    1    |    3     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::PHP ( void )
{
   sB();
   PUSH ( rF() );

   return;
}

//  BPL                     BPL Branch on result plus                     BPL
//
//  Operation:  Branch on N = 0                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BPL Oper            |    10   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 2 if branch occurs to different page.
void C6502::BPL ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( !rN() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  CLC                       CLC Clear carry flag                        CLC
//
//  Operation:  0 -> C                                    N Z C I D V
//                                                        _ _ 0 _ _ _
//                                (Ref: 3.0.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   CLC                 |    18   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CLC ( void )
{
   cC ();

   return;
}

//  JSR          JSR Jump to new location saving return address           JSR
//
//  Operation:  PC + 2 toS, (PC + 1) -> PCL               N Z C I D V
//                          (PC + 2) -> PCH               _ _ _ _ _ _
//                                 (Ref: 8.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Absolute      |   JSR Oper            |    20   |    3    |    6     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::JSR ( void )
{
   // Synchronize CPU and APU.
   MEM ( GETSTACKADDR() );

   PUSH ( GETHI8(rPC()) );
   PUSH ( GETLO8(rPC()) );

   *(data+1) = FETCH ();

   wPC ( MAKE16(GETUNSIGNED8(data,0),GETUNSIGNED8(data,1)) );

   if ( rPC() == m_pcGoto )
   {
      NES()->STEPCPUBREAKPOINT();
      m_pcGoto = 0xFFFFFFFF;
   }

   return;
}

//  AND                  "AND" memory with accumulator                    AND
//
//  Operation:  A /\ M -> A                               N Z C I D V
//                                                        / / _ _ _ _
//                               (Ref: 2.2.3.0)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   AND #Oper           |    29   |    2    |    2     |
//  |  Zero Page     |   AND Oper            |    25   |    2    |    3     |
//  |  Zero Page,X   |   AND Oper,X          |    35   |    2    |    4     |
//  |  Absolute      |   AND Oper            |    2D   |    3    |    4     |
//  |  Absolute,X    |   AND Oper,X          |    3D   |    3    |    4*    |
//  |  Absolute,Y    |   AND Oper,Y          |    39   |    3    |    4*    |
//  |  (Indirect,X)  |   AND (Oper,X)        |    21   |    2    |    6     |
//  |  (Indirect,Y)  |   AND (Oper),Y        |    31   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if page boundary is crossed.
void C6502::AND ( void )
{
   uint16_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wA ( rA()&(*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wA ( rA()&MEM(addr) );
   }

   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  BIT             BIT Test bits in memory with accumulator              BIT
//
//  Operation:  A /\ M, M7 -> N, M6 -> V
//
//  Bit 6 and 7 are transferred to the status register.   N Z C I D V
//  If the result of A /\ M is zero then Z = 1, otherwise M7/ _ _ _ M6
//  Z = 0
//                               (Ref: 4.2.1.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   BIT Oper            |    24   |    2    |    3     |
//  |  Absolute      |   BIT Oper            |    2C   |    3    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::BIT ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );

   val = MEM ( addr );

   wN ( val&0x80 );
   wV ( val&0x40 );
   val &= rA ();
   wZ ( !val );

   return;
}

//  ROL          ROL Rotate one bit left (memory or accumulator)          ROL
//
//               +------------------------------+
//               |         M or A               |
//               |   +-+-+-+-+-+-+-+-+    +-+   |
//  Operation:   +-< |7|6|5|4|3|2|1|0| <- |C| <-+         N Z C I D V
//                   +-+-+-+-+-+-+-+-+    +-+             / / / _ _ _
//                                 (Ref: 10.3)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Accumulator   |   ROL A               |    2A   |    1    |    2     |
//  |  Zero Page     |   ROL Oper            |    26   |    2    |    5     |
//  |  Zero Page,X   |   ROL Oper,X          |    36   |    2    |    6     |
//  |  Absolute      |   ROL Oper            |    2E   |    3    |    6     |
//  |  Absolute,X    |   ROL Oper,X          |    3E   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::ROL ( void )
{
   uint16_t addr = 0x0000;
   uint16_t val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );

      // dummy write
      MEM ( addr, val );
   }

   val <<= 1;
   val |= rC();
   wC ( val&0x100 );
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (uint8_t)val );
   }
   else
   {
      MEM ( addr, (uint8_t)val );
   }

   return;
}

//  PLP               PLP Pull processor status from stack                PLP
//
//  Operation:  P fromS                                   N Z C I D V
//                                                         From Stack
//                                 (Ref: 8.12)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   PLP                 |    28   |    1    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::PLP ( void )
{
   // Synchronize CPU and APU...
   MEM ( GETSTACKADDR() );

   wF ( POP() );

   return;
}

//  BMI                    BMI Branch on result minus                     BMI
//
//  Operation:  Branch on N = 1                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BMI Oper            |    30   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 1 if branch occurs to different page.
void C6502::BMI ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( rN() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  SEC                        SEC Set carry flag                         SEC
//
//  Operation:  1 -> C                                    N Z C I D V
//                                                        _ _ 1 _ _ _
//                                (Ref: 3.0.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   SEC                 |    38   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::SEC ( void )
{
   sC ();

   return;
}

//  RTI                    RTI Return from interrupt                      RTI
//                                                        N Z C I D V
//  Operation:  P fromS PC fromS                           From Stack
//                                 (Ref: 9.6)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   RTI                 |    4D   |    1    |    6     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::RTI ( void )
{
   uint8_t pclo;
   uint8_t pchi;
   uint8_t f;

   // Synchronize CPU and APU...
   MEM ( GETSTACKADDR() );

   f = POP();
   pclo = POP ();
   pchi = POP ();
   wPC ( MAKE16(pclo,pchi) );

   wF ( f );

   m_irqPending = false;

   if ( rPC() == m_pcGoto )
   {
      NES()->STEPCPUBREAKPOINT();
      m_pcGoto = 0xFFFFFFFF;
   }

   return;
}

//  EOR            EOR "Exclusive-Or" memory with accumulator             EOR
//
//  Operation:  A EOR M -> A                              N Z C I D V
//                                                        / / _ _ _ _
//                               (Ref: 2.2.3.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   EOR #Oper           |    49   |    2    |    2     |
//  |  Zero Page     |   EOR Oper            |    45   |    2    |    3     |
//  |  Zero Page,X   |   EOR Oper,X          |    55   |    2    |    4     |
//  |  Absolute      |   EOR Oper            |    4D   |    3    |    4     |
//  |  Absolute,X    |   EOR Oper,X          |    5D   |    3    |    4*    |
//  |  Absolute,Y    |   EOR Oper,Y          |    59   |    3    |    4*    |
//  |  (Indirect,X)  |   EOR (Oper,X)        |    41   |    2    |    6     |
//  |  (Indirect),Y  |   EOR (Oper),Y        |    51   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if page boundary is crossed.
void C6502::EOR ( void )
{
   uint16_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wA ( rA()^(*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wA ( rA()^MEM(addr) );
   }

   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

// SRE (SRE) [LSE]
// ~~~~~~~~~~~~~~~
// Shift right one bit in memory, then EOR accumulator with
// memory. Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |SRE arg    |$47| 2 | 5
// Zero Page,X |SRE arg,X  |$57| 2 | 6
// Absolute    |SRE arg    |$4F| 3 | 6
// Absolute,X  |SRE arg,X  |$5F| 3 | 7
// Absolute,Y  |SRE arg,Y  |$5B| 3 | 7
// (Indirect,X)|SRE (arg,X)|$43| 2 | 8
// (Indirect),Y|SRE (arg),Y|$53| 2 | 8
void C6502::LSE ( void )
{
   uint16_t addr;
   uint16_t val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   MEM ( addr, (uint8_t)val );
   wA ( rA()^val );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

//  LSR          LSR Shift right one bit (memory or accumulator)          LSR
//
//                   +-+-+-+-+-+-+-+-+
//  Operation:  0 -> |7|6|5|4|3|2|1|0| -> C               N Z C I D V
//                   +-+-+-+-+-+-+-+-+                    0 / / _ _ _
//                                 (Ref: 10.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Accumulator   |   LSR A               |    4A   |    1    |    2     |
//  |  Zero Page     |   LSR Oper            |    46   |    2    |    5     |
//  |  Zero Page,X   |   LSR Oper,X          |    56   |    2    |    6     |
//  |  Absolute      |   LSR Oper            |    4E   |    3    |    6     |
//  |  Absolute,X    |   LSR Oper,X          |    5E   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::LSR ( void )
{
   uint16_t addr = 0x0000;
   uint16_t val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );

      // dummy write
      MEM ( addr, val );
   }

   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   cN ();
   wZ ( !val );

   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (uint8_t)val );
   }
   else
   {
      MEM ( addr, (uint8_t)val );
   }

   return;
}

//  PHA                   PHA Push accumulator on stack                   PHA
//
//  Operation:  A toS                                     N Z C I D V
//                                                        _ _ _ _ _ _
//                                 (Ref: 8.5)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   PHA                 |    48   |    1    |    3     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::PHA ( void )
{
   PUSH ( rA() );

   return;
}

//  JMP                     JMP Jump to new location                      JMP
//
//  Operation:  (PC + 1) -> PCL                           N Z C I D V
//              (PC + 2) -> PCH   (Ref: 4.0.2)            _ _ _ _ _ _
//                                (Ref: 9.8.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Absolute      |   JMP Oper            |    4C   |    3    |    3     |
//  |  Indirect      |   JMP (Oper)          |    6C   |    3    |    5     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::JMP ( void )
{
   uint32_t addr = MAKE16(GETUNSIGNED8(data,0),GETUNSIGNED8(data,1));

   if ( amode == AM_ABSOLUTE )
   {
      wPC ( addr );
   }
   else
   {
      if ( (addr&0xFF) == 0xFF )
      {
         wPC ( MAKE16(MEM(addr),MEM(addr&0xFF00)) );
      }
      else
      {
         wPC ( MAKE16(MEM(addr),MEM(addr+1)) );
      }
   }

   if ( rPC() == m_pcGoto )
   {
      NES()->STEPCPUBREAKPOINT();
      m_pcGoto = 0xFFFFFFFF;
   }

   return;
}

//  BVC                   BVC Branch on overflow clear                    BVC
//
//  Operation:  Branch on V = 0                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.8)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BVC Oper            |    50   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 2 if branch occurs to different page.
void C6502::BVC ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( !rV() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  CLI                  CLI Clear interrupt disable bit                  CLI
//
//  Operation: 0 -> I                                     N Z C I D V
//                                                        _ _ _ 0 _ _
//                                (Ref: 3.2.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   CLI                 |    58   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CLI ( void )
{
   cI ();

   return;
}

//  RTS                    RTS Return from subroutine                     RTS
//                                                        N Z C I D V
//  Operation:  PC fromS, PC + 1 -> PC                    _ _ _ _ _ _
//                                 (Ref: 8.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   RTS                 |    60   |    1    |    6     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::RTS ( void )
{
   uint8_t pclo;
   uint8_t pchi;

   // Synchronize CPU and APU...
   MEM ( GETSTACKADDR() );

   pclo = POP ();
   pchi = POP ();

   // Synchronize CPU and APU...
   wPC ( (MAKE16(pclo,pchi)) );
   FETCH ();
   wPC ( (MAKE16(pclo,pchi))+1 );

   if ( rPC() == m_pcGoto )
   {
      NES()->STEPCPUBREAKPOINT();
      m_pcGoto = 0xFFFFFFFF;
   }

   return;
}

//  ADC               Add memory to accumulator with carry                ADC
//
//  Operation:  A + M + C -> A, C                         N Z C I D V
//                                                        / / / _ _ /
//                                (Ref: 2.2.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   ADC #Oper           |    69   |    2    |    2     |
//  |  Zero Page     |   ADC Oper            |    65   |    2    |    3     |
//  |  Zero Page,X   |   ADC Oper,X          |    75   |    2    |    4     |
//  |  Absolute      |   ADC Oper            |    6D   |    3    |    4     |
//  |  Absolute,X    |   ADC Oper,X          |    7D   |    3    |    4*    |
//  |  Absolute,Y    |   ADC Oper,Y          |    79   |    3    |    4*    |
//  |  (Indirect,X)  |   ADC (Oper,X)        |    61   |    2    |    6     |
//  |  (Indirect),Y  |   ADC (Oper),Y        |    71   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if page boundary is crossed.
void C6502::ADC ( void )
{
   uint32_t addr;
   int16_t result;
   uint8_t val;

   if ( amode == AM_IMMEDIATE )
   {
      val = (*data);
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }

   result = rA () + val + rC ();

   wV ( !((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (uint8_t)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( result&0x100 );

   return;
}

//  ROR          ROR Rotate one bit right (memory or accumulator)         ROR
//
//               +------------------------------+
//               |                              |
//               |   +-+    +-+-+-+-+-+-+-+-+   |
//  Operation:   +-> |C| -> |7|6|5|4|3|2|1|0| >-+         N Z C I D V
//                   +-+    +-+-+-+-+-+-+-+-+             / / / _ _ _
//                                 (Ref: 10.4)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Accumulator   |   ROR A               |    6A   |    1    |    2     |
//  |  Zero Page     |   ROR Oper            |    66   |    2    |    5     |
//  |  Zero Page,X   |   ROR Oper,X          |    76   |    2    |    6     |
//  |  Absolute      |   ROR Oper            |    6E   |    3    |    6     |
//  |  Absolute,X    |   ROR Oper,X          |    7E   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
//
//    Note: ROR instruction is available on MCS650X microprocessors after
//          June, 1976.
void C6502::ROR ( void )
{
   uint32_t addr = 0x0000;
   uint16_t val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );

      // dummy write
      MEM ( addr, val );
   }

   val |= ( rC()*0x100 );
   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (uint8_t)val );
   }
   else
   {
      MEM ( addr, (uint8_t)val );
   }

   return;
}

// RLA (RLA) [RLA]
// ~~~~~~~~~~~~~~~
// Rotate one bit left in memory, then AND accumulator with
// memory. Status flags: N,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |RLA arg    |$27| 2 | 5
// Zero Page,X |RLA arg,X  |$37| 2 | 6
// Absolute    |RLA arg    |$2F| 3 | 6
// Absolute,X  |RLA arg,X  |$3F| 3 | 7
// Absolute,Y  |RLA arg,Y  |$3B| 3 | 7
// (Indirect,X)|RLA (arg,X)|$23| 2 | 8
// (Indirect),Y|RLA (arg),Y|$33| 2 | 8
void C6502::RLA ( void )
{
   uint32_t addr;
   uint16_t val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val <<= 1;
   val |= rC();
   wC ( val&0x100 );
   val &= 0xFF;
   wA ( rA()&val );
   MEM ( addr, (uint8_t)val );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

// RRA (RRA) [RRA]
// ~~~~~~~~~~~~~~~
// Rotate one bit right in memory, then add memory to accumulator
// (with carry).
//
// Status flags: N,V,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |RRA arg    |$67| 2 | 5
// Zero Page,X |RRA arg,X  |$77| 2 | 6
// Absolute    |RRA arg    |$6F| 3 | 6
// Absolute,X  |RRA arg,X  |$7F| 3 | 7
// Absolute,Y  |RRA arg,Y  |$7B| 3 | 7
// (Indirect,X)|RRA (arg,X)|$63| 2 | 8
// (Indirect),Y|RRA (arg),Y|$73| 2 | 8
void C6502::RRA ( void )
{
   uint32_t addr;
   uint16_t val;
   int16_t result;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val |= ( rC()*0x100 );
   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   MEM ( addr, (uint8_t)val );

   result = rA () + val + rC ();

   wV ( !((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (uint8_t)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( result&0x100 );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

//  PLA                 PLA Pull accumulator from stack                   PLA
//
//  Operation:  A fromS                                   N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 8.6)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   PLA                 |    68   |    1    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::PLA ( void )
{
   // Synchronize CPU and APU...
   MEM ( GETSTACKADDR() );

   wA ( POP() );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  BVS                    BVS Branch on overflow set                     BVS
//
//  Operation:  Branch on V = 1                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.7)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BVS Oper            |    70   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 2 if branch occurs to different page.
void C6502::BVS ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( rV() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  SEI                 SEI Set interrupt disable status                  SED
//                                                        N Z C I D V
//  Operation:  1 -> I                                    _ _ _ 1 _ _
//                                (Ref: 3.2.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   SEI                 |    78   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::SEI ( void )
{
   sI ();

   return;
}

//  STA                  STA Store accumulator in memory                  STA
//
//  Operation:  A -> M                                    N Z C I D V
//                                                        _ _ _ _ _ _
//                                (Ref: 2.1.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   STA Oper            |    85   |    2    |    3     |
//  |  Zero Page,X   |   STA Oper,X          |    95   |    2    |    4     |
//  |  Absolute      |   STA Oper            |    8D   |    3    |    4     |
//  |  Absolute,X    |   STA Oper,X          |    9D   |    3    |    5     |
//  |  Absolute,Y    |   STA Oper, Y         |    99   |    3    |    5     |
//  |  (Indirect,X)  |   STA (Oper,X)        |    81   |    2    |    6     |
//  |  (Indirect),Y  |   STA (Oper),Y        |    91   |    2    |    6     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::STA ( void )
{
   uint16_t addr;

   addr = MAKEADDR ( amode, data );
   MEM ( addr, rA() );

   return;
}

//  STY                    STY Store index Y in memory                    STY
//
//  Operation: Y -> M                                     N Z C I D V
//                                                        _ _ _ _ _ _
//                                 (Ref: 7.3)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   STY Oper            |    84   |    2    |    3     |
//  |  Zero Page,X   |   STY Oper,X          |    94   |    2    |    4     |
//  |  Absolute      |   STY Oper            |    8C   |    3    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::STY ( void )
{
   uint16_t addr;

   addr = MAKEADDR ( amode, data );
   MEM ( addr, rY() );

   return;
}

//  STX                    STX Store index X in memory                    STX
//
//  Operation: X -> M                                     N Z C I D V
//                                                        _ _ _ _ _ _
//                                 (Ref: 7.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   STX Oper            |    86   |    2    |    3     |
//  |  Zero Page,Y   |   STX Oper,Y          |    96   |    2    |    4     |
//  |  Absolute      |   STX Oper            |    8E   |    3    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::STX ( void )
{
   uint16_t addr;

   addr = MAKEADDR ( amode, data );
   MEM ( addr, rX() );

   return;
}

//  DEY                   DEY Decrement index Y by one                    DEY
//
//  Operation:  Y - 1 -> Y                                N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.7)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   DEY                 |    88   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::DEY ( void )
{
   wY ( rY()-1 );
   wN ( rY()&0x80 );
   wZ ( !rY() );

   return;
}

//  TXA                TXA Transfer index X to accumulator                TXA
//                                                        N Z C I D V
//  Operation:  X -> A                                    / / _ _ _ _
//                                 (Ref: 7.12)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TXA                 |    8A   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TXA ( void )
{
   wA ( rX() );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  BCC                     BCC Branch on Carry Clear                     BCC
//                                                        N Z C I D V
//  Operation:  Branch on C = 0                           _ _ _ _ _ _
//                               (Ref: 4.1.1.3)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BCC Oper            |    90   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 2 if branch occurs to different page.
void C6502::BCC ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( !rC() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  TYA                TYA Transfer index Y to accumulator                TYA
//
//  Operation:  Y -> A                                    N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.14)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TYA                 |    98   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TYA ( void )
{
   wA ( rY() );
   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  TXS              TXS Transfer index X to stack pointer                TXS
//                                                        N Z C I D V
//  Operation:  X -> S                                    _ _ _ _ _ _
//                                 (Ref: 8.8)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TXS                 |    9A   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TXS ( void )
{
   wSP ( rX() );

   return;
}

//  LDY                   LDY Load index Y with memory                    LDY
//                                                        N Z C I D V
//  Operation:  M -> Y                                    / / _ _ _ _
//                                 (Ref: 7.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   LDY #Oper           |    A0   |    2    |    2     |
//  |  Zero Page     |   LDY Oper            |    A4   |    2    |    3     |
//  |  Zero Page,X   |   LDY Oper,X          |    B4   |    2    |    4     |
//  |  Absolute      |   LDY Oper            |    AC   |    3    |    4     |
//  |  Absolute,X    |   LDY Oper,X          |    BC   |    3    |    4*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 when page boundary is crossed.
void C6502::LDY ( void )
{
   uint32_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wY ( (*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wY ( MEM(addr) );
   }

   wN ( rY()&0x80 );
   wZ ( !rY() );

   return;
}

// LAX (LAX) [LAX]
// ~~~~~~~~~~~~~~~
// Load accumulator and X register with memory.
// Status flags: N,Z
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |LAX arg    |$A7| 2 | 3
// Zero Page,Y |LAX arg,Y  |$B7| 2 | 4
// Absolute    |LAX arg    |$AF| 3 | 4
// Absolute,Y  |LAX arg,Y  |$BF| 3 | 4 *
// (Indirect,X)|LAX (arg,X)|$A3| 2 | 6
// (Indirect),Y|LAX (arg),Y|$B3| 2 | 5 *
//  * Add 1 if page boundary is crossed.
void C6502::LAX ( void )
{
   uint8_t val;

   uint32_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wA ( (*data) );
      wX ( (*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr ); // Single memory access cycle...
      wA ( val );
      wX ( val );
   }

   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  LDA                  LDA Load accumulator with memory                 LDA
//
//  Operation:  M -> A                                    N Z C I D V
//                                                        / / _ _ _ _
//                                (Ref: 2.1.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   LDA #Oper           |    A9   |    2    |    2     |
//  |  Zero Page     |   LDA Oper            |    A5   |    2    |    3     |
//  |  Zero Page,X   |   LDA Oper,X          |    B5   |    2    |    4     |
//  |  Absolute      |   LDA Oper            |    AD   |    3    |    4     |
//  |  Absolute,X    |   LDA Oper,X          |    BD   |    3    |    4*    |
//  |  Absolute,Y    |   LDA Oper,Y          |    B9   |    3    |    4*    |
//  |  (Indirect,X)  |   LDA (Oper,X)        |    A1   |    2    |    6     |
//  |  (Indirect),Y  |   LDA (Oper),Y        |    B1   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if page boundary is crossed.
void C6502::LDA ( void )
{
   uint32_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wA ( (*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wA ( MEM(addr) );
   }

   wN ( rA()&0x80 );
   wZ ( !rA() );

   return;
}

//  LDX                   LDX Load index X with memory                    LDX
//
//  Operation:  M -> X                                    N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.0)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   LDX #Oper           |    A2   |    2    |    2     |
//  |  Zero Page     |   LDX Oper            |    A6   |    2    |    3     |
//  |  Zero Page,Y   |   LDX Oper,Y          |    B6   |    2    |    4     |
//  |  Absolute      |   LDX Oper            |    AE   |    3    |    4     |
//  |  Absolute,Y    |   LDX Oper,Y          |    BE   |    3    |    4*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 when page boundary is crossed.
void C6502::LDX ( void )
{
   uint32_t addr;

   if ( amode == AM_IMMEDIATE )
   {
      wX ( (*data) );
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      wX ( MEM(addr) );
   }

   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

//  TAY                TAY Transfer accumulator to index Y                TAY
//
//  Operation:  A -> Y                                    N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.13)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TAY                 |    A8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TAY ( void )
{
   wY ( rA() );
   wN ( rY()&0x80 );
   wZ ( !rY() );

   return;
}

//  TAX                TAX Transfer accumulator to index X                TAX
//
//  Operation:  A -> X                                    N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.11)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TAX                 |    AA   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TAX ( void )
{
   wX ( rA() );
   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

//  BCS                      BCS Branch on carry set                      BCS
//
//  Operation:  Branch on C = 1                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.4)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BCS Oper            |    B0   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same  page.
//  * Add 2 if branch occurs to next  page.
void C6502::BCS ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( rC() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  CLV                      CLV Clear overflow flag                      CLV
//
//  Operation: 0 -> V                                     N Z C I D V
//                                                        _ _ _ _ _ 0
//                                (Ref: 3.6.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   CLV                 |    B8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CLV ( void )
{
   cV ();

   return;
}

//  TSX              TSX Transfer stack pointer to index X                TSX
//
//  Operation:  S -> X                                    N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 8.9)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   TSX                 |    BA   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::TSX ( void )
{
   wX ( rSP() );
   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

//  CPY                  CPY Compare memory and index Y                   CPY
//                                                        N Z C I D V
//  Operation:  Y - M                                     / / / _ _ _
//                                 (Ref: 7.9)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   CPY *Oper           |    C0   |    2    |    2     |
//  |  Zero Page     |   CPY Oper            |    C4   |    2    |    3     |
//  |  Absolute      |   CPY Oper            |    CC   |    3    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CPY ( void )
{
   uint16_t addr;
   uint8_t  val;

   if ( amode == AM_IMMEDIATE )
   {
      val = (*data)&0xFF;
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }

   wC ( rY()>=val );
   val = rY() - val;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   return;
}

//  CMP                CMP Compare memory and accumulator                 CMP
//
//  Operation:  A - M                                     N Z C I D V
//                                                        / / / _ _ _
//                                (Ref: 4.2.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   CMP #Oper           |    C9   |    2    |    2     |
//  |  Zero Page     |   CMP Oper            |    C5   |    2    |    3     |
//  |  Zero Page,X   |   CMP Oper,X          |    D5   |    2    |    4     |
//  |  Absolute      |   CMP Oper            |    CD   |    3    |    4     |
//  |  Absolute,X    |   CMP Oper,X          |    DD   |    3    |    4*    |
//  |  Absolute,Y    |   CMP Oper,Y          |    D9   |    3    |    4*    |
//  |  (Indirect,X)  |   CMP (Oper,X)        |    C1   |    2    |    6     |
//  |  (Indirect),Y  |   CMP (Oper),Y        |    D1   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if page boundary is crossed.
void C6502::CMP ( void )
{
   uint32_t addr;
   uint8_t val;

   if ( amode == AM_IMMEDIATE )
   {
      val = (*data)&0xFF;
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }

   wC ( rA()>=val );
   val = rA() - val;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   return;
}

// DCP (DCP) [DCM]
// ~~~~~~~~~~~~~~~
// Subtract 1 from memory (without borrow).
// Status flags: C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |DCP arg    |$C7| 2 | 5
// Zero Page,X |DCP arg,X  |$D7| 2 | 6
// Absolute    |DCP arg    |$CF| 3 | 6
// Absolute,X  |DCP arg,X  |$DF| 3 | 7
// Absolute,Y  |DCP arg,Y  |$DB| 3 | 7
// (Indirect,X)|DCP (arg,X)|$C3| 2 | 8
// (Indirect),Y|DCP (arg),Y|$D3| 2 | 8
void C6502::DCM ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
   val -= 1;
   MEM ( addr, val );
   wC ( rA()>=val );
   val = rA() - val;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

//  DEC                   DEC Decrement memory by one                     DEC
//
//  Operation:  M - 1 -> M                                N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 10.7)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   DEC Oper            |    C6   |    2    |    5     |
//  |  Zero Page,X   |   DEC Oper,X          |    D6   |    2    |    6     |
//  |  Absolute      |   DEC Oper            |    CE   |    3    |    6     |
//  |  Absolute,X    |   DEC Oper,X          |    DE   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::DEC ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   // dummy write
   MEM ( addr, val );

   val -= 1;
   MEM ( addr, val );
   wN ( val&0x80 );
   wZ ( !val );

   return;
}

//  INY                    INY Increment Index Y by one                   INY
//
//  Operation:  Y + 1 -> Y                                N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.5)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   INY                 |    C8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::INY ( void )
{
   wY ( rY()+1 );
   wN ( rY()&0x80 );
   wZ ( !rY() );

   return;
}

//  DEX                   DEX Decrement index X by one                    DEX
//
//  Operation:  X - 1 -> X                                N Z C I D V
//                                                        / / _ _ _ _
//                                 (Ref: 7.6)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   DEX                 |    CA   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::DEX ( void )
{
   wX ( rX()-1 );
   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

//  BNE                   BNE Branch on result not zero                   BNE
//
//  Operation:  Branch on Z = 0                           N Z C I D V
//                                                        _ _ _ _ _ _
//                               (Ref: 4.1.1.6)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BNE Oper            |    D0   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same page.
//  * Add 2 if branch occurs to different page.
void C6502::BNE ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( !rZ() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  CLD                      CLD Clear decimal mode                       CLD
//
//  Operation:  0 -> D                                    N A C I D V
//                                                        _ _ _ _ 0 _
//                                (Ref: 3.3.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   CLD                 |    D8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CLD ( void )
{
   cD ();

   return;
}

//  CPX                  CPX Compare Memory and Index X                   CPX
//                                                        N Z C I D V
//  Operation:  X - M                                     / / / _ _ _
//                                 (Ref: 7.8)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   CPX *Oper           |    E0   |    2    |    2     |
//  |  Zero Page     |   CPX Oper            |    E4   |    2    |    3     |
//  |  Absolute      |   CPX Oper            |    EC   |    3    |    4     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::CPX ( void )
{
   uint16_t addr;
   uint8_t  val;

   if ( amode == AM_IMMEDIATE )
   {
      val = (*data)&0xFF;
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }

   wC ( rX()>=val );
   val = rX() - val;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

   return;
}

//  SBC          SBC Subtract memory from accumulator with borrow         SBC
//                      -
//  Operation:  A - M - (1-C) -> A                        N Z C I D V
//         -                                              / / / _ _ /
//    Note:C = Borrow             (Ref: 2.2.2)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Immediate     |   SBC #Oper           |    E9   |    2    |    2     |
//  |  Zero Page     |   SBC Oper            |    E5   |    2    |    3     |
//  |  Zero Page,X   |   SBC Oper,X          |    F5   |    2    |    4     |
//  |  Absolute      |   SBC Oper            |    ED   |    3    |    4     |
//  |  Absolute,X    |   SBC Oper,X          |    FD   |    3    |    4*    |
//  |  Absolute,Y    |   SBC Oper,Y          |    F9   |    3    |    4*    |
//  |  (Indirect,X)  |   SBC (Oper,X)        |    E1   |    2    |    6     |
//  |  (Indirect),Y  |   SBC (Oper),Y        |    F1   |    2    |    5*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 when page boundary is crossed.
void C6502::SBC ( void )
{
   uint32_t addr;
   int16_t result;
   uint8_t val;

   if ( amode == AM_IMMEDIATE )
   {
      val = (*data);
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }

   result = (rA() - val - (1-rC()));

   wV ( ((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (uint8_t)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( !(result&0x100) );

   return;
}

//  INC                    INC Increment memory by one                    INC
//                                                        N Z C I D V
//  Operation:  M + 1 -> M                                / / _ _ _ _
//                                 (Ref: 10.6)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Zero Page     |   INC Oper            |    E6   |    2    |    5     |
//  |  Zero Page,X   |   INC Oper,X          |    F6   |    2    |    6     |
//  |  Absolute      |   INC Oper            |    EE   |    3    |    6     |
//  |  Absolute,X    |   INC Oper,X          |    FE   |    3    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::INC ( void )
{
   uint16_t addr;
   uint8_t  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   // dummy write
   MEM ( addr, val );

   val++;
   MEM ( addr, val );
   wN ( val&0x80 );
   wZ ( !val );

   return;
}

// ISC (ISB) [INS]
// ~~~~~~~~~~~~~~~
// Increase memory by one, then subtract memory from accumulator
// (with borrow). Status flags: N,V,Z,C
//
// Addressing  |Mnemonics  |Opc|Sz | n
// ------------|-----------|---|---|---
// Zero Page   |ISC arg    |$E7| 2 | 5
// Zero Page,X |ISC arg,X  |$F7| 2 | 6
// Absolute    |ISC arg    |$EF| 3 | 6
// Absolute,X  |ISC arg,X  |$FF| 3 | 7
// Absolute,Y  |ISC arg,Y  |$FB| 3 | 7
// (Indirect,X)|ISC (arg,X)|$E3| 2 | 8
// (Indirect),Y|ISC (arg),Y|$F3| 2 | 8
void C6502::INS ( void )
{
   uint16_t addr;
   uint8_t  val;
   int16_t result;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
   val++;
   MEM ( addr, val );

   result = (rA() - val - (1-rC()));

   wV ( ((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (uint8_t)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( !(result&0x100) );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

//  INX                    INX Increment Index X by one                   INX
//                                                        N Z C I D V
//  Operation:  X + 1 -> X                                / / _ _ _ _
//                                 (Ref: 7.4)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   INX                 |    E8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::INX ( void )
{
   wX ( rX()+1 );
   wN ( rX()&0x80 );
   wZ ( !rX() );

   return;
}

//  NOP                         NOP No operation                          NOP
//                                                        N Z C I D V
//  Operation:  No Operation (2 cycles)                   _ _ _ _ _ _
//
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   NOP                 |    EA   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::NOP ( void )
{
   return;
}

void C6502::DOP ( void )
{
   /*uint16_t addr = */MAKEADDR ( amode, data );

   if ( amode != AM_IMMEDIATE )
   {
      // A missing memory cycle here?
      // Synchronize CPU and APU...
      ADVANCE ();
   }

   return;
}

void C6502::TOP ( void )
{
   /*uint16_t addr = */MAKEADDR ( amode, data );

   // A missing memory cycle here?
   // Synchronize CPU and APU...
   ADVANCE ();

   return;
}

//  BEQ                    BEQ Branch on result zero                      BEQ
//                                                        N Z C I D V
//  Operation:  Branch on Z = 1                           _ _ _ _ _ _
//                               (Ref: 4.1.1.5)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Relative      |   BEQ Oper            |    F0   |    2    |    2*    |
//  +----------------+-----------------------+---------+---------+----------+
//  * Add 1 if branch occurs to same  page.
//  * Add 2 if branch occurs to next  page.
void C6502::BEQ ( void )
{
   uint32_t target;

   target = rPC()+GETSIGNED8(data,0);

   if ( rZ() )
   {
      // Synchronize CPU and APU...
      MEM ( (rPC()&0xFF00)|(((rPC()&0x00FF)+GETUNSIGNED8(data,0))&0xFF) );

      // Extra cycle.
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         // Synchronize CPU and APU...
         MEM ( target );
      }

      wPC ( target );

      if ( rPC() == m_pcGoto )
      {
         NES()->STEPCPUBREAKPOINT();
         m_pcGoto = 0xFFFFFFFF;
      }
   }

   return;
}

//  SED                       SED Set decimal mode                        SED
//                                                        N Z C I D V
//  Operation:  1 -> D                                    _ _ _ _ 1 _
//                                (Ref: 3.3.1)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   SED                 |    F8   |    1    |    2     |
//  +----------------+-----------------------+---------+---------+----------+
void C6502::SED ( void )
{
   sD ();

   return;
}

//  BRK                          BRK Force Break                          BRK
//
//  Operation:  Forced Interrupt PC + 2 toS P toS         N Z C I D V
//                                                        _ _ _ 1 _ _
//                                 (Ref: 9.11)
//  +----------------+-----------------------+---------+---------+----------+
//  | Addressing Mode| Assembly Language Form| OP CODE |No. Bytes|No. Cycles|
//  +----------------+-----------------------+---------+---------+----------+
//  |  Implied       |   BRK                 |    00   |    1    |    7     |
//  +----------------+-----------------------+---------+---------+----------+
//  1. A BRK command cannot be masked by setting I.
void C6502::BRK ( void )
{
   uint8_t         pchi;
   static uint8_t  pclo = 0x00;
   static bool     doingIrq = false;

   if ( !m_killed )
   {
      if ( m_instrCycle == 2 )
      {
         PUSH ( GETHI8(rPC()) );
      }
      else if ( m_instrCycle == 3 )
      {
         PUSH ( GETLO8((rPC())) );
      }
      else if ( m_instrCycle == 4 )
      {
         PUSH ( rF() );
         if ( m_nmiPending )
         {
            doingIrq = false;
         }
         else
         {
            doingIrq = true;
         }
      }
      else
      {
         if ( m_nmiPending && !doingIrq )
         {
            if ( m_instrCycle == 5 )
            {
               pclo = MEM(VECTOR_NMI);
            }
            else if ( m_instrCycle == 6 )
            {
               pchi = MEM(VECTOR_NMI+1);

               wPC ( MAKE16(pclo,pchi) );

               if ( rPC() == m_pcGoto )
               {
                  NES()->STEPCPUBREAKPOINT();
                  m_pcGoto = 0xFFFFFFFF;
               }

               if ( nesIsDebuggable )
               {
                  // Check for NMI breakpoint...
                  NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_NMI_ENTERED);
               }

               sI();
               m_nmiPending = false;
               doingIrq = false;
            }
         }
         else
         {
            if ( m_instrCycle == 5 )
            {
               pclo = MEM(VECTOR_IRQ);
            }
            else if ( m_instrCycle == 6 )
            {
               pchi = MEM(VECTOR_IRQ+1);

               wPC ( MAKE16(pclo,pchi) );

               if ( rPC() == m_pcGoto )
               {
                  NES()->STEPCPUBREAKPOINT();
                  m_pcGoto = 0xFFFFFFFF;
               }

               if ( nesIsDebuggable )
               {
                  // Check for IRQ breakpoint...
                  NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_IRQ_ENTERED);
               }

               sI();
               m_irqPending = false;
               doingIrq = false;
            }
         }
      }
   }

   return;
}

void C6502::ASSERTIRQ ( int8_t source )
{
   m_irqAsserted = true;

   if ( nesIsDebuggable )
   {
      if ( source == eNESSource_Mapper )
      {
         NES()->TRACER()->AddIRQ ( m_cycles, source );
      }
      else
      {
         NES()->TRACER()->AddIRQ ( APU()->CYCLES(), source );
      }

      // Check for IRQ breakpoint...
      NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_IRQ_FIRES);
   }
}

void C6502::RELEASEIRQ ( int8_t source )
{
   if ( nesIsDebuggable )
   {
      if ( source == eNESSource_Mapper )
      {
         NES()->TRACER()->AddIRQRelease ( m_cycles, source );
      }
      else
      {
         NES()->TRACER()->AddIRQRelease ( APU()->CYCLES(), source );
      }
   }
   m_irqAsserted = false;
}

void C6502::ASSERTNMI ()
{
   m_nmiAsserted = true;

   if ( nesIsDebuggable )
   {
      NES()->TRACER()->AddNMI ( NES()->PPU()->_CYCLES(), eNESSource_PPU );

      // Check for NMI breakpoint...
      NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_NMI_FIRES);
   }
}

void C6502::RESET ( bool soft )
{
   m_killed = false;

   APU()->RESET ();

// CP ALWAYS DO   if ( nesIsDebuggable )
   {
      NES()->TRACER()->AddRESET ();
   }

   m_cycles = 0;
   m_curCycles = 0;
   m_phase = 0;

   m_dmaRequest = -1;
   m_writeDmaCounter = 0;
   m_readDmaCounter = 0;

   m_write = false;

   // Clear the disassembly sample...
   pDisassemblySample = NULL;

   m_irqAsserted = false;
   m_irqPending = false;
   m_instrCycle = 0;
   m_nmiAsserted = false;
   m_nmiPending = false;

   wEA ( 0 );

   m_pcGoto = 0xFFFFFFFF;

   wF ( 0 );
   sI ();
   sB ();

   // Fake cycle -- stuff is being cleared
   MEM(0xFF);

   // Fake opcode reads
   MEM(0xFF);
   MEM(0xFF);

   wA ( 0 );
   wX ( 0 );
   wY ( 0 );
   wSP ( 0 );
   MEM ( 0x00 ); // These emulate the three "fake stack pushes" that occur
   wSP ( 0xFF );
   MEM ( 0x00 ); // during the 6502 RESET sequence.  The stack pushes are fake
   wSP ( 0xFE );
   MEM ( 0x00 ); // in that they occur but are READs instead of WRITEs.
   wSP ( 0xFD );
   wPC ( MAKE16(MEM(VECTOR_RESET),MEM(VECTOR_RESET+1)) );

   m_pcSync = rPC();
   m_pcSyncSet = true;

   // Clear memory...
   if ( !soft )
   {
      m_6502memory.MEMCLR ();
   }

   if ( nesIsDebuggable )
   {
      // Check for RESET breakpoint...
      NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_RESET);
   }
}

uint8_t C6502::LOAD ( uint32_t addr, int8_t* pTarget )
{
   uint8_t data = C6502::OPENBUS();

   if ( addr >= 0x8000 )
   {
      (*pTarget) = eTarget_Mapper;
      data = NES()->CART()->HMAPPER(addr);
   }
   else if ( addr < 0x2000 )
   {
      (*pTarget) = eTarget_RAM;
      data = m_6502memory.MEM(addr);
   }
   else if ( addr < 0x4000 )
   {
      (*pTarget) = eTarget_PPURegister;
      data = NES()->PPU()->PPU ( addr );
   }
   else if ( addr >= 0x6000 )
   {
      (*pTarget) = eTarget_SRAM;
      data = NES()->CART()->LMAPPER(addr);
   }
   else if ( addr >= 0x5C00 )
   {
      (*pTarget) = eTarget_EXRAM;
      data = NES()->CART()->LMAPPER(addr);
   }
   else if ( addr >= 0x4018 )
   {
      (*pTarget) = eTarget_Mapper;
      data = NES()->CART()->LMAPPER(addr);
   }
   else
   {
      // Use function maps for controller-type-based accesses...
      if ( addr == 0x4014 )
      {
         // Read of 4014 returns open bus, I think.
         (*pTarget) = eTarget_IORegister;
         data = OPENBUS();
      }
      if ( addr == 0x4016 )
      {
         (*pTarget) = eTarget_IORegister;
         data = iofunc[NES()->CONTROLLER(CONTROLLER1)].emuread(addr);
      }
      else if ( addr == 0x4017 )
      {
         (*pTarget) = eTarget_IORegister;
         data = iofunc[NES()->CONTROLLER(CONTROLLER2)].emuread(addr);
      }
      // Otherwise if not accessing a controller port, use default...
      else
      {
         (*pTarget) = eTarget_APURegister;
         data = APU()->APU ( addr );
      }
   }

   return data;
}

void C6502::STORE ( uint32_t addr, uint8_t data, int8_t* pTarget )
{
   if ( addr < 0x2000 )
   {
      (*pTarget) = eTarget_RAM;
      m_6502memory.MEM(addr,data&0xFF);
   }
   else if ( addr < 0x4000 )
   {
      (*pTarget) = eTarget_PPURegister;
      NES()->PPU()->PPU ( addr, data );
   }
   else if ( addr < 0x4018 )
   {
      // Use function maps for controller-type-based accesses...
      if ( addr == 0x4016 )
      {
         (*pTarget) = eTarget_IORegister;

         // Writes to $4016 need to be given to all controllers.
         iofunc[NES()->CONTROLLER(CONTROLLER1)].emuwrite(addr,data);
         iofunc[NES()->CONTROLLER(CONTROLLER2)].emuwrite(addr,data);
      }
      else if ( addr == 0x4014 )
      {
         (*pTarget) = eTarget_IORegister;

         // DMA
         // Note: DMA is done in C6502::EMULATE, it is only set-up here.
         m_writeDmaAddr = data<<8;
         m_writeDmaCounter = 513;
         if ( _CYCLES()&1 )
         {
            m_writeDmaCounter++;
         }
      }
      // Otherwise if not accessing a controller port, use default...
      else
      {
         (*pTarget) = eTarget_APURegister;
         APU()->APU ( addr, data );
      }
   }
   else if ( addr < 0x5C00 )
   {
      (*pTarget) = eTarget_Mapper;
      NES()->CART()->LMAPPER(addr,data);
   }
   else if ( addr < 0x6000 )
   {
      (*pTarget) = eTarget_EXRAM;
      NES()->CART()->LMAPPER(addr,data);
   }
   else if ( addr < 0x8000 )
   {
      (*pTarget) = eTarget_SRAM;
      NES()->CART()->LMAPPER(addr,data);
   }
   else
   {
      (*pTarget) = eTarget_Mapper;
      NES()->CART()->HMAPPER(addr,data);
   }
}

uint8_t C6502::FETCH ()
{
   int8_t target;
   uint8_t data;
   uint8_t instrCycle = m_instrCycle;

   // Not writing...
   m_write = false;

   // Set effective address.
   wEA ( rPC() );
   if ( nesIsDebuggable )
   {
      NES()->TRACER()->SetEffectiveAddress ( NES()->TRACER()->GetLastCPUSample(), rEA() );
   }

   // Synchronize CPU and APU...
   ADVANCE ();

   data = LOAD ( rPC(), &target );

   // Store data to return as open-bus.
   m_openBusData = data;

   if ( nesIsDebuggable )
   {
      // Add Tracer sample...
      if ( instrCycle == 0 )
      {
         NES()->TRACER()->AddSample ( m_cycles, eTracer_InstructionFetch, eNESSource_CPU, target, rPC(), data );
      }
      else
      {
         NES()->TRACER()->AddSample ( m_cycles, eTracer_OperandFetch, eNESSource_CPU, target, rPC(), data );
      }

      // If ROM is being accessed, log code/data logger...
      if ( (target == eTarget_Mapper) &&
           (rPC() >= MEM_32KB) )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( rPC() );
         if ( instrCycle == 0 )
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_InstructionFetch, eNESSource_CPU );
         }
         else
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
         }

         // Update Markers...
         m_marker->UpdateMarkers ( NES()->CART()->PRGROMPHYSADDR(rPC()), C6502::_CYCLES(), NES()->PPU()->_FRAME(), NES()->PPU()->_CYCLES() );

         // ... and update opcode masking for disassembler...
         NES()->CART()->PRGROMOPCODEMASK ( rPC(), (uint8_t)(instrCycle==0) );
      }
      else if ( target == eTarget_SRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->SRAMLOGGERVIRT ( rPC() );
         if ( instrCycle == 0 )
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_InstructionFetch, eNESSource_CPU );
         }
         else
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
         }

         // Update opcode masking for disassembler...
         NES()->CART()->SRAMOPCODEMASK ( rPC(), (uint8_t)(instrCycle==0) );
      }
      else if ( target == eTarget_EXRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->EXRAMLOGGER ();
         if ( instrCycle == 0 )
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_InstructionFetch, eNESSource_CPU );
         }
         else
         {
            pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
         }

         // Update opcode masking for disassembler...
         NES()->CART()->EXRAMOPCODEMASK ( rPC(), (uint8_t)(instrCycle==0) );
      }
      else if ( target == eTarget_RAM )
      {
         if ( instrCycle == 0 )
         {
            m_6502memory.LOGGER()->LogAccess ( m_cycles, rPC(), data, eLogger_InstructionFetch, eNESSource_CPU );
         }
         else
         {
            m_6502memory.LOGGER()->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
         }

         // ... and update opcode masking for disassembler...
         OPCODEMASK ( rPC(), (uint8_t)(instrCycle==0) );
      }
   }

   return data;
}

uint8_t C6502::EXTRAFETCH ()
{
   int8_t target;
   uint8_t data;

   // Not writing...
   m_write = false;

   // Set effective address.
   wEA ( rPC() );
   if ( nesIsDebuggable )
   {
      NES()->TRACER()->SetEffectiveAddress ( NES()->TRACER()->GetLastCPUSample(), rEA() );
   }

   // Synchronize CPU and APU...
   ADVANCE ();

   data = LOAD ( rPC(), &target );

   if ( nesIsDebuggable )
   {
      // Add Tracer sample...
      NES()->TRACER()->AddSample ( m_cycles, eTracer_OperandFetch, eNESSource_CPU, target, rPC(), data );

#if 0
      // If ROM is being accessed, log code/data logger...
      if ( target == eTarget_Mapper )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( rPC() );
         pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
      }
      else if ( target == eTarget_SRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->SRAMLOGGERVIRT ( rPC() );
         pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
      }
      else if ( target == eTarget_EXRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->EXRAMLOGGER ();
         pLogger->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
      }
      else if ( target == eTarget_RAM )
      {
         m_6502memory.LOGGER()->LogAccess ( m_cycles, rPC(), data, eLogger_OperandFetch, eNESSource_CPU );
      }
#endif
   }

   return data;
}

uint8_t C6502::DMA ( uint32_t addr )
{
   int8_t target;
   uint8_t data;

   // Not writing...
   m_write = false;

   // Synchronize CPU and APU...
   ADVANCE ( true );

   data = LOAD ( addr, &target );

   if ( nesIsDebuggable )
   {
      // Add Tracer sample...
      NES()->TRACER()->AddSample ( m_cycles, eTracer_DMA, eNESSource_CPU, target, addr, data );

      // If ROM or RAM is being accessed, log code/data logger...
      if ( (target == eTarget_Mapper) &&
           (addr >= MEM_32KB) )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DMA, eNESSource_APU );
      }
      else if ( target == eTarget_RAM )
      {
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DMA, eNESSource_APU );
      }

      // Check for breakpoint...
      NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUMemoryRead, data );
   }

   return data;
}

void C6502::DMA ( uint32_t srcAddr, uint32_t dstAddr, uint8_t data )
{
   TracerInfo* pSample = NULL;
   int8_t target = -1;

   // Writing...
   m_write = true;

   // Synchronize CPU and APU...
   ADVANCE ( true );

   if ( nesIsDebuggable )
   {
      // Store unknown target because otherwise the trace will be out of order...
      pSample = NES()->TRACER()->AddSample ( m_cycles, eTracer_DMA, eNESSource_CPU, target, dstAddr, data );
   }

   STORE ( dstAddr, data, &target );

   if ( nesIsDebuggable )
   {
      // If ROM or RAM is being accessed, log code/data logger...
      if ( srcAddr >= MEM_32KB )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( srcAddr );
         pLogger->LogAccess ( m_cycles, srcAddr, data, eLogger_DMA, eNESSource_PPU );
      }
      else if ( srcAddr < MEM_8KB )
      {
         m_6502memory.LOGGER()->LogAccess ( m_cycles, srcAddr, data, eLogger_DMA, eNESSource_PPU );
      }
   }

   // Store real target...
   if ( pSample )
   {
      pSample->target = target;
   }

   if ( nesIsDebuggable )
   {
      // Check for breakpoint...
      NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUMemoryWrite, data );
   }
}

uint8_t C6502::MEM ( uint32_t addr )
{
   int8_t target;
   uint8_t data;

   // Not writing...
   m_write = false;

   // Set effective address.
   wEA ( addr );
   if ( nesIsDebuggable )
   {
      NES()->TRACER()->SetEffectiveAddress ( NES()->TRACER()->GetLastCPUSample(), rEA() );
   }

   // Synchronize CPU and APU...
   ADVANCE ();

   data = LOAD ( addr, &target );

   if ( nesIsDebuggable )
   {
      // Add Tracer sample...
      NES()->TRACER()->AddSample ( m_cycles, eTracer_DataRead, eNESSource_CPU, target, addr, data );

      // If ROM or RAM is being accessed, log code/data logger...
      if ( (target == eTarget_Mapper) &&
           (addr >= MEM_32KB) )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_SRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->SRAMLOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_EXRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->EXRAMLOGGER ();
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_RAM )
      {
         // Log to Code/Data Logger...
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else
      {
         // Registers...
         // Log to Code/Data Logger...
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }

      // Check for breakpoint...
      NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUMemoryRead, data );
   }

   return data;
}

void C6502::MEM ( uint32_t addr, uint8_t data )
{
   TracerInfo* pSample = NULL;
   int8_t target;

   // Writing...
   m_write = true;

   // Set effective address.
   wEA ( addr );
   if ( nesIsDebuggable )
   {
      NES()->TRACER()->SetEffectiveAddress ( NES()->TRACER()->GetLastCPUSample(), rEA() );
   }

   // Synchronize CPU and APU...
   ADVANCE ();

   if ( nesIsDebuggable )
   {
      // Store unknown target because otherwise the trace will be out of order...
      pSample = NES()->TRACER()->AddSample ( m_cycles, eTracer_DataWrite, eNESSource_CPU, 0, addr, data );
   }

   STORE ( addr, data, &target );

   if ( nesIsDebuggable )
   {
      // If ROM or RAM is being accessed, log code/data logger...
      if ( (target == eTarget_Mapper) &&
           (addr >= MEM_32KB) )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eNESSource_CPU );
      }
      else if ( target == eTarget_SRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->SRAMLOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eNESSource_CPU );
      }
      else if ( target == eTarget_EXRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->EXRAMLOGGER ();
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eNESSource_CPU );
      }
      else if ( target == eTarget_RAM )
      {
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eNESSource_CPU );
      }
      else
      {
         // Registers...
         // Log to Code/Data Logger...
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eNESSource_CPU );
      }
   }

   // Store real target...
   if ( pSample )
   {
      pSample->target = target;
   }

   if ( nesIsDebuggable )
   {
      // Check for breakpoint...
      NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUMemoryWrite, data );
   }
}

uint8_t C6502::STEAL ( uint32_t addr, uint8_t source )
{
   int8_t target;
   uint8_t data;

   // Not writing...
   m_write = false;

   // Set effective address.
   wEA ( addr );
   if ( nesIsDebuggable )
   {
      NES()->TRACER()->SetEffectiveAddress ( NES()->TRACER()->GetLastCPUSample(), rEA() );
   }

   // Synchronize CPU and APU...
   ADVANCE ( true );

   data = LOAD ( addr, &target );

   if ( nesIsDebuggable )
   {
      // Add Tracer sample...
      NES()->TRACER()->AddStolenCycle ( m_cycles, source );

      // If ROM or RAM is being accessed, log code/data logger...
      if ( (target == eTarget_Mapper) &&
           (addr >= MEM_32KB) )
      {
         CCodeDataLogger* pLogger = NES()->CART()->LOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_SRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->SRAMLOGGERVIRT ( addr );
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_EXRAM )
      {
         // Log to Code/Data Logger...
         CCodeDataLogger* pLogger = NES()->CART()->EXRAMLOGGER ();
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else if ( target == eTarget_RAM )
      {
         // Log to Code/Data Logger...
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }
      else
      {
         // Registers...
         // Log to Code/Data Logger...
         m_6502memory.LOGGER()->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eNESSource_CPU );
      }

      // Check stolen cycles breakpoint.
      NES()->CHECKBREAKPOINT(eBreakInCPU,eBreakOnCPUEvent,0,CPU_EVENT_STOLEN_CYCLE);

      // Check for breakpoint...
      NES()->CHECKBREAKPOINT ( eBreakInCPU, eBreakOnCPUMemoryRead, data );
   }

   return data;
}

uint8_t C6502::_MEM ( uint32_t addr )
{
   int8_t target;

   return LOAD ( addr, &target );
}

void C6502::_MEM ( uint32_t addr, uint8_t data )
{
   int8_t target;

   STORE ( addr, data, &target );
}

uint32_t C6502::MAKEADDR ( int32_t amode, uint8_t* data )
{
   uint16_t addr = 0x00, addrpre;

   if ( amode == AM_ZEROPAGE )
   {
      addr = (*data);
   }
   else if ( amode == AM_ZEROPAGE_INDEXED_X )
   {
      // dummy read
      MEM(*data);
      addr = ((*data)+rX())&0xFF;
   }
   else if ( amode == AM_ZEROPAGE_INDEXED_Y )
   {
      // dummy read
      MEM(*data);
      addr = ((*data)+rY())&0xFF;
   }
   else if ( amode == AM_ABSOLUTE )
   {
      addr = MAKE16((*data),(*(data+1)));
   }
   else if ( amode == AM_ABSOLUTE_INDEXED_X )
   {
      addrpre = MAKE16((*data),(*(data+1)));
      addr = addrpre+rX();

      // Check for ROL special case...
      if ( ((*opcodeData) == ROL_ABS_X) || ((addrpre>>8) != (addr>>8)) || (pOpcodeStruct->forceExtraCycle) )
      {
         // dummy read
         MEM((addrpre&0xFF00)+((addrpre+rX())&0xFF));
      }
   }
   else if ( amode == AM_ABSOLUTE_INDEXED_Y )
   {
      addrpre = MAKE16((*data),(*(data+1)));
      addr = addrpre+rY();

      if ( ((addrpre>>8) != (addr>>8)) || (pOpcodeStruct->forceExtraCycle) )
      {
         // dummy read
         MEM((addrpre&0xFF00)+((addrpre+rY())&0xFF));
      }
   }
   else if ( amode == AM_PREINDEXED_INDIRECT )
   {
      // dummy read
      MEM(*data);
      addr = MAKE16(MEM(((*data)+rX())&0xFF),MEM(((*data)+rX()+1)&0xFF));
   }
   else if ( amode == AM_POSTINDEXED_INDIRECT )
   {
      addrpre = MAKE16(MEM((*data)),MEM(((*data)+1)&0xFF));
      addr = addrpre+rY();

      if ( ((addrpre>>8) != (addr>>8)) || (pOpcodeStruct->forceExtraCycle) )
      {
         // dummy read
         MEM((addrpre&0xFF00)+((addrpre+rY())&0xFF));
      }
   }

   // Set "Effective Address" for use by Tracer, Breakpoints, etc...
   wEA ( addr );

   return addr;
}

void C6502::DISASSEMBLE ()
{
   if ( __PCSYNC() < 0x800 )
   {
      m_6502memory.DISASSEMBLE();
   }
}

void C6502::DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint16_t* addr2sloc, uint32_t* sourceLength )
{
   CNES6502_opcode* pOp;
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

void C6502::PRINTABLEADDR ( char* buffer, uint32_t addr )
{
   m_6502memory.PRINTABLEADDR(buffer,addr);
}

void C6502::PRINTABLEADDR ( char* buffer, uint32_t addr, uint32_t absAddr )
{
   m_6502memory.PRINTABLEADDR(buffer,addr,absAddr);
}

char* DISASSEMBLE ( uint8_t* pOpcode, char* buffer )
{
   char* lbuffer = buffer;
   CNES6502_opcode* pOp = C6502::m_6502opcode+(*pOpcode);

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
