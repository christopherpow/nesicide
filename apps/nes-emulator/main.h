#ifndef MAIN_H
#define MAIN_H

#include "nesemulatorthread.h"

#include "ccartridge.h"

#include "mainwindow.h"

extern CCartridge* cartridge;

extern MainWindow* nesicideWindow;
extern NESEmulatorThread* emulator;

#endif // MAIN_H
