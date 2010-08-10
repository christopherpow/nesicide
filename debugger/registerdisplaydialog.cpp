#include "registerdisplaydialog.h"
#include "ui_registerdisplaydialog.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"
#include "dbg_cnesapu.h"
#include "dbg_cnesmappers.h"

#include "main.h"

RegisterDisplayDialog::RegisterDisplayDialog(QWidget *parent, eMemoryType display) :
    QDialog(parent),
    ui(new Ui::RegisterDisplayDialog)
{
    ui->setupUi(this);
    binaryModel = new CDebuggerMemoryDisplayModel(this,display);
    ui->binaryView->setModel(binaryModel);
    bitfieldModel = new CDebuggerRegisterDisplayModel(this,display);
    bitfieldDelegate = new CDebuggerRegisterComboBoxDelegate();
    ui->bitfieldView->setModel(bitfieldModel);
    ui->bitfieldView->setItemDelegate(bitfieldDelegate);
    m_display = display;
    switch ( display )
    {
       case eMemory_CPUregs:
         m_tblRegisters = C6502DBG::REGISTERS();
       break;
       case eMemory_PPUregs:
          m_tblRegisters = CPPUDBG::REGISTERS();
       break;
       case eMemory_IOregs:
          m_tblRegisters = CAPUDBG::REGISTERS();
       break;
       case eMemory_PPUoam:
          m_tblRegisters = tblOAMRegisters;
       break;
       case eMemory_cartMapper:
          m_tblRegisters = CROMDBG::REGISTERS();
       break;
       default:
          m_tblRegisters = NULL;
       break;
    }
    m_register = 0;
    if ( m_display == eMemory_cartMapper )
    {
       ui->info->setVisible(true);
    }
    else
    {
       ui->info->setVisible(false);
    }
    ui->label->setText ( "" );
    QObject::connect ( bitfieldModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateMemory()) );
    QObject::connect ( binaryModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateMemory()) );

    QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(updateMemory()) );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
    QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateMemory()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateMemory()) );
}

RegisterDisplayDialog::~RegisterDisplayDialog()
{
   delete ui;
   delete binaryModel;
   delete bitfieldModel;
   delete bitfieldDelegate;
}

void RegisterDisplayDialog::showEvent(QShowEvent *e)
{
   QDialog::showEvent(e);
   updateMemory();
}

void RegisterDisplayDialog::contextMenuEvent(QContextMenuEvent *)
{
}

void RegisterDisplayDialog::changeEvent(QEvent *e)
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

void RegisterDisplayDialog::updateMemory ()
{
   CBreakpointInfo* pBreakpoints = CNESDBG::BREAKPOINTS();
   eMemoryType memoryType = binaryModel->memoryType();
   int idx;
   int row = 0, col = 0;
   int low = 0, high = 0;
   int itemActual;
   char buffer [ 128 ];

   if ( isVisible() )
   {
      binaryModel->layoutChangedEvent();
      bitfieldModel->layoutChangedEvent();
      ui->binaryView->resizeColumnsToContents();
   }

   if ( m_display == eMemory_cartMapper )
   {
      m_tblRegisters = CROMDBG::REGISTERS();
      sprintf ( buffer, "Mapper %d: %s", CROMDBG::MAPPER(), mapperNameFromID(CROMDBG::MAPPER()) );
      ui->info->setText ( buffer );
   }
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( pBreakpoint->hit )
      {
         if ( (pBreakpoint->type == eBreakOnOAMPortalAccess) ||
              (pBreakpoint->type == eBreakOnOAMPortalRead) ||
              (pBreakpoint->type == eBreakOnOAMPortalWrite) )
         {
            // Check memory range...
            low = binaryModel->memoryBottom();
            high = binaryModel->memoryTop();

            if ( (pBreakpoint->itemActual >= low) &&
                 (pBreakpoint->itemActual <= high) )
            {
               if ( ((pBreakpoint->target == eBreakInPPU) &&
                    (memoryType == eMemory_PPUoam)) )
               {
                  // Change memory address into row/column of display...
                  itemActual = pBreakpoint->itemActual - binaryModel->memoryBottom();
                  row = itemActual/binaryModel->columnCount();
                  col = itemActual%binaryModel->columnCount();

                  // Update display...
                  emit showMe(memoryType);
                  ui->binaryView->setCurrentIndex(binaryModel->index(row,col));
                  on_binaryView_clicked(binaryModel->index(row,col));
                  ui->bitfieldView->setCurrentIndex(bitfieldModel->index(row,col));
               }
            }
         }
         else if ( (pBreakpoint->type == eBreakOnCPUState) ||
                   (pBreakpoint->type == eBreakOnPPUState) ||
                   (pBreakpoint->type == eBreakOnAPUState) ||
                   (pBreakpoint->type == eBreakOnMapperState) )
         {
            if ( ((pBreakpoint->target == eBreakInCPU) &&
                 (memoryType == eMemory_CPUregs)) ||
                 ((pBreakpoint->target == eBreakInPPU) &&
                 (memoryType == eMemory_PPUregs)) ||
                 ((pBreakpoint->target == eBreakInAPU) &&
                 (memoryType == eMemory_IOregs)) ||
                 ((pBreakpoint->target == eBreakInMapper) &&
                 (memoryType == eMemory_cartMapper)) )
            {
               // Change register into row/column of display...
               row = pBreakpoint->item1/binaryModel->columnCount();
               col = pBreakpoint->item1%binaryModel->columnCount();

               // Update display...
               emit showMe(memoryType);
               ui->binaryView->setCurrentIndex(binaryModel->index(row,col));
               on_binaryView_clicked(binaryModel->index(row,col));
               ui->bitfieldView->setCurrentIndex(bitfieldModel->index(pBreakpoint->item2,0));
            }
         }
      }
   }
}

void RegisterDisplayDialog::on_binaryView_clicked(QModelIndex index)
{
   char buffer [ 128 ];
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_doubleClicked(QModelIndex index)
{
   char buffer [ 128 ];
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_bitfieldView_clicked(QModelIndex index)
{
   bitfieldDelegate->setBitfield ( m_tblRegisters[m_register]->GetBitfield(index.row()) );
   binaryModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_bitfieldView_doubleClicked(QModelIndex index)
{
   bitfieldDelegate->setBitfield ( m_tblRegisters[m_register]->GetBitfield(index.row()) );
   binaryModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_pressed(QModelIndex index)
{
   char buffer [ 128 ];
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_activated(QModelIndex index)
{
   char buffer [ 128 ];
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_entered(QModelIndex index)
{
   char buffer [ 128 ];
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}
