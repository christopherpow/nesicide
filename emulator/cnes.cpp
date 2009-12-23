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

// NES.cpp: implementation of the CNES class.
//
//////////////////////////////////////////////////////////////////////

#include "cnes.h"

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesio.h"
#include "cnesapu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool         CNES::m_bReplay = false;
unsigned int CNES::m_frame = 0;

CNES::CNES()
{
}

CNES::~CNES()
{

}

void CNES::RESET ( void )
{
   m_frame = 0;
   m_bReplay = false;
}

bool CNES::RUN ( unsigned char* joy )
{
// CPTODO: get controller configuration...
//   UINT* pControllerConfig;
   bool  brkpt = false;
   unsigned int   idx;
   unsigned char ljoy [ NUM_JOY ];
   //JoypadLoggerInfo* pSample;

// CPTODO: removed joypad logging for now...
//   if ( m_bReplay )
//   {
//      if ( m_frame >= CIO::LOGGER(0)->GetNumSamples() )
//      {
//         m_bReplay = false;
//      }
//   }

   // Copy joy data locally for replay edits...
   *(ljoy+JOY1) = *(joy+JOY1);
   *(ljoy+JOY2) = *(joy+JOY2);

// CPTODO: removed replay support for now...
//   if ( !m_bReplay )
//   {
//      CIO::LOGGER(0)->AddSample ( C6502::CYCLES(), *(ljoy+JOY1) );
//   }

//   if ( !m_bReplay )
//   {
//      CIO::LOGGER(1)->AddSample ( C6502::CYCLES(), *(ljoy+JOY2) );
//   }

// CPTODO: part of joypad configuration is whether or not it is connected at all...
   // Feed Joypad inputs in...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY1 );
//   if ( pControllerConfig[idxJOY_CONNECTED] )
//   {
//      if ( m_bReplay )
//      {
//         pSample = CIO::LOGGER(0)->GetSample ( m_frame );
//         *(ljoy+JOY1) |= (pSample->data);
//      }
      CIO::JOY ( JOY1, *(ljoy+JOY1) );
//   }
// CPTODO: this is what to fee into the emu if the joypad is not connected...
//   else
//   {
//      CIO::JOY ( JOY1, 0x02 );
//   }
// CPTODO: part of joypad configuration is whether or not it is connected at all...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY2 );
//   if ( pControllerConfig[idxJOY_CONNECTED] )
//   {
//      if ( m_bReplay )
//      {
//         pSample = CIO::LOGGER(1)->GetSample ( m_frame );
//         *(ljoy+JOY2) |= (pSample->data);
//      }
      CIO::JOY ( JOY2, *(ljoy+JOY2) );
//   }
// CPTODO: this is what to fee into the emu if the joypad is not connected...
//   else
//   {
//      CIO::JOY ( JOY2, 0x02 );
//   }

   // Update NameTable inspector...
   CPPU::RENDERNAMETABLE ();

   // Do VBLANK processing (scanlines 0-19)...
   // Set VBLANK flag...
   CPPU::RESETCYCLECOUNTER ();
   CPPU::_PPU ( PPUSTATUS, CPPU::_PPU(PPUSTATUS)|PPUSTATUS_VBLANK );

   // One-cycle VBLANK delay...
//   CPPU::INCCYCLE ();

   if ( CPPU::_PPU(PPUCTRL)&PPUCTRL_GENERATE_NMI )
   {
      C6502::NMI ( eSource_PPU );
      CPPU::NMI ( eSource_PPU ); // Just for Tracer tag
   }

   // Emulate 20 VBLANK non-render scanlines...
   brkpt = CPPU::NONRENDERSCANLINE ( 20 );
   if ( brkpt ) goto done;

   // Clear VBLANK flag...
   CPPU::_PPU ( PPUSTATUS, CPPU::_PPU(PPUSTATUS)&(~(PPUSTATUS_VBLANK)) );

   // Pre-render scanline...
   brkpt = CPPU::RENDERSCANLINE ( -1 );
   if ( brkpt ) goto done;

   // Clear Sprite 0 Hit flag and sprite overflow...
   CPPU::_PPU ( PPUSTATUS, CPPU::_PPU(PPUSTATUS)&(~(PPUSTATUS_SPRITE_0_HIT|PPUSTATUS_SPRITE_OVFLO)) );

   if ( mapperfunc[CROM::MAPPER()].synch(0) )
   {
      C6502::IRQ( eSource_Mapper );
      CPPU::IRQ ( eSource_Mapper ); // Just for Tracer tag
   }

   // Do scanline processing for scanlines 0 - 239 (the screen!)...
   for ( idx = 0; idx < SCANLINES_VISIBLE; idx++ )
   {
      if ( (idx >= 8) && (idx <= 231) )
      {
         brkpt = CPPU::RENDERSCANLINE ( idx );
         if ( brkpt ) goto done;
      }
      else
      {
         brkpt = CPPU::RENDERSCANLINE ( idx );
         if ( brkpt ) goto done;
         CPPU::RENDERRESET ( idx );
      }

      if ( mapperfunc[CROM::MAPPER()].synch(idx) )
      {
         C6502::IRQ( eSource_Mapper );
         CPPU::IRQ ( eSource_Mapper ); // Just for Tracer tag
      }

      // Update CHR memory inspector at appropriate scanline...
      if ( idx == CPPU::GetPPUViewerScanline() )
      {
         CPPU::RENDERCHRMEM ();
      }

      // Update OAM inspector at appropriate scanline...
      if ( idx == CPPU::GetOAMViewerScanline() )
      {
         CPPU::RENDEROAM ();
      }
   }

   // Emulate PPU resting scanline...
   brkpt = CPPU::NONRENDERSCANLINE ( 1 );
   if ( brkpt ) goto done;

   // Run APU for 1 frame...
   // The RUN method fills a buffer.  The SDL library's callback method
   // is passed this buffer, and that callback method is used to trigger
   // emulation of the next frame.
   CAPU::RUN ();

// CPTODO: this is not an appropriate way to break out of emulation on a
// breakpoint...when we continue we'll act as if the previous frame was complete
// which is usually NOT the case.  need to figure out a way to pick up RIGHT
// where we left off...at the exact PPU/CPU cycle...
done:
   m_frame++;

   return brkpt;
}
