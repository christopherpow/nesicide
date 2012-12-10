#if !defined ( ROM_MAPPER073_H )
#define ROM_MAPPER073_H

#include "cnesrom.h"

class CROMMapper073 : public CROM
{
public:
   CROMMapper073();
   ~CROMMapper073();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC3
   static uint8_t  m_reg [ 7 ];
   static uint16_t m_irqReload;
   static uint16_t m_irqCounter;
   static bool     m_irqEnabled;
};

#endif
