#ifndef MAIN_H
#define MAIN_H

#include "mainwindow.h"

#include "nesemulatorthread.h"
#include "breakpointwatcherthread.h"
#include "compilerthread.h"
#include "searcherthread.h"
#include "testsuiteexecutivedialog.h"

#include "cgamedatabasehandler.h"
#include "cnesicideproject.h"

char* ideGetVersion();

extern MainWindow* nesicideWindow;

extern NESEmulatorThread* emulator;
extern BreakpointWatcherThread* breakpointWatcher;

extern CompilerThread* compiler;

extern SearcherThread* searcher;

extern TestSuiteExecutiveDialog* testSuiteExecutive;

extern CGameDatabaseHandler gameDatabase;

extern CNesicideProject* nesicideProject;

#endif // MAIN_H
