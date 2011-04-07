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

// Modeless dialog for Test Suite executive.
TestSuiteExecutiveDialog* testSuiteExecutive = NULL;

// Interface to compiler.
CompilerThread* compiler = NULL;
const char* compilers[] =
{
   "Internal PASM",
   "External CC65 in PATH",
   NULL
};

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

// The project container.
CNesicideProject* nesicideProject = (CNesicideProject*)NULL;

int main(int argc, char* argv[])
{
   QApplication nesicideApplication(argc, argv);

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   QSettings settings;

   // Initialize the game database object...
   if ( settings.value("useInternalDB",QVariant(true)).toBool() )
   {
      // Use internal resource.
      gameDatabase.initialize(":GameDatabase");
   }
   else
   {
      // Use named file resource.  Default to internal if it's not set.
      gameDatabase.initialize(settings.value("GameDatabase",QVariant(":GameDatabase")).toString());
   }

   // Initialize the plugin manager
   pluginManager = new CPluginManager();

   // Run the startup splash
   StartupSplashDialog* splash = new StartupSplashDialog();
   //splash->exec();
   delete splash;

   // Set up default OpenGL format.
   QGLFormat fmt = QGLFormat::defaultFormat();

   // Disable VSYNC waiting.
   fmt.setSwapInterval(0);

   QGLFormat::setDefaultFormat(fmt);

   // Create the NES emulator and breakpoint watcher threads...
   emulator = new NESEmulatorThread ();
   breakpointWatcher = new BreakpointWatcherThread ();

   // Create the compiler thread...
   compiler = new CompilerThread ();

   // Start breakpoint-watcher thread...
   breakpointWatcher->start();

   // Start compiler thread...
   compiler->start();

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   nesicideWindow->show();

   // Create the Test Suite executive modeless dialog...
   testSuiteExecutive = new TestSuiteExecutiveDialog(nesicideWindow);

   int result = nesicideApplication.exec();

   // Properly kill and destroy the threads we created above.
   breakpointWatcher->kill();
   breakpointWatcher->wait();
   compiler->kill();
   compiler->wait();
   emulator->kill();
   emulator->wait();

   delete breakpointWatcher;
   breakpointWatcher = NULL;
   delete compiler;
   compiler = NULL;
   delete emulator;
   emulator = NULL;

   delete pluginManager;
   pluginManager = NULL;

   return result;
}
