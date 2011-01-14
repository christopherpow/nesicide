#include "codebrowserdockwidget.h"
#include "ui_codebrowserdockwidget.h"

#include "dbg_cnes.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesrom.h"
#include "pasm_lib.h"

#include "inspectorregistry.h"
#include "main.h"

#include "cmarker.h"

#include <QMessageBox>

#define BROWSE_ASSEMBLY 0
#define BROWSE_SOURCE   1

CodeBrowserDockWidget::CodeBrowserDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CodeBrowserDockWidget)
{
   QDockWidget* breakpointInspector = InspectorRegistry::getInspector("Breakpoints");

   ui->setupUi(this);
   assemblyViewModel = new CCodeBrowserDisplayModel(this);
   sourceViewModel = new CSourceBrowserDisplayModel(this);
   ui->tableView->setModel(assemblyViewModel);
   ui->displayMode->setCurrentIndex ( 0 );
   ui->sourceFiles->setEnabled(false);

   // Connect signals to the UI to have the UI update.
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateDisassembly(bool)) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(breakpointHit()) );
   QObject::connect ( assemblyViewModel, SIGNAL(layoutChanged()), this, SLOT(updateDisassembly()) );

   // Connect signals to the models to have the model update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), assemblyViewModel, SLOT(update()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), assemblyViewModel, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( breakpointInspector->widget(), SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( this, SIGNAL(breakpointsChanged()), assemblyViewModel, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), sourceViewModel, SLOT(update()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), sourceViewModel, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), sourceViewModel, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), sourceViewModel, SLOT(update()) );
   QObject::connect ( breakpointInspector->widget(), SIGNAL(breakpointsChanged()), sourceViewModel, SLOT(update()) );
   QObject::connect ( this, SIGNAL(breakpointsChanged()), sourceViewModel, SLOT(update()) );
}

CodeBrowserDockWidget::~CodeBrowserDockWidget()
{
   delete ui;
   delete assemblyViewModel;
   delete sourceViewModel;
}

void CodeBrowserDockWidget::showEvent(QShowEvent* e)
{
   int file;
   
   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
         break;
      case BROWSE_SOURCE:
         ui->sourceFiles->clear();
         for ( file = 0; file < pasm_get_num_source_files(); file++ )
         {
            ui->sourceFiles->insertItem(file,pasm_get_source_file_name_by_index(file));
         }
         ui->sourceFiles->setCurrentIndex(pasm_get_source_file_index_by_name(pasm_get_source_file_name_by_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))));
         ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum_by_absolute_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))-1,0));
         break;
   }

   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QMenu menu;
   int bp;
   int addr = 0;
   QModelIndex index = ui->tableView->currentIndex();

   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         addr = nesGetAddressFromSLOC(index.row());
         break;
      case BROWSE_SOURCE:
         addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
         break;
   }

   if ( addr != -1 )
   {
      bp = pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                          eBreakpointItemAddress,
                                          0,
                                          addr,
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
   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
         break;
      case BROWSE_SOURCE:
         ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum_by_absolute_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))-1,0));
         break;
   }
   show();
}

void CodeBrowserDockWidget::updateDisassembly(bool showMe)
{
   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
         break;
      case BROWSE_SOURCE:
         ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum_by_absolute_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))-1,0));
         break;
   }
   if ( showMe )
   {
      show();
   }
}

void CodeBrowserDockWidget::updateBrowser()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         switch ( ui->displayMode->currentIndex() )
         {
            case BROWSE_ASSEMBLY:
               ui->tableView->setCurrentIndex(assemblyViewModel->index(nesGetSLOCFromAddress(nesGetCPUProgramCounterOfLastSync()),0));
               break;
            case BROWSE_SOURCE:
               ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum_by_absolute_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))-1,0));
               break;
         }
         show();
         break;
      }
   }
}

void CodeBrowserDockWidget::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   bool added;
   int addr = 0;

   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         addr = nesGetAddressFromSLOC(index.row());
         break;
      case BROWSE_SOURCE:
         addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
         break;
   }

   if ( addr != -1 )
   {
      added = pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            eBreakpointConditionNone,
                                            0,
                                            eBreakpointDataNone,
                                            0 );
   
      if ( !added )
      {
         QMessageBox::information(0, "Error", "Cannot add breakpoint, already have 8 defined.");
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

   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         addr = nesGetAddressFromSLOC(index.row());
         break;
      case BROWSE_SOURCE:
         addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
         break;
   }

   if ( addr != -1 )
   {
      C6502DBG::GOTO(addr);
   }
}

void CodeBrowserDockWidget::on_displayMode_currentIndexChanged(int index)
{
   int file;
   
   switch ( index )
   {
      case BROWSE_ASSEMBLY:
         ui->tableView->setModel(assemblyViewModel);
         ui->sourceFiles->setEnabled(false);
         break;
      case BROWSE_SOURCE:
         ui->tableView->setModel(sourceViewModel);
         ui->sourceFiles->setEnabled(true);
         ui->sourceFiles->clear();
         for ( file = 0; file < pasm_get_num_source_files(); file++ )
         {
            ui->sourceFiles->insertItem(file,pasm_get_source_file_name_by_index(file));
         }
         ui->sourceFiles->setCurrentIndex(pasm_get_source_file_index_by_name(pasm_get_source_file_name_by_addr(nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()))));
         break;
   }

   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;

   if ( index.isValid() )
   {
      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            addr = nesGetAddressFromSLOC(index.row());
            break;
         case BROWSE_SOURCE:
            addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
            break;
      }

      if ( addr != -1 )
      {
         bp = pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                             eBreakpointItemAddress,
                                             0,
                                             addr,
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
      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            addr = nesGetAddressFromSLOC(index.row());
            break;
         case BROWSE_SOURCE:
            addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
            break;
      }

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
      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            addr = nesGetAddressFromSLOC(index.row());
            break;
         case BROWSE_SOURCE:
            addr = pasm_get_source_addr_by_linenum_and_file ( index.row()+1, ui->sourceFiles->currentText().toAscii().data() );
            break;
      }

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


void CodeBrowserDockWidget::on_sourceFiles_currentIndexChanged(int index)
{
   QStringList  source;
   QString      str;   
   
   if ( index >= 0 )
   {
      str = pasm_get_source_file_text_by_index(index);
      source = str.split ( QRegExp("[\r\n]") );
      sourceViewModel->setSource(source);
      sourceViewModel->setSourceFilename(pasm_get_source_file_name_by_index(index));
      sourceViewModel->force();
   }
}
