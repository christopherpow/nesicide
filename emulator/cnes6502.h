// cnes6502.h: interface for the C6502 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NESCPU_H )
#define NESCPU_H

#include "cnesicidecommon.h"

#include "ctracer.h"
#include "ccodedatalogger.h"
#include "cregisterdata.h"

#include "cnes.h"

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
#define wA(a) { m_a = a; CNES::CHECKBREAKPOINT(eBreakInCPU); }

#define rX() m_x
#define wX(x) { m_x = x; CNES::CHECKBREAKPOINT(eBreakInCPU); }

#define rY() m_y
#define wY(y) { m_y = y; CNES::CHECKBREAKPOINT(eBreakInCPU); }

#define rF() m_f
#define wF(f) { m_f = f; CNES::CHECKBREAKPOINT(eBreakInCPU); }

#define rN() (m_f&FLAG_N)
#define rV() (m_f&FLAG_V)
#define rB() (m_f&FLAG_B)
#define rD() (m_f&FLAG_D)
#define rI() (m_f&FLAG_I)
#define rZ() (m_f&FLAG_Z)
#define rC() (m_f&FLAG_C)
#define sN() { m_f|=FLAG_N; }
#define cN() { m_f&=~(FLAG_N); }
#define wN(set) { m_f&=(~(FLAG_N)); m_f|=((!!(set))<<FLAG_N_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sV() { m_f|=FLAG_V; }
#define cV() { m_f&=~(FLAG_V); }
#define wV(set) { m_f&=(~(FLAG_V)); m_f|=((!!(set))<<FLAG_V_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sB() { m_f|=FLAG_B; }
#define cB() { m_f&=~(FLAG_B); }
#define wB(set) { m_f&=(~(FLAG_B)); m_f|=((!!(set))<<FLAG_B_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sD() { m_f|=FLAG_D; }
#define cD() { m_f&=~(FLAG_D); }
#define wD(set) { m_f&=(~(FLAG_D)); m_f|=((!!(set))<<FLAG_D_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sI() { m_f|=FLAG_I; }
#define cI() { m_f&=~(FLAG_I); }
#define wI(set) { m_f&=(~(FLAG_I)); m_f|=((!!(set))<<FLAG_I_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sZ() { m_f|=FLAG_Z; }
#define cZ() { m_f&=~(FLAG_Z); }
#define wZ(set) { m_f&=(~(FLAG_Z)); m_f|=((!!(set))<<FLAG_Z_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }
#define sC() { m_f|=FLAG_C; }
#define cC() { m_f&=~(FLAG_C); }
#define wC(set) { m_f&=(~(FLAG_C)); m_f|=((!!(set))<<FLAG_C_SHIFT); CNES::CHECKBREAKPOINT(eBreakInCPU); }

class C6502  
{
public:
	C6502();

   // Disassembly routines for display
//   static void Disassemble ( QString& dis8000, QString& disC000, qint8* szBinary, int len, bool decorate );
   static char* Disassemble ( unsigned char* pOpcode, char* buffer );
   static char* MakePrintableBinaryText ( void );

   // Emulation routines
   static void EMULATE ( bool bRun, int cycles );
   static void GOTO ( UINT pcGoto ) { m_pcGoto = pcGoto; }
   static void GOTO () { m_pcGoto = 0xFFFFFFFF; }
   static unsigned char STEP ( void );

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

   static inline CTracer* TRACER ( void ) { return &m_tracer; }
   static inline CCodeDataLogger& LOGGER ( void ) { return m_logger; }
   static inline unsigned int CYCLES ( void ) { return m_cycles; }

   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static inline int NUMREGISTERS ( void ) { return m_numRegisters; }

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

   static CTracer         m_tracer;
   static CCodeDataLogger m_logger;
   static unsigned int    m_cycles;
   static int             m_curCycles; // must be allowed to go negative!

   static int             amode; // TODO: rename!
   static unsigned char*  data; // TODO: rename!

   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;
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

#endif // !defined(AFX_6502_H__05B65C6F_076A_4975_9B2A_E7E2F4466F0A__INCLUDED_)
