#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CHRROMDisplayDialog)
{
    UINT ppuAddr = 0x0000;
    unsigned char patternData1;
    unsigned char patternData2;
    unsigned char bit1, bit2;
    unsigned char colorIdx;

    ui->setupUi(this);

    char *imgData = new char[256*256*3];

    memset ( imgData, 0,sizeof(imgData));

    // four random colors...
            palette [ 0 ] [ 0 ] = 173;
            palette [ 0 ] [ 1 ] = 173;
            palette [ 0 ] [ 2 ] = 173;
            palette [ 1 ] [ 0 ] = 100;
            palette [ 1 ] [ 1 ] = 176;
            palette [ 1 ] [ 2 ] = 255;
            palette [ 2 ] [ 0 ] = 92;
            palette [ 2 ] [ 1 ] = 228;
            palette [ 2 ] [ 2 ] = 48;
            palette [ 3 ] [ 0 ] = 251;
            palette [ 3 ] [ 1 ] = 194;
            palette [ 3 ] [ 2 ] = 255;

    // ===================================================================================
    for (int y = 0; y < 128; y++)
    {
        for (int x = 0; x < 256; x += 8)
        {
            ppuAddr = ((y>>3)<<8)+((x%128)<<1)+(y&0x7);
            if ( x >= 128 ) ppuAddr += 0x1000;
            patternData1 = *(chrrom+ppuAddr);
            patternData2 = *(chrrom+ppuAddr+8);

            for ( int xf = 0; xf < 8; xf++ )
            {
               bit1 = (patternData1>>(7-(xf)))&0x1;
               bit2 = (patternData2>>(7-(xf)))&0x1;
               colorIdx = (bit1|(bit2<<1));
               imgData[(y * 256 * 3) + (x * 8 * 3) + (xf * 3) + 0] = palette[colorIdx][0];
               imgData[(y * 256 * 3) + (x * 8 * 3) + (xf * 3) + 1] = palette[colorIdx][1];
               imgData[(y * 256 * 3) + (x * 8 * 3) + (xf * 3) + 2] = palette[colorIdx][2];
           }
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
