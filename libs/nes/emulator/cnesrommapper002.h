#if !defined ( ROM_MAPPER002_H )
#define ROM_MAPPER002_H

#include "cnesrom.h"

class CROMMapper002 : public CROM
{
private:
   CROMMapper002();
public:
   static inline CROMMapper002* CARTFACTORY() { return new CROMMapper002(); }
   virtual ~CROMMapper002();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   uint8_t  m_reg;
};

#endif
