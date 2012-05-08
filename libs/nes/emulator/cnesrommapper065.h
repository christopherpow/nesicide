#if !defined ( ROM_MAPPER065_H )
#define ROM_MAPPER065_H

#include "cnesrom.h"

class CROMMapper065 : public CROM
{
public:
   CROMMapper065();
   virtual ~CROMMapper065();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // Irem H-3001
   static uint8_t  m_reg [ 17 ];
   static uint8_t  m_irqCounter;
   static bool           m_irqEnable;
};

#endif
