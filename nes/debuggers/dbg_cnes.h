#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"

#include "nes_emulator_core.h"

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

   // This method retrieves information to be displayed in ToolTips
   // for the Code Browser window.
   static void CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer );
};

#endif
