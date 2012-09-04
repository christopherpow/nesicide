#ifndef MAIN_H
#define MAIN_H

#include "mainwindow.h"

#include "testsuiteexecutivedialog.h"

#include "cgamedatabasehandler.h"
#include "cnesicideproject.h"

char* ideGetVersion();

extern TestSuiteExecutiveDialog* testSuiteExecutive;

extern CGameDatabaseHandler gameDatabase;

extern CNesicideProject* nesicideProject;

#endif // MAIN_H
