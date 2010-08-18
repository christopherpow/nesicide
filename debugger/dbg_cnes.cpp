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

#include "dbg_cnes.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesrom.h"
#include "dbg_cnesppu.h"
#include "dbg_cnesapu.h"

#include <QSemaphore>

CBreakpointInfo CNESDBG::m_breakpoints;
bool            CNESDBG::m_bAtBreakpoint = false;
bool            CNESDBG::m_bStepCPUBreakpoint = false;
bool            CNESDBG::m_bStepPPUBreakpoint = false;

QSemaphore breakpointSemaphore(0);
extern QSemaphore breakpointWatcherSemaphore;

CNESDBG::CNESDBG()
{
}

CNESDBG::~CNESDBG()
{

}

void CNESDBG::CLEAROPCODEMASKS ( void )
{
   // Clear code/data logger info...
   nesClearOpcodeMasks();
}

void CNESDBG::CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer )
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
         ptr += sprintf ( ptr, "6502  @ $%X<br>EXRAM @ $%X", addr, nesGetAbsoluteAddressFromAddress(addr) );
      }
      else if ( addr < 0x8000 )
      {
         ptr += sprintf ( ptr, "6502 @ $%X<br>SRAM @ $%X", addr, nesGetAbsoluteAddressFromAddress(addr) );
      }
      else
      {
         ptr += sprintf ( ptr, "6502    @ $%X<br>PRG-ROM @ $%X", addr, nesGetAbsoluteAddressFromAddress(addr) );
      }
   }
   else if ( tipType == TOOLTIP_INFO )
   {
      ptr += sprintf ( ptr, "%s", OPCODEINFO(nesGetMemory(addr)) );
   }
   ptr += sprintf ( ptr, "</pre>" );
}

void CNESDBG::BREAKPOINTS ( bool enable )
{
   CBreakpointInfo* pBreakpoints = CNESDBG::BREAKPOINTS();
   int32_t bp;

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

void CNESDBG::CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type, int32_t data, int32_t event )
{
   int32_t idx;
   BreakpointInfo* pBreakpoint;
   CRegisterData* pRegister;
   CBitfieldData* pBitfield;
   int32_t addr = 0;
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
                        addr = C6502DBG::__PC();
                        if ( (C6502DBG::_SYNC()) &&
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
                        addr = C6502DBG::_EA();
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
                           pRegister = C6502DBG::REGISTERS()[pBreakpoint->item1];
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           switch ( pBreakpoint->item1 )
                           {
                              case 0:
                                 value = C6502DBG::__PC();
                              break;
                              case 1:
                                 value = C6502DBG::_A();
                              break;
                              case 2:
                                 value = C6502DBG::_X();
                              break;
                              case 3:
                                 value = C6502DBG::_Y();
                              break;
                              case 4:
                                 value = C6502DBG::_SP();
                              break;
                              case 5:
                                 value = C6502DBG::_F();
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
                        addr = CPPUDBG::_OAMADDR();
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
                        addr = CPPUDBG::_PPUADDR();
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
                           pRegister = CPPUDBG::REGISTERS()[pBreakpoint->item1];
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           value = CPPUDBG::_PPU(pRegister->GetAddr());

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
                           pRegister = CAPUDBG::REGISTERS()[pBreakpoint->item1];
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           value = CAPUDBG::_APU(pRegister->GetAddr());

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
                           pRegister = CROMDBG::REGISTERS()[pBreakpoint->item1];
                           pBitfield = pRegister->GetBitfield(pBreakpoint->item2);

                           // Get actual register data...
                           if ( pRegister->GetAddr() >= MEM_32KB )
                           {
                              value = CROMDBG::HIGHREAD(pRegister->GetAddr());
                           }
                           else
                           {
                              value = CROMDBG::LOWREAD(pRegister->GetAddr());
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

void CNESDBG::FORCEBREAKPOINT ( void )
{
   m_bAtBreakpoint = true;
   breakpointWatcherSemaphore.release();
   breakpointSemaphore.acquire();
}
