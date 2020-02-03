#if !defined ( ROM_MAPPER013_H )
#define ROM_MAPPER013_H

#include "cnesrom.h"

class CROMMapper013 : public CROM
{
private:
   CROMMapper013();
public:
   static inline CROMMapper013* CARTFACTORY() { return new CROMMapper013(); }
   ~CROMMapper013();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg;
};

#endif
