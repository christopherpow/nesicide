#undef main
#include <sdl.h>
#undef main
#include <QtGui/QApplication>
#include "main.h"
#include "mainwindow.h"

NESEmulatorThread* emulator = new NESEmulatorThread ();
BreakpointWatcherThread* breakpointWatcher = new BreakpointWatcherThread ();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Start emulator and breakpoint-watcher threads...
    emulator->start();
    breakpointWatcher->start();

    MainWindow w;
    w.show();
    return a.exec();
}
