
#include <QtGui/QApplication>
#include "main.h"
#include "mainwindow.h"

NESEmulatorThread* emulator = NULL;
BreakpointWatcherThread* breakpointWatcher = NULL;
QHash<QString,QDockWidget*> inspectors;

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   emulator = new NESEmulatorThread ();
   breakpointWatcher = new BreakpointWatcherThread ();

   // Start emulator and breakpoint-watcher threads...
   emulator->start();
   breakpointWatcher->start();

   MainWindow w;
   w.show();
   int result = a.exec();

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
