#include "cdesignereditorbase.h"
#include "cdockwidgetregistry.h"

#include <QMessageBox>

CDesignerEditorBase::CDesignerEditorBase(IProjectTreeViewItem* link,QWidget *parent) :
    QMainWindow(parent)
{
   QWidget* searchBar = CDockWidgetRegistry::getInstance()->getWidget("Search Bar");
   QWidget* search = CDockWidgetRegistry::getInstance()->getWidget("Search");

   InitTabItem(link);

   QObject::connect ( searchBar, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
   QObject::connect ( searchBar, SIGNAL(replaceText(QString,QString,bool)), this, SLOT(replaceText(QString,QString,bool)));
   QObject::connect ( search, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
   QObject::connect ( this, SIGNAL(activateSearchBar(QString)), searchBar, SLOT(activateMe(QString)) );
   QObject::connect ( this, SIGNAL(setSearchBarHint(bool)), searchBar, SLOT(hintMe(bool)) );

   setModified(false);
}

QMessageBox::StandardButton CDesignerEditorBase::onCloseQuery()
{
   if ( isModified() )
   {
      return QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed without saving.\n"
                                            "Do you want to save it?"),
                                    QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
   }
   else
   {
      return QMessageBox::Yes;
   }
}

void CDesignerEditorBase::onClose()
{
   if ( _treeLink )
   {
      _treeLink->closeItemEvent();
   }
}

void CDesignerEditorBase::onSave()
{
   if ( _treeLink )
   {
      _treeLink->saveItemEvent();
      emit applyChanges(_treeLink->uuid());
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

QMenu& CDesignerEditorBase::editorMenu()
{
   QList<QAction*> actions;
   QString str;

   if ( _treeLink )
   {
      str.sprintf("Undo '%s'",_treeLink->caption().toLatin1().constData());
   }
   actions.append(m_undoStack.createUndoAction(this,str));
   if ( _treeLink )
   {
      str.sprintf("Redo '%s'",_treeLink->caption().toLatin1().constData());
   }
   actions.append(m_undoStack.createRedoAction(this,str));

   m_menu.clear();
   m_menu.addActions(actions);

   return m_menu;
}
