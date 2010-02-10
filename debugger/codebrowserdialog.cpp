#include "codebrowserdialog.h"
#include "ui_codebrowserdialog.h"

#include "cnes6502.h"
#include "cnesrom.h"

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
   tableViewModel->layoutChangedEvent();

   ui->tableView->resizeColumnsToContents();
}

void CodeBrowserDialog::contextMenuEvent(QContextMenuEvent *e)
{
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

   ui->tableView->setCurrentIndex(tableViewModel->index(CROM::ADDR2SLOC(C6502::__PC()),0));

   tableViewModel->layoutChangedEvent();
}

void CodeBrowserDialog::updateBrowser()
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
         CROM::DISASSEMBLE();

         emit showMe();

         ui->tableView->setCurrentIndex(tableViewModel->index(CROM::ADDR2SLOC(pBreakpoint->itemActual),0));
      }
   }

   tableViewModel->layoutChangedEvent();
}
