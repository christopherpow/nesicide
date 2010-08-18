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

#include "cnessystempalette.h"

#include "dbg_cnesppu.h"

#include "ccodedatalogger.h"

#include <QColor>

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
bool ppuAlwaysFireEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuRasterPositionEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (pBreakpoint->item1 == CPPUDBG::_X()) &&
        (pBreakpoint->item2 == CPPUDBG::_Y()) )
   {
      return true;
   }
   return false;
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

bool ppuAddressEqualsEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }
   return false;
}

static CBreakpointEventInfo* tblPPUEvents [] =
{
   new CBreakpointEventInfo("Raster Position", ppuRasterPositionEvent, 2, "Break at pixel (%d,%d)", 10, "X:", "Y:"),
   new CBreakpointEventInfo("PPU Address Equals", ppuAddressEqualsEvent, 1, "Break if PPU address is $%04X", 16, "PPU Address:"),
   new CBreakpointEventInfo("Pre-render Scanline Start (X=0,Y=-1)", ppuAlwaysFireEvent, 0, "Break at start of pre-render scanline", 10),
   new CBreakpointEventInfo("Pre-render Scanline End (X=256,Y=-1)", ppuAlwaysFireEvent, 0, "Break at end of pre-render scanline", 10),
   new CBreakpointEventInfo("Scanline Start (X=0,Y=[0,239])", ppuAlwaysFireEvent, 0, "Break at start of scanline", 10),
   new CBreakpointEventInfo("Scanline End (X=256,Y=[0,239])", ppuAlwaysFireEvent, 0, "Break at end of scanline", 10),
   new CBreakpointEventInfo("X Scroll Updated", ppuAlwaysFireEvent, 0, "Break if PPU's X scroll is updated", 10),
   new CBreakpointEventInfo("Y Scroll Updated", ppuAlwaysFireEvent, 0, "Break if PPU's Y scroll is updated", 10),
   new CBreakpointEventInfo("Sprite DMA", ppuAlwaysFireEvent, 0, "Break on sprite DMA", 10),
   new CBreakpointEventInfo("Sprite 0 Hit", ppuAlwaysFireEvent, 0, "Break on sprite 0 hit", 10),
   new CBreakpointEventInfo("Sprite enters multiplexer", ppuSpriteInMultiplexerEvent, 1, "Break if sprite %d enters multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite selected by multiplexer", ppuSpriteSelectedEvent, 1, "Break if sprite %d is selected by multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite rendering", ppuSpriteRenderingEvent, 1, "Break if rendering sprite %d on scanline", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite overflow", ppuAlwaysFireEvent, 0, "Break on sprite-per-scanline overflow", 10)
};

int8_t*          CPPUDBG::m_pCodeDataLoggerInspectorTV = NULL;

int8_t*          CPPUDBG::m_pCHRMEMInspectorTV = NULL;
bool           CPPUDBG::m_bCHRMEMInspector = false;
int8_t*          CPPUDBG::m_pOAMInspectorTV = NULL;
bool           CPPUDBG::m_bOAMInspector = false;
int8_t*          CPPUDBG::m_pNameTableInspectorTV = NULL;
bool           CPPUDBG::m_bNameTableInspector = false;

uint8_t  CPPUDBG::m_last2005x = 0;
uint8_t  CPPUDBG::m_last2005y = 0;
uint16_t CPPUDBG::m_2005x [256][240] = { {0,}, };
uint16_t CPPUDBG::m_2005y [256][240] = { {0,}, };
uint8_t  CPPUDBG::m_lastSprite0HitX = 0xFF;
uint8_t  CPPUDBG::m_lastSprite0HitY = 0xFF;
uint8_t  CPPUDBG::m_x = 0xFF;
uint8_t  CPPUDBG::m_y = 0xFF;

uint32_t CPPUDBG::m_iPPUViewerScanline = 0;
uint32_t CPPUDBG::m_iOAMViewerScanline = 0;
bool CPPUDBG::m_bPPUViewerShowVisible = true;
bool CPPUDBG::m_bOAMViewerShowVisible = false;

CRegisterData** CPPUDBG::m_tblRegisters = tblPPURegisters;
int             CPPUDBG::m_numRegisters = NUM_PPU_REGISTERS;

CBreakpointEventInfo** CPPUDBG::m_tblBreakpointEvents = tblPPUEvents;
int                    CPPUDBG::m_numBreakpointEvents = NUM_PPU_EVENTS;

CPPUDBG::CPPUDBG()
{
}

CPPUDBG::~CPPUDBG()
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

void CPPUDBG::RENDERCODEDATALOGGER ( void )
{
   uint32_t idxx;
   uint32_t cycleDiff;
   uint32_t curCycle = CCodeDataLogger::GetCurCycle ();
   QColor lcolor;
   CCodeDataLogger* pLogger;
   LoggerInfo* pLogEntry;
   int8_t* pTV;

   // Clearly...
   memset ( m_pCodeDataLoggerInspectorTV, 255, 49152 );

   pTV = (int8_t*)m_pCodeDataLoggerInspectorTV;

   // Show PPU memory...
   pLogger = nesGetPpuCodeDataLoggerDatabase();
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
            lcolor = color[(int)pLogEntry->type];
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
         *pTV = lcolor.red();
         *(pTV+1) = lcolor.green();
         *(pTV+2) = lcolor.blue();

         pTV += 3;
      }
   }
}

void CPPUDBG::RENDERCHRMEM ( void )
{
   uint32_t ppuAddr = 0x0000;
   uint8_t patternData1;
   uint8_t patternData2;
   uint8_t bit1, bit2;
   uint8_t colorIdx;
   int32_t color[4][3];
   int8_t* pTV;

   if ( m_bCHRMEMInspector )
   {
      pTV = (int8_t*)m_pCHRMEMInspectorTV;

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
              patternData1 = CPPUDBG::_MEM(ppuAddr);
              patternData2 = CPPUDBG::_MEM(ppuAddr+8);

              for ( int32_t xf = 0; xf < 8; xf++ )
              {
                 bit1 = (patternData1>>(7-(xf)))&0x1;
                 bit2 = (patternData2>>(7-(xf)))&0x1;
                 colorIdx = (bit1|(bit2<<1));
                 *pTV = color[colorIdx][0];
                 *(pTV+1) = color[colorIdx][1];
                 *(pTV+2) = color[colorIdx][2];

                 pTV += 3;
             }
          }
      }
   }
}

void CPPUDBG::RENDEROAM ( void )
{
   int32_t x, xf, y, yf;
   uint16_t spritePatBase = 0x0000;
   uint8_t patternIdx;
   uint8_t spriteAttr;
   int32_t           spriteSize;
   int32_t           sprite;
   uint8_t spriteFlipVert;
   uint8_t spriteFlipHoriz;
   uint8_t patternData1;
   uint8_t patternData2;
   uint8_t attribData;
   uint8_t bit1, bit2;
   uint8_t colorIdx;
   uint8_t spriteY;
   QColor color[4];
   int8_t* pTV;

   if ( m_bOAMInspector )
   {
      pTV = (int8_t*)m_pOAMInspectorTV;

      color[0] = CBasePalette::GetPalette ( 0x0D );
      color[1] = CBasePalette::GetPalette ( 0x10 );
      color[2] = CBasePalette::GetPalette ( 0x20 );
      color[3] = CBasePalette::GetPalette ( 0x30 );

      spriteSize = ((!!(CPPUDBG::_PPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;
      if ( spriteSize == 8 )
      {
         spritePatBase = (!!(CPPUDBG::_PPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
      }

      for ( y = 0; y < spriteSize<<1; y++ )
      {
         for ( x = 0; x < 256; x += PATTERN_SIZE ) // pattern-slice rendering...
         {
            sprite = (spriteSize==8)?((y>>3)<<5)+(x>>3):
                                     ((y>>4)<<5)+(x>>3);
            spriteY = CPPUDBG::_OAM ( SPRITEY, sprite );
            if ( ((m_bOAMViewerShowVisible) && ((spriteY+1) < SPRITE_YMAX)) ||
                 (!m_bOAMViewerShowVisible) )
            {
               patternIdx = CPPUDBG::_OAM ( SPRITEPAT, sprite );
               if ( spriteSize == 16 )
               {
                  spritePatBase = (patternIdx&0x01)<<12;
                  patternIdx &= 0xFE;
               }

               spriteAttr = CPPUDBG::_OAM ( SPRITEATT, sprite );
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

               patternData1 = CPPUDBG::_MEM ( spritePatBase+(patternIdx<<4)+(yf) );
               patternData2 = CPPUDBG::_MEM ( spritePatBase+(patternIdx<<4)+(yf)+PATTERN_SIZE );

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
                  *pTV = CBasePalette::GetPaletteR(CPPUDBG::_PALETTE(0x10+colorIdx));
                  *(pTV+1) = CBasePalette::GetPaletteG(CPPUDBG::_PALETTE(0x10+colorIdx));
                  *(pTV+2) = CBasePalette::GetPaletteB(CPPUDBG::_PALETTE(0x10+colorIdx));

                  pTV += 3;
               }
            }
            else
            {
               for ( xf = 0; xf < PATTERN_SIZE; xf++ )
               {
                  *pTV = 0x00;
                  *(pTV+1) = 0x00;
                  *(pTV+2) = 0x00;

                  pTV += 3;
               }
            }
         }
      }
   }
}

void CPPUDBG::RENDERNAMETABLE ( void )
{
   int32_t x, xf, y;
   int32_t lbx, ubx, lby, uby;

   uint32_t ppuAddr = 0x0000;
   uint16_t patternIdx;
   int32_t tileX;
   int32_t tileY;
   int32_t nameAddr;
   int32_t attribAddr;
   int32_t bkgndPatBase;
   uint8_t attribData;
   uint8_t patternData1;
   uint8_t patternData2;
   uint8_t bit1, bit2;
   uint8_t colorIdx;
   int8_t* pTV;

   if ( m_bNameTableInspector )
   {
      pTV = (int8_t*)m_pNameTableInspectorTV;

      for ( y = 0; y < 480; y++ )
      {
         for ( x = 0; x < 512; x += PATTERN_SIZE ) // pattern-slice rendering...
         {
            tileX = ppuAddr&0x001F;
            tileY = (ppuAddr&0x03E0)>>5;
            nameAddr = 0x2000 + (ppuAddr&0x0FFF);
            attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
            bkgndPatBase = (!!(CPPUDBG::_PPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;

            patternIdx = bkgndPatBase+(CPPUDBG::_NAMETABLE(nameAddr)<<4)+((ppuAddr&0x7000)>>12);
            attribData = CPPUDBG::_ATTRTABLE ( attribAddr );
            patternData1 = CPPUDBG::_PATTERNDATA ( patternIdx );
            patternData2 = CPPUDBG::_PATTERNDATA ( patternIdx+PATTERN_SIZE );

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
               *pTV = CBasePalette::GetPaletteR(CPPUDBG::_PALETTE(colorIdx));
               *(pTV+1) = CBasePalette::GetPaletteG(CPPUDBG::_PALETTE(colorIdx));
               *(pTV+2) = CBasePalette::GetPaletteB(CPPUDBG::_PALETTE(colorIdx));

               pTV += 3;
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

      if ( m_bPPUViewerShowVisible )
      {
         pTV = (int8_t*)m_pNameTableInspectorTV;

         for ( y = 0; y < 480; y++ )
         {
            for ( x = 0; x < 512; x++ )
            {
               lbx = CPPUDBG::_SCROLLX(x&0xFF,y%240);
               ubx = lbx>>8?lbx&0xFF:lbx+255;
               lby = CPPUDBG::_SCROLLY(x&0xFF,y%240);
               uby = lby/240?lby%240:lby+239;

               if ( !( (((lbx <= ubx) && (x >= lbx) && (x <= ubx)) ||
                    ((lbx > ubx) && (!((x <= lbx) && (x >= ubx))))) &&
                    (((lby <= uby) && (y >= lby) && (y <= uby)) ||
                    ((lby > uby) && (!((y <= lby) && (y >= uby))))) ) )
               {
                  if ( (uint8_t)m_pNameTableInspectorTV [ ((y<<9)*3)+(x*3)+0 ] >= 0x30 )
                  {
                     *pTV -= 0x30;
                  }
                  else
                  {
                     *pTV = 0x00;
                  }
                  if ( (uint8_t)m_pNameTableInspectorTV [ ((y<<9)*3)+(x*3)+1 ] >= 0x30 )
                  {
                     *(pTV+1) -= 0x30;
                  }
                  else
                  {
                     *(pTV+1) = 0x00;
                  }
                  if ( (uint8_t)m_pNameTableInspectorTV [ ((y<<9)*3)+(x*3)+2 ] >= 0x30 )
                  {
                     *(pTV+2) -= 0x30;
                  }
                  else
                  {
                     *(pTV+2) = 0x00;
                  }
               }

               pTV += 3;
            }
         }
      }
   }
}
