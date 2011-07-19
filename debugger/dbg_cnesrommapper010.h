#if !defined ( ROM_MAPPER010_H )
#define ROM_MAPPER010_H

#include "dbg_cnesrom.h"

class CROMMapper010 : public CROM
{
public:
   CROMMapper010();
   virtual ~CROMMapper010();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void LATCH ( uint32_t addr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // MMC3
   static uint8_t  m_reg [ 6 ];
   static uint8_t  m_latch0;
   static uint8_t  m_latch1;
   static uint8_t  m_latch0FD;
   static uint8_t  m_latch0FE;
   static uint8_t  m_latch1FD;
   static uint8_t  m_latch1FE;
};

#endif
