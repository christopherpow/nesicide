#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "ccc65interface.h"

#include "main.h"

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{
   buildTextLogger->write("<b>Building PRG-ROM Banks:</b>");

   // External CC65 in PATH
   return CCC65Interface::assemble();
}
