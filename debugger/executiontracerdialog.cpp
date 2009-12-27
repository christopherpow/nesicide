#include "executiontracerdialog.h"
#include "ui_executiontracerdialog.h"

#include "cnes6502.h"

#include "main.h"

ExecutionTracerDialog::ExecutionTracerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExecutionTracerDialog)
{
    ui->setupUi(this);
    tableViewModel = new CDebuggerExecutionTracerModel(this,&C6502::TRACER());
    ui->tableView->setModel(tableViewModel);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateTracer()) );
}

ExecutionTracerDialog::~ExecutionTracerDialog()
{
    delete ui;
    delete tableViewModel;
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

void ExecutionTracerDialog::updateTracer ()
{
   tableViewModel->layoutChangedEvent();
}
