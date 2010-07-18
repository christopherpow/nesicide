#include <QtGui/QApplication>

#include "main.h"
#include "mainwindow.h"
#include "cconfigurator.h"

// Thread for NES emulator.  This thread runs the NES core.
NESEmulatorThread* emulator = NULL;

// Thread for watching for breakpoints ejected by the NES
// emulator thread.
BreakpointWatcherThread* breakpointWatcher = NULL;

// Hash table of debugger inspector windows to support automagic
// opening/closing of inspector windows on things like breakpoints.
QHash<QString,QDockWidget*> inspectors;

// Database of all known games.
CGameDatabaseHandler gameDatabase;

// Main window of application.
MainWindow* nesicideWindow;

// Persistent settings for application.
CConfigurator* CONFIG;

int main(int argc, char *argv[])
{
   QApplication nesicideApplication(argc, argv);

   // Initialize the game database object...
   gameDatabase.initialize("NesCarts (2010-02-08).xml");

   // Initialize the persistent configuration...
   CONFIG = new CConfigurator();

   // Create the NES emulator and breakpoint watcher threads...
   emulator = new NESEmulatorThread ();
   breakpointWatcher = new BreakpointWatcherThread ();

   // Start emulator and breakpoint-watcher threads...
   emulator->start();
   breakpointWatcher->start();

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   nesicideWindow->show();

   int result = nesicideApplication.exec();

   // Properly kill and destroy the threads we created above.
   breakpointWatcher->kill();
   breakpointWatcher->wait();
   emulator->kill();
   emulator->wait();

   delete breakpointWatcher;
   breakpointWatcher = NULL;
   delete emulator;
   emulator = NULL;

   return result;
}
