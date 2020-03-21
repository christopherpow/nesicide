#if !defined ( CMEMORY_H )
#define CMEMORY_H

#include "nes_emulator_core.h"

#include "cnesmappers.h"
#include "cnes.h"

#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cmemorydata.h"

extern char* DISASSEMBLE(uint8_t* pOpcode, char* buffer);

class C6502;
class CMEMORY;

class CMEMORYBANK
{
public:
   CMEMORYBANK();
   virtual ~CMEMORYBANK ();

   void INITIALIZE(CMEMORY *parent,
                   uint32_t bankNum,
                   uint32_t size,
                   uint32_t sizeMask);

   inline uint32_t BANKNUM() const { return m_bankNum; }

   uint32_t BASEADDR() const;

   // Support functions for inline disassembly
   inline void OPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      addr &= m_sizeMask;
      if ( (*(m_opcodeMask+addr)) != mask )
      {
         m_opcodeMaskDirty = true;
      }
      *(m_opcodeMask+addr) = mask;
   }
   inline void OPCODEMASKATPHYSADDR ( uint32_t absAddr, uint8_t mask )
   {
      absAddr &= m_sizeMask;
      *(m_opcodeMask+absAddr) = mask;
   }
   inline void OPCODEMASKCLR ( void )
   {
      uint32_t idx1;
      for ( idx1 = 0; idx1 < m_size; idx1++ )
      {
         m_opcodeMask[idx1] = 0;
      }
      m_opcodeMaskDirty = true;
   }
   uint32_t SLOC2VIRTADDR ( uint16_t sloc )
   {
      if ( sloc < m_size )
      {
         return *(m_sloc2addr+sloc);
      }
      return 0;
   }
   uint16_t ADDR2SLOC ( uint32_t addr )
   {
      // addr is enforced within bounds of this bank by caller
      return *(m_addr2sloc+addr);
   }
   inline uint32_t SLOC ()
   {
      return m_sloc;
   }
   char* DISASSEMBLY ( uint32_t addr )
   {
      addr &= m_sizeMask;
      return *(m_disassembly+addr);
   }
   void DISASSEMBLE ();

   void MEMCLR ()
   {
      memset(m_memory,0,m_size);
   }

   void MEMSET (uint8_t* data)
   {
      memcpy(m_memory,data,m_size);
   }

   inline uint8_t* MEMPTR (uint32_t addr)
   {
      addr &= m_sizeMask;
      return m_memory+addr;
   }

   inline uint8_t MEM (uint32_t addr)
   {
      addr &= m_sizeMask;
      return *(m_memory+addr);
   }

   inline void MEM (uint32_t addr, uint8_t data)
   {
      addr &= m_sizeMask;
      *(m_memory+addr) = data;
   }

   inline CCodeDataLogger* LOGGER() const
   {
      return m_pLogger;
   }

protected:
   CMEMORY *m_parent;
   uint8_t* m_memory;
   uint32_t m_bankNum;
   uint32_t m_size;
   uint32_t m_sizeMask;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the CPU core as it performs fetches, reads,
   // writes, and DMA transfers to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   CCodeDataLogger* m_pLogger;

   uint8_t*  m_opcodeMask;
   bool      m_opcodeMaskDirty;
   char**    m_disassembly;
   uint16_t* m_sloc2addr;
   uint16_t* m_addr2sloc;
   uint32_t  m_sloc;
};

class CMEMORY
{
public:
   CMEMORY(uint32_t virtBaseAddress,
           uint32_t bankSize,
           uint32_t numPhysBanks = 1,
           uint32_t numVirtBanks = 1);        // represent unity for no bankswitch
   virtual ~CMEMORY();

   void PRINTABLEADDR(char* buffer, uint32_t virtAddr);
   void PRINTABLEADDR(char* buffer, uint32_t virtAddr, uint32_t physAddr);

   uint32_t VIRTBASEADDR() const { return m_virtBaseAddress; }

   void RESET ( bool soft );

   inline CMEMORYBANK* PHYSBANK(uint32_t bank) const { return &m_bank[bank]; }
   inline CMEMORYBANK* VIRTBANK(uint32_t bank) const { return m_pBank[bank]; }

   inline uint32_t TOTALSIZE() const { return m_totalPhysSize; }

   // Code/Data logger support functions
   virtual CCodeDataLogger* LOGGER (uint32_t virtAddr = 0)
   {
      return ((*m_pBank+virtBankFromVirtAddr(virtAddr)))->LOGGER();
   }
   virtual CCodeDataLogger* LOGGERATPHYSADDR (uint32_t physAddr)
   {
      return (m_bank+physBankFromPhysAddr(physAddr))->LOGGER();
   }

   // Support functions for inline disassembly
   virtual void OPCODEMASK ( uint32_t virtAddr, uint8_t mask )
   {
      (*(m_pBank+virtBankFromVirtAddr(virtAddr)))->OPCODEMASK(virtAddr,mask);
   }
   virtual void OPCODEMASKATPHYSADDR ( uint32_t physAddr, uint8_t mask )
   {
      (m_bank+physBankFromPhysAddr(physAddr))->OPCODEMASK(physAddr,mask);
   }
   inline void OPCODEMASKCLR ( void )
   {
      uint32_t bank;
      for ( bank = 0; bank < m_numPhysBanks; bank++ )
      {
         m_bank[bank].OPCODEMASKCLR();
      }
   }
   uint32_t SLOC2VIRTADDR ( uint16_t sloc );
   uint16_t ADDR2SLOC ( uint32_t virtAddr );
   virtual uint32_t SLOC ( uint32_t virtAddr )
   {
      return (*(m_pBank+virtBankFromVirtAddr(virtAddr)))->SLOC();
   }
   virtual char* DISASSEMBLY ( uint32_t virtAddr )
   {
      return (*(m_pBank+virtBankFromVirtAddr(virtAddr)))->DISASSEMBLY(virtAddr);
   }
   virtual char* DISASSEMBLYATPHYSADDR ( uint32_t physAddr, char* buffer )
   {
      return ::DISASSEMBLE((m_bank+physBankFromPhysAddr(physAddr))->MEMPTR(physAddr),buffer);
   }

   void DISASSEMBLE ();

   void MEMCLR()
   {
      uint32_t bank;

      for ( bank = 0; bank < m_numPhysBanks; bank++ )
      {
         m_bank[bank].MEMCLR();
      }
   }

   void MEMSET(uint8_t* data)
   {
      uint32_t bank;

      for ( bank = 0; bank < m_numPhysBanks; bank++ )
      {
         m_bank[bank].MEMSET(data+(bank*m_bankSize));
      }
   }

   virtual uint8_t* MEMPTR (uint32_t addr)
   {
      return (*(m_pBank+virtBankFromVirtAddr(addr)))->MEMPTR(addr);
   }

   virtual uint8_t MEM (uint32_t addr)
   {
      return (*(m_pBank+virtBankFromVirtAddr(addr)))->MEM(addr);
   }

   virtual void MEM (uint32_t addr, uint8_t data)
   {
      (*(m_pBank+virtBankFromVirtAddr(addr)))->MEM(addr,data);
   }

   virtual uint8_t MEMATPHYSADDR (uint32_t absAddr)
   {
      return (m_bank+physBankFromPhysAddr(absAddr))->MEM(absAddr);
   }

   virtual void MEMATPHYSADDR (uint32_t absAddr, uint8_t data)
   {
      (m_bank+physBankFromPhysAddr(absAddr))->MEM(absAddr,data);
   }

   virtual void REMAP(uint32_t virt, uint32_t phys)
   {
      (*(m_pBank+virt)) = m_bank+phys;
   }

   virtual void REMAPEXT(uint32_t virt, CMEMORYBANK* phys)
   {
      (*(m_pBank+virt)) = phys;
   }

   inline uint32_t bankSize() const
   {
      return m_bankSize;
   }

   inline uint32_t offsetFromBase(uint32_t addr) const
   {
      return addr-m_virtBaseAddress;
   }

   inline uint32_t virtBankFromVirtAddr(uint32_t virtAddr) const
   {
      // Return the bank within the physical constraint of the number
      // of virtual banks (bank slots in emulated machine) that the
      // address falls within.
      return (offsetFromBase(virtAddr)&m_totalVirtSizeMask)>>m_bankBits;
   }

   inline uint32_t offsetInBank(uint32_t addr) const
   {
      // Return the inside-bank portion of addr.
      return addr&m_bankSizeMask;
   }

   inline uint32_t physBankFromPhysAddr(uint32_t physAddr) const
   {
      // Return the bank within the physical constraint of the number
      // of physical banks (bank slots in storage media) that the
      // address falls within.
      return (physAddr>>m_bankBits)&m_numPhysBanksMask;
   }

   inline uint32_t physBankFromVirtAddr(uint32_t virtAddr) const
   {
      // Return the bank within the physical constraint of the number
      // of physical banks (bank slots in storage media) that the
      // address falls within after converting the address from its
      // virtual (bank slots in emulated machine) to physical form.
      return (*(m_pBank+virtBankFromVirtAddr(virtAddr)))->BANKNUM();
   }

   inline uint32_t physAddrFromVirtAddr(uint32_t virtAddr) const
   {
      return (physBankFromVirtAddr(virtAddr)<<m_bankBits)+offsetInBank(virtAddr);
   }

protected:
   CMEMORYBANK* m_bank;
   CMEMORYBANK** m_pBank;
   uint32_t m_bankSize;
   uint32_t m_bankSizeMask;
   uint32_t m_numPhysBanks;
   uint32_t m_numPhysBanksMask;
   uint32_t m_numVirtBanks;
   uint32_t m_numVirtBanksMask;
   uint32_t m_bankBits;
   uint32_t m_virtBaseAddress;
   uint32_t m_totalPhysSize;
   uint32_t m_totalPhysSizeMask;
   uint32_t m_totalVirtSize;
   uint32_t m_totalVirtSizeMask;
};

class COPENBUS: public CMEMORY
{
public:
   COPENBUS() : CMEMORY(0,1) {}
   virtual ~COPENBUS() {}

   // Code/Data logger support functions
   CCodeDataLogger* LOGGER (uint32_t virtAddr = 0);
   CCodeDataLogger* LOGGERATPHYSADDR (uint32_t physAddr);

   void OPCODEMASK ( uint32_t virtAddr, uint8_t mask ) {}
   virtual void OPCODEMASKATPHYSADDR ( uint32_t physAddr, uint8_t mask ) {}

   uint8_t MEM (uint32_t addr);
   void MEM (uint32_t addr, uint8_t data);
   uint8_t MEMATPHYSADDR (uint32_t absAddr);
   void MEMATPHYSADDR (uint32_t absAddr, uint8_t data);

   uint8_t* MEMPTR (uint32_t addr) { return m_pBank[0]->MEMPTR(0); }

   void REMAP(uint32_t virt, uint32_t phys) {}
   void REMAPEXT(uint32_t virt, CMEMORYBANK* phys) {}

   uint32_t SLOC ( uint32_t virtAddr ) { return 1; }
   char* DISASSEMBLY ( uint32_t virtAddr ) { return "???"; }
   char* DISASSEMBLYATPHYSADDR ( uint32_t physAddr, char* buffer ) { return "???"; }

   uint32_t TOTALSIZE() const { return 0; }
};

#endif // CMEMORY_H
