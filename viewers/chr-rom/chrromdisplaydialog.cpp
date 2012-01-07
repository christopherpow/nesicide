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

   renderer = new PanZoomRenderer(256,128,2000,imgData,true,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   QObject::connect(renderer,SIGNAL(repaintNeeded()),this,SLOT(repaintNeeded()));

   if ( m_usePPU )
   {
      CPPUDBG::CHRMEMInspectorTV ( (int8_t*)imgData );
      CPPUDBG::SetCHRMEMInspectorColor(0,renderer->getColor(0));
      CPPUDBG::SetCHRMEMInspectorColor(1,renderer->getColor(1));
      CPPUDBG::SetCHRMEMInspectorColor(2,renderer->getColor(2));
      CPPUDBG::SetCHRMEMInspectorColor(3,renderer->getColor(3));

      pThread = new DebuggerUpdateThread(&CPPUDBG::RENDERCHRMEM);

      QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
      QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
   }
   else
   {
      // No thread necessary.
      pThread = NULL;

      // show CHR-ROM bank data...
      memcpy(chrrom,data,MEM_8KB);
      renderData();
   }

   ui->updateScanline->setText ( "0" );
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
   delete ui;
   delete imgData;
   delete renderer;
   if ( m_usePPU )
   {
      delete pThread;
   }
}

void CHRROMDisplayDialog::showEvent(QShowEvent* event)
{
   if ( m_usePPU )
   {
      QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

      pThread->updateDebuggers();
   }
}

void CHRROMDisplayDialog::hideEvent(QHideEvent* event)
{
   if ( m_usePPU )
   {
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
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

void CHRROMDisplayDialog::repaintNeeded()
{
   if ( m_usePPU )
   {
      CPPUDBG::SetCHRMEMInspectorColor(0,renderer->getColor(0));
      CPPUDBG::SetCHRMEMInspectorColor(1,renderer->getColor(1));
      CPPUDBG::SetCHRMEMInspectorColor(2,renderer->getColor(2));
      CPPUDBG::SetCHRMEMInspectorColor(3,renderer->getColor(3));

      pThread->updateDebuggers();
   }
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
      renderer->reloadData(imgData);
   }
   else
   {
      color[0] = renderer->getColor(0);
      color[1] = renderer->getColor(1);
      color[2] = renderer->getColor(2);
      color[3] = renderer->getColor(3);

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
