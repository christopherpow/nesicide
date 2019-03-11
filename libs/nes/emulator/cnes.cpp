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
#include "cnesios.h"
#include "cnesio.h"
#include "cnesapu.h"

int32_t  CNES::m_videoMode = MODE_NTSC;
int32_t  CNES::m_controllerType [] = { IO_StandardJoypad, IO_Zapper };
int32_t  CNES::m_controllerPositionX [] = { 0, };
int32_t  CNES::m_controllerPositionY [] = { 0, };
int32_t  CNES::m_windowX1 = 0;
int32_t  CNES::m_windowY1 = 0;
int32_t  CNES::m_windowX2 = 0;
int32_t  CNES::m_windowY2 = 0;

bool     CNES::m_bReplay = false;
bool     CNES::m_bRecord = true;
uint32_t CNES::m_frame = 0;

CTracer*         CNES::m_tracer = NULL;

CBreakpointInfo* CNES::m_breakpoints;
bool            CNES::m_bBreakpointsEnabled = true;
bool            CNES::m_bAtBreakpoint = false;
bool            CNES::m_bStepCPUBreakpoint = false;
bool            CNES::m_bStepPPUBreakpoint = false;
int32_t         CNES::m_ppuCycleToStepTo = -1;
uint32_t        CNES::m_ppuFrameToStepTo = -1;

static CNES __init __attribute__((unused));

CNES::CNES()
{
   m_breakpoints = new CNESBreakpointInfo();

   m_tracer = new CTracer();
}

CNES::~CNES()
{
   delete m_breakpoints;

   delete m_tracer;
}

uint8_t CNES::_MEM ( uint32_t addr )
{
   if ( addr < 0x800 )
   {
      return C6502::_MEM ( addr );
   }
   else if ( addr < 0x5C00 )
   {
      return C6502::OPENBUS();
   }
   else if ( addr < 0x6000 )
   {
      return CROM::EXRAM ( addr );
   }
   else if ( addr < 0x8000 )
   {
      return CROM::SRAMVIRT ( addr );
   }
   else
   {
      return CROM::PRGROM ( addr );
   }
}

char* CNES::DISASSEMBLY ( uint32_t addr )
{
   if ( addr < 0x800 )
   {
      return C6502::DISASSEMBLY ( addr );
   }
   else if ( addr < 0x5C00 )
   {
      return "N/A";
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

void CNES::PRINTABLEADDR ( char* buffer, uint32_t addr )
{
   if ( addr < 0x5C00 )
   {
      sprintf ( buffer, "%04X", addr );
   }
   else if ( addr < 0x6000 )
   {
      sprintf ( buffer, "%04X(%04X)", addr&MASK_1KB, addr );
   }
   else if ( addr < 0x8000 )
   {
      sprintf ( buffer, "%02X:%04X(%04X)", SRAMBANK_ABSBANK(CROM::SRAMABSADDR(addr)), SRAMBANK_OFF(addr), addr );
   }
   else
   {
      sprintf ( buffer, "%02X:%04X(%04X)", PRGBANK_ABSBANK(CROM::PRGROMABSADDR(addr)), PRGBANK_OFF(addr), addr );
   }
}

void CNES::PRINTABLEADDR ( char* buffer, uint32_t addr, uint32_t absAddr )
{
   if ( addr < 0x5C00 )
   {
      sprintf ( buffer, "%04X", addr );
   }
   else if ( addr < 0x6000 )
   {
      sprintf ( buffer, "%04X(%04X)", addr&MASK_1KB, addr );
   }
   else if ( addr < 0x8000 )
   {
      if ( absAddr != 0xFFFFFFFF )
      {
         sprintf ( buffer, "%02X:%04X(%04X)", SRAMBANK_ABSBANK(absAddr), SRAMBANK_OFF(addr), addr );
      }
      else
      {
         sprintf ( buffer, "??:%04X(%04X)", SRAMBANK_OFF(addr), addr );
      }
   }
   else
   {
      if ( absAddr != 0xFFFFFFFF )
      {
         sprintf ( buffer, "%02X:%04X(%04X)", PRGBANK_ABSBANK(absAddr), PRGBANK_OFF(addr), addr );
      }
      else
      {
         sprintf ( buffer, "??:%04X(%04X)", PRGBANK_OFF(addr), addr );
      }
   }
}

uint32_t CNES::SLOC2ADDR ( uint16_t sloc )
{
   if ( C6502::__PC() < 0x800 )
   {
      return C6502::SLOC2ADDR ( sloc );
   }
   else if ( C6502::__PC() < 0x5C00 )
   {
      return 0;
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

uint16_t CNES::ADDR2SLOC ( uint32_t addr )
{
   if ( addr < 0x800 )
   {
      return C6502::ADDR2SLOC ( addr );
   }
   else if ( addr < 0x5C00 )
   {
      return 0;
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

uint32_t CNES::SLOC ( uint32_t addr )
{
   if ( addr < 0x800 )
   {
      return C6502::SLOC ();
   }
   else if ( addr < 0x5C00 )
   {
      return 0;
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

uint32_t CNES::ABSADDR ( uint32_t addr )
{
   if ( addr < 0x800 )
   {
      return addr;
   }
   else if ( addr < 0x5C00 )
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
      return CROM::PRGROMABSADDR ( addr );
   }
}

void CNES::RESET ( uint32_t mapper, bool soft )
{
   if ( nesIsDebuggable() )
   {
      // Clear execution tracer sample buffer...
      m_tracer->ClearSampleBuffer ();

      // Zero visualizer markers...
      C6502::MARKERS()->ZeroAllMarkers();
   }

   // Reset mapper and set up quick access pointer to mapper function table.
   MAPPERFUNC = &(_mapperfunc[mapper]);
   MAPPERFUNC->reset ( soft );

   // Reset emulated PPU...
   CPPU::RESET ( soft );

   // Reset emulated 6502 and APU [APU reset internal to 6502]...
   // The APU reset will trigger the SDL callback by initializing SDL;
   // The SDL callback triggers emulation...
   C6502::RESET ( soft );

   m_frame = 0;
}

void CNES::STEPCPUBREAKPOINT ( void )
{
   m_bStepCPUBreakpoint = true;
}

void CNES::STEPPPUBREAKPOINT ( bool goFrame )
{
   m_bStepPPUBreakpoint = true;
   if ( goFrame )
   {
      m_ppuFrameToStepTo = CPPU::_FRAME()+1;
      m_ppuCycleToStepTo = CPPU::_CYCLES();
   }
   else
   {
      m_ppuFrameToStepTo = -1;
      m_ppuCycleToStepTo = -1;
   }
}

void CNES::CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type, int32_t data, int32_t event )
{
   int32_t idx;
   BreakpointInfo* pBreakpoint;
   CRegisterData* pRegister;
   CBitfieldData* pBitfield;
   int32_t addr = 0;
   int32_t absAddr = 0;
   int32_t value = 0;
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
             (type == eBreakOnPPUCycle) &&
             ((m_ppuCycleToStepTo == -1) ||
             ((m_ppuCycleToStepTo == CPPU::_CYCLES()) &&
             (m_ppuFrameToStepTo == CPPU::_FRAME()))) )
   {
      m_bStepPPUBreakpoint = false;
      force = true;
   }
   // For all breakpoints...if we're not stepping...
   else
   {
      for ( idx = 0; idx < m_breakpoints->GetNumBreakpoints(); idx++ )
      {
         // Get breakpoint data...
         pBreakpoint = m_breakpoints->GetBreakpoint(idx);

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
                        addr = C6502::__PCSYNC();
                        absAddr = CNES::ABSADDR(C6502::__PCSYNC());

                        if ( pBreakpoint->item1 == pBreakpoint->item2 )
                        {
                           if ( ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                                (addr >= pBreakpoint->item1) &&
                                (addr <= pBreakpoint->item2) &&
                                (((!pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask)) ||
                                 ((pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask) && ((addr&(~pBreakpoint->itemMask)) == 0))) )
                           {
                              pBreakpoint->itemActual = addr;
                              pBreakpoint->hit = true;
                              force = true;
                           }
                        }
                        else
                        {
                           if ( (addr >= pBreakpoint->item1) &&
                                (addr <= pBreakpoint->item2) &&
                                (((!pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask)) ||
                                 ((pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask) && ((addr&(~pBreakpoint->itemMask)) == 0))) )
                           {
                              pBreakpoint->itemActual = addr;
                              pBreakpoint->hit = true;
                              force = true;
                           }
                        }
                        break;
                     case eBreakOnCPUMemoryAccess:
                     case eBreakOnCPUMemoryRead:
                     case eBreakOnCPUMemoryWrite:
                        addr = C6502::_EA();

                        if ( (addr >= pBreakpoint->item1) &&
                             (addr <= pBreakpoint->item2) &&
                             (((!pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask)) ||
                              ((pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask) && ((addr&(~pBreakpoint->itemMask)) == 0))) )
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
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (data&pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (data&pBreakpoint->data) &&
                                     ((data&(~pBreakpoint->data)) == 0) )
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
                           pRegister = C6502::REGISTERS()->GetRegister(pBreakpoint->item1);
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           switch ( pBreakpoint->item1 )
                           {
                              case CPU_PC:
                                 value = C6502::__PC();
                                 break;
                              case CPU_A:
                                 value = C6502::_A();
                                 break;
                              case CPU_X:
                                 value = C6502::_X();
                                 break;
                              case CPU_Y:
                                 value = C6502::_Y();
                                 break;
                              case CPU_SP:
                                 value = C6502::_SP();
                                 break;
                              case CPU_F:
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
                                     (pBitfield->GetValueRaw(value) < pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                     (pBitfield->GetValueRaw(value) > pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) &&
                                     ((pBitfield->GetValueRaw(value)&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) )
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
                             (addr <= pBreakpoint->item2) &&
                             (((!pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask)) ||
                              ((pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask) && ((addr&(~pBreakpoint->itemMask)) == 0))) )
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
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (data&pBreakpoint->data) &&
                                     ((data&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (data&pBreakpoint->data) )
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
                             (addr <= pBreakpoint->item2) &&
                             (((!pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask)) ||
                              ((pBreakpoint->itemMaskExclusive) && (addr&pBreakpoint->itemMask) && ((addr&(~pBreakpoint->itemMask)) == 0))) )
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
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (data&pBreakpoint->data) &&
                                     ((data&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (data&pBreakpoint->data) )
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
                           pRegister = CPPU::REGISTERS()->GetRegister(pBreakpoint->item1);
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
                                     (pBitfield->GetValueRaw(value) < pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                     (pBitfield->GetValueRaw(value) > pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) &&
                                     ((pBitfield->GetValueRaw(value)&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) )
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
                           pRegister = CAPU::REGISTERS()->GetRegister(pBreakpoint->item1);
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
                                     (pBitfield->GetValueRaw(value) < pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                     (pBitfield->GetValueRaw(value) > pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) &&
                                     ((pBitfield->GetValueRaw(value)&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) )
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
                           pRegister = CROM::REGISTERS()->GetRegister(pBreakpoint->item1);
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           if ( pRegister->GetAddr() >= MEM_32KB )
                           {
                              value = MAPPERFUNC->debuginfo(pRegister->GetAddr());
                           }
                           else
                           {
                              value = MAPPERFUNC->debuginfo(pRegister->GetAddr());
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
                                     (pBitfield->GetValueRaw(value) < pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfGreaterThan) &&
                                     (pBitfield->GetValueRaw(value) > pBreakpoint->data) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfExclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) &&
                                     ((pBitfield->GetValueRaw(value)&(~pBreakpoint->data)) == 0) )
                           {
                              pBreakpoint->hit = true;
                              force = true;
                           }
                           else if ( (pBreakpoint->condition == eBreakIfInclusiveMask) &&
                                     (pBitfield->GetValueRaw(value)&pBreakpoint->data) )
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
   if ( m_bBreakpointsEnabled )
   {
      m_bAtBreakpoint = true;

      // Hook back to IDE to force it to update...
      nesBreak();
   }
}

void CNES::RUN ( uint32_t* joy )
{
   uint32_t  ljoy [ NUM_CONTROLLERS ];
   JoypadLoggerInfo* pSample;

   if ( m_bReplay )
   {
      if ( m_frame >= CIOStandardJoypad::LOGGER(0)->GetNumSamples() )
      {
         m_bReplay = false;
      }
   }

   // Copy joy data locally for replay edits...
   *(ljoy+CONTROLLER1) = *(joy+CONTROLLER1);
   *(ljoy+CONTROLLER2) = *(joy+CONTROLLER2);

   if ( m_bRecord )
   {
      CIOStandardJoypad::LOGGER(0)->AddSample ( C6502::_CYCLES(), *(ljoy+CONTROLLER1) );
      CIOStandardJoypad::LOGGER(1)->AddSample ( C6502::_CYCLES(), *(ljoy+CONTROLLER2) );
   }

   if ( CONTROLLER(0) == IO_StandardJoypad )
   {
      if ( m_bReplay )
      {
         pSample = CIOStandardJoypad::LOGGER(0)->GetSample ( m_frame );
         *(ljoy+CONTROLLER1) |= (pSample->data);
      }
      CIOStandardJoypad::JOY ( CONTROLLER1, *(ljoy+CONTROLLER1) );
   }
   else if ( CONTROLLER(0) == IO_TurboJoypad )
   {
      if ( m_bReplay )
      {
         pSample = CIOTurboJoypad::LOGGER(0)->GetSample ( m_frame );
         *(ljoy+CONTROLLER1) |= (pSample->data);
      }
      CIOTurboJoypad::JOY ( CONTROLLER1, *(ljoy+CONTROLLER1) );
   }
   else if ( CONTROLLER(0) == IO_Zapper )
   {
      CIOZapper::JOY ( CONTROLLER1, *(ljoy+CONTROLLER1) );
   }
   else if ( CONTROLLER(0) == IO_Vaus )
   {
      CIOVaus::JOY ( CONTROLLER1, *(ljoy+CONTROLLER1) );
   }
   else
   {
      CIOStandardJoypad::JOY ( CONTROLLER1, 0x00 );
   }

   if ( CONTROLLER(1) == IO_StandardJoypad )
   {
      if ( m_bReplay )
      {
         pSample = CIOStandardJoypad::LOGGER(1)->GetSample ( m_frame );
         *(ljoy+CONTROLLER2) |= (pSample->data);
      }
      CIOStandardJoypad::JOY ( CONTROLLER2, *(ljoy+CONTROLLER2) );
   }
   else if ( CONTROLLER(1) == IO_TurboJoypad )
   {
      if ( m_bReplay )
      {
         pSample = CIOTurboJoypad::LOGGER(1)->GetSample ( m_frame );
         *(ljoy+CONTROLLER2) |= (pSample->data);
      }
      CIOTurboJoypad::JOY ( CONTROLLER2, *(ljoy+CONTROLLER2) );
   }
   else if ( CONTROLLER(1) == IO_Zapper )
   {
      CIOZapper::JOY ( CONTROLLER2, *(ljoy+CONTROLLER2) );
   }
   else if ( CONTROLLER(1) == IO_Vaus )
   {
      CIOVaus::JOY ( CONTROLLER2, *(ljoy+CONTROLLER2) );
   }
   else
   {
      CIOStandardJoypad::JOY ( CONTROLLER2, 0x00 );
   }

   // PPU cycles repeat...
   CPPU::RESETCYCLECOUNTER ();

   m_frame = CPPU::_FRAME();

   if ( nesIsDebuggable() )
   {
      m_tracer->SetFrame ( m_frame );

      // Emit start-of-frame indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_StartPPUFrame, eNESSource_PPU, 0, 0, 0 );
   }

   // Do scanline processing for scanlines 0 - 239 (the screen!)...
   CPPU::RENDERSCANLINE ( SCANLINES_VISIBLE );

#if 0

// CPTODO: move this to ide...
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

#endif

   if ( nesIsDebuggable() )
   {
      // Emit start-of-quiet scanline indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_QuietStart, eNESSource_PPU, 0, 0, 0 );
   }

   // Emulate PPU resting scanlines...
   CPPU::QUIETSCANLINES ();

   if ( nesIsDebuggable() )
   {
      // Emit end-of-quiet scanline indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_QuietEnd, eNESSource_PPU, 0, 0, 0 );

      // Do VBLANK processing (scanlines 0-19 NTSC or 0-69 PAL)...
      // Emit start-VBLANK indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_VBLANKStart, eNESSource_PPU, 0, 0, 0 );
   }

   // Emulate VBLANK non-render scanlines...
   CPPU::VBLANKSCANLINES ();

   if ( nesIsDebuggable() )
   {
      // Emit end-VBLANK indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_VBLANKEnd, eNESSource_PPU, 0, 0, 0 );

      // Emit start-of-prerender scanline indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_PreRenderStart, eNESSource_PPU, 0, 0, 0 );
   }

   // Pre-render scanline...
   CPPU::RENDERSCANLINE ( -1 );

   if ( nesIsDebuggable() )
   {
      // Emit end-of-prerender scanline indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_PreRenderEnd, eNESSource_PPU, 0, 0, 0 );

      // Emit end-of-frame indication to Tracer...
      m_tracer->AddSample ( CPPU::_CYCLES(), eTracer_EndPPUFrame, eNESSource_PPU, 0, 0, 0 );
   }
}
