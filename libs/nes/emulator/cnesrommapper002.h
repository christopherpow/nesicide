#if !defined ( ROM_MAPPER002_H )
#define ROM_MAPPER002_H

#include "cnesrom.h"

class CROMMapper002 : public CROM
{
public:
   CROMMapper002();
   ~CROMMapper002();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg;
};

#endif
