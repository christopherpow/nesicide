#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Source/FamiTracker.h"

#include "Source/cqtmfc_famitracker.h"
#include "Source/MainFrm.h"

#include <QFileInfo>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   
   ui->setupUi(this);

   // Initialize the app...
   qtMfcInit(this);
   theApp.InitInstance();
   
   openFile("");
   
   setCentralWidget(theApp.m_pMainWnd->toQWidget());

   QObject::connect(theApp.m_pMainWnd,SIGNAL(addToolBarWidget(QToolBar*)),this,SLOT(addToolBarWidget(QToolBar*)));
   QObject::connect(theApp.m_pMainWnd,SIGNAL(removeToolBarWidget(QToolBar*)),this,SLOT(removeToolBarWidget(QToolBar*)));
   QObject::connect(theApp.m_pMainWnd,SIGNAL(editor_modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));
   QObject::connect(theApp.m_pMainWnd,SIGNAL(documentSaved()),this,SLOT(documentSaved()));
   QObject::connect(theApp.m_pMainWnd,SIGNAL(documentClosed()),this,SLOT(documentClosed()));
   
   restoreGeometry(settings.value("FamiTrackerWindowGeometry").toByteArray());
   restoreState(settings.value("FamiTrackerWindowState").toByteArray());
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_actionExit_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   
   settings.setValue("FamiTrackerWindowGeometry",saveGeometry());
   settings.setValue("FamiTrackerWindowState",saveState());
   
   // Closing the main window kills the app
   close();
}

void MainWindow::addToolBarWidget(QToolBar* toolBar)
{
   addToolBar(toolBar);
}

void MainWindow::removeToolBarWidget(QToolBar* toolBar)
{
   removeToolBar(toolBar);
}

void MainWindow::editor_modificationChanged(bool m)
{
}

void MainWindow::documentSaved()
{
}

void MainWindow::documentClosed()
{
   // TODO: Handle unsaved documents or other pre-close stuffs
   theApp.ExitInstance();

   exit(0);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

void MainWindow::dropEvent(QDropEvent *event)
{
qDebug("dropEvent");
   QList<QUrl> fileUrls;
   QString     fileName;
   QFileInfo   fileInfo;
   QStringList extensions;

   if ( event->mimeData()->hasUrls() )
   {
      fileUrls = event->mimeData()->urls();

      foreach ( QUrl fileUrl, fileUrls )
      {
         fileName = fileUrl.toLocalFile();

         fileInfo.setFile(fileName);

         if ( !fileInfo.suffix().compare("ftm",Qt::CaseInsensitive) )
         {
            openFile(fileName);
            event->acceptProposedAction();
         }
      }
   }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
 
   AfxGetMainWnd()->OnClose();
   
   settings.setValue("FamiTrackerWindowGeometry",saveGeometry());
   settings.setValue("FamiTrackerWindowState",saveState());
   
   // Ignore the close event.  "MFC" will close the document which will trigger app closure.
   event->ignore();
}

