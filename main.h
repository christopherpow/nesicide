#ifndef MAIN_H
#define MAIN_H

#include "mainwindow.h"

#include "nesemulatorthread.h"
#include "breakpointwatcherthread.h"
#include "compilerthread.h"

#include "cgamedatabasehandler.h"

char* ideGetVersion();

extern qint8 hex_char[];

extern MainWindow* nesicideWindow;

extern NESEmulatorThread* emulator;
extern BreakpointWatcherThread* breakpointWatcher;
extern CompilerThread* compiler;

extern CGameDatabaseHandler gameDatabase;

#endif // MAIN_H
