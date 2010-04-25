#undef main
#include <SDL.h>
#undef main
#include <QtGui/QApplication>
#include "main.h"
#include "mainwindow.h"

NESEmulatorThread* emulator = new NESEmulatorThread ();
BreakpointWatcherThread* breakpointWatcher = new BreakpointWatcherThread ();
QHash<QString,QDockWidget*> inspectors;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
    delete emulator;

    return result;
}
