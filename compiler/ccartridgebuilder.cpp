#include "ccartridgebuilder.h"

#include "cnesicideproject.h"
#include "main.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}


void CCartridgeBuilder::build()
{
   emulator->pauseEmulation(false);

   buildTextLogger.clear();
   buildTextLogger.write("<b>Project build started.</b>");
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;

   if (!(sourceAssembler.assemble() && graphicsAssembler.assemble()))
   {
      buildTextLogger.write("<font color='red'><b>Build failed.</b></font>");
      return;
   }
   buildTextLogger.write("<b>Build completed successfully.</b>");

   emulator->primeEmulator();
   emulator->resetEmulator();
}
