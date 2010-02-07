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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
#define NUM_PPU_EVENTS 1

bool ppuRasterPositionEvent(BreakpointInfo* pBreakpoint)
{
   if ( (pBreakpoint->item1 == CPPU::_X()) &&
        (pBreakpoint->item2 == CPPU::_Y()) )
   {
      return true;
   }
   return false;
}

static CBreakpointEventInfo* tblPPUEvents [] =
{
   new CBreakpointEventInfo("Raster Position (Data1=X,Data2=Y)", ppuRasterPositionEvent, 2, "Break at pixel (%d,%d)", 10)
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
unsigned char  CPPU::m_ppuReadLatch2007 = 0x00;
unsigned char  CPPU::m_PPUreg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  CPPU::m_PPUoam [] = { 0x00, };
unsigned char  CPPU::m_ppuScrollX = 0x00;
bool           CPPU::m_mirrorVert = false;
bool           CPPU::m_mirrorHoriz = false;
int            CPPU::m_oneScreen = -1;
bool           CPPU::m_extraVRAM = false;

CCodeDataLogger CPPU::m_logger;
unsigned int   CPPU::m_cycles = 0;
int            CPPU::m_mode = MODE_NTSC;
char           CPPU::m_szBinaryText [] = { 0, };

char*          CPPU::m_pTV = NULL;

char*          CPPU::m_pCHRMEMInspectorTV = NULL;
bool           CPPU::m_bCHRMEMInspector = false;
char*          CPPU::m_pOAMInspectorTV = NULL;
bool           CPPU::m_bOAMInspector = false;
char*          CPPU::m_pNameTableInspectorTV = NULL;
bool           CPPU::m_bNameTableInspector = false;

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

void CPPU::INCCYCLE()
{
   m_curCycles++;
   m_cycles++;
}

UINT CPPU::LOAD ( UINT addr, char source, char type )
{
   unsigned char data = 0xFF;

   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      data = CROM::CHRMEM ( addr );

      // Add Tracer sample...
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );

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
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );

         return data;
      }

      addr &= 0xEFFF;
   }

   data = *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));

   // Add Tracer sample...
   if ( (addr&0x3FF) < 0x3C0 )
   {
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
   }
   else
   {
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
   }

   return data;
}

void CPPU::STORE ( UINT addr, unsigned char data, char source, char type )
{
   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      // Add Tracer sample...
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );

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
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );

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
   if ( (addr&0x3FF) < 0x3C0 )
   {
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
   }
   else
   {
      CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
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

      spriteSize = (!!(CPPU::_PPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE)) + 1;
      if ( spriteSize == 1 )
      {
         spritePatBase = (!!(CPPU::_PPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
      }

      for ( y = 0; y < spriteSize<<4; y++ )
      {
         for ( x = 0; x < 256; x += PATTERN_SIZE ) // pattern-slice rendering...
         {
            sprite = (spriteSize==1)?((y>>3)<<5)+(x>>3):
                                     ((y>>4)<<5)+(x>>3);
            spriteY = CPPU::OAM ( SPRITEY, sprite );
   // CPTODO: find replacement way to do OnScreen check
   //         if ( ((m_bOnscreen) && ((spriteY+1) < SPRITE_YMAX)) ||
   //              (!m_bOnscreen) )
            {
               patternIdx = CPPU::OAM ( SPRITEPAT, sprite );
               if ( spriteSize == 2 )
               {
                  spritePatBase = (patternIdx&0x01)<<12;
                  patternIdx &= 0xFE;
               }

               spriteAttr = CPPU::OAM ( SPRITEATT, sprite );
               spriteFlipVert = !!(spriteAttr&SPRITE_FLIP_VERT);
               spriteFlipHoriz = !!(spriteAttr&SPRITE_FLIP_HORIZ);
               attribData = (spriteAttr&SPRITE_PALETTE_IDX_MSK)<<2;

               // For 8x16 sprites...
               if ( (spriteSize == 2) &&
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
   unsigned char lbx, lby, ubx, uby;

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

   m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, addr, data, eLogger_DataRead, eLoggerSource_PPU );

   // Address/Data bus multiplexed thus 2 cycles required per access...
   INCCYCLE();
   INCCYCLE();

   // Check for breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch, data );

   return data;
}

void CPPU::GARBAGE ( char target )
{
   CNES::TRACER()->AddGarbageFetch ( m_cycles, target );

   // Address/Data bus multiplexed thus 2 cycles required per access...
   INCCYCLE();
   INCCYCLE();

   // Check for breakpoint...
   CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
}

void CPPU::EXTRA ()
{
   CNES::TRACER()->AddGarbageFetch ( m_cycles, eTarget_Unknown );

   // Idle cycle...
   INCCYCLE();

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

   CNES::TRACER()->AddRESET ();

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

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalRead, data );
      }
      else if ( addr == PPUDATA )
      {
         m_ppuReadLatch = m_ppuReadLatch2007;
         oldPpuAddr = m_ppuAddr;

         m_ppuAddr += m_ppuAddrIncrement;

         if ( oldPpuAddr < 0x3F00 )
         {
            m_ppuReadLatch2007 = LOAD ( oldPpuAddr, eSource_CPU, eTracer_DataRead );

            data = m_ppuReadLatch;
         }
         else
         {
            data = LOAD ( oldPpuAddr, eSource_CPU, eTracer_DataRead );
         }

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalRead, data );

         // Log Code/Data logger...
         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataRead, eLoggerSource_CPU );

         // Toggling A12 causes IRQ count in some mappers...
         if ( (!(oldPpuAddr&0x1000)) && ((oldPpuAddr^m_ppuAddr)&0x1000) )
         {
            if ( mapperfunc[CROM::MAPPER()].synch(-1) )
            {
               C6502::IRQ( eSource_Mapper );
            }
         }
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

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalWrite, data );

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
            }
         }
      }
      else if ( addr == PPUDATA )
      {
         STORE ( m_ppuAddr, data, eSource_CPU, eTracer_DataWrite );

         oldPpuAddr = m_ppuAddr;

         m_logger.LogAccess ( C6502::CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataWrite, eLoggerSource_CPU );

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalWrite, data );

         // Increment PPUADDR
         m_ppuAddr += m_ppuAddrIncrement;

         // Toggling A12 causes IRQ count in some mappers...
         if ( (!(oldPpuAddr&0x1000)) && ((oldPpuAddr^m_ppuAddr)&0x1000) )
         {
            if ( mapperfunc[CROM::MAPPER()].synch(-1) )
            {
               C6502::IRQ( eSource_Mapper );
            }
         }
      }

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
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
#if 0
   int idxx;
   int idxy = (scanline<<8)*3;
   int offsetx = 0;

   if ( m_pTV )
   {
       for ( idxx = 0; idxx < 256; idxx++ )
       {
          // black!
          *(m_pTV+idxy+offsetx) = 0;
          *(m_pTV+idxy+offsetx+1) = 0;
          *(m_pTV+idxy+offsetx+2) = 0;
          offsetx += 3;
       }
   }
#endif
}

void CPPU::NONRENDERSCANLINE ( int scanlines )
{
   int idxx, idxy;

   for ( idxy = 0; idxy < scanlines; idxy++ )
   {
      for ( idxx = 0; idxx < 341; idxx++ )
      {
         INCCYCLE();
         C6502::EMULATE ( true, m_curCycles/3 );
         m_curCycles %= 3;

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU );
      }
   }
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

void CPPU::RENDERSCANLINE ( int scanline )
{
   int rasttv = (scanline<<8)*3;
   int idxx;
   int sprite;
   int p = 0;
   bool sprite0HitSet = false;
   SpriteBufferData* pSprite;
   int idx2;
   char* pTV = (char*)(m_pTV+rasttv);

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

            m_x = idxx+patternMask;
            m_y = scanline;

            // Check for PPU event breakpoint...
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0);

            PIXELPIPELINES ( rSCROLLX(), patternMask, &a, &b1, &b2 );
            colorIdx = (a|b1|(b2<<1));

            // Update variables for PPU viewer
            *(*(m_2005x+(idxx+patternMask))+scanline) = m_last2005x+((rPPU(PPUCTRL)&0x1)<<8);
            *(*(m_2005y+(idxx+patternMask))+scanline) = m_last2005y+(((rPPU(PPUCTRL)&0x2)>>1)*240);

            if ( (idxx>=startBkgnd) && (rPPU(PPUMASK)&PPUMASK_RENDER_BKGND) )
            {
               if ( !(colorIdx&0x3) ) colorIdx = 0;
               if ( colorIdx&0x3 ) tvSet |= 1;

               *pTV = CBasePalette::GetPaletteR(rPALETTE(colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
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
                           *pTV = CBasePalette::GetPaletteR(rPALETTE(0x10+colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                           *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0x10+colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                           *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0x10+colorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
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
                              CNES::TRACER()->AddSample ( m_cycles, eTracer_Sprite0Hit, 0, 0, 0, 0 );
                           }
                        }
                        tvSet |= 0x2;
                     }
                  }
               }
            }

            // Move to next pixel...
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
   C6502::EMULATE ( true, m_curCycles/3 );
   m_curCycles %= 3;
}

void CPPU::GATHERBKGND ( void )
{
   UINT ppuAddr = rPPUADDR();
   unsigned short patternIdx;
   int tileX = ppuAddr&0x001F;
   int tileY = (ppuAddr&0x03E0)>>5;
   int nameAddr = 0x2000 + (ppuAddr&0x0FFF);
   int attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
   int bkgndPatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;
   BackgroundBufferData* pBkgnd = m_bkgndBuffer.data + 1;

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

   C6502::EMULATE ( true, m_curCycles/3 );
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
}

void CPPU::GATHERSPRITES ( int scanline )
{
   int idx1, idx2;
   int sprite;
   unsigned short spritePatBase = 0x0000;
   short         spriteY;
   unsigned char patternIdx;
   unsigned char spriteAttr;
   int           spriteSize;
   SpriteBufferData* pSprite;

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

         C6502::EMULATE ( true, m_curCycles/3 );
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

      C6502::EMULATE ( true, m_curCycles/3 );
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
}
