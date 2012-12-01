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

#include "nes_emulator_core.h"

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

void CROMDBG::RESET ( uint32_t /*mapper*/ )
{
}
