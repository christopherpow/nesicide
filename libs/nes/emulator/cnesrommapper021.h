#if !defined ( ROM_MAPPER021_H )
#define ROM_MAPPER021_H

#include "cnesrom.h"

class CROMMapper021 : public CROM
{
public:
   CROMMapper021();
   virtual ~CROMMapper021();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
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
