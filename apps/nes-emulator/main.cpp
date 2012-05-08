#include <QtGui/QApplication>
#include "main.h"
#include "mainwindow.h"

#include "nes_emulator_core.h"

// Main window of application.
MainWindow* nesicideWindow;

// Thread for NES emulator.  This thread runs the NES core.
NESEmulatorThread* emulator = NULL;

// Cartridge for NES emulator.
CCartridge* cartridge = NULL;

int main(int argc, char* argv[])
{
   QApplication nesicideApplication(argc, argv);

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   // Set up default OpenGL format.
   QGLFormat fmt = QGLFormat::defaultFormat();

   // Disable VSYNC waiting.
   fmt.setSwapInterval(0);

   QGLFormat::setDefaultFormat(fmt);

   // Create the NES emulator thread...
   emulator = new NESEmulatorThread ();

   // Start emulator thread...
   emulator->start();

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   nesicideWindow->show();

   int result = nesicideApplication.exec();

   // Properly kill and destroy the thread we created above.
   emulator->kill();
   emulator->wait();

   emulator->deleteLater();
   emulator = NULL;

   return result;
}
