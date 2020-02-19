#include "emulatorcontrol.h"
#include "ui_emulatorcontrol.h"

#include "cobjectregistry.h"

EmulatorControl::EmulatorControl(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::EmulatorControl)
{
   ui->setupUi(this);

   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect(emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorStarted()), this, SLOT(internalPlay()));

   // Buttons are disabled until a cartridge is loaded...then they go to the "play-just-happened" state.
   QObject::connect(emulator, SIGNAL(cartridgeLoaded()), this, SLOT(internalPlay()));

   // Connect menu actions to slots.
   QObject::connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(on_playButton_clicked()));
   QObject::connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(on_pauseButton_clicked()));
   QObject::connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(on_resetButton_clicked()));
   QObject::connect(ui->actionSoft_Reset, SIGNAL(triggered()), this, SLOT(on_softButton_clicked()));

   // Connect control signals to emulator.
   QObject::connect(this,SIGNAL(startEmulation()),emulator,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),emulator,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(resetEmulator()),emulator,SLOT(resetEmulator()));
   QObject::connect(this,SIGNAL(softResetEmulator()),emulator,SLOT(softResetEmulator()));
}

EmulatorControl::~EmulatorControl()
{
   delete ui;
}

QList<QAction*> EmulatorControl::menu()
{
   QList<QAction*> items;
   items.append(ui->actionRun);
   items.append(ui->actionPause);
   items.append(ui->actionSoft_Reset);
   items.append(ui->actionReset);
   return items;
}

void EmulatorControl::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->actionRun->setEnabled(false);
   ui->actionPause->setEnabled(true);
}

void EmulatorControl::internalPause()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->actionRun->setEnabled(true);
   ui->actionPause->setEnabled(false);
}

void EmulatorControl::on_playButton_clicked()
{
   emit startEmulation();
}

void EmulatorControl::on_pauseButton_clicked()
{
   emit pauseEmulation(true);
}

void EmulatorControl::on_resetButton_clicked()
{
   emit resetEmulator();
   emit startEmulation();
}

void EmulatorControl::on_softButton_clicked()
{
   emit softResetEmulator();
   emit startEmulation();
}
