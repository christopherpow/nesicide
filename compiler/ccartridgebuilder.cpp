#include "ccartridgebuilder.h"

#include "cnesicideproject.h"
#include "main.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}

void CCartridgeBuilder::build()
{
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;

   buildTextLogger.write("<b>Project build started.</b>");

   if (!(sourceAssembler.assemble() && graphicsAssembler.assemble()))
   {
      buildTextLogger.write("<font color='red'><b>Build failed.</b></font>");
      return;
   }
   buildTextLogger.write("<b>Build completed successfully.</b>");
}
