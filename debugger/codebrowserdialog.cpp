#include "codebrowserdialog.h"
#include "ui_codebrowserdialog.h"

#include "cnes6502.h"
#include "cnesrom.h"
#include "pasm_lib.h"

#include "inspectorregistry.h"
#include "main.h"

#include <QMessageBox>

#define BROWSE_ASSEMBLY 0
#define BROWSE_SOURCE   1

CodeBrowserDialog::CodeBrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeBrowserDialog)
{
    QDockWidget* breakpointInspector = InspectorRegistry::getInspector("Breakpoints");

    ui->setupUi(this);
    assemblyViewModel = new CCodeBrowserDisplayModel(this);
    sourceViewModel = new CSourceBrowserDisplayModel(this);
    ui->tableView->setModel(assemblyViewModel);
    ui->displayMode->setCurrentIndex ( 0 );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateBrowser()) );
    QObject::connect ( emulator, SIGNAL(emulatorPaused()), this, SLOT(updateDisassembly()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateDisassembly()) );
    QObject::connect ( breakpointInspector->widget(), SIGNAL(breakpointsChanged()), this, SLOT(updateBrowser()) );
}

CodeBrowserDialog::~CodeBrowserDialog()
{
    delete ui;
    delete assemblyViewModel;
    delete sourceViewModel;
}

void CodeBrowserDialog::showEvent(QShowEvent*)
{
   if ( C6502::__PC() < MEM_2KB )
   {
      // Update display...
      C6502::DISASSEMBLE();

      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(C6502::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }
   else
   {
      // Update display...
      CROM::DISASSEMBLE();

      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }

   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDialog::contextMenuEvent(QContextMenuEvent *e)
{
   QMenu menu;

   menu.addAction(ui->actionBreak_on_CPU_execution_here);
   menu.addAction(ui->actionRun_to_here);
   menu.exec(e->globalPos());
}

void CodeBrowserDialog::changeEvent(QEvent *e)
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

void CodeBrowserDialog::updateDisassembly()
{
   if ( C6502::__PC() < MEM_2KB )
   {
      // Update display...
      C6502::DISASSEMBLE();

      emit showMe();

      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(C6502::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }
   else
   {
      // Update display...
      CROM::DISASSEMBLE();

      emit showMe();

      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }
}

void CodeBrowserDialog::updateBrowser()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( pBreakpoint->hit )
      {
         // Update display...
         if ( C6502::__PC() < MEM_2KB )
         {
            C6502::DISASSEMBLE();
         }
         else
         {
            CROM::DISASSEMBLE();
         }

         emit showMe();
         break;
      }
   }

   if ( C6502::__PC() < MEM_2KB )
   {
      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(C6502::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }
   else
   {
      switch ( ui->displayMode->currentIndex() )
      {
         case BROWSE_ASSEMBLY:
            assemblyViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(assemblyViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));
         break;
         case BROWSE_SOURCE:
            sourceViewModel->layoutChangedEvent();
            ui->tableView->setCurrentIndex(sourceViewModel->index(pasm_get_source_linenum(C6502::__PC()<0xC000?0:1,C6502::__PC())-1,0));
         break;
      }
   }
}

void CodeBrowserDialog::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   QModelIndex index = ui->tableView->currentIndex();
   bool added;
   int addr;

   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         addr = CROM::SLOC2ADDR(index.row());
      break;
      case BROWSE_SOURCE:
         addr = pasm_get_source_addr_from_linenum ( index.row()+1 );
      break;
   }

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
}

void CodeBrowserDialog::on_actionRun_to_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int addr;

   switch ( ui->displayMode->currentIndex() )
   {
      case BROWSE_ASSEMBLY:
         addr = CROM::SLOC2ADDR(index.row());
      break;
      case BROWSE_SOURCE:
         addr = pasm_get_source_addr_from_linenum ( index.row()+1 );
      break;
   }

   C6502::GOTO(addr);
}

void CodeBrowserDialog::on_displayMode_currentIndexChanged(int index)
{
   switch ( index )
   {
      case BROWSE_ASSEMBLY:
         ui->tableView->setModel(assemblyViewModel);
         assemblyViewModel->layoutChangedEvent();
      break;
      case BROWSE_SOURCE:
         ui->tableView->setModel(sourceViewModel);
         sourceViewModel->layoutChangedEvent();
      break;
   }

   updateDisassembly();

   ui->tableView->resizeColumnsToContents();
}
