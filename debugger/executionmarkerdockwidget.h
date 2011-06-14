#ifndef EXECUTIONMARKERDOCKWIDGET_H
#define EXECUTIONMARKERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cexecutionmarkerdisplaymodel.h"

namespace Ui {
   class ExecutionMarkerDockWidget;
}

class ExecutionMarkerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit ExecutionMarkerDockWidget(QWidget *parent = 0);
   ~ExecutionMarkerDockWidget();

protected:
   void showEvent(QShowEvent *e);

private:
   Ui::ExecutionMarkerDockWidget *ui;
   CExecutionMarkerDisplayModel *model;
};

#endif // EXECUTIONMARKERDOCKWIDGET_H
