#include "executiontracerdialog.h"
#include "ui_executiontracerdialog.h"

#include "dbg_cnes.h"

#include "inspectorregistry.h"
#include "main.h"

ExecutionTracerDialog::ExecutionTracerDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::ExecutionTracerDialog)
{
   ui->setupUi(this);
   model = new CDebuggerExecutionTracerModel(this);
   model->showCPU ( true );
   model->showPPU ( true );
   ui->showCPU->setChecked(true);
   ui->showPPU->setChecked(true);
   ui->tableView->setModel(model);
//   QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateTracer()) );
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateTracer()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateTracer()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateTracer()) );
}

ExecutionTracerDialog::~ExecutionTracerDialog()
{
   delete ui;
   delete model;
}

void ExecutionTracerDialog::showEvent(QShowEvent* e)
{
   QDialog::showEvent(e);
   updateTracer();
   ui->tableView->resizeColumnsToContents();
}

void ExecutionTracerDialog::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu menu;

   menu.addAction(ui->actionBreak_on_CPU_execution_here);
   menu.exec(e->globalPos());
}

void ExecutionTracerDialog::changeEvent(QEvent* e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void ExecutionTracerDialog::updateTracer ()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
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
         ui->tableView->setCurrentIndex(model->index(0,0));
      }
      else if ( (pBreakpoint->type == eBreakOnPPUFetch) &&
                (pBreakpoint->hit) )
      {
         // Update display...
         emit showMe();
         ui->tableView->setCurrentIndex(model->index(0,0));
      }
   }

   model->layoutChangedEvent();
}

void ExecutionTracerDialog::cartridgeLoaded ()
{
   updateTracer();
   model->layoutChangedEvent();
}

void ExecutionTracerDialog::on_showCPU_toggled(bool checked)
{
   model->showCPU ( checked );
   model->layoutChangedEvent();
}

void ExecutionTracerDialog::on_showPPU_toggled(bool checked)
{
   model->showPPU ( checked );
   model->layoutChangedEvent();
}

void ExecutionTracerDialog::on_actionBreak_on_CPU_execution_here_triggered()
{
}
