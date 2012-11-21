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

#include "cnesrommapper011.h"
#include "cnesppu.h"

uint8_t  CROMMapper011::m_reg = 0x00;

CROMMapper011::CROMMapper011()
{
}

CROMMapper011::~CROMMapper011()
{
}

void CROMMapper011::RESET ( bool soft )
{
   m_mapper = 11;

   CROM::RESET ( m_mapper, soft );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper011::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper011::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}

uint32_t CROMMapper011::MAPPER ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper011::MAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   m_reg = data;

   bank = ((data&0x03)<<2);

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];

   m_pCHRmemory [ 0 ] = m_CHRmemory [ ((data>>1)&0x78)+0 ];
   m_pCHRmemory [ 1 ] = m_CHRmemory [ ((data>>1)&0x78)+1 ];
   m_pCHRmemory [ 2 ] = m_CHRmemory [ ((data>>1)&0x78)+2 ];
   m_pCHRmemory [ 3 ] = m_CHRmemory [ ((data>>1)&0x78)+3 ];
   m_pCHRmemory [ 4 ] = m_CHRmemory [ ((data>>1)&0x78)+4 ];
   m_pCHRmemory [ 5 ] = m_CHRmemory [ ((data>>1)&0x78)+5 ];
   m_pCHRmemory [ 6 ] = m_CHRmemory [ ((data>>1)&0x78)+6 ];
   m_pCHRmemory [ 7 ] = m_CHRmemory [ ((data>>1)&0x78)+7 ];

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}
