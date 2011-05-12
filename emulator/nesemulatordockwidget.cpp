#include "nesemulatordockwidget.h"
#include "ui_nesemulatordockwidget.h"

#include "main.h"

#include "emulator_core.h"

NESEmulatorDockWidget::NESEmulatorDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::NESEmulatorDockWidget)
{
   int32_t i;

   imgData = new char[256*256*4];

   ui->setupUi(this);
   ui->pauseButton->setEnabled(false);

   renderer = new CNESEmulatorRenderer(ui->frame, imgData);
   renderer->setMouseTracking(true);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   QObject::connect(emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()));
   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(internalPauseWithoutShow()));
   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()));
   QObject::connect(emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(internalPause(bool)));
   QObject::connect(emulator, SIGNAL(emulatorStarted()), this, SLOT(internalPlay()));

   m_joy [ CONTROLLER1 ] = 0x00;
   m_joy [ CONTROLLER2 ] = 0x00;

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }
   nesSetTVOut((int8_t*)imgData);
}

NESEmulatorDockWidget::~NESEmulatorDockWidget()
{
    delete ui;
    delete renderer;
    delete imgData;
}

void NESEmulatorDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void NESEmulatorDockWidget::mousePressEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   if ( event->button()&Qt::LeftButton )
   {
      m_joy [ CONTROLLER2 ] |= JOY_TRIGGER;

      nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());

      emulator->controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::mouseReleaseEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   if ( event->button()&Qt::LeftButton )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_TRIGGER));

      nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());

      emulator->controllerInput ( m_joy );

      event->accept();
   }
}

void NESEmulatorDockWidget::mouseMoveEvent(QMouseEvent* event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());
   event->accept();
}

void NESEmulatorDockWidget::keyPressEvent(QKeyEvent* event)
{
// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( CONTROLLER1 );
   if ( (char)event->key() == 'A' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_LEFT;
   }
   else if ( (char)event->key() == 'D' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_RIGHT;
   }
   else if ( (char)event->key() == 'W' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_UP;
   }
   else if ( (char)event->key() == 'S' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_DOWN;
   }
   else if ( (char)event->key() == 'U' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_SELECT;
   }
   else if ( (char)event->key() == 'I' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_START;
   }
   else if ( (char)event->key() == 'O' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_B;
   }
   else if ( (char)event->key() == 'P' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_A;
   }

// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( CONTROLLER2 );
// CPTODO: only need one controller for now...(don't feel like picking keys)
#if 0

   if ( lChar == pControllerConfig[idxJOY_LEFT] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_LEFT;
   }
   else if ( lChar == pControllerConfig[idxJOY_RIGHT] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_RIGHT;
   }
   else if ( lChar == pControllerConfig[idxJOY_UP] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_UP;
   }
   else if ( lChar == pControllerConfig[idxJOY_DOWN] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_DOWN;
   }
   else if ( lChar == pControllerConfig[idxJOY_SELECT] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_SELECT;
   }
   else if ( lChar == pControllerConfig[idxJOY_START] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_START;
   }
   else if ( lChar == pControllerConfig[idxJOY_B] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_B;
   }
   else if ( lChar == pControllerConfig[idxJOY_A] )
   {
      m_joy [ CONTROLLER2 ] |= JOY_A;
   }

#endif

   emulator->controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::keyReleaseEvent(QKeyEvent* event)
{
// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( CONTROLLER1 );
   if ( (char)event->key() == 'A' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_LEFT));
   }
   else if ( (char)event->key() == 'D' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_RIGHT));
   }
   else if ( (char)event->key() == 'W' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_UP));
   }
   else if ( (char)event->key() == 'S' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_DOWN));
   }
   else if ( (char)event->key() == 'U' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_SELECT));
   }
   else if ( (char)event->key() == 'I' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_START));
   }
   else if ( (char)event->key() == 'O' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_B));
   }
   else if ( (char)event->key() == 'P' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_A));
   }

// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( CONTROLLER2 );
// CPTODO: only need one controller for now...(don't feel like picking keys)
#if 0

   if ( lChar == pControllerConfig[idxJOY_LEFT] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_LEFT));
   }
   else if ( lChar == pControllerConfig[idxJOY_RIGHT] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_RIGHT));
   }
   else if ( lChar == pControllerConfig[idxJOY_UP] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_UP));
   }
   else if ( lChar == pControllerConfig[idxJOY_DOWN] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_DOWN));
   }
   else if ( lChar == pControllerConfig[idxJOY_SELECT] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_SELECT));
   }
   else if ( lChar == pControllerConfig[idxJOY_START] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_START));
   }
   else if ( lChar == pControllerConfig[idxJOY_B] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_B));
   }
   else if ( lChar == pControllerConfig[idxJOY_A] )
   {
      m_joy [ CONTROLLER2 ] &= (~(JOY_A));
   }

#endif

   emulator->controllerInput ( m_joy );

   event->accept();
}

void NESEmulatorDockWidget::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);
}

void NESEmulatorDockWidget::internalPause(bool)
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
}

void NESEmulatorDockWidget::internalPauseWithoutShow()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
}

void NESEmulatorDockWidget::on_playButton_clicked()
{
#if 0
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);
#endif
   emulator->startEmulation();
}

void NESEmulatorDockWidget::on_pauseButton_clicked()
{
#if 0
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
#endif

   emulator->pauseEmulation(true);
}

void NESEmulatorDockWidget::on_stepCPUButton_clicked()
{
   emulator->stepCPUEmulation();
}

void NESEmulatorDockWidget::on_stepPPUButton_clicked()
{
   emulator->stepPPUEmulation();
}

void NESEmulatorDockWidget::on_resetButton_clicked()
{
#if 0
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
#endif

   emulator->resetEmulator();
}

void NESEmulatorDockWidget::renderData()
{
   renderer->updateGL();
}
