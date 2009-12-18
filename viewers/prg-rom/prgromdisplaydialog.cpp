#include "prgromdisplaydialog.h"
#include "ui_prgromdisplaydialog.h"

PRGROMDisplayDialog::PRGROMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PRGROMDisplayDialog)
{
    ui->setupUi(this);
}

PRGROMDisplayDialog::~PRGROMDisplayDialog()
{
    delete ui;
}

void PRGROMDisplayDialog::changeEvent(QEvent *e)
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

void PRGROMDisplayDialog::setRomData(QString data)
{
    ui->textBrowser->setText(data);
}
