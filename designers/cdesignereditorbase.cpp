#include "cdesignereditorbase.h"

#include <QMessageBox>

CDesignerEditorBase::CDesignerEditorBase(IProjectTreeViewItem* link,QWidget *parent) :
    QWidget(parent)
{
   InitTabItem(link);
   setModified(false);
}

bool CDesignerEditorBase::onCloseQuery()
{
   if ( isModified() )
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

void CDesignerEditorBase::onClose()
{
   if ( treeLink )
   {
      treeLink->closeItemEvent();
   }
}

bool CDesignerEditorBase::isDocumentSaveable()
{
   bool saveMe = false;
   if ( isModified() )
   {
      saveMe = true;
   }
   return saveMe;
}

void CDesignerEditorBase::onSaveDocument()
{
   if ( treeLink )
   {
      treeLink->saveItemEvent();
   }
   setModified(false);
}
