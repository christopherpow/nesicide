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
    
// 6502.cpp: implementation of the C6502 class.
//
//////////////////////////////////////////////////////////////////////

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesrom.h"
#include "cnesapu.h"
#include "cnesio.h"

#include "cnesmappers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool            C6502::m_killed = false;              // KIL opcode not executed
unsigned char   C6502::m_6502memory [] = { 0, };
char            C6502::m_szBinaryText [] = { 0, };
unsigned char   C6502::m_a = 0x00;
unsigned char   C6502::m_x = 0x00;
unsigned char   C6502::m_y = 0x00;
unsigned char   C6502::m_f = FLAG_CLEAR;
unsigned short  C6502::m_pc = VECTOR_RESET;
unsigned int    C6502::m_ea = 0xFFFFFFFF;
UINT            C6502::m_pcGoto = 0xFFFFFFFF;
unsigned char   C6502::m_sp = 0xFF;

UINT            C6502::m_brkptAddr = 0x0000;
UINT            C6502::m_brkptAddr2 = 0x0000;
int             C6502::m_brkptType = -1;
bool            C6502::m_brkptHit = false;
UINT            C6502::m_brkptCond = 0x0000;
int             C6502::m_brkptIf = -1;

CTracer         C6502::m_tracer;
CCodeDataLogger C6502::m_logger ( MEM_2KB, MASK_2KB );
unsigned int    C6502::m_cycles = 0;
int             C6502::m_curCycles = 0;

int             C6502::amode;
unsigned char*  C6502::data = NULL;

C6502::C6502()
{
}

// CPTODO: support func to print CPU memory as bintext
#if 0
char* C6502::MakePrintableBinaryText ( void )
{
   int idx1;
   char* ptr = m_szBinaryText;

   for ( idx1 = 0; idx1 < MEM_2KB; idx1++ )
   {
      if ( !(idx1&0xF) )
      {
         if ( idx1 ) 
         {
            (*ptr) = '\r'; ptr++;
            (*ptr) = '\n'; ptr++;
         }
         sprintf04xc ( &ptr, idx1 );         
      }
      if ( idx1&0xF ) { (*ptr) = ' '; ptr++; }
      sprintf02x ( &ptr, *(m_6502memory+idx1) );
   }
   (*ptr) = 0;

   return m_szBinaryText;
}
#endif

bool C6502::EMULATE ( bool bRun, int cycles )
{
   bool setbrkpt = false;
   bool brkpt = false;
   m_curCycles += cycles;
   if ( (!m_killed) && (m_curCycles > 0) )
   {
      do
      {         
         cycles = STEP ( &setbrkpt );
         if ( (m_pc == m_pcGoto) ||
              (!bRun) ) 
         {
            brkpt = true;
            m_pcGoto = 0xFFFFFFFF;
         }
         m_curCycles -= cycles;
      } while ( (!m_killed) && (m_curCycles > 0) && (!setbrkpt) && (!brkpt)  );
      if ( (bRun) && (setbrkpt) )
      {
// CPTODO: Qt method for messageboxes?  better yet...set a flag and stop emulation and let the UI handle it.
//         MessageBox ( AfxGetMainWnd()->GetSafeHwnd(), "Execution stopped due to breakpoint.", "Breakpoint", MB_ICONSTOP );
         brkpt = true;
      }
      if ( m_killed )
      {
// CPTODO: Qt method for messageboxes?  better yet...set a flag and stop emulation and let the UI handle it.
//         MessageBox ( AfxGetMainWnd()->GetSafeHwnd(), "Execution stopped due to KIL opcode.\nKIL opcodes are: $02,$12,$22,$32,$42,$52,$62,$72,$92,$B2,$D2,$F2", "CPU halted", MB_ICONSTOP );
         brkpt = true;
      }
   }

   return brkpt;
}

unsigned char C6502::STEP ( bool* bBrkptHit )
{
   UINT cycles;
   unsigned char opData [ 4 ]; // 3 opcode bytes and 1 byte for operand return data [extra cycle]
   unsigned char* pOpcode = opData;
   C6502_opcode* pOpcodeStruct;
   int opcodeSize;

   // Reset effective address...
   m_ea = 0xFFFFFFFF;

   // Fetch
   (*pOpcode) = FETCH ( m_pc );
   (*(pOpcode+3)) = 0; // no extra cycle yet
   INCPC ();
// CPTODO: configuration (registry) object removed for now...
//   if ( (CONFIG.IsIllegalsEnabled()) &&
//        (((*pOpcode) == 0x02) ||
//        ((*pOpcode) == 0x12) ||
//        ((*pOpcode) == 0x22) ||
//        ((*pOpcode) == 0x32) ||
//        ((*pOpcode) == 0x42) ||
//        ((*pOpcode) == 0x52) ||
//        ((*pOpcode) == 0x62) ||
//        ((*pOpcode) == 0x72) ||
//        ((*pOpcode) == 0x92) ||
//        ((*pOpcode) == 0xB2) ||
//        ((*pOpcode) == 0xD2) ||
//        ((*pOpcode) == 0xF2)) )
//   {
//      // KIL opcodes halt PC dead!  Simulate by moving it back...
//      DECPC ();
//   }
   (*bBrkptHit) = ISBRKPT ();

   // Get information about current opcode...
   pOpcodeStruct = m_6502opcode+(*pOpcode);
   opcodeSize = (*(opcode_size+(pOpcodeStruct->amode)));

   // Set up class data so we don't need to pass it down to each func...
   amode = pOpcodeStruct->amode;
   data = pOpcode+1;

   if ( pOpcodeStruct->documented
// ||
// CPTODO: configuration object removed for now...
//        CONFIG.IsIllegalsEnabled()
      )
   {
      // Get second opcode byte
      if ( opcodeSize > 1 )
      {
         (*(pOpcode+1)) = FETCH ( m_pc );
         INCPC ();
         if ( !(*bBrkptHit) )
         {
            (*bBrkptHit) = ISBRKPT ();
         }
      }
      // Get third opcode byte
      if ( opcodeSize > 2 )
      {
         (*(pOpcode+2)) = FETCH ( m_pc );
         INCPC ();
         if ( !(*bBrkptHit) )
         {
            (*bBrkptHit) = ISBRKPT ();
         }
      }

      // Update Tracer
      TracerInfo* pSample = m_tracer.SetDisassembly ( pOpcode ); 
      m_tracer.SetRegisters ( pSample, rA(), rX(), rY(), rSP(), rF() );

      // Execute
      pOpcodeStruct->pFn ();

      // Update Tracer
      m_tracer.SetEffectiveAddress ( pSample, rEA() );
   }

   if ( !(*bBrkptHit) )
   {
      (*bBrkptHit) = ISBRKPT ();
   }

   cycles = pOpcodeStruct->cycles;
   cycles += (*(pOpcode+3)); // use extra cycle indication from opcode execution

   m_cycles += cycles;

   return cycles;
}

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
   wA ( rA()&(*data) );
   wX ( rA() );
   wN ( rA()&0x80 );
   wZ ( !rA() );

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
   unsigned short addr;

   addr = MAKEADDR ( amode, data );
   wA ( rA()&MEM(addr) );
   wX ( rA() );
   wF ( rA() );
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
   return;
}

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
   unsigned short addr;

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
   unsigned short addr = 0x0000;
   unsigned short val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }
   val <<= 1;
   wC ( val&0x100 );
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );
   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (unsigned char)val );
   }
   else
   {
      MEM ( addr, (unsigned char)val );
   }

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
   unsigned short addr;
   unsigned short val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val <<= 1;
   MEM ( addr, (unsigned char)val );
   wC ( val&0x100 );
   val &= 0xFF;
   wA ( rA()|val );
   wN ( rA()&0x80 );
   wZ ( !rA() );

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
   unsigned short addr;

   addr = MAKEADDR ( amode, data );

   MEM ( addr, rA()&rX() );

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
   PUSH ( rF()|FLAG_MISC );

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( !rN() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   UINT gopc = rPC()-1; // This was +2 but since PC is incremented in FETCH now it needs to be -1
   PUSH ( GETHI8(gopc) );
   PUSH ( GETLO8(gopc) );
   wPC ( MAKE16(GETUNSIGNED8(data,0),GETUNSIGNED8(data,1)) );

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
   unsigned short addr;

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
   unsigned short addr;
   unsigned char  val;

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
   unsigned short addr = 0x0000;
   unsigned short val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }
   val <<= 1;
   val |= rC();
   wC ( val&0x100 );
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );
   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (unsigned char)val );
   }
   else
   {
      MEM ( addr, (unsigned char)val );
   }

   return;
}

//  PLP               PLP Pull processor status from stack                PLA
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
   wF ( POP()|FLAG_MISC );

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( rN() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   unsigned char pclo;
   wF ( POP() );
   pclo = POP ();
   wPC ( MAKE16(pclo,POP()) );

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
   unsigned short addr;

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
   unsigned short addr;
   unsigned short val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   MEM ( addr, (unsigned char)val );
   wA ( rA()^val );
   wN ( rA()&0x80 );
   wZ ( !rA() );

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
   unsigned short addr = 0x0000;
   unsigned short val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }
   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   cN ();
   wZ ( !val );
   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (unsigned char)val );
   }
   else
   {
      MEM ( addr, (unsigned char)val );
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
   UINT addr = MAKE16(GETUNSIGNED8(data,0),GETUNSIGNED8(data,1));
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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( !rV() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   unsigned short gopc = POP ();
   gopc |= (POP()<<8);
   gopc++;
   wPC ( gopc );

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
   UINT addr;
   short result;
   unsigned char val;

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
   wA ( (unsigned char)result );
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
//  |  Absolute,X    |   ROR Oper,X          |    7E   |    3    |    7*    | ???
//  +----------------+-----------------------+---------+---------+----------+
//
//    Note: ROR instruction is available on MCS650X microprocessors after
//          June, 1976.
void C6502::ROR ( void )
{
   UINT addr = 0x0000;
   unsigned short val;

   if ( amode == AM_ACCUMULATOR )
   {
      val = rA ();
   }
   else
   {
      addr = MAKEADDR ( amode, data );
      val = MEM ( addr );
   }
   val |= ( rC()*0x100 );
   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );
   if ( amode == AM_ACCUMULATOR )
   {
      wA ( (unsigned char)val );
   }
   else
   {
      MEM ( addr, (unsigned char)val );
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
   unsigned short addr;
   unsigned short val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val <<= 1;
   val |= rC();
   wC ( val&0x100 );
   val &= 0xFF;
   MEM ( addr, (unsigned char)val );
   wA ( rA()&val );
   wN ( val&0x80 );
   wZ ( !val );

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
   UINT addr;
   unsigned short val;
   short result;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );

   val |= ( rC()*0x100 );
   wC ( val&0x01 );
   val >>= 1;
   val &= 0xFF;
   MEM ( addr, (unsigned char)val );
   
   result = rA () + val + rC ();

	wV ( !((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (unsigned char)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( result&0x100 );

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( rV() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   unsigned short addr;

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
   unsigned short addr;

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
   unsigned short addr;

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( !rC() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   UINT addr;

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
   unsigned char val;

   UINT addr;
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
   UINT addr;

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
   UINT addr;

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( rC() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   unsigned short addr;
   unsigned char  val;

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
   UINT addr;
   unsigned char val;

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
   unsigned short addr;
   unsigned char  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
   val -= 1;
   MEM ( addr, val );
   wC ( rA()>=val );
   val = rA() - val;
   val &= 0xFF;
   wN ( val&0x80 );
   wZ ( !val );

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
   unsigned short addr;
   unsigned char  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( !rZ() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   unsigned short addr;
   unsigned char  val;

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
   UINT addr;
   short result;
   unsigned char val;

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
   wA ( (unsigned char)result );
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
   unsigned short addr;
   unsigned char  val;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
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
   unsigned short addr;
   unsigned char  val;
   short result;

   addr = MAKEADDR ( amode, data );
   val = MEM ( addr );
   val++;
   MEM ( addr, val );

   result = (rA() - val - (1-rC()));

   wV ( ((rA()^val)&0x80) && ((rA()^result)&0x80) );
   wA ( (unsigned char)result );
   wN ( rA()&0x80 );
   wZ ( !rA() );
   wC ( !(result&0x100) );

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
   UINT target;

   target = rPC()+GETSIGNED8(data,0);
   if ( rZ() )
   {
      if ( (rPC()&0xFF00) != (target&0xFF00) )
      {
         (*(data+2))++; // extra cycles stored here...
      }
      wPC ( target );
      (*(data+2))++; // extra cycles stored here...
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
   PUSH ( GETHI8(rPC()) );
   PUSH ( GETLO8(rPC()) );
   sB ();
   PUSH ( rF() );
   wPC ( MAKE16(MEM(VECTOR_IRQ),MEM(VECTOR_IRQ+1)) );
   sI ();

   return;
}

void C6502::IRQ ( char source )
{
   if ( (!m_killed) && (!rI()) )
   {
      m_tracer.AddIRQ ( source );

      PUSH ( GETHI8(rPC()) );
      PUSH ( GETLO8(rPC()) );
      cB ();
      PUSH ( rF()|FLAG_MISC );
      wPC ( MAKE16(MEM(VECTOR_IRQ),MEM(VECTOR_IRQ+1)) );
      sI ();
   }
}

void C6502::NMI ( char source )
{
   if ( !m_killed )
   {
      m_tracer.AddNMI ( source );

      PUSH ( GETHI8(rPC()) );
      PUSH ( GETLO8(rPC()) );
      cB ();
      PUSH ( rF()|FLAG_MISC );
      wPC ( MAKE16(MEM(VECTOR_NMI),MEM(VECTOR_NMI+1)) );
   }
}

void C6502::RESET ( void )
{
   m_killed = false;

   CAPU::RESET ();

   m_tracer.AddRESET ();

   m_cycles = 0;
   m_curCycles = 0;

   m_ea = 0xFFFFFFFF;

   m_pcGoto = 0xFFFFFFFF;

   sI ();
   wF ( rF()|FLAG_MISC );
   wPC ( MAKE16(MEM(VECTOR_RESET),MEM(VECTOR_RESET+1)) );
}

void C6502::CHECKBRKPT ( void )
{
   unsigned char data;
   bool set = false;
   if ( (m_brkptType >= BRKPT_CPU_A) && (m_brkptType <= BRKPT_CPU_SP) )
   {
      switch ( m_brkptType )
      {
         case BRKPT_CPU_A:
            data = m_a;
            set = true;
         break;
         case BRKPT_CPU_X:
            data = m_x;
            set = true;
         break;
         case BRKPT_CPU_Y:
            data = m_y;
            set = true;
         break;
         case BRKPT_CPU_F:
            data = m_f;
            set = true;
         break;
         case BRKPT_CPU_SP:
            data = m_sp;
            set = true;
         break;
      }
      if ( set )
      {
         if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
              ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
              ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
              ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
              ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
              ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
              ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
         {
            m_brkptHit = true;
         }
      }
   }
}

unsigned char C6502::LOAD ( UINT addr, bool checkBrkpt, char* pTarget )
{
   unsigned char data = 0xff;

   if ( addr >= 0x8000 )
   {
      (*pTarget) = eTarget_ROM;
      data = mapperfunc [ CROM::MAPPER() ].highread ( addr );
      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_CPU_ACCESS:
            case BRKPT_CPU_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
            case BRKPT_CPU_EXEC:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  m_brkptHit = true;
               }
            break;
         }
      }
   }
   else if ( addr >= 0x6000 )
   {
      (*pTarget) = eTarget_SRAM;
      data = CROM::SRAM ( addr ); 
      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_CPU_ACCESS:
            case BRKPT_CPU_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
            case BRKPT_CPU_EXEC:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  m_brkptHit = true;
               }
            break;
         }
      }
   }
   else if ( addr >= 0x5000 )
   {
      (*pTarget) = eTarget_Mapper;
      data = mapperfunc [ CROM::MAPPER() ].lowread ( addr );
      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_CPU_ACCESS:
            case BRKPT_CPU_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
            case BRKPT_CPU_EXEC:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  m_brkptHit = true;
               }
            break;
         }
      }
   }
   else if ( addr >= 0x4000 )
   {
      (*pTarget) = eTarget_APURegister;
      if ( addr == 0x4014 || addr == 0x4016 || addr == 0x4017 )
      {
         (*pTarget) = eTarget_IORegister;
      }
      data = CIO::IO ( addr );
      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_IO_ACCESS:
            case BRKPT_IO_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
         }
      }
   }
   else if ( addr >= 0x2000 )
   {
      (*pTarget) = eTarget_PPURegister;
      data = CPPU::PPU ( addr );
      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_CPU_ACCESS:
            case BRKPT_CPU_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;

            case BRKPT_PPU_ACCESS:
            case BRKPT_PPU_READ:
               if ( (CPPU::_PPUADDR() >= m_brkptAddr) && (CPPU::_PPUADDR() <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
         }
      }
   }
   else
   {
      (*pTarget) = eTarget_RAM;
      addr &= 0x7FF; // RAM mirrored...
      data = m_6502memory[addr]; 

      if ( checkBrkpt )
      {
         switch ( m_brkptType )
         {
            case BRKPT_CPU_ACCESS:
            case BRKPT_CPU_READ:
               if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
               {
                  if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                       ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                       ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
                  {
                     m_brkptHit = true;
                  }
               }
            break;
         }
      }
   }

   return data;
}

void C6502::STORE ( UINT addr, unsigned char data, bool checkBrkpt, char* pTarget )
{
   if ( addr < 0x2000 )
   {
      (*pTarget) = eTarget_RAM;
      addr &= 0x7FF; // RAM mirrored...
      m_6502memory[addr] = data&0xFF;
   }
   else if ( addr < 0x4000 )
   {
      (*pTarget) = eTarget_PPURegister;
      CPPU::PPU ( addr, data );
   }
   else if ( addr < 0x5000 )
   {
      (*pTarget) = eTarget_APURegister;
      if ( addr == 0x4014 || addr == 0x4016 )
      {
         (*pTarget) = eTarget_IORegister;
      }
      CIO::IO ( addr, data );
   }
   else if ( addr < 0x6000 )
   {
      (*pTarget) = eTarget_Mapper;
      mapperfunc [ CROM::MAPPER() ].lowwrite ( addr, data );
   }
   else if ( addr < 0x8000 )
   {
      (*pTarget) = eTarget_SRAM;
      CROM::SRAM ( addr, data ); 
   }
   else
   {
      (*pTarget) = eTarget_Mapper;
      mapperfunc [ CROM::MAPPER() ].highwrite ( addr, data );
   }

   if ( checkBrkpt )
   {
      switch ( m_brkptType )
      {
         case BRKPT_CPU_ACCESS:
         case BRKPT_CPU_WRITE:
            if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
            {
               if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                    ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
               {
                  m_brkptHit = true;
               }
            }
         break;

         case BRKPT_IO_ACCESS:
         case BRKPT_IO_WRITE:
            if ( (addr >= m_brkptAddr) && (addr <= m_brkptAddr2) )
            {
               if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                    ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
               {
                  m_brkptHit = true;
               }
            }
         break;

         case BRKPT_PPU_ACCESS:
         case BRKPT_PPU_WRITE:
            if ( (CPPU::_PPUADDR() >= m_brkptAddr) && (CPPU::_PPUADDR() <= m_brkptAddr2) )
            {
               if ( (m_brkptIf == -1) || (m_brkptIf == BRKPT_NA) ||
                    ((m_brkptIf == BRKPT_EQ) && (data == m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_NE) && (data != m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_LT) && (data < m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_GT) && (data > m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BS) && (data&m_brkptCond)) ||
                    ((m_brkptIf == BRKPT_BC) && (!(data&m_brkptCond))) )
               {
                  m_brkptHit = true;
               }
            }
         break;
      }
   }
}

unsigned char C6502::FETCH ( UINT addr )
{
   char target;
   unsigned char data = LOAD ( addr, true, &target );

   m_tracer.AddSample ( m_cycles, eTracer_InstructionFetch, eSource_CPU, target, addr, data );
   if ( addr >= MEM_32KB )
   {
      CCodeDataLogger* pLogger = CROM::LOGGER ( addr );
      if ( pLogger )
      {
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_InstructionFetch, eLoggerSource_CPU );
      }
   }

   return data;
}

unsigned char C6502::DMA ( UINT addr, char source )
{
   char target;
   unsigned char data = LOAD ( addr, false, &target );
   
   m_tracer.AddSample ( m_cycles, eTracer_DMA, eSource_CPU, target, addr, data );
   if ( addr >= MEM_32KB )
   {
      CCodeDataLogger* pLogger = CROM::LOGGER ( addr );
      if ( pLogger )
      {
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DMA, source );
      }
   }
   else if ( addr < MEM_2KB )
   {
      m_logger.LogAccess ( m_cycles, addr, data, eLogger_DMA, source );
   }

   return data;
}

unsigned char C6502::MEM ( UINT addr )
{
   char target;
   unsigned char data = LOAD ( addr, true, &target );

   m_tracer.AddSample ( m_cycles, eTracer_DataRead, eSource_CPU, target, addr, data );

   if ( addr >= MEM_32KB )
   {
      CCodeDataLogger* pLogger = CROM::LOGGER ( addr );
      if ( pLogger )
      {
         pLogger->LogAccess ( m_cycles, addr, data, eLogger_DataRead, eLoggerSource_CPU );
      }
   }
   else if ( addr < MEM_2KB )
   {
      m_logger.LogAccess ( m_cycles, addr, data, eLogger_DataRead, eLoggerSource_CPU );
   }

   return data;
}

unsigned char C6502::_DMA ( UINT addr )
{
   char target;

   return LOAD ( addr, false, &target );
}

unsigned char C6502::_MEM ( UINT addr )
{
   char target;

   return LOAD ( addr, false, &target );
}

void C6502::MEM ( UINT addr, unsigned char data )
{
   TracerInfo* pSample;
   char target;

   if ( addr > 0xFFFF )
   {
      return;
   }

   // store unknown target because otherwise the trace will be out of order...
   pSample = m_tracer.AddSample ( m_cycles, eTracer_DataWrite, eSource_CPU, 0, addr, data );

   // Code/Data Logger accesses of internal CPU RAM...
   if ( addr < MEM_2KB )
   {
      m_logger.LogAccess ( m_cycles, addr, data, eLogger_DataWrite, eLoggerSource_CPU );
   }

   STORE ( addr, data, true, &target );

   // store real target...
   if ( pSample )
   {
      pSample->target = target;
   }
}

void C6502::_MEM ( UINT addr, unsigned char data )
{
   char target;

   STORE ( addr, data, false, &target );
}

UINT C6502::MAKEADDR ( int amode, unsigned char* data )
{
   unsigned short addr = 0x00, addrpre;

   if ( amode == AM_ZEROPAGE )
   {
      addr = (*data);
   }
   else if ( amode == AM_ZEROPAGE_INDEXED_X )
   {
      addr = ((*data)+rX())&0xFF;
   }
   else if ( amode == AM_ZEROPAGE_INDEXED_Y )
   {
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
      if ( (addrpre>>8) != (addr>>8) )
      {
         (*(data+2)) = 1; // extra cycles stored here...
      }
   }
   else if ( amode == AM_ABSOLUTE_INDEXED_Y )
   {
      addrpre = MAKE16((*data),(*(data+1)));
      addr = addrpre+rY();
      if ( (addrpre>>8) != (addr>>8) )
      {
         (*(data+2)) = 1; // extra cycles stored here...
      }
   }
   else if ( amode == AM_PREINDEXED_INDIRECT )
   {
      addr = MAKE16(MEM(((*data)+rX())&0xFF),MEM(((*data)+rX()+1)&0xFF));
   }
   else if ( amode == AM_POSTINDEXED_INDIRECT )
   {
      addrpre = MAKE16(MEM((*data)),MEM(((*data)+1)&0xFF));
      addr = addrpre+rY();
      if ( (addrpre>>8) != (addr>>8) )
      {
         (*(data+2)) = 1; // extra cycles stored here...
      }
   }
   m_ea = addr;
   return addr;
}

// CPTODO: support funcs for inline disassembly and for tracer disassembly
#if 0
void C6502::Disassemble ( QString& dis8000, QString& disC000, qint8* szBinary, int len, bool decorate )
{
   C6502_opcode* pOp;
   int opSize;
   char* ptr8000;
   char* ptrC000;
   QByteArray ba8000;
   QByteArray baC000;
   unsigned char op;

   dis8000.reserve ( 524288 );
   disC000.reserve ( 524288 );
   ba8000 = dis8000.toUtf8 ();
   baC000 = dis8000.toUtf8 ();
   ptr8000 = ba8000.data ();
   ptrC000 = baC000.data ();

   for ( int i = 0; i < len; )
   {
      if ( decorate == true )
      {
         sprintf04xc ( &ptr8000, 0x8000+i );
         sprintf04xc ( &ptrC000, 0xC000+i );
      }

      op = *(szBinary+i);
      pOp = m_6502opcode+op;
      opSize = *(opcode_size+pOp->amode);

      if ( (pOp->documented) && ((len-i) >= opSize) )
      {
         if ( decorate == true )
         {
            ptr8000 += sprintf ( ptr8000, dispFmt[opSize],
                                 szBinary[i],
                                 szBinary[i+1],
                                 szBinary[i+2] );
            ptrC000 += sprintf ( ptrC000, dispFmt[opSize],
                                 szBinary[i],
                                 szBinary[i+1],
                                 szBinary[i+2] );
         }

         ptr8000 += sprintf ( ptr8000, pOp->name );
         ptrC000 += sprintf ( ptrC000, pOp->name );

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
               ptr8000 += sprintf ( ptr8000, operandFmt[pOp->amode],
                                    szBinary[i+1] );
               ptrC000 += sprintf ( ptrC000, operandFmt[pOp->amode],
                                    szBinary[i+1] );
            break;

            // Two byte operands
            case AM_ABSOLUTE:
            case AM_ABSOLUTE_INDEXED_X:
            case AM_ABSOLUTE_INDEXED_Y:
            case AM_INDIRECT:
               ptr8000 += sprintf ( ptr8000, operandFmt[pOp->amode],
                                    szBinary[i+2],
                                    szBinary[i+1] );
               ptrC000 += sprintf ( ptrC000, operandFmt[pOp->amode],
                                    szBinary[i+2],
                                    szBinary[i+1] );
            break;
         }

         if ( decorate == false )
         {
            ptr8000 += sprintf ( ptr8000, "\t\t; $%04X", i );
            ptrC000 += sprintf ( ptrC000, "\t\t; $%04X", i );
         }

         i += opSize;
      }
      else
      {
         if ( decorate == true )
         {
            ptr8000 += sprintf ( ptr8000, dispFmt[1], szBinary[i] );
            ptrC000 += sprintf ( ptrC000, dispFmt[1], szBinary[i] );
         }

         ptr8000 += sprintf ( ptr8000, ".DB $%02X", szBinary[i] );
         ptrC000 += sprintf ( ptrC000, ".DB $%02X", szBinary[i] );

         i++;
      }

      (*ptr8000) = '\r'; ptr8000++;
      (*ptr8000) = '\n'; ptr8000++;;
      (*ptrC000) = '\r'; ptrC000++;
      (*ptrC000) = '\n'; ptrC000++;;
   }

   (*ptr8000) = 0;
   (*ptrC000) = 0;
}

char* C6502::Disassemble ( unsigned char* pOpcode, char* buffer )
{
   char* lbuffer = buffer;
   C6502_opcode* pOp = m_6502opcode+(*pOpcode);
   int opSize = *(opcode_size+pOp->amode);

   if ( (pOp->documented) || (CONFIG.IsIllegalsEnabled()) )
   {
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
   }
   else
   {
      buffer += sprintf ( buffer, ".DB $%02X", *pOpcode );
   }
   (*buffer) = 0;

   return lbuffer;
}
#endif
