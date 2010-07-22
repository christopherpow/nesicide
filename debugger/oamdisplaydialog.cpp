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

    CPPU::OAMInspectorTV ( imgData );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

    renderer = new COAMPreviewRenderer(ui->frame,imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();

    ui->updateScanline->setText ( "0" );
    ui->showVisible->setChecked ( false );
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

void OAMDisplayDialog::showEvent(QShowEvent *event)
{
   QDialog::showEvent(event);
   CPPU::EnableOAMInspector(true);
   renderData();
}

void OAMDisplayDialog::hideEvent(QHideEvent *event)
{
   QDialog::hideEvent(event);
   CPPU::EnableOAMInspector(false);
}

void OAMDisplayDialog::renderData()
{
   if ( this->isVisible() )
   {
      renderer->updateGL ();
   }
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

void OAMDisplayDialog::on_updateScanline_editingFinished()
{
    CPPU::SetOAMViewerScanline ( ui->updateScanline->text().toInt() );
}

void OAMDisplayDialog::on_showVisible_toggled(bool checked)
{
    CPPU::SetOAMViewerShowVisible ( checked );
}
