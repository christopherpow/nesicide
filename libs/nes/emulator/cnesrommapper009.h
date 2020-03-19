#if !defined ( ROM_MAPPER009_H )
#define ROM_MAPPER009_H

#include "cnesrom.h"

class CROMMapper009 : public CROM
{
private:
   CROMMapper009();
public:
   static inline CROMMapper009* CARTFACTORY() { return new CROMMapper009(); }
   virtual ~CROMMapper009();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t LATCH0FD ( void )
   {
      return m_latch0FD;
   }
   uint32_t LATCH0FE ( void )
   {
      return m_latch0FE;
   }
   uint32_t LATCH1FD ( void )
   {
      return m_latch1FD;
   }
   uint32_t LATCH1FE ( void )
   {
      return m_latch1FE;
   }
   uint32_t LATCH0 ( void )
   {
      return m_latch0;
   }
   uint32_t LATCH1 ( void )
   {
      return m_latch0;
   }

protected:
   // MMC3
   uint8_t  m_reg [ 6 ];
   uint8_t  m_latch0;
   uint8_t  m_latch1;
   uint8_t  m_latch0FD;
   uint8_t  m_latch0FE;
   uint8_t  m_latch1FD;
   uint8_t  m_latch1FE;
};

#endif
