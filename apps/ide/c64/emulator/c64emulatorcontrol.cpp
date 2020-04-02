#include "c64emulatorcontrol.h"
#include "ui_c64emulatorcontrol.h"

#include "ccc65interface.h"

#include "c64_emulator_core.h"

#include "cobjectregistry.h"

C64EmulatorControl::C64EmulatorControl(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::C64EmulatorControl),
   debugging(true)
{
   ui->setupUi(this);

   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");
   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");

   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorReset()), this, SLOT(internalPause()));
   QObject::connect(emulator, SIGNAL(emulatorStarted()), this, SLOT(internalPlay()));

   // Buttons are disabled until a cartridge is loaded...then they go to the "pause-just-happened" state.
   QObject::connect(emulator, SIGNAL(machineReady()), this, SLOT(internalPause()));

   // Connect menu actions to slots.
   QObject::connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(on_playButton_clicked()));
   QObject::connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(on_pauseButton_clicked()));
   QObject::connect(ui->actionStep_CPU, SIGNAL(triggered()), this, SLOT(on_stepCPUButton_clicked()));
   QObject::connect(ui->actionStep_Over, SIGNAL(triggered()), this, SLOT(on_stepOverButton_clicked()));
   QObject::connect(ui->actionStep_Out, SIGNAL(triggered()), this, SLOT(on_stepOutButton_clicked()));
   QObject::connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(on_resetButton_clicked()));

   // Connect control signals to emulator.
   QObject::connect(this,SIGNAL(startEmulation()),emulator,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),emulator,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(stepCPUEmulation()),emulator,SLOT(stepCPUEmulation()));
   QObject::connect(this,SIGNAL(stepOverCPUEmulation()),emulator,SLOT(stepOverCPUEmulation()));
   QObject::connect(this,SIGNAL(stepOutCPUEmulation()),emulator,SLOT(stepOutCPUEmulation()));
   QObject::connect(this,SIGNAL(resetEmulator()),emulator,SLOT(resetEmulator()));

   ui->debugButton->setChecked(debugging);

   // Features not yet supported.
   ui->stepOutButton->setEnabled(false);
   ui->actionStep_Out->setEnabled(false);
}

C64EmulatorControl::~C64EmulatorControl()
{
   delete ui;
}

QList<QAction*> C64EmulatorControl::menu()
{
   QList<QAction*> items;
   items.append(ui->actionDebugging);
   items.append(ui->actionRun);
   items.append(ui->actionPause);
   items.append(ui->actionStep_CPU);
   items.append(ui->actionStep_Over);
   items.append(ui->actionStep_Out);
   items.append(ui->actionReset);
   return items;
}

void C64EmulatorControl::internalPlay()
{
   ui->playButton->setEnabled(false);
   ui->pauseButton->setEnabled(true);
   ui->stepCPUButton->setEnabled(false);
   ui->stepOverButton->setEnabled(debugging);
//   ui->stepOutButton->setEnabled(debugging);
   ui->actionRun->setEnabled(false);
   ui->actionPause->setEnabled(true);
   ui->actionStep_CPU->setEnabled(false);
   ui->actionStep_Over->setEnabled(debugging);
//   ui->actionStep_Out->setEnabled(debugging);
}

void C64EmulatorControl::internalPause()
{
   ui->playButton->setEnabled(true);
   ui->pauseButton->setEnabled(false);
   ui->stepCPUButton->setEnabled(debugging);
   ui->stepOverButton->setEnabled(debugging);
//   ui->stepOutButton->setEnabled(debugging);
   ui->actionRun->setEnabled(true);
   ui->actionPause->setEnabled(false);
   ui->actionStep_CPU->setEnabled(debugging);
   ui->actionStep_Over->setEnabled(debugging);
//   ui->actionStep_Out->setEnabled(debugging);
}

void C64EmulatorControl::on_playButton_clicked()
{
   CCC65Interface::instance()->isBuildUpToDate();

   emit startEmulation();
}

void C64EmulatorControl::on_pauseButton_clicked()
{
   emit pauseEmulation(true);
}

void C64EmulatorControl::on_stepCPUButton_clicked()
{
   CCC65Interface::instance()->isBuildUpToDate();

   emit stepCPUEmulation();
}

void C64EmulatorControl::on_resetButton_clicked()
{
   CCC65Interface::instance()->isBuildUpToDate();

   emit resetEmulator();
}

void C64EmulatorControl::on_stepOverButton_clicked()
{
   CCC65Interface::instance()->isBuildUpToDate();

   emit stepOverCPUEmulation();
}

void C64EmulatorControl::on_stepOutButton_clicked()
{
   CCC65Interface::instance()->isBuildUpToDate();

   emit stepOutCPUEmulation();
}

void C64EmulatorControl::on_debugButton_toggled(bool checked)
{
   debugging = checked;
   ui->stepCPUButton->setEnabled(checked);
   ui->stepOverButton->setEnabled(checked);
//   ui->stepOutButton->setEnabled(checked);
   ui->actionStep_CPU->setEnabled(checked);
   ui->actionStep_Over->setEnabled(checked);
//   ui->actionStep_Out->setEnabled(checked);

   if ( debugging )
   {
      c64EnableDebug();
   }
   else
   {
      c64DisableDebug();
   }
}
