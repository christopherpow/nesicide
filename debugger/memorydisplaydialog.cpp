#include "memorydisplaydialog.h"
#include "ui_memorydisplaydialog.h"

#include "emulator_core.h"

#include "dbg_cnes.h"

#include "inspectorregistry.h"
#include "main.h"

#include <QMessageBox>

MemoryDisplayDialog::MemoryDisplayDialog(QWidget* parent, eMemoryType display) :
   QDialog(parent),
   ui(new Ui::MemoryDisplayDialog),
   m_display(display)
{
   ui->setupUi(this);
   model = new CDebuggerMemoryDisplayModel(this,display);
   ui->tableView->setModel(model);
   
   // Connect signals to the UI to have the UI update.
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateMemory()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateMemory()) );

   // Connect signals to the models to have the model update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), model, SLOT(update()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), model, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), model, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), model, SLOT(update()) );
   QObject::connect ( this, SIGNAL(showMe(eMemoryType)), model, SLOT(update()) );
}

MemoryDisplayDialog::~MemoryDisplayDialog()
{
   delete ui;
   delete model;
}

void MemoryDisplayDialog::showEvent(QShowEvent* e)
{
   ui->tableView->resizeColumnsToContents();
   updateMemory();
}

void MemoryDisplayDialog::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu menu;

   switch ( m_display )
   {
      case eMemory_CPU:
      case eMemory_cartROM:
      case eMemory_cartSRAM:
      case eMemory_cartEXRAM:
      case eMemory_cartMapper:
         menu.addAction(ui->actionBreak_on_CPU_access_here);
         menu.addAction(ui->actionBreak_on_CPU_read_here);
         menu.addAction(ui->actionBreak_on_CPU_write_here);
         break;
      default:
         break;
   }

   menu.exec(e->globalPos());
}

void MemoryDisplayDialog::changeEvent(QEvent* e)
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

void MemoryDisplayDialog::updateMemory ()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   eMemoryType memoryType = model->memoryType();
   int idx;
   int row = 0, col = 0;
   int low = 0, high = 0;
   int itemActual;

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         if ( (pBreakpoint->type == eBreakOnCPUMemoryAccess) ||
               (pBreakpoint->type == eBreakOnCPUMemoryRead) ||
               (pBreakpoint->type == eBreakOnCPUMemoryWrite) ||
               (pBreakpoint->type == eBreakOnPPUPortalAccess) ||
               (pBreakpoint->type == eBreakOnPPUPortalRead) ||
               (pBreakpoint->type == eBreakOnPPUPortalWrite) )
         {
            // Check memory range...
            low = model->memoryBottom();
            high = model->memoryTop();

            if ( (pBreakpoint->itemActual >= low) &&
                  (pBreakpoint->itemActual <= high) )
            {
               if ( ((pBreakpoint->target == eBreakInCPU) &&
                     ((memoryType == eMemory_CPU) ||
                      (memoryType == eMemory_cartSRAM) ||
                      (memoryType == eMemory_cartEXRAM) ||
                      (memoryType == eMemory_cartROM))) ||
                     ((pBreakpoint->target == eBreakInPPU) &&
                      ((memoryType == eMemory_PPU) ||
                       (memoryType == eMemory_PPUpalette) ||
                       (memoryType == eMemory_cartCHRMEM))) )
               {
                  // Change memory address into row/column of display...
                  itemActual = pBreakpoint->itemActual - model->memoryBottom();
                  row = itemActual/model->columnCount();
                  col = itemActual%model->columnCount();

                  // Update display...
                  emit showMe(memoryType);
                  ui->tableView->resizeColumnsToContents();
                  ui->tableView->setCurrentIndex(model->index(row,col));
               }
            }
         }
      }
   }
}

void MemoryDisplayDialog::on_actionBreak_on_CPU_access_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = model->memoryBottom()+(row*model->columnCount())+col;
   bool added;

   added = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         0,
                                         eBreakpointDataPure,
                                         0 );

   if ( !added )
   {
      QMessageBox::information(0, "Error", "Cannot add breakpoint, already have 8 defined.");
   }

   InspectorRegistry::getInspector("Breakpoints")->hide();
   InspectorRegistry::getInspector("Breakpoints")->show();
}

void MemoryDisplayDialog::on_actionBreak_on_CPU_read_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = model->memoryBottom()+(row*model->columnCount())+col;
   bool added;

   added = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryRead,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         0,
                                         eBreakpointDataPure,
                                         0 );

   if ( !added )
   {
      QMessageBox::information(0, "Error", "Cannot add breakpoint, already have 8 defined.");
   }

   InspectorRegistry::getInspector("Breakpoints")->hide();
   InspectorRegistry::getInspector("Breakpoints")->show();
}

void MemoryDisplayDialog::on_actionBreak_on_CPU_write_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = model->memoryBottom()+(row*model->columnCount())+col;
   bool added;

   added = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryWrite,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         0,
                                         eBreakpointDataPure,
                                         0 );

   if ( !added )
   {
      QMessageBox::information(0, "Error", "Cannot add breakpoint, already have 8 defined.");
   }

   InspectorRegistry::getInspector("Breakpoints")->hide();
   InspectorRegistry::getInspector("Breakpoints")->show();
}
