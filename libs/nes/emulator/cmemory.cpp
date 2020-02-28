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

#include "cmemory.h"
#include "cnes6502.h"

CMEMORYBANK::CMEMORYBANK() :
   m_memory(NULL),
   m_bankNum(0),
   m_size(0),
   m_sizeMask(0)
{}

CMEMORYBANK::~CMEMORYBANK()
{
   uint32_t addr;

   for ( addr = 0; addr < m_size; addr++ )
   {
      delete m_disassembly[addr];
   }

   delete m_pLogger;

   delete [] m_memory;
   delete [] m_opcodeMask;
   delete [] m_sloc2addr;
   delete [] m_addr2sloc;
}

uint32_t CMEMORYBANK::BASEADDR() const
{
   return m_parent->VIRTBASEADDR()+(m_bankNum*m_size);
}


void CMEMORYBANK::INITIALIZE(CMEMORY *parent,
                        uint32_t bankNum,
                        uint32_t size,
                        uint32_t sizeMask)
{
   uint32_t addr;

   m_parent = parent;

   m_bankNum = bankNum;
   m_size = size;
   m_sizeMask = sizeMask;

   m_memory = new uint8_t[m_size];
   m_disassembly = new char*[m_size];
   m_opcodeMaskDirty = true;
   m_opcodeMask = new uint8_t[m_size];
   m_sloc2addr = new uint16_t[m_size];
   m_addr2sloc = new uint16_t[m_size];
   m_sloc = m_size; // assume 1:1 map until otherwise known

   m_pLogger = new CCodeDataLogger ( m_size, m_sizeMask );

   for ( addr = 0; addr < m_size; addr++ )
   {
      m_disassembly[addr] = new char [ 16 ];
      m_disassembly[addr][0] = '\0';
      m_opcodeMask[addr] = 0;
      m_sloc2addr[addr] = addr; // assume 1:1 map until otherwise known
      m_addr2sloc[addr] = addr; // assume 1:1 map until otherwise known
   }
}

void CMEMORYBANK::DISASSEMBLE()
{
   // Disassemble if necessary...
   if ( m_opcodeMaskDirty )
   {
      CNES::NES()->CPU()->DISASSEMBLE ( m_disassembly,
                           m_memory,
                           m_size,
                           m_opcodeMask,
                           m_sloc2addr,
                           m_addr2sloc,
                           &(m_sloc) );

      m_opcodeMaskDirty = false;
   }
}

uint32_t intLog(uint32_t a)
{
   uint32_t p = 0;
   if ( a == 0 ) return p;
   do
   {
      a >>= 1;
      if ( a )
      {
         p++;
      }
   } while ( a );
   return p;
}

CMEMORY::CMEMORY(uint32_t virtBaseAddress,
                 uint32_t bankSize,
                 uint32_t numPhysBanks,
                 uint32_t numVirtBanks) :
   m_bank(NULL),
   m_bankSize(bankSize),
   m_bankSizeMask(bankSize-1),
   m_numPhysBanks(numPhysBanks),
   m_numPhysBanksMask(numPhysBanks-1),
   m_numVirtBanks(numVirtBanks),
   m_numVirtBanksMask(numVirtBanks-1),
   m_virtBaseAddress(virtBaseAddress),
   m_totalPhysSize(bankSize*numPhysBanks),
   m_totalPhysSizeMask(m_totalPhysSize-1),
   m_totalVirtSize(bankSize*numVirtBanks),
   m_totalVirtSizeMask(m_totalVirtSize-1)
{
   uint32_t bank;
   uint32_t physBank;

   m_bankBits = intLog(m_bankSize);

   m_bank = new CMEMORYBANK[m_numPhysBanks];
   m_pBank = new CMEMORYBANK*[m_numVirtBanks];

   for ( bank = 0; bank < m_numPhysBanks; bank++ )
   {
      // Set up bank...
      m_bank[bank].INITIALIZE(this,
                              bank,
                              m_bankSize,
                              m_bankSizeMask);
   }
   for ( bank = 0, physBank = 0; bank < m_numVirtBanks; bank++, physBank++ )
   {
      // Make sure if there's more virtual than physical that we don't go in weeds.
      physBank %= m_numPhysBanks;

      // Start with identity mapping.
      m_pBank[bank] = &m_bank[physBank];
   }
}

CMEMORY::~CMEMORY()
{
   delete [] m_bank;
   delete [] m_pBank;
}

void CMEMORY::RESET ( bool soft )
{
   uint32_t bank;
   uint32_t physBank;

// CP ALWAYS DO   if ( nesIsDebuggable )
   {
      // Clear Code/Data Logger info...
      for ( bank = 0; bank < m_numPhysBanks; bank++ )
      {
         m_bank[bank].LOGGER()->ClearData();
      }
   }

   for ( bank = 0, physBank = 0; bank < m_numVirtBanks; bank++, physBank++ )
   {
      // Make sure if there's more virtual than physical that we don't go in weeds.
      physBank %= m_numPhysBanks;

      // Start with identity mapping.
      m_pBank[bank] = &m_bank[physBank];
   }
}

void CMEMORY::DISASSEMBLE ()
{
   uint32_t bank;

   for ( bank = 0; bank < m_numPhysBanks; bank++ )
   {
      m_bank[bank].DISASSEMBLE();
   }
}

uint32_t CMEMORY::SLOC2VIRTADDR ( uint16_t sloc )
{
   uint32_t slocSoFar = 0;
   uint32_t addr = m_virtBaseAddress;
   uint32_t bank;

   for ( bank = 0; bank < m_numVirtBanks; bank++ )
   {
      slocSoFar += m_pBank[bank]->SLOC();
      addr += m_bankSize;
      if ( sloc <= slocSoFar )
      {
         addr -= m_bankSize;
         slocSoFar -= m_pBank[bank]->SLOC();

         sloc -= slocSoFar;
         break;
      }
   }

   if ( bank < m_numVirtBanks )
   {
      return addr+m_pBank[bank]->SLOC2VIRTADDR(sloc);
   }
   return 0;
}

uint16_t CMEMORY::ADDR2SLOC ( uint32_t virtAddr )
{
   uint32_t virtBank = virtBankFromVirtAddr(virtAddr);
   uint32_t slocSoFar = 0;
   uint32_t bank;

   for ( bank = 0; bank < virtBank; bank++ )
   {
      slocSoFar += m_pBank[bank]->SLOC();
   }

   return slocSoFar+m_pBank[virtBank]->ADDR2SLOC(offsetInBank(virtAddr));
}

void CMEMORY::PRINTABLEADDR(char* buffer, uint32_t virtAddr)
{
   if ( m_numPhysBanks == 1 )
   {
      sprintf ( buffer, "%04X", virtAddr );
   }
   else
   {
      sprintf ( buffer, "%02X:%04X(%04X)", physBankFromVirtAddr(virtAddr), offsetInBank(virtAddr), virtAddr );
   }
}

void CMEMORY::PRINTABLEADDR(char* buffer, uint32_t virtAddr, uint32_t physAddr)
{
   if ( m_numPhysBanks == 1 )
   {
      sprintf ( buffer, "%04X", virtAddr );
   }
   else
   {
      sprintf ( buffer, "%02X:%04X(%04X)", physBankFromPhysAddr(physAddr), offsetInBank(virtAddr), virtAddr );
   }
}

uint8_t COPENBUS::MEM (uint32_t addr)
{
   return CNES::NES()->CPU()->OPENBUS();
}

void COPENBUS::MEM (uint32_t addr, uint8_t data)
{
   return;
}

uint8_t COPENBUS::MEMATPHYSADDR (uint32_t absAddr)
{
   return CNES::NES()->CPU()->OPENBUS();
}

void COPENBUS::MEMATPHYSADDR (uint32_t absAddr, uint8_t data)
{
   return;
}
