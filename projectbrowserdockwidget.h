#ifndef PROJECTBROWSERDOCKWIDGET_H
#define PROJECTBROWSERDOCKWIDGET_H

#include <QDockWidget>

#include "cprojecttabwidget.h"

#include "cprojecttreeviewmodel.h"
#include "cprojecttreeview.h"

namespace Ui {
    class ProjectBrowserDockWidget;
}

class ProjectBrowserDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit ProjectBrowserDockWidget(CProjectTabWidget* pTarget,QWidget *parent = 0);
   virtual ~ProjectBrowserDockWidget();

   void layoutChangedEvent();
   void enableNavigation();
   void disableNavigation();

private:
   Ui::ProjectBrowserDockWidget *ui;
   CProjectTabWidget* m_pTarget;
   CProjectTreeViewModel* m_pProjectTreeviewModel;
};

#endif // PROJECTBROWSERDOCKWIDGET_H
