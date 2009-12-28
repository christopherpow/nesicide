#include "memorydisplaydialog.h"
#include "ui_memorydisplaydialog.h"

#include "cnes6502.h"

#include "main.h"

MemoryDisplayDialog::MemoryDisplayDialog(QWidget *parent, eMemoryType display) :
    QDialog(parent),
    ui(new Ui::MemoryDisplayDialog)
{
    ui->setupUi(this);
    tableViewModel = new CDebuggerMemoryDisplayModel(this,display);
    ui->tableView->setModel(tableViewModel);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
}

MemoryDisplayDialog::~MemoryDisplayDialog()
{
    delete ui;
    delete tableViewModel;
}

void MemoryDisplayDialog::changeEvent(QEvent *e)
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

void MemoryDisplayDialog::updateMemory ()
{
   tableViewModel->layoutChangedEvent();
}
