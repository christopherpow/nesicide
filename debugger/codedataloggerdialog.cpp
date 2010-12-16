#include "codedataloggerdialog.h"
#include "ui_codedataloggerdialog.h"

#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"

#include "main.h"

CodeDataLoggerDialog::CodeDataLoggerDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::CodeDataLoggerDialog)
{
   ui->setupUi(this);
   cpuImgData = new char[256*256*4];
   ppuImgData = new char[256*256*4];

   memset(cpuImgData,0xFF,256*256*4);
   memset(ppuImgData,0xFF,256*256*4);
   C6502DBG::CodeDataLoggerInspectorTV ( (int8_t*)cpuImgData );
   CPPUDBG::CodeDataLoggerInspectorTV ( (int8_t*)ppuImgData );
   QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CCodeDataLoggerRenderer(ui->frame,cpuImgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
}

void CodeDataLoggerDialog::changeEvent(QEvent* e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void CodeDataLoggerDialog::showEvent(QShowEvent* event)
{
   QDialog::showEvent(event);
   renderData();
}

void CodeDataLoggerDialog::hideEvent(QHideEvent* event)
{
   QDialog::hideEvent(event);
}

void CodeDataLoggerDialog::renderData()
{
   if ( isVisible() )
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
}

CodeDataLoggerDialog::~CodeDataLoggerDialog()
{
   delete cpuImgData;
   delete ppuImgData;
   delete ui;
}

void CodeDataLoggerDialog::resizeEvent(QResizeEvent* event)
{
   QDialog::resizeEvent(event);
   updateScrollbars();
}

void CodeDataLoggerDialog::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void CodeDataLoggerDialog::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)256 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void CodeDataLoggerDialog::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDialog::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDialog::on_displaySelect_currentIndexChanged(int index)
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
