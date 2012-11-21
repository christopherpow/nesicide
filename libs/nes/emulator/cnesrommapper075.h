#if !defined ( ROM_MAPPER075_H )
#define ROM_MAPPER075_H

#include "cnesrom.h"

class CROMMapper075 : public CROM
{
public:
   CROMMapper075();
   virtual ~CROMMapper075();

   static void RESET ( bool soft );
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // VRC1
   static uint8_t  m_reg [ 6 ];
   static uint8_t  m_prg [ 4 ];
   static uint8_t  m_chr [ 8 ];
};

#endif
