#include "nametablevisualizerdockwidget.h"
#include "ui_nametablevisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

NameTableVisualizerDockWidget::NameTableVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
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

   renderer = new CNameTablePreviewRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->showVisible->setChecked ( true );

   pThread = new DebuggerUpdateThread(&CPPUDBG::RENDERNAMETABLE);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

NameTableVisualizerDockWidget::~NameTableVisualizerDockWidget()
{
   delete ui;
   delete imgData;
   delete pThread;
}

void NameTableVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   updateScrollbars();

   pThread->updateDebuggers();
}

void NameTableVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void NameTableVisualizerDockWidget::changeEvent(QEvent* e)
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

void NameTableVisualizerDockWidget::mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == Qt::LeftButton )
   {
      pressPos = event->pos();
   }
}

void NameTableVisualizerDockWidget::mouseMoveEvent(QMouseEvent *event)
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

void NameTableVisualizerDockWidget::wheelEvent(QWheelEvent *event)
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

void NameTableVisualizerDockWidget::renderData()
{
   renderer->updateGL ();
}

void NameTableVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void NameTableVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void NameTableVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
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
   renderer->update();
}

void NameTableVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->update();
}

void NameTableVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   CPPUDBG::SetPPUViewerShowVisible ( checked );
}
