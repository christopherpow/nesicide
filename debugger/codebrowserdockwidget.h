#ifndef CODEBROWSERDOCKWIDGET_H
#define CODEBROWSERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "ccodebrowserdisplaymodel.h"

namespace Ui {
   class CodeBrowserDockWidget;
}

class CodeBrowserDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit CodeBrowserDockWidget(QWidget *parent = 0);
   virtual ~CodeBrowserDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void breakpointHit();
   void emulatorPaused(bool showMe);
   void cartridgeLoaded();

signals:
   void breakpointsChanged();
   void snapToTab(QString item);

private:
   Ui::CodeBrowserDockWidget *ui;
   CCodeBrowserDisplayModel* assemblyViewModel;
   int m_breakpointIndex;

private slots:
   void on_actionGo_to_Source_triggered();
   void on_tableView_doubleClicked(QModelIndex index);
   void snapTo(QString item);
   void on_tableView_pressed(QModelIndex index);
   void on_actionEnable_breakpoint_triggered();
   void on_actionRemove_breakpoint_triggered();
   void on_actionDisable_breakpoint_triggered();
   void on_actionRun_to_here_triggered();
   void on_actionBreak_on_CPU_execution_here_triggered();
   void on_actionStart_marker_here_triggered();
   void on_actionEnd_marker_here_triggered();
   void on_actionClear_marker_triggered();
};

#endif // CODEBROWSERDOCKWIDGET_H
