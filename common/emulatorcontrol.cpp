#include "emulatorcontrol.h"
#include "ui_emulatorcontrol.h"

#include "ccc65interface.h"

#include "emulator_core.h"

#include "main.h"

EmulatorControl::EmulatorControl(QWidget *parent) :
   QWidget(parent),
   debugging(true),
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
   QObject::connect(ui->actionStep_Over, SIGNAL(triggered()), this, SLOT(on_stepOverButton_clicked()));
   QObject::connect(ui->actionStep_Out, SIGNAL(triggered()), this, SLOT(on_stepOutButton_clicked()));
   QObject::connect(ui->actionStep_PPU, SIGNAL(triggered()), this, SLOT(on_stepPPUButton_clicked()));
   QObject::connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(on_resetButton_clicked()));
   QObject::connect(ui->actionFrame_Advance, SIGNAL(triggered()), this, SLOT(on_frameAdvance_clicked()));

   // Connect control signals to emulator.
   QObject::connect(this,SIGNAL(startEmulation()),emulator,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),emulator,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(stepCPUEmulation()),emulator,SLOT(stepCPUEmulation()));
   QObject::connect(this,SIGNAL(stepOverCPUEmulation()),emulator,SLOT(stepOverCPUEmulation()));
   QObject::connect(this,SIGNAL(stepOutCPUEmulation()),emulator,SLOT(stepOutCPUEmulation()));
   QObject::connect(this,SIGNAL(stepPPUEmulation()),emulator,SLOT(stepPPUEmulation()));
   QObject::connect(this,SIGNAL(advanceFrame()),emulator,SLOT(advanceFrame()));
   QObject::connect(this,SIGNAL(resetEmulator()),emulator,SLOT(resetEmulator()));

   ui->debugButton->setChecked(debugging);
}

EmulatorControl::~EmulatorControl()
{
   delete ui;
}

QList<QAction*> EmulatorControl::menu()
{
   QList<QAction*> items;
   items.append(ui->actionDebugging);
   items.append(ui->actionRun);
   items.append(ui->actionPause);
   items.append(ui->actionStep_CPU);
   items.append(ui->actionStep_Over);
   items.append(ui->actionStep_Out);
   items.append(ui->actionStep_PPU);
   items.append(ui->actionFrame_Advance);
   items.append(ui->actionReset);
   return items;
}

void EmulatorControl::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(debugging);
   ui->stepOverButton->setEnabled(debugging);
   ui->stepOutButton->setEnabled(debugging);
   ui->stepPPUButton->setEnabled(debugging);
   ui->frameAdvance->setEnabled(debugging);
   ui->actionRun->setEnabled(false);
   ui->actionPause->setEnabled(true);
   ui->actionStep_CPU->setEnabled(debugging);
   ui->actionStep_Over->setEnabled(debugging);
   ui->actionStep_Out->setEnabled(debugging);
   ui->actionStep_PPU->setEnabled(debugging);
   ui->actionFrame_Advance->setEnabled(debugging);
}

void EmulatorControl::internalPause()
{
   if ( nesROMIsLoaded() )
   {
      ui->playButton->setEnabled(true);
      ui->pauseButton->setEnabled(false);
      ui->stepCPUButton->setEnabled(debugging);
      ui->stepOverButton->setEnabled(debugging);
      ui->stepOutButton->setEnabled(debugging);
      ui->stepPPUButton->setEnabled(debugging);
      ui->frameAdvance->setEnabled(debugging);
      ui->actionRun->setEnabled(true);
      ui->actionPause->setEnabled(false);
      ui->actionStep_CPU->setEnabled(debugging);
      ui->actionStep_Over->setEnabled(debugging);
      ui->actionStep_Out->setEnabled(debugging);
      ui->actionStep_PPU->setEnabled(debugging);
      ui->actionFrame_Advance->setEnabled(debugging);
   }
   else
   {
      ui->playButton->setEnabled(false);
      ui->pauseButton->setEnabled(false);
      ui->stepCPUButton->setEnabled(debugging);
      ui->stepOverButton->setEnabled(debugging);
      ui->stepOutButton->setEnabled(debugging);
      ui->stepPPUButton->setEnabled(debugging);
      ui->frameAdvance->setEnabled(debugging);
      ui->actionRun->setEnabled(false);
      ui->actionPause->setEnabled(false);
      ui->actionStep_CPU->setEnabled(debugging);
      ui->actionStep_Over->setEnabled(debugging);
      ui->actionStep_Out->setEnabled(debugging);
      ui->actionStep_PPU->setEnabled(debugging);
      ui->actionFrame_Advance->setEnabled(debugging);
   }
}

void EmulatorControl::on_playButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit startEmulation();

   emit focusEmulator();
}

void EmulatorControl::on_pauseButton_clicked()
{
   emit pauseEmulation(true);
}

void EmulatorControl::on_stepCPUButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit stepCPUEmulation();
}

void EmulatorControl::on_stepPPUButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit stepPPUEmulation();
}

void EmulatorControl::on_resetButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit resetEmulator();
}

void EmulatorControl::on_frameAdvance_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit advanceFrame();
}

void EmulatorControl::on_stepOverButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit stepOverCPUEmulation();
}

void EmulatorControl::on_stepOutButton_clicked()
{
   CCC65Interface::isBuildUpToDate();

   emit stepOutCPUEmulation();
}

void EmulatorControl::on_debugButton_toggled(bool checked)
{
   debugging = checked;
   ui->stepCPUButton->setEnabled(checked);
   ui->stepOverButton->setEnabled(checked);
   ui->stepOutButton->setEnabled(checked);
   ui->stepPPUButton->setEnabled(checked);
   ui->frameAdvance->setEnabled(checked);
   ui->actionStep_CPU->setEnabled(checked);
   ui->actionStep_Over->setEnabled(checked);
   ui->actionStep_Out->setEnabled(checked);
   ui->actionStep_PPU->setEnabled(checked);
   ui->actionFrame_Advance->setEnabled(checked);

   if ( debugging )
   {
      nesEnableDebug();
   }
   else
   {
      nesDisableDebug();
   }
}
