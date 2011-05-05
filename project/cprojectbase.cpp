#include "cprojectbase.h"

#include "main.h"

CProjectBase::CProjectBase()
{
   m_name = "";
   m_path = "";
}

IProjectTreeViewItem* findProjectItem(QString uuid)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject());

   while ( iter.current() != NULL )
   {
      IProjectTreeViewItem* pItem = iter.current();

      if ( pItem->uuid() == uuid )
      {
         return pItem;
      }

      iter.next();
   }

   return NULL;
}

QString CProjectBase::absolutePath()
{
   QDir sourceDir(nesicideProject->getProjectSourceBasePath());

   return sourceDir.toNativeSeparators(sourceDir.absoluteFilePath(m_path));
}
