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

#include "cnesrommapper111.h"

#include "cregisterdata.h"

// Mapper 111 Registers
static CBitfieldData* tbl5000_5FFFBitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0),
   new CBitfieldData("CHR Bank", 4, 1, "%X", 0),
   new CBitfieldData("Nametable Bank", 5, 1, "%X", 0),
   new CBitfieldData("Red LED", 6, 1, "%X", 2, "On", "Off"),
   new CBitfieldData("Green LED", 7, 1, "%X", 2, "On", "Off")
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x5000, "Bank Select", nesMapperHighRead, nesMapperHighWrite, 5, tbl5000_5FFFBitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "5000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,1,1,tblRegisters,rowHeadings,columnHeadings);

uint8_t  CROMMapper111::m_reg = 0x00;

CROMMapper111::CROMMapper111()
{
}

CROMMapper111::~CROMMapper111()
{
}

void CROMMapper111::RESET ( bool soft )
{
   m_mapper = 111;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 3 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper111::DEBUGINFO ( uint32_t addr )
{
   return m_reg;
}

void CROMMapper111::LMAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   m_reg = data;

   bank = (m_reg&0x0f)<<2;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank+1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ bank+2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ bank+3 ];

   bank = (m_reg&0x10)>>1;

   m_pCHRmemory [ 0 ] = m_CHRmemory [ bank+0 ];
   m_pCHRmemory [ 1 ] = m_CHRmemory [ bank+1 ];
   m_pCHRmemory [ 2 ] = m_CHRmemory [ bank+2 ];
   m_pCHRmemory [ 3 ] = m_CHRmemory [ bank+3 ];
   m_pCHRmemory [ 4 ] = m_CHRmemory [ bank+4 ];
   m_pCHRmemory [ 5 ] = m_CHRmemory [ bank+5 ];
   m_pCHRmemory [ 6 ] = m_CHRmemory [ bank+6 ];
   m_pCHRmemory [ 7 ] = m_CHRmemory [ bank+7 ];

   bank = (m_reg&0x20)>>2;

   m_pVRAMmemory [ 0 ] = m_VRAMmemory+((bank+0)*MEM_1KB);
   m_pVRAMmemory [ 1 ] = m_VRAMmemory+((bank+1)*MEM_1KB);
   m_pVRAMmemory [ 2 ] = m_VRAMmemory+((bank+2)*MEM_1KB);
   m_pVRAMmemory [ 3 ] = m_VRAMmemory+((bank+3)*MEM_1KB);
   m_pVRAMmemory [ 4 ] = m_VRAMmemory+((bank+4)*MEM_1KB);
   m_pVRAMmemory [ 5 ] = m_VRAMmemory+((bank+5)*MEM_1KB);
   m_pVRAMmemory [ 6 ] = m_VRAMmemory+((bank+6)*MEM_1KB);
   m_pVRAMmemory [ 7 ] = m_VRAMmemory+((bank+7)*MEM_1KB);

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}
