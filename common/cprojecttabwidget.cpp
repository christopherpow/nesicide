#include "cprojecttabwidget.h"

#include "cdesignereditorbase.h"

#include "main.h"

CProjectTabWidget::CProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

int CProjectTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
   CDesignerEditorBase* editor = dynamic_cast<CDesignerEditorBase*>(widget);
   int tabIdx;

   if ( editor )
   {
      QObject::connect(editor,SIGNAL(editor_modified(bool)),this,SLOT(tabModified(bool)));
      QObject::connect(editor,SIGNAL(markProjectDirty(bool)),this,SLOT(projectDirtied(bool)));
   }

   tabIdx = QTabWidget::addTab(widget,icon,label);

   emit tabAdded(tabIdx);

   return tabIdx;
}

int CProjectTabWidget::addTab(QWidget *widget, const QString &label)
{
   CDesignerEditorBase* editor = dynamic_cast<CDesignerEditorBase*>(widget);
   int tabIdx;

   if ( editor )
   {
      QObject::connect(editor,SIGNAL(editor_modified(bool)),this,SLOT(tabModified(bool)));
      QObject::connect(editor,SIGNAL(markProjectDirty(bool)),this,SLOT(projectDirtied(bool)));
   }

   tabIdx = QTabWidget::addTab(widget,label);

   emit tabAdded(tabIdx);

   return tabIdx;
}

void CProjectTabWidget::removeTab(int index)
{
   QTabWidget::removeTab(index);
}

void CProjectTabWidget::tabModified(bool modified)
{
   int tab;

   for ( tab = 0; tab < count(); tab++ )
   {
      if ( widget(tab) == sender() )
      {
         if ( modified )
         {
            if ( tabBar()->tabText(tab).right(1) != "*" )
            {
               tabBar()->setTabText(tab,tabBar()->tabText(tab)+"*");
            }
         }
         else
         {
            if ( tabBar()->tabText(tab).right(1) == "*" )
            {
               tabBar()->setTabText(tab,tabBar()->tabText(tab).left(tabBar()->tabText(tab).length()-1));
            }
         }

         emit tabModified(tab,modified);
      }
   }
}

void CProjectTabWidget::projectDirtied(bool dirtied)
{
   nesicideProject->setDirty(dirtied);
}
