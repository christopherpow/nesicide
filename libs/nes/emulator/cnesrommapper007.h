#if !defined ( ROM_MAPPER007_H )
#define ROM_MAPPER007_H

#include "cnesrom.h"

class CROMMapper007 : public CROM
{
public:
   CROMMapper007();
   virtual ~CROMMapper007();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg;
};

#endif
