#include "emulatorcontrol.h"
#include "ui_emulatorcontrol.h"

#include "main.h"

EmulatorControl::EmulatorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmulatorControl)
{
    ui->setupUi(this);

    // Disable buttons to start.
    ui->playButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->stepCPUButton->setEnabled(false);
    ui->stepPPUButton->setEnabled(false);
    ui->resetButton->setEnabled(false);

    QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(internalPause()));
    QObject::connect(emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(internalPause()));
    QObject::connect(emulator, SIGNAL(emulatorStarted()), this, SLOT(internalPlay()));

    // Buttons are disabled until a cartridge is loaded...then they go to the "pause-just-happened" state.
    QObject::connect(emulator, SIGNAL(cartridgeLoaded()), this, SLOT(internalPause()));
}

EmulatorControl::~EmulatorControl()
{
    delete ui;
}

void EmulatorControl::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);
   ui->resetButton->setEnabled(true);
}

void EmulatorControl::internalPause()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
   ui->resetButton->setEnabled(true);
}

void EmulatorControl::on_playButton_clicked()
{
   emulator->startEmulation();
}

void EmulatorControl::on_pauseButton_clicked()
{
   emulator->pauseEmulation(true);
}

void EmulatorControl::on_stepCPUButton_clicked()
{
   emulator->stepCPUEmulation();
}

void EmulatorControl::on_stepPPUButton_clicked()
{
   emulator->stepPPUEmulation();
}

void EmulatorControl::on_resetButton_clicked()
{
   emulator->resetEmulator();
}
