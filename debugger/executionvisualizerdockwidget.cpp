#include "executionvisualizerdockwidget.h"
#include "ui_executionvisualizerdockwidget.h"

#include "dbg_cnes6502.h"

#include "main.h"

ExecutionVisualizerDockWidget::ExecutionVisualizerDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ExecutionVisualizerDockWidget)
{
   int i;

   ui->setupUi(this);
   imgData = new char[512*512*4];

   // Clear image...
   for ( i = 0; i < 512*512*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }
   C6502DBG::ExecutionVisualizerInspectorTV ( (int8_t*)imgData );

   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CExecutionVisualizerRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
}

ExecutionVisualizerDockWidget::~ExecutionVisualizerDockWidget()
{
   delete ui;
   delete imgData;
}

void ExecutionVisualizerDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void ExecutionVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   renderData();
}

void ExecutionVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
}

void ExecutionVisualizerDockWidget::renderData()
{
   C6502DBG::RENDEREXECUTIONVISUALIZER();
   renderer->updateGL ();
}

void ExecutionVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void ExecutionVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void ExecutionVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)341 * ((float)value / 100.0f);
   int viewHeight = (float)262 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void ExecutionVisualizerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void ExecutionVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}
