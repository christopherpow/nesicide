#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cqtmfc_famitracker.h"

#include "Source/FamiTracker.h"
#include "Source/MainFrm.h"
#include "Source/VisualizerWnd.h"

#include <QFileInfo>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow),
   _initialized(false)
{
   ui->setupUi(this);
}

MainWindow::~MainWindow()
{
   delete ui;
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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");
   
   settings.setValue("FamiTrackerWindowGeometry",saveGeometry());
   settings.setValue("FamiTrackerWindowState",saveState());

   // TODO: Handle unsaved documents or other pre-close stuffs
   theApp.ExitInstance();

   exit(0);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

   if ( (event->type() == QEvent::Paint) &&
        pMainFrame &&
        (object == pMainFrame->GetVisualizerWindow()) )
   {
      QPainter p;
      QRect rect = pMainFrame->GetVisualizerWindow()->toQWidget()->rect().adjusted(3,2,-3,-3);
      QPixmap pixmap(rect.size());
      p.begin(&pixmap);
      pMainFrame->GetVisualizerWindow()->toQWidget()->render(&p,QPoint(),QRegion(3,3,rect.width(),rect.height()));
      p.end();
      p.begin(pMainFrame->GetVisualizerWindow()->toQWidget());
      p.drawPixmap(0,0,pixmap.scaled(p.window().size(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
      p.end();
      return true;
   }
   return false;
}

void MainWindow::showEvent(QShowEvent *)
{
   if ( !_initialized )
   {
      QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");

      // Initialize the app...
      qtMfcInit(this);
      bool start = AfxGetApp()->InitInstance();
      // Failed to initialize
      if ( !start )
      {
         exit(0);
      }

      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      setCentralWidget(pMainFrame->toQWidget());
      pMainFrame->toQWidget()->setAcceptDrops(true);
      pMainFrame->GetVisualizerWindow()->toQWidget()->installEventFilter(this);

      QObject::connect(theApp.m_pMainWnd,SIGNAL(addToolBarWidget(QToolBar*)),this,SLOT(addToolBarWidget(QToolBar*)));
      QObject::connect(theApp.m_pMainWnd,SIGNAL(removeToolBarWidget(QToolBar*)),this,SLOT(removeToolBarWidget(QToolBar*)));
      QObject::connect(theApp.m_pMainWnd,SIGNAL(editor_modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));

      restoreGeometry(settings.value("FamiTrackerWindowGeometry").toByteArray());
      restoreState(settings.value("FamiTrackerWindowState").toByteArray());

      _initialized = true;
   }
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
   
   settings.setValue("FamiTrackerWindowGeometry",saveGeometry());
   settings.setValue("FamiTrackerWindowState",saveState());
   
   // CP: Force synchronization because we're terminating in OnClose and the settings object
   // can't synchronize to disk if we wait for that.
   settings.sync();

   if ( AfxGetMainWnd() )
   {
      AfxGetMainWnd()->OnClose();

      // Ignore the close event.  "MFC" will close the document which will trigger app closure.
      event->ignore();
   }
}

