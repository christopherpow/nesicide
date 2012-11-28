#if !defined ( ROM_MAPPER065_H )
#define ROM_MAPPER065_H

#include "cnesrom.h"

class CROMMapper065 : public CROM
{
public:
   CROMMapper065();
   virtual ~CROMMapper065();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // Irem H-3001
   static uint8_t  m_reg [ 16 ];
   static uint8_t  m_irqCounter;
   static bool           m_irqEnable;
   static uint16_t m_irqReload;
};

#endif
