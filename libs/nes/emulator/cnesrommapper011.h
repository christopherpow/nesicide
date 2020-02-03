#if !defined ( ROM_MAPPER011_H )
#define ROM_MAPPER011_H

#include "cnesrom.h"

class CROMMapper011 : public CROM
{
private:
   CROMMapper011();
public:
   static inline CROMMapper011* CARTFACTORY() { return new CROMMapper011(); }
   ~CROMMapper011();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t m_reg;
};

#endif
