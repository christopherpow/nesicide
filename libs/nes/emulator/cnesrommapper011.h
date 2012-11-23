#if !defined ( ROM_MAPPER011_H )
#define ROM_MAPPER011_H

#include "cnesrom.h"

class CROMMapper011 : public CROM
{
public:
   CROMMapper011();
   virtual ~CROMMapper011();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t m_reg;
};

#endif
