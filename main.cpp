#include <QtGui/QApplication>
#include "mainwindow.h"
#include "nesemulatorthread.h"

NESEmulatorThread* emulator = new NESEmulatorThread();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    emulator->start();

    MainWindow w;
    w.show();
    return a.exec();
}
