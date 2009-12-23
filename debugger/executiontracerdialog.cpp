#include "executiontracerdialog.h"
#include "ui_executiontracerdialog.h"

ExecutionTracerDialog::ExecutionTracerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExecutionTracerDialog)
{
    ui->setupUi(this);
}

ExecutionTracerDialog::~ExecutionTracerDialog()
{
    delete ui;
}

void ExecutionTracerDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
