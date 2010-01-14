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
   if ( m_display == eMemory_cartMapper )
   {
      m_tblRegisters = CROM::REGISTERS();
   }
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
   }
   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::updateMemory ()
{
   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_clicked(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_doubleClicked(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
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
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_activated(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_entered(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   if ( m_tblRegisters )
   {
      ui->label->setText ( m_tblRegisters[m_register]->GetName() );
   }
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}
