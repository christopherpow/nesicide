#include "nametablevisualizerdockwidget.h"
#include "ui_nametablevisualizerdockwidget.h"

#include "nes_emulator_core.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "cobjectregistry.h"
#include "main.h"

NameTableVisualizerDockWidget::NameTableVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::NameTableVisualizerDockWidget)
{
   ui->setupUi(this);

   RENDERNAMETABLE();

   renderer = new PanZoomRenderer(512,480,2000,NAMETABLETV(),false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->showVisible->setChecked ( true );

   renderer->installEventFilter(this);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->address->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->address->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->address->setFont(QFont("Consolas", 11));
#endif

   pThread = new DebuggerUpdateThread(&RENDERNAMETABLE);
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

NameTableVisualizerDockWidget::~NameTableVisualizerDockWidget()
{
   delete pThread;
   delete ui;
   delete renderer;
}

void NameTableVisualizerDockWidget::updateTargetMachine(QString target)
{
   if ( !target.compare("nes") )
   {
      QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
      QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

      QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   }
}

void NameTableVisualizerDockWidget::showEvent(QShowEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void NameTableVisualizerDockWidget::hideEvent(QHideEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

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


bool NameTableVisualizerDockWidget::eventFilter(QObject* obj,QEvent* event)
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

void NameTableVisualizerDockWidget::renderer_enterEvent(QEvent */*event*/)
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

void NameTableVisualizerDockWidget::renderer_leaveEvent(QEvent */*event*/)
{
   updateInfoText();
}

void NameTableVisualizerDockWidget::renderer_mouseMoveEvent(QMouseEvent */*event*/)
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

void NameTableVisualizerDockWidget::updateInfoText(int x, int y)
{
   int addr;
   uint16_t sc[4], q = 0;
   int tileX;
   int tileY;

   if ( (x >= 0) && (y >= 0) )
   {
      QString str;

      addr = 0x2000+(x>>8);
      addr |= (y<<5);

      if ( y >= 256 ) q += 2;
      if ( x >= 256 ) q += 1;

      nesGetMirroring(&sc[0],&sc[1],&sc[2],&sc[3]);

      if ( x >= 256 )
      {
         x -= 256;
      }
      if ( y >= 240 )
      {
         y -= 240;
      }

      tileX = PIXEL_TO_TILE(x);
      tileY = PIXEL_TO_TILE(y);

      str.sprintf("Pixel(%d,%d) Tile(%d,%d) $%04X",
                  x,y,
                  tileX,tileY,
                  sc[q]+((tileY*32)+tileX));

      ui->address->setText(str);
   }
   else
   {
      ui->address->clear();
   }
}

void NameTableVisualizerDockWidget::renderData()
{
   renderer->reloadData(NAMETABLETV());
}

void NameTableVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   SetPPUViewerShowVisible ( checked );
}
