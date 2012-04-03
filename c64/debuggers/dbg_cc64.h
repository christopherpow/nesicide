#if !defined ( C64_H )
#define C64_H

#include "cbreakpointinfo.h"

#include "c64_emulator_core.h"

// Identifiers of tooltip info to provide.
#define TOOLTIP_BYTES 0
#define TOOLTIP_INFO  1

// The CC64DBG class is a container for all debug elements
// relevant to the C64 overall.
class CC64DBG
{
public:
   CC64DBG();
   virtual ~CC64DBG();

   // This method retrieves information to be displayed in ToolTips
   // for the Code Browser window.
   static void CODEBROWSERTOOLTIP ( int32_t tipType, uint32_t addr, char* tooltipBuffer );
};

#endif
