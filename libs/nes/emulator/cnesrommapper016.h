#if !defined ( ROM_MAPPER016_H )
#define ROM_MAPPER016_H

#include "cnesrom.h"

class CROMMapper016 : public CROM
{
private:
   CROMMapper016();
public:
   static inline CROMMapper016* CARTFACTORY() { return new CROMMapper016(); }
   ~CROMMapper016();

   void RESET016 ( bool soft );
   void RESET159 ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t LMAPPER ( uint32_t addr );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t IRQENABLED ( void )
   {
      return m_irqEnabled;
   }
   uint32_t IRQCOUNTER ( void )
   {
      return m_irqCounter;
   }
   uint32_t IRQASSERTED ( void )
   {
      return m_irqAsserted;
   }
   uint32_t EEPROMSTATE ( void )
   {
      return m_eepromState;
   }
   uint32_t EEPROMBITCOUNTER ( void )
   {
      return m_eepromBitCounter;
   }
   uint32_t EEPROMCMD ( void )
   {
      return m_eepromCmd;
   }
   uint32_t EEPROMADDR ( void )
   {
      return m_eepromAddr;
   }
   uint32_t EEPROMDATABUF ( void )
   {
      return m_eepromDataBuf;
   }

protected:
   uint8_t  m_reg [ 14 ];
   uint16_t m_irqCounter;
   bool     m_irqEnabled;
   bool     m_irqAsserted;
   uint8_t  m_eepromBitCounter;
   uint8_t  m_eepromState;
   uint8_t  m_eepromCmd;
   uint8_t  m_eepromAddr;
   uint8_t  m_eepromDataBuf;
   uint8_t  m_eepromRWBit;
   // We'll use the SRAM interface in CROM object to store
   // the EEPROM data so that it gets saved using the regular
   // save-state mechanics.
};

#endif
