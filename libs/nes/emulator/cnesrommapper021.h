#if !defined ( ROM_MAPPER021_H )
#define ROM_MAPPER021_H

#include "cnesrom.h"

class CROMMapper021 : public CROM
{
private:
   CROMMapper021();
public:
   static inline CROMMapper021* CARTFACTORY() { return new CROMMapper021(); }
   ~CROMMapper021();

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
