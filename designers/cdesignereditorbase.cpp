#include "cdesignereditorbase.h"
#include "cdockwidgetregistry.h"

#include <QMessageBox>

CDesignerEditorBase::CDesignerEditorBase(IProjectTreeViewItem* link,QWidget *parent) :
    QWidget(parent)
{
   QWidget* searchBar = CDockWidgetRegistry::getWidget("Search Bar");
   QWidget* search = CDockWidgetRegistry::getWidget("Search");

   InitTabItem(link);

   QObject::connect ( searchBar, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
   QObject::connect ( search, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
   QObject::connect ( search, SIGNAL(replaceText(QString,QString,bool)), this, SLOT(replaceText(QString,QString,bool)));
   QObject::connect ( this, SIGNAL(activateSearchBar()), searchBar, SLOT(setFocus()) );

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
