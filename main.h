#ifndef MAIN_H
#define MAIN_H

#include "nesemulatorthread.h"
#include "breakpointwatcherthread.h"
#include "cgamedatabasehandler.h"

#include "mainwindow.h"

extern CGameDatabaseHandler gameDatabase;

extern MainWindow* nesicideWindow;
extern NESEmulatorThread* emulator;
extern BreakpointWatcherThread* breakpointWatcher;

#endif // MAIN_H
