#if !defined ( ROM_MAPPER023_H )
#define ROM_MAPPER023_H

#include "cnesrom.h"

class CROMMapper023 : public CROM
{
private:
   CROMMapper023();
public:
   static inline CROMMapper023* CARTFACTORY() { return new CROMMapper023(); }
   ~CROMMapper023();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2+VRC4
   uint8_t  m_reg [ 23 ];
   uint8_t  m_chr [ 8 ];
   uint8_t  m_irqReload;
   uint8_t  m_irqCounter;
   uint8_t  m_irqPrescaler;
   uint8_t  m_irqPrescalerPhase;
   bool     m_irqEnabled;
};

#endif
