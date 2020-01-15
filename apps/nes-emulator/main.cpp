#include <QApplication>
#include "main.h"
#include "mainwindow.h"

#include "appeventfilter.h"

// Cartridge for NES emulator.
CCartridge* cartridge = NULL;

int main(int argc, char* argv[])
{
   // Main window of application.
   MainWindow* nesicideWindow;

   QApplication nesicideApplication(argc, argv);
   AppEventFilter nesicideEventFilter;
   nesicideApplication.installEventFilter(&nesicideEventFilter); // Installing the event filter

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   // Set up default OpenGL format.
   QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();

   // Disable VSYNC waiting.
   fmt.setSwapInterval(0);

   QSurfaceFormat::setDefaultFormat(fmt);

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   QObject::connect(&nesicideEventFilter,SIGNAL(applicationActivationChanged(bool)),nesicideWindow,SLOT(applicationActivationChanged(bool)));
   nesicideWindow->show();

   int result = nesicideApplication.exec();
   
   delete nesicideWindow;

   return result;
}
