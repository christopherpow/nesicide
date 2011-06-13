#include "codedataloggerdockwidget.h"
#include "ui_codedataloggerdockwidget.h"

#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"

#include "main.h"

CodeDataLoggerDockWidget::CodeDataLoggerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeDataLoggerDockWidget)
{
   int i;

   ui->setupUi(this);
   cpuImgData = new char[256*256*4];
   ppuImgData = new char[256*256*4];

   // Clear images...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      cpuImgData[i] = 0;
      cpuImgData[i+1] = 0;
      cpuImgData[i+2] = 0;
      cpuImgData[i+3] = 0xFF;
      ppuImgData[i] = 0;
      ppuImgData[i+1] = 0;
      ppuImgData[i+2] = 0;
      ppuImgData[i+3] = 0xFF;
   }
   C6502DBG::CodeDataLoggerInspectorTV ( (int8_t*)cpuImgData );
   CPPUDBG::CodeDataLoggerInspectorTV ( (int8_t*)ppuImgData );

   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CCodeDataLoggerRenderer(ui->frame,cpuImgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
}

CodeDataLoggerDockWidget::~CodeDataLoggerDockWidget()
{
   delete ui;
   delete cpuImgData;
   delete ppuImgData;
}

void CodeDataLoggerDockWidget::changeEvent(QEvent* e)
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

void CodeDataLoggerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   renderData();
}

void CodeDataLoggerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
}

void CodeDataLoggerDockWidget::renderData()
{
   switch ( ui->displaySelect->currentIndex() )
   {
      case 0:
         C6502DBG::RENDERCODEDATALOGGER();
         break;
      case 1:
         CPPUDBG::RENDERCODEDATALOGGER();
         break;
   }

   renderer->updateGL ();
}

void CodeDataLoggerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void CodeDataLoggerDockWidget::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void CodeDataLoggerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)256 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void CodeDataLoggerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDockWidget::on_displaySelect_currentIndexChanged(int index)
{
   switch ( index )
   {
      case 0:
         renderer->changeImage(cpuImgData);
         break;
      case 1:
         renderer->changeImage(ppuImgData);
         break;
   }

   renderer->repaint();
}
