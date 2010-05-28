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

#include <QSemaphore>
#include <QMessageBox>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool         CNES::m_bReplay = false;
unsigned int CNES::m_frame = 0;
CBreakpointInfo CNES::m_breakpoints;
bool            CNES::m_bAtBreakpoint = false;
bool            CNES::m_bStepCPUBreakpoint = false;
bool            CNES::m_bStepPPUBreakpoint = false;

CTracer         CNES::m_tracer;

QSemaphore breakpointSemaphore(0);
extern QSemaphore breakpointWatcherSemaphore;

CNES::CNES()
{
}

CNES::~CNES()
{

}

void CNES::HARDRESET ( void )
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int bp;
   int bpr = 0;
   int bps = pBreakpoints->GetNumBreakpoints();
   for ( bp = bps-1; bp >= 0; bp-- )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(bp);
      if ( pBreakpoint->type == eBreakOnCPUExecution )
      {
         pBreakpoints->RemoveBreakpoint(bp);
         bpr++;
      }
   }
   if ( bpr )
   {
      QMessageBox::information ( 0, "Breakpoints removed", "One or more CPU execution breakpoints have been removed because a new ROM has been loaded." );
   }
}

void CNES::RESET ( UINT mapper )
{
   // Reset mapper...this sets up the CROM object with the
   // correct mapper information so the appropriate mapper
   // functions are called.
   mapperfunc [ mapper ].reset ();

   // Reset emulated PPU...
   CPPU::RESET ();

   // Reset emulated 6502 and APU [APU reset internal to 6502]...
   // The APU reset will trigger the SDL callback by initializing SDL;
   // The SDL callback triggers emulation...
   C6502::RESET ();

   // Clear emulated machine memory and registers...
   C6502::MEMCLR ();
   CPPU::MEMCLR ();
   CPPU::OAMCLR ();
   CROM::CHRRAMCLR ();

   // Clear code/data logger info...
   C6502::OPCODEMASKCLR ();
   CROM::PRGROMOPCODEMASKCLR ();
   CROM::SRAMOPCODEMASKCLR ();
   CROM::EXRAMOPCODEMASKCLR ();

   // Zero visualizer markers...
   C6502::MARKERS().ZeroAllMarkers();

   m_frame = 0;
   m_bReplay = false;
}

void CNES::RUN ( unsigned char* joy )
{
// CPTODO: get controller configuration...
//   UINT* pControllerConfig;
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

   // PPU cycles repeat...
   CPPU::RESETCYCLECOUNTER ();

   // Do VBLANK processing (scanlines 0-19)...
   m_tracer.AddSample ( CPPU::CYCLES(), eTracer_StartPPUFrame, eSource_PPU, 0, 0, 0 );

   // Do scanline processing for scanlines 0 - 239 (the screen!)...
   for ( idx = 0; idx < SCANLINES_VISIBLE; idx++ )
   {
      // Draw pretty pictures...
      CPPU::RENDERSCANLINE ( idx );

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
   CPPU::NONRENDERSCANLINES ( 1 );

   // Set VBLANK flag...
   CPPU::_PPU ( PPUSTATUS, CPPU::_PPU(PPUSTATUS)|PPUSTATUS_VBLANK );

   if ( CPPU::_PPU(PPUCTRL)&PPUCTRL_GENERATE_NMI )
   {
      C6502::NMI ();
   }

   // Emulate VBLANK non-render scanlines...
   if ( CPPU::MODE() == MODE_NTSC )
   {
      CPPU::NONRENDERSCANLINES ( SCANLINES_VBLANK_NTSC );
   }
   else
   {
      CPPU::NONRENDERSCANLINES ( SCANLINES_VBLANK_PAL );
   }

   // Clear VBLANK, Sprite 0 Hit flag and sprite overflow...
   CPPU::_PPU ( PPUSTATUS, CPPU::_PPU(PPUSTATUS)&(~(PPUSTATUS_VBLANK|PPUSTATUS_SPRITE_0_HIT|PPUSTATUS_SPRITE_OVFLO)) );

   // Pre-render scanline...
   CPPU::RENDERSCANLINE ( -1 );

   // Do VBLANK processing (scanlines 0-19)...
   m_tracer.AddSample ( CPPU::CYCLES(), eTracer_EndPPUFrame, eSource_PPU, 0, 0, 0 );

   // Run APU for 1 frame...
   // The RUN method fills a buffer.  The SDL library's callback method
   // is passed this buffer, and that callback method is used to trigger
   // emulation of the next frame.
   CAPU::RUN ();

   // Update NameTable inspector...
   CPPU::RENDERNAMETABLE ();

   // Increment PPU frame counter...
   m_frame++;
}

void CNES::CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type, int data, int event )
{
   int idx;
   BreakpointInfo* pBreakpoint;
   CRegisterData* pRegister;
   CBitfieldData* pBitfield;
   int addr = 0;
   int value = 0;
   bool force = false;

   // If stepping, break...
   if ( (m_bStepCPUBreakpoint) &&
        (target == eBreakInCPU) &&
        (type == eBreakOnCPUExecution) )
   {
      m_bStepCPUBreakpoint = false;
      force = true;
   }
   else if ( (m_bStepPPUBreakpoint) &&
             (target == eBreakInPPU) &&
             (type == eBreakOnPPUCycle) )
   {
      m_bStepPPUBreakpoint = false;
      force = true;
   }

   // For all breakpoints...
   for ( idx = 0; idx < m_breakpoints.GetNumBreakpoints(); idx++ )
   {
      // Get breakpoint data...
      pBreakpoint = m_breakpoints.GetBreakpoint(idx);

      // Not hit yet...
      pBreakpoint->hit = false;

      // Is this breakpoint enabled?
      if ( pBreakpoint->enabled )
      {
         // Are there any for the specified target?
         if ( pBreakpoint->target == target )
         {
            // Promote "Access" types...
            if ( (pBreakpoint->type == eBreakOnCPUMemoryAccess) &&
                 ((type == eBreakOnCPUMemoryRead) || (type == eBreakOnCPUMemoryWrite)) )
            {
               type = eBreakOnCPUMemoryAccess;
            }
            if ( (pBreakpoint->type == eBreakOnOAMPortalAccess) &&
                 ((type == eBreakOnOAMPortalRead) || (type == eBreakOnOAMPortalWrite)) )
            {
               type = eBreakOnOAMPortalAccess;
            }
            if ( (pBreakpoint->type == eBreakOnPPUPortalAccess) &&
                 ((type == eBreakOnPPUPortalRead) || (type == eBreakOnPPUPortalWrite)) )
            {
               type = eBreakOnPPUPortalAccess;
            }
            if ( pBreakpoint->type == type )
            {
               switch ( pBreakpoint->type )
               {
                  case eBreakOnCPUExecution:
                     addr = C6502::__PC();
                     if ( (C6502::SYNC()) &&
                          (addr >= pBreakpoint->item1) &&
                          (addr <= pBreakpoint->item2) )
                     {
                        pBreakpoint->itemActual = addr;
                        pBreakpoint->hit = true;
                        force = true;
                     }
                  break;
                  case eBreakOnCPUMemoryAccess:
                  case eBreakOnCPUMemoryRead:
                  case eBreakOnCPUMemoryWrite:
                     addr = C6502::_EA();
                     if ( (addr >= pBreakpoint->item1) &&
                          (addr <= pBreakpoint->item2) )
                     {
                        pBreakpoint->itemActual = addr;
                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (data == pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (data != pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (data < pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (data > pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnCPUState:
                     // Is the breakpoint on this register?
                     if ( pBreakpoint->item1 == data )
                     {
                        pRegister = C6502::REGISTERS()[pBreakpoint->item1];
                        pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                        // Get actual register data...
                        switch ( pBreakpoint->item1 )
                        {
                           case 0:
                              value = C6502::__PC();
                           break;
                           case 1:
                              value = C6502::_A();
                           break;
                           case 2:
                              value = C6502::_X();
                           break;
                           case 3:
                              value = C6502::_Y();
                           break;
                           case 4:
                              value = C6502::_SP();
                           break;
                           case 5:
                              value = C6502::_F();
                           break;
                        }

                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (pBreakpoint->data == pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (pBreakpoint->data != pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (pBreakpoint->data < pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (pBreakpoint->data > pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnOAMPortalAccess:
                  case eBreakOnOAMPortalRead:
                  case eBreakOnOAMPortalWrite:
                     addr = CPPU::_OAMADDR();
                     if ( (addr >= pBreakpoint->item1) &&
                          (addr <= pBreakpoint->item2) )
                     {
                        pBreakpoint->itemActual = addr;
                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (data == pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (data != pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (data < pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (data > pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnPPUFetch:
                  case eBreakOnPPUPortalAccess:
                  case eBreakOnPPUPortalRead:
                  case eBreakOnPPUPortalWrite:
                     addr = CPPU::_PPUADDR();
                     if ( (addr >= pBreakpoint->item1) &&
                          (addr <= pBreakpoint->item2) )
                     {
                        pBreakpoint->itemActual = addr;
                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (data == pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (data != pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (data < pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (data > pBreakpoint->data) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnPPUState:
                     // Is the breakpoint on this register?
                     if ( pBreakpoint->item1 == data )
                     {
                        pRegister = CPPU::REGISTERS()[pBreakpoint->item1];
                        pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                        // Get actual register data...
                        value = CPPU::_PPU(pRegister->GetAddr());

                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (pBreakpoint->data == pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (pBreakpoint->data != pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (pBreakpoint->data < pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (pBreakpoint->data > pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnAPUState:
                     // Is the breakpoint on this register?
                     if ( pBreakpoint->item1 == data )
                     {
                        pRegister = CAPU::REGISTERS()[pBreakpoint->item1];
                        pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                        // Get actual register data...
                        value = CAPU::_APU(pRegister->GetAddr());

                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (pBreakpoint->data == pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (pBreakpoint->data != pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (pBreakpoint->data < pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (pBreakpoint->data > pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnMapperState:
                     // Is the breakpoint on this register?
                     if ( pBreakpoint->item1 == data )
                     {
                        pRegister = CROM::REGISTERS()[pBreakpoint->item1];
                        pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                        // Get actual register data...
                        if ( pRegister->GetAddr() >= MEM_32KB )
                        {
                           value = mapperfunc[CROM::MAPPER()].highread(pRegister->GetAddr());
                        }
                        else
                        {
                           value = mapperfunc[CROM::MAPPER()].lowread(pRegister->GetAddr());
                        }

                        if ( pBreakpoint->condition == eBreakIfAnything )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfEqual) &&
                                  (pBreakpoint->data == pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfNotEqual) &&
                                  (pBreakpoint->data != pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfLessThan) &&
                                  (pBreakpoint->data < pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                        else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                  (pBreakpoint->data > pBitfield->GetValueRaw(value)) )
                        {
                           pBreakpoint->hit = true;
                           force = true;
                        }
                     }
                  break;
                  case eBreakOnCPUEvent:
                  case eBreakOnPPUEvent:
                  case eBreakOnAPUEvent:
                  case eBreakOnMapperEvent:
                     // If this is the right event to check, check it...
                     if ( pBreakpoint->event == event )
                     {
                        pBreakpoint->hit = pBreakpoint->pEvent->Evaluate(pBreakpoint,data);
                        if ( pBreakpoint->hit )
                        {
                           force = true;
                        }
                     }
                  break;
               }
            }
         }
      }
   }

   if ( force )
   {
      FORCEBREAKPOINT();
   }
}

void CNES::FORCEBREAKPOINT ( void )
{
   m_bAtBreakpoint = true;
   breakpointWatcherSemaphore.release();
   breakpointSemaphore.acquire();
}

char* CNES::DISASSEMBLY ( UINT addr )
{
   if ( addr < 0x800 )
   {
      return C6502::DISASSEMBLY ( addr );
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAMDISASSEMBLY ( addr );
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAMDISASSEMBLY ( addr );
   }
   else
   {
      return CROM::PRGROMDISASSEMBLY ( addr );
   }
}

void CNES::DISASSEMBLE ( void )
{
   C6502::DISASSEMBLE();
   CROM::DISASSEMBLE();
}

UINT CNES::SLOC2ADDR ( unsigned short sloc )
{
   if ( C6502::__PC() < 0x800 )
   {
      return C6502::SLOC2ADDR ( sloc );
   }
   else if ( C6502::__PC() < 0x6000 )
   {
      return CROM::EXRAMSLOC2ADDR ( sloc );
   }
   else if ( C6502::__PC() < 0x8000 )
   {
      return CROM::SRAMSLOC2ADDR ( sloc );
   }
   else
   {
      return CROM::PRGROMSLOC2ADDR ( sloc );
   }
}

unsigned short CNES::ADDR2SLOC ( UINT addr )
{
   if ( addr < 0x800 )
   {
      return C6502::ADDR2SLOC ( addr );
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAMADDR2SLOC ( addr );
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAMADDR2SLOC ( addr );
   }
   else
   {
      return CROM::PRGROMADDR2SLOC ( addr );
   }
}

unsigned int CNES::SLOC ( UINT addr )
{
   if ( addr < 0x800 )
   {
      return C6502::SLOC ();
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAMSLOC ();
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAMSLOC ( addr );
   }
   else
   {
      return CROM::PRGROMSLOC(0x8000)+CROM::PRGROMSLOC(0xA000)+CROM::PRGROMSLOC(0xC000)+CROM::PRGROMSLOC(0xE000);
   }
}

unsigned char CNES::_MEM ( UINT addr )
{
   if ( addr < 0x800 )
   {
      return C6502::_MEM ( addr );
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAM ( addr );
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAM ( addr );
   }
   else
   {
      return CROM::PRGROM ( addr );
   }
}
