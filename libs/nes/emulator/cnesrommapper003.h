#if !defined ( ROM_MAPPER003_H )
#define ROM_MAPPER003_H

#include "cnesrom.h"

class CROMMapper003 : public CROM
{
public:
   CROMMapper003();
   virtual ~CROMMapper003();

   static void RESET ( bool soft );
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static uint8_t  m_reg;
};

#endif
