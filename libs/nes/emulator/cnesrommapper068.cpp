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

#include "cnesrommapper068.h"

#include "cnesppu.h"

// Mapper 068 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("NT-ROM Bank", 0, 7, "%02X", 0)
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 1, "%X", 2, "Vertical", "Horizontal"),
   new CBitfieldData("Source", 4, 1, "%X", 2, "Normal mirroring", "Use CHR-ROM")
};

static CBitfieldData* tblF000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "CHR Reg 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "CHR Reg 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xA000, "CHR Reg 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Reg 3", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xC000, "NT-ROM Reg 0", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xD000, "NT-ROM Reg 1", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xE000, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 2, tblE000Bitfields),
   new CRegisterData(0xF000, "PRG Reg", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields),
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000","9000","A000","B000","C000","D000","E000","F000"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,8,8,tblRegisters,rowHeadings,columnHeadings);

// Sunsoft Mapper #4 stuff
uint8_t  CROMMapper068::m_reg [] = { 0x00, };

CROMMapper068::CROMMapper068()
{
}

CROMMapper068::~CROMMapper068()
{
}

void CROMMapper068::RESET ( bool soft )
{
   int32_t idx;

   m_mapper = 68;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = 0x00;
   }

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

uint32_t CROMMapper068::DEBUGINFO ( uint32_t addr )
{
   switch ( addr&0xF000 )
   {
   case 0x8000:
      return m_reg[0];
      break;
   case 0x9000:
      return m_reg[1];
      break;
   case 0xA000:
      return m_reg[2];
      break;
   case 0xB000:
      return m_reg[3];
      break;
   case 0xC000:
      return m_reg[4];
      break;
   case 0xD000:
      return m_reg[5];
      break;
   case 0xE000:
      return m_reg[6];
      break;
   case 0xF000:
      return m_reg[7];
      break;
   }
}

void CROMMapper068::HMAPPER ( uint32_t addr, uint8_t data )
{
   switch ( addr&0xF000 )
   {
   case 0x8000:
      m_reg[0] = data;
      m_pCHRmemory [ 0 ] = m_CHRmemory [ (data<<1)+0 ];
      m_pCHRmemory [ 1 ] = m_CHRmemory [ (data<<1)+1 ];
      break;
   case 0x9000:
      m_reg[1] = data;
      m_pCHRmemory [ 2 ] = m_CHRmemory [ (data<<1)+0 ];
      m_pCHRmemory [ 3 ] = m_CHRmemory [ (data<<1)+1 ];
      break;
   case 0xA000:
      m_reg[2] = data;
      m_pCHRmemory [ 4 ] = m_CHRmemory [ (data<<1)+0 ];
      m_pCHRmemory [ 5 ] = m_CHRmemory [ (data<<1)+1 ];
      break;
   case 0xB000:
      m_reg[3] = data;
      m_pCHRmemory [ 6 ] = m_CHRmemory [ (data<<1)+0 ];
      m_pCHRmemory [ 7 ] = m_CHRmemory [ (data<<1)+1 ];
      break;
   case 0xC000:
      m_reg [ 4 ] = data|0x80;

      if ( (m_reg[6]>>4)&0x01 )
      {
         switch ( m_reg[6]&0x01 )
         {
         case 0:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x2, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x3, m_CHRmemory [ m_reg[5] ] );
            break;
         case 1:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x2, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x3, m_CHRmemory [ m_reg[5] ] );
            break;
         }
      }
      else
      {
         switch ( m_reg[6]&0x01 )
         {
            case 0:
               CPPU::MIRRORVERT ();
               break;
            case 1:
               CPPU::MIRRORHORIZ ();
               break;
         }
      }

      break;
   case 0xD000:
      m_reg [ 5 ] = data|0x80;

      if ( (m_reg[6]>>4)&0x01 )
      {
         switch ( m_reg[6]&0x01 )
         {
         case 0:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            break;
         case 1:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            break;
         }
      }
      else
      {
         switch ( m_reg[6]&0x01 )
         {
            case 0:
               CPPU::MIRRORVERT ();
               break;
            case 1:
               CPPU::MIRRORHORIZ ();
               break;
         }
      }

      break;
   case 0xE000:
      m_reg[6] = data;

      if ( (m_reg[6]>>4)&0x01 )
      {
         switch ( m_reg[6]&0x01 )
         {
         case 0:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            break;
         case 1:
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[4] ] );
            CPPU::REMAPVRAM ( 0x0, m_CHRmemory [ m_reg[5] ] );
            CPPU::REMAPVRAM ( 0x1, m_CHRmemory [ m_reg[5] ] );
            break;
         }
      }
      else
      {
         switch ( m_reg[6]&0x01 )
         {
            case 0:
               CPPU::MIRRORVERT ();
               break;
            case 1:
               CPPU::MIRRORHORIZ ();
               break;
         }
      }

      break;
   case 0xF000:
      m_reg[7] = data;
      m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ (data<<1)+0 ];
      m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ (data<<1)+1 ];
      break;
   }
}
