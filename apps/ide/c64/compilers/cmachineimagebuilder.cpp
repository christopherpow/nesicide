#include "cmachineimagebuilder.h"

#include "ccc65interface.h"
#include "cnesicideproject.h"
#include "main.h"

CMachineImageBuilder::CMachineImageBuilder()
{
}

void CMachineImageBuilder::clean()
{
   CSourceAssembler sourceAssembler;

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::instance()->getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getAssemblerSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getCustomSourcesFromProject().count()) )
   {
      sourceAssembler.clean();
   }
   else
   {
      buildTextLogger->write("<font color='red'>Nothing to clean.</font>");
   }

   buildTextLogger->write("<b>Build completed successfully.</b>");
}

bool CMachineImageBuilder::build()
{
   CSourceAssembler sourceAssembler;
   bool ok;

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::instance()->getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getAssemblerSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getCustomSourcesFromProject().count()) )
   {
      if ( !nesicideProject->getLinkerConfigFile().isEmpty() )
      {
         ok = sourceAssembler.assemble();
         if ( !ok )
         {
            buildTextLogger->write("<font color='red'><b>Build failed.</b></font>");
            return false;
         }
      }
      else
      {
         buildTextLogger->write("<font color='red'>Nothing to build, no linker config file specified.  Go to Project menu, select Project Properties, and switch to the Linker tab to add one.</font>");
      }
   }
   else
   {
      buildTextLogger->write("<font color='red'>Nothing to build.</font>");
   }
   buildTextLogger->write("<b>Build completed successfully.</b>");
   return true;
}
