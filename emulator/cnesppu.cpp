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

#include "cnessystempalette.h"

#include "cnesppu.h"
#include "cnes6502.h"
#include "cnesrom.h"
#include "cnesapu.h"

#include <QColor>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define GARBAGE_SPRITE_FETCH 0xFF

// PPU Registers
static CBitfieldData* tblPPUCTRLBitfields [] =
{
   new CBitfieldData("Generate NMI", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("PPU Master/Slave", 6, 1, "%X", 2, "Master", "Slave"),
   new CBitfieldData("Sprite Size", 5, 1, "%X", 2, "8x8", "8x16"),
   new CBitfieldData("Playfield Pattern Table", 4, 1, "%X", 2, "$0000", "$1000"),
   new CBitfieldData("Sprite Pattern Table", 3, 1, "%X", 2, "$0000", "$1000"),
   new CBitfieldData("VRAM Address Increment", 2, 1, "%X", 2, "+1", "+32"),
   new CBitfieldData("NameTable", 0, 2, "%X", 4, "NT1", "NT2", "NT3", "NT4")
};

static CBitfieldData* tblPPUMASKBitfields [] =
{
   new CBitfieldData("Blue Emphasis", 7, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Green Emphasis", 6, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Red Emphasis", 5, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Sprite Rendering", 4, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Playfield Rendering", 3, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Sprite Clipping", 2, 1, "%X", 2, "Yes", "No"),
   new CBitfieldData("Playfield Clipping", 1, 1, "%X", 2, "Yes", "No"),
   new CBitfieldData("Greyscale", 0, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblPPUSTATUSBitfields [] =
{
   new CBitfieldData("Vertical Blank", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite 0 Hit", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite Overflow", 5, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblOAMADDRBitfields [] =
{
   new CBitfieldData("OAM Address", 0, 8, "%02X", 0)
};

static CBitfieldData* tblOAMDATABitfields [] =
{
   new CBitfieldData("OAM Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUSCROLLBitfields [] =
{
   new CBitfieldData("PPU Scroll", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUADDRBitfields [] =
{
   new CBitfieldData("PPU Address", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUDATABitfields [] =
{
   new CBitfieldData("PPU Data", 0, 8, "%02X", 0)
};

static CRegisterData* tblPPURegisters [] =
{
   new CRegisterData(0x2000, "PPUCTRL", 7, tblPPUCTRLBitfields),
   new CRegisterData(0x2001, "PPUMASK", 8, tblPPUMASKBitfields),
   new CRegisterData(0x2002, "PPUSTATUS", 3, tblPPUSTATUSBitfields),
   new CRegisterData(0x2003, "OAMADDR", 1, tblOAMADDRBitfields),
   new CRegisterData(0x2004, "OAMDATA", 1, tblOAMDATABitfields),
   new CRegisterData(0x2005, "PPUSCROLL", 1, tblPPUSCROLLBitfields),
   new CRegisterData(0x2006, "PPUADDR", 1, tblPPUADDRBitfields),
   new CRegisterData(0x2007, "PPUDATA", 1, tblPPUDATABitfields)
};

// PPU Event breakpoints
bool ppuRasterPositionEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (pBreakpoint->item1 == CPPU::_X()) &&
        (pBreakpoint->item2 == CPPU::_Y()) )
   {
      return true;
   }
   return false;
}

bool ppuScanlineStartEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuScanlineEndEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuPrerenderScanlineStartEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuPrerenderScanlineEndEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuSpriteDMAEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuSprite0HitEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuSpriteInMultiplexerEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }
   return false;
}

bool ppuSpriteSelectedEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }
   return false;
}

bool ppuSpriteRenderingEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }
   return false;
}

bool ppuSpriteOverflowEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* tblPPUEvents [] =
{
   new CBreakpointEventInfo("Raster Position", ppuRasterPositionEvent, 2, "Break at pixel (%d,%d)", 10, "X:", "Y:"),
   new CBreakpointEventInfo("Pre-render Scanline Start (X=0,Y=-1)", ppuPrerenderScanlineStartEvent, 0, "Break at start of pre-render scanline", 10),
   new CBreakpointEventInfo("Pre-render Scanline End (X=256,Y=-1)", ppuPrerenderScanlineEndEvent, 0, "Break at end of pre-render scanline", 10),
   new CBreakpointEventInfo("Scanline Start (X=0,Y=[0,239])", ppuScanlineStartEvent, 0, "Break at start of scanline", 10),
   new CBreakpointEventInfo("Scanline End (X=256,Y=[0,239])", ppuScanlineEndEvent, 0, "Break at end of scanline", 10),
   new CBreakpointEventInfo("Sprite DMA", ppuSpriteDMAEvent, 0, "Break on sprite DMA", 10),
   new CBreakpointEventInfo("Sprite 0 Hit", ppuSprite0HitEvent, 0, "Break on sprite 0 hit", 10),
   new CBreakpointEventInfo("Sprite enters multiplexer", ppuSpriteInMultiplexerEvent, 1, "Break if sprite %d enters multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite selected by multiplexer", ppuSpriteSelectedEvent, 1, "Break if sprite %d is selected by multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite rendering", ppuSpriteRenderingEvent, 1, "Break if rendering sprite %d on scanline", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite overflow", ppuSpriteOverflowEvent, 0, "Break on sprite-per-scanline overflow", 10)
};

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
unsigned char  CPPU::m_PPUreg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  CPPU::m_PPUoam [] = { 0x00, };
unsigned char  CPPU::m_ppuScrollX = 0x00;
int            CPPU::m_oneScreen = -1;
bool           CPPU::m_extraVRAM = false;

CCodeDataLogger CPPU::m_logger ( MEM_16KB, MASK_16KB );

char*          CPPU::m_pCodeDataLoggerInspectorTV = NULL;

unsigned int   CPPU::m_cycles = 0;

bool           CPPU::m_vblankChoked = false;

char*          CPPU::m_pTV = NULL;

char*          CPPU::m_pCHRMEMInspectorTV = NULL;
bool           CPPU::m_bCHRMEMInspector = false;
char*          CPPU::m_pOAMInspectorTV = NULL;
bool           CPPU::m_bOAMInspector = false;
char*          CPPU::m_pNameTableInspectorTV = NULL;
bool           CPPU::m_bNameTableInspector = false;

unsigned int   CPPU::m_frame = 0;
int            CPPU::m_curCycles = 0;

SpriteBuffer     CPPU::m_spriteBuffer;
BackgroundBuffer CPPU::m_bkgndBuffer;

unsigned char  CPPU::m_last2005x = 0;
unsigned char  CPPU::m_last2005y = 0;
unsigned short CPPU::m_2005x [256][240] = { {0,}, };
unsigned short CPPU::m_2005y [256][240] = { {0,}, };
unsigned char  CPPU::m_lastSprite0HitX = 0xFF;
unsigned char  CPPU::m_lastSprite0HitY = 0xFF;
unsigned char  CPPU::m_x = 0xFF;
unsigned char  CPPU::m_y = 0xFF;

UINT CPPU::m_iPPUViewerScanline = 0;
UINT CPPU::m_iOAMViewerScanline = 0;

CRegisterData** CPPU::m_tblRegisters = tblPPURegisters;
int             CPPU::m_numRegisters = NUM_PPU_REGISTERS;

CBreakpointEventInfo** CPPU::m_tblBreakpointEvents = tblPPUEvents;
int                    CPPU::m_numBreakpointEvents = NUM_PPU_EVENTS;

CPPU::CPPU()
{
}

CPPU::~CPPU()
{
}

static QColor color [] =
{
   QColor(255,0,0),
   QColor(0,255,0),
   QColor(0,0,255),
   QColor(255,255,0)
};

static QColor renderColor = QColor(0,0,255);

void CPPU::RENDERCODEDATALOGGER ( void )
{
   unsigned int idxx;
   UINT cycleDiff;
   UINT curCycle = CCodeDataLogger::GetCurCycle ();
   QColor lcolor;
   CCodeDataLogger* pLogger;
   LoggerInfo* pLogEntry;

   // Clearly...
   memset ( m_pCodeDataLoggerInspectorTV, 255, 49152 );

   // Show PPU memory...
   pLogger = &m_logger;
   for ( idxx = 0; idxx < 0x4000; idxx++ )
   {
      pLogEntry = pLogger->GetLogEntry(idxx);
      cycleDiff = (curCycle-pLogEntry->cycle)/17800;
      if ( cycleDiff > 199 ) cycleDiff = 199;

      if ( pLogEntry->count )
      {
         // PPU fetches are one color, CPU fetches are others...
         if ( pLogEntry->source == eSource_PPU )
         {
            lcolor = renderColor;
         }
         else
         {
            lcolor = color[pLogEntry->type];
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
         m_pCodeDataLoggerInspectorTV[(idxx * 3) + 0] = lcolor.red();
         m_pCodeDataLoggerInspectorTV[(idxx * 3) + 1] = lcolor.green();
         m_pCodeDataLoggerInspectorTV[(idxx * 3) + 2] = lcolor.blue();
      }
   }
}

void CPPU::INCCYCLE(void)
{
   m_curCycles += CPU_CYCLE_ADJUST;
   m_cycles += 1;
}

void CPPU::EMULATE(void)
{
   // Keep track of remaining cycle count before manipulations...
   int storedCyclesNotExecuted = m_curCycles;

   // Only do 6502 stuff if the cycle-stealing DMA is not happening...
   if ( m_curCycles > 0 )
   {
      if ( CNES::VIDEOMODE() == MODE_NTSC )
      {
         C6502::EMULATE ( m_curCycles/PPU_CPU_RATIO_NTSC );

         // Check cycles available again just incase the emulated
         // 6502 instruction caused a cycle-stealing DMA operation...
         if ( m_curCycles > 0 )
         {
            m_curCycles %= PPU_CPU_RATIO_NTSC;
         }
         else
         {
            // Give back the unused cycles...
//            m_curCycles += (storedCyclesNotExecuted%PPU_CPU_RATIO_NTSC);
         }
      }
      else
      {         
         // Check cycles available again just incase the emulated
         // 6502 instruction caused a cycle-stealing DMA operation...
         C6502::EMULATE ( m_curCycles/PPU_CPU_RATIO_PAL );

         if ( m_curCycles > 0 )
         {
            m_curCycles %= PPU_CPU_RATIO_PAL;
         }
         else
         {
            // Give back the unused cycles...
//            m_curCycles += (storedCyclesNotExecuted%PPU_CPU_RATIO_PAL);
         }
      }
   }
}

UINT CPPU::LOAD ( UINT addr, char source, char type, bool trace )
{
   unsigned char data = 0xFF;

   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      data = CROM::CHRMEM ( addr );

      // Add Tracer sample...
      if ( trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
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

         // Add Tracer sample...
         if ( trace )
         {
            CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
         }
         return data;
      }

      addr &= 0xEFFF;
   }

   data = *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));

   // Add Tracer sample...
   if ( trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   return data;
}

void CPPU::STORE ( UINT addr, unsigned char data, char source, char type, bool trace )
{
   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      // Add Tracer sample...
      if ( trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
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
         // Add Tracer sample...
         if ( trace )
         {
            CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
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

   // Add Tracer sample...
   if ( trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)) = data;
}

void CPPU::RENDERCHRMEM ( void )
{
   unsigned int ppuAddr = 0x0000;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char bit1, bit2;
   unsigned char colorIdx;
   int color[4][3];

   if ( m_bCHRMEMInspector )
   {
      color[0][0] = CBasePalette::GetPaletteR ( 0x0D );
      color[0][1] = CBasePalette::GetPaletteG ( 0x0D );
      color[0][2] = CBasePalette::GetPaletteB ( 0x0D );
      color[1][0] = CBasePalette::GetPaletteR ( 0x10 );
      color[1][1] = CBasePalette::GetPaletteG ( 0x10 );
      color[1][2] = CBasePalette::GetPaletteB ( 0x10 );
      color[2][0] = CBasePalette::GetPaletteR ( 0x20 );
      color[2][1] = CBasePalette::GetPaletteG ( 0x20 );
      color[2][2] = CBasePalette::GetPaletteB ( 0x20 );
      color[3][0] = CBasePalette::GetPaletteR ( 0x30 );
      color[3][1] = CBasePalette::GetPaletteG ( 0x30 );
      color[3][2] = CBasePalette::GetPaletteB ( 0x30 );

      for (int y = 0; y < 128; y++)
      {
          for (int x = 0; x < 256; x += 8)
          {
              ppuAddr = ((y>>3)<<8)+((x%128)<<1)+(y&0x7);
              if ( x >= 128 ) ppuAddr += 0x1000;
              patternData1 = CPPU::_MEM(ppuAddr);
              patternData2 = CPPU::_MEM(ppuAddr+8);

              for ( int xf = 0; xf < 8; xf++ )
              {
                 bit1 = (patternData1>>(7-(xf)))&0x1;
                 bit2 = (patternData2>>(7-(xf)))&0x1;
                 colorIdx = (bit1|(bit2<<1));
                 m_pCHRMEMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 0] = color[colorIdx][0];
                 m_pCHRMEMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 1] = color[colorIdx][1];
                 m_pCHRMEMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 2] = color[colorIdx][2];
             }
          }
      }
   }
}

void CPPU::RENDEROAM ( void )
{
   int x, xf, y, yf;
   unsigned short spritePatBase = 0x0000;
   unsigned char patternIdx;
   unsigned char spriteAttr;
   int           spriteSize;
   int           sprite;
   unsigned char spriteFlipVert;
   unsigned char spriteFlipHoriz;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char attribData;
   unsigned char bit1, bit2;
   unsigned char colorIdx;
   unsigned char spriteY;
   QColor color[4];

   if ( m_bOAMInspector )
   {
      color[0] = CBasePalette::GetPalette ( 0x0D );
      color[1] = CBasePalette::GetPalette ( 0x10 );
      color[2] = CBasePalette::GetPalette ( 0x20 );
      color[3] = CBasePalette::GetPalette ( 0x30 );

      spriteSize = ((!!(CPPU::_PPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;
      if ( spriteSize == 8 )
      {
         spritePatBase = (!!(CPPU::_PPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
      }

      for ( y = 0; y < spriteSize<<1; y++ )
      {
         for ( x = 0; x < 256; x += PATTERN_SIZE ) // pattern-slice rendering...
         {
            sprite = (spriteSize==8)?((y>>3)<<5)+(x>>3):
                                     ((y>>4)<<5)+(x>>3);
            spriteY = CPPU::OAM ( SPRITEY, sprite );
   // CPTODO: find replacement way to do OnScreen check
   //         if ( ((m_bOnscreen) && ((spriteY+1) < SPRITE_YMAX)) ||
   //              (!m_bOnscreen) )
            {
               patternIdx = CPPU::OAM ( SPRITEPAT, sprite );
               if ( spriteSize == 16 )
               {
                  spritePatBase = (patternIdx&0x01)<<12;
                  patternIdx &= 0xFE;
               }

               spriteAttr = CPPU::OAM ( SPRITEATT, sprite );
               spriteFlipVert = !!(spriteAttr&SPRITE_FLIP_VERT);
               spriteFlipHoriz = !!(spriteAttr&SPRITE_FLIP_HORIZ);
               attribData = (spriteAttr&SPRITE_PALETTE_IDX_MSK)<<2;

               // For 8x16 sprites...
               if ( (spriteSize == 16) &&
                    (((!spriteFlipVert) && (((y>>3)&1))) ||
                    ((spriteFlipVert) && (!((y>>3)&1)))) )
               {
                  patternIdx++;
               }

               yf = y&0x7;
               if ( spriteFlipVert )
               {
                  yf = (7-yf);
               }

               patternData1 = CPPU::_MEM ( spritePatBase+(patternIdx<<4)+(yf) );
               patternData2 = CPPU::_MEM ( spritePatBase+(patternIdx<<4)+(yf)+PATTERN_SIZE );

               for ( xf = 0; xf < PATTERN_SIZE; xf++ )
               {
                  if ( spriteFlipHoriz )
                  {
                     bit1 = (patternData1>>((xf)))&0x1;
                     bit2 = (patternData2>>((xf)))&0x1;
                  }
                  else
                  {
                     bit1 = (patternData1>>(7-(xf)))&0x1;
                     bit2 = (patternData2>>(7-(xf)))&0x1;
                  }
                  colorIdx = (attribData|bit1|(bit2<<1));
                  m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 0] = CBasePalette::GetPaletteR(CPPU::_PALETTE(0x10+colorIdx));
                  m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 1] = CBasePalette::GetPaletteG(CPPU::_PALETTE(0x10+colorIdx));
                  m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 2] = CBasePalette::GetPaletteB(CPPU::_PALETTE(0x10+colorIdx));
               }
            }
   // CPTODO: find replacement way to do OnScreen check
   //         else
   //         {
   //            for ( xf = 0; xf < PATTERN_SIZE; xf++ )
   //            {
   //               m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 0] = 0x00;
   //               m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 1] = 0x00;
   //               m_pOAMInspectorTV[(y * 256 * 3) + (x * 3) + (xf * 3) + 2] = 0x00;
   //            }
   //         }
         }
      }
   }
}

void CPPU::RENDERNAMETABLE ( void )
{
   int x, xf, y;
   //int lbx, ubx, lby, uby;

   UINT ppuAddr = 0x0000;
   unsigned short patternIdx;
   int tileX;
   int tileY;
   int nameAddr;
   int attribAddr;
   int bkgndPatBase;
   unsigned char attribData;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char bit1, bit2;
   unsigned char colorIdx;
#if 0
   unsigned char lbx, lby, ubx, uby;
#endif

   if ( m_bNameTableInspector )
   {
      for ( y = 0; y < 480; y++ )
      {
         for ( x = 0; x < 512; x += PATTERN_SIZE ) // pattern-slice rendering...
         {
            tileX = ppuAddr&0x001F;
            tileY = (ppuAddr&0x03E0)>>5;
            nameAddr = 0x2000 + (ppuAddr&0x0FFF);
            attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
            bkgndPatBase = (!!(CPPU::_PPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;

            patternIdx = bkgndPatBase+(CPPU::_NAMETABLE(nameAddr)<<4)+((ppuAddr&0x7000)>>12);
            attribData = CPPU::_ATTRTABLE ( attribAddr );
            patternData1 = CPPU::_PATTERNDATA ( patternIdx );
            patternData2 = CPPU::_PATTERNDATA ( patternIdx+PATTERN_SIZE );

            if ( (tileY&0x0002) == 0 )
            {
               if ( (tileX&0x0002) == 0 )
               {
                  attribData = (attribData&0x03)<<2;
               }
               else
               {
                  attribData = (attribData&0x0C);
               }
            }
            else
            {
               if ( (tileX&0x0002) == 0 )
               {
                  attribData = (attribData&0x30)>>2;
               }
               else
               {
                  attribData = (attribData&0xC0)>>4;
               }
            }

            for ( xf = 0; xf < PATTERN_SIZE; xf++ )
            {
               bit1 = (patternData1>>(7-(xf)))&0x1;
               bit2 = (patternData2>>(7-(xf)))&0x1;
               colorIdx = (attribData|bit1|(bit2<<1));
               m_pNameTableInspectorTV [ (y*512*3)+(x*3)+(xf*3)+0] = CBasePalette::GetPaletteR(CPPU::_PALETTE(colorIdx));
               m_pNameTableInspectorTV [ (y*512*3)+(x*3)+(xf*3)+1] = CBasePalette::GetPaletteG(CPPU::_PALETTE(colorIdx));
               m_pNameTableInspectorTV [ (y*512*3)+(x*3)+(xf*3)+2] = CBasePalette::GetPaletteB(CPPU::_PALETTE(colorIdx));
            }

            ppuAddr++;
            if ( x == 248 || x == 504 )
            {
               ppuAddr ^= 0x400;
               ppuAddr -= 0x20;
            }
         }
         if ( (ppuAddr&0x7000) == 0x7000 )
         {
            ppuAddr &= 0x8FFF;
            if ( (ppuAddr&0x03E0) == 0x03A0 )
            {
               ppuAddr ^= 0x0800;
               ppuAddr &= 0xFC1F;
            }
            else
            {
               if ( (ppuAddr&0x03E0) == 0x03E0 )
               {
                  ppuAddr &= 0xFC1F;
               }
               else
               {
                  ppuAddr += 0x0020;
               }
            }
         }
         else
         {
            ppuAddr += 0x1000;
         }
      }

// CPTODO: implement shadow disablement
#if 0
      //   if ( m_bShadow )
      {
         for ( y = 0; y < 480; y++ )
         {
            for ( x = 0; x < 512; x++ )
            {
               lbx = CPPU::_SCROLLX(x&0xFF,y%240);
               ubx = lbx>>8?lbx&0xFF:lbx+255;
               lby = CPPU::_SCROLLY(x&0xFF,y%240);
               uby = lby/240?lby%240:lby+239;

               if ( !( (((lbx <= ubx) && (x >= lbx) && (x <= ubx)) ||
                    ((lbx > ubx) && (!((x <= lbx) && (x >= ubx))))) &&
                    (((lby <= uby) && (y >= lby) && (y <= uby)) ||
                    ((lby > uby) && (!((y <= lby) && (y >= uby))))) ) )
               {
                  if ( m_pNameTableInspectorTV [ (y*512*3)+(x*3)+0 ] >= 0x30 )
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+0 ] -= 0x30;
                  }
                  else
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+0 ] = 0x00;
                  }
                  if ( m_pNameTableInspectorTV [ (y*512*3)+(x*3)+1 ] >= 0x30 )
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+1 ] -= 0x30;
                  }
                  else
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+1 ] = 0x00;
                  }
                  if ( m_pNameTableInspectorTV [ (y*512*3)+(x*3)+2 ] >= 0x30 )
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+2 ] -= 0x30;
                  }
                  else
                  {
                     m_pNameTableInspectorTV [ (y*512*3)+(x*3)+2 ] = 0x00;
                  }
               }
            }
         }
      }
#endif
   }
}

UINT CPPU::RENDER ( UINT addr, char target )
{
   UINT data;

   data = LOAD ( addr, eSource_PPU, target );

   m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, addr, data, eLogger_DataRead, eSource_PPU );

   // Provide PPU cycle and address to mappers that watch such things!
   mapperfunc[CROM::MAPPER()].synch(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE();
   INCCYCLE();

   // Check for PPU cycle breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );

   // Check for breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch, data );

   return data;
}

void CPPU::GARBAGE ( UINT addr, char target )
{
   CNES::TRACER()->AddGarbageFetch ( m_cycles, target, addr );

   // Provide PPU cycle and address to mappers that watch such things!
   mapperfunc[CROM::MAPPER()].synch(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE();
   INCCYCLE();

   // Check for PPU cycle breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );

   // Check for breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
}

void CPPU::EXTRA ()
{
   CNES::TRACER()->AddGarbageFetch ( m_cycles, eTarget_ExtraCycle, 0 );

   // Idle cycle...
   EMULATE();
   INCCYCLE();

   // Check for PPU cycle breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );

   // Check for breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
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

   m_ppuAddr = 0x0000;
   m_ppuAddrLatch = 0x0000;
   m_ppuAddrIncrement = 1;
   m_ppuReadLatch = 0x00;
   m_ppuRegByte = 0;
}

UINT CPPU::PPU ( UINT addr )
{
   unsigned char data = 0xFF;
   unsigned short fixAddr;
   unsigned short oldPpuAddr;

   if ( addr < 0x4000 )
   {
      fixAddr = addr&0x0007;
      if ( fixAddr == PPUSTATUS_REG )
      {
         data = *(m_PPUreg+fixAddr);
         *(m_PPUreg+fixAddr) &= (~PPUSTATUS_VBLANK); // VBLANK clear-on-read
         m_ppuRegByte = 0; // Clear PPUADDR address latch
         data = (data&0xE0)|(m_ppuReadLatch&0x1F);

         // Kill NMI if flag is read at 'wrong' time...
         if ( CYCLES() == PPU_CYCLE_START_VBLANK-1 )
         {
            VBLANKCHOKED ( true );
         }
      }
      else if ( fixAddr == OAMDATA_REG )
      {
         if ( (m_oamAddr&3) == SPRITEATT )
         {
            // Third sprite byte should be masked with E3 on read.
            data = (*(m_PPUoam+m_oamAddr))&0xE3;
         }
         else
         {
            data = *(m_PPUoam+m_oamAddr);
         }

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalRead, data );
      }
      else if ( fixAddr == PPUDATA_REG )
      {
         oldPpuAddr = m_ppuAddr;

         m_ppuAddr += m_ppuAddrIncrement;

         if ( oldPpuAddr < 0x3F00 )
         {
            data = m_ppuReadLatch;

            m_ppuReadLatch = LOAD ( oldPpuAddr, eSource_CPU, eTracer_DataRead );
         }
         else
         {
            data = LOAD ( oldPpuAddr, eSource_CPU, eTracer_DataRead );

            // Shadow palette/VRAM read, don't use regular LOAD or it will be logged in Tracer...
            m_ppuReadLatch = *((*(m_pPPUmemory+(((oldPpuAddr&(~0x1000))&0x1FFF)>>10)))+((oldPpuAddr&(~0x1000))&0x3FF));
         }

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalRead, data );

         // Log Code/Data logger...
         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataRead, eSource_CPU );

         // Toggling A12 causes IRQ count in some mappers...
         mapperfunc[CROM::MAPPER()].synch(m_cycles,m_ppuAddr);
      }
      else
      {
         data = m_ppuReadLatch;
      }

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
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

   if ( addr < 0x4000 )
   {
      fixAddr = addr&0x0007;
      if ( fixAddr != PPUSTATUS_REG )
      {
         *(m_PPUreg+fixAddr) = data;
      }
      if ( fixAddr == PPUCTRL_REG )
      {
         m_ppuAddrLatch &= 0x73FF;
         m_ppuAddrLatch |= ((((unsigned short)data&0x03))<<10);
         m_ppuAddrIncrement = (((!!(data&PPUCTRL_VRAM_ADDR_INC))*31)+1);
      }
      else if ( fixAddr == OAMADDR_REG )
      {
         m_oamAddr = data;
      }
      else if ( fixAddr == OAMDATA_REG )
      {
         *(m_PPUoam+m_oamAddr) = data;

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalWrite, data );

         m_oamAddr++;
      }
      else if ( fixAddr == PPUSCROLL_REG )
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
      else if ( fixAddr == PPUADDR_REG )
      {
         oldPpuAddr = m_ppuAddr;
         if ( m_ppuRegByte )
         {
            m_ppuAddrLatch &= 0x7F00;
            m_ppuAddrLatch |= data;

            m_ppuAddr = m_ppuAddrLatch;

            // Toggling A12 causes IRQ count in some mappers...
            mapperfunc[CROM::MAPPER()].synch(m_cycles,m_ppuAddr);
         }
         else
         {
            m_ppuAddrLatch &= 0x00FF;
            m_ppuAddrLatch |= ((((unsigned short)data&0x3F))<<8);
         }
         m_ppuRegByte = !m_ppuRegByte;
      }
      else if ( fixAddr == PPUDATA_REG )
      {
         STORE ( m_ppuAddr, data, eSource_CPU, eTracer_DataWrite );

         oldPpuAddr = m_ppuAddr;

         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataWrite, eSource_CPU );

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalWrite, data );

         // Increment PPUADDR
         m_ppuAddr += m_ppuAddrIncrement;

         // Toggling A12 causes IRQ count in some mappers...
         mapperfunc[CROM::MAPPER()].synch(m_cycles,m_ppuAddr);
      }

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
   }
   else if ( addr < 0x5000 )
   {
      if ( addr == IOSPRITEDMA )
      {
         // Check for PPU cycle breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_SPRITE_DMA );

         // DMA
         int start = m_oamAddr;
         for ( dma = 0; dma < NUM_OAM_REGS; dma++ )
         {
            *(m_PPUoam+((dma+start)&0xFF)) = C6502::DMA ( (data<<8)|(dma&0xFF), eSource_PPU );
         }

         // Steal CPU cycles...
         // 512 CPU cycles * 3 PPU cycles per CPU
         if ( CNES::VIDEOMODE() == MODE_NTSC )
         {
            STEALCYCLES(512*PPU_CPU_RATIO_NTSC);
         }
         else
         {
            STEALCYCLES(512*PPU_CPU_RATIO_PAL);
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
   else if ( vert )
   {
      MIRRORVERT ();
   }
   else
   {
      MIRRORHORIZ ();
   }
}

void CPPU::MIRRORVERT ( void )
{
   MIRROR ( 0, 1, 0, 1 );
}

void CPPU::MIRRORHORIZ ( void )
{
   MIRROR ( 0, 0, 1, 1 );
}

void CPPU::MIRROR ( int nt1, int nt2, int nt3, int nt4 )
{
   nt1 &= 0x3;
   nt2 &= 0x3;
   nt3 &= 0x3;
   nt4 &= 0x3;
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

void CPPU::NONRENDERSCANLINES ( int scanlines )
{
   int idxx, idxy;

   for ( idxy = 0; idxy < scanlines; idxy++ )
   {
      for ( idxx = 0; idxx < PPU_CYCLES_PER_SCANLINE; idxx++ )
      {
         EMULATE();
         INCCYCLE();

         // Check for breakpoints...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );
      }
   }
}

void CPPU::PIXELPIPELINES ( int pickoff, unsigned char *a, unsigned char *b1, unsigned char *b2 )
{
   BackgroundBufferData* pBkgnd1 = m_bkgndBuffer.data;
   BackgroundBufferData* pBkgnd2 = m_bkgndBuffer.data+1;

   // Adjust pickoff point to place within stored data...
   pickoff = 7-pickoff;

   // Retrieve tile color-bit values at pickoff point...
   (*b1) = (pBkgnd1->patternData1>>pickoff)&0x1;
   (*b2) = (pBkgnd1->patternData2>>pickoff)&0x1;

   // Retrieve attribute table color-bit values at pickoff point...
   (*a) = ((pBkgnd1->attribData1>>pickoff)&0x1)|(((pBkgnd1->attribData2>>pickoff)&0x1)<<1);

   // Shift to the left to line up for next pickoff...
   pBkgnd1->patternData1 <<= 1;
   pBkgnd1->patternData2 <<= 1;
   pBkgnd1->patternData1 |= (!!(pBkgnd2->patternData1&0x80));
   pBkgnd1->patternData2 |= (!!(pBkgnd2->patternData2&0x80));
   pBkgnd2->patternData1 <<= 1;
   pBkgnd2->patternData2 <<= 1;
   pBkgnd1->attribData1 <<= 1;
   pBkgnd1->attribData2 <<= 1;
   pBkgnd1->attribData1 |= (!!(pBkgnd2->attribData1&0x80));
   pBkgnd1->attribData2 |= (!!(pBkgnd2->attribData2&0x80));
   pBkgnd2->attribData1 <<= 1;
   pBkgnd2->attribData2 <<= 1;
}

void CPPU::RENDERSCANLINE ( int scanline )
{
   int rasttv = (scanline<<8)*3;
   int idxx;
   int sprite;
   int p = 0;
   SpriteBufferData* pSprite;
   SpriteBufferData* pSelectedSprite;
   int idx2;
   char* pTV = (char*)(m_pTV+rasttv);

   m_x = 0;
   m_y = scanline;

   // Check for start-of-scanline breakpoints...
   if ( scanline == -1 )
   {
      CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_START);
   }
   else
   {
      CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_START);
   }

   for ( idxx = 0; idxx < 256; idxx += 8 )
   {
      int spriteColorIdx = 0;
      int bkgndColorIdx = 0;
      int startBkgnd = (!(rPPU(PPUMASK)&PPUMASK_BKGND_CLIPPING))<<3;
      int startSprite = (!(rPPU(PPUMASK)&PPUMASK_SPRITE_CLIPPING))<<3;
      int patternMask;

      for ( patternMask = 0; patternMask < 8; patternMask++ )
      {
         unsigned char a, b1, b2;

         if ( scanline >= 0 )
         {
            m_x = idxx+patternMask;

            // Update variables for PPU viewer
            *(*(m_2005x+m_x)+m_y) = m_last2005x+((rPPU(PPUCTRL)&0x1)<<8);
            *(*(m_2005y+m_x)+m_y) = m_last2005y+(((rPPU(PPUCTRL)&0x2)>>1)*240);

            // Check for PPU pixel-at breakpoint...
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PIXEL_XY);
         }

         // Only render to the screen on the visible scanlines...
         if ( scanline >= 0 )
         {
            // Run sprite multiplexer to figure out what, if any,
            // sprite pixel to draw here...
            pSelectedSprite = NULL;

            for ( sprite = 0; sprite < m_spriteBuffer.count; sprite++ )
            {
               pSprite = m_spriteBuffer.data + (*(m_spriteBuffer.order+sprite));
               idx2 = p - pSprite->spriteX;
               if ( (idx2 >= 0) && (idx2 < PATTERN_SIZE) &&
                    (pSprite->spriteX+idx2 >= startSprite) &&
                    (pSprite->spriteX+idx2 >= startBkgnd) )
               {
                  // Check for sprite-in-multiplexer event breakpoint...
                  CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSprite->spriteIdx,PPU_EVENT_SPRITE_IN_MULTIPLEXER);

                  if ( pSprite->spriteFlipHoriz )
                  {
                     spriteColorIdx = ((pSprite->patternData1>>idx2)&0x01)|((((pSprite->patternData2>>idx2)&0x01)<<1) );
                  }
                  else
                  {
                     spriteColorIdx = ((pSprite->patternData1>>(7-idx2))&0x01)|((((pSprite->patternData2>>(7-idx2))&0x01)<<1) );
                  }
                  spriteColorIdx |= (pSprite->attribData<<2);

                  if ( !(spriteColorIdx&0x3) ) spriteColorIdx = 0;

                  if ( spriteColorIdx&0x3 )
                  {
                     // Check for sprite selected event breakpoint...
                     CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSprite->spriteIdx,PPU_EVENT_SPRITE_SELECTED);

                     // Save rendered sprite for multiplexing with background...
                     pSelectedSprite = pSprite;
                     break;
                  }
               }
            }

            // Background pixel determination...
            PIXELPIPELINES ( rSCROLLX(), &a, &b1, &b2 );
            bkgndColorIdx = ((a<<2)|b1|(b2<<1));

            if ( !(bkgndColorIdx&0x3) ) bkgndColorIdx = 0;

            // Sprite/background pixel rendering determination...
            if ( (pSelectedSprite) &&
                 ((!(pSelectedSprite->spriteBehind)) ||
                 ((bkgndColorIdx == 0) &&
                 (spriteColorIdx != 0))) &&
                 (rPPU(PPUMASK)&PPUMASK_RENDER_SPRITES) )
            {
               // Check for sprite rendering event breakpoint...
               CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSelectedSprite->spriteIdx,PPU_EVENT_SPRITE_RENDERING);

               // Draw sprite...
               *pTV = CBasePalette::GetPaletteR(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
            }
            else
            {
               if ( (idxx>=startBkgnd) && (rPPU(PPUMASK)&PPUMASK_RENDER_BKGND) )
               {
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
               else
               {
                  if ( (m_ppuAddr&0x3F00) == 0x3F00 )
                  {
                     *pTV = CBasePalette::GetPaletteR(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                     *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                     *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  }
                  else
                  {
                     *pTV = CBasePalette::GetPaletteR(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                     *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                     *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  }
               }
            }

            // Sprite 0 hit checks...
            if ( (pSelectedSprite) &&
                 (pSelectedSprite->spriteIdx == 0) &&
                 (!(rPPU(PPUSTATUS)&PPUSTATUS_SPRITE_0_HIT)) &&
                 (bkgndColorIdx != 0) &&
                 (p < 255) )
            {
               if ( ((rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) == (PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) )
               {
                  wPPU ( PPUSTATUS, rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_0_HIT );

                  // Save last sprite 0 hit coords for OAM viewer...
                  m_lastSprite0HitX = p;
                  m_lastSprite0HitY = scanline;

                  // TODO: update tracer info
                  CNES::TRACER()->AddSample ( m_cycles, eTracer_Sprite0Hit, eSource_PPU, 0, 0, 0 );

                  // Check for Sprite 0 Hit breakpoint...
                  CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SPRITE0_HIT);
               }
            }

            // Move to next pixel...
            pTV += 3;
            p++;
         }

         GATHERBKGND ( patternMask );
      }
   }

   // Check for end-of-scanline breakpoints...
   if ( scanline == -1 )
   {
      CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_END);
   }
   else
   {
      CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_END);
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
   GATHERBKGND ( 0 );
   GATHERBKGND ( 1 );
   GATHERBKGND ( 2 );
   GATHERBKGND ( 3 );
   GATHERBKGND ( 4 );
   GATHERBKGND ( 5 );
   GATHERBKGND ( 6 );
   GATHERBKGND ( 7 );
   memcpy ( m_bkgndBuffer.data, m_bkgndBuffer.data+1, sizeof(BackgroundBufferData) );
   GATHERBKGND ( 0 );
   GATHERBKGND ( 1 );
   GATHERBKGND ( 2 );
   GATHERBKGND ( 3 );
   GATHERBKGND ( 4 );
   GATHERBKGND ( 5 );
   GATHERBKGND ( 6 );
   GATHERBKGND ( 7 );

   // Finish off scanline render clock cycles...
   EMULATE();
   INCCYCLE();
   GARBAGE ( 0x2000, eTarget_NameTable );
   EMULATE();
   INCCYCLE();
   GARBAGE ( 0x2000, eTarget_NameTable );

   // If this is a visible scanline it is 341 clocks long both NTSC and PAL...
   if ( scanline >= 0 )
   {
      // ...account for extra clock (341)
      EXTRA ();
   }
   // Otherwise, if this is the pre-render scanline it is:
   // 341 dots for PAL, always
   // 340 dots for NTSC odd frames and 341 dots for NTSC even frames if rendering is off, 341 dots for all frames otherwise
   else
   {
      if ( (CNES::VIDEOMODE() == MODE_PAL) || ((CNES::VIDEOMODE() == MODE_NTSC) && ((!(m_frame&1)) || (!(rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND))))) )
      {
         // account for extra clock (341)
         EXTRA ();
      }
   }
}

void CPPU::GATHERBKGND ( char phase )
{
   static unsigned short patternIdx;
   static BackgroundBufferData bkgndTemp;

   UINT ppuAddr = rPPUADDR();
   int tileX = ppuAddr&0x001F;
   int tileY = (ppuAddr&0x03E0)>>5;
   int nameAddr = 0x2000 + (ppuAddr&0x0FFF);
   int attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
   unsigned char attribData;
   int bkgndPatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;
   BackgroundBufferData* pBkgnd = m_bkgndBuffer.data + 1;

   if ( !(phase&1) )
   {
      EMULATE();
      INCCYCLE();
   }

   if ( phase == 1 )
   {
      patternIdx = bkgndPatBase+(RENDER(nameAddr,eTracer_RenderBkgnd)<<4)+((ppuAddr&0x7000)>>12);
      mapperfunc[CROM::MAPPER()].latch ( patternIdx );
   }
   else if ( phase == 3 )
   {
      attribData = RENDER ( attribAddr,eTracer_RenderBkgnd );

      if ( (tileY&0x0002) == 0 )
      {
         if ( (tileX&0x0002) == 0 )
         {
            if ( attribData&0x01 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }
            if ( attribData&0x02 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
         else
         {
            if ( attribData&0x04 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }
            if ( attribData&0x08 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
      }
      else
      {
         if ( (tileX&0x0002) == 0 )
         {
            if ( attribData&0x10 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }
            if ( attribData&0x20 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
         else
         {
            if ( attribData&0x40 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }
            if ( attribData&0x80 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
      }
   }
   else if ( phase == 5 )
   {
      bkgndTemp.patternData1 = RENDER ( patternIdx,eTracer_RenderBkgnd );
   }
   else if ( phase == 7 )
   {
      bkgndTemp.patternData2 = RENDER ( patternIdx+PATTERN_SIZE,eTracer_RenderBkgnd );

      memcpy ( pBkgnd, &bkgndTemp, sizeof(BackgroundBufferData) );

      if ( rPPU(PPUMASK)&PPUMASK_RENDER_BKGND )
      {
         m_ppuAddr++;
         if ((m_ppuAddr&0x001F) == 0)
         {
            m_ppuAddr   ^= 0x0400;
            m_ppuAddr   -= 0x0020;
         }
      }
   }
}

void CPPU::GATHERSPRITES ( int scanline )
{
   int idx1, idx2;
   int sprite;
   int spritesFound = 0;
   unsigned short spritePatBase = 0x0000;
   int           spriteY;
   unsigned char patternIdx;
   unsigned char spriteAttr;
   int           spriteSize;
   SpriteBufferData* pSprite;
   int           yByte = SPRITEY;
   int           roll = 0;
   SpriteBufferData devNull;

   // Clear sprite buffer....
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

   // Loop invariants...
   spriteSize = ((!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;
   if ( spriteSize == 8 )
   {
      spritePatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
   }

   scanline++;

   // Populate sprite buffer...
   for ( sprite = 0; sprite < 64; sprite++ )
   {
      // Retrieve OAM byte for scanline check...
      // Note: obscure PPU 'bug' in that 9th sprite on scanline
      // causes other sprite data to be used as the Y-coordinate.
      spriteY = OAM ( yByte, sprite ) + 1;

      // idx1 is sprite slice (it will be in range 0-7 or 0-15 if the sprite
      // is on the next scanline.
      idx1 = scanline-spriteY;

      // If we've found 8 sprites on this scanline, enable the
      // obscure PPU 'bug'.
      if ( (spritesFound == 8) && (!(idx1 >= 0) && (idx1 < spriteSize)) )
      {
         roll = 1;
      }

      // Is the sprite on this scanline?
      if ( (pSprite != &devNull) && (idx1 >= 0) && (idx1 < spriteSize) )
      {
         // Put sprite in sprite buffer...
         pSprite->spriteIdx = sprite;

         patternIdx = OAM ( SPRITEPAT, sprite );
         if ( spriteSize == 16 )
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
         if ( (spriteSize == 16) &&
              (((!pSprite->spriteFlipVert) && (idx1 >= PATTERN_SIZE)) ||
              ((pSprite->spriteFlipVert) && (idx1 < PATTERN_SIZE))) )
         {
            patternIdx++;
         }
         if ( pSprite->spriteFlipVert )
         {
            idx1 = spriteSize-1-idx1;
         }

         // Garbage nametable fetches according to Samus Aran...
         EMULATE();
         INCCYCLE();
         GARBAGE ( 0x2000, eTarget_NameTable );
         EMULATE();
         INCCYCLE();
         GARBAGE ( 0x2000, eTarget_NameTable );

         // Get sprite's pattern data...
         EMULATE();
         INCCYCLE();
         pSprite->patternData1 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7), eTracer_RenderSprite );
         mapperfunc[CROM::MAPPER()].latch ( spritePatBase+(patternIdx<<4)+(idx1&0x7) );
         EMULATE();
         INCCYCLE();
         pSprite->patternData2 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7)+PATTERN_SIZE, eTracer_RenderSprite );
      }

      // Is the sprite on this scanline?
      if ( (idx1 >= 0) && (idx1 < spriteSize) )
      {
         // Calculate sprite-per-scanline limit...
         spritesFound++;
         if ( spritesFound < 8 )
         {
            // Not at limit yet, keep storing sprites in the buffer...
            pSprite++;
         }
         else
         {
            // Found 8 sprites, point to /dev/null for future sprites on this scanline...
            pSprite = &devNull;

            // Should we assert sprite overflow?
            if ( spritesFound == 9 )
            {
               if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
               {
                  if ( !(rPPU(PPUSTATUS)&PPUSTATUS_SPRITE_OVFLO) )
                  {
                     wPPU(PPUSTATUS,rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_OVFLO );

                     // Check for breakpoint...
                     CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_SPRITE_OVERFLOW );
                  }
               }
            }
         }
      }

      // The obscure PPU 'bug' behavior rolls through different
      // sprite data bytes as the Y-coordinate...
      if ( roll )
      {
         yByte++;
         yByte %= OAM_SIZE;
      }
   }

   // Fix up the sprite buffer data if necessary...
   if ( spritesFound > 8 )
   {
      spritesFound = 8;
   }
   m_spriteBuffer.count = spritesFound;

   // Perform remaining garbage fetches to finish out the scanline's
   // PPU cycles...
   for ( sprite = m_spriteBuffer.count; sprite < 8; sprite++ )
   {
      // Garbage nametable fetches according to Samus Aran...
      EMULATE();
      INCCYCLE();
      GARBAGE ( 0x2000, eTarget_NameTable );
      EMULATE();
      INCCYCLE();
      GARBAGE ( 0x2000, eTarget_NameTable );
      if ( spriteSize == 16 )
      {
         spritePatBase = (GARBAGE_SPRITE_FETCH&0x01)<<12;
         patternIdx &= 0xFE;
      }
      EMULATE();
      INCCYCLE();
      GARBAGE ( spritePatBase+(GARBAGE_SPRITE_FETCH<<4), eTarget_PatternMemory );
      EMULATE();
      INCCYCLE();
      GARBAGE ( spritePatBase+(GARBAGE_SPRITE_FETCH<<4)+PATTERN_SIZE, eTarget_PatternMemory );
   }

   // Order sprites by priority for rendering...
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
}
