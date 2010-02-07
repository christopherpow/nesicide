#include "registerdisplaydialog.h"
#include "ui_registerdisplaydialog.h"

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

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
         m_tblRegisters = C6502::REGISTERS();
       break;
       case eMemory_PPUregs:
          m_tblRegisters = CPPU::REGISTERS();
       break;
       case eMemory_IOregs:
          m_tblRegisters = CAPU::REGISTERS();
       break;
       case eMemory_PPUoam:
          m_tblRegisters = tblOAMRegisters;
       break;
       case eMemory_cartMapper:
          m_tblRegisters = CROM::REGISTERS();
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

    QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
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
   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
   ui->binaryView->resizeColumnsToContents();
}

void RegisterDisplayDialog::contextMenuEvent(QContextMenuEvent *e)
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

void RegisterDisplayDialog::cartridgeLoaded ()
{
   char buffer [ 128 ];

   if ( m_display == eMemory_cartMapper )
   {
      m_tblRegisters = CROM::REGISTERS();
      sprintf ( buffer, "Mapper %d: ", CROM::MAPPER() );
      ui->info->setText ( buffer );
   }
   if ( m_tblRegisters )
   {
      sprintf ( buffer, "$%04X: %s", m_tblRegisters[m_register]->GetAddr(), m_tblRegisters[m_register]->GetName() );
      ui->label->setText ( buffer );
   }
   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::updateMemory ()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   eMemoryType memoryType = binaryModel->memoryType();
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
               // Update display...
               emit showMe(memoryType);
               ui->binaryView->setCurrentIndex(binaryModel->index(0,pBreakpoint->item1));
               on_binaryView_clicked(binaryModel->index(0,pBreakpoint->item1));
               ui->bitfieldView->setCurrentIndex(bitfieldModel->index(pBreakpoint->item2,0));
            }
         }
      }
   }

   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
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
