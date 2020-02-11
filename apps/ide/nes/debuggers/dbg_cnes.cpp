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

CNESDBG::CNESDBG()
{
}

CNESDBG::~CNESDBG()
{
}

void CNESDBG::CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer )
{
   char* ptr = tooltipBuffer;
   ptr += sprintf ( ptr, "<pre>" );

   if ( tipType == TOOLTIP_BYTES )
   {
      if ( addr < 0x800 )
      {
         ptr += sprintf ( ptr, "6502 @ %X<br>RAM  @ %X", addr, addr );
      }
      else if ( addr < 0x6000 )
      {
         ptr += sprintf ( ptr, "6502  @ %X<br>EXRAM @ %X", addr, nesGetPhysicalAddressFromAddress(addr) );
      }
      else if ( addr < 0x8000 )
      {
         ptr += sprintf ( ptr, "6502 @ %X<br>SRAM @ %X", addr, nesGetPhysicalAddressFromAddress(addr) );
      }
      else
      {
         ptr += sprintf ( ptr, "6502    @ %X<br>PRG-ROM @ %X", addr, nesGetPhysicalAddressFromAddress(addr) );
      }
   }
   else if ( tipType == TOOLTIP_INFO )
   {
      ptr += sprintf ( ptr, "%s", OPCODEINFO(nesGetMemory(addr)) );
   }

   ptr += sprintf ( ptr, "</pre>" );
}
