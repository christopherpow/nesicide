#ifndef CODEPROFILERDOCKWIDGET_H
#define CODEPROFILERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cdebuggercodeprofilermodel.h"
#include "ixmlserializable.h"

namespace Ui {
   class CodeProfilerDockWidget;
}

class CodeProfilerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit CodeProfilerDockWidget(QWidget *parent = 0);
   ~CodeProfilerDockWidget();

protected:
   void showEvent(QShowEvent *event);
   void hideEvent(QHideEvent *event);

private:
   Ui::CodeProfilerDockWidget *ui;
   CDebuggerCodeProfilerModel *model;

signals:

private slots:
   void on_clear_clicked();
   void on_tableView_doubleClicked(QModelIndex index);
   void updateUi();
   void updateTargetMachine(QString target);
};

#endif // CODEPROFILERDOCKWIDGET_H
