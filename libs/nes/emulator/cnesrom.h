#if !defined ( ROM_H )
#define ROM_H

#include "cnesmappers.h"
#include "cmemory.h"

#include "ccodedatalogger.h"

#include "cnesppu.h"

#define PPU_A13        (1<<13)
#define CART_UNCLAIMED 0xFFFFFFFF

CROM* CARTFACTORY(uint32_t mapper);

class CROM
{
protected:
   CROM(uint32_t mapper);
public:
   static inline CROM* CARTFACTORY() { return new CROM(0); }
   virtual ~CROM();

   // Priming interfaces (data setup/initialization)
   void ClearPRGBanks ()
   {
      m_numPrgBanks = 0;
   }
   void ClearCHRBanks ()
   {
      m_numChrBanks = 0;
   }
   void SetCHRBank ( int32_t bank, uint8_t* data );
   void SetPRGBank ( int32_t bank, uint8_t* data );
   void DoneLoadingBanks ( void );
   uint32_t NUMPRGROMBANKS ( void )
   {
      return m_numPrgBanks;
   }
   uint32_t NUMCHRROMBANKS ( void )
   {
      return m_numChrBanks;
   }

   // Operations
   bool IsWriteProtected ( void )
   {
      return (m_numChrBanks>0);
   }
   inline uint32_t PRGROMPHYSADDR ( uint32_t addr )
   {
      return (m_PRGROMmemory.physBankFromVirtAddr(addr)*m_PRGROMmemory.bankSize())+m_PRGROMmemory.offsetInBank(addr);
   }
   inline uint32_t PRGROM ( uint32_t addr )
   {
      return m_PRGROMmemory.MEM(addr);
   }
   inline void PRGROM ( uint32_t, uint8_t ) {}
   inline uint32_t PRGROMPHYS ( uint32_t addr )
   {
      return m_PRGROMmemory.MEMATPHYSADDR(addr);
   }
   uint32_t CHRMEMPHYSADDR ( uint32_t addr )
   {
      return (m_CHRmemory.physBankFromVirtAddr(addr)*m_CHRmemory.bankSize())+m_CHRmemory.offsetInBank(addr);
   }
   inline void CHRMEM ( uint32_t addr, uint8_t data )
   {
      m_CHRmemory.MEM(addr,data);
   }
   inline uint32_t CHRMEM ( uint32_t addr )
   {
      return m_CHRmemory.MEM(addr);
   }
   inline uint32_t SRAMPHYSADDR ( uint32_t addr )
   {
      return (m_SRAMmemory.physBankFromVirtAddr(addr)*m_SRAMmemory.bankSize())+m_SRAMmemory.offsetInBank(addr);
   }
   inline uint32_t SRAMVIRT ( uint32_t addr )
   {
      return m_SRAMmemory.MEM(addr);
   }
   inline void SRAMVIRT ( uint32_t addr, uint8_t data )
   {
      m_SRAMmemory.MEM(addr,data);
      m_SRAMdirty = true;
   }
   inline uint32_t SRAMPHYS ( uint32_t addr )
   {
      return m_SRAMmemory.MEMATPHYSADDR(addr);
   }
   inline void SRAMPHYS ( uint32_t addr, uint8_t data, bool setDirty = true )
   {
      m_SRAMmemory.MEMATPHYSADDR(addr,data);
      if ( setDirty )
      {
         m_SRAMdirty = true;
      }
   }
   inline void REMAPSRAM ( uint32_t virtAddr, uint8_t bank )
   {
      m_SRAMmemory.REMAP(m_SRAMmemory.virtBankFromVirtAddr(virtAddr),bank);
   }
   inline uint32_t EXRAMPHYSADDR ( uint32_t addr )
   {
      return m_EXRAMmemory.offsetInBank(addr);
   }
   inline uint8_t EXRAM ( uint32_t addr )
   {
      return m_EXRAMmemory.MEM(addr);
   }
   inline void EXRAM ( uint32_t addr, uint8_t data )
   {
      m_EXRAMmemory.MEM(addr,data);
   }
   inline uint32_t VRAM ( uint32_t addr )
   {
      if ( m_VRAMmemory.TOTALSIZE() && (addr&PPU_A13) )
      {
         return m_VRAMmemory.MEM(addr);
      }
      return CART_UNCLAIMED;
   }
   inline uint32_t VRAM ( uint32_t addr, uint8_t data )
   {
      if ( m_VRAMmemory.TOTALSIZE() && (addr&PPU_A13) )
      {
         m_VRAMmemory.MEM(addr,data);
      }
      return CART_UNCLAIMED;
   }
   inline void REMAPVRAM ( uint32_t bank, uint32_t newBank )
   {
      m_VRAMmemory.REMAP(bank,newBank);
   }

   // Mapper interfaces
   virtual void RESET ( bool soft );
   uint32_t MAPPER ( void )
   {
      return m_mapper;
   }
   virtual uint32_t HMAPPER ( uint32_t addr )
   {
      return PRGROM(addr);
   }
   virtual void HMAPPER ( uint32_t, uint8_t ) {}
   virtual uint32_t LMAPPER ( uint32_t addr );
   virtual void LMAPPER ( uint32_t addr, uint8_t data);
   virtual void SYNCPPU ( uint32_t, uint32_t ) {}
   virtual void SYNCCPU ( void ) {}
   virtual uint32_t DEBUGINFO ( uint32_t addr )
   {
      return PRGROM(addr);
   }
   virtual uint16_t AMPLITUDE ( void )
   {
      return 0; // soundless...
   }

   // Code/Data logger support functions
   inline CCodeDataLogger* LOGGERVIRT ( uint32_t addr )
   {
      return m_PRGROMmemory.LOGGER(addr);
   }
   inline CCodeDataLogger* SRAMLOGGERVIRT ( uint32_t addr )
   {
      return m_SRAMmemory.LOGGER(addr);
   }
   inline CCodeDataLogger* LOGGERPHYS ( uint32_t absAddr )
   {
      return m_PRGROMmemory.LOGGERATPHYSADDR(absAddr);
   }
   inline CCodeDataLogger* SRAMLOGGERPHYS ( uint32_t absAddr )
   {
      return m_SRAMmemory.LOGGERATPHYSADDR(absAddr);
   }
   inline CCodeDataLogger* EXRAMLOGGER ()
   {
      return m_EXRAMmemory.LOGGER(0);
   }

   // Support functions for inline disassembly in PRG-ROM, SRAM, and EXRAM
   inline void PRGROMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      m_PRGROMmemory.OPCODEMASK(addr,mask);
   }
   inline void PRGROMOPCODEMASKATPHYSADDR ( uint32_t absAddr, uint8_t mask )
   {
      m_PRGROMmemory.OPCODEMASKATPHYSADDR(absAddr,mask);
   }
   inline void PRGROMOPCODEMASKCLR ( void )
   {
      m_PRGROMmemory.OPCODEMASKCLR();
   }
   inline char* PRGROMDISASSEMBLY ( uint32_t addr )
   {
      return m_PRGROMmemory.DISASSEMBLY(addr);
   }
   inline char* PRGROMDISASSEMBLYATPHYSADDR ( uint32_t physAddr, char* buffer )
   {
      return m_PRGROMmemory.DISASSEMBLYATPHYSADDR(physAddr,buffer);
   }
   uint32_t PRGROMSLOC2VIRTADDR ( uint16_t sloc );
   uint16_t PRGROMADDR2SLOC ( uint32_t addr );
   inline uint32_t PRGROMSLOC ( uint32_t addr )
   {
      return m_PRGROMmemory.SLOC(addr);
   }
   inline void SRAMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      m_SRAMmemory.OPCODEMASK(addr,mask);
   }
   inline void SRAMOPCODEMASKCLR ( void )
   {
      m_SRAMmemory.OPCODEMASKCLR();
   }
   inline char* SRAMDISASSEMBLY ( uint32_t addr )
   {
      return m_SRAMmemory.DISASSEMBLY(addr);
   }
   uint32_t SRAMSLOC2VIRTADDR ( uint16_t sloc );
   uint16_t SRAMADDR2SLOC ( uint32_t addr );
   inline uint32_t SRAMSLOC ( uint32_t addr)
   {
      return m_SRAMmemory.SLOC(addr);
   }
   bool SRAMDIRTY() { return m_SRAMdirty; }
   inline void EXRAMOPCODEMASK ( uint32_t addr, uint8_t mask )
   {
      m_EXRAMmemory.OPCODEMASK(addr,mask);
   }
   inline void EXRAMOPCODEMASKCLR ( void )
   {
      m_EXRAMmemory.OPCODEMASKCLR();
   }
   inline char* EXRAMDISASSEMBLY ( uint32_t addr )
   {
      return m_EXRAMmemory.DISASSEMBLY(addr);
   }
   uint32_t EXRAMSLOC2VIRTADDR ( uint16_t sloc );
   uint16_t EXRAMADDR2SLOC ( uint32_t addr );
   inline uint32_t EXRAMSLOC ()
   {
      return m_EXRAMmemory.SLOC(0);
   }
   void DISASSEMBLE ();
   void PRINTABLEADDR ( char* buffer, uint32_t addr );
   void PRINTABLEADDR ( char* buffer, uint32_t addr, uint32_t absAddr );

   CMEMORY* VRAMMEMORY() { return &m_VRAMmemory; }

protected:
   CMEMORY      m_PRGROMmemory;
   CMEMORY      m_CHRmemory;
   CMEMORY      m_SRAMmemory;
   bool         m_SRAMdirty;
   CMEMORY      m_EXRAMmemory;
   CMEMORY      m_VRAMmemory;

   // Mapper stuff...
   uint32_t m_mapper;
   uint32_t m_numPrgBanks;
   uint32_t m_numChrBanks;
};

#endif
