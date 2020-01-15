#include <QFileDialog>

#include "chrromdisplaydialog.h"
#include "ui_chrromdisplaydialog.h"
#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "cobjectregistry.h"
#include "main.h"

CHRROMDisplayDialog::CHRROMDisplayDialog(bool usePPU,qint8* data,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::CHRROMDisplayDialog)
{
   ui->setupUi(this);

   info = new QLabel(this);

   m_usePPU = usePPU;

   RENDERCHRMEM();

   imgData = new int8_t[256*128*4];

   if ( m_usePPU )
   {
      renderer = new PanZoomRenderer(256,128,2000,CHRMEMTV(),true,ui->frame);

      SetCHRMEMInspectorColor(0,renderer->getColor(0));
      SetCHRMEMInspectorColor(1,renderer->getColor(1));
      SetCHRMEMInspectorColor(2,renderer->getColor(2));
      SetCHRMEMInspectorColor(3,renderer->getColor(3));

      pThread = new DebuggerUpdateThread(&RENDERCHRMEM);
      QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
   }
   else
   {
      renderer = new PanZoomRenderer(256,128,2000,imgData,true,ui->frame);

      // No thread necessary.
      pThread = NULL;

      // show CHR-ROM bank data...
      memcpy(chrrom,data,MEM_8KB);
      renderData();
   }

   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
   setCentralWidget(ui->window);

   renderer->installEventFilter(this);

   QObject::connect(renderer,SIGNAL(repaintNeeded()),this,SLOT(repaintNeeded()));

   ui->updateScanline->setText ( "0" );
}

CHRROMDisplayDialog::~CHRROMDisplayDialog()
{
   if ( info->parent() == this )
   {
      delete info;
   }
   if ( m_usePPU )
   {
      delete pThread;
   }
   else
   {
      delete imgData;
   }
   delete ui;
   delete renderer;
}

void CHRROMDisplayDialog::updateTargetMachine(QString /*target*/)
{
   if ( m_usePPU )
   {
      QObject* breakpointWatcher = CObjectRegistry::getObject("Breakpoint Watcher");
      QObject* emulator = CObjectRegistry::getObject("Emulator");

      QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   }
}

void CHRROMDisplayDialog::showEvent(QShowEvent* /*event*/)
{
   if ( m_usePPU )
   {
      QObject* emulator = CObjectRegistry::getObject("Emulator");

      QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

      pThread->updateDebuggers();
   }
   emit addStatusBarWidget(info);
}

void CHRROMDisplayDialog::hideEvent(QHideEvent* /*event*/)
{
   if ( m_usePPU )
   {
      QObject* emulator = CObjectRegistry::getObject("Emulator");

      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
   }
   emit removeStatusBarWidget(info);
}

bool CHRROMDisplayDialog::eventFilter(QObject* obj,QEvent* event)
{
   if ( obj == renderer )
   {
      if ( event->type() == QEvent::MouseMove )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseMoveEvent(mouseEvent);
      }
      else if ( event->type() == QEvent::Enter )
      {
         QEvent* enterEvent = dynamic_cast<QEvent*>(event);
         renderer_enterEvent(enterEvent);
      }
      else if ( event->type() == QEvent::Leave )
      {
         QEvent* leaveEvent = dynamic_cast<QEvent*>(event);
         renderer_leaveEvent(leaveEvent);
      }
   }
   return false;
}

void CHRROMDisplayDialog::renderer_enterEvent(QEvent */*event*/)
{
   int pixx;
   int pixy;
   bool visible;

   visible = renderer->pointToPixel(QCursor::pos().x(),QCursor::pos().y(),&pixx,&pixy);

   if ( visible )
   {
      updateInfoText(pixx,pixy);
   }
}

void CHRROMDisplayDialog::renderer_leaveEvent(QEvent */*event*/)
{
   updateInfoText();
}

void CHRROMDisplayDialog::renderer_mouseMoveEvent(QMouseEvent */*event*/)
{
   int pixx;
   int pixy;
   bool visible;

   visible = renderer->pointToPixel(QCursor::pos().x(),QCursor::pos().y(),&pixx,&pixy);

   if ( visible )
   {
      updateInfoText(pixx,pixy);
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

void CHRROMDisplayDialog::updateInfoText(int x, int y)
{
   int tileX;
   int tileY;
   int side;
   const char* sideStr[] = { "LEFT", "RIGHT" };

   if ( (x >= 0) && (y >= 0) )
   {
      QString str;

      tileX = PIXEL_TO_TILE(x);
      tileY = PIXEL_TO_TILE(y);

      if ( tileX >= 16 )
      {
         side = 1;
         tileX -= 16;
      }
      else
      {
         side = 0;
      }

      str.sprintf("Cursor:Pixel(%d,%d) Tile(%d,%d) %s",
                  x,y,
                  tileX,tileY,
                  sideStr[side]);

      info->setText(str);
   }
   else
   {
      info->clear();
   }
}

void CHRROMDisplayDialog::repaintNeeded()
{
   if ( m_usePPU )
   {
      SetCHRMEMInspectorColor(0,renderer->getColor(0));
      SetCHRMEMInspectorColor(1,renderer->getColor(1));
      SetCHRMEMInspectorColor(2,renderer->getColor(2));
      SetCHRMEMInspectorColor(3,renderer->getColor(3));

      pThread->updateDebuggers();
   }
   else
   {
      renderData();
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
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = color[colorIdx].red();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = color[colorIdx].green();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = color[colorIdx].blue();
            }
         }
      }
      renderer->reloadData(imgData);
   }
}

void CHRROMDisplayDialog::on_updateScanline_editingFinished()
{
   SetPPUViewerScanline ( ui->updateScanline->text().toInt() );
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
