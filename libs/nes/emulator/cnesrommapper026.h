#if !defined ( ROM_MAPPER026_H )
#define ROM_MAPPER026_H

#include "cnesrommapper024.h"

class CROMMapper026 : public CROMMapper024
{
private:
   CROMMapper026();
public:
   static inline CROMMapper026* CARTFACTORY() { return new CROMMapper026(); }
   ~CROMMapper026();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC6
   uint8_t  m_reg [ 23 ];
   uint8_t  m_chr [ 8 ];
   uint8_t  m_irqReload;
   uint8_t  m_irqCounter;
   uint8_t  m_irqPrescaler;
   uint8_t  m_irqPrescalerPhase;
   bool     m_irqEnabled;
};

#endif
