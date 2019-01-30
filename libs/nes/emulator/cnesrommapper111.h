#if !defined ( ROM_MAPPER111_H )
#define ROM_MAPPER111_H

#include "cnesrom.h"

class CROMMapper111 : public CROM
{
public:
   CROMMapper111();
   ~CROMMapper111();

   static void RESET ( bool soft );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg;
};

#endif
