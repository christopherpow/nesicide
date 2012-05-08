#ifndef APUINFORMATIONDOCKWIDGET_H
#define APUINFORMATIONDOCKWIDGET_H

#include "cdebuggerbase.h"

namespace Ui {
   class APUInformationDockWidget;
}

class APUInformationDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit APUInformationDockWidget(QWidget *parent = 0);
   virtual ~APUInformationDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateInformation();
   void updateTargetMachine(QString target);

private:
   Ui::APUInformationDockWidget *ui;
};

#endif // APUINFORMATIONDOCKWIDGET_H
