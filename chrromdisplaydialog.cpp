#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"


const int defaultPalette[][3] = {
    {0x75, 0x75, 0x75}, {0x27, 0x1B, 0x8F}, {0x00, 0x00, 0xAB}, {0x47, 0x00, 0x9F}, {0x8F, 0x00, 0x77},
    {0xAB, 0x00, 0x13}, {0xA7, 0x00, 0x00}, {0x7F, 0x0B, 0x00}, {0x43, 0x2F, 0x00}, {0x00, 0x47, 0x00},
    {0x00, 0x51, 0x00}, {0x00, 0x3F, 0x17}, {0x1B, 0x3F, 0x5F}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}, {0xBC, 0xBC, 0xBC}, {0x00, 0x73, 0xEF}, {0x23, 0x3B, 0xEF}, {0x83, 0x00, 0xF3},
    {0xBF, 0x00, 0xBF}, {0xE7, 0x00, 0x5B}, {0xDB, 0x2B, 0x00}, {0xCB, 0x4F, 0x0F}, {0x8B, 0x73, 0x00},
    {0x00, 0x97, 0x00}, {0x00, 0xAB, 0x00}, {0x00, 0x93, 0x3B}, {0x00, 0x83, 0x8B}, {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0xFF, 0xFF, 0xFF}, {0x3F, 0xBF, 0xFF}, {0x5F, 0x97, 0xFF},
    {0xA7, 0x8B, 0xFD}, {0xF7, 0x7B, 0xFF}, {0xFF, 0x77, 0xB7}, {0xFF, 0x77, 0x63}, {0xFF, 0x9B, 0x3B},
    {0xF3, 0xBF, 0x3F}, {0x83, 0xD3, 0x13}, {0x4F, 0xDF, 0x4B}, {0x58, 0xF8, 0x98}, {0x00, 0xEB, 0xDB},
    {0x4F, 0x4F, 0x4F}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0xFF, 0xFF, 0xFF}, {0xAB, 0xE7, 0xFF},
    {0xC7, 0xD7, 0xFF}, {0xD7, 0xCB, 0xFF}, {0xFF, 0xC7, 0xFF}, {0xFF, 0xC7, 0xDB}, {0xFF, 0xBF, 0xB3},
    {0xFF, 0xDB, 0xAB}, {0xFF, 0xE7, 0xA3}, {0xE3, 0xFF, 0xA3}, {0xAB, 0xF3, 0xBF}, {0xB3, 0xFF, 0xCF},
    {0x9F, 0xFF, 0xF3}, {0xB8, 0xB8, 0xB8}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}
};

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget *parent, qint8 *data) :
    QDialog(parent),
    ui(new Ui::CHRROMDisplayDialog)
{
    ui->setupUi(this);
    imgData = new char[256*256*3];
    for (int i=0; i<0x3F; i++)
    {
        ui->col0PushButton->insertColor(QColor(defaultPalette[i][0], defaultPalette[i][1], defaultPalette[i][2]), "");
        ui->col1PushButton->insertColor(QColor(defaultPalette[i][0], defaultPalette[i][1], defaultPalette[i][2]), "");
        ui->col2PushButton->insertColor(QColor(defaultPalette[i][0], defaultPalette[i][1], defaultPalette[i][2]), "");
        ui->col3PushButton->insertColor(QColor(defaultPalette[i][0], defaultPalette[i][1], defaultPalette[i][2]), "");
    }

    ui->col0PushButton->setCurrentColor(QColor(0x00,0x00,0x00));
    ui->col1PushButton->setCurrentColor(QColor(0x66,0x66,0x66));
    ui->col2PushButton->setCurrentColor(QColor(0xCC,0xCC,0xCC));
    ui->col3PushButton->setCurrentColor(QColor(0xFF,0xFF,0xFF));

    ui->col0PushButton->setText("");
    ui->col1PushButton->setText("");
    ui->col2PushButton->setText("");
    ui->col3PushButton->setText("");

    connect(ui->col0PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
    connect(ui->col1PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
    connect(ui->col2PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
    connect(ui->col3PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));

    chrrom = data;
    renderData();

    renderer = new CCHRROMPreviewRenderer(ui->frame, imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();
}

void CHRROMDisplayDialog::colorChanged (const QColor &color)
{
    ui->col0PushButton->setText("");
    ui->col1PushButton->setText("");
    ui->col2PushButton->setText("");
    ui->col3PushButton->setText("");

    renderData();
    renderer->setBGColor(ui->col0PushButton->currentColor());
    renderer->reloadData(imgData);
}

void CHRROMDisplayDialog::renderData()
{
    UINT ppuAddr = 0x0000;
    unsigned char patternData1;
    unsigned char patternData2;
    unsigned char bit1, bit2;
    unsigned char colorIdx;

    memset ( imgData, 0,sizeof(imgData));

    palette [ 0 ] [ 0 ] = ui->col0PushButton->currentColor().red();
    palette [ 0 ] [ 1 ] = ui->col0PushButton->currentColor().green();
    palette [ 0 ] [ 2 ] = ui->col0PushButton->currentColor().blue();
    palette [ 1 ] [ 0 ] = ui->col1PushButton->currentColor().red();
    palette [ 1 ] [ 1 ] = ui->col1PushButton->currentColor().green();
    palette [ 1 ] [ 2 ] = ui->col1PushButton->currentColor().blue();
    palette [ 2 ] [ 0 ] = ui->col2PushButton->currentColor().red();
    palette [ 2 ] [ 1 ] = ui->col2PushButton->currentColor().green();
    palette [ 2 ] [ 2 ] = ui->col2PushButton->currentColor().blue();
    palette [ 3 ] [ 0 ] = ui->col3PushButton->currentColor().red();
    palette [ 3 ] [ 1 ] = ui->col3PushButton->currentColor().green();
    palette [ 3 ] [ 2 ] = ui->col3PushButton->currentColor().blue();

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
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 0] = palette[colorIdx][0];
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 1] = palette[colorIdx][1];
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 2] = palette[colorIdx][2];
           }
        }
    }
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
    delete ui;
}

void CHRROMDisplayDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateScrollbars();
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
    updateScrollbars();
}

void CHRROMDisplayDialog::updateScrollbars()
{
    int value = ui->zoomSlider->value();
    int viewWidth = (float)256 * ((float)value / 100.0f);
    int viewHeight = (float)128 * ((float)value / 100.0f);
    ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
    ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->scrollY = ui->verticalScrollBar->value();
}

void CHRROMDisplayDialog::on_verticalScrollBar_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_horizontalScrollBar_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_horizontalScrollBar_valueChanged(int value)
{
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->repaint();
}

void CHRROMDisplayDialog::on_verticalScrollBar_valueChanged(int value)
{
    renderer->scrollY = ui->verticalScrollBar->value();
    renderer->repaint();
}
