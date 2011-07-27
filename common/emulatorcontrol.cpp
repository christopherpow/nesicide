#include "emulatorcontrol.h"
#include "ui_emulatorcontrol.h"

#include "main.h"

EmulatorControl::EmulatorControl(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::EmulatorControl)
{
   ui->setupUi(this);

   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorStarted()), this, SLOT(internalPlay()));

   // Buttons are disabled until a cartridge is loaded...then they go to the "pause-just-happened" state.
   QObject::connect(emulator, SIGNAL(cartridgeLoaded()), this, SLOT(internalPause()));

   // Connect menu actions to slots.
   QObject::connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(on_playButton_clicked()));
   QObject::connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(on_pauseButton_clicked()));
   QObject::connect(ui->actionStep_CPU, SIGNAL(triggered()), this, SLOT(on_stepCPUButton_clicked()));
   QObject::connect(ui->actionStep_PPU, SIGNAL(triggered()), this, SLOT(on_stepPPUButton_clicked()));
   QObject::connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(on_resetButton_clicked()));
   QObject::connect(ui->actionFrame_Advance, SIGNAL(triggered()), this, SLOT(on_frameAdvance_clicked()));
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
   items.append(ui->actionStep_CPU);
   items.append(ui->actionStep_PPU);
   items.append(ui->actionReset);
   items.append(ui->actionFrame_Advance);
   return items;
}

void EmulatorControl::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepPPUButton->setEnabled(false);
   ui->frameAdvance->setEnabled(false);
   ui->actionRun->setEnabled(false);
   ui->actionPause->setEnabled(true);
   ui->actionStep_CPU->setEnabled(false);
   ui->actionStep_PPU->setEnabled(false);
   ui->actionFrame_Advance->setEnabled(false);
}

void EmulatorControl::internalPause()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(true);
   ui->stepPPUButton->setEnabled(true);
   ui->frameAdvance->setEnabled(true);
   ui->actionRun->setEnabled(true);
   ui->actionPause->setEnabled(false);
   ui->actionStep_CPU->setEnabled(true);
   ui->actionStep_PPU->setEnabled(true);
   ui->actionFrame_Advance->setEnabled(true);
}

void EmulatorControl::on_playButton_clicked()
{
   emulator->startEmulation();

   emit focusEmulator();
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

void EmulatorControl::on_frameAdvance_clicked()
{
   emulator->advanceFrame();
}

void EmulatorControl::on_stepCPUOverButton_clicked()
{
   emulator->stepOverCPUEmulation();
}
