#include "oamvisualizerdockwidget.h"
#include "ui_oamvisualizerdockwidget.h"

#include "cnessystempalette.h"
#include "dbg_cnesppu.h"

#include "nes_emulator_core.h"

#include "cobjectregistry.h"
#include "main.h"

OAMVisualizerDockWidget::OAMVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::OAMVisualizerDockWidget)
{
   ui->setupUi(this);

   RENDEROAM();

   renderer = new PanZoomRenderer(256,32,2000,OAMTV(),false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->updateScanline->setText ( "0" );
   ui->showVisible->setChecked ( false );

   pThread = new DebuggerUpdateThread(&RENDEROAM);
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

OAMVisualizerDockWidget::~OAMVisualizerDockWidget()
{
   delete pThread;
   delete ui;
   delete renderer;
}

void OAMVisualizerDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
}

void OAMVisualizerDockWidget::changeEvent(QEvent* e)
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

void OAMVisualizerDockWidget::showEvent(QShowEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void OAMVisualizerDockWidget::hideEvent(QHideEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void OAMVisualizerDockWidget::renderData()
{
    uint8_t x, y;
    nesGetLastSprite0Hit(&x,&y);
    QString str;

    str.sprintf("%d",x);
    ui->sprite0HitX->setText(str);
    str.sprintf("%d",y);
    ui->sprite0HitY->setText(str);
    renderer->reloadData(OAMTV());
}

void OAMVisualizerDockWidget::on_updateScanline_editingFinished()
{
   SetOAMViewerScanline ( ui->updateScanline->text().toInt() );
   renderer->update();
}

void OAMVisualizerDockWidget::on_showVisible_toggled(bool checked)
{
   SetOAMViewerShowVisible ( checked );
   renderer->update();
}
