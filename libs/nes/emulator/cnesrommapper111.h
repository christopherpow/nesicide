#if !defined ( ROM_MAPPER111_H )
#define ROM_MAPPER111_H

#include "cnesrom.h"

class CROMMapper111 : public CROM
{
private:
   CROMMapper111();
public:
   static inline CROMMapper111* CARTFACTORY() { return new CROMMapper111(); }
   virtual ~CROMMapper111();

   void RESET ( bool soft );
   void LMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg;
};

#endif
