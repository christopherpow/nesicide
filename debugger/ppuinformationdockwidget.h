#ifndef PPUINFORMATIONDOCKWIDGET_H
#define PPUINFORMATIONDOCKWIDGET_H

#include "cdebuggerbase.h"

namespace Ui {
   class PPUInformationDockWidget;
}

class PPUInformationDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit PPUInformationDockWidget(QWidget *parent = 0);
   ~PPUInformationDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateInformation();

private:
   Ui::PPUInformationDockWidget *ui;
};

#endif // PPUINFORMATIONDOCKWIDGET_H
