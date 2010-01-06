#include "memorydisplaydialog.h"
#include "ui_memorydisplaydialog.h"

#include "cnes6502.h"

#include "main.h"

MemoryDisplayDialog::MemoryDisplayDialog(QWidget *parent, eMemoryType display) :
    QDialog(parent),
    ui(new Ui::MemoryDisplayDialog)
{
    ui->setupUi(this);
    model = new CDebuggerMemoryDisplayModel(this,display);
    ui->tableView->setModel(model);

    QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateMemory()) );
}

MemoryDisplayDialog::~MemoryDisplayDialog()
{
    delete ui;
    delete model;
}

void MemoryDisplayDialog::contextMenuEvent(QContextMenuEvent *e)
{
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

void MemoryDisplayDialog::cartridgeLoaded ()
{
   model->layoutChangedEvent();
}

void MemoryDisplayDialog::updateMemory ()
{
   model->layoutChangedEvent();
}
