#ifndef EXECUTIONINSPECTORDOCKWIDGET_H
#define EXECUTIONINSPECTORDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cdebuggerexecutiontracermodel.h"

namespace Ui {
   class ExecutionInspectorDockWidget;
}

class ExecutionInspectorDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit ExecutionInspectorDockWidget(QWidget *parent = 0);
   virtual ~ExecutionInspectorDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateTracer();
   void updateTargetMachine(QString target);

private:
   Ui::ExecutionInspectorDockWidget *ui;
   CDebuggerExecutionTracerModel* model;

private slots:
   void on_actionBreak_on_CPU_execution_here_triggered();
   void on_showCPU_toggled(bool checked);
   void on_showPPU_toggled(bool checked);
};

#endif // EXECUTIONINSPECTORDOCKWIDGET_H
