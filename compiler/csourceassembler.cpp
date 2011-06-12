#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "ccc65interface.h"

#include "main.h"

CSourceAssembler::CSourceAssembler()
{
}

void CSourceAssembler::clean()
{
   return CCC65Interface::clean();
}

bool CSourceAssembler::assemble()
{
   // External CC65 in PATH
   return CCC65Interface::assemble();
}
