#if !defined ( ROM_MAPPER004_H )
#define ROM_MAPPER004_H

#include "emulator_core.h"

class CROMMapper004DBG
{
public:
   CROMMapper004DBG();
   virtual ~CROMMapper004DBG();

   // Internal accessors for mapper information inspector...
   // Note: called directly!
// CPTODO: REIMPLEMENT!
   static uint32_t IRQENABLED ( void ) { return 0; }
   static uint32_t IRQASSERTED ( void ) { return 0; }
   static uint32_t IRQRELOAD ( void ) { return 0; }
   static uint32_t IRQCOUNTER ( void ) { return 0; }
   static uint32_t PPUADDRA12 ( void ) { return 0; }
   static uint32_t PPUCYCLE ( void ) { return 0; }
};

#endif
