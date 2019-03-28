#if !defined ( ROM_MAPPER075_H )
#define ROM_MAPPER075_H

#include "cnesrom.h"

class CROMMapper075 : public CROM
{
private:
   CROMMapper075();
public:
   static inline CROMMapper075* CARTFACTORY() { return new CROMMapper075(); }
   ~CROMMapper075();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC1
   uint8_t  m_reg [ 6 ];
   uint8_t  m_prg [ 4 ];
   uint8_t  m_chr [ 8 ];
};

#endif
