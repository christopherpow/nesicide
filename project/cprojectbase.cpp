#include "cprojectbase.h"

#include "main.h"

CProjectBase::CProjectBase()
{
   m_name = "";
   m_path = "";
   m_isModified = false;
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

bool CProjectBase::onCloseQuery()
{
   if ( m_isModified )
   {
      return (QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed. Close anyway?"),
                                    QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes);
   }
   else
   {
      return true;
   }
}
