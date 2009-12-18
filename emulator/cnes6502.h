// cnes6502.h: interface for the C6502 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NESCPU_H )
#define NESCPU_H

#include "cnesicidecommon.h"

#include "ctracer.h"
#include "ccodedatalogger.h"

#define VECTOR_IRQ   0xFFFE
#define VECTOR_RESET 0xFFFC
#define VECTOR_NMI   0xFFFA

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80
#define FLAG_C_SHIFT 0
#define FLAG_Z_SHIFT 1
#define FLAG_I_SHIFT 2
#define FLAG_D_SHIFT 3
#define FLAG_B_SHIFT 4
#define FLAG_V_SHIFT 6
#define FLAG_N_SHIFT 7
#define FLAG_MISC 0x20

#define FLAG_CLEAR 0x20

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

#define SCANLINES_VISIBLE (240)
#define SCANLINES_VBLANK_NTSC (20)
#define SCANLINES_VBLANK_PAL (70)
#define SCANLINES_MISC (2)
#define SCANLINES_NTSC (SCANLINES_VISIBLE+SCANLINES_VBLANK_NTSC+SCANLINES_MISC)
#define SCANLINES_PAL (SCANLINES_VISIBLE+SCANLINES_VBLANK_PAL+SCANLINES_MISC)
#define PPU_CYCLES_PER_SCANLINE (341)
#define PPU_CYCLES_PER_FRAME_NTSC (PPU_CYCLES_PER_SCANLINE*SCANLINES_NTSC)
#define PPU_CYCLES_PER_FRAME_PAL (PPU_CYCLES_PER_SCANLINE*SCANLINES_PAL)
#define NTSC_PPU_CPU_REL (3)
#define PAL_PPU_CPU_REL  (3.2)

#define MAKE16(lo,hi) (((lo&0xFF)|((hi&0xFF)<<8)))
#define GETSIGNED8(data,op) ((char)((*(data+op))&0xFF))
#define GETUNSIGNED8(data,op) ((unsigned char)((*(data+op))&0xFF))
#define GETSTACKADDR() (MAKE16(rSP(),0x01))
#define GETSTACKDATA() (MEM(GETSTACKADDR()))
#define GETHI8(wd) ((wd>>8)&0xFF)
#define GETLO8(wd) (wd&0xFF)

#define rPC() (UINT)m_pc
#define wPC(pc) m_pc = (pc)
#define INCPC() { m_pc++; }
#define DECPC() { m_pc--; }

#define rSP() m_sp
#define wSP(sp) m_sp = (sp)
#define DECSP() m_sp--
#define INCSP() m_sp++
#define PUSH(data) { MEM(GETSTACKADDR(),data); DECSP(); }

#define rEA() m_ea

#define rA() m_a
#define wA(a) { m_a = a; CHECKBRKPT(); }

#define rX() m_x
#define wX(x) { m_x = x; CHECKBRKPT(); }

#define rY() m_y
#define wY(y) { m_y = y; CHECKBRKPT(); }

#define rF() m_f
#define wF(f) { m_f = f; CHECKBRKPT(); }

#define rN() (m_f&FLAG_N)
#define rV() (m_f&FLAG_V)
#define rB() (m_f&FLAG_B)
#define rD() (m_f&FLAG_D)
#define rI() (m_f&FLAG_I)
#define rZ() (m_f&FLAG_Z)
#define rC() (m_f&FLAG_C)
#define sN() { m_f|=FLAG_N; }
#define cN() { m_f&=~(FLAG_N); }
#define wN(set) { m_f&=(~(FLAG_N)); m_f|=((!!(set))<<FLAG_N_SHIFT); CHECKBRKPT(); }
#define sV() { m_f|=FLAG_V; }
#define cV() { m_f&=~(FLAG_V); }
#define wV(set) { m_f&=(~(FLAG_V)); m_f|=((!!(set))<<FLAG_V_SHIFT); CHECKBRKPT(); }
#define sB() { m_f|=FLAG_B; }
#define cB() { m_f&=~(FLAG_B); }
#define wB(set) { m_f&=(~(FLAG_B)); m_f|=((!!(set))<<FLAG_B_SHIFT); CHECKBRKPT(); }
#define sD() { m_f|=FLAG_D; }
#define cD() { m_f&=~(FLAG_D); }
#define wD(set) { m_f&=(~(FLAG_D)); m_f|=((!!(set))<<FLAG_D_SHIFT); CHECKBRKPT(); }
#define sI() { m_f|=FLAG_I; }
#define cI() { m_f&=~(FLAG_I); }
#define wI(set) { m_f&=(~(FLAG_I)); m_f|=((!!(set))<<FLAG_I_SHIFT); CHECKBRKPT(); }
#define sZ() { m_f|=FLAG_Z; }
#define cZ() { m_f&=~(FLAG_Z); }
#define wZ(set) { m_f&=(~(FLAG_Z)); m_f|=((!!(set))<<FLAG_Z_SHIFT); CHECKBRKPT(); }
#define sC() { m_f|=FLAG_C; }
#define cC() { m_f&=~(FLAG_C); }
#define wC(set) { m_f&=(~(FLAG_C)); m_f|=((!!(set))<<FLAG_C_SHIFT); CHECKBRKPT(); }

class C6502  
{
public:
	C6502();

   // Disassembly routines for display
//   static void Disassemble ( QString& dis8000, QString& disC000, qint8* szBinary, int len, bool decorate );
//   static char* Disassemble ( unsigned char* pOpcode, char* buffer );
   static char* MakePrintableBinaryText ( void );

   // Emulation routines
   static bool EMULATE ( bool bRun, int cycles );
   static void GOTO ( UINT pcGoto ) { m_pcGoto = pcGoto; }
   static void GOTO () { m_pcGoto = 0xFFFFFFFF; }
   static unsigned char STEP ( bool* bBrkptHit );

   static void RESET ( void );
   static void IRQ ( char source );
   static void NMI ( char source );

   static void ADC ( void );
   static void AND ( void );
   static void ASL ( void );
   static void ASO ( void );
   static void AXS ( void );
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
   static void DCM ( void );
   static void DEC ( void );
   static void DEX ( void );
   static void DEY ( void );
   static void EOR ( void );
   static void INC ( void );
   static void INS ( void );
   static void INX ( void );
   static void INY ( void );
   static void JMP ( void );
   static void JSR ( void );
   static void LAX ( void );
   static void LDA ( void );
   static void LDY ( void );
   static void LDX ( void );
   static void LSE ( void );
   static void LSR ( void );
   static void NOP ( void );
   static void ORA ( void );
   static void PHA ( void );
   static void PHP ( void );
   static void PLA ( void );
   static void PLP ( void );
   static void ROL ( void );
   static void ROR ( void );
   static void RLA ( void );
   static void RRA ( void );
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

   static void KIL ( void );
   static void ANC ( void );
   static void ALR ( void );
   static void ARR ( void );
   static void XAA ( void );
   static void AXA ( void );
   static void TAS ( void );
   static void SAY ( void );
   static void XAS ( void );
   static void OAL ( void );
   static void LAS ( void );
   static void SAX ( void );

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

   static unsigned char MEM ( UINT addr );
   static unsigned char DMA ( UINT addr, char source );
   static unsigned char _DMA ( UINT addr );
   static unsigned char FETCH ( UINT addr );
   static unsigned char LOAD ( UINT addr, bool checkBrkpt, char* pTarget );
   static void STORE ( UINT addr, unsigned char data, bool checkBrkpt, char* pTarget );
   static unsigned char _MEM ( UINT addr );
   static void MEM ( UINT addr, unsigned char data );
   static void _MEM ( UINT addr, unsigned char data );
   static void MEMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_6502memory+addr,data,length); };
   static void MEMCLR ( void ) { memset(m_6502memory,0,MEM_2KB); }

   static inline UINT MAKEADDR ( int amode, unsigned char* data );

   static void SETBRKPT ( int type, UINT addr, UINT addr2, int iIf, UINT cond ) 
   { m_brkptType = type; m_brkptAddr = addr; m_brkptAddr2 = addr2; m_brkptIf = iIf; m_brkptCond = cond; }
   static bool ISBRKPT ( void ) { bool hit = m_brkptHit; m_brkptHit = false; return hit; }
   static void CHECKBRKPT ( void );

   static inline CTracer& TRACER ( void ) { return m_tracer; }
   static inline CCodeDataLogger& LOGGER ( void ) { return m_logger; }
   static inline unsigned int CYCLES ( void ) { return m_cycles; }

protected:
   static bool            m_killed;
   static unsigned char   m_6502memory [ MEM_2KB ];
   static char            m_szBinaryText [ 6913 ]; // 54-bytes per line for 128 lines + 1 null
   static unsigned char   m_a;
   static unsigned char   m_x;
   static unsigned char   m_y;
   static unsigned char   m_f;
   static unsigned short  m_pc;
   static unsigned char   m_sp;
   static unsigned int    m_ea;
   static UINT            m_pcGoto;

   static UINT            m_brkptAddr;
   static UINT            m_brkptAddr2;
   static int             m_brkptType;
   static UINT            m_brkptCond;
   static int             m_brkptIf;
   static bool            m_brkptHit;

   static CTracer         m_tracer;
   static CCodeDataLogger m_logger;
   static unsigned int    m_cycles;
   static int             m_curCycles; // must be allowed to go negative!

   static int             amode; // TODO: rename!
   static unsigned char*  data; // TODO: rename!
};

typedef struct _C6502_opcode
{
   int op;
   const char* name;
   void (*pFn)(void);
   int amode;
   int cycles;
   bool documented;
} C6502_opcode;

static int opcode_size [ NUM_ADDRESSING_MODES ] =
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

static const char* dispFmt[4] =
{
   "",
   "%02X       ",
   "%02X %02X    ",
   "%02X %02X %02X "
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
   { 0x00, "BRK", C6502::BRK, AM_IMPLIED, 7, true }, // BRK
   { 0x01, "ORA", C6502::ORA, AM_PREINDEXED_INDIRECT, 6, true }, // ORA - (Indirect,X)         
   { 0x02, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0x03, "ASO", C6502::ASO, AM_PREINDEXED_INDIRECT, 8, false }, // ASO - (Indirect,X) (undocumented)           
   { 0x04, "DOP", C6502::NOP, AM_ZEROPAGE, 3, false }, // DOP (undocumented) 
   { 0x05, "ORA", C6502::ORA, AM_ZEROPAGE, 3, true }, // ORA - Zero Page            
   { 0x06, "ASL", C6502::ASL, AM_ZEROPAGE, 5, true }, // ASL - Zero Page            
   { 0x07, "ASO", C6502::ASO, AM_ZEROPAGE, 5, false }, // ASO - Zero Page (undocumented)           
   { 0x08, "PHP", C6502::PHP, AM_IMPLIED, 3, true }, // PHP                        
   { 0x09, "ORA", C6502::ORA, AM_IMMEDIATE, 2, true }, // ORA - Immediate            
   { 0x0A, "ASL", C6502::ASL, AM_ACCUMULATOR, 2, true }, // ASL - Accumulator          
   { 0x0B, "ANC", C6502::ANC, AM_IMMEDIATE, 2, false }, // ANC - Immediate (undocumented)           
   { 0x0C, "TOP", C6502::NOP, AM_ABSOLUTE, 4, false }, // TOP (undocumented)           
   { 0x0D, "ORA", C6502::ORA, AM_ABSOLUTE, 4, true }, // ORA - Absolute             
   { 0x0E, "ASL", C6502::ASL, AM_ABSOLUTE, 6, true }, // ASL - Absolute             
   { 0x0F, "ASO", C6502::ASO, AM_ABSOLUTE, 6, false }, // ASO - Absolute (undocumented)           
   { 0x10, "BPL", C6502::BPL, AM_RELATIVE, 2, true }, // BPL                        
   { 0x11, "ORA", C6502::ORA, AM_POSTINDEXED_INDIRECT, 5, true }, // ORA - (Indirect),Y         
   { 0x12, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0x13, "ASO", C6502::ASO, AM_POSTINDEXED_INDIRECT, 8, false }, // ASO - (Indirect),Y (undocumented)           
   { 0x14, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0x15, "ORA", C6502::ORA, AM_ZEROPAGE_INDEXED_X, 4, true }, // ORA - Zero Page,X          
   { 0x16, "ASL", C6502::ASL, AM_ZEROPAGE_INDEXED_X, 6, true }, // ASL - Zero Page,X          
   { 0x17, "ASO", C6502::ASO, AM_ZEROPAGE_INDEXED_X, 6, false }, // ASO - Zero Page,X (undocumented)           
   { 0x18, "CLC", C6502::CLC, AM_IMPLIED, 2, true }, // CLC                        
   { 0x19, "ORA", C6502::ORA, AM_ABSOLUTE_INDEXED_Y, 4, true }, // ORA - Absolute,Y           
   { 0x1A, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0x1B, "ASO", C6502::ASO, AM_ABSOLUTE_INDEXED_Y, 7, false }, // ASO - Absolute,Y (undocumented)           
   { 0x1C, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0x1D, "ORA", C6502::ORA, AM_ABSOLUTE_INDEXED_X, 4, true }, // ORA - Absolute,X 
   { 0x1E, "ASL", C6502::ASL, AM_ABSOLUTE_INDEXED_X, 7, true }, // ASL - Absolute,X           
   { 0x1F, "ASO", C6502::ASO, AM_ABSOLUTE_INDEXED_X, 7, false }, // ASO - Absolute,X (undocumented)           
   { 0x20, "JSR", C6502::JSR, AM_ABSOLUTE, 6, true }, // JSR
   { 0x21, "AND", C6502::AND, AM_PREINDEXED_INDIRECT, 6, true }, // AND - (Indirect,X)
   { 0x22, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0x23, "RLA", C6502::RLA, AM_PREINDEXED_INDIRECT, 8, false }, // RLA - (Indirect,X) (undocumented)
   { 0x24, "BIT", C6502::BIT, AM_ZEROPAGE, 3, true }, // BIT - Zero Page
   { 0x25, "AND", C6502::AND, AM_ZEROPAGE, 3, true }, // AND - Zero Page
   { 0x26, "ROL", C6502::ROL, AM_ZEROPAGE, 5, true }, // ROL - Zero Page
   { 0x27, "RLA", C6502::RLA, AM_ZEROPAGE, 5, false }, // RLA - Zero Page (undocumented)
   { 0x28, "PLP", C6502::PLP, AM_IMPLIED, 4, true }, // PLP
   { 0x29, "AND", C6502::AND, AM_IMMEDIATE, 2, true }, // AND - Immediate
   { 0x2A, "ROL", C6502::ROL, AM_ACCUMULATOR, 2, true }, // ROL - Accumulator
   { 0x2B, "ANC", C6502::ANC, AM_IMMEDIATE, 2, false }, // ANC - Immediate (undocumented)
   { 0x2C, "BIT", C6502::BIT, AM_ABSOLUTE, 4, true }, // BIT - Absolute
   { 0x2D, "AND", C6502::AND, AM_ABSOLUTE, 4, true }, // AND - Absolute
   { 0x2E, "ROL", C6502::ROL, AM_ABSOLUTE, 6, true }, // ROL - Absolute
   { 0x2F, "RLA", C6502::RLA, AM_ABSOLUTE, 6, false }, // RLA - Absolute (undocumented)
   { 0x30, "BMI", C6502::BMI, AM_RELATIVE, 2, true }, // BMI
   { 0x31, "AND", C6502::AND, AM_POSTINDEXED_INDIRECT, 5, true }, // AND - (Indirect),Y
   { 0x32, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0x33, "RLA", C6502::RLA, AM_POSTINDEXED_INDIRECT, 8, false }, // RLA - (Indirect),Y (undocumented)
   { 0x34, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0x35, "AND", C6502::AND, AM_ZEROPAGE_INDEXED_X, 4, true }, // AND - Zero Page,X
   { 0x36, "ROL", C6502::ROL, AM_ZEROPAGE_INDEXED_X, 6, true }, // ROL - Zero Page,X
   { 0x37, "RLA", C6502::RLA, AM_ZEROPAGE_INDEXED_X, 6, false }, // RLA - Zero Page,X (undocumented)
   { 0x38, "SEC", C6502::SEC, AM_IMPLIED, 2, true }, // SEC
   { 0x39, "AND", C6502::AND, AM_ABSOLUTE_INDEXED_Y, 4, true }, // AND - Absolute,Y
   { 0x3A, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0x3B, "RLA", C6502::RLA, AM_ABSOLUTE_INDEXED_Y, 7, false }, // RLA - Absolute,Y (undocumented)
   { 0x3C, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0x3D, "AND", C6502::AND, AM_ABSOLUTE_INDEXED_X, 4, true }, // AND - Absolute,X
   { 0x3E, "ROL", C6502::ROL, AM_ABSOLUTE_INDEXED_X, 7, true }, // ROL - Absolute,X
   { 0x3F, "RLA", C6502::RLA, AM_ABSOLUTE_INDEXED_X, 7, false }, // RLA - Absolute,X (undocumented)
   { 0x40, "RTI", C6502::RTI, AM_IMPLIED, 6, true }, // RTI                        
   { 0x41, "EOR", C6502::EOR, AM_PREINDEXED_INDIRECT, 6, true }, // EOR - (Indirect,X)         
   { 0x42, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0x43, "LSE", C6502::LSE, AM_PREINDEXED_INDIRECT, 8, false }, // LSE - (Indirect,X) (undocumented)           
   { 0x44, "DOP", C6502::NOP, AM_ZEROPAGE, 3, false }, // DOP (undocumented) 
   { 0x45, "EOR", C6502::EOR, AM_ZEROPAGE, 3, true }, // EOR - Zero Page            
   { 0x46, "LSR", C6502::LSR, AM_ZEROPAGE, 5, true }, // LSR - Zero Page            
   { 0x47, "LSE", C6502::LSE, AM_ZEROPAGE, 5, false }, // LSE - Zero Page (undocumented)          
   { 0x48, "PHA", C6502::PHA, AM_IMPLIED, 3, true }, // PHA                        
   { 0x49, "EOR", C6502::EOR, AM_IMMEDIATE, 2, true }, // EOR - Immediate            
   { 0x4A, "LSR", C6502::LSR, AM_ACCUMULATOR, 2, true }, // LSR - Accumulator          
   { 0x4B, "ALR", C6502::ALR, AM_IMMEDIATE, 2, false }, // ALR - Immediate (undocumented)          
   { 0x4C, "JMP", C6502::JMP, AM_ABSOLUTE, 3, true }, // JMP - Absolute
   { 0x4D, "EOR", C6502::EOR, AM_ABSOLUTE, 4, true }, // EOR - Absolute             
   { 0x4E, "LSR", C6502::LSR, AM_ABSOLUTE, 6, true }, // LSR - Absolute             
   { 0x4F, "LSE", C6502::LSE, AM_ABSOLUTE, 6, false }, // LSE - Absolute (undocumented)           
   { 0x50, "BVC", C6502::BVC, AM_RELATIVE, 2, true }, // BVC                        
   { 0x51, "EOR", C6502::EOR, AM_POSTINDEXED_INDIRECT, 5, true }, // EOR - (Indirect),Y         
   { 0x52, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0x53, "LSE", C6502::LSE, AM_POSTINDEXED_INDIRECT, 8, false }, // LSE - (Indirect),Y           
   { 0x54, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0x55, "EOR", C6502::EOR, AM_ZEROPAGE_INDEXED_X, 4, true }, // EOR - Zero Page,X          
   { 0x56, "LSR", C6502::LSR, AM_ZEROPAGE_INDEXED_X, 6, true }, // LSR - Zero Page,X          
   { 0x57, "LSE", C6502::LSE, AM_ZEROPAGE_INDEXED_X, 6, false }, // LSE - Zero Page,X (undocumented)           
   { 0x58, "CLI", C6502::CLI, AM_IMPLIED, 2, true }, // CLI                        
   { 0x59, "EOR", C6502::EOR, AM_ABSOLUTE_INDEXED_Y, 4, true }, // EOR - Absolute,Y           
   { 0x5A, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0x5B, "LSE", C6502::LSE, AM_ABSOLUTE_INDEXED_Y, 7, false }, // LSE - Absolute,Y (undocumented)           
   { 0x5C, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0x5D, "EOR", C6502::EOR, AM_ABSOLUTE_INDEXED_X, 4, true }, // EOR - Absolute,X           
   { 0x5E, "LSR", C6502::LSR, AM_ABSOLUTE_INDEXED_X, 7, true }, // LSR - Absolute,X           
   { 0x5F, "LSE", C6502::LSE, AM_ABSOLUTE_INDEXED_X, 7, false }, // LSE - Absolute,X (undocumented)           
   { 0x60, "RTS", C6502::RTS, AM_IMPLIED, 6, true }, // RTS
   { 0x61, "ADC", C6502::ADC, AM_PREINDEXED_INDIRECT, 6, true }, // ADC - (Indirect,X)
   { 0x62, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0x63, "RRA", C6502::RRA, AM_PREINDEXED_INDIRECT, 8, false }, // RRA - (Indirect,X) (undocumented)
   { 0x64, "DOP", C6502::NOP, AM_ZEROPAGE, 3, false }, // DOP (undocumented) 
   { 0x65, "ADC", C6502::ADC, AM_ZEROPAGE, 3, true }, // ADC - Zero Page
   { 0x66, "ROR", C6502::ROR, AM_ZEROPAGE, 5, true }, // ROR - Zero Page
   { 0x67, "RRA", C6502::RRA, AM_ZEROPAGE, 5, false }, // RRA - Zero Page (undocumented)
   { 0x68, "PLA", C6502::PLA, AM_IMPLIED, 4, true }, // PLA
   { 0x69, "ADC", C6502::ADC, AM_IMMEDIATE, 2, true }, // ADC - Immediate
   { 0x6A, "ROR", C6502::ROR, AM_ACCUMULATOR, 2, true }, // ROR - Accumulator
   { 0x6B, "ARR", C6502::ARR, AM_IMMEDIATE, 2, false }, // ARR - Immediate (undocumented)
   { 0x6C, "JMP", C6502::JMP, AM_INDIRECT, 5, true }, // JMP - Indirect
   { 0x6D, "ADC", C6502::ADC, AM_ABSOLUTE, 4, true }, // ADC - Absolute
   { 0x6E, "ROR", C6502::ROR, AM_ABSOLUTE, 6, true }, // ROR - Absolute
   { 0x6F, "RRA", C6502::RRA, AM_ABSOLUTE, 6, false }, // RRA - Absolute (undocumented)
   { 0x70, "BVS", C6502::BVS, AM_RELATIVE, 2, true }, // BVS
   { 0x71, "ADC", C6502::ADC, AM_POSTINDEXED_INDIRECT, 5, true }, // ADC - (Indirect),Y
   { 0x72, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0x73, "RRA", C6502::RRA, AM_POSTINDEXED_INDIRECT, 8, false }, // RRA - (Indirect),Y (undocumented)
   { 0x74, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0x75, "ADC", C6502::ADC, AM_ZEROPAGE_INDEXED_X, 4, true }, // ADC - Zero Page,X
   { 0x76, "ROR", C6502::ROR, AM_ZEROPAGE_INDEXED_X, 6, true }, // ROR - Zero Page,X
   { 0x77, "RRA", C6502::RRA, AM_ZEROPAGE_INDEXED_X, 6, false }, // RRA - Zero Page,X (undocumented)
   { 0x78, "SEI", C6502::SEI, AM_IMPLIED, 2, true }, // SEI
   { 0x79, "ADC", C6502::ADC, AM_ABSOLUTE_INDEXED_Y, 4, true }, // ADC - Absolute,Y
   { 0x7A, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0x7B, "RRA", C6502::RRA, AM_ABSOLUTE_INDEXED_Y, 7, false }, // RRA - Absolute,Y (undocumented)
   { 0x7C, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0x7D, "ADC", C6502::ADC, AM_ABSOLUTE_INDEXED_X, 4, true }, // ADC - Absolute,X
   { 0x7E, "ROR", C6502::ROR, AM_ABSOLUTE_INDEXED_X, 7, true }, // ROR - Absolute,X
   { 0x7F, "RRA", C6502::RRA, AM_ABSOLUTE_INDEXED_X, 7, false }, // RRA - Absolute,X (undocumented)
   { 0x80, "DOP", C6502::NOP, AM_IMMEDIATE, 2, false }, // DOP (undocumented) 
   { 0x81, "STA", C6502::STA, AM_PREINDEXED_INDIRECT, 6, true }, // STA - (Indirect,X)         
   { 0x82, "DOP", C6502::NOP, AM_IMMEDIATE, 2, false }, // DOP (undocumented) 
   { 0x83, "AXS", C6502::AXS, AM_PREINDEXED_INDIRECT, 6, false }, // AXS - (Indirect,X) (undocumented)           
   { 0x84, "STY", C6502::STY, AM_ZEROPAGE, 3, true }, // STY - Zero Page            
   { 0x85, "STA", C6502::STA, AM_ZEROPAGE, 3, true }, // STA - Zero Page            
   { 0x86, "STX", C6502::STX, AM_ZEROPAGE, 3, true }, // STX - Zero Page            
   { 0x87, "AXS", C6502::AXS, AM_ZEROPAGE, 3, false }, // AXS - Zero Page (undocumented)           
   { 0x88, "DEY", C6502::DEY, AM_IMPLIED, 2, true }, // DEY                        
   { 0x89, "DOP", C6502::NOP, AM_IMMEDIATE, 2, false }, // DOP (undocumented) 
   { 0x8A, "TXA", C6502::TXA, AM_IMPLIED, 2, true }, // TXA                        
   { 0x8B, "XAA", C6502::XAA, AM_IMMEDIATE, 2, false }, // XAA - Immediate (undocumented)           
   { 0x8C, "STY", C6502::STY, AM_ABSOLUTE, 4, true }, // STY - Absolute             
   { 0x8D, "STA", C6502::STA, AM_ABSOLUTE, 4, true }, // STA - Absolute             
   { 0x8E, "STX", C6502::STX, AM_ABSOLUTE, 4, true }, // STX - Absolute             
   { 0x8F, "AXS", C6502::AXS, AM_ABSOLUTE, 4, false }, // AXS - Absolulte (undocumented)           
   { 0x90, "BCC", C6502::BCC, AM_RELATIVE, 2, true }, // BCC                        
   { 0x91, "STA", C6502::STA, AM_POSTINDEXED_INDIRECT, 6, true }, // STA - (Indirect),Y         
   { 0x92, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0x93, "AXA", C6502::AXA, AM_POSTINDEXED_INDIRECT, 6, false }, // AXA - (Indirect),Y           
   { 0x94, "STY", C6502::STY, AM_ZEROPAGE_INDEXED_X, 4, true }, // STY - Zero Page,X          
   { 0x95, "STA", C6502::STA, AM_ZEROPAGE_INDEXED_X, 4, true }, // STA - Zero Page,X          
   { 0x96, "STX", C6502::STX, AM_ZEROPAGE_INDEXED_Y, 4, true }, // STX - Zero Page,Y          
   { 0x97, "AXS", C6502::AXS, AM_ZEROPAGE_INDEXED_Y, 4, false }, // AXS - Zero Page,Y           
   { 0x98, "TYA", C6502::TYA, AM_IMPLIED, 2, true }, // TYA                        
   { 0x99, "STA", C6502::STA, AM_ABSOLUTE_INDEXED_Y, 5, true }, // STA - Absolute,Y           
   { 0x9A, "TXS", C6502::TXS, AM_IMPLIED, 2, true }, // TXS                        
   { 0x9B, "TAS", C6502::TAS, AM_ABSOLUTE_INDEXED_Y, 5, false }, // TAS - Absolute,Y (undocumented)           
   { 0x9C, "SAY", C6502::SAY, AM_ABSOLUTE_INDEXED_X, 5, false }, // SAY - Absolute,X (undocumented)           
   { 0x9D, "STA", C6502::STA, AM_ABSOLUTE_INDEXED_X, 5, true }, // STA - Absolute,X           
   { 0x9E, "XAS", C6502::XAS, AM_ABSOLUTE_INDEXED_Y, 5, false }, // XAS - Absolute,Y (undocumented)
   { 0x9F, "AXA", C6502::AXA, AM_ABSOLUTE_INDEXED_Y, 5, false }, // AXA - Absolute,Y (undocumented)          
   { 0xA0, "LDY", C6502::LDY, AM_IMMEDIATE, 2, true }, // LDY - Immediate
   { 0xA1, "LDA", C6502::LDA, AM_PREINDEXED_INDIRECT, 6, true }, // LDA - (Indirect,X)
   { 0xA2, "LDX", C6502::LDX, AM_IMMEDIATE, 2, true }, // LDX - Immediate
   { 0xA3, "LAX", C6502::LAX, AM_PREINDEXED_INDIRECT, 6, false }, // LAX - (Indirect,X) (undocumented)
   { 0xA4, "LDY", C6502::LDY, AM_ZEROPAGE, 3, true }, // LDY - Zero Page
   { 0xA5, "LDA", C6502::LDA, AM_ZEROPAGE, 3, true }, // LDA - Zero Page
   { 0xA6, "LDX", C6502::LDX, AM_ZEROPAGE, 3, true }, // LDX - Zero Page
   { 0xA7, "LAX", C6502::LAX, AM_ZEROPAGE, 3, false }, // LAX - Zero Page (undocumented)
   { 0xA8, "TAY", C6502::TAY, AM_IMPLIED, 2, true }, // TAY
   { 0xA9, "LDA", C6502::LDA, AM_IMMEDIATE, 2, true }, // LDA - Immediate
   { 0xAA, "TAX", C6502::TAX, AM_IMPLIED, 2, true }, // TAX
   { 0xAB, "OAL", C6502::OAL, AM_IMMEDIATE, 2, false }, // OAL - Immediate
   { 0xAC, "LDY", C6502::LDY, AM_ABSOLUTE, 4, true }, // LDY - Absolute
   { 0xAD, "LDA", C6502::LDA, AM_ABSOLUTE, 4, true }, // LDA - Absolute
   { 0xAE, "LDX", C6502::LDX, AM_ABSOLUTE, 4, true }, // LDX - Absolute
   { 0xAF, "LAX", C6502::LAX, AM_ABSOLUTE, 4, false }, // LAX - Absolute (undocumented)
   { 0xB0, "BCS", C6502::BCS, AM_RELATIVE, 2, true }, // BCS
   { 0xB1, "LDA", C6502::LDA, AM_POSTINDEXED_INDIRECT, 5, true }, // LDA - (Indirect),Y
   { 0xB2, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0xB3, "LAX", C6502::LAX, AM_POSTINDEXED_INDIRECT, 5, false }, // LAX - (Indirect),Y (undocumented)
   { 0xB4, "LDY", C6502::LDY, AM_ZEROPAGE_INDEXED_X, 4, true }, // LDY - Zero Page,X
   { 0xB5, "LDA", C6502::LDA, AM_ZEROPAGE_INDEXED_X, 4, true }, // LDA - Zero Page,X
   { 0xB6, "LDX", C6502::LDX, AM_ZEROPAGE_INDEXED_Y, 4, true }, // LDX - Zero Page,Y
   { 0xB7, "LAX", C6502::LAX, AM_ZEROPAGE_INDEXED_Y, 4, false }, // LAX - Zero Page,X (undocumented)
   { 0xB8, "CLV", C6502::CLV, AM_IMPLIED, 2, true }, // CLV
   { 0xB9, "LDA", C6502::LDA, AM_ABSOLUTE_INDEXED_Y, 4, true }, // LDA - Absolute,Y
   { 0xBA, "TSX", C6502::TSX, AM_IMPLIED, 2, true }, // TSX
   { 0xBB, "LAS", C6502::LAS, AM_ABSOLUTE_INDEXED_Y, 4, false }, // LAS - Absolute,Y (undocumented)
   { 0xBC, "LDY", C6502::LDY, AM_ABSOLUTE_INDEXED_X, 4, true }, // LDY - Absolute,X
   { 0xBD, "LDA", C6502::LDA, AM_ABSOLUTE_INDEXED_X, 4, true }, // LDA - Absolute,X
   { 0xBE, "LDX", C6502::LDX, AM_ABSOLUTE_INDEXED_Y, 4, true }, // LDX - Absolute,Y
   { 0xBF, "LAX", C6502::LAX, AM_ABSOLUTE_INDEXED_Y, 4, false }, // LAX - Absolute,Y (undocumented)
   { 0xC0, "CPY", C6502::CPY, AM_IMMEDIATE, 2, true }, // CPY - Immediate            
   { 0xC1, "CMP", C6502::CMP, AM_PREINDEXED_INDIRECT, 6, true }, // CMP - (Indirect,X)         
   { 0xC2, "DOP", C6502::NOP, AM_IMMEDIATE, 2, false }, // DOP (undocumented) 
   { 0xC3, "DCM", C6502::DCM, AM_PREINDEXED_INDIRECT, 8, false }, // DCM - (Indirect,X) (undocumented)         
   { 0xC4, "CPY", C6502::CPY, AM_ZEROPAGE, 3, true }, // CPY - Zero Page            
   { 0xC5, "CMP", C6502::CMP, AM_ZEROPAGE, 3, true }, // CMP - Zero Page            
   { 0xC6, "DEC", C6502::DEC, AM_ZEROPAGE, 5, true }, // DEC - Zero Page            
   { 0xC7, "DCM", C6502::DCM, AM_ZEROPAGE, 5, true }, // DCM - Zero Page (undocumented)           
   { 0xC8, "INY", C6502::INY, AM_IMPLIED, 2, true }, // INY                        
   { 0xC9, "CMP", C6502::CMP, AM_IMMEDIATE, 2, true }, // CMP - Immediate            
   { 0xCA, "DEX", C6502::DEX, AM_IMPLIED, 2, true }, // DEX                        
   { 0xCB, "SAX", C6502::SAX, AM_IMMEDIATE, 2, false }, // SAX - Immediate (undocumented)           
   { 0xCC, "CPY", C6502::CPY, AM_ABSOLUTE, 4, true }, // CPY - Absolute             
   { 0xCD, "CMP", C6502::CMP, AM_ABSOLUTE, 4, true }, // CMP - Absolute             
   { 0xCE, "DEC", C6502::DEC, AM_ABSOLUTE, 6, true }, // DEC - Absolute             
   { 0xCF, "DCM", C6502::DCM, AM_ABSOLUTE, 6, false }, // DCM - Absolute (undocumented)           
   { 0xD0, "BNE", C6502::BNE, AM_RELATIVE, 2, true }, // BNE                        
   { 0xD1, "CMP", C6502::CMP, AM_POSTINDEXED_INDIRECT, 5, true }, // CMP   (Indirect),Y         
   { 0xD2, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)           
   { 0xD3, "DCM", C6502::DCM, AM_POSTINDEXED_INDIRECT, 8, false }, // DCM - (Indirect),Y (undocumented)           
   { 0xD4, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0xD5, "CMP", C6502::CMP, AM_ZEROPAGE_INDEXED_X, 4, true }, // CMP - Zero Page,X          
   { 0xD6, "DEC", C6502::DEC, AM_ZEROPAGE_INDEXED_X, 6, true }, // DEC - Zero Page,X          
   { 0xD7, "DCM", C6502::DCM, AM_ZEROPAGE_INDEXED_X, 6, false }, // DCM - Zero Page,X (undocumented)           
   { 0xD8, "CLD", C6502::CLD, AM_IMPLIED, 2, true }, // CLD                        
   { 0xD9, "CMP", C6502::CMP, AM_ABSOLUTE_INDEXED_Y, 4, true }, // CMP - Absolute,Y           
   { 0xDA, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0xDB, "DCM", C6502::DCM, AM_ABSOLUTE_INDEXED_Y, 7, false }, // DCM - Absolute,Y (undocumented)           
   { 0xDC, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0xDD, "CMP", C6502::CMP, AM_ABSOLUTE_INDEXED_X, 4, true }, // CMP - Absolute,X           
   { 0xDE, "DEC", C6502::DEC, AM_ABSOLUTE_INDEXED_X, 7, true }, // DEC - Absolute,X           
   { 0xDF, "DCM", C6502::DCM, AM_ABSOLUTE_INDEXED_X, 7, false }, // DCM - Absolute,X (undocumented)           
   { 0xE0, "CPX", C6502::CPX, AM_IMMEDIATE, 2, true }, // CPX - Immediate
   { 0xE1, "SBC", C6502::SBC, AM_PREINDEXED_INDIRECT, 6, true }, // SBC - (Indirect,X)
   { 0xE2, "DOP", C6502::NOP, AM_IMMEDIATE, 2, false }, // DOP (undocumented) 
   { 0xE3, "INS", C6502::INS, AM_PREINDEXED_INDIRECT, 8, false }, // INS - (Indirect,X) (undocumented)
   { 0xE4, "CPX", C6502::CPX, AM_ZEROPAGE, 3, true }, // CPX - Zero Page
   { 0xE5, "SBC", C6502::SBC, AM_ZEROPAGE, 3, true }, // SBC - Zero Page
   { 0xE6, "INC", C6502::INC, AM_ZEROPAGE, 5, true }, // INC - Zero Page
   { 0xE7, "INS", C6502::INS, AM_ZEROPAGE, 5, false }, // INS - Zero Page (undocumented)
   { 0xE8, "INX", C6502::INX, AM_IMPLIED, 2, true }, // INX
   { 0xE9, "SBC", C6502::SBC, AM_IMMEDIATE, 2, true }, // SBC - Immediate
   { 0xEA, "NOP", C6502::NOP, AM_IMPLIED, 2, true }, // NOP
   { 0xEB, "SBC", C6502::SBC, AM_IMMEDIATE, 2, false }, // SBC - Immediate (undocumented)
   { 0xEC, "CPX", C6502::CPX, AM_ABSOLUTE, 4, true }, // CPX - Absolute
   { 0xED, "SBC", C6502::SBC, AM_ABSOLUTE, 4, true }, // SBC - Absolute
   { 0xEE, "INC", C6502::INC, AM_ABSOLUTE, 6, true }, // INC - Absolute
   { 0xEF, "INS", C6502::INS, AM_ABSOLUTE, 6, false }, // INS - Absolute (undocumented)
   { 0xF0, "BEQ", C6502::BEQ, AM_RELATIVE, 2, true }, // BEQ
   { 0xF1, "SBC", C6502::SBC, AM_POSTINDEXED_INDIRECT, 5, true }, // SBC - (Indirect),Y
   { 0xF2, "KIL", C6502::KIL, AM_IMPLIED, 0, false }, // KIL - Implied (processor lock up!)
   { 0xF3, "INS", C6502::INS, AM_POSTINDEXED_INDIRECT, 8, false }, // INS - (Indirect),Y (undocumented)
   { 0xF4, "DOP", C6502::NOP, AM_ZEROPAGE_INDEXED_X, 4, false }, // DOP (undocumented) 
   { 0xF5, "SBC", C6502::SBC, AM_ZEROPAGE_INDEXED_X, 4, true }, // SBC - Zero Page,X
   { 0xF6, "INC", C6502::INC, AM_ZEROPAGE_INDEXED_X, 6, true }, // INC - Zero Page,X
   { 0xF7, "INS", C6502::INS, AM_ZEROPAGE_INDEXED_X, 6, false }, // INS - Zero Page,X (undocumented)
   { 0xF8, "SED", C6502::SED, AM_IMPLIED, 2, true }, // SED
   { 0xF9, "SBC", C6502::SBC, AM_ABSOLUTE_INDEXED_Y, 4, true }, // SBC - Absolute,Y
   { 0xFA, "NOP", C6502::NOP, AM_IMPLIED, 2, false }, // NOP (undocumented)           
   { 0xFB, "INS", C6502::INS, AM_ABSOLUTE_INDEXED_Y, 7, false }, // INS - Absolute,Y (undocumented)
   { 0xFC, "TOP", C6502::NOP, AM_ABSOLUTE_INDEXED_X, 4, false }, // TOP (undocumented)           
   { 0xFD, "SBC", C6502::SBC, AM_ABSOLUTE_INDEXED_X, 4, true }, // SBC - Absolute,X
   { 0xFE, "INC", C6502::INC, AM_ABSOLUTE_INDEXED_X, 7, true }, // INC - Absolute,X
   { 0xFF, "INS", C6502::INS, AM_ABSOLUTE_INDEXED_X, 7, false }  // INS - Absolute,X (undocumented)
};

#endif // !defined(AFX_6502_H__05B65C6F_076A_4975_9B2A_E7E2F4466F0A__INCLUDED_)
