#if !defined ( ROM_MAPPER065_H )
#define ROM_MAPPER065_H

#include "cnesrom.h"

class CROMMapper065 : public CROM
{
private:
   CROMMapper065();
public:
   static inline CROMMapper065* CARTFACTORY() { return new CROMMapper065(); }
   ~CROMMapper065();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

protected:
   // Irem H-3001
   uint8_t  m_reg [ 16 ];
   uint8_t  m_irqCounter;
   bool           m_irqEnable;
   uint16_t m_irqReload;
};

#endif
