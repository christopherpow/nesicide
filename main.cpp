#include <QtGui/QApplication>

#include "main.h"
#include "mainwindow.h"
#include "cpluginmanager.h"
#include "startupsplashdialog.h"

#if defined ( QT_NO_DEBUG )
static char __ide_version__ [] = "RELEASE";
#else
static char __ide_version__ [] = "DEBUG";
#endif

char* ideGetVersion()
{
   return __ide_version__;
}

// Thread for NES emulator.  This thread runs the NES core.
NESEmulatorThread* emulator = NULL;

// Thread for watching for breakpoints ejected by the NES
// emulator thread.
BreakpointWatcherThread* breakpointWatcher = NULL;

// Thread for compiling NES ROMs.
CompilerThread* compiler = NULL;

// Hash table of debugger inspector windows to support automagic
// opening/closing of inspector windows on things like breakpoints.
QHash<QString,QDockWidget*> inspectors;

// Database of all known games.
CGameDatabaseHandler gameDatabase;

// Main window of application.
MainWindow* nesicideWindow;

// The project container.
CNesicideProject* nesicideProject = (CNesicideProject*)NULL;

int main(int argc, char* argv[])
{
   QApplication nesicideApplication(argc, argv);

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   // Initialize the game database object...
   gameDatabase.initialize("NesCarts (2010-02-08).xml");

   // Initialize the plugin manager
   pluginManager = new CPluginManager();

   // Run the startup splash
   StartupSplashDialog* splash = new StartupSplashDialog();
   //splash->exec();
   delete splash;

   // Create the NES emulator and breakpoint watcher threads...
   emulator = new NESEmulatorThread ();
   breakpointWatcher = new BreakpointWatcherThread ();

   // Start breakpoint-watcher thread...emulator thread starts later.
   breakpointWatcher->start();

   // Create compiler threads...
   compiler = new CompilerThread ();
   
   // Start the compiler thread...
   compiler->start();

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   nesicideWindow->show();

   int result = nesicideApplication.exec();

   // Properly kill and destroy the threads we created above.
   breakpointWatcher->kill();
   breakpointWatcher->wait();
   emulator->kill();
   emulator->wait();
   compiler->kill();
   compiler->wait();

   delete breakpointWatcher;
   breakpointWatcher = NULL;
   delete emulator;
   emulator = NULL;
   delete compiler;
   compiler = NULL;
   delete pluginManager;
   pluginManager = NULL;

   return result;
}
