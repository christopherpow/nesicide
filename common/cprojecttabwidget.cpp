#include "cprojecttabwidget.h"

#include "codeeditorform.h"

CProjectTabWidget::CProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

int CProjectTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
   CodeEditorForm* codeEditor = dynamic_cast<CodeEditorForm*>(widget);
   qDebug("addTab1");

   if ( codeEditor )
   {
      QObject::connect(codeEditor,SIGNAL(editor_modified(bool)),this,SLOT(tabModified(bool)));
   }
   return QTabWidget::addTab(widget,icon,label);
}

int CProjectTabWidget::addTab(QWidget *widget, const QString &label)
{
   CodeEditorForm* codeEditor = dynamic_cast<CodeEditorForm*>(widget);
qDebug("addTab2");
   if ( codeEditor )
   {
      QObject::connect(codeEditor,SIGNAL(editor_modified(bool)),this,SLOT(tabModified(bool)));
   }
   return QTabWidget::addTab(widget,label);
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
            tabBar()->setTabText(tab,tabBar()->tabText(tab)+"*");
         }
         else
         {
            tabBar()->setTabText(tab,tabBar()->tabText(tab).left(tabBar()->tabText(tab).length()-1));
         }
      }
   }
}
