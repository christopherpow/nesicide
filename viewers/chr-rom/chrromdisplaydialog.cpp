#include <QFileDialog>

#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"
#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(bool usePPU,qint8* data,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::CHRROMDisplayDialog)
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

   m_usePPU = usePPU;

   ui->col0PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
   ui->col1PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->col2PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->col3PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   connect(ui->col0PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col1PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col2PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col3PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));

   if ( m_usePPU )
   {
      CPPUDBG::CHRMEMInspectorTV ( (int8_t*)imgData );
      CPPUDBG::SetCHRMEMInspectorColor(0,ui->col0PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(1,ui->col1PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(2,ui->col2PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(3,ui->col3PushButton->currentColor());

      pThread = new DebuggerUpdateThread(&CPPUDBG::RENDERCHRMEM);

      QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
      QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
   }
   else
   {
      // show CHR-ROM bank data...
      memcpy(chrrom,data,MEM_8KB);
      renderData();
   }

   renderer = new CCHRROMPreviewRenderer(ui->frame, imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->updateScanline->setText ( "0" );
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
   delete imgData;
   delete ui;
   if ( m_usePPU )
   {
      delete pThread;
   }
}

void CHRROMDisplayDialog::showEvent(QShowEvent* event)
{
   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void CHRROMDisplayDialog::hideEvent(QHideEvent* event)
{
   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void CHRROMDisplayDialog::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);
   updateScrollbars();
}

void CHRROMDisplayDialog::mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == Qt::LeftButton )
   {
      pressPos = event->pos();
   }
}

void CHRROMDisplayDialog::mouseMoveEvent(QMouseEvent *event)
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

void CHRROMDisplayDialog::wheelEvent(QWheelEvent *event)
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

void CHRROMDisplayDialog::changeEvent(QEvent* event)
{
   QWidget::changeEvent(event);

   switch (event->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void CHRROMDisplayDialog::contextMenuEvent(QContextMenuEvent *event)
{
}

void CHRROMDisplayDialog::colorChanged (const QColor& color)
{
   ui->col0PushButton->setText("");
   ui->col1PushButton->setText("");
   ui->col2PushButton->setText("");
   ui->col3PushButton->setText("");

   if ( m_usePPU )
   {
      CPPUDBG::SetCHRMEMInspectorColor(0,ui->col0PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(1,ui->col1PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(2,ui->col2PushButton->currentColor());
      CPPUDBG::SetCHRMEMInspectorColor(3,ui->col3PushButton->currentColor());
   }

   pThread->updateDebuggers();
   renderer->setBGColor(ui->col0PushButton->currentColor());
   renderer->reloadData(imgData);
}

void CHRROMDisplayDialog::renderData()
{
   unsigned int ppuAddr = 0x0000;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char bit1, bit2;
   unsigned char colorIdx;
   QColor color[4];

   if ( m_usePPU )
   {
      renderer->updateGL ();
   }
   else
   {
      color[0] = ui->col0PushButton->currentColor();
      color[1] = ui->col1PushButton->currentColor();
      color[2] = ui->col2PushButton->currentColor();
      color[3] = ui->col3PushButton->currentColor();

      for (int y = 0; y < 128; y++)
      {
         for (int x = 0; x < 256; x += 8)
         {
            ppuAddr = ((y>>3)<<8)+((x%128)<<1)+(y&0x7);

            if ( x >= 128 )
            {
               ppuAddr += 0x1000;
            }

            patternData1 = *(chrrom+ppuAddr);
            patternData2 = *(chrrom+ppuAddr+8);

            for ( int xf = 0; xf < 8; xf++ )
            {
               bit1 = (patternData1>>(7-(xf)))&0x1;
               bit2 = (patternData2>>(7-(xf)))&0x1;
               colorIdx = (bit1|(bit2<<1));
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = color[colorIdx].blue();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = color[colorIdx].green();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = color[colorIdx].red();
            }
         }
      }
   }
}

void CHRROMDisplayDialog::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void CHRROMDisplayDialog::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)128 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void CHRROMDisplayDialog::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->update();
}

void CHRROMDisplayDialog::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->update();
}

void CHRROMDisplayDialog::on_updateScanline_editingFinished()
{
   CPPUDBG::SetPPUViewerScanline ( ui->updateScanline->text().toInt() );
}

void CHRROMDisplayDialog::on_exportPushButton_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(this, "Save CHR-ROM Bank", QDir::currentPath(),
                                                   "Binary File (*.bin)");

   if (!fileName.isEmpty())
   {
      QFile file(fileName);
      file.open(QIODevice::WriteOnly);
      QDataStream* ds = new QDataStream(&file);
      ds->writeRawData((char*)chrrom, MEM_8KB);
      file.close();
      delete ds;

   }
}

void CHRROMDisplayDialog::applyProjectPropertiesToTab()
{
}
