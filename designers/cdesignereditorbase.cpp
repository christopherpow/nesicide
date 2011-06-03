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

bool CDesignerEditorBase::onSaveQuery()
{
   if ( isModified() )
   {
      return (QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed without saving.\n"
                                            "Do you want to save it?"),
                                    QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
   }
   else
   {
      return false;
   }
}

void CDesignerEditorBase::onSave()
{
   if ( treeLink )
   {
      treeLink->saveItemEvent();
   }
   setModified(false);
}

void CDesignerEditorBase::keyPressEvent(QKeyEvent *e)
{
   if ( (e->modifiers() == Qt::ControlModifier) &&
        (e->key() == Qt::Key_S) )
   {
      onSave();
   }
}
