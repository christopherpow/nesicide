#if !defined ( ROM_MAPPER034_H )
#define ROM_MAPPER034_H

#include "cnesrom.h"

class CROMMapper034 : public CROM
{
private:
   CROMMapper034();
public:
   static inline CROMMapper034* CARTFACTORY() { return new CROMMapper034(); }
   virtual ~CROMMapper034();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t LMAPPER ( uint32_t addr );
   void LMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg[4];
};

#endif
