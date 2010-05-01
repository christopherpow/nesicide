#include "nesemulatordialog.h"
#include "ui_nesemulatordialog.h"
#include "main.h"

#include "cnes.h"
#include "cnesppu.h"

NESEmulatorDialog::NESEmulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESEmulatorDialog)
{
   imgData = new char[256*256*3];
   memset ( imgData, 0, sizeof(char)*256*256*3 );

   ui->setupUi(this);
   ui->pauseButton->setEnabled(false);

   renderer = new CNESEmulatorRenderer(ui->frame, imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   QObject::connect(this, SIGNAL(startEmulation()), emulator, SLOT(startEmulation()));
   QObject::connect(this, SIGNAL(pauseEmulation(bool)), emulator, SLOT(pauseEmulation(bool)));
   QObject::connect(this, SIGNAL(stepCPUEmulation()), emulator, SLOT(stepCPUEmulation()));
   QObject::connect(this, SIGNAL(stepPPUEmulation()), emulator, SLOT(stepPPUEmulation()));
   QObject::connect(this, SIGNAL(resetEmulator()), emulator, SLOT(resetEmulator()));

   QObject::connect(emulator, SIGNAL(emulatedFrame()), this, SLOT(renderData()));
   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(on_pauseButton_clicked()));
   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()));

   m_joy [ JOY1 ] = 0x00;
   m_joy [ JOY2 ] = 0x00;
   CPPU::TV ( imgData );
}

NESEmulatorDialog::~NESEmulatorDialog()
{
    delete ui;
}

void NESEmulatorDialog::stopEmulation()
{
    emit pauseEmulation(false);
    ui->playButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stepCPUButton->setEnabled(false);
    ui->stepPPUButton->setEnabled(false);
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

void NESEmulatorDialog::keyPressEvent(QKeyEvent *event)
{
// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY1 );
   if ( (char)event->key() == 'A' )
   {
      m_joy [ JOY1 ] |= JOY_LEFT;
   }
   else if ( (char)event->key() == 'D' )
   {
      m_joy [ JOY1 ] |= JOY_RIGHT;
   }
   else if ( (char)event->key() == 'W' )
   {
      m_joy [ JOY1 ] |= JOY_UP;
   }
   else if ( (char)event->key() == 'S' )
   {
      m_joy [ JOY1 ] |= JOY_DOWN;
   }
   else if ( (char)event->key() == '[' )
   {
      m_joy [ JOY1 ] |= JOY_SELECT;
   }
   else if ( (char)event->key() == ']' )
   {
      m_joy [ JOY1 ] |= JOY_START;
   }
   else if ( (char)event->key() == '.' )
   {
      m_joy [ JOY1 ] |= JOY_B;
   }
   else if ( (char)event->key() == '/' )
   {
      m_joy [ JOY1 ] |= JOY_A;
   }

// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY2 );
// CPTODO: only need one controller for now...(don't feel like picking keys)
#if 0
   if ( lChar == pControllerConfig[idxJOY_LEFT] )
   {
      m_joy [ JOY2 ] |= JOY_LEFT;
   }
   else if ( lChar == pControllerConfig[idxJOY_RIGHT] )
   {
      m_joy [ JOY2 ] |= JOY_RIGHT;
   }
   else if ( lChar == pControllerConfig[idxJOY_UP] )
   {
      m_joy [ JOY2 ] |= JOY_UP;
   }
   else if ( lChar == pControllerConfig[idxJOY_DOWN] )
   {
      m_joy [ JOY2 ] |= JOY_DOWN;
   }
   else if ( lChar == pControllerConfig[idxJOY_SELECT] )
   {
      m_joy [ JOY2 ] |= JOY_SELECT;
   }
   else if ( lChar == pControllerConfig[idxJOY_START] )
   {
      m_joy [ JOY2 ] |= JOY_START;
   }
   else if ( lChar == pControllerConfig[idxJOY_B] )
   {
      m_joy [ JOY2 ] |= JOY_B;
   }
   else if ( lChar == pControllerConfig[idxJOY_A] )
   {
      m_joy [ JOY2 ] |= JOY_A;
   }
#endif

   emit controllerInput ( m_joy );
}

void NESEmulatorDialog::keyReleaseEvent(QKeyEvent *event)
{
// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY1 );
   if ( (char)event->key() == 'A' )
   {
      m_joy [ JOY1 ] &= (~(JOY_LEFT));
   }
   else if ( (char)event->key() == 'D' )
   {
      m_joy [ JOY1 ] &= (~(JOY_RIGHT));
   }
   else if ( (char)event->key() == 'W' )
   {
      m_joy [ JOY1 ] &= (~(JOY_UP));
   }
   else if ( (char)event->key() == 'S' )
   {
      m_joy [ JOY1 ] &= (~(JOY_DOWN));
   }
   else if ( (char)event->key() == '[' )
   {
      m_joy [ JOY1 ] &= (~(JOY_SELECT));
   }
   else if ( (char)event->key() == ']' )
   {
      m_joy [ JOY1 ] &= (~(JOY_START));
   }
   else if ( (char)event->key() == '.' )
   {
      m_joy [ JOY1 ] &= (~(JOY_B));
   }
   else if ( (char)event->key() == '/' )
   {
      m_joy [ JOY1 ] &= (~(JOY_A));
   }

// CPTODO: get controller configuration...
//   pControllerConfig = CONFIG.GetControllerConfig ( JOY2 );
// CPTODO: only need one controller for now...(don't feel like picking keys)
#if 0
   if ( lChar == pControllerConfig[idxJOY_LEFT] )
   {
      m_joy [ JOY2 ] &= (~(JOY_LEFT));
   }
   else if ( lChar == pControllerConfig[idxJOY_RIGHT] )
   {
      m_joy [ JOY2 ] &= (~(JOY_RIGHT));
   }
   else if ( lChar == pControllerConfig[idxJOY_UP] )
   {
      m_joy [ JOY2 ] &= (~(JOY_UP));
   }
   else if ( lChar == pControllerConfig[idxJOY_DOWN] )
   {
      m_joy [ JOY2 ] &= (~(JOY_DOWN));
   }
   else if ( lChar == pControllerConfig[idxJOY_SELECT] )
   {
      m_joy [ JOY2 ] &= (~(JOY_SELECT));
   }
   else if ( lChar == pControllerConfig[idxJOY_START] )
   {
      m_joy [ JOY2 ] &= (~(JOY_START));
   }
   else if ( lChar == pControllerConfig[idxJOY_B] )
   {
      m_joy [ JOY2 ] &= (~(JOY_B));
   }
   else if ( lChar == pControllerConfig[idxJOY_A] )
   {
      m_joy [ JOY2 ] &= (~(JOY_A));
   }
#endif

   emit controllerInput ( m_joy );
}

void NESEmulatorDialog::on_playButton_clicked()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);

   emit startEmulation();
}

void NESEmulatorDialog::on_pauseButton_clicked()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);

   emit pauseEmulation(true);
}

void NESEmulatorDialog::on_resetButton_clicked()
{
   emit resetEmulator();
}

void NESEmulatorDialog::renderData()
{
   renderer->updateGL();
}

void NESEmulatorDialog::on_stepCPUButton_clicked()
{
   emit stepCPUEmulation();
}

void NESEmulatorDialog::on_stepPPUButton_clicked()
{
   emit stepPPUEmulation();
}
