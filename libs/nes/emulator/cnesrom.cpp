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

#include "cnesrom.h"
#include "cnesppu.h"
#include "cnes6502.h"

#include "cnesrommapper001.h"
#include "cnesrommapper002.h"
#include "cnesrommapper003.h"
#include "cnesrommapper004.h"
#include "cnesrommapper005.h"
#include "cnesrommapper007.h"
#include "cnesrommapper009.h"
#include "cnesrommapper010.h"
#include "cnesrommapper011.h"
#include "cnesrommapper013.h"
#include "cnesrommapper016.h"
#include "cnesrommapper018.h"
#include "cnesrommapper019.h"
#include "cnesrommapper021.h"
#include "cnesrommapper022.h"
#include "cnesrommapper023.h"
#include "cnesrommapper024.h"
#include "cnesrommapper025.h"
#include "cnesrommapper026.h"
#include "cnesrommapper028.h"
#include "cnesrommapper033.h"
#include "cnesrommapper034.h"
#include "cnesrommapper065.h"
#include "cnesrommapper068.h"
#include "cnesrommapper069.h"
#include "cnesrommapper073.h"
#include "cnesrommapper075.h"
#include "cnesrommapper111.h"

CROM* CARTFACTORY(uint32_t mapper)
{
   switch ( mapper )
   {
   default:
      return CROM::CARTFACTORY();
   case 1:
      return CROMMapper001::CARTFACTORY();
   case 2:
      return CROMMapper002::CARTFACTORY();
   case 3:
      return CROMMapper003::CARTFACTORY();
   case 4:
      return CROMMapper004::CARTFACTORY();
   case 5:
      return CROMMapper005::CARTFACTORY();
   case 7:
      return CROMMapper007::CARTFACTORY();
   case 9:
      return CROMMapper009::CARTFACTORY();
   case 10:
      return CROMMapper010::CARTFACTORY();
   case 11:
      return CROMMapper011::CARTFACTORY();
   case 13:
      return CROMMapper013::CARTFACTORY();
   case 16:
      return CROMMapper016::CARTFACTORY();
   case 18:
      return CROMMapper018::CARTFACTORY();
   case 19:
      return CROMMapper019::CARTFACTORY();
   case 21:
      return CROMMapper021::CARTFACTORY();
   case 22:
      return CROMMapper022::CARTFACTORY();
   case 23:
      return CROMMapper023::CARTFACTORY();
   case 24:
      return CROMMapper024::CARTFACTORY();
   case 25:
      return CROMMapper025::CARTFACTORY();
   case 26:
      return CROMMapper026::CARTFACTORY();
   case 28:
      return CROMMapper028::CARTFACTORY();
   case 33:
      return CROMMapper033::CARTFACTORY();
   case 34:
      return CROMMapper034::CARTFACTORY();
   case 65:
      return CROMMapper065::CARTFACTORY();
   case 68:
      return CROMMapper068::CARTFACTORY();
   case 69:
      return CROMMapper069::CARTFACTORY();
   case 73:
      return CROMMapper073::CARTFACTORY();
   case 75:
      return CROMMapper075::CARTFACTORY();
   case 111:
      return CROMMapper111::CARTFACTORY();
   }
}

CROM::CROM(uint32_t mapper)
   : m_PRGROMmemory(CMEMORY(0x8000,MEM_8KB,NUM_ROM_BANKS,4)),
     m_CHRmemory(CMEMORY(0,MEM_1KB,NUM_CHR_BANKS,8)),
     m_pSRAMmemory(new COPENBUS()),
     m_pEXRAMmemory(new COPENBUS()),
     m_pVRAMmemory(new COPENBUS())
{
   m_mapper = mapper;
   m_numPrgBanks = 0;
   m_numChrBanks = 0;
   m_prgRemappable = false;
   m_chrRemappable = false;

   CROM::RESET ( false );
}

CROM::~CROM()
{
   delete m_pSRAMmemory;
   delete m_pEXRAMmemory;
   delete m_pVRAMmemory;
}

void CROM::SetPRGBank ( int32_t bank, uint8_t* data )
{
   m_PRGROMmemory.PHYSBANK(bank)->MEMSET(data);
   m_numPrgBanks++;
}

void CROM::SetCHRBank ( int32_t bank, uint8_t* data )
{
   uint8_t ibank;
   for ( ibank = 0; ibank < 8; ibank++ )
   {
      m_CHRmemory.PHYSBANK((bank<<3)+ibank)->MEMSET(data+(ibank*MEM_1KB));
   }
   m_numChrBanks += 1;
}

void CROM::DoneLoadingBanks ()
{
   if ( nesIsDebuggable )
   {
      // Initial disassembly will be 'crap' but do it anyway...
      DISASSEMBLE();
   }

   // We just put a cartridge in...the SRAM isn't dirty yet.
   m_SRAMdirty = false;
}

void CROM::RESET ( bool soft )
{
   if ( m_mapper == 0 )
   {
      m_dbCartRegisters = NULL;
   }

   m_PRGROMmemory.RESET(soft);
   m_CHRmemory.RESET(soft);
   m_pSRAMmemory->RESET(soft);
   m_pVRAMmemory->RESET(soft);

   // Support for NROM-368 for Shiru and crew.
   if ( (m_mapper == 0) && (m_numPrgBanks > 4) )
   {
      // A NROM-368 file is laid out linearly so the six 8KB PRG-ROM banks will be
      // loaded into the following memory addresses:
      // 0: $4000-$5fff
      // 1: $6000-$7fff
      // 2: $8000-$9fff
      // 3: $a000-$bfff
      // 4: $c000-$dfff
      // 5: $e000-$ffff
      // The code here takes care of shifting the PRG-ROM visible window from $8000-$ffff
      // so that the CPU sees the upper end of the linear memory there.  The lower end
      // from $4000-$7fff will be mapped in by the existing mapper "low read" APIs called
      // whenever the CPU reads from $4018-$7fff.
      m_PRGROMmemory.REMAP(0,2);
      m_PRGROMmemory.REMAP(1,3);

      m_PRGROMmemory.REMAP(2,4);
      m_PRGROMmemory.REMAP(3,5);
   }
   else
   {
      m_PRGROMmemory.REMAP(0,0);
      m_PRGROMmemory.REMAP(1,1);

      // If the ROM contains only one 16KB PRG-ROM bank then it needs to be replicated
      // to the second PRG-ROM bank slot...
      if ( m_numPrgBanks == 2 )
      {
         m_PRGROMmemory.REMAP(2,0);
         m_PRGROMmemory.REMAP(3,1);
      }
      else
      {
         m_PRGROMmemory.REMAP(2,2);
         m_PRGROMmemory.REMAP(3,3);
      }
   }
}

uint32_t CROM::LMAPPER ( uint32_t addr )
{
   uint8_t data = CNES::NES()->CPU()->OPENBUS();

   if ( (m_mapper == 0) && (m_numPrgBanks > 4) )
   {
      return m_PRGROMmemory.MEM(addr);
   }
   else
   {
      if ( addr >= 0x6000 )
      {
         data = CROM::SRAMVIRT(addr);
      }
   }

   return data;
}

void CROM::LMAPPER ( uint32_t addr, uint8_t data )
{
   if ( addr >= 0x6000 )
   {
      CROM::SRAMVIRT(addr,data);
   }
}

void CROM::DISASSEMBLE ()
{
   // Disassemble PRG-ROM banks...
   m_PRGROMmemory.DISASSEMBLE();

   // Disassemble SRAM...
   m_pSRAMmemory->DISASSEMBLE();

   // Disassemble EXRAM...
   if ( m_pEXRAMmemory )
   {
      m_pEXRAMmemory->DISASSEMBLE();
   }
}

void CROM::PRINTABLEADDR ( char* buffer, uint32_t addr )
{
   if ( addr >= 0x8000 )
   {
      m_PRGROMmemory.PRINTABLEADDR(buffer,addr);
   }
   else if ( addr >= 0x6000 )
   {
      m_pSRAMmemory->PRINTABLEADDR(buffer,addr);
   }
   else
   {
      m_pEXRAMmemory->PRINTABLEADDR(buffer,addr);
   }
}

void CROM::PRINTABLEADDR ( char* buffer, uint32_t addr, uint32_t absAddr )
{
   if ( addr >= 0x8000 )
   {
      m_PRGROMmemory.PRINTABLEADDR(buffer,addr,absAddr);
   }
   else if ( addr >= 0x6000 )
   {
      m_pSRAMmemory->PRINTABLEADDR(buffer,addr,absAddr);
   }
   else
   {
      m_pEXRAMmemory->PRINTABLEADDR(buffer,addr,absAddr);
   }
}

uint32_t CROM::PRGROMSLOC2VIRTADDR ( uint16_t sloc )
{
   return m_PRGROMmemory.SLOC2VIRTADDR(sloc);
}

uint32_t CROM::SRAMSLOC2VIRTADDR ( uint16_t sloc )
{
   return m_pSRAMmemory->SLOC2VIRTADDR(sloc);
}

uint32_t CROM::EXRAMSLOC2VIRTADDR ( uint16_t sloc )
{
   return m_pEXRAMmemory->SLOC2VIRTADDR(sloc);
}

uint16_t CROM::PRGROMADDR2SLOC ( uint32_t addr )
{
   return m_PRGROMmemory.ADDR2SLOC(addr);
}

uint16_t CROM::SRAMADDR2SLOC ( uint32_t addr )
{
   return m_pSRAMmemory->ADDR2SLOC(addr);
}

uint16_t CROM::EXRAMADDR2SLOC ( uint32_t addr )
{
   return m_pEXRAMmemory->ADDR2SLOC(addr);
}
