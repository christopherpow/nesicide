#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "cpasminterface.h"
#include "ccc65interface.h"

#include "main.h"

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{   
   if ( nesicideProject->getCompilerToolchain() == compilers[0] )
   {
      // Internal PASM
      CPASMInterface pasm;
      
      return pasm.assemble();
   }
   else if ( nesicideProject->getCompilerToolchain() == compilers[1] )
   {
      // External CC65 in PATH
      CCC65Interface cc65;
      
      return cc65.assemble();
   }
}
