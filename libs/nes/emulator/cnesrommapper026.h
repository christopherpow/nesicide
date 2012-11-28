#if !defined ( ROM_MAPPER026_H )
#define ROM_MAPPER026_H

#include "cnesrom.h"

class CROMMapper026 : public CROM
{
public:
   CROMMapper026();
   virtual ~CROMMapper026();

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
