#if !defined ( ROM_MAPPER023_H )
#define ROM_MAPPER023_H

#include "cnesrom.h"

class CROMMapper023 : public CROM
{
public:
   CROMMapper023();
   virtual ~CROMMapper023();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2
   static uint8_t  m_reg [ 19 ];
   static uint8_t  m_chr [ 8 ];
};

#endif
