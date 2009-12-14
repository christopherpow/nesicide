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
    
// PPU.cpp: implementation of the CPPU class.
//
//////////////////////////////////////////////////////////////////////

#include "BasePalette.h"

#include "PPU.h"
#include "6502.h"
#include "ROM.h"
#include "APU.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CPPU::m_PPUmemory [] = { 0, };
unsigned char  CPPU::m_PALETTEmemory [] = { 0, };
unsigned char* CPPU::m_pPPUmemory [] = { m_PPUmemory+(0<<UPSHIFT_1KB),
                                         m_PPUmemory+(1<<UPSHIFT_1KB),
                                         m_PPUmemory+(0<<UPSHIFT_1KB),
                                         m_PPUmemory+(1<<UPSHIFT_1KB),
                                         m_PPUmemory+(0<<UPSHIFT_1KB),
                                         m_PPUmemory+(1<<UPSHIFT_1KB),
                                         m_PPUmemory+(0<<UPSHIFT_1KB),
                                         m_PPUmemory+(1<<UPSHIFT_1KB) };
unsigned char  CPPU::m_oamAddr = 0x00;
int            CPPU::m_ppuRegByte = 0;
unsigned short CPPU::m_ppuAddr = 0x0000;
unsigned char  CPPU::m_ppuAddrIncrement = 1;
unsigned short CPPU::m_ppuAddrLatch = 0x0000;
unsigned char  CPPU::m_ppuReadLatch = 0x00;
unsigned char  CPPU::m_ppuReadLatch2007 = 0x00;
unsigned char  CPPU::m_PPUreg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  CPPU::m_PPUoam [] = { 0x00, };
unsigned char  CPPU::m_ppuScrollX = 0x00;
bool           CPPU::m_mirrorVert = false;
bool           CPPU::m_mirrorHoriz = false;
int            CPPU::m_oneScreen = -1;
bool           CPPU::m_extraVRAM = false;
UINT           CPPU::m_brkptAddr = 0x0000;
UINT           CPPU::m_brkptAddr2 = 0x0000;
int            CPPU::m_brkptType = -1;
UINT           CPPU::m_brkptCond = 0x0000;
int            CPPU::m_brkptIf = -1;
CTracer         CPPU::m_tracer;
CCodeDataLogger CPPU::m_logger;
unsigned int   CPPU::m_cycles = 0;
int            CPPU::m_mode = MODE_NTSC;
char           CPPU::m_szBinaryText [] = { 0, };

char*          CPPU::m_pTV = NULL;

unsigned char  CPPU::m_frame = 0;
int            CPPU::m_curCycles = 0;
SpriteBuffer     CPPU::m_spriteBuffer;
BackgroundBuffer CPPU::m_bkgndBuffer;

unsigned char  CPPU::m_last2005x = 0;
unsigned char  CPPU::m_last2005y = 0;
unsigned short CPPU::m_2005x [256][240] = { {0,}, };
unsigned short CPPU::m_2005y [256][240] = { {0,}, };
unsigned char  CPPU::m_lastSprite0HitX = 0xFF;
unsigned char  CPPU::m_lastSprite0HitY = 0xFF;

UINT CPPU::m_iPPUViewerScanline = 0;
UINT CPPU::m_iOAMViewerScanline = 0;

CPPU::CPPU()
{
}

CPPU::~CPPU()
{
}

// CPTODO: support function for displaying PPU memory as bintext
#if 0
char* CPPU::MakePrintableBinaryText ( void )
{
   int idx1;
   char* ptr = m_szBinaryText;
   unsigned char data;

   for ( idx1 = 0; idx1 < MEM_16KB; idx1++ )
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
      data = CPPU::LOAD(idx1,false,false);
      sprintf02x ( &ptr, data );
   }
   (*ptr) = 0;

   return m_szBinaryText;
}
#endif

UINT CPPU::LOAD ( UINT addr, bool bTrace, bool checkBrkpt, char source, char type )
{
   unsigned char data = 0xFF;

   addr &= 0x3FFF;

   if ( addr < 0x2000 ) 
   {
      data = CROM::CHRMEM ( addr );

      if ( bTrace )
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }
      return data;
   }

   if ( addr >= 0x3000 )
   {
      if ( addr >= 0x3F00 )
      {  
         if ( !(addr&0x3) )
         {
            addr = 0x3F00;
         }

         data = *(m_PALETTEmemory+(addr&0x1F));

         if ( bTrace )
         {
            m_tracer.AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
         }
         return data;
      }

      addr &= 0xEFFF;
   }

   data = *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));

   if ( bTrace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }
   return data;      
}

void CPPU::STORE ( UINT addr, unsigned char data, bool bTrace, bool checkBrkpt, char source, char type )
{ 
   addr &= 0x3FFF;

   if ( addr < 0x2000 ) 
   {
      if ( bTrace )
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }

      if ( CROM::IsWriteProtected() == false )
      {
         CROM::CHRMEM ( addr, data );
      }

      return;
   }

   if ( addr >= 0x3000 )
   {
      if ( addr >= 0x3F00 )
      {  
         if ( bTrace )
         {
            m_tracer.AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
         }

         if ( !(addr&0xF) )
         {
            *(m_PALETTEmemory+0x00) = data;
            *(m_PALETTEmemory+0x10) = data;
         }
         else
         {
            *(m_PALETTEmemory+(addr&0x1F)) = data;
         }

         return;
      }

      addr &= 0xEFFF;
   }
   
   if ( bTrace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         m_tracer.AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)) = data;     
}

UINT CPPU::RENDER ( UINT addr, char target )
{
   UINT data;

   data = LOAD ( addr, true, false, eSource_PPU, target );

   m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, addr, data, eLogger_DataRead, eLoggerSource_PPU );

   m_curCycles += 2; // Address/Data bus multiplexed thus 2 cycles required per access...
   m_cycles += 2; // Address/Data bus multiplexed thus 2 cycles required per access...

   return data;
}

void CPPU::GARBAGE ( char target )
{
   m_tracer.AddGarbageFetch ( m_cycles, target );

   m_curCycles += 2; // Address/Data bus multiplexed thus 2 cycles required per access...
   m_cycles += 2; // Address/Data bus multiplexed thus 2 cycles required per access...
}

void CPPU::EXTRA ()
{
   m_tracer.AddGarbageFetch ( m_cycles, eTarget_Unknown );

   m_curCycles++; // Idle cycle...
   m_cycles++; // Idle cycle...
}

void CPPU::RESET ( void )
{
   int idx;

   m_PPUreg [ 0 ] = 0x00;
   m_PPUreg [ 1 ] = 0x00;
   m_PPUreg [ 2 ] = 0x00;

   for ( idx = 0; idx < 4; idx++ )
   {
      m_pPPUmemory [ idx ] = m_PPUmemory+((idx&1)<<UPSHIFT_1KB);
   }

   m_cycles = 0;
   m_curCycles = 0;
   m_frame = 0;

   m_tracer.AddRESET ();

   m_ppuAddr = 0x0000;
   m_ppuAddrLatch = 0x0000;
   m_ppuAddrIncrement = 1;
   m_ppuReadLatch = 0x00;
   m_ppuReadLatch2007 = 0x00;
   m_ppuRegByte = 0;

   for ( idx = 0; idx < 240; idx++ )
   {
      RENDERRESET ( idx );
   }
}

UINT CPPU::PPU ( UINT addr )
{
   unsigned char data = 0xFF;
   unsigned short fixAddr;
   unsigned short oldPpuAddr;
   bool           read = false;

   if ( addr < 0x4000 )
   {
      fixAddr = addr&0x0007;
      if ( addr == PPUSTATUS )
      {
         data = *(m_PPUreg+fixAddr);
         *(m_PPUreg+fixAddr) &= (~PPUSTATUS_VBLANK); // VBLANK clear-on-read
         m_ppuRegByte = 0; // Clear PPUADDR address latch
         data = (data&0xE0)|(m_ppuReadLatch&0x1F);
      }
      else if ( addr == OAMDATA )
      {
         data = *(m_PPUoam+m_oamAddr);
      }
      else if ( addr == PPUDATA )
      {
         m_ppuReadLatch = m_ppuReadLatch2007;
         oldPpuAddr = m_ppuAddr;

         m_ppuAddr += m_ppuAddrIncrement;

         if ( oldPpuAddr < 0x3F00 )
         {
            m_ppuReadLatch2007 = LOAD ( oldPpuAddr, true, true, eSource_CPU, eTracer_DataRead );

            data = m_ppuReadLatch;
         }
         else
         {
            data = LOAD ( oldPpuAddr, true, true, eSource_CPU, eTracer_DataRead );
         }

         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataRead, eLoggerSource_CPU );

         // Toggling A12 causes IRQ count in some mappers...
         if ( (!(oldPpuAddr&0x1000)) && ((oldPpuAddr^m_ppuAddr)&0x1000) )
         {
            if ( mapperfunc[CROM::MAPPER()].synch(-1) )
            {
               C6502::IRQ( eSource_Mapper );
               CPPU::IRQ ( eSource_Mapper ); // Just for Tracer tag
            }
         }
      }
      else
      {
         data = m_ppuReadLatch;
      }
   }
   else if ( addr < 0x5000 )
   {
      if ( addr == IOSPRITEDMA )
      {
         data = *(m_PPUoam+m_oamAddr);
      }
   }

   return data;
}

void CPPU::PPU ( UINT addr, unsigned char data )
{
   unsigned short fixAddr;
   unsigned short oldPpuAddr;
   int dma;

   m_ppuReadLatch = data;

   if ( addr < 0x4000 )
   {
      fixAddr = addr&0x0007;
      if ( addr != PPUSTATUS )
      {
         *(m_PPUreg+fixAddr) = data;
      }

      if ( addr == PPUCTRL )
      {
         m_ppuAddrLatch &= 0x73FF;
         m_ppuAddrLatch |= ((((unsigned short)data&0x03))<<10);
         m_ppuAddrIncrement = (((!!(data&PPUCTRL_VRAM_ADDR_INC))*31)+1);
      }
      else if ( addr == OAMADDR )
      {
         m_oamAddr = data;
      }
      else if ( addr == OAMDATA )
      {
         *(m_PPUoam+m_oamAddr) = data;
         m_oamAddr++;
      }
      else if ( addr == PPUSCROLL )
      {
         if ( m_ppuRegByte )
         {
            m_last2005y = data;
            m_ppuAddrLatch &= 0x8C1F;
            m_ppuAddrLatch |= ((((unsigned short)data&0xF8))<<2);
            m_ppuAddrLatch |= ((((unsigned short)data&0x07))<<12);
         }
         else
         {
            m_last2005x = data;
            m_ppuScrollX = data&0x07;
            m_ppuAddrLatch &= 0xFFE0;
            m_ppuAddrLatch |= ((((unsigned short)data&0xF8))>>3);
         }
         m_ppuRegByte = !m_ppuRegByte;
      }
      else if ( addr == PPUADDR )
      {
         oldPpuAddr = m_ppuAddr;
         if ( m_ppuRegByte )
         {
            m_ppuAddrLatch &= 0x7F00;
            m_ppuAddrLatch |= data;

            m_ppuAddr = m_ppuAddrLatch;
         }
         else
         {
            m_ppuAddrLatch &= 0x00FF;
            m_ppuAddrLatch |= ((((unsigned short)data&0x3F))<<8);
         }
         m_ppuRegByte = !m_ppuRegByte;

         // Toggling A12 causes IRQ count in some mappers...
         if ( (!(oldPpuAddr&0x1000)) && ((oldPpuAddr^m_ppuAddr)&0x1000) )
         {
            if ( mapperfunc[CROM::MAPPER()].synch(-1) )
            {
               C6502::IRQ( eSource_Mapper );
               CPPU::IRQ ( eSource_Mapper ); // Just for Tracer tag
            }
         }
      }
      else if ( addr == PPUDATA )
      {
         STORE ( m_ppuAddr, data, true, true, eSource_CPU, eTracer_DataWrite );

         oldPpuAddr = m_ppuAddr;

         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataWrite, eLoggerSource_CPU );

         // Increment PPUADDR
         m_ppuAddr += m_ppuAddrIncrement;
         
         // Toggling A12 causes IRQ count in some mappers...
         if ( (!(oldPpuAddr&0x1000)) && ((oldPpuAddr^m_ppuAddr)&0x1000) )
         {
            if ( mapperfunc[CROM::MAPPER()].synch(-1) )
            {
               C6502::IRQ( eSource_Mapper );
               CPPU::IRQ ( eSource_Mapper ); // Just for Tracer tag
            }
         }
      }
   }
   else if ( addr < 0x5000 )
   {
      if ( addr == IOSPRITEDMA )
      {
         // DMA
         int start = m_oamAddr; 
         for ( dma = 0; dma < NUM_OAM_REGS; dma++ )
         {
            *(m_PPUoam+((dma+start)&0xFF)) = C6502::DMA ( (data<<8)|((dma+start)&0xFF), eLoggerSource_PPU );
         }
      }
   }
}

void CPPU::FRAMESTART ( void )
{
   m_ppuAddr = m_ppuAddrLatch;
   m_lastSprite0HitX = 0xFF;
   m_lastSprite0HitY = 0xFF;
}

void CPPU::SCANLINESTART ( void )
{
   m_ppuAddr &= 0xFBE0; 
   m_ppuAddr |= m_ppuAddrLatch&0x41F;
}

void CPPU::MIRROR ( int oneScreen, bool vert, bool extraVRAM )
{ 
   m_mirrorVert = vert; 
   m_mirrorHoriz = !vert; 
   m_oneScreen = oneScreen;
   m_extraVRAM = extraVRAM;

   if ( m_oneScreen >= 0 )
   {
      MIRROR ( m_oneScreen, m_oneScreen, m_oneScreen, m_oneScreen );
   }
   else if ( m_extraVRAM )
   {
      MIRROR ( 0, 1, 2, 3 );
   }
   else if ( m_mirrorVert )
   {
      MIRRORVERT ();
   }
   else if ( m_mirrorHoriz )
   {
      MIRRORHORIZ ();
   }
}

void CPPU::MIRRORVERT ( void )
{
   m_mirrorVert = true; 
   m_mirrorHoriz = false; 
   MIRROR ( 0, 1, 0, 1 );
}

void CPPU::MIRRORHORIZ ( void )
{
   m_mirrorVert = false; 
   m_mirrorHoriz = true; 
   MIRROR ( 0, 0, 1, 1 );
}

void CPPU::MIRROR ( int nt1, int nt2, int nt3, int nt4 )
{
   nt1 &= 0x3;
   nt2 &= 0x3;
   nt3 &= 0x3;
   nt4 &= 0x3;
   m_mirrorVert = false; 
   m_mirrorHoriz = false; 
   Move1KBank ( 0x8, &(m_PPUmemory[(nt1<<UPSHIFT_1KB)]) );
   Move1KBank ( 0x9, &(m_PPUmemory[(nt2<<UPSHIFT_1KB)]) );
   Move1KBank ( 0xA, &(m_PPUmemory[(nt3<<UPSHIFT_1KB)]) );
   Move1KBank ( 0xB, &(m_PPUmemory[(nt4<<UPSHIFT_1KB)]) );
}

void CPPU::SCANLINEEND ( void )
{
   if ( (m_ppuAddr&0x7000) == 0x7000 )
   {
      m_ppuAddr &= 0x8FFF;
      if ( (m_ppuAddr&0x03E0) == 0x03A0 )
      {
         m_ppuAddr ^= 0x0800;
         m_ppuAddr &= 0xFC1F;
      }
      else
      {
         if ( (m_ppuAddr&0x03E0) == 0x03E0 )
         {
            m_ppuAddr &= 0xFC1F;
         }
         else
         {
            m_ppuAddr += 0x0020;
         }
      }
   }
   else
   {
      m_ppuAddr += 0x1000;
   }
}

void CPPU::RENDERRESET ( int scanline )
{
   int idxx;
   int idxy = scanline<<8;
   COLORREF base = CBasePalette::GetDisplayPalette(rPALETTE(0));

   if ( m_pTV )
   {
       for ( idxx = 0; idxx < 256; idxx++ )
       {
          *((COLORREF*)(m_pTV+(idxy*3)+(idxx*3))) = base;
       }
   }
}

bool CPPU::NONRENDERSCANLINE ( int scanlines )
{
   int idxx, idxy;
   bool brkpt = false;

   for ( idxy = 0; idxy < scanlines; idxy++ )
   {
      for ( idxx = 0; idxx < 341; idxx++ )
      {
         m_cycles += 1;
         m_curCycles += 1;
         brkpt = C6502::EMULATE ( true, m_curCycles/3 );
         if ( brkpt ) goto done;
         m_curCycles %= 3;
      }
   }

   // CPTODO: this is not an appropriate way to break out of emulation on a
   // breakpoint...when we continue we'll act as if the previous frame was complete
   // which is usually NOT the case.  need to figure out a way to pick up RIGHT
   // where we left off...at the exact PPU/CPU cycle...
done:
   return brkpt;
}

void CPPU::PIXELPIPELINES ( int x, int off, unsigned char *a, unsigned char *b1, unsigned char *b2 )
{
   BackgroundBufferData* pBkgnd1 = m_bkgndBuffer.data;
   BackgroundBufferData* pBkgnd2 = m_bkgndBuffer.data+1;
   (*b1) = (pBkgnd1->patternData1>>(7-x))&0x1;
   (*b2) = (pBkgnd1->patternData2>>(7-x))&0x1;
   (*a) = m_bkgndBuffer.data[(x+off)>>3].attribData;
   pBkgnd1->patternData1 <<= 1;
   pBkgnd1->patternData2 <<= 1;
   pBkgnd1->patternData1 |= (!!(pBkgnd2->patternData1&0x80));
   pBkgnd1->patternData2 |= (!!(pBkgnd2->patternData2&0x80));
   pBkgnd2->patternData1 <<= 1;
   pBkgnd2->patternData2 <<= 1;
}

bool CPPU::RENDERSCANLINE ( int scanline )
{
   int rasttv = scanline<<8;
   int idxx;
   int sprite;
   int p = 0;
   bool sprite0HitSet = false;
   SpriteBufferData* pSprite;
   BackgroundBufferData* pBkgnd = m_bkgndBuffer.data;
   int idx2;
   bool brkpt = false;
   char* pTV = (char*)(m_pTV+(rasttv*3));

   // even-odd framing for extra-cycle on pre-render scanline...
   m_frame = !m_frame; 

   for ( idxx = 0; idxx < 256; idxx += 8 )
   {
      // Check for pre-render scanline...
      if ( scanline >= 0 )
      {
         int colorIdx;
         int startBkgnd = (!(rPPU(PPUMASK)&PPUMASK_BKGND_CLIPPING))<<3;
         int startSprite = (!(rPPU(PPUMASK)&PPUMASK_SPRITE_CLIPPING))<<3;
         int patternMask;
         for ( patternMask = 0; patternMask < 8; patternMask++ )
         {
            char tvSet = 0x00;
            unsigned char a, b1, b2;
            PIXELPIPELINES ( rSCROLLX(), patternMask, &a, &b1, &b2 );
            colorIdx = (a|b1|(b2<<1));

            // Update variables for PPU viewer
            *(*(m_2005x+(idxx+patternMask))+scanline) = m_last2005x+((rPPU(PPUCTRL)&0x1)<<8);
            *(*(m_2005y+(idxx+patternMask))+scanline) = m_last2005y+(((rPPU(PPUCTRL)&0x2)>>1)*240);

            if ( (idxx>=startBkgnd) && (rPPU(PPUMASK)&PPUMASK_RENDER_BKGND) )
            {
               if ( !(colorIdx&0x3) ) colorIdx = 0;
               if ( colorIdx&0x3 ) tvSet |= 1;

               (*(COLORREF*)pTV) = CBasePalette::GetDisplayPalette(rPALETTE(colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
            }
            else
            {
               if ( (m_ppuAddr&0x3F00) == 0x3F00 )
               {
                  (*(COLORREF*)pTV) = CBasePalette::GetDisplayPalette(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
               else
               {
                  (*(COLORREF*)pTV) = CBasePalette::GetDisplayPalette(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
            }

            for ( sprite = 0; sprite < m_spriteBuffer.count; sprite++ )
            {
               pSprite = m_spriteBuffer.data + (*(m_spriteBuffer.order+sprite));
               idx2 = p - pSprite->spriteX; 
               if ( (idx2 >= 0) && (idx2 < PATTERN_SIZE) )
               {
                  if ( pSprite->spriteX+idx2 >= startSprite )
                  {
                     if ( pSprite->spriteFlipHoriz )
                     {
                        colorIdx = ((pSprite->patternData1>>idx2)&0x01)|((((pSprite->patternData2>>idx2)&0x01)<<1) );
                     }
                     else
                     {
                        colorIdx = ((pSprite->patternData1>>(7-idx2))&0x01)|((((pSprite->patternData2>>(7-idx2))&0x01)<<1) );
                     }
                     colorIdx |= (pSprite->attribData<<2);
                     if ( colorIdx&0x3 )
                     {
                        if ( (rPPU(PPUMASK)&PPUMASK_RENDER_SPRITES) && 
                             (((!pSprite->spriteBehind) && (!(tvSet&0x2))) ||
                             ((pSprite->spriteBehind) && (!(tvSet&0x1)))) )
                        {
                           (*(COLORREF*)pTV) = CBasePalette::GetDisplayPalette(rPALETTE(0x10+colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                        }
                        if ( (pSprite->spriteIdx == 0) && 
                             (!sprite0HitSet) && 
                             (pSprite->spriteX+idx2 < 255) &&
                             ((tvSet&0x1) == 0x1) )
                        {
                           if ( (!(rPPU(PPUSTATUS)&PPUSTATUS_SPRITE_0_HIT)) && 
                                ((rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) == (PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) )
                           {
                              wPPU ( PPUSTATUS, rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_0_HIT );
                              sprite0HitSet = true;

                              // Save last sprite 0 hit coords for OAM viewer...
                              m_lastSprite0HitX = p;
                              m_lastSprite0HitY = scanline;

                              // TODO: update tracer info
                              m_tracer.AddSample ( m_cycles, eTracer_Sprite0Hit, 0, 0, 0, 0 );
                           }
                        }
                        tvSet |= 0x2;
                     }
                  }
               }
            }

            pTV += 3;
            p++;
         }
      }

      GATHERBKGND ();
   }

   // Re-latch PPU address...
   if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
   {
      SCANLINEEND ();
   }

   GATHERSPRITES ( scanline );

   if ( scanline == -1 )
   {
      // Re-latch PPU address...
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         FRAMESTART ();
      }
   }

   // Re-latch PPU address...
   if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
   {
      SCANLINESTART ();
   }

   // Fill pipeline for next scanline...
   memcpy ( m_bkgndBuffer.data, m_bkgndBuffer.data+1, sizeof(BackgroundBufferData) );
   GATHERBKGND ();
   memcpy ( m_bkgndBuffer.data, m_bkgndBuffer.data+1, sizeof(BackgroundBufferData) );
   GATHERBKGND ();

   // Finish off scanline render clock cycles...
   GARBAGE ( eTarget_NameTable );
   GARBAGE ( eTarget_NameTable );

   // If this is the non-render scanline on an odd frame and the PPU is on
   if ( (scanline >= 0) ||
        ((m_frame) || (!(rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND)))) )
   {
      // account for extra clock (341)
      EXTRA ();
   }
   brkpt = C6502::EMULATE ( true, m_curCycles/3 );
   m_curCycles %= 3;

   return brkpt;
}

bool CPPU::GATHERBKGND ( void )
{
   UINT ppuAddr = rPPUADDR();
   unsigned short patternIdx;
   int tileX = ppuAddr&0x001F;
   int tileY = (ppuAddr&0x03E0)>>5;
   int nameAddr = 0x2000 + (ppuAddr&0x0FFF);
   int attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
   int bkgndPatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;
   BackgroundBufferData* pBkgnd = m_bkgndBuffer.data + 1;
   bool brkpt = false;

   m_bkgndBuffer.data[0].attribData = m_bkgndBuffer.data[1].attribData;

   patternIdx = bkgndPatBase+(RENDER(nameAddr,eTracer_RenderBkgnd)<<4)+((ppuAddr&0x7000)>>12);
   mapperfunc[CROM::MAPPER()].latch ( patternIdx );
   pBkgnd->attribData = RENDER ( attribAddr,eTracer_RenderBkgnd );
   pBkgnd->patternData1 = RENDER ( patternIdx,eTracer_RenderBkgnd );
   pBkgnd->patternData2 = RENDER ( patternIdx+PATTERN_SIZE,eTracer_RenderBkgnd );

   if ( (tileY&0x0002) == 0 )
   {
      if ( (tileX&0x0002) == 0 )
      {
         pBkgnd->attribData = (pBkgnd->attribData&0x03)<<2;
      }
      else
      {
         pBkgnd->attribData = (pBkgnd->attribData&0x0C);
      }
   }
   else
   {
      if ( (tileX&0x0002) == 0 )
      {
         pBkgnd->attribData = (pBkgnd->attribData&0x30)>>2;
      }
      else
      {
         pBkgnd->attribData = (pBkgnd->attribData&0xC0)>>4;
      }
   }

   brkpt = C6502::EMULATE ( true, m_curCycles/3 );
   m_curCycles %= 3;

   if ( rPPU(PPUMASK)&PPUMASK_RENDER_BKGND )
   {
      m_ppuAddr++;
      if ((m_ppuAddr&0x001F) == 0) 
      {
         m_ppuAddr   ^= 0x0400;
         m_ppuAddr   -= 0x0020;
      }
   }

   return brkpt;
}

bool CPPU::GATHERSPRITES ( int scanline )
{
   int idx1, idx2;
   int sprite;
   unsigned short spritePatBase;
   short         spriteY;
   unsigned char patternIdx;
   unsigned char spriteAttr;
   int           spriteSize;
   SpriteBufferData* pSprite;
   bool brkpt = false;

   m_spriteBuffer.count = 0;
   m_spriteBuffer.order [ 0 ] = 0;
   m_spriteBuffer.order [ 1 ] = 1;
   m_spriteBuffer.order [ 2 ] = 2;
   m_spriteBuffer.order [ 3 ] = 3;
   m_spriteBuffer.order [ 4 ] = 4;
   m_spriteBuffer.order [ 5 ] = 5;
   m_spriteBuffer.order [ 6 ] = 6;
   m_spriteBuffer.order [ 7 ] = 7;
   pSprite = m_spriteBuffer.data;

   spriteSize = (!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE)) + 1;
   if ( spriteSize == 1 )
   {
      spritePatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
   }

   for ( sprite = 0; sprite < 64; sprite++ )
   {
      spriteY = OAM ( SPRITEY, sprite );
      idx1 = scanline-spriteY;
      if ( (spriteY < SPRITE_YMAX) && (idx1 >= 0) && (idx1 < (spriteSize<<3)) )
      {
         if ( m_spriteBuffer.count == 8 )
         {
            if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
            {
               wPPU(PPUSTATUS,rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_OVFLO );
            }
            break;
         }
         
         pSprite->spriteIdx = sprite;

         patternIdx = OAM ( SPRITEPAT, sprite );
         if ( spriteSize == 2 )
         {
            spritePatBase = (patternIdx&0x01)<<12;
            patternIdx &= 0xFE;
         }
         spriteAttr = OAM ( SPRITEATT, sprite );
         pSprite->spriteBehind = !!(spriteAttr&SPRITE_PRIORITY);
         pSprite->spriteFlipVert = !!(spriteAttr&SPRITE_FLIP_VERT);
         pSprite->spriteFlipHoriz = !!(spriteAttr&SPRITE_FLIP_HORIZ);

         pSprite->spriteX = OAM ( SPRITEX, sprite );
         
         pSprite->attribData = spriteAttr&SPRITE_PALETTE_IDX_MSK;

         // For 8x16 sprites...
         if ( (spriteSize == 2) && 
              (((!pSprite->spriteFlipVert) && (idx1 >= PATTERN_SIZE)) ||
              ((pSprite->spriteFlipVert) && (idx1 < PATTERN_SIZE))) )
         {
            patternIdx++;
         }
         if ( pSprite->spriteFlipVert )
         {
            idx1 = (spriteSize<<3)-1-idx1;
         }

         // Garbage nametable fetches according to Samus Aran...
         GARBAGE ( eTarget_NameTable );
         GARBAGE ( eTarget_NameTable );
         pSprite->patternData1 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7), eTracer_RenderSprite );
         mapperfunc[CROM::MAPPER()].latch ( spritePatBase+(patternIdx<<4)+(idx1&0x7) );
         pSprite->patternData2 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7)+PATTERN_SIZE, eTracer_RenderSprite );

         brkpt = C6502::EMULATE ( true, m_curCycles/3 );
         if ( brkpt ) goto done;
         m_curCycles %= 3;

         m_spriteBuffer.count++;
         pSprite++;
      }
   }

   for ( sprite = m_spriteBuffer.count; sprite < 8; sprite++ )
   {
      // Garbage nametable fetches according to Samus Aran...
      GARBAGE ( eTarget_NameTable );
      GARBAGE ( eTarget_NameTable );
      GARBAGE ( eTarget_PatternMemory );
      GARBAGE ( eTarget_PatternMemory );

      brkpt = C6502::EMULATE ( true, m_curCycles/3 );
      if ( brkpt ) goto done;
      m_curCycles %= 3;
   }

   // order sprites by priority for rendering...
   unsigned char temp;
   for ( idx1 = 0; idx1 < m_spriteBuffer.count; idx1++ )
   {
      for ( idx2 = idx1; idx2 < m_spriteBuffer.count-idx1; idx2++ )
      {
         if ( m_spriteBuffer.data[idx1].spriteIdx > m_spriteBuffer.data[idx2].spriteIdx )
         {
            // swap...
            temp = m_spriteBuffer.order [ idx1 ];
            m_spriteBuffer.order [ idx1 ] = m_spriteBuffer.order [ idx2 ];
            m_spriteBuffer.order [ idx2 ] = temp;
         }
      }
   }

   // CPTODO: this is not an appropriate way to break out of emulation on a
   // breakpoint...when we continue we'll act as if the previous frame was complete
   // which is usually NOT the case.  need to figure out a way to pick up RIGHT
   // where we left off...at the exact PPU/CPU cycle...
done:
   return brkpt;
}
