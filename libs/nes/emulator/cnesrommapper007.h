#if !defined ( ROM_MAPPER007_H )
#define ROM_MAPPER007_H

#include "cnesrom.h"

class CROMMapper007 : public CROM
{
private:
   CROMMapper007();
public:
   static inline CROMMapper007* CARTFACTORY() { return new CROMMapper007(); }
   ~CROMMapper007();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg;
};

#endif
