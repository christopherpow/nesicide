#include "oamvisualizerdockwidget.h"
#include "ui_oamvisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

OAMVisualizerDockWidget::OAMVisualizerDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OAMVisualizerDockWidget)
{
   ui->setupUi(this);
   imgData = new char[256*256*4];
   memset(imgData,0xFF,256*256*4);

   CPPUDBG::OAMInspectorTV ( (int8_t*)imgData );
//   QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new COAMPreviewRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->updateScanline->setText ( "0" );
   ui->showVisible->setChecked ( false );
}

OAMVisualizerDockWidget::~OAMVisualizerDockWidget()
{
   delete ui;
   delete imgData;
}

void OAMVisualizerDockWidget::changeEvent(QEvent* e)
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

void OAMVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QDockWidget::showEvent(event);
   CPPUDBG::EnableOAMInspector(true);
   renderData();
}

void OAMVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QDockWidget::hideEvent(event);
   CPPUDBG::EnableOAMInspector(false);
}

void OAMVisualizerDockWidget::renderData()
{
   CPPUDBG::RENDEROAM();
   renderer->updateGL ();
}

void OAMVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void OAMVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void OAMVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)32 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void OAMVisualizerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void OAMVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void OAMVisualizerDockWidget::on_updateScanline_editingFinished()
{
   CPPUDBG::SetOAMViewerScanline ( ui->updateScanline->text().toInt() );
   renderData();
   renderer->repaint();
}

void OAMVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   CPPUDBG::SetOAMViewerShowVisible ( checked );
   renderData();
   renderer->repaint();
}
