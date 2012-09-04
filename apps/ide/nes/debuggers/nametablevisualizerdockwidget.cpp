#include "nametablevisualizerdockwidget.h"
#include "ui_nametablevisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "cobjectregistry.h"
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

   renderer = new PanZoomRenderer(512,480,2000,imgData,false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->showVisible->setChecked ( true );

   pThread = new DebuggerUpdateThread(&CPPUDBG::RENDERNAMETABLE);
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

NameTableVisualizerDockWidget::~NameTableVisualizerDockWidget()
{
   delete ui;
   delete imgData;
   delete renderer;
   delete pThread;
}

void NameTableVisualizerDockWidget::updateTargetMachine(QString target)
{
   QObject* breakpointWatcher = CObjectRegistry::getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
}

void NameTableVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void NameTableVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

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

void NameTableVisualizerDockWidget::renderData()
{
   renderer->reloadData(imgData);
}

void NameTableVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   CPPUDBG::SetPPUViewerShowVisible ( checked );
}
