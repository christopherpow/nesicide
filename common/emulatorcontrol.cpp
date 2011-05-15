#include "emulatorcontrol.h"
#include "ui_emulatorcontrol.h"

#include "main.h"

EmulatorControl::EmulatorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmulatorControl)
{
    ui->setupUi(this);
}

EmulatorControl::~EmulatorControl()
{
    delete ui;
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
