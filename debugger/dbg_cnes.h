#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"

#include "emulator_core.h"

// Identifiers of tooltip info to provide.
#define TOOLTIP_BYTES 0
#define TOOLTIP_INFO  1

// The CNESDBG class is a container for all debug elements
// relevant to the NES overall.
class CNESDBG
{
public:
   CNESDBG();
   virtual ~CNESDBG();

   // This method clears the runtime disassembly cache.  It is used
   // whenever a new ROM image is loaded into the machine so as not
   // to confuse the user with previously disassembled code appearing
   // where new code should replace it.  This has the 'unfortunate' side
   // effect of forcing the disassembly to show all ".DB" entries for
   // each byte, but as the ROM is emulated the disassembly of actual
   // executed code fills in.
   static void CLEAROPCODEMASKS ( void );

   // This method retrieves information to be displayed in ToolTips
   // for the Code Browser window.
   static void CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer );
};

#endif
