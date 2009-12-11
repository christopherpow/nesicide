#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CHRROMDisplayDialog)
{
    ui->setupUi(this);
    renderer = new CCHRROMPreviewRenderer(ui->frame);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
    delete ui;
}

void CHRROMDisplayDialog::changeEvent(QEvent *e)
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

void CHRROMDisplayDialog::on_zoomSlider_sliderMoved(int position)
{
    renderer->changeZoom(position);
    ui->zoomValueLabel->setText(QString::number(position).append("%"));
}
