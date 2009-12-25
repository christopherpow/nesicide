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
    void changeEvent(QEvent *e);

public slots:
    void updateTracer();

private:
    Ui::ExecutionTracerDialog *ui;
    CDebuggerExecutionTracerModel *tableViewModel;
};

#endif // EXECUTIONTRACERDIALOG_H
