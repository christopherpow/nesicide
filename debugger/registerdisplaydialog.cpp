#include "registerdisplaydialog.h"
#include "ui_registerdisplaydialog.h"

#include "cnes6502.h"

#include "main.h"

RegisterDisplayDialog::RegisterDisplayDialog(QWidget *parent, eMemoryType display) :
    QDialog(parent),
    ui(new Ui::RegisterDisplayDialog)
{
    ui->setupUi(this);
    binaryModel = new CDebuggerMemoryDisplayModel(this,display);
    ui->binaryView->setModel(binaryModel);
    bitfieldModel = new CDebuggerRegisterDisplayModel(this,display);
    ui->bitfieldView->setModel(bitfieldModel);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
}

RegisterDisplayDialog::~RegisterDisplayDialog()
{
   delete ui;
   delete binaryModel;
   delete bitfieldModel;
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
   bitfieldModel->setRegister ( (index.row()*cols)+index.column() );
}

void RegisterDisplayDialog::on_binaryView_doubleClicked(QModelIndex index)
{
   int cols = index.model()->columnCount();
   bitfieldModel->setRegister ( (index.row()*cols)+index.column() );
}
