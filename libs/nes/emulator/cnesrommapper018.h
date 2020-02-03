#if !defined ( ROM_MAPPER018_H )
#define ROM_MAPPER018_H

#include "cnesrom.h"

class CROMMapper018 : public CROM
{
private:
   CROMMapper018();
public:
   static inline CROMMapper018* CARTFACTORY() { return new CROMMapper018(); }
   ~CROMMapper018();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg [ 29 ];
   uint8_t  m_prg [ 3 ];
   uint8_t  m_chr [ 8 ];
   uint16_t m_irqReload;
   uint16_t m_irqCounter;
   bool     m_irqEnabled;
};

#endif
