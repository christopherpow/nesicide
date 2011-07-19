#if !defined ( ROM_MAPPER002_H )
#define ROM_MAPPER002_H

#include "dbg_cnesrom.h"

class CROMMapper002 : public CROM
{
public:
   CROMMapper002();
   virtual ~CROMMapper002();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static uint8_t  m_reg;
};

#endif
