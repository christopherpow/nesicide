#if !defined ( ROM_MAPPER010_H )
#define ROM_MAPPER010_H

#include "cnesrom.h"

class CROMMapper010 : public CROM
{
public:
   CROMMapper010();
   virtual ~CROMMapper010();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t LATCH0FD ( void )
   {
      return m_latch0FD;
   }
   static uint32_t LATCH0FE ( void )
   {
      return m_latch0FE;
   }
   static uint32_t LATCH1FD ( void )
   {
      return m_latch1FD;
   }
   static uint32_t LATCH1FE ( void )
   {
      return m_latch1FE;
   }
   static uint32_t LATCH0 ( void )
   {
      return m_latch0;
   }
   static uint32_t LATCH1 ( void )
   {
      return m_latch0;
   }

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
