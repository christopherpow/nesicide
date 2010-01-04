#include "registerdisplaydialog.h"
#include "ui_registerdisplaydialog.h"

#include "cnes6502.h"
#include "cregisterdata.h"

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
    switch ( display )
    {
       case eMemory_PPUregs:
          m_tblRegisters = tblPPURegisters;
       break;
       case eMemory_IOregs:
          m_tblRegisters = tblAPURegisters;
       break;
       default:
          m_tblRegisters = NULL;
       break;
    }
    m_register = 0;
    QObject::connect ( bitfieldModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateMemory()) );
    QObject::connect ( binaryModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateMemory()) );

    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
}

RegisterDisplayDialog::~RegisterDisplayDialog()
{
   delete ui;
   delete binaryModel;
   delete bitfieldModel;
   delete bitfieldDelegate;
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
   binaryModel->layoutChangedEvent();
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_clicked(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}

void RegisterDisplayDialog::on_binaryView_doubleClicked(QModelIndex index)
{
   int cols = index.model()->columnCount();
   m_register = (index.row()*cols)+index.column();
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
   bitfieldModel->setRegister ( m_register );
   bitfieldModel->layoutChangedEvent();
}
