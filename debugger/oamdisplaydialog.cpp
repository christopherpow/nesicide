#include "oamdisplaydialog.h"
#include "ui_oamdisplaydialog.h"

#include "cnessystempalette.h"
#include "cnesppu.h"

#include "main.h"

OAMDisplayDialog::OAMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OAMDisplayDialog)
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

    CPPU::OAMInspectorTV ( imgData );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );

    renderer = new COAMPreviewRenderer(ui->frame,imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();
}

void OAMDisplayDialog::changeEvent(QEvent *e)
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

void OAMDisplayDialog::colorChanged (const QColor &color)
{
   ui->col0PushButton->setText("");
   ui->col1PushButton->setText("");
   ui->col2PushButton->setText("");
   ui->col3PushButton->setText("");

   renderData();
   renderer->setBGColor(ui->col0PushButton->currentColor());
}

void OAMDisplayDialog::renderData()
{
   renderer->updateGL ();
}

OAMDisplayDialog::~OAMDisplayDialog()
{
   delete imgData;
   delete ui;
}

void OAMDisplayDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateScrollbars();
}

void OAMDisplayDialog::on_zoomSlider_sliderMoved(int position)
{
}

void OAMDisplayDialog::on_zoomSlider_actionTriggered(int action)
{
}

void OAMDisplayDialog::on_zoomSlider_valueChanged(int value)
{
    renderer->changeZoom(value);
    ui->zoomValueLabel->setText(QString::number(value).append("%"));
    updateScrollbars();
}

void OAMDisplayDialog::updateScrollbars()
{
    int value = ui->zoomSlider->value();
    int viewWidth = (float)256 * ((float)value / 100.0f);
    int viewHeight = (float)32 * ((float)value / 100.0f);
    ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
    ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->scrollY = ui->verticalScrollBar->value();
}

void OAMDisplayDialog::on_verticalScrollBar_actionTriggered(int action)
{
}

void OAMDisplayDialog::on_horizontalScrollBar_actionTriggered(int action)
{
}

void OAMDisplayDialog::on_horizontalScrollBar_valueChanged(int value)
{
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->repaint();
}

void OAMDisplayDialog::on_verticalScrollBar_valueChanged(int value)
{
    renderer->scrollY = ui->verticalScrollBar->value();
    renderer->repaint();
}
