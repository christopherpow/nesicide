#if !defined ( ROM_MAPPER033_H )
#define ROM_MAPPER033_H

#include "cnesrom.h"

class CROMMapper033 : public CROM
{
public:
   CROMMapper033();
   virtual ~CROMMapper033();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg[8];
};

#endif
