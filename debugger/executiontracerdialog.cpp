#include "executiontracerdialog.h"
#include "ui_executiontracerdialog.h"

#include "cnes6502.h"
#include "cnesrom.h"

#include "inspectorregistry.h"
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

void ExecutionTracerDialog::showEvent(QShowEvent* e)
{
   ui->tableView->resizeColumnsToContents();
}

void ExecutionTracerDialog::contextMenuEvent(QContextMenuEvent *e)
{
   QMenu menu;

   menu.addAction(ui->actionBreak_on_CPU_execution_here);
   menu.exec(e->globalPos());
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
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( (pBreakpoint->type == eBreakOnCPUExecution) &&
           (pBreakpoint->hit) )
      {
         // Update display...
         emit showMe();
         ui->tableView->setCurrentIndex(tableViewModel->index(0,0));
      }
      else if ( (pBreakpoint->type == eBreakOnPPUFetch) &&
                (pBreakpoint->hit) )
      {
         // Update display...
         emit showMe();
         ui->tableView->setCurrentIndex(tableViewModel->index(0,0));
      }
   }

   tableViewModel->layoutChangedEvent();
}

void ExecutionTracerDialog::on_showCPU_toggled(bool checked)
{
   tableViewModel->showCPU ( checked );
   tableViewModel->layoutChangedEvent();
}

void ExecutionTracerDialog::on_showPPU_toggled(bool checked)
{
   tableViewModel->showPPU ( checked );   
   tableViewModel->layoutChangedEvent();
}

void ExecutionTracerDialog::on_actionBreak_on_CPU_execution_here_triggered()
{
}
