#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>

#include "memoryinspectordockwidget.h"
#include "ui_memoryinspectordockwidget.h"

#include "cobjectregistry.h"
#include "environmentsettingsdialog.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

MemoryInspectorDockWidget::MemoryInspectorDockWidget(memDBFunc memDB,CBreakpointInfo* pBreakpoints,QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::MemoryInspectorDockWidget)
{
   ui->setupUi(this);

   m_pBreakpoints = pBreakpoints;

   model = new CDebuggerMemoryDisplayModel(memDB);
   delegate = new CDebuggerNumericItemDelegate();
   ui->tableView->setModel(model);
   ui->tableView->setItemDelegate(delegate);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->tableView->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->tableView->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->tableView->setFont(QFont("Consolas", 11));
#endif

   ui->tableView->resizeRowsToContents();
   ui->tableView->hide();

   m_memDBFunc = memDB;
   m_memDB = memDB();

   m_editor = new QHexEdit();

   m_editor->setAddressAreaBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_editor->setAddressAreaForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());
   m_editor->setHexCaps(true);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   m_editor->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   m_editor->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   m_editor->setFont(QFont("Consolas", 11));
#endif
   m_editor->setAddressOffset(m_memDB->GetBase());

   ui->gridLayout->addWidget(m_editor);
}

MemoryInspectorDockWidget::~MemoryInspectorDockWidget()
{
   delete ui;
   delete model;
   delete delegate;
}

void MemoryInspectorDockWidget::updateTargetMachine(QString target)
{
   if ( target.compare("none") )
   {
      QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
      QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

      QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateMemory()) );
      QObject::connect ( emulator, SIGNAL(machineReady()), this, SLOT(updateMemory()));
      QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateMemory()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateMemory()) );
   }
}

void MemoryInspectorDockWidget::showEvent(QShowEvent* /*e*/)
{
   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), model, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(machineReady()), model, SLOT(update()));
      QObject::connect ( emulator, SIGNAL(emulatorReset()), model, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), model, SLOT(update()) );
      QObject::connect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   }
   model->update();
   ui->tableView->resizeColumnsToContents();
}

void MemoryInspectorDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect ( breakpointWatcher, SIGNAL(breakpointHit()), model, SLOT(update()) );
      QObject::disconnect ( emulator, SIGNAL(machineReady()), model, SLOT(update()));
      QObject::disconnect ( emulator, SIGNAL(emulatorReset()), model, SLOT(update()) );
      QObject::disconnect ( emulator, SIGNAL(emulatorPaused(bool)), model, SLOT(update()) );
      QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   }
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
   int memoryType = model->memoryType();
   int idx;
   int row = 0, col = 0;
   int low = 0, high = 0;
   int itemActual;

   m_memDB = m_memDBFunc();

   m_snapshot.clear();
   for ( idx = 0; idx < m_memDB->GetSize(); idx++ )
   {
      m_snapshot += m_memDB->Get(idx);
   }
   qint64 cp = m_editor->cursorPosition();
   m_editor->setData(m_snapshot);
   m_editor->setCursorPosition(cp);
   m_editor->ensureVisible();

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < m_pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(idx);

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
                       (memoryType == eMemory_cartCHRMEM) ||
                       (memoryType == eMemory_cartVRAM))) )
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
}

void MemoryInspectorDockWidget::on_actionBreak_on_CPU_access_here_triggered()
{
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB->GetBase()+(row*m_memDB->GetNumColumns())+col;
   int bpIdx;

   bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                           eBreakpointItemAddress,
                                           0,
                                           addr,
                                           addr,
                                           addr,
                                           0xFFFF,
                                           false,
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
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB->GetBase()+(row*m_memDB->GetNumColumns())+col;
   int bpIdx;

   bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryRead,
                                           eBreakpointItemAddress,
                                           0,
                                           addr,
                                           addr,
                                           addr,
                                           0xFFFF,
                                           false,
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
   QModelIndex index = ui->tableView->currentIndex();
   int row = index.row();
   int col = index.column();
   int addr = m_memDB->GetBase()+(row*m_memDB->GetNumColumns())+col;
   int bpIdx;

   bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryWrite,
                                           eBreakpointItemAddress,
                                           0,
                                           addr,
                                           addr,
                                           addr,
                                           0xFFFF,
                                           false,
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

void MemoryInspectorDockWidget::snapToHandler(QString item)
{
   int memoryType = model->memoryType();
   uint32_t addr = 0xFFFFFFFF;
   int row = 0, col = 0;
   int low = 0, high = 0;
   int itemActual;

   // Restrict to CPU memory types.
   if ( (memoryType == eMemory_CPU) ||
        (memoryType == eMemory_cartSRAM) ||
        (memoryType == eMemory_cartEXRAM) ||
        (memoryType == eMemory_cartROM) )
   {
      // Check memory range...
      low = model->memoryBottom();
      high = model->memoryTop();

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

         if ( (addr >= low) &&
               (addr <= high) )
         {
            // Change memory address into row/column of display...
            itemActual = addr - model->memoryBottom();
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

void MemoryInspectorDockWidget::applyEnvironmentSettings()
{
   m_editor->setAddressAreaBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_editor->setAddressAreaForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());
   m_editor->update();
}
