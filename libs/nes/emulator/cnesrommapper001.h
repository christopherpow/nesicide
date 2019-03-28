#if !defined ( ROM_MAPPER001_H )
#define ROM_MAPPER001_H

#include "cnesrom.h"

class CROMMapper001 : public CROM
{
private:
   CROMMapper001();
public:
   static inline CROMMapper001* CARTFACTORY() { return new CROMMapper001(); }
   ~CROMMapper001();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t SHIFTREGISTER ( void )
   {
      return m_sr;
   }
   uint32_t SHIFTREGISTERBIT ( void )
   {
      return m_srCount;
   }

protected:
   // MMC1
   uint8_t  m_reg [ 4 ];
   uint8_t  m_regdef [ 4 ];
   uint8_t  m_sr;
   uint8_t  m_sel;
   uint8_t  m_srCount;
   uint32_t m_cpuCycleOfLastWrite;
   uint32_t m_cpuCycle;
};

#endif
