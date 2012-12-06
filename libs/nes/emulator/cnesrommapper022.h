#if !defined ( ROM_MAPPER022_H )
#define ROM_MAPPER022_H

#include "cnesrom.h"

class CROMMapper022 : public CROM
{
public:
   CROMMapper022();
   ~CROMMapper022();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2
   static uint8_t  m_reg [ 19 ];
   static uint8_t  m_chr [ 8 ];
};

#endif
