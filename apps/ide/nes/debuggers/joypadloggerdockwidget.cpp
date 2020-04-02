#include "joypadloggerdockwidget.h"
#include "ui_joypadloggerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "nes_emulator_core.h"

#include "cobjectregistry.h"

JoypadLoggerDockWidget::JoypadLoggerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::JoypadLoggerDockWidget)
{
//   int i;

   ui->setupUi(this);
//   imgData = new char[256*256*4];

//   // Clear image...
//   for ( i = 0; i < 256*256*4; i+=4 )
//   {
//      imgData[i] = 0;
//      imgData[i+1] = 0;
//      imgData[i+2] = 0;
//      imgData[i+3] = 0xFF;
//   }
//   CPPUDBG::OAMInspectorTV ( (int8_t*)imgData );

//   renderer = new PanZoomRenderer(256,32,2000,imgData,false,ui->frame);
//   ui->frame->layout()->addWidget(renderer);
//   ui->frame->layout()->update();

//   ui->updateScanline->setText ( "0" );
//   ui->showVisible->setChecked ( false );

//   pThread = new DebuggerUpdateThread(&CPPUDBG::RENDEROAM);
//   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

JoypadLoggerDockWidget::~JoypadLoggerDockWidget()
{
//   delete pThread;
//   delete ui;
//   delete imgData;
//   delete renderer;
}

void JoypadLoggerDockWidget::updateTargetMachine(QString target)
{
   if ( !target.compare("nes") )
   {
//   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
//   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

//   QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
//   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
//   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
//   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   }
}

void JoypadLoggerDockWidget::changeEvent(QEvent* e)
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

void JoypadLoggerDockWidget::showEvent(QShowEvent* /*event*/)
{
//   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

//   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

//   pThread->updateDebuggers();
}

void JoypadLoggerDockWidget::hideEvent(QHideEvent* /*event*/)
{
//   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

//   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void JoypadLoggerDockWidget::renderData()
{
//    uint8_t x, y;
//    nesGetLastSprite0Hit(&x,&y);
//    QString str;

//    str.sprintf("%d",x);
//    ui->sprite0HitX->setText(str);
//    str.sprintf("%d",y);
//    ui->sprite0HitY->setText(str);
//   renderer->reloadData(imgData);
}
