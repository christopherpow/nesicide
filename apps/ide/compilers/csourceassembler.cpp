#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "ccc65interface.h"

CSourceAssembler::CSourceAssembler()
{
}

void CSourceAssembler::clean()
{
   CCC65Interface::instance()->clean();
}

bool CSourceAssembler::assemble()
{
   IProjectTreeViewItemIterator iter(CNesicideProject::instance());
   IProjectTreeViewItem*        item;
   bool ok = true;

   // Go through project elements invoking their exporters to prepare
   // the environment with everything it needs to build the project.
   while ( iter.current() )
   {
      item = iter.current();

      ok = item->exportData();
      if ( !ok )
      {
         buildTextLogger->write("Failed to export '"+item->caption()+"'.");
         break;
      }

      // Go to next item.
      iter.next();
   }

   if ( ok )
   {
      ok = CCC65Interface::instance()->assemble();
   }

   return ok;
}
