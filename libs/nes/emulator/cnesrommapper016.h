#if !defined ( ROM_MAPPER016_H )
#define ROM_MAPPER016_H

#include "cnesrom.h"

class CROMMapper016 : public CROM
{
public:
   CROMMapper016();
   ~CROMMapper016();

   static void RESET016 ( bool soft );
   static void RESET159 ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t IRQENABLED ( void )
   {
      return m_irqEnabled;
   }
   static uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   static uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   static uint32_t EEPROMSTATE ( void )
   {
      return m_eepromState;
   }
   static uint32_t EEPROMBITCOUNTER ( void )
   {
      return m_eepromBitCounter;
   }
   static uint32_t EEPROMCMD ( void )
   {
      return m_eepromCmd;
   }
   static uint32_t EEPROMADDR ( void )
   {
      return m_eepromAddr;
   }
   static uint32_t EEPROMDATABUF ( void )
   {
      return m_eepromDataBuf;
   }

protected:
   static uint8_t  m_reg [ 14 ];
   static uint16_t m_irqCounter;
   static bool     m_irqEnabled;
   static bool     m_irqAsserted;
   static uint8_t  m_eepromBitCounter;
   static uint8_t  m_eepromState;
   static uint8_t  m_eepromCmd;
   static uint8_t  m_eepromAddr;
   static uint8_t  m_eepromDataBuf;
   static uint8_t  m_eepromRWBit;
   // We'll use the SRAM interface in CROM object to store
   // the EEPROM data so that it gets saved using the regular
   // save-state mechanics.
};

#endif
