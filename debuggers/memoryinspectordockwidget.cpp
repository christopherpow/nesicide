#include "memoryinspectordockwidget.h"
#include "ui_memoryinspectordockwidget.h"

#include "nes_emulator_core.h"

#include "dbg_cnes.h"

#include "main.h"

#include <QMessageBox>

MemoryInspectorDockWidget::MemoryInspectorDockWidget(memDBFunc memDB, QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::MemoryInspectorDockWidget)
{
   ui->setupUi(this);

   model = new CDebuggerMemoryDisplayModel(memDB);
   delegate = new CDebuggerNumericItemDelegate();
   ui->tableView->setModel(model);
   ui->tableView->setItemDelegate(delegate);

   // Connect signals to the UI to have the UI update.
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateMemory()) );

   // Connect signals to the models to have the model update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), model, SLOT(update()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), model, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), model, SLOT(update()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), model, SLOT(update()) );

   m_memDB = memDB;
}

MemoryInspectorDockWidget::~MemoryInspectorDockWidget()
{
   delete ui;
   delete model;
   delete delegate;
}

void MemoryInspectorDockWidget::showEvent(QShowEvent* e)
{
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   model->update();
   ui->tableView->resizeColumnsToContents();
}

void MemoryInspectorDockWidget::hideEvent(QHideEvent* e)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
}

void MemoryInspectorDockWidget::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu menu;

   menu.addAction(ui->actionBreak_on_CPU_access_here);
   menu.addAction(ui->actionBreak_on_CPU_read_here);
   menu.addAction(ui->actionBreak_on_CPU_write_here);

   menu.exec(e->globalPos());

   emit breakpointsChanged();
}

void MemoryInspectorDockWidget::changeEvent(QEvent* e)
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

void MemoryInspectorDockWidget::updateMemory ()
{
#if 0
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
                  show();
                  ui->tableView->resizeColumnsToContents();
                  ui->tableView->setCurrentIndex(model->index(row,col));
               }
            }
         }
      }
   }
#endif
}

void MemoryInspectorDockWidget::on_actionBreak_on_CPU_access_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB()->GetBase()+(row*m_memDB()->GetNumColumns())+col;
   int bpIdx;

   bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         eBreakIfAnything,
                                         eBreakpointDataPure,
                                         0,
                                         true );

   if ( bpIdx < 0 )
   {
      QString str;
      str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
      QMessageBox::information(0, "Error", str);
   }
}

void MemoryInspectorDockWidget::on_actionBreak_on_CPU_read_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB()->GetBase()+(row*m_memDB()->GetNumColumns())+col;
   int bpIdx;

   bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryRead,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         eBreakIfAnything,
                                         eBreakpointDataPure,
                                         0,
                                         true );

   if ( bpIdx < 0 )
   {
      QString str;
      str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
      QMessageBox::information(0, "Error", str);
   }
}

void MemoryInspectorDockWidget::on_actionBreak_on_CPU_write_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB()->GetBase()+(row*m_memDB()->GetNumColumns())+col;
   int bpIdx;

   bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryWrite,
                                         eBreakpointItemAddress,
                                         0,
                                         addr,
                                         addr,
                                         addr,
                                         eBreakpointConditionTest,
                                         eBreakIfAnything,
                                         eBreakpointDataPure,
                                         0,
                                         true );

   if ( bpIdx < 0 )
   {
      QString str;
      str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
      QMessageBox::information(0, "Error", str);
   }
}
