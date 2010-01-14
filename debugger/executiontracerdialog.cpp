#include "executiontracerdialog.h"
#include "ui_executiontracerdialog.h"

#include "cnes6502.h"

#include "main.h"

ExecutionTracerDialog::ExecutionTracerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExecutionTracerDialog)
{
    ui->setupUi(this);
    tableViewModel = new CDebuggerExecutionTracerModel(this);
    tableViewModel->showCPU ( true );
    tableViewModel->showPPU ( false );
    ui->showCPU->setChecked(true);
    ui->showPPU->setChecked(false);
    ui->tableView->setModel(tableViewModel);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateTracer()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateTracer()) );
}

ExecutionTracerDialog::~ExecutionTracerDialog()
{
    delete ui;
    delete tableViewModel;
}

void ExecutionTracerDialog::contextMenuEvent(QContextMenuEvent *e)
{
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

void ExecutionTracerDialog::on_showCPU_toggled(bool checked)
{
   tableViewModel->showCPU ( checked );
}

void ExecutionTracerDialog::on_showPPU_toggled(bool checked)
{
   tableViewModel->showPPU ( checked );
}
