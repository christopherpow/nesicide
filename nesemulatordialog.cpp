#include "nesemulatordialog.h"
#include "ui_nesemulatordialog.h"
#include "main.h"

#include "PPU.h"

NESEmulatorDialog::NESEmulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NESEmulatorDialog)
{
   imgData = new char[256*256*3];

   ui->setupUi(this);
   ui->stopButton->setEnabled(false);
   ui->pauseButton->setEnabled(false);
   ui->stepButton->setEnabled(false);

   renderer = new CNESEmulatorRenderer(ui->frame, imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   memset ( imgData, 0, sizeof(imgData));

   QObject::connect(ui->playButton, SIGNAL(pressed()), emulator, SLOT(startEmulation()));
   QObject::connect(ui->pauseButton, SIGNAL(pressed()), emulator, SLOT(pauseEmulation()));
   QObject::connect(ui->stopButton, SIGNAL(pressed()), emulator, SLOT(stopEmulation()));
   QObject::connect(ui->resetButton, SIGNAL(pressed()), emulator, SLOT(resetEmulator()));

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
    emit on_stopButton_clicked();
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
    ui->stopButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->stepButton->setEnabled(false);
}

void NESEmulatorDialog::on_stopButton_clicked()
{
    // The following happens if the user closes the emulation window from the main UI. The main
    // UI calls a stop method to halt emulation, which method generates a signal that hits this slot.
    // In some cases the emulator is already stopped (in which the stop button will be hidden) so
    // we simply check for this condition before trying to do anything else.
    if (!ui->stopButton->isVisible())
        return;

    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->stepButton->setEnabled(false);
}

void NESEmulatorDialog::on_pauseButton_clicked()
{
    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stepButton->setEnabled(true);
}

void NESEmulatorDialog::on_resetButton_clicked()
{
}

void NESEmulatorDialog::on_stepButton_clicked()
{
    // TODO: Run one PPU cycle's worth of clocks on the PPU.
    //       Since PPU executes faster it makes sense to align the steps
    //       on PPU cycles but you can do it on CPU cycles if you really want to.
    // CPTODO: re-factor stepping routine first...
}
