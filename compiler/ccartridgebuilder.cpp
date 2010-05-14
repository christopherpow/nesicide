#include "ccartridgebuilder.h"

#include "cnesicideproject.h"
#include "main.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}


void CCartridgeBuilder::build()
{
   builderTextLogger.clear();
   builderTextLogger.write("<b>Project build started.</b>");
   CSourceAssembler sourceAssembler;

   if (!sourceAssembler.assemble())
   {
      builderTextLogger.write("<font color='red'><b>Build failed.</b></font>");
      return;
   }
   builderTextLogger.write("<b>Build completed successfully.</b>");

   emulator->pauseEmulation(false);
   emulator->primeEmulator();
   emulator->resetEmulator();
}
