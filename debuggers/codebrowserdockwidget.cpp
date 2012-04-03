#include "codebrowserdockwidget.h"
#include "ui_codebrowserdockwidget.h"

#include "nes_emulator_core.h"

#include "cmarker.h"
#include "cbreakpointinfo.h"
#include "cdockwidgetregistry.h"

#include "cthreadregistry.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include <QMenu>
#include <QMessageBox>
#include <QContextMenuEvent>

CodeBrowserDockWidget::CodeBrowserDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeBrowserDockWidget)
{
   ui->setupUi(this);
   assemblyViewModel = new CCodeBrowserDisplayModel(this);
   ui->tableView->setModel(assemblyViewModel);

   m_loadedTarget = "none";

   QObject::connect ( this, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
}

CodeBrowserDockWidget::~CodeBrowserDockWidget()
{
   delete ui;
   delete assemblyViewModel;
}

void CodeBrowserDockWidget::updateTargetMachine(QString target)
{
   QThread* breakpointWatcher = CThreadRegistry::getThread("Breakpoint Watcher");
   QThread* emulator = CThreadRegistry::getThread("Emulator");

   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(breakpointHit()) );

   m_loadedTarget = target;

   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(machineReady()), assemblyViewModel, SLOT(update()));
      QObject::connect ( emulator, SIGNAL(emulatorReset()), assemblyViewModel, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), assemblyViewModel, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(machineReady()), this, SLOT(machineReady()) );
      QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(machineReady()) );
   }
}

void CodeBrowserDockWidget::showEvent(QShowEvent* e)
{
   QDockWidget* breakpointInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   QDockWidget* executionVisualizer = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Execution Visualizer"));
   QThread* emulator = CThreadRegistry::getThread("Emulator");

   // Specifically not connecting to updateDebuggers signal here since it doesn't make much sense to
   // update the code position until a pause/breakpoint.
   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(emulatorPaused(bool)) );
   }
   QObject::connect ( breakpointInspector, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( breakpointInspector, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
   if ( executionVisualizer )
   {
      QObject::connect ( executionVisualizer, SIGNAL(snapTo(QString)), this, SLOT(snapTo(QString)) );
      QObject::connect ( executionVisualizer, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
   }
   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   }
   else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
   {
      ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
   }
   ui->tableView->scrollTo(ui->tableView->currentIndex());
}

void CodeBrowserDockWidget::hideEvent(QHideEvent* e)
{
   QThread* emulator = CThreadRegistry::getThread("Emulator");

   if ( emulator )
   {
      QObject::disconnect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(emulatorPaused(bool)) );
   }
}

void CodeBrowserDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QMenu menu;
   int bp;
   int addr = 0;
   int absAddr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetAddressFromSLOC(index.row());
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
   }
   else
   {
      addr = c64GetAddressFromSLOC(index.row());
      absAddr = c64GetAbsoluteAddressFromAddress(addr);
   }

   if ( addr != -1 )
   {
      menu.addAction(ui->actionGo_to_Source);
      menu.addSeparator();

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
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::changeEvent(QEvent* e)
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

void CodeBrowserDockWidget::snapTo(QString item)
{
   uint32_t addr;

   // Make sure item is something we care about
   if ( item.startsWith("Address,") )
   {
      QStringList splits;
      splits = item.split(QRegExp("[,():]"),QString::SkipEmptyParts);

      if ( splits.count() == 4 )
      {
         addr = splits.at(3).toInt(NULL,16);

         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(addr),0));
         }
         else
         {
            ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(addr),0));
         }
         ui->tableView->scrollTo(ui->tableView->currentIndex());
         ui->tableView->resizeColumnsToContents();
      }
   }
}

void CodeBrowserDockWidget::breakpointHit()
{
   if ( nesROMIsLoaded() )
   {
      show();
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
      }
      else
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
      }
      ui->tableView->scrollTo(ui->tableView->currentIndex());
      ui->tableView->resizeColumnsToContents();
   }
}

void CodeBrowserDockWidget::emulatorPaused(bool showMe)
{
   if ( showMe )
   {
      show();
   }
   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   }
   else
   {
      ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
   }
   ui->tableView->scrollTo(ui->tableView->currentIndex());
   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDockWidget::machineReady()
{
   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      if ( nesROMIsLoaded() )
      {
         show();
      }
      ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
   }
   else
   {
      show();
      ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
   }
   ui->tableView->scrollTo(ui->tableView->currentIndex());
   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDockWidget::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int bpIdx;
   int addr = 0;
   int absAddr = 0;

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetAddressFromSLOC(index.row());
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
   }
   else
   {
      addr = c64GetAddressFromSLOC(index.row());
      absAddr = c64GetAbsoluteAddressFromAddress(addr);
   }

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
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void CodeBrowserDockWidget::on_actionRun_to_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int addr = 0;
   int absAddr = 0;

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetAddressFromSLOC(index.row());
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
      if ( addr != -1 )
      {
         nesSetGotoAddress(addr);
      }// CPTODO: fix the goto for absolute
   }
   else
   {
      addr = c64GetAddressFromSLOC(index.row());
      absAddr = c64GetAbsoluteAddressFromAddress(addr);
      if ( addr != -1 )
      {
         c64SetGotoAddress(addr);
      }// CPTODO: fix the goto for absolute
   }
}

void CodeBrowserDockWidget::on_actionDisable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::on_actionRemove_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::on_actionEnable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
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
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         addr = nesGetAddressFromSLOC(index.row());
      }
      else
      {
         addr = c64GetAddressFromSLOC(index.row());
      }

      if ( addr != -1 )
      {
         // Find unused Marker entry...
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            marker = markers->AddMarker(addr,nesGetAbsoluteAddressFromAddress(addr));
         }
         else
         {
            marker = markers->AddMarker(addr,c64GetAbsoluteAddressFromAddress(addr));
         }

         emit breakpointsChanged();
         emit markProjectDirty(true);
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
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         addr = nesGetAddressFromSLOC(index.row());
      }
      else
      {
         addr = c64GetAddressFromSLOC(index.row());
      }

      if ( addr != -1 )
      {
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            markers->CompleteMarker(marker,addr,nesGetAbsoluteAddressFromAddress(addr));
         }
         else
         {
            markers->CompleteMarker(marker,addr,c64GetAbsoluteAddressFromAddress(addr));
         }

         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void CodeBrowserDockWidget::on_actionClear_marker_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   markers->RemoveAllMarkers();

   emit breakpointsChanged();
   emit markProjectDirty(true);
}

void CodeBrowserDockWidget::on_tableView_pressed(QModelIndex index)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;

   if ( QApplication::mouseButtons() == Qt::LeftButton )
   {
      if ( index.isValid() && index.column() == 0 )
      {
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            addr = nesGetAddressFromSLOC(index.row());
            absAddr = nesGetAbsoluteAddressFromAddress(addr);
         }
         else
         {
            addr = c64GetAddressFromSLOC(index.row());
            absAddr = c64GetAbsoluteAddressFromAddress(addr);
         }

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
               if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
               {
                  pBreakpoints->RemoveBreakpoint(bp);
               }
               else
               {
                  pBreakpoints->SetEnabled(bp,false);
               }
            }

            emit breakpointsChanged();
            emit markProjectDirty(true);
         }
      }
   }
}

void CodeBrowserDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   if ( index.isValid() )
   {
      QString item = "Address,";

      item += assemblyViewModel->data(assemblyViewModel->index(index.row(),CodeBrowserCol_Address),Qt::DisplayRole).toString();

      emit snapToTab(item);
   }
}

void CodeBrowserDockWidget::on_actionGo_to_Source_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();

   if ( index.isValid() )
   {
      QString item = "Address,";

      item += assemblyViewModel->data(assemblyViewModel->index(index.row(),CodeBrowserCol_Address),Qt::DisplayRole).toString();

      emit snapToTab(item);
   }
}
