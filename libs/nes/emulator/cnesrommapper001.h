#if !defined ( ROM_MAPPER001_H )
#define ROM_MAPPER001_H

#include "cnesrom.h"

class CROMMapper001 : public CROM
{
public:
   CROMMapper001();
   virtual ~CROMMapper001();

   static void RESET ( bool soft );
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t SHIFTREGISTER ( void )
   {
      return m_sr;
   }
   static uint32_t SHIFTREGISTERBIT ( void )
   {
      return m_srCount;
   }

protected:
   // MMC1
   static uint8_t  m_reg [ 4 ];
   static uint8_t  m_regdef [ 4 ];
   static uint8_t  m_sr;
   static uint8_t  m_sel;
   static uint8_t  m_srCount;
   static uint32_t m_cpuCycleOfLastWrite;
   static uint32_t m_cpuCycle;
};

#endif
