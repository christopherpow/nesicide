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

#include "cnesrommapper009.h"

#include "cnesppu.h"

// Mapper 009 Registers
static CBitfieldData* tblA000_AFFFBitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tblB000_BFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 0A", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC000_CFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 0B", 0, 8, "%02X", 0)
};

static CBitfieldData* tblD000_DFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 1A", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE000_EFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 1B", 0, 8, "%02X", 0)
};

static CBitfieldData* tblF000_FFFFBitfields [] =
{
   new CBitfieldData("Mirroring", 0, 1, "%X", 2, "Vertical", "Horizontal")
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0xA000, "PRG Reg", nesMapperHighRead, nesMapperHighWrite, 1, tblA000_AFFFBitfields),
   new CRegisterData(0xB000, "CHR Reg 0A", nesMapperHighRead, nesMapperHighWrite, 1, tblB000_BFFFBitfields),
   new CRegisterData(0xC000, "CHR Reg 0B", nesMapperHighRead, nesMapperHighWrite, 1, tblC000_CFFFBitfields),
   new CRegisterData(0xD000, "CHR Reg 1A", nesMapperHighRead, nesMapperHighWrite, 1, tblD000_DFFFBitfields),
   new CRegisterData(0xE000, "CHR Reg 1B", nesMapperHighRead, nesMapperHighWrite, 1, tblE000_EFFFBitfields),
   new CRegisterData(0xF000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblF000_FFFFBitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "A000", "B000", "C000", "D000", "E000", "F000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,6,6,tblRegisters,rowHeadings,columnHeadings);

CROMMapper009::CROMMapper009()
   : CROM(9)
{
   // MMC2 stuff
   memset(m_reg,0,sizeof(m_reg));
   m_latch0 = 0xFE;
   m_latch1 = 0xFE;
   m_latch0FD = 0;
   m_latch0FE = 1;
   m_latch1FD = 0;
   m_latch1FE = 0;
}

CROMMapper009::~CROMMapper009()
{
}

void CROMMapper009::RESET ( bool soft )
{
   m_dbCartRegisters = dbRegisters;

   CROM::RESET ( soft );

   if ( m_numPrgBanks != 2 )
   {
      m_PRGROMmemory.REMAP(1,m_numPrgBanks-3);
   }
   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   m_latch0 = 0xFE;
   m_latch1 = 0xFE;
   m_latch0FD = 0;
   m_latch0FE = 1;
   m_latch1FD = 0;
   m_latch1FE = 0;

   m_CHRmemory.REMAP(0,(m_latch0FE<<2)+0);
   m_CHRmemory.REMAP(1,(m_latch0FE<<2)+1);
   m_CHRmemory.REMAP(2,(m_latch0FE<<2)+2);
   m_CHRmemory.REMAP(3,(m_latch0FE<<2)+3);
   m_CHRmemory.REMAP(4,(m_latch1FE<<2)+0);
   m_CHRmemory.REMAP(5,(m_latch1FE<<2)+1);
   m_CHRmemory.REMAP(6,(m_latch1FE<<2)+2);
   m_CHRmemory.REMAP(7,(m_latch1FE<<2)+3);
}

uint32_t CROMMapper009::DEBUGINFO ( uint32_t addr )
{
   switch ( addr&0xF000 )
   {
      case 0xA000:
         return m_reg [ 0 ];
         break;
      case 0xB000:
         return m_reg [ 1 ];
         break;
      case 0xC000:
         return m_reg [ 2 ];
         break;
      case 0xD000:
         return m_reg [ 3 ];
         break;
      case 0xE000:
         return m_reg [ 4 ];
         break;
      case 0xF000:
         return m_reg [ 5 ];
         break;
   }

   return 0xFF;
}

void CROMMapper009::HMAPPER ( uint32_t addr, uint8_t data )
{
   int32_t reg = 0;

   switch ( addr&0xF000 )
   {
      case 0xA000:
         reg = 0;
         m_reg [ 0 ] = data;
         m_PRGROMmemory.REMAP(0,data);
         break;
      case 0xB000:
         reg = 1;
         m_reg [ 1 ] = data;
         m_latch0FD = data;

         if ( m_latch0 == 0xFD )
         {
            m_CHRmemory.REMAP(0,(m_latch0FD<<2)+0);
            m_CHRmemory.REMAP(1,(m_latch0FD<<2)+1);
            m_CHRmemory.REMAP(2,(m_latch0FD<<2)+2);
            m_CHRmemory.REMAP(3,(m_latch0FD<<2)+3);
         }

         break;
      case 0xC000:
         reg = 2;
         m_reg [ 2 ] = data;
         m_latch0FE = data;

         if ( m_latch0 == 0xFE )
         {
            m_CHRmemory.REMAP(0,(m_latch0FE<<2)+0);
            m_CHRmemory.REMAP(1,(m_latch0FE<<2)+1);
            m_CHRmemory.REMAP(2,(m_latch0FE<<2)+2);
            m_CHRmemory.REMAP(3,(m_latch0FE<<2)+3);
         }

         break;
      case 0xD000:
         reg = 3;
         m_reg [ 3 ] = data;
         m_latch1FD = data;

         if ( m_latch1 == 0xFD )
         {
            m_CHRmemory.REMAP(4,(m_latch1FD<<2)+0);
            m_CHRmemory.REMAP(5,(m_latch1FD<<2)+1);
            m_CHRmemory.REMAP(6,(m_latch1FD<<2)+2);
            m_CHRmemory.REMAP(7,(m_latch1FD<<2)+3);
         }

         break;
      case 0xE000:
         reg = 4;
         m_reg [ 4 ] = data;
         m_latch1FE = data;

         if ( m_latch1 == 0xFE )
         {
            m_CHRmemory.REMAP(4,(m_latch1FE<<2)+0);
            m_CHRmemory.REMAP(5,(m_latch1FE<<2)+1);
            m_CHRmemory.REMAP(6,(m_latch1FE<<2)+2);
            m_CHRmemory.REMAP(7,(m_latch1FE<<2)+3);
         }

         break;
      case 0xF000:
         reg = 5;
         m_reg [ 5 ] = data;

         if ( data&0x01 )
         {
            CNES::NES()->PPU()->MIRRORHORIZ ();
         }
         else
         {
            CNES::NES()->PPU()->MIRRORVERT ();
         }

         break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}

void CROMMapper009::SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr )
{
   if ( (ppuAddr&0x1FF0) == 0x0FD0 )
   {
      m_latch0 = 0xFD;
      m_CHRmemory.REMAP(0,(m_latch0FD<<2)+0);
      m_CHRmemory.REMAP(1,(m_latch0FD<<2)+1);
      m_CHRmemory.REMAP(2,(m_latch0FD<<2)+2);
      m_CHRmemory.REMAP(3,(m_latch0FD<<2)+3);
   }
   else if ( (ppuAddr&0x1FF0) == 0x0FE0 )
   {
      m_latch0 = 0xFE;
      m_CHRmemory.REMAP(0,(m_latch0FE<<2)+0);
      m_CHRmemory.REMAP(1,(m_latch0FE<<2)+1);
      m_CHRmemory.REMAP(2,(m_latch0FE<<2)+2);
      m_CHRmemory.REMAP(3,(m_latch0FE<<2)+3);
   }
   else if ( (ppuAddr&0x1FF0) == 0x1FD0 )
   {
      m_latch1 = 0xFD;
      m_CHRmemory.REMAP(4,(m_latch1FD<<2)+0);
      m_CHRmemory.REMAP(5,(m_latch1FD<<2)+1);
      m_CHRmemory.REMAP(6,(m_latch1FD<<2)+2);
      m_CHRmemory.REMAP(7,(m_latch1FD<<2)+3);
   }
   else if ( (ppuAddr&0x1FF0) == 0x1FE0 )
   {
      m_latch1 = 0xFE;
      m_CHRmemory.REMAP(4,(m_latch1FE<<2)+0);
      m_CHRmemory.REMAP(5,(m_latch1FE<<2)+1);
      m_CHRmemory.REMAP(6,(m_latch1FE<<2)+2);
      m_CHRmemory.REMAP(7,(m_latch1FE<<2)+3);
   }
}
