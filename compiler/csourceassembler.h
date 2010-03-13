#ifndef CSOURCEASSEMBLER_H
#define CSOURCEASSEMBLER_H

#include <QString>
#include "csources.h"
#include "csourceitem.h"
#include "cprgrombank.h"
#include "cbinaryfiles.h"
#include "cbinaryfile.h"

typedef struct AssemblerInstructionItem
{
    QString mnemonic;
} AssemblerInstructionItem_s;

const AssemblerInstructionItem AssemblerInstructionItems[] =
{
   {"BRK"}, // BRK
   {"ORA"}, // ORA - (Indirect,X)
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"ASO"}, // ASO - (Indirect,X) (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"ORA"}, // ORA - Zero Page
   {"ASL"}, // ASL - Zero Page
   {"ASO"}, // ASO - Zero Page (undocumented)
   {"PHP"}, // PHP
   {"ORA"}, // ORA - Immediate
   {"ASL"}, // ASL - Accumulator
   {"ANC"}, // ANC - Immediate (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"ORA"}, // ORA - Absolute
   {"ASL"}, // ASL - Absolute
   {"ASO"}, // ASO - Absolute (undocumented)
   {"BPL"}, // BPL
   {"ORA"}, // ORA - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"ASO"}, // ASO - (Indirect),Y (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"ORA"}, // ORA - Zero Page,X
   {"ASL"}, // ASL - Zero Page,X
   {"ASO"}, // ASO - Zero Page,X (undocumented)
   {"CLC"}, // CLC
   {"ORA"}, // ORA - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"ASO"}, // ASO - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"ORA"}, // ORA - Absolute,X
   {"ASL"}, // ASL - Absolute,X
   {"ASO"}, // ASO - Absolute,X (undocumented)
   {"JSR"}, // JSR
   {"AND"}, // AND - (Indirect,X)
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"RLA"}, // RLA - (Indirect,X) (undocumented)
   {"BIT"}, // BIT - Zero Page
   {"AND"}, // AND - Zero Page
   {"ROL"}, // ROL - Zero Page
   {"RLA"}, // RLA - Zero Page (undocumented)
   {"PLP"}, // PLP
   {"AND"}, // AND - Immediate
   {"ROL"}, // ROL - Accumulator
   {"ANC"}, // ANC - Immediate (undocumented)
   {"BIT"}, // BIT - Absolute
   {"AND"}, // AND - Absolute
   {"ROL"}, // ROL - Absolute
   {"RLA"}, // RLA - Absolute (undocumented)
   {"BMI"}, // BMI
   {"AND"}, // AND - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"RLA"}, // RLA - (Indirect),Y (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"AND"}, // AND - Zero Page,X
   {"ROL"}, // ROL - Zero Page,X
   {"RLA"}, // RLA - Zero Page,X (undocumented)
   {"SEC"}, // SEC
   {"AND"}, // AND - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"RLA"}, // RLA - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"AND"}, // AND - Absolute,X
   {"ROL"}, // ROL - Absolute,X
   {"RLA"}, // RLA - Absolute,X (undocumented)
   {"RTI"}, // RTI
   {"EOR"}, // EOR - (Indirect,X)
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"LSE"}, // LSE - (Indirect,X) (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"EOR"}, // EOR - Zero Page
   {"LSR"}, // LSR - Zero Page
   {"LSE"}, // LSE - Zero Page (undocumented)
   {"PHA"}, // PHA
   {"EOR"}, // EOR - Immediate
   {"LSR"}, // LSR - Accumulator
   {"ALR"}, // ALR - Immediate (undocumented)
   {"JMP"}, // JMP - Absolute
   {"EOR"}, // EOR - Absolute
   {"LSR"}, // LSR - Absolute
   {"LSE"}, // LSE - Absolute (undocumented)
   {"BVC"}, // BVC
   {"EOR"}, // EOR - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"LSE"}, // LSE - (Indirect),Y
   {"DOP"}, // DOP (undocumented)
   {"EOR"}, // EOR - Zero Page,X
   {"LSR"}, // LSR - Zero Page,X
   {"LSE"}, // LSE - Zero Page,X (undocumented)
   {"CLI"}, // CLI
   {"EOR"}, // EOR - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"LSE"}, // LSE - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"EOR"}, // EOR - Absolute,X
   {"LSR"}, // LSR - Absolute,X
   {"LSE"}, // LSE - Absolute,X (undocumented)
   {"RTS"}, // RTS
   {"ADC"}, // ADC - (Indirect,X)
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"RRA"}, // RRA - (Indirect,X) (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"ADC"}, // ADC - Zero Page
   {"ROR"}, // ROR - Zero Page
   {"RRA"}, // RRA - Zero Page (undocumented)
   {"PLA"}, // PLA
   {"ADC"}, // ADC - Immediate
   {"ROR"}, // ROR - Accumulator
   {"ARR"}, // ARR - Immediate (undocumented)
   {"JMP"}, // JMP - Indirect
   {"ADC"}, // ADC - Absolute
   {"ROR"}, // ROR - Absolute
   {"RRA"}, // RRA - Absolute (undocumented)
   {"BVS"}, // BVS
   {"ADC"}, // ADC - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"RRA"}, // RRA - (Indirect),Y (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"ADC"}, // ADC - Zero Page,X
   {"ROR"}, // ROR - Zero Page,X
   {"RRA"}, // RRA - Zero Page,X (undocumented)
   {"SEI"}, // SEI
   {"ADC"}, // ADC - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"RRA"}, // RRA - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"ADC"}, // ADC - Absolute,X
   {"ROR"}, // ROR - Absolute,X
   {"RRA"}, // RRA - Absolute,X (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"STA"}, // STA - (Indirect,X)
   {"DOP"}, // DOP (undocumented)
   {"AXS"}, // AXS - (Indirect,X) (undocumented)
   {"STY"}, // STY - Zero Page
   {"STA"}, // STA - Zero Page
   {"STX"}, // STX - Zero Page
   {"AXS"}, // AXS - Zero Page (undocumented)
   {"DEY"}, // DEY
   {"DOP"}, // DOP (undocumented)
   {"TXA"}, // TXA
   {"XAA"}, // XAA - Immediate (undocumented)
   {"STY"}, // STY - Absolute
   {"STA"}, // STA - Absolute
   {"STX"}, // STX - Absolute
   {"AXS"}, // AXS - Absolulte (undocumented)
   {"BCC"}, // BCC
   {"STA"}, // STA - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"AXA"}, // AXA - (Indirect),Y
   {"STY"}, // STY - Zero Page,X
   {"STA"}, // STA - Zero Page,X
   {"STX"}, // STX - Zero Page,Y
   {"AXS"}, // AXS - Zero Page,Y
   {"TYA"}, // TYA
   {"STA"}, // STA - Absolute,Y
   {"TXS"}, // TXS
   {"TAS"}, // TAS - Absolute,Y (undocumented)
   {"SAY"}, // SAY - Absolute,X (undocumented)
   {"STA"}, // STA - Absolute,X
   {"XAS"}, // XAS - Absolute,Y (undocumented)
   {"AXA"}, // AXA - Absolute,Y (undocumented)
   {"LDY"}, // LDY - Immediate
   {"LDA"}, // LDA - (Indirect,X)
   {"LDX"}, // LDX - Immediate
   {"LAX"}, // LAX - (Indirect,X) (undocumented)
   {"LDY"}, // LDY - Zero Page
   {"LDA"}, // LDA - Zero Page
   {"LDX"}, // LDX - Zero Page
   {"LAX"}, // LAX - Zero Page (undocumented)
   {"TAY"}, // TAY
   {"LDA"}, // LDA - Immediate
   {"TAX"}, // TAX
   {"OAL"}, // OAL - Immediate
   {"LDY"}, // LDY - Absolute
   {"LDA"}, // LDA - Absolute
   {"LDX"}, // LDX - Absolute
   {"LAX"}, // LAX - Absolute (undocumented)
   {"BCS"}, // BCS
   {"LDA"}, // LDA - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"LAX"}, // LAX - (Indirect),Y (undocumented)
   {"LDY"}, // LDY - Zero Page,X
   {"LDA"}, // LDA - Zero Page,X
   {"LDX"}, // LDX - Zero Page,Y
   {"LAX"}, // LAX - Zero Page,X (undocumented)
   {"CLV"}, // CLV
   {"LDA"}, // LDA - Absolute,Y
   {"TSX"}, // TSX
   {"LAS"}, // LAS - Absolute,Y (undocumented)
   {"LDY"}, // LDY - Absolute,X
   {"LDA"}, // LDA - Absolute,X
   {"LDX"}, // LDX - Absolute,Y
   {"LAX"}, // LAX - Absolute,Y (undocumented)
   {"CPY"}, // CPY - Immediate
   {"CMP"}, // CMP - (Indirect,X)
   {"DOP"}, // DOP (undocumented)
   {"DCM"}, // DCM - (Indirect,X) (undocumented)
   {"CPY"}, // CPY - Zero Page
   {"CMP"}, // CMP - Zero Page
   {"DEC"}, // DEC - Zero Page
   {"DCM"}, // DCM - Zero Page (undocumented)
   {"INY"}, // INY
   {"CMP"}, // CMP - Immediate
   {"DEX"}, // DEX
   {"SAX"}, // SAX - Immediate (undocumented)
   {"CPY"}, // CPY - Absolute
   {"CMP"}, // CMP - Absolute
   {"DEC"}, // DEC - Absolute
   {"DCM"}, // DCM - Absolute (undocumented)
   {"BNE"}, // BNE
   {"CMP"}, // CMP   (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"DCM"}, // DCM - (Indirect),Y (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"CMP"}, // CMP - Zero Page,X
   {"DEC"}, // DEC - Zero Page,X
   {"DCM"}, // DCM - Zero Page,X (undocumented)
   {"CLD"}, // CLD
   {"CMP"}, // CMP - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"DCM"}, // DCM - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"CMP"}, // CMP - Absolute,X
   {"DEC"}, // DEC - Absolute,X
   {"DCM"}, // DCM - Absolute,X (undocumented)
   {"CPX"}, // CPX - Immediate
   {"SBC"}, // SBC - (Indirect,X)
   {"DOP"}, // DOP (undocumented)
   {"INS"}, // INS - (Indirect,X) (undocumented)
   {"CPX"}, // CPX - Zero Page
   {"SBC"}, // SBC - Zero Page
   {"INC"}, // INC - Zero Page
   {"INS"}, // INS - Zero Page (undocumented)
   {"INX"}, // INX
   {"SBC"}, // SBC - Immediate
   {"NOP"}, // NOP
   {"SBC"}, // SBC - Immediate (undocumented)
   {"CPX"}, // CPX - Absolute
   {"SBC"}, // SBC - Absolute
   {"INC"}, // INC - Absolute
   {"INS"}, // INS - Absolute (undocumented)
   {"BEQ"}, // BEQ
   {"SBC"}, // SBC - (Indirect),Y
   {"KIL"}, // KIL - Implied (processor lock up!)
   {"INS"}, // INS - (Indirect),Y (undocumented)
   {"DOP"}, // DOP (undocumented)
   {"SBC"}, // SBC - Zero Page,X
   {"INC"}, // INC - Zero Page,X
   {"INS"}, // INS - Zero Page,X (undocumented)
   {"SED"}, // SED
   {"SBC"}, // SBC - Absolute,Y
   {"NOP"}, // NOP (undocumented)
   {"INS"}, // INS - Absolute,Y (undocumented)
   {"TOP"}, // TOP (undocumented)
   {"SBC"}, // SBC - Absolute,X
   {"INC"}, // INC - Absolute,X
   {"INS"}, // INS - Absolute,X (undocumented)
   {""}
};

class CSourceAssembler
{
public:
    CSourceAssembler();
    bool assemble();
};

#endif // CSOURCEASSEMBLER_H
