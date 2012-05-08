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

#include "cnesrommapper033.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 033 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 6, "%X", 0),
   new CBitfieldData("Mirroring", 6, 1, "%X", 2, "Vertical", "Horizontal")
};

static CBitfieldData* tbl8001Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 6, "%X", 0)
};

static CBitfieldData* tbl8002Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CBitfieldData* tbl8003Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CBitfieldData* tblA001Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CBitfieldData* tblA002Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CBitfieldData* tblA003Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Mapping 1", nesMapperHighRead, nesMapperHighWrite, 2, tbl8000Bitfields),
   new CRegisterData(0x8001, "PRG Mapping 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8001Bitfields),
   new CRegisterData(0x8002, "CHR Mapping 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8002Bitfields),
   new CRegisterData(0x8003, "CHR Mapping 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8003Bitfields),
   new CRegisterData(0xA000, "CHR Mapping 3", nesMapperHighRead, nesMapperHighWrite, 1, tblA000Bitfields),
   new CRegisterData(0xA001, "CHR Mapping 4", nesMapperHighRead, nesMapperHighWrite, 1, tblA001Bitfields),
   new CRegisterData(0xA002, "CHR Mapping 5", nesMapperHighRead, nesMapperHighWrite, 1, tblA002Bitfields),
   new CRegisterData(0xA003, "CHR Mapping 6", nesMapperHighRead, nesMapperHighWrite, 1, tblA003Bitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","8001","8002","8003","A000","A001","A002","A003"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,8,8,tblRegisters,rowHeadings,columnHeadings);

uint8_t  CROMMapper033::m_reg[8] = { 0x00, };

CROMMapper033::CROMMapper033()
{
}

CROMMapper033::~CROMMapper033()
{
}

void CROMMapper033::RESET ()
{
   m_mapper = 33;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper );

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper033::MAPPER ( uint32_t addr )
{
   switch ( addr&0xA003 )
   {
   case 0x8000:
      return m_reg[0];
      break;
   case 0x8001:
      return m_reg[1];
      break;
   case 0x8002:
      return m_reg[2];
      break;
   case 0x8003:
      return m_reg[3];
      break;
   case 0xA000:
      return m_reg[4];
      break;
   case 0xA001:
      return m_reg[5];
      break;
   case 0xA002:
      return m_reg[6];
      break;
   case 0xA003:
      return m_reg[7];
      break;
   }
}

void CROMMapper033::MAPPER ( uint32_t addr, uint8_t data )
{
   uint8_t bank;

   switch ( addr&0xA003 )
   {
   case 0x8000:
      m_reg[0] = data;
      if ( m_reg[0]&0x40 )
      {
         CPPU::MIRRORHORIZ ();
      }
      else
      {
         CPPU::MIRRORVERT ();
      }
      bank = (m_reg[0]&0x3F)%m_numPrgBanks;
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ bank ];
      break;
   case 0x8001:
      m_reg[1] = data;
      bank = (m_reg[1]&0x3F)%m_numPrgBanks;
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ bank ];
      break;
   case 0x8002:
      m_reg[2] = data;
      m_pCHRmemory [ 0 ] = m_CHRROMmemory [ (m_reg[2]>>2)%m_numChrBanks ] + ((m_reg[2]&0x3)<<UPSHIFT_2KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 1 ] = m_CHRROMmemory [ (m_reg[2]>>2)%m_numChrBanks ] + ((m_reg[2]&0x3)<<UPSHIFT_2KB) + (1<<UPSHIFT_1KB);
      break;
   case 0x8003:
      m_reg[3] = data;
      m_pCHRmemory [ 2 ] = m_CHRROMmemory [ (m_reg[3]>>2)%m_numChrBanks ] + ((m_reg[3]&0x3)<<UPSHIFT_2KB) + (0<<UPSHIFT_1KB);
      m_pCHRmemory [ 3 ] = m_CHRROMmemory [ (m_reg[3]>>2)%m_numChrBanks ] + ((m_reg[3]&0x3)<<UPSHIFT_2KB) + (1<<UPSHIFT_1KB);
      break;
   case 0xA000:
      m_reg[4] = data;
      m_pCHRmemory [ 4 ] = m_CHRROMmemory [ (m_reg[4]>>3)%m_numChrBanks ] + ((m_reg[4]&0x7)<<UPSHIFT_1KB);
      break;
   case 0xA001:
      m_reg[5] = data;
      m_pCHRmemory [ 5 ] = m_CHRROMmemory [ (m_reg[5]>>3)%m_numChrBanks ] + ((m_reg[5]&0x7)<<UPSHIFT_1KB);
      break;
   case 0xA002:
      m_reg[6] = data;
      m_pCHRmemory [ 6 ] = m_CHRROMmemory [ (m_reg[6]>>3)%m_numChrBanks ] + ((m_reg[6]&0x7)<<UPSHIFT_1KB);
      break;
   case 0xA003:
      m_reg[7] = data;
      m_pCHRmemory [ 7 ] = m_CHRROMmemory [ (m_reg[7]>>3)%m_numChrBanks ] + ((m_reg[7]&0x7)<<UPSHIFT_1KB);
      break;
   }

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,0);
   }
}

void CROMMapper033::LOAD ( MapperState* data )
{
   CROM::LOAD ( data );
}

void CROMMapper033::SAVE ( MapperState* data )
{
   CROM::SAVE ( data );
}
