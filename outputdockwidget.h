#ifndef OUTPUTDOCKWIDGET_H
#define OUTPUTDOCKWIDGET_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "outputdialog.h"

class OutputDockWidget : public QDockWidget
{
   Q_OBJECT
public:
   OutputDockWidget();
   virtual ~OutputDockWidget();

public slots:
   void showGeneralPane()
   {
      dialog->setCurrentOutputTab(0);
      this->show();
   }
   void showBuildPane()
   {
      dialog->setCurrentOutputTab(1);
      this->show();
   }
   void showDebugPane()
   {
      dialog->setCurrentOutputTab(2);
      this->show();
   }
   void clearAllPanes()
   {
      dialog->clearAllTabs();
   }

protected:
   OutputDialog* dialog;
   QFrame* frame;
};

#endif // OUTPUTDOCKWIDGET_H
