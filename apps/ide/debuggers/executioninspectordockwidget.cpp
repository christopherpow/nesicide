#include "executioninspectordockwidget.h"
#include "ui_executioninspectordockwidget.h"

#include "dbg_cnes.h"

#include "cobjectregistry.h"
#include "main.h"

ExecutionInspectorDockWidget::ExecutionInspectorDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::ExecutionInspectorDockWidget)
{
   ui->setupUi(this);
   model = new CDebuggerExecutionTracerModel(this);
   model->showCPU ( true );
   model->showPPU ( true );
   ui->showCPU->setChecked(true);
   ui->showPPU->setChecked(true);
   ui->tableView->setModel(model);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->tableView->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->tableView->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->tableView->setFont(QFont("Consolas", 11));
#endif
}

ExecutionInspectorDockWidget::~ExecutionInspectorDockWidget()
{
   delete ui;
   delete model;
}

void ExecutionInspectorDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateTracer()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), model, SLOT(update()) );
   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(machineReady()), model, SLOT(update()));
      QObject::connect ( emulator, SIGNAL(emulatorReset()), model, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), model, SLOT(update()) );
   }
}

void ExecutionInspectorDockWidget::showEvent(QShowEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   }
   model->update();
   ui->tableView->resizeColumnsToContents();
}

void ExecutionInspectorDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   }
}

void ExecutionInspectorDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu menu;

   menu.addAction(ui->actionBreak_on_CPU_execution_here);
   menu.exec(e->globalPos());
}

void ExecutionInspectorDockWidget::changeEvent(QEvent* e)
{
   CDebuggerBase::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void ExecutionInspectorDockWidget::updateTracer ()
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
         show();
         ui->tableView->resizeColumnsToContents();
         ui->tableView->setCurrentIndex(model->index(0,0));
      }
      else if ( (pBreakpoint->type == eBreakOnPPUFetch) &&
                (pBreakpoint->hit) )
      {
         // Update display...
         show();
         ui->tableView->resizeColumnsToContents();
         ui->tableView->setCurrentIndex(model->index(0,0));
      }
   }
}

void ExecutionInspectorDockWidget::on_showCPU_toggled(bool checked)
{
   model->showCPU ( checked );
   model->update();
   ui->tableView->resizeColumnsToContents();
}

void ExecutionInspectorDockWidget::on_showPPU_toggled(bool checked)
{
   model->showPPU ( checked );
   model->update();
   ui->tableView->resizeColumnsToContents();
}

void ExecutionInspectorDockWidget::on_actionBreak_on_CPU_execution_here_triggered()
{
}
