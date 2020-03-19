#if !defined ( ROM_MAPPER022_H )
#define ROM_MAPPER022_H

#include "cnesrom.h"

class CROMMapper022 : public CROM
{
private:
   CROMMapper022();
public:
   static inline CROMMapper022* CARTFACTORY() { return new CROMMapper022(); }
   virtual ~CROMMapper022();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC2
   uint8_t  m_reg [ 19 ];
   uint8_t  m_chr [ 8 ];
};

#endif
