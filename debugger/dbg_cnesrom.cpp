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

#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"

#include "emulator_core.h"

// Mapper Event breakpoints
bool mapperIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* tblMapperEvents [] =
{
   new CBreakpointEventInfo("IRQ", mapperIRQEvent, 0, "Break if mapper asserts IRQ", 10),
};

CRegisterData**  CROMDBG::m_tblRegisters = NULL;
int              CROMDBG::m_numRegisters = 0;

CBreakpointEventInfo** CROMDBG::m_tblBreakpointEvents = tblMapperEvents;
int                    CROMDBG::m_numBreakpointEvents = NUM_MAPPER_EVENTS;

static CROMDBG __init __attribute((unused));

CROMDBG::CROMDBG()
{
   CROMDBG::RESET ();
}

CROMDBG::~CROMDBG()
{
}

void CROMDBG::RESET ( void )
{
   RESET ( 0 );
}

void CROMDBG::RESET ( uint32_t mapper )
{
}
