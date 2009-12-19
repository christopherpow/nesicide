#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"
#include "cnessystempalette.h"

#include "main.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget *parent, qint8 *data) :
    QDialog(parent),
    ui(new Ui::CHRROMDisplayDialog)
{
    ui->setupUi(this);
    imgData = new char[256*256*3];
    for (int i=0; i<0x40; i++)
    {
        ui->col0PushButton->insertColor(CBasePalette::GetPalette(i), "", i);
        ui->col1PushButton->insertColor(CBasePalette::GetPalette(i), "", i);
        ui->col2PushButton->insertColor(CBasePalette::GetPalette(i), "", i);
        ui->col3PushButton->insertColor(CBasePalette::GetPalette(i), "", i);
    }

    ui->col0PushButton->setCurrentColor(CBasePalette::GetPalette(0x0D));
    ui->col1PushButton->setCurrentColor(CBasePalette::GetPalette(0x00));
    ui->col2PushButton->setCurrentColor(CBasePalette::GetPalette(0x10));
    ui->col3PushButton->setCurrentColor(CBasePalette::GetPalette(0x20));

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
    unsigned int ppuAddr = 0x0000;
    unsigned char patternData1;
    unsigned char patternData2;
    unsigned char bit1, bit2;
    unsigned char colorIdx;
    QColor color[4];

    memset ( imgData, 0,sizeof(imgData));

    color[0] = ui->col0PushButton->currentColor();
    color[1] = ui->col1PushButton->currentColor();
    color[2] = ui->col2PushButton->currentColor();
    color[3] = ui->col3PushButton->currentColor();

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
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 0] = color[colorIdx].red();
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 1] = color[colorIdx].green();
               imgData[(y * 256 * 3) + (x * 3) + (xf * 3) + 2] = color[colorIdx].blue();
           }
        }
    }
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
   delete imgData;
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
