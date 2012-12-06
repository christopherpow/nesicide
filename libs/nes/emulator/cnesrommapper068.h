#if !defined ( ROM_MAPPER068_H )
#define ROM_MAPPER068_H

#include "cnesrom.h"

class CROMMapper068 : public CROM
{
public:
   CROMMapper068();
   ~CROMMapper068();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // Sunsoft Mapper #4
   static uint8_t  m_reg [ 8 ];
};

#endif
