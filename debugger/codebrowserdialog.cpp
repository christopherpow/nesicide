#include "codebrowserdialog.h"
#include "ui_codebrowserdialog.h"

#include "cnes6502.h"
#include "cnesrom.h"

#include "inspectorregistry.h"
#include "main.h"

CodeBrowserDialog::CodeBrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeBrowserDialog)
{
    ui->setupUi(this);
    tableViewModel = new CCodeBrowserDisplayModel(this);
    ui->tableView->setModel(tableViewModel);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateBrowser()) );
    QObject::connect ( emulator, SIGNAL(emulatorPaused()), this, SLOT(updateDisassembly()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateBrowser()) );
}

CodeBrowserDialog::~CodeBrowserDialog()
{
    delete ui;
    delete tableViewModel;
}

void CodeBrowserDialog::showEvent(QShowEvent* e)
{
   // Update display...
   CROM::DISASSEMBLE();

   tableViewModel->layoutChangedEvent();

   ui->tableView->setCurrentIndex(tableViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));

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
   // Update display...
   CROM::DISASSEMBLE();

   emit showMe();

   tableViewModel->layoutChangedEvent();

   ui->tableView->setCurrentIndex(tableViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));
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
         CROM::DISASSEMBLE();

         emit showMe();
         break;
      }
   }

   tableViewModel->layoutChangedEvent();

   ui->tableView->setCurrentIndex(tableViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));
}

void CodeBrowserDialog::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   QModelIndex index = ui->tableView->currentIndex();
   int addr = CROM::SLOC2ADDR(index.row());

   pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                 eBreakpointItemAddress,
                                 0,
                                 addr,
                                 addr,
                                 eBreakpointConditionNone,
                                 0,
                                 eBreakpointDataNone,
                                 0 );

   InspectorRegistry::getInspector("Breakpoints")->hide();
   InspectorRegistry::getInspector("Breakpoints")->show();
}

void CodeBrowserDialog::on_actionRun_to_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int addr = CROM::SLOC2ADDR(index.row());
   C6502::GOTO(addr);
}
