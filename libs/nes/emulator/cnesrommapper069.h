#if !defined ( ROM_MAPPER069_H )
#define ROM_MAPPER069_H

#include "cnesrom.h"

class CROMMapper069 : public CROM
{
private:
   CROMMapper069();
public:
   static inline CROMMapper069* CARTFACTORY() { return new CROMMapper069(); }
   virtual ~CROMMapper069();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t LMAPPER ( uint32_t addr );
   void LMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t IRQENABLED ( void )
   {
      return m_irqEnable;
   }
   uint32_t IRQCOUNTENABLED ( void )
   {
      return m_irqCountEnable;
   }
   uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   uint32_t SRAMENABLED ( void )
   {
      return m_sramAreaEnabled;
   }
   uint32_t ISSRAM ( void )
   {
      return m_sramAreaIsSram;
   }
   uint32_t SRAMPRGBANK ( void )
   {
      return m_prg[0];
   }
   uint32_t REGSELECTED ( void )
   {
      return m_reg[0]&0x0F;
   }
   uint32_t REGVALUE ( void )
   {
      return m_subReg[m_reg[0]&0x0F];
   }

protected:
   // MMC3
   uint8_t  m_reg [ 4 ];
   uint8_t  m_subReg [ 16 ];
   bool           m_irqAsserted;
   uint16_t  m_irqCounter;
   bool           m_irqEnable;
   bool           m_irqCountEnable;
   uint8_t  m_prg [ 4 ];
   uint8_t  m_chr [ 8 ];
   bool m_sramAreaIsSram;
   bool m_sramAreaEnabled;
};

#endif
