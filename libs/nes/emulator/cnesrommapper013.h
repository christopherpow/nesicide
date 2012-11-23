#if !defined ( ROM_MAPPER013_H )
#define ROM_MAPPER013_H

#include "cnesrom.h"

class CROMMapper013 : public CROM
{
public:
   CROMMapper013();
   virtual ~CROMMapper013();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg;
};

#endif
