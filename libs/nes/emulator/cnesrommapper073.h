#if !defined ( ROM_MAPPER073_H )
#define ROM_MAPPER073_H

#include "cnesrom.h"

class CROMMapper073 : public CROM
{
private:
   CROMMapper073();
public:
   static inline CROMMapper073* CARTFACTORY() { return new CROMMapper073(); }
   virtual ~CROMMapper073();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC3
   uint8_t  m_reg [ 7 ];
   uint16_t m_irqReload;
   uint16_t m_irqCounter;
   bool     m_irqEnabled;
};

#endif
