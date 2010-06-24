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

#include "cnes.h"

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesio.h"
#include "cnesapu.h"

#include <QSemaphore>
#include <QMessageBox>

int          CNES::m_videoMode = MODE_NTSC;

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

void CNES::BREAKPOINTS ( bool enable )
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int bp;

   // Disable all breakpoints (Cartridge is being 'removed'), or
   // re-enable all previously enabled breakpoints (Cartridge is
   // being 'inserted').
   for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      if ( enable )
      {
         pBreakpoints->WasEnabled(bp);
      }
      else
      {
         pBreakpoints->SetEnabled(bp,enable);
      }
   }

   // Remove any breakpoints that were set for a mapper that is
   // not equivalent to the current loaded Cartridge's mapper.
   if ( enable )
   {
      for ( bp = pBreakpoints->GetNumBreakpoints()-1; bp >= 0; bp-- )
      {
// CPTODO: HANDLE THIS CASE, otherwise mapper-state or mapper-event breakpoints
// turn into garbled mush when a cartridge with a different mapper (or no mapper)
// is loaded after the one that was running when the breakpoint was created.
      }
   }
}

void CNES::CLEAROPCODEMASKS ( void )
{
   // Clear code/data logger info...
   C6502::OPCODEMASKCLR ();
   CROM::PRGROMOPCODEMASKCLR ();
   CROM::SRAMOPCODEMASKCLR ();
   CROM::EXRAMOPCODEMASKCLR ();
}

void CNES::RESET ( UINT mapper )
{
   // Reset mapper...this sets up the CROM object with the
   // correct mapper information so the appropriate mapper
   // functions are called.
   mapperfunc [ mapper ].reset ();

   // Clear execution tracer sample buffer...
   m_tracer.ClearSampleBuffer ();

   // Reset emulated PPU...
   CPPU::RESET ();

   // Reset emulated 6502 and APU [APU reset internal to 6502]...
   // The APU reset will trigger the SDL callback by initializing SDL;
   // The SDL callback triggers emulation...
   C6502::RESET ();

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
//      CIO::LOGGER(0)->AddSample ( C6502::_CYCLES(), *(ljoy+JOY1) );
//   }

//   if ( !m_bReplay )
//   {
//      CIO::LOGGER(1)->AddSample ( C6502::_CYCLES(), *(ljoy+JOY2) );
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

   // Emit start-of-frame indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_StartPPUFrame, eSource_PPU, 0, 0, 0 );

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

   // Emit start-of-quiet scanline indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_QuietStart, eSource_PPU, 0, 0, 0 );

   // Emulate PPU resting scanline...
   CPPU::QUIETSCANLINE ();

   // Emit end-of-quiet scanline indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_QuietEnd, eSource_PPU, 0, 0, 0 );

   // Do VBLANK processing (scanlines 0-19 NTSC or 0-69 PAL)...
   // Emit start-VBLANK indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_VBLANKStart, eSource_PPU, 0, 0, 0 );

   // Emulate VBLANK non-render scanlines...
   CPPU::VBLANKSCANLINES ();

   // Emit end-VBLANK indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_VBLANKEnd, eSource_PPU, 0, 0, 0 );

   // Emit start-of-prerender scanline indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_PreRenderStart, eSource_PPU, 0, 0, 0 );

   // Pre-render scanline...
   CPPU::RENDERSCANLINE ( -1 );

   // Emit end-of-prerender scanline indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_PreRenderEnd, eSource_PPU, 0, 0, 0 );

   // Emit end-of-frame indication to Tracer...
   m_tracer.AddSample ( CPPU::_CYCLES(), eTracer_EndPPUFrame, eSource_PPU, 0, 0, 0 );

   // Update NameTable inspector...
   CPPU::RENDERNAMETABLE ();

   // Increment PPU frame counter...
   m_frame++;
   m_tracer.SetFrame ( m_frame );
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
   // For all breakpoints...if we're not stepping...
   else
   {
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
                     case eBreakOnPPUCycle:
                        // Nothing to do here; make the warning go away...
                     break;
                     case eBreakOnCPUExecution:
                        addr = C6502::__PC();
                        if ( (C6502::_SYNC()) &&
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

void CNES::CODEBROWSERTOOLTIP ( int tipType, UINT addr, char* tooltipBuffer )
{
   char* ptr = tooltipBuffer;
   ptr += sprintf ( ptr, "<pre>" );
   if ( tipType == TOOLTIP_BYTES )
   {
      if ( addr < 0x800 )
      {
         ptr += sprintf ( ptr, "6502 @ $%X<br>RAM  @ $%X", addr, addr );
      }
      else if ( addr < 0x6000 )
      {
         ptr += sprintf ( ptr, "6502  @ $%X<br>EXRAM @ $%X", addr, CROM::EXRAMABSADDR(addr) );
      }
      else if ( addr < 0x8000 )
      {
         ptr += sprintf ( ptr, "6502 @ $%X<br>SRAM @ $%X", addr, CROM::SRAMABSADDR(addr) );
      }
      else
      {
         ptr += sprintf ( ptr, "6502    @ $%X<br>PRG-ROM @ $%X", addr, CROM::ABSADDR(addr) );
      }
   }
   else if ( tipType == TOOLTIP_INFO )
   {
      ptr += sprintf ( ptr, OPCODEINFO(C6502::_MEM(addr)) );
   }
   ptr += sprintf ( ptr, "</pre>" );
}

UINT CNES::ABSADDR ( UINT addr )
{
   if ( addr < 0x800 )
   {
      return addr;
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAMABSADDR ( addr );
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAMABSADDR ( addr );
   }
   else
   {
      return CROM::ABSADDR ( addr );
   }
}
