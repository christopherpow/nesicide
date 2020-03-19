#if !defined ( ROM_MAPPER004_H )
#define ROM_MAPPER004_H

#include "cnesrom.h"

class CROMMapper004 : public CROM
{
private:
   CROMMapper004();
public:
   static inline CROMMapper004* CARTFACTORY() { return new CROMMapper004(); }
   ~CROMMapper004();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t IRQENABLED ( void )
   {
      return m_irqEnable;
   }
   uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   uint32_t IRQRELOAD ( void )
   {
      return m_irqReload;
   }
   uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   uint32_t IRQLATCH ( void )
   {
      return m_irqLatch;
   }
   uint32_t PPUADDRA12 ( void )
   {
      return m_lastPPUAddrA12;
   }
   uint32_t PPUCYCLE ( void )
   {
      return m_lastPPUCycle;
   }

protected:
   // MMC3
   uint8_t  m_reg [ 8 ];
   bool           m_irqAsserted;
   uint8_t  m_irqCounter;
   uint8_t  m_irqLatch;
   bool           m_irqEnable;
   bool           m_irqReload;
   uint8_t  m_prg [ 2 ];
   uint8_t  m_chr [ 6 ];

   uint32_t   m_lastPPUAddrA12;
   uint32_t   m_lastPPUCycle;
};

#endif
