#include "executionvisualizerdialog.h"
#include "ui_executionvisualizerdialog.h"

#include "cnes6502.h"

#include "main.h"

ExecutionVisualizerDialog::ExecutionVisualizerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExecutionVisualizerDialog)
{
    ui->setupUi(this);
    imgData = new char[512*512*3];
    memset ( imgData, 0, sizeof(imgData) );

    C6502::ExecutionVisualizerInspectorTV ( imgData );

    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );
    QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()) );

    renderer = new CExecutionVisualizerRenderer(ui->frame,imgData);
    ui->frame->layout()->addWidget(renderer);
    ui->frame->layout()->update();
}

void ExecutionVisualizerDialog::changeEvent(QEvent *e)
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

void ExecutionVisualizerDialog::showEvent(QShowEvent *event)
{
   QDialog::showEvent(event);
   renderData();
}

void ExecutionVisualizerDialog::hideEvent(QHideEvent *event)
{
   QDialog::hideEvent(event);
}

void ExecutionVisualizerDialog::renderData()
{
   if ( isVisible() )
   {
      C6502::RENDEREXECUTIONVISUALIZER();
      renderer->updateGL ();
   }
}

ExecutionVisualizerDialog::~ExecutionVisualizerDialog()
{
   delete imgData;
   delete ui;
}

void ExecutionVisualizerDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateScrollbars();
}

void ExecutionVisualizerDialog::on_zoomSlider_valueChanged(int value)
{
    renderer->changeZoom(value);
    ui->zoomValueLabel->setText(QString::number(value).append("%"));
    updateScrollbars();
}

void ExecutionVisualizerDialog::updateScrollbars()
{
    int value = ui->zoomSlider->value();
    int viewWidth = (float)341 * ((float)value / 100.0f);
    int viewHeight = (float)262 * ((float)value / 100.0f);
    ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
    ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->scrollY = ui->verticalScrollBar->value();
}

void ExecutionVisualizerDialog::on_horizontalScrollBar_valueChanged(int value)
{
    renderer->scrollX = ui->horizontalScrollBar->value();
    renderer->repaint();
}

void ExecutionVisualizerDialog::on_verticalScrollBar_valueChanged(int value)
{
    renderer->scrollY = ui->verticalScrollBar->value();
    renderer->repaint();
}
