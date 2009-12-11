#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CHRROMDisplayDialog)
{
    ui->setupUi(this);

    char *imgData = new char[256*256*3];

    // ===================================================================================
    for (int y = 0; y< 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            imgData[(y * 256 * 3) + (x * 3) + 0] = (x & 1) ? 255 : 0;
            imgData[(y * 256 * 3) + (x * 3) + 1] = (x & 1) ? 255 : 0;
            imgData[(y * 256 * 3) + (x * 3) + 2] = (x & 1) ? 255 : 0;
        }
    }
    // ===================================================================================

    renderer = new CCHRROMPreviewRenderer(ui->frame, imgData);
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
}

void CHRROMDisplayDialog::on_zoomSlider_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_zoomSlider_valueChanged(int value)
{
    renderer->changeZoom(value);
    ui->zoomValueLabel->setText(QString::number(value).append("%"));
}
