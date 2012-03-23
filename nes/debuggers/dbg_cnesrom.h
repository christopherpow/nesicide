#if !defined ( ROM_H )
#define ROM_H

#include "nes_emulator_core.h"

#include "cregisterdata.h"
#include "ccodedatalogger.h"
#include "cbreakpointinfo.h"

class CROMDBG
{
public:
   CROMDBG();
   virtual ~CROMDBG();

   static void RESET ( void );
   static void RESET ( uint32_t mapper );
};

#endif
