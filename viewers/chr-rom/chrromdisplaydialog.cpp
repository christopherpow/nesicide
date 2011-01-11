#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"
#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "main.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(QWidget* parent, bool usePPU, qint8* data) :
   QDialog(parent),
   ui(new Ui::CHRROMDisplayDialog)
{
   ui->setupUi(this);
   imgData = new char[256*256*4];
   memset(imgData,0xFF,256*256*4);
   m_usePPU = usePPU;

   for (int i=0; i<NUM_PALETTES; i++)
   {
      ui->col0PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col1PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col2PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col3PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
   }

   ui->col0PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
   ui->col1PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->col2PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->col3PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->col0PushButton->setText("");
   ui->col1PushButton->setText("");
   ui->col2PushButton->setText("");
   ui->col3PushButton->setText("");

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
//      QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()) );
      QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()));
      QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
      QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );
   }
   else
   {
      // show CHR-ROM bank data...
      chrrom = data;
      renderData();
   }

   renderer = new CCHRROMPreviewRenderer(ui->frame, imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->updateScanline->setText ( "0" );
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
   
   renderData();
   renderer->setBGColor(ui->col0PushButton->currentColor());
   renderer->reloadData(imgData);
}

void CHRROMDisplayDialog::showEvent(QShowEvent* event)
{
   QDialog::showEvent(event);
   CPPUDBG::EnableCHRMEMInspector(true);
   renderData();
}

void CHRROMDisplayDialog::hideEvent(QHideEvent* event)
{
   QDialog::hideEvent(event);
   CPPUDBG::EnableCHRMEMInspector(false);
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
      if ( this->isVisible() )
      {
         CPPUDBG::RENDERCHRMEM();
         renderer->updateGL ();
      }
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

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
   delete imgData;
   delete ui;
}

void CHRROMDisplayDialog::resizeEvent(QResizeEvent* event)
{
   QDialog::resizeEvent(event);
   updateScrollbars();
}


void CHRROMDisplayDialog::changeEvent(QEvent* e)
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

void CHRROMDisplayDialog::on_zoomSlider_sliderMoved(int position)
{
}

void CHRROMDisplayDialog::on_zoomSlider_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void CHRROMDisplayDialog::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)128 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void CHRROMDisplayDialog::on_verticalScrollBar_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_horizontalScrollBar_actionTriggered(int action)
{
}

void CHRROMDisplayDialog::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void CHRROMDisplayDialog::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void CHRROMDisplayDialog::on_updateScanline_editingFinished()
{
   CPPUDBG::SetPPUViewerScanline ( ui->updateScanline->text().toInt() );
}

void CHRROMDisplayDialog::on_exportPushButton_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(this, QString("Save CHR-ROM Bank"), QString(""),
                      QString("Binary File (*.bin)"));

   if (!fileName.isEmpty())
   {
      QFile file(fileName);
      file.open(QIODevice::WriteOnly);
      QDataStream* ds = new QDataStream(&file);
      ds->writeRawData((char*)chrrom, 0x2000);
      file.close();
      delete ds;

   }
}
