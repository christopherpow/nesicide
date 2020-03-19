#if !defined ( ROM_MAPPER025_H )
#define ROM_MAPPER025_H

#include "cnesrom.h"

class CROMMapper025 : public CROM
{
private:
   CROMMapper025();
public:
   static inline CROMMapper025* CARTFACTORY() { return new CROMMapper025(); }
   virtual ~CROMMapper025();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2
   uint8_t  m_reg [ 24 ];
   uint8_t  m_chr [ 8 ];
   uint8_t  m_irqReload;
   uint8_t  m_irqCounter;
   uint8_t  m_irqPrescaler;
   uint8_t  m_irqPrescalerPhase;
   bool     m_irqEnabled;
};

#endif
