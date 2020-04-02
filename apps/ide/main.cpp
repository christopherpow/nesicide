#include <QApplication>

#include "mainwindow.h"
#include "cpluginmanager.h"
#include "startupsplashdialog.h"
#include "environmentsettingsdialog.h"

#include "appeventfilter.h"

#include "model/cprojectmodel.h"

int main(int argc, char* argv[])
{
   // Main window of application.
   MainWindow* nesicideWindow;

   QApplication::setAttribute(Qt::AA_ShareOpenGLContexts,true);

   // Set up default OpenGL format.
   QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();

   // Disable VSYNC waiting.
   //fmt.setSwapInterval(0);

   QSurfaceFormat::setDefaultFormat(fmt);

   QApplication nesicideApplication(argc, argv);

   AppEventFilter nesicideEventFilter;
   nesicideApplication.installEventFilter(&nesicideEventFilter); // Installing the event filter

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   // Run the startup splash
   StartupSplashDialog* splash = new StartupSplashDialog();
   //splash->exec();
   delete splash;

   // Create the project model.
   CProjectModel projectModel;

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow(&projectModel);
   QObject::connect(&nesicideEventFilter,SIGNAL(applicationActivationChanged(bool)),nesicideWindow,SLOT(applicationActivationChanged(bool)));
   nesicideWindow->show();

   int result = nesicideApplication.exec();

   delete nesicideWindow;
   
   return result;
}
