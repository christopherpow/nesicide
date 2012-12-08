#if !defined ( ROM_MAPPER075_H )
#define ROM_MAPPER075_H

#include "cnesrom.h"

class CROMMapper075 : public CROM
{
public:
   CROMMapper075();
   ~CROMMapper075();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // VRC1
   static uint8_t  m_reg [ 6 ];
   static uint8_t  m_prg [ 4 ];
   static uint8_t  m_chr [ 8 ];
};

#endif
