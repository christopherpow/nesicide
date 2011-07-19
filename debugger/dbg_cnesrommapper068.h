#if !defined ( ROM_MAPPER068_H )
#define ROM_MAPPER068_H

#include "dbg_cnesrom.h"

class CROMMapper068 : public CROM
{
public:
   CROMMapper068();
   virtual ~CROMMapper068();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // Sunsoft Mapper #4
   static uint8_t  m_reg [ 4 ];
};

#endif
