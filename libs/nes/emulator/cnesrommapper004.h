#if !defined ( ROM_MAPPER004_H )
#define ROM_MAPPER004_H

#include "cnesrom.h"

class CROMMapper004 : public CROM
{
public:
   CROMMapper004();
   virtual ~CROMMapper004();

   static void RESET ( bool soft );
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t IRQENABLED ( void )
   {
      return m_irqEnable;
   }
   static uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   static uint32_t IRQRELOAD ( void )
   {
      return m_irqReload;
   }
   static uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   static uint32_t IRQLATCH ( void )
   {
      return m_irqLatch;
   }
   static uint32_t PPUADDRA12 ( void )
   {
      return m_lastPPUAddrA12;
   }
   static uint32_t PPUCYCLE ( void )
   {
      return m_lastPPUCycle;
   }

protected:
   // MMC3
   static uint8_t  m_reg [ 8 ];
   static bool           m_irqAsserted;
   static uint8_t  m_irqCounter;
   static uint8_t  m_irqLatch;
   static bool           m_irqEnable;
   static bool           m_irqReload;
   static uint8_t  m_prg [ 2 ];
   static uint8_t  m_chr [ 8 ];

   static uint32_t   m_lastPPUAddrA12;
   static uint32_t   m_lastPPUCycle;
};

#endif
