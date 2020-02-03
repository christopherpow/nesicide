#if !defined ( ROM_MAPPER003_H )
#define ROM_MAPPER003_H

#include "cnesrom.h"

class CROMMapper003 : public CROM
{
private:
   CROMMapper003();
public:
   static inline CROMMapper003* CARTFACTORY() { return new CROMMapper003(); }
   ~CROMMapper003();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg;
};

#endif
