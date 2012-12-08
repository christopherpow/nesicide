#if !defined ( ROM_MAPPER025_H )
#define ROM_MAPPER025_H

#include "cnesrom.h"

class CROMMapper025 : public CROM
{
public:
   CROMMapper025();
   ~CROMMapper025();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2
   static uint8_t  m_reg [ 24 ];
   static uint8_t  m_chr [ 8 ];
   static uint8_t  m_irqReload;
   static uint8_t  m_irqCounter;
   static uint8_t  m_irqPrescaler;
   static uint8_t  m_irqPrescalerPhase;
   static bool     m_irqEnabled;
};

#endif
