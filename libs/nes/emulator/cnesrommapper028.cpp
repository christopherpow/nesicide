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

#include "cnesrommapper028.h"

#include "cnes6502.h"
#include "cnesppu.h"

CROMMapper028::CROMMapper028()
   : CROM(28)
{
   m_reg_sel = 0x00;
   m_chr_bank = 0x00;
   m_prg_inner_bank = 0x00;
   m_prg_size = 0x00;
   m_prg_mode = 0x00;
   m_mirror = 0x00;
   m_prg_outer_bank = 0x00;
   m_bank_size_mask[0] = 0x1;
   m_bank_size_mask[1] = 0x3;
   m_bank_size_mask[2] = 0x7;
   m_bank_size_mask[3] = 0xF;
}

CROMMapper028::~CROMMapper028()
{
}

void CROMMapper028::RESET ( bool soft )
{
   if ( !soft )
   {
      CROM::RESET ( soft );

      m_reg_sel = 0x00;
      m_chr_bank = 0x00;
      m_prg_inner_bank = 0x00;
      m_prg_size = 0x00;
      m_prg_mode = 0x00;
      m_mirror = 0x00;
      m_prg_outer_bank = 0x00;

      m_PRGROMmemory.REMAP(0,m_numPrgBanks-4);
      m_PRGROMmemory.REMAP(1,m_numPrgBanks-3);

      // If the ROM contains only one 16KB PRG-ROM bank then it needs to be replicated
      // to the second PRG-ROM bank slot...
      if ( m_numPrgBanks == 2 )
      {
         m_PRGROMmemory.REMAP(2,m_numPrgBanks-4);
         m_PRGROMmemory.REMAP(3,m_numPrgBanks-3);
      }
      else
      {
         m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
         m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);
      }

      // CHR ROM/RAM already set up in CROM::RESET()...
   }
}

void CROMMapper028::SETCPU ( void )
{
   uint8_t bank[2];
   uint8_t prg_mode_bank[2];
   uint8_t current_bank[2];
   uint8_t prg_size[2];
   uint8_t outer_bank;

   outer_bank  = m_prg_outer_bank << 1;
   outer_bank &= ((m_numPrgBanks-1)>>1);

   // in UNROM fixed bank?
   prg_mode_bank[0] = m_prg_mode;
   prg_mode_bank[1] = m_prg_mode;
   prg_size[0] = m_prg_size;
   prg_size[1] = m_prg_size;
   if ( ((m_prg_mode^0)&3) == 2 )
   {
      prg_mode_bank[0] = 0;
      prg_size[0] = 0;
   }
   if ( ((m_prg_mode^1)&3) == 2 )
   {
      prg_mode_bank[1] = 0;
      prg_size[1] = 0;
   }

   current_bank[0] = m_prg_inner_bank;
   current_bank[1] = m_prg_inner_bank;
   if ( (prg_mode_bank[0] & 0x02) == 0 ) // in 32K bank mode?
   {
      current_bank[0] <<= 1;
   }
   if ( (prg_mode_bank[1] & 0x02) == 0 ) // in 32K bank mode?
   {
      current_bank[1] <<= 1;
      current_bank[1] |= 0x01;
   }

   bank[0] = ((current_bank[0]^outer_bank)&m_bank_size_mask[prg_size[0]])^outer_bank;
   bank[1] = ((current_bank[1]^outer_bank)&m_bank_size_mask[prg_size[1]])^outer_bank;

   bank[0] <<= 1;
   bank[1] <<= 1;

   m_PRGROMmemory.REMAP(0,bank[0]+0);
   m_PRGROMmemory.REMAP(1,bank[0]+1);
   m_PRGROMmemory.REMAP(2,bank[1]+0);
   m_PRGROMmemory.REMAP(3,bank[1]+1);
}

void CROMMapper028::SETPPU ( void )
{
   if ( m_mirror == 0 )
   {
      CNES::NES()->PPU()->MIRROR(0,0,0,0);
   }
   else if ( m_mirror == 1 )
   {
      CNES::NES()->PPU()->MIRROR(1,1,1,1);
   }
   else if ( m_mirror == 2 )
   {
      CNES::NES()->PPU()->MIRRORVERT();
   }
   else if ( m_mirror == 3 )
   {
      CNES::NES()->PPU()->MIRRORHORIZ();
   }

   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+0);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+1);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+2);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+3);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+4);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+5);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+6);
   m_CHRmemory.REMAP(0,((m_chr_bank)<<3)+7);
}

uint32_t CROMMapper028::DEBUGINFO ( uint32_t addr )
{
   // CPTODO: return mapper registers here...
   return 0xA1;
}

void CROMMapper028::HMAPPER ( uint32_t addr, uint8_t data )
{
	if ( ((m_reg_sel&0x02) == 0) && ((m_mirror&0x02) == 0) )
   {
		//$0x registers while in mirror mode 0 & 1 only
      m_mirror &= 0xFE;
		m_mirror |= (data&0x10)>>4;
   }

   switch ( m_reg_sel )
   {
   case 0:
      m_chr_bank = (data&0x03);
      break;
   case 1:
      m_prg_inner_bank = (data&0x0F);
      break;
   case 2:
      m_prg_size = (data&0x30)>>4;
      m_prg_mode = (data&0x0C)>>2;
      m_mirror = (data&0x03);
      break;
   case 3:
      m_prg_outer_bank = (data&0x3F);
      break;
   }
   SETCPU();
   SETPPU();
}

uint32_t CROMMapper028::LMAPPER ( uint32_t addr )
{
   uint8_t data = 0xff;

   return data;
}

void CROMMapper028::LMAPPER ( uint32_t addr, uint8_t data )
{
   m_reg_sel = (data&0x01)|((data&0x80)>>6);
   SETCPU();
   SETPPU();
}
