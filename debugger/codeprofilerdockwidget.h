#ifndef CODEPROFILERDOCKWIDGET_H
#define CODEPROFILERDOCKWIDGET_H

#include "cdebuggerbase.h"
#include "cprojecttabwidget.h"

#include "cdebuggercodeprofilermodel.h"
#include "ixmlserializable.h"

namespace Ui {
   class CodeProfilerDockWidget;
}

class CodeProfilerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit CodeProfilerDockWidget(CProjectTabWidget* pTarget, QWidget *parent = 0);
   ~CodeProfilerDockWidget();

private:
   Ui::CodeProfilerDockWidget *ui;
   CDebuggerCodeProfilerModel *model;
   CProjectTabWidget* m_pTarget;

signals:
   void snapTo(QString item);

private slots:
   void on_clear_clicked();
   void on_tableView_doubleClicked(QModelIndex index);
   void updateUi();
};

#endif // CODEPROFILERDOCKWIDGET_H
