#if !defined ( ROM_MAPPER068_H )
#define ROM_MAPPER068_H

#include "cnesrom.h"

class CROMMapper068 : public CROM
{
private:
   CROMMapper068();
public:
   static inline CROMMapper068* CARTFACTORY() { return new CROMMapper068(); }
   virtual ~CROMMapper068();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // Sunsoft Mapper #4
   uint8_t  m_reg [ 8 ];
};

#endif
