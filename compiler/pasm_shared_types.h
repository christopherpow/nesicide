#if !defined ( PASM_SHARED_TYPES_H )
#define PASM_SHARED_TYPES_H

typedef struct _symbol_table
{
	char*          symbol;
   struct _ir_table* ir;
   unsigned int  btab_ent;
	unsigned short addr;
   unsigned char global;
   int value;
} symbol_table;

typedef struct _ir_table
{
   unsigned char data[3];
   unsigned int  btab_ent;
   unsigned int  addr;
   unsigned int  len;
   unsigned char emitted;
   unsigned char multi;
   unsigned char fixed;
   unsigned char label;
   struct _ir_table* ref;
   struct _ir_table* next;
   struct _ir_table* prev;
} ir_table;

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

typedef struct _C6502_opcode
{
   int op;
   char* name;
   int amode;
} C6502_opcode;

static C6502_opcode m_6502opcode [ 256 ] =
{
   { 0x00, "BRK", AM_IMPLIED }, // BRK
   { 0x01, "ORA", AM_PREINDEXED_INDIRECT }, // ORA - (Indirect,X)
   { 0x02, "", AM_IMPLIED }, // Future Expansion
   { 0x03, "", AM_IMPLIED }, // Future Expansion
   { 0x04, "", AM_IMPLIED }, // Future Expansion
   { 0x05, "ORA", AM_ZEROPAGE }, // ORA - Zero Page
   { 0x06, "ASL", AM_ZEROPAGE }, // ASL - Zero Page
   { 0x07, "", AM_IMPLIED }, // Future Expansion
   { 0x08, "PHP", AM_IMPLIED }, // PHP
   { 0x09, "ORA", AM_IMMEDIATE }, // ORA - Immediate
   { 0x0A, "ASL", AM_ACCUMULATOR }, // ASL - Accumulator
   { 0x0B, "", AM_IMPLIED }, // Future Expansion
   { 0x0C, "", AM_IMPLIED }, // Future Expansion
   { 0x0D, "ORA", AM_ABSOLUTE }, // ORA - Absolute
   { 0x0E, "ASL", AM_ABSOLUTE }, // ASL - Absolute
   { 0x0F, "", AM_IMPLIED }, // Future Expansion
   { 0x10, "BPL", AM_RELATIVE }, // BPL
   { 0x11, "ORA", AM_POSTINDEXED_INDIRECT }, // ORA - (Indirect),Y
   { 0x12, "", AM_IMPLIED }, // Future Expansion
   { 0x13, "", AM_IMPLIED }, // Future Expansion
   { 0x14, "", AM_IMPLIED }, // Future Expansion
   { 0x15, "ORA", AM_ZEROPAGE_INDEXED_X }, // ORA - Zero Page,X
   { 0x16, "ASL", AM_ZEROPAGE_INDEXED_X }, // ASL - Zero Page,X
   { 0x17, "", AM_IMPLIED }, // Future Expansion
   { 0x18, "CLC", AM_IMPLIED }, // CLC
   { 0x19, "ORA", AM_ABSOLUTE_INDEXED_Y }, // ORA - Absolute,Y
   { 0x1A, "", AM_IMPLIED }, // Future Expansion
   { 0x1B, "", AM_IMPLIED }, // Future Expansion
   { 0x1C, "", AM_IMPLIED }, // Future Expansion
   { 0x1D, "ORA", AM_ABSOLUTE_INDEXED_X }, // ORA - Absolute,X
   { 0x1E, "ASL", AM_ABSOLUTE_INDEXED_X }, // ASL - Absolute,X
   { 0x1F, "", AM_IMPLIED }, // Future Expansion
   { 0x20, "JSR", AM_ABSOLUTE }, // JSR
   { 0x21, "AND", AM_PREINDEXED_INDIRECT }, // AND - (Indirect,X)
   { 0x22, "", AM_IMPLIED }, // Future Expansion
   { 0x23, "", AM_IMPLIED }, // Future Expansion
   { 0x24, "BIT", AM_ZEROPAGE }, // BIT - Zero Page
   { 0x25, "AND", AM_ZEROPAGE }, // AND - Zero Page
   { 0x26, "ROL", AM_ZEROPAGE }, // ROL - Zero Page
   { 0x27, "", AM_IMPLIED }, // Future Expansion
   { 0x28, "PLP", AM_IMPLIED }, // PLP
   { 0x29, "AND", AM_IMMEDIATE }, // AND - Immediate
   { 0x2A, "ROL", AM_ACCUMULATOR }, // ROL - Accumulator
   { 0x2B, "", AM_IMPLIED }, // Future Expansion
   { 0x2C, "BIT", AM_ABSOLUTE }, // BIT - Absolute
   { 0x2D, "AND", AM_ABSOLUTE }, // AND - Absolute
   { 0x2E, "ROL", AM_ABSOLUTE }, // ROL - Absolute
   { 0x2F, "", AM_IMPLIED }, // Future Expansion
   { 0x30, "BMI", AM_RELATIVE }, // BMI
   { 0x31, "AND", AM_POSTINDEXED_INDIRECT }, // AND - (Indirect),Y
   { 0x32, "", AM_IMPLIED }, // Future Expansion
   { 0x33, "", AM_IMPLIED }, // Future Expansion
   { 0x34, "", AM_IMPLIED }, // Future Expansion
   { 0x35, "AND", AM_ZEROPAGE_INDEXED_X }, // AND - Zero Page,X
   { 0x36, "ROL", AM_ZEROPAGE_INDEXED_X }, // ROL - Zero Page,X
   { 0x37, "", AM_IMPLIED }, // Future Expansion
   { 0x38, "SEC", AM_IMPLIED }, // SEC
   { 0x39, "AND", AM_ABSOLUTE_INDEXED_Y }, // AND - Absolute,Y
   { 0x3A, "", AM_IMPLIED }, // Future Expansion
   { 0x3B, "", AM_IMPLIED }, // Future Expansion
   { 0x3C, "", AM_IMPLIED }, // Future Expansion
   { 0x3D, "AND", AM_ABSOLUTE_INDEXED_X }, // AND - Absolute,X
   { 0x3E, "ROL", AM_ABSOLUTE_INDEXED_X }, // ROL - Absolute,X
   { 0x3F, "", AM_IMPLIED }, // Future Expansion
   { 0x40, "RTI", AM_IMPLIED }, // RTI
   { 0x41, "EOR", AM_PREINDEXED_INDIRECT }, // EOR - (Indirect,X)
   { 0x42, "", AM_IMPLIED }, // Future Expansion
   { 0x43, "", AM_IMPLIED }, // Future Expansion
   { 0x44, "", AM_IMPLIED }, // Future Expansion
   { 0x45, "EOR", AM_ZEROPAGE }, // EOR - Zero Page
   { 0x46, "LSR", AM_ZEROPAGE }, // LSR - Zero Page
   { 0x47, "", AM_IMPLIED }, // Future Expansion
   { 0x48, "PHA", AM_IMPLIED }, // PHA
   { 0x49, "EOR", AM_IMMEDIATE }, // EOR - Immediate
   { 0x4A, "LSR", AM_ACCUMULATOR }, // LSR - Accumulator
   { 0x4B, "", AM_IMPLIED }, // Future Expansion
   { 0x4C, "JMP", AM_ABSOLUTE }, // JMP - Absolute
   { 0x4D, "EOR", AM_ABSOLUTE }, // EOR - Absolute
   { 0x4E, "LSR", AM_ABSOLUTE }, // LSR - Absolute
   { 0x4F, "", AM_IMPLIED }, // Future Expansion
   { 0x50, "BVC", AM_RELATIVE }, // BVC
   { 0x51, "EOR", AM_POSTINDEXED_INDIRECT }, // EOR - (Indirect),Y
   { 0x52, "", AM_IMPLIED }, // Future Expansion
   { 0x53, "", AM_IMPLIED }, // Future Expansion
   { 0x54, "", AM_IMPLIED }, // Future Expansion
   { 0x55, "EOR", AM_ZEROPAGE_INDEXED_X }, // EOR - Zero Page,X
   { 0x56, "LSR", AM_ZEROPAGE_INDEXED_X }, // LSR - Zero Page,X
   { 0x57, "", AM_IMPLIED }, // Future Expansion
   { 0x58, "CLI", AM_IMPLIED }, // CLI
   { 0x59, "EOR", AM_ABSOLUTE_INDEXED_Y }, // EOR - Absolute,Y
   { 0x5A, "", AM_IMPLIED }, // Future Expansion
   { 0x5B, "", AM_IMPLIED }, // Future Expansion
   { 0x5C, "", AM_IMPLIED }, // Future Expansion
   { 0x5D, "EOR", AM_ABSOLUTE_INDEXED_X }, // EOR - Absolute,X
   { 0x5E, "LSR", AM_ABSOLUTE_INDEXED_X }, // LSR - Absolute,X
   { 0x5F, "", AM_IMPLIED }, // Future Expansion
   { 0x60, "RTS", AM_IMPLIED }, // RTS
   { 0x61, "ADC", AM_PREINDEXED_INDIRECT }, // ADC - (Indirect,X)
   { 0x62, "", AM_IMPLIED }, // Future Expansion
   { 0x63, "", AM_IMPLIED }, // Future Expansion
   { 0x64, "", AM_IMPLIED }, // Future Expansion
   { 0x65, "ADC", AM_ZEROPAGE }, // ADC - Zero Page
   { 0x66, "ROR", AM_ZEROPAGE }, // ROR - Zero Page
   { 0x67, "", AM_IMPLIED }, // Future Expansion
   { 0x68, "PLA", AM_IMPLIED }, // PLA
   { 0x69, "ADC", AM_IMMEDIATE }, // ADC - Immediate
   { 0x6A, "ROR", AM_ACCUMULATOR }, // ROR - Accumulator
   { 0x6B, "", AM_IMPLIED }, // Future Expansion
   { 0x6C, "JMP", AM_INDIRECT }, // JMP - Indirect
   { 0x6D, "ADC", AM_ABSOLUTE }, // ADC - Absolute
   { 0x6E, "ROR", AM_ABSOLUTE }, // ROR - Absolute
   { 0x6F, "", AM_IMPLIED }, // Future Expansion
   { 0x70, "BVS", AM_RELATIVE }, // BVS
   { 0x71, "ADC", AM_POSTINDEXED_INDIRECT }, // ADC - (Indirect),Y
   { 0x72, "", AM_IMPLIED }, // Future Expansion
   { 0x73, "", AM_IMPLIED }, // Future Expansion
   { 0x74, "", AM_IMPLIED }, // Future Expansion
   { 0x75, "ADC", AM_ZEROPAGE_INDEXED_X }, // ADC - Zero Page,X
   { 0x76, "ROR", AM_ZEROPAGE_INDEXED_X }, // ROR - Zero Page,X
   { 0x77, "", AM_IMPLIED }, // Future Expansion
   { 0x78, "SEI", AM_IMPLIED }, // SEI
   { 0x79, "ADC", AM_ABSOLUTE_INDEXED_Y }, // ADC - Absolute,Y
   { 0x7A, "", AM_IMPLIED }, // Future Expansion
   { 0x7B, "", AM_IMPLIED }, // Future Expansion
   { 0x7C, "", AM_IMPLIED }, // Future Expansion
   { 0x7D, "ADC", AM_ABSOLUTE_INDEXED_X }, // ADC - Absolute,X
   { 0x7E, "ROR", AM_ABSOLUTE_INDEXED_X }, // ROR - Absolute,X
   { 0x7F, "", AM_IMPLIED }, // Future Expansion
   { 0x80, "", AM_IMPLIED }, // Future Expansion
   { 0x81, "STA", AM_PREINDEXED_INDIRECT }, // STA - (Indirect,X)
   { 0x82, "", AM_IMPLIED }, // Future Expansion
   { 0x83, "", AM_IMPLIED }, // Future Expansion
   { 0x84, "STY", AM_ZEROPAGE }, // STY - Zero Page
   { 0x85, "STA", AM_ZEROPAGE }, // STA - Zero Page
   { 0x86, "STX", AM_ZEROPAGE }, // STX - Zero Page
   { 0x87, "", AM_IMPLIED }, // Future Expansion
   { 0x88, "DEY", AM_IMPLIED }, // DEY
   { 0x89, "", AM_IMPLIED }, // Future Expansion
   { 0x8A, "TXA", AM_IMPLIED }, // TXA
   { 0x8B, "", AM_IMPLIED }, // Future Expansion
   { 0x8C, "STY", AM_ABSOLUTE }, // STY - Absolute
   { 0x8D, "STA", AM_ABSOLUTE }, // STA - Absolute
   { 0x8E, "STX", AM_ABSOLUTE }, // STX - Absolute
   { 0x8F, "", AM_IMPLIED }, // Future Expansion
   { 0x90, "BCC", AM_RELATIVE }, // BCC
   { 0x91, "STA", AM_POSTINDEXED_INDIRECT }, // STA - (Indirect),Y
   { 0x92, "", AM_IMPLIED }, // Future Expansion
   { 0x93, "", AM_IMPLIED }, // Future Expansion
   { 0x94, "STY", AM_ZEROPAGE_INDEXED_X }, // STY - Zero Page,X
   { 0x95, "STA", AM_ZEROPAGE_INDEXED_X }, // STA - Zero Page,X
   { 0x96, "STX", AM_ZEROPAGE_INDEXED_Y }, // STX - Zero Page,Y
   { 0x97, "", AM_IMPLIED }, // Future Expansion
   { 0x98, "TYA", AM_IMPLIED }, // TYA
   { 0x99, "STA", AM_ABSOLUTE_INDEXED_Y }, // STA - Absolute,Y
   { 0x9A, "TXS", AM_IMPLIED }, // TXS
   { 0x9B, "", AM_IMPLIED }, // Future Expansion
   { 0x9C, "", AM_IMPLIED }, // Future Expansion
   { 0x9D, "STA", AM_ABSOLUTE_INDEXED_X }, // STA - Absolute,X
   { 0x9E, "", AM_IMPLIED }, // Future Expansion
   { 0x9F, "", AM_IMPLIED }, // Future Expansion
   { 0xA0, "LDY", AM_IMMEDIATE }, // LDY - Immediate
   { 0xA1, "LDA", AM_PREINDEXED_INDIRECT }, // LDA - (Indirect,X)
   { 0xA2, "LDX", AM_IMMEDIATE }, // LDX - Immediate
   { 0xA3, "", AM_IMPLIED }, // Future Expansion
   { 0xA4, "LDY", AM_ZEROPAGE }, // LDY - Zero Page
   { 0xA5, "LDA", AM_ZEROPAGE }, // LDA - Zero Page
   { 0xA6, "LDX", AM_ZEROPAGE }, // LDX - Zero Page
   { 0xA7, "", AM_IMPLIED }, // Future Expansion
   { 0xA8, "TAY", AM_IMPLIED }, // TAY
   { 0xA9, "LDA", AM_IMMEDIATE }, // LDA - Immediate
   { 0xAA, "TAX", AM_IMPLIED }, // TAX
   { 0xAB, "", AM_IMPLIED }, // Future Expansion
   { 0xAC, "LDY", AM_ABSOLUTE }, // LDY - Absolute
   { 0xAD, "LDA", AM_ABSOLUTE }, // LDA - Absolute
   { 0xAE, "LDX", AM_ABSOLUTE }, // LDX - Absolute
   { 0xAF, "", AM_IMPLIED }, // Future Expansion
   { 0xB0, "BCS", AM_RELATIVE }, // BCS
   { 0xB1, "LDA", AM_POSTINDEXED_INDIRECT }, // LDA - (Indirect),Y
   { 0xB2, "", AM_IMPLIED }, // Future Expansion
   { 0xB3, "", AM_IMPLIED }, // Future Expansion
   { 0xB4, "LDY", AM_ZEROPAGE_INDEXED_X }, // LDY - Zero Page,X
   { 0xB5, "LDA", AM_ZEROPAGE_INDEXED_X }, // LDA - Zero Page,X
   { 0xB6, "LDX", AM_ZEROPAGE_INDEXED_Y }, // LDX - Zero Page,Y
   { 0xB7, "", AM_IMPLIED }, // Future Expansion
   { 0xB8, "CLV", AM_IMPLIED }, // CLV
   { 0xB9, "LDA", AM_ABSOLUTE_INDEXED_Y }, // LDA - Absolute,Y
   { 0xBA, "TSX", AM_IMPLIED }, // TSX
   { 0xBB, "", AM_IMPLIED }, // Future Expansion
   { 0xBC, "LDY", AM_ABSOLUTE_INDEXED_X }, // LDY - Absolute,X
   { 0xBD, "LDA", AM_ABSOLUTE_INDEXED_X }, // LDA - Absolute,X
   { 0xBE, "LDX", AM_ABSOLUTE_INDEXED_Y }, // LDX - Absolute,Y
   { 0xBF, "", AM_IMPLIED }, // Future Expansion
   { 0xC0, "CPY", AM_IMMEDIATE }, // CPY - Immediate
   { 0xC1, "CMP", AM_PREINDEXED_INDIRECT }, // CMP - (Indirect,X)
   { 0xC2, "", AM_IMPLIED }, // Future Expansion
   { 0xC3, "", AM_IMPLIED }, // Future Expansion
   { 0xC4, "CPY", AM_ZEROPAGE }, // CPY - Zero Page
   { 0xC5, "CMP", AM_ZEROPAGE }, // CMP - Zero Page
   { 0xC6, "DEC", AM_ZEROPAGE }, // DEC - Zero Page
   { 0xC7, "", AM_IMPLIED }, // Future Expansion
   { 0xC8, "INY", AM_IMPLIED }, // INY
   { 0xC9, "CMP", AM_IMMEDIATE }, // CMP - Immediate
   { 0xCA, "DEX", AM_IMPLIED }, // DEX
   { 0xCB, "", AM_IMPLIED }, // Future Expansion
   { 0xCC, "CPY", AM_ABSOLUTE }, // CPY - Absolute
   { 0xCD, "CMP", AM_ABSOLUTE }, // CMP - Absolute
   { 0xCE, "DEC", AM_ABSOLUTE }, // DEC - Absolute
   { 0xCF, "", AM_IMPLIED }, // Future Expansion
   { 0xD0, "BNE", AM_RELATIVE }, // BNE
   { 0xD1, "CMP", AM_POSTINDEXED_INDIRECT }, // CMP   (Indirect),Y
   { 0xD2, "", AM_IMPLIED }, // Future Expansion
   { 0xD3, "", AM_IMPLIED }, // Future Expansion
   { 0xD4, "", AM_IMPLIED }, // Future Expansion
   { 0xD5, "CMP", AM_ZEROPAGE_INDEXED_X }, // CMP - Zero Page,X
   { 0xD6, "DEC", AM_ZEROPAGE_INDEXED_X }, // DEC - Zero Page,X
   { 0xD7, "", AM_IMPLIED }, // Future Expansion
   { 0xD8, "CLD", AM_IMPLIED }, // CLD
   { 0xD9, "CMP", AM_ABSOLUTE_INDEXED_Y }, // CMP - Absolute,Y
   { 0xDA, "", AM_IMPLIED }, // Future Expansion
   { 0xDB, "", AM_IMPLIED }, // Future Expansion
   { 0xDC, "", AM_IMPLIED }, // Future Expansion
   { 0xDD, "CMP", AM_ABSOLUTE_INDEXED_X }, // CMP - Absolute,X
   { 0xDE, "DEC", AM_ABSOLUTE_INDEXED_X }, // DEC - Absolute,X
   { 0xDF, "", AM_IMPLIED }, // Future Expansion
   { 0xE0, "CPX", AM_IMMEDIATE }, // CPX - Immediate
   { 0xE1, "SBC", AM_PREINDEXED_INDIRECT }, // SBC - (Indirect,X)
   { 0xE2, "", AM_IMPLIED }, // Future Expansion
   { 0xE3, "", AM_IMPLIED }, // Future Expansion
   { 0xE4, "CPX", AM_ZEROPAGE }, // CPX - Zero Page
   { 0xE5, "SBC", AM_ZEROPAGE }, // SBC - Zero Page
   { 0xE6, "INC", AM_ZEROPAGE }, // INC - Zero Page
   { 0xE7, "", AM_IMPLIED }, // Future Expansion
   { 0xE8, "INX", AM_IMPLIED }, // INX
   { 0xE9, "SBC", AM_IMMEDIATE }, // SBC - Immediate
   { 0xEA, "NOP", AM_IMPLIED }, // NOP
   { 0xEB, "", AM_IMPLIED }, // Future Expansion
   { 0xEC, "CPX", AM_ABSOLUTE }, // CPX - Absolute
   { 0xED, "SBC", AM_ABSOLUTE }, // SBC - Absolute
   { 0xEE, "INC", AM_ABSOLUTE }, // INC - Absolute
   { 0xEF, "", AM_IMPLIED }, // Future Expansion
   { 0xF0, "BEQ", AM_RELATIVE }, // BEQ
   { 0xF1, "SBC", AM_POSTINDEXED_INDIRECT }, // SBC - (Indirect),Y
   { 0xF2, "", AM_IMPLIED }, // Future Expansion
   { 0xF3, "", AM_IMPLIED }, // Future Expansion
   { 0xF4, "", AM_IMPLIED }, // Future Expansion
   { 0xF5, "SBC", AM_ZEROPAGE_INDEXED_X }, // SBC - Zero Page,X
   { 0xF6, "INC", AM_ZEROPAGE_INDEXED_X }, // INC - Zero Page,X
   { 0xF7, "", AM_IMPLIED }, // Future Expansion
   { 0xF8, "SED", AM_IMPLIED }, // SED
   { 0xF9, "SBC", AM_ABSOLUTE_INDEXED_Y }, // SBC - Absolute,Y
   { 0xFA, "", AM_IMPLIED }, // Future Expansion
   { 0xFB, "", AM_IMPLIED }, // Future Expansion
   { 0xFC, "", AM_IMPLIED }, // Future Expansion
   { 0xFD, "SBC", AM_ABSOLUTE_INDEXED_X }, // SBC - Absolute,X
   { 0xFE, "INC", AM_ABSOLUTE_INDEXED_X }, // INC - Absolute,X
   { 0xFF, "", AM_IMPLIED }  // Future Expansion
};

#endif
