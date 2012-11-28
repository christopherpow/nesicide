#if !defined ( ROM_MAPPER069_H )
#define ROM_MAPPER069_H

#include "cnesrom.h"

class CROMMapper069 : public CROM
{
public:
   CROMMapper069();
   virtual ~CROMMapper069();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t IRQENABLED ( void )
   {
      return m_irqEnable;
   }
   static uint32_t IRQCOUNTENABLED ( void )
   {
      return m_irqCountEnable;
   }
   static uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   static uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   static uint32_t SRAMENABLED ( void )
   {
      return m_sramAreaEnabled;
   }
   static uint32_t ISSRAM ( void )
   {
      return m_sramAreaIsSram;
   }
   static uint32_t SRAMPRGBANK ( void )
   {
      return m_prg[0];
   }
   static uint32_t REGSELECTED ( void )
   {
      return m_reg[0]&0x0F;
   }
   static uint32_t REGVALUE ( void )
   {
      return m_subReg[m_reg[0]&0x0F];
   }

protected:
   // MMC3
   static uint8_t  m_reg [ 4 ];
   static uint8_t  m_subReg [ 16 ];
   static bool           m_irqAsserted;
   static uint16_t  m_irqCounter;
   static bool           m_irqEnable;
   static bool           m_irqCountEnable;
   static uint8_t  m_prg [ 4 ];
   static uint8_t  m_chr [ 8 ];
   static bool m_sramAreaIsSram;
   static bool m_sramAreaEnabled;
};

#endif
