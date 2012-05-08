#if !defined ( ROM_MAPPER033_H )
#define ROM_MAPPER033_H

#include "cnesrom.h"

class CROMMapper033 : public CROM
{
public:
   CROMMapper033();
   virtual ~CROMMapper033();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static uint8_t  m_reg[8];
};

#endif
