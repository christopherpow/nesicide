#ifndef PROJECTBROWSERDOCKWIDGET_H
#define PROJECTBROWSERDOCKWIDGET_H

#include <QTabWidget>
#include <QDockWidget>

#include "cprojecttreeviewmodel.h"
#include "cprojecttreeview.h"
#include "sourcenavigator.h"

namespace Ui {
    class ProjectBrowserDockWidget;
}

class ProjectBrowserDockWidget : public QDockWidget
{
   Q_OBJECT
   
public:
   explicit ProjectBrowserDockWidget(QTabWidget* pTarget,SourceNavigator* pSourceNavigator,QWidget *parent = 0);
   ~ProjectBrowserDockWidget();

   void layoutChangedEvent();
   void enableNavigation();
   void disableNavigation();
   
private:
   Ui::ProjectBrowserDockWidget *ui;
   QTabWidget* m_pTarget;
   CProjectTreeViewModel* m_pProjectTreeviewModel;
   SourceNavigator* m_pSourceNavigator;
};

#endif // PROJECTBROWSERDOCKWIDGET_H
