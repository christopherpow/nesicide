#if !defined ( ROM_MAPPER016_H )
#define ROM_MAPPER016_H

#include "cnesrom.h"

class CROMMapper016 : public CROM
{
public:
   CROMMapper016();
   ~CROMMapper016();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

protected:
   static uint8_t  m_reg [ 14 ];
   static uint16_t m_irqCounter;
   static bool     m_irqEnabled;
   static uint8_t  m_eepromBitCounter;
   static uint8_t  m_eepromState;
   static uint8_t  m_eepromCmd;
   static uint8_t  m_eepromAddr;
   // We'll use the SRAM interface in CROM object to store
   // the EEPROM data so that it gets saved using the regular
   // save-state mechanics.
};

#endif
