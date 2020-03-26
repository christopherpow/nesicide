#include "codebrowserdockwidget.h"
#include "ui_codebrowserdockwidget.h"

#include "nes_emulator_core.h"

#include "cmarker.h"
#include "cbreakpointinfo.h"
#include "cdockwidgetregistry.h"

#include "cobjectregistry.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include <QMenu>
#include <QMessageBox>
#include <QContextMenuEvent>

CodeBrowserDockWidget::CodeBrowserDockWidget(CBreakpointInfo* pBreakpoints,QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeBrowserDockWidget)
{
   ui->setupUi(this);

   m_pBreakpoints = pBreakpoints;

   m_loadedTarget = "none";

   assemblyViewModel = new CCodeBrowserDisplayModel(pBreakpoints,this);
   ui->tableView->setModel(assemblyViewModel);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   setFont(QFont("Consolas", 11));
#endif

   QObject::connect ( this, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
}

CodeBrowserDockWidget::~CodeBrowserDockWidget()
{
   delete ui;
   delete assemblyViewModel;
}

void CodeBrowserDockWidget::updateTargetMachine(QString target)
{
   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

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

void CodeBrowserDockWidget::showEvent(QShowEvent* /*e*/)
{
   QDockWidget* breakpointInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Breakpoints"));
   QDockWidget* executionVisualizer = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Execution Visualizer"));
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   // Specifically not connecting to updateDebuggers signal here since it doesn't make much sense to
   // update the code position until a pause/breakpoint.
   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateDebuggers()) );
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
   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateDebuggers()) );
   }
}

void CodeBrowserDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu menu;
   int bp;
   int addr = 0;
   int physAddr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetVirtualAddressFromSLOC(index.row());
      physAddr = nesGetPhysicalAddressFromAddress(addr);
   }
   else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
   {
      addr = c64GetAddressFromSLOC(index.row());
      physAddr = c64GetPhysicalAddressFromAddress(addr);
   }

   if ( addr != -1 )
   {
      menu.addAction(ui->actionGo_to_Source);
      menu.addSeparator();

      bp = m_pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            physAddr,
                                            addr,
                                            0xFFFF,
                                            true,
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
         if ( m_pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
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

      menu.addAction(ui->actionStart_End_marker_here);

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

      if ( splits.count() == 2 )
      {
         addr = splits.at(1).toInt(NULL,16);
      }
      else if ( splits.count() == 3 )
      {
         addr = splits.at(2).toInt(NULL,16);
      }
      else if ( splits.count() == 4 )
      {
         addr = splits.at(3).toInt(NULL,16);
      }

      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(addr),0));
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(addr),0));
      }
      ui->tableView->scrollTo(ui->tableView->currentIndex(),QAbstractItemView::PositionAtTop);
      ui->tableView->resizeColumnsToContents();
   }
}

void CodeBrowserDockWidget::breakpointHit()
{
   if ( isVisible() )
   {
      if ( nesROMIsLoaded() )
      {
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
         }
         else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
         {
            ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
         }
         ui->tableView->scrollTo(ui->tableView->currentIndex());
         ui->tableView->resizeColumnsToContents();
      }
   }
}

void CodeBrowserDockWidget::emulatorPaused(bool showMe)
{
   if ( isVisible() )
   {
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
      }
      ui->tableView->scrollTo(ui->tableView->currentIndex());
      ui->tableView->resizeColumnsToContents();
   }
}

void CodeBrowserDockWidget::machineReady()
{
   if ( isVisible() )
   {
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         ui->tableView->setCurrentIndex(assemblyViewModel->index(c64GetSLOCFromAddress(c64GetCPURegister(CPU_PC)),0));
      }
      ui->tableView->scrollTo(ui->tableView->currentIndex());
      ui->tableView->resizeColumnsToContents();
   }
}

void CodeBrowserDockWidget::on_actionBreak_on_CPU_execution_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int bpIdx;
   int addr = 0;
   int physAddr = 0;

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetVirtualAddressFromSLOC(index.row());
      physAddr = nesGetPhysicalAddressFromAddress(addr);
   }
   else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
   {
      addr = c64GetAddressFromSLOC(index.row());
      physAddr = c64GetPhysicalAddressFromAddress(addr);
   }

   if ( addr != -1 )
   {
      bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                              eBreakpointItemAddress,
                                              0,
                                              addr,
                                              physAddr,
                                              addr,
                                              0xFFFF,
                                              true,
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
   int physAddr = 0;

   if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
   {
      addr = nesGetVirtualAddressFromSLOC(index.row());
      physAddr = nesGetPhysicalAddressFromAddress(addr);
      if ( addr != -1 )
      {
         nesSetGotoAddress(addr);
      }// CPTODO: fix the goto for absolute
   }
   else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
   {
      addr = c64GetAddressFromSLOC(index.row());
      physAddr = c64GetPhysicalAddressFromAddress(addr);
      if ( addr != -1 )
      {
         c64SetGotoAddress(addr);
      }// CPTODO: fix the goto for absolute
   }
}

void CodeBrowserDockWidget::on_actionDisable_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::on_actionRemove_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->RemoveBreakpoint(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::on_actionEnable_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeBrowserDockWidget::on_actionStart_End_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
   int marker = -1;
   int line, lineFrom, lineTo, indexFrom, indexTo;
   int addr = 0;
   int absAddr = 0;

   lineFrom = indexes[0].row();
   lineTo = indexes[indexes.count()-1].row();

   if ( lineFrom != -1 )
   {
//      nesGetAddrFromSLOC(lineFrom);

//      if ( addr != -1 )
//      {
//         // Find unused Marker entry...
//         marker = markers->AddMarker(addr,absAddr);

//         emit breakpointsChanged();
//         emit markProjectDirty(true);
//      }
   }
   if ( lineTo != -1 )
   {
//      if ( marker >= 0 )
//      {
//         resolveLineAddress(lineTo,&addr,&absAddr);

//         if ( addr != -1 )
//         {
//            markers->CompleteMarker(marker,addr,absAddr);

//            emit breakpointsChanged();
//            emit markProjectDirty(true);
//         }
//      }
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
         addr = nesGetVirtualAddressFromSLOC(index.row());
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         addr = c64GetAddressFromSLOC(index.row());
      }

      if ( addr != -1 )
      {
         // Find unused Marker entry...
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            marker = markers->AddMarker(addr,nesGetPhysicalAddressFromAddress(addr));
         }
         else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
         {
            marker = markers->AddMarker(addr,c64GetPhysicalAddressFromAddress(addr));
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
         addr = nesGetVirtualAddressFromSLOC(index.row());
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         addr = c64GetAddressFromSLOC(index.row());
      }

      if ( addr != -1 )
      {
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            markers->CompleteMarker(marker,addr,nesGetPhysicalAddressFromAddress(addr));
         }
         else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
         {
            markers->CompleteMarker(marker,addr,c64GetPhysicalAddressFromAddress(addr));
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
   int bp;
   int addr = 0;
   int physAddr = 0;

   if ( QApplication::mouseButtons() == Qt::LeftButton )
   {
      if ( index.isValid() && index.column() == 0 )
      {
         if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
         {
            addr = nesGetVirtualAddressFromSLOC(index.row());
            physAddr = nesGetPhysicalAddressFromAddress(addr);
         }
         else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
         {
            addr = c64GetAddressFromSLOC(index.row());
            physAddr = c64GetPhysicalAddressFromAddress(addr);
         }

         if ( addr != -1 )
         {
            bp = m_pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                                  eBreakpointItemAddress,
                                                  0,
                                                  addr,
                                                  physAddr,
                                                  addr,
                                                  0xFFFF,
                                                  true,
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
               if ( m_pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
               {
                  m_pBreakpoints->RemoveBreakpoint(bp);
               }
               else
               {
                  m_pBreakpoints->SetEnabled(bp,false);
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
