#if !defined ( ROM_MAPPER023_H )
#define ROM_MAPPER023_H

#include "cnesrom.h"

class CROMMapper023 : public CROM
{
public:
   CROMMapper023();
   ~CROMMapper023();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2+VRC4
   static uint8_t  m_reg [ 23 ];
   static uint8_t  m_chr [ 8 ];
   static uint8_t  m_irqReload;
   static uint8_t  m_irqCounter;
   static uint8_t  m_irqPrescaler;
   static uint8_t  m_irqPrescalerPhase;
   static bool     m_irqEnabled;
};

#endif
