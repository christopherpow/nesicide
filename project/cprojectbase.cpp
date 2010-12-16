#include "cprojectbase.h"

#include "main.h"

CProjectBase::CProjectBase()
{
}

IProjectTreeViewItem* findProjectItem(QString uuid)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject());

   while ( iter.current() != NULL )
   {
      IProjectTreeViewItem* pItem = iter.current();

      if ( pItem->getIdent() == uuid )
      {
         return pItem;
      }

      iter.next();
   }

   return NULL;
}
