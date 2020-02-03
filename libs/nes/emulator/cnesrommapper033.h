#if !defined ( ROM_MAPPER033_H )
#define ROM_MAPPER033_H

#include "cnesrom.h"

class CROMMapper033 : public CROM
{
private:
   CROMMapper033();
public:
   static inline CROMMapper033* CARTFACTORY() { return new CROMMapper033(); }
   ~CROMMapper033();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg[8];
};

#endif
