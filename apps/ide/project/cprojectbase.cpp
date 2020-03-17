#include "cprojectbase.h"

#include "main.h"

CProjectBase::CProjectBase()
{
   m_name = "";
   m_path = "";
   m_editor = NULL;
   m_deleted = false;
   m_includeInBuild = true;
}

CProjectBase::~CProjectBase()
{
   if ( m_editor )
   {
      delete m_editor;
   }
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

void CProjectBase::closeItemEvent()
{
   if ( m_editor )
   {
      delete m_editor;
   }
   m_editor = NULL;
}
