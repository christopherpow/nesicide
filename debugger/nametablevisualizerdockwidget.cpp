#include "nametablevisualizerdockwidget.h"
#include "ui_nametablevisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

NameTableVisualizerDockWidget::NameTableVisualizerDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::NameTableVisualizerDockWidget)
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
   CPPUDBG::NameTableInspectorTV((int8_t*)imgData);

   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CNameTablePreviewRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->showVisible->setChecked ( true );
}

NameTableVisualizerDockWidget::~NameTableVisualizerDockWidget()
{
   delete ui;
   delete imgData;
}

void NameTableVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   CPPUDBG::EnableNameTableInspector(true);
   renderData();
}

void NameTableVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   CPPUDBG::EnableNameTableInspector(false);
}

void NameTableVisualizerDockWidget::changeEvent(QEvent* e)
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

void NameTableVisualizerDockWidget::renderData()
{
   CPPUDBG::RENDERNAMETABLE();
   renderer->updateGL ();
}

void NameTableVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void NameTableVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void NameTableVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)512 * ((float)value / 100.0f);
   int viewHeight = (float)480 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void NameTableVisualizerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void NameTableVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void NameTableVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   CPPUDBG::SetPPUViewerShowVisible ( checked );
}
