#include "codebrowserdockwidget.h"
#include "ui_codebrowserdockwidget.h"

#include "dbg_cnes.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesrom.h"

#include "emulator_core.h"

#include "inspectorregistry.h"
#include "main.h"

#include "cmarker.h"

#include <QMessageBox>

CodeBrowserDockWidget::CodeBrowserDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CodeBrowserDockWidget)
{
   ui->setupUi(this);
   assemblyViewModel = new CCodeBrowserDisplayModel(this);
   ui->tableView->setModel(assemblyViewModel);

   // Connect signals to the models to have the model update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), assemblyViewModel, SLOT(update()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), assemblyViewModel, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( this, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );

   // Connect signals to the UI to have the UI update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateDisassembly()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateDisassembly(bool)) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(breakpointHit()) );
}

CodeBrowserDockWidget::~CodeBrowserDockWidget()
{
   delete ui;
   delete assemblyViewModel;
}

void CodeBrowserDockWidget::showEvent(QShowEvent* e)
{
   QDockWidget* breakpointInspector = InspectorRegistry::getInspector("Breakpoints");
   
   QObject::connect ( breakpointInspector, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
   
   ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   ui->tableView->resizeColumnToContents(0);
   ui->tableView->resizeColumnToContents(1);
   ui->tableView->resizeColumnToContents(2);
   ui->tableView->resizeColumnToContents(3);
   ui->tableView->resizeColumnToContents(4);
}

void CodeBrowserDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QMenu menu;
   int bp;
   int addr = 0;
   int absAddr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   addr = nesGetAddressFromSLOC(index.row());
   
   absAddr = nesGetAbsoluteAddressFromAddress(addr);

   if ( addr != -1 )
   {
      bp = pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                          eBreakpointItemAddress,
                                          0,
                                          addr,
                                          absAddr,
                                          addr,
                                          eBreakpointConditionNone,
                                          0,
                                          eBreakpointDataNone,
                                          0 );
   
      // Build context menu...
      menu.addAction(ui->actionRun_to_here);
      menu.addSeparator();
   
      // If breakpoint isn't set here, give menu options to set one...
      if ( bp < 0 )
      {
         menu.addAction(ui->actionBreak_on_CPU_execution_here);
      }
      else
      {
         if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
         {
            menu.addAction(ui->actionEnable_breakpoint);
            menu.addAction(ui->actionRemove_breakpoint);
         }
         else
         {
            menu.addAction(ui->actionDisable_breakpoint);
            menu.addAction(ui->actionRemove_breakpoint);
         }
      }
   
      menu.addSeparator();
      menu.addAction(ui->actionClear_marker);
      menu.addSeparator();
   
      menu.addAction(ui->actionStart_marker_here);
      menu.addAction(ui->actionEnd_marker_here);
   
      // Run the context menu...
      // CPTODO: Hokey trick to provide the breakpoint-of-interest to action handlers...
      m_breakpointIndex = bp;
   
      menu.exec(e->globalPos());
   
      emit breakpointsChanged();
   }
}

void CodeBrowserDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void CodeBrowserDockWidget::breakpointHit()
{
   ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   show();
}

void CodeBrowserDockWidget::cartridgeLoaded()
{
   if ( nesROMIsLoaded() )
   {
      show();
   }
}

void CodeBrowserDockWidget::updateDisassembly(bool showMe)
{
   ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   if ( showMe )
   {
      show();
   }
}

void CodeBrowserDockWidget::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int bpIdx;
   int addr = 0;
   int absAddr = 0;

   addr = nesGetAddressFromSLOC(index.row());
   
   absAddr = nesGetAbsoluteAddressFromAddress(addr);

   if ( addr != -1 )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            absAddr,
                                            addr,
                                            eBreakpointConditionNone,
                                            0,
                                            eBreakpointDataNone,
                                            0,
                                            true );
   
      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
   
      InspectorRegistry::getInspector("Breakpoints")->hide();
      InspectorRegistry::getInspector("Breakpoints")->show();
   
      emit breakpointsChanged();
   }
}

void CodeBrowserDockWidget::on_actionRun_to_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int addr = 0;
   int absAddr = 0;

   addr = nesGetAddressFromSLOC(index.row());
   
   absAddr = nesGetAbsoluteAddressFromAddress(addr);

   if ( addr != -1 )
   {
      C6502DBG::GOTO(addr);
   }// CPTODO: fix the goto for absolute
}

void CodeBrowserDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;

   if ( index.isValid() )
   {
      addr = nesGetAddressFromSLOC(index.row());
      
      absAddr = nesGetAbsoluteAddressFromAddress(addr);

      if ( addr != -1 )
      {
         bp = pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                             eBreakpointItemAddress,
                                             0,
                                             addr,
                                             absAddr,
                                             addr,
                                             eBreakpointConditionNone,
                                             0,
                                             eBreakpointDataNone,
                                             0 );
   
         if ( bp < 0 )
         {
            on_actionBreak_on_CPU_execution_here_triggered();
         }
         else
         {
            pBreakpoints->RemoveBreakpoint(bp);

            InspectorRegistry::getInspector("Breakpoints")->hide();
            InspectorRegistry::getInspector("Breakpoints")->show();
         }
   
         emit breakpointsChanged();
      }
   }
}

void CodeBrowserDockWidget::on_actionDisable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);
   }
}

void CodeBrowserDockWidget::on_actionRemove_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(m_breakpointIndex);
   }
}

void CodeBrowserDockWidget::on_actionEnable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);
   }
}

void CodeBrowserDockWidget::on_actionStart_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker;
   int addr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   if ( index.isValid() )
   {
      addr = nesGetAddressFromSLOC(index.row());

      if ( addr != -1 )
      {
         // Find unused Marker entry...
         marker = markers->AddMarker(nesGetAbsoluteAddressFromAddress(addr));
      }
   }
}

void CodeBrowserDockWidget::on_actionEnd_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker = markers->FindInProgressMarker();
   int addr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   if ( marker >= 0 )
   {
      addr = nesGetAddressFromSLOC(index.row());

      if ( addr != -1 )
      {
         markers->CompleteMarker(marker,nesGetAbsoluteAddressFromAddress(addr));
      }
   }
}

void CodeBrowserDockWidget::on_actionClear_marker_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   markers->ClearAllMarkers();
}
