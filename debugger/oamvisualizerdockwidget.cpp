#include "oamvisualizerdockwidget.h"
#include "ui_oamvisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

OAMVisualizerDockWidget::OAMVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::OAMVisualizerDockWidget)
{
   int i;

   ui->setupUi(this);
   imgData = new char[256*256*4];

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }
   CPPUDBG::OAMInspectorTV ( (int8_t*)imgData );

   renderer = new COAMPreviewRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->updateScanline->setText ( "0" );
   ui->showVisible->setChecked ( false );

   pThread = new DebuggerUpdateThread(&CPPUDBG::RENDEROAM);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

OAMVisualizerDockWidget::~OAMVisualizerDockWidget()
{
   delete ui;
   delete imgData;
   delete pThread;
}

void OAMVisualizerDockWidget::changeEvent(QEvent* e)
{
   CDebuggerBase::changeEvent(e);

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
   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   updateScrollbars();

   pThread->updateDebuggers();
}

void OAMVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void OAMVisualizerDockWidget::renderData()
{
   renderer->updateGL ();
}

void OAMVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void OAMVisualizerDockWidget::mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == Qt::LeftButton )
   {
      pressPos = event->pos();
   }
}

void OAMVisualizerDockWidget::mouseMoveEvent(QMouseEvent *event)
{
   int zf = ui->zoomSlider->value();
   zf = zf-(zf%100);
   zf /= 100;

   if ( event->buttons() == Qt::LeftButton )
   {
      ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value()-((event->pos().x()/zf)-(pressPos.x()/zf)));
      ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()-((event->pos().y()/zf)-(pressPos.y()/zf)));
   }
   else if ( event->buttons() == Qt::RightButton )
   {
      if ( event->pos().y() < pressPos.y() )
      {
         ui->zoomSlider->setValue(ui->zoomSlider->value()+100);
      }
      else
      {
         ui->zoomSlider->setValue(ui->zoomSlider->value()-100);
      }
   }
   pressPos = event->pos();
}

void OAMVisualizerDockWidget::wheelEvent(QWheelEvent *event)
{
   if ( event->delta() > 0 )
   {
      ui->zoomSlider->setValue(ui->zoomSlider->value()+100);
   }
   else if ( event->delta() < 0 )
   {
      ui->zoomSlider->setValue(ui->zoomSlider->value()-100);
   }
}

void OAMVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void OAMVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
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
   renderer->update();
}

void OAMVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->update();
}

void OAMVisualizerDockWidget::on_updateScanline_editingFinished()
{
   CPPUDBG::SetOAMViewerScanline ( ui->updateScanline->text().toInt() );
   renderer->update();
}

void OAMVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   CPPUDBG::SetOAMViewerShowVisible ( checked );
   renderer->update();
}
