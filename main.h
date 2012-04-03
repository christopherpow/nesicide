#ifndef MAIN_H
#define MAIN_H

#include "mainwindow.h"

#include "breakpointwatcherthread.h"
#include "compilerthread.h"
#include "searcherthread.h"
#include "testsuiteexecutivedialog.h"

#include "cgamedatabasehandler.h"
#include "cnesicideproject.h"

char* ideGetVersion();

extern MainWindow* nesicideWindow;

extern TestSuiteExecutiveDialog* testSuiteExecutive;

extern CGameDatabaseHandler gameDatabase;

extern CNesicideProject* nesicideProject;

#endif // MAIN_H
