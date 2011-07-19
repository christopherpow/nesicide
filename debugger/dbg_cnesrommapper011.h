#if !defined ( ROM_MAPPER011_H )
#define ROM_MAPPER011_H

#include "dbg_cnesrom.h"

class CROMMapper011 : public CROM
{
public:
   CROMMapper011();
   virtual ~CROMMapper011();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static uint8_t m_reg;
};

#endif
