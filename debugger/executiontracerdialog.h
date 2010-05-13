#ifndef EXECUTIONTRACERDIALOG_H
#define EXECUTIONTRACERDIALOG_H

#include <QDialog>
#include "cdebuggerexecutiontracermodel.h"

namespace Ui {
    class ExecutionTracerDialog;
}

class ExecutionTracerDialog : public QDialog {
    Q_OBJECT
public:
    ExecutionTracerDialog(QWidget *parent = 0);
    ~ExecutionTracerDialog();

protected:
    void showEvent(QShowEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void updateTracer();
    void cartridgeLoaded();

signals:
    void showMe();

private:
    Ui::ExecutionTracerDialog *ui;
    CDebuggerExecutionTracerModel *model;

private slots:
    void on_actionBreak_on_CPU_execution_here_triggered();
    void on_showPPU_toggled(bool checked);
    void on_showCPU_toggled(bool checked);
};

#endif // EXECUTIONTRACERDIALOG_H
