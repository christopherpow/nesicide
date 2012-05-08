#if !defined ( ROM_MAPPER034_H )
#define ROM_MAPPER034_H

#include "cnesrom.h"

class CROMMapper034 : public CROM
{
public:
   CROMMapper034();
   virtual ~CROMMapper034();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static uint8_t  m_reg[4];
};

#endif
