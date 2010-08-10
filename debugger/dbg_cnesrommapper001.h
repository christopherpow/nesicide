#if !defined ( ROM_MAPPER001_H )
#define ROM_MAPPER001_H

#include "emulator_core.h"

class CROMMapper001DBG
{
public:
   CROMMapper001DBG();
   virtual ~CROMMapper001DBG();

// CPTODO: REIMPLEMENT!!
   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t SHIFTREGISTER ( void ) { return 0; }
   static uint32_t SHIFTREGISTERBIT ( void ) { return 0; }
};

#endif
