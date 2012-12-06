#if !defined ( ROM_MAPPER018_H )
#define ROM_MAPPER018_H

#include "cnesrom.h"

class CROMMapper018 : public CROM
{
public:
   CROMMapper018();
   ~CROMMapper018();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg [ 29 ];
   static uint8_t  m_prg [ 3 ];
   static uint8_t  m_chr [ 8 ];
   static uint16_t m_irqReload;
   static uint16_t m_irqCounter;
   static bool     m_irqEnabled;
};

#endif
