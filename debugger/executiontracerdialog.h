#ifndef EXECUTIONTRACERDIALOG_H
#define EXECUTIONTRACERDIALOG_H

#include <QDialog>

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

private:
    Ui::ExecutionTracerDialog *ui;
};

#endif // EXECUTIONTRACERDIALOG_H
