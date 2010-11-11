#include "nesemulatordialog.h"
#include "ui_nesemulatordialog.h"
#include "main.h"

#include "emulator_core.h"

NESEmulatorDialog::NESEmulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESEmulatorDialog)
{
   imgData = new char[256*256*4];
   memset ( imgData, 0, sizeof(char)*256*256*4 );

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
   
   // Clear image to set alpha channel...
   memset ( imgData, 0xFF, 256*256*4 );
   nesSetTVOut((int8_t*)imgData);
}

NESEmulatorDialog::~NESEmulatorDialog()
{
    delete ui;
}

void NESEmulatorDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NESEmulatorDialog::mousePressEvent(QMouseEvent *event)
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

void NESEmulatorDialog::mouseReleaseEvent(QMouseEvent *event)
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

void NESEmulatorDialog::mouseMoveEvent(QMouseEvent *event)
{
   // CPTODO: defaulted controller 2 to zapper for now
   nesSetControllerScreenPosition(CONTROLLER2,event->pos().x()-ui->frame->pos().x(),event->pos().y()-ui->frame->pos().y());
   event->accept();
}

void NESEmulatorDialog::keyPressEvent(QKeyEvent *event)
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
   else if ( (char)event->key() == '[' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_SELECT;
   }
   else if ( (char)event->key() == ']' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_START;
   }
   else if ( (char)event->key() == '.' )
   {
      m_joy [ CONTROLLER1 ] |= JOY_B;
   }
   else if ( (char)event->key() == '/' )
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

void NESEmulatorDialog::keyReleaseEvent(QKeyEvent *event)
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
   else if ( (char)event->key() == '[' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_SELECT));
   }
   else if ( (char)event->key() == ']' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_START));
   }
   else if ( (char)event->key() == '.' )
   {
      m_joy [ CONTROLLER1 ] &= (~(JOY_B));
   }
   else if ( (char)event->key() == '/' )
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

void NESEmulatorDialog::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);
}

void NESEmulatorDialog::on_playButton_clicked()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);

   emulator->startEmulation();
}

void NESEmulatorDialog::internalPause(bool)
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
}

void NESEmulatorDialog::internalPauseWithoutShow()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
}

void NESEmulatorDialog::on_pauseButton_clicked()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);

   emulator->pauseEmulation(true);
}

void NESEmulatorDialog::on_stepCPUButton_clicked()
{
   emulator->stepCPUEmulation();
}

void NESEmulatorDialog::on_stepPPUButton_clicked()
{
   emulator->stepPPUEmulation();
}

void NESEmulatorDialog::on_resetButton_clicked()
{
   emulator->resetEmulator();
}

void NESEmulatorDialog::renderData()
{
   renderer->updateGL();
}
