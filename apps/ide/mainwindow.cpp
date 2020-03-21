#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cdockwidgetregistry.h"
#include "cobjectregistry.h"
#include "cpluginmanager.h"

#include "testsuiteexecutivedialog.h"

#include "main.h"

#include "compilerthread.h"
#include "ccc65interface.h"

#include "searcherthread.h"
#include "breakpointwatcherthread.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include "model/cprojectmodel.h"
#include "model/cmusicmodel.h"

#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>

#include "SDL.h"

OutputPaneDockWidget* output = NULL;
ProjectBrowserDockWidget* m_pProjectBrowser = NULL;

MainWindow::MainWindow(CProjectModel *projectModel, QWidget* parent) :
   QMainWindow(parent),
   m_pProjectModel(projectModel),
   m_pNESEmulatorThread(NULL),
   m_pC64EmulatorThread(NULL)
{
   int idx;

   if ( !((QCoreApplication::applicationDirPath().contains("Program Files")) ||
        (QCoreApplication::applicationDirPath().contains("apps/ide"))) ) // Developer builds
   {
      QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
      QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
   }

#if defined(Q_OS_WIN)
   if ( QCoreApplication::applicationDirPath().contains("apps/ide") )
   {
      // Developer build?  Set environment assuming deps/ is at top level.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      QDir dir;
      dir.setPath("../../deps/Windows");
      envdat += dir.absolutePath();
      envdat += "/GnuWin32/bin;";
      dir.setPath("../../deps");
      envdat += dir.absolutePath();
      envdat += "/cc65/bin;";
      dir.setPath("../../tools");
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/text2data;";
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/nsf2data;";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      dir.setPath("../../deps");
      envdat = dir.absolutePath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
   else
   {
      // Set environment.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/GnuWin32/bin;";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/cc65/bin;";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/famitone2/bin;";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
#elif defined(Q_OS_MAC) || defined(Q_OS_MAC64) || defined(Q_OS_MACX)
   if ( QCoreApplication::applicationDirPath().contains("apps/ide") )
   {
      // Developer build?  Set environment assuming deps/ is at top level.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      QDir dir;
      dir.setPath("../../../../../../deps");
      envdat += dir.absolutePath();
      envdat += "/cc65/bin:";
      dir.setPath("../../../../../../tools");
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/text2data:";
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/nsf2data:";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      dir.setPath("../../../../../../deps");
      envdat = dir.absolutePath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
   else
   {
      // Set environment.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/cc65/bin:";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/famitone2/bin:";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
#else
   if ( QCoreApplication::applicationDirPath().contains("apps/ide") )
   {
      // Developer build?  Set environment assuming deps/ is at top level.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      QDir dir;
      dir.setPath("../../../../../../deps");
      envdat += dir.absolutePath();
      envdat += "/cc65/bin:";
      dir.setPath("../../../../../../tools");
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/text2data:";
      envdat += dir.absolutePath();
      envdat += "/famitone2/src/nsf2data:";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      dir.setPath("../../../../../../deps");
      envdat = dir.absolutePath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = dir.absolutePath();
      envdat += "/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
   else
   {
      // Set environment.
      QString envvar = qgetenv("PATH");
      QString envdat = "";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/cc65/bin:";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/famitone2/bin:";
      qputenv("PATH",QString(envdat+envvar).toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65";
      qputenv("CC65_HOME",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/lib";
      qputenv("LD65_LIB",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/asminc";
      qputenv("CA65_INC",envdat.toLatin1());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65/share/cc65/include";
      qputenv("CC65_INC",envdat.toLatin1());
   }
#endif

   qDebug(QString("PATH="+qgetenv("PATH")).toUtf8().data());
   qDebug(QString("CC65_HOME="+qgetenv("CC65_HOME")).toUtf8().data());
   qDebug(QString("LD65_LIB="+qgetenv("LD65_LIB")).toUtf8().data());
   qDebug(QString("CA65_INC="+qgetenv("CA65_INC")).toUtf8().data());
   qDebug(QString("CC65_INC="+qgetenv("CC65_INC")).toUtf8().data());

   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Create application [transient] settings bucket.
   appSettings = new AppSettings();

   // Initialize Environment settings.
   EnvironmentSettingsDialog::readSettings();

   // Initialize the game database object...
   if ( EnvironmentSettingsDialog::useInternalGameDatabase() )
   {
      // Use internal resource.
      gameDatabase.initialize(":GameDatabase");
   }
   else
   {
      // Use named file resource.  Default to internal if it's not set.
      gameDatabase.initialize(EnvironmentSettingsDialog::getGameDatabase());
   }

   // Initialize the plugin manager
   pluginManager = new CPluginManager();

   // Create the search engine thread.
   SearcherThread* searcher = new SearcherThread();
   CObjectRegistry::instance()->addObject ( "Searcher", searcher );

   // Create the breakpoint watcher thread...
   BreakpointWatcherThread* breakpointWatcher = new BreakpointWatcherThread();
   CObjectRegistry::instance()->addObject ( "Breakpoint Watcher", breakpointWatcher );

   // Create the compiler thread...
   CompilerThread* compiler = new CompilerThread();
   QObject::connect(this,SIGNAL(compile()),compiler,SLOT(compile()));
   QObject::connect(this,SIGNAL(clean()),compiler,SLOT(clean()));
   CObjectRegistry::instance()->addObject ( "Compiler", compiler );

   // Create the Test Suite executive modeless dialog...
   testSuiteExecutive = new TestSuiteExecutiveDialog(this);
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),testSuiteExecutive,SLOT(updateTargetMachine(QString)));
   QObject::connect(testSuiteExecutive,SIGNAL(openNesROM(QString,bool)),this,SLOT(openNesROM(QString,bool)));

   // Initialize preferences dialogs.
   EmulatorPrefsDialog::readSettings();

   QObject::connect(compiler, SIGNAL(compileStarted()), this, SLOT(compiler_compileStarted()));
   QObject::connect(compiler, SIGNAL(compileDone(bool)), this, SLOT(compiler_compileDone(bool)));

   generalTextLogger = new CTextLogger();
   buildTextLogger = new CTextLogger();
   debugTextLogger = new CTextLogger();
   searchTextLogger = new CTextLogger();

   setupUi(this);

   nesicideProject = new CNesicideProject();
   QObject::connect(nesicideProject,SIGNAL(createTarget(QString)),this,SLOT(createTarget(QString)));

   QObject::connect(tabWidget,SIGNAL(tabModified(int,bool)),this,SLOT(tabWidget_tabModified(int,bool)));
   QObject::connect(tabWidget,SIGNAL(tabAdded(int)),this,SLOT(tabWidget_tabAdded(int)));
   QObject::connect(tabWidget,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   QObject::connect(this,SIGNAL(checkOpenFiles(QDateTime)),tabWidget,SLOT(checkOpenFiles(QDateTime)));
   QObject::connect(this,SIGNAL(applyProjectProperties()),tabWidget,SLOT(applyProjectProperties()));
   QObject::connect(this,SIGNAL(applyEnvironmentSettings()),tabWidget,SLOT(applyEnvironmentSettings()));
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),tabWidget,SIGNAL(updateTargetMachine(QString)));
   QObject::connect(appSettings,SIGNAL(appSettingsChanged()),tabWidget,SLOT(applyAppSettings()));

   QObject::connect(menuEdit,SIGNAL(aboutToShow()),this,SLOT(menuEdit_aboutToShow()));

   QObject::connect(menuWindow,SIGNAL(aboutToShow()),this,SLOT(menuWindow_aboutToShow()));

   menuWindow->setEnabled(false);

   // Start with no target loaded.
   m_targetLoaded = "none";

   // Set up common UI elements.
   m_pSourceNavigator = new SourceNavigator();
   compilerToolbar->addWidget(m_pSourceNavigator);
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pSourceNavigator,SLOT(updateTargetMachine(QString)));
   CDockWidgetRegistry::instance()->addWidget ( "Source Navigator", m_pSourceNavigator );

   m_pSearchBar = new SearchBar("SearchBar");
   m_pSearchBar->showCloseButton(true);
   centralWidget()->layout()->addWidget(m_pSearchBar);
   m_pSearchBar->hide();
   CDockWidgetRegistry::instance()->addWidget ( "Search Bar", m_pSearchBar );

   m_pProjectBrowser = new ProjectBrowserDockWidget(tabWidget);
   m_pProjectBrowser->setProjectModel(m_pProjectModel);
   addDockWidget(Qt::LeftDockWidgetArea, m_pProjectBrowser );
   m_pProjectBrowser->hide();
   CDockWidgetRegistry::instance()->addWidget ( "Project Browser", m_pProjectBrowser );

//   expandableStatusBar = new CExpandableStatusBar();
//   appStatusBar->insertWidget(0, expandableStatusBar, 100); // Stretch is big to allow it to stretch across app pane.

   output = new OutputPaneDockWidget(this);
   addDockWidget(Qt::BottomDockWidgetArea, output );
//   expandableStatusBar->addExpandingWidget(output);
   QObject::connect(generalTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateGeneralPane(QString)));
   QObject::connect(buildTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateBuildPane(QString)));
   QObject::connect(debugTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateDebugPane(QString)));
   QObject::connect(searchTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateSearchPane(QString)));
   QObject::connect(generalTextLogger,SIGNAL(eraseText()),output,SLOT(eraseGeneralPane()));
   QObject::connect(buildTextLogger,SIGNAL(eraseText()),output,SLOT(eraseBuildPane()));
   QObject::connect(debugTextLogger,SIGNAL(eraseText()),output,SLOT(eraseDebugPane()));
   QObject::connect(searchTextLogger,SIGNAL(eraseText()),output,SLOT(eraseSearchPane()));
   QObject::connect(breakpointWatcher,SIGNAL(showPane(int)),output,SLOT(showPane(int)));
   QObject::connect(output,SIGNAL(addStatusBarWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
   QObject::connect(output,SIGNAL(removeStatusBarWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
   QObject::connect(output,SIGNAL(addPermanentStatusBarWidget(QWidget*)),this,SLOT(addPermanentStatusBarWidget(QWidget*)));
   QObject::connect(output,SIGNAL(removePermanentStatusBarWidget(QWidget*)),this,SLOT(removePermanentStatusBarWidget(QWidget*)));
   QObject::connect(compiler,SIGNAL(compileStarted()),output,SLOT(compiler_compileStarted()));
   QObject::connect(compiler,SIGNAL(compileDone(bool)),output,SLOT(compiler_compileDone(bool)));
   QObject::connect(compiler,SIGNAL(cleanStarted()),output,SLOT(compiler_cleanStarted()));
   QObject::connect(compiler,SIGNAL(cleanDone(bool)),output,SLOT(compiler_cleanDone(bool)));
   QObject::connect(searcher,SIGNAL(searchDone(int)),output,SLOT(searcher_searchDone(int)));
   CDockWidgetRegistry::instance()->addWidget ( "Output", output );
   output->hide();

   generalTextLogger->write("<strong>NESICIDE</strong> Alpha Release");
   generalTextLogger->write("<strong>Plugin Scripting Subsystem:</strong> " + pluginManager->getVersionInfo());

   m_pExecutionInspector = new ExecutionInspectorDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pExecutionInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionInspector );
   m_pExecutionInspector->hide();
   QObject::connect(m_pExecutionInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Execution Inspector", m_pExecutionInspector );

   m_pSymbolInspector = new SymbolWatchDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pSymbolInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pSymbolInspector );
   m_pSymbolInspector->hide();
   QObject::connect(m_pSymbolInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Symbol Inspector", m_pSymbolInspector );

   m_pCodeProfiler = new CodeProfilerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pCodeProfiler,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::LeftDockWidgetArea, m_pCodeProfiler );
   m_pCodeProfiler->hide();
   CDockWidgetRegistry::instance()->addWidget ( "Code Profiler", m_pCodeProfiler );

   emit updateTargetMachine("none");

   // Connect snapTo's from various debuggers to the central widget.
   QObject::connect ( m_pSourceNavigator, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pCodeProfiler, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( output, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );

   // Slots for updating status bar.
   QObject::connect ( tabWidget, SIGNAL(addStatusBarWidget(QWidget*)), this, SLOT(addStatusBarWidget(QWidget*)));
   QObject::connect ( tabWidget, SIGNAL(removeStatusBarWidget(QWidget*)), this, SLOT(removeStatusBarWidget(QWidget*)));

   // Slots for updating tool bars.
   QObject::connect ( tabWidget, SIGNAL(addToolBarWidget(QToolBar*)), this, SLOT(addToolBarWidget(QToolBar*)));
   QObject::connect ( tabWidget, SIGNAL(removeToolBarWidget(QToolBar*)), this, SLOT(removeToolBarWidget(QToolBar*)));

   if ( EnvironmentSettingsDialog::showWelcomeOnStart() )
   {
      tabWidget->addTab(welcome,"Welcome Page");
   }
   else
   {
      tabWidget->removeTab(tabWidget->indexOf(welcome));
   }

   // Load all plugins.
   pluginManager->doInitScript();
   pluginManager->loadPlugins();

   // Instantiate music editor form single instance...
   MusicEditorForm::instance();

   // Set up UI in "Coding" mode.
   appSettings->setAppMode(AppSettings::CodingMode);
   actionCoding_Mode->setChecked(true);
   if ( EnvironmentSettingsDialog::rememberWindowSettings() )
   {
      restoreGeometry(settings.value("CodingModeIDEGeometry").toByteArray());
      restoreState(settings.value("CodingModeIDEState").toByteArray());
   }

   QObject::connect(menuProject,SIGNAL(aboutToShow()),this,SLOT(updateRecentFiles()));
   m_menuRecentFiles = new QMenu("Recent Projects/Files...",menuProject);
   for ( idx = 0; idx < MAX_RECENT_FILES; idx++ )
   {
      QAction* action = new QAction("Recent File "+QString::number(idx+1));
      QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(openRecentFile()));
      m_menuRecentFiles->addAction(action);
   }
   menuRecent_Projects_Files->addActions(m_menuRecentFiles->actions());

   CDockWidgetRegistry::instance()->hideAll();
   QWidget* widget = CDockWidgetRegistry::instance()->getWidget("Project Browser");

   widget->show();

   QStringList argv = QApplication::arguments();

   // Insert last project loaded into argument stream if one isn't specified.
   if ( EnvironmentSettingsDialog::trackRecentProjects() )
   {
      if ( argv.count() == 1 ) // Only executable name is on argv stack.
      {
         argv.append(settings.value("LastProject").toString());
      }
   }

   // Filter for supported files to open.
   QStringList argv_nesproject = argv.filter ( QRegExp(".*[.]nesproject$",Qt::CaseInsensitive) );
   QStringList argv_nes = argv.filter ( QRegExp(".*[.]nes$",Qt::CaseInsensitive) );
   QStringList argv_c64project = argv.filter ( QRegExp(".*[.]c64project$",Qt::CaseInsensitive) );
   QStringList argv_c64 = argv.filter ( QRegExp(".*[.](c64|prg|d64)$",Qt::CaseInsensitive) );
   QStringList argv_ftm = argv.filter ( QRegExp(".*[.](ftm)$",Qt::CaseInsensitive) );

   // Only one file can be specified.
   if ( argv_nes.count()
        +argv_nesproject.count()
        +argv_c64.count()
        +argv_c64project.count() > 1 )
   {
      QString error = "Conflicting command line arguments:\n\n"+argv.join(" ");
      QMessageBox::information ( 0, "Command Line Error", error );
      QApplication::exit(-1);
   }

   if ( argv_nes.count() >= 1 )
   {
      openNesROM(argv_nes.at(0));
   }
   else if ( argv_nesproject.count() >= 1 )
   {
      openNesProject(argv_nesproject.at(0));

      foreach ( QString ftm, argv_ftm )
      {
         qDebug("ftm: %s\n",ftm.toLatin1().data());
         m_pProjectModel->getMusicModel()->addExistingMusicFile(ftm);
      }
   }
   else if ( argv_c64.count() >= 1 )
   {
      openC64File(argv_c64.at(0));
   }
   else if ( argv_c64project.count() >= 1 )
   {
      openC64Project(argv_c64project.at(0));
   }

   projectDataChangesEvent();

   // Sync the Output Pane buttons.
   output->initialize();
   output->hide();

   // Start timer for doing background stuff.
   m_periodicTimer = startTimer(5000);
}

MainWindow::~MainWindow()
{
   BreakpointWatcherThread* breakpointWatcher = dynamic_cast<BreakpointWatcherThread*>(CObjectRegistry::instance()->getObject("Breakpoint Watcher"));
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::instance()->getObject("Compiler"));
   SearcherThread* searcher = dynamic_cast<SearcherThread*>(CObjectRegistry::instance()->getObject("Searcher"));

   killTimer(m_periodicTimer);

   // Destroy music editor form single instance...
   delete MusicEditorForm::instance();

   tabWidget->clear();

   delete breakpointWatcher;
   breakpointWatcher = NULL;
   delete compiler;
   compiler = NULL;
   delete searcher;
   searcher = NULL;

   delete testSuiteExecutive;

   delete generalTextLogger;
   delete buildTextLogger;
   delete debugTextLogger;
   delete searchTextLogger;

   delete nesicideProject;
   delete pluginManager;

   delete m_pNESEmulatorThread;
   delete m_pC64EmulatorThread;
}

void MainWindow::openRecentFile()
{
   QAction* action = dynamic_cast<QAction*>(sender());
   QString fileName = action->text();

   openAnyFile(fileName);
}

void MainWindow::updateRecentFiles()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int idx;
   bool updated = false;

   QStringList recentFiles = settings.value("RecentFiles").toStringList();

   for ( idx = recentFiles.count()-1; idx >= 0; idx-- )
   {
      QFileInfo fi(recentFiles.at(idx));
      if ( !fi.exists() )
      {
         recentFiles.removeAt(idx);
         updated = true;
      }
   }

   for ( idx = 0; idx < recentFiles.count(); idx++ )
   {
      m_menuRecentFiles->actions().at(idx)->setText(recentFiles[idx]);
      m_menuRecentFiles->actions().at(idx)->setVisible(true);
   }
   for ( ; idx < MAX_RECENT_FILES; idx++ )
   {
      m_menuRecentFiles->actions().at(idx)->setVisible(false);
   }

   if ( updated )
   {
      settings.setValue("RecentFiles",recentFiles);
   }
}

void MainWindow::saveRecentFiles(QString fileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   QStringList recentFiles = settings.value("RecentFiles").toStringList();
   recentFiles.removeAll(fileName);
   recentFiles.insert(0,fileName);
   if ( recentFiles.count() > MAX_RECENT_FILES )
   {
      recentFiles.removeLast();
   }
   settings.setValue("RecentFiles",recentFiles);
}

void MainWindow::applicationActivationChanged(bool activated)
{
   QFileInfo fileInfo;
#if QT_VERSION >= 0x040700
   QDateTime now = QDateTime::currentDateTimeUtc();
#else
   QDateTime now = QDateTime::currentDateTime();
#endif
   QString str;
   int result;

   // ACTIVATING
   if ( activated )
   {
      // Check whether the current open project file has changed.
      if ( m_lastActivationChangeTime.isValid() && nesicideProject->isInitialized() )
      {
         fileInfo.setFile(nesicideProject->getProjectFileName());
         if ( fileInfo.lastModified() > m_lastActivationChangeTime )
         {
            str = "The currently loaded project:\n\n";
            str += nesicideProject->getProjectFileName();
            str += "\n\nhas been modified outside of NESICIDE.\n\n";
            str += "Do you want to re-load the project?";
            result = QMessageBox::warning(this,"External interference detected!",str,QMessageBox::Yes,QMessageBox::No);

            if ( result == QMessageBox::Yes )
            {
               if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
               {
                  closeProject();
                  openNesProject(fileInfo.fileName());
               }
               else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
               {
                  closeProject();
                  openC64Project(fileInfo.fileName());
               }
            }
         }
      }

      emit checkOpenFiles(m_lastActivationChangeTime);
   }
   else
   {
      // Only embedded emulators pause when task switching...
      if ( nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) &&
           EmulatorPrefsDialog::getPauseOnTaskSwitch() )
      {
         emit pauseEmulation(false);
      }
   }

   // Save the date/time so we know what to compare against next time.
   m_lastActivationChangeTime = now;
}

void MainWindow::createTarget(QString target)
{
   if ( !target.compare("nes",Qt::CaseInsensitive) )
   {
      createNesUi();
   }
   else if ( !target.compare("c64",Qt::CaseInsensitive) )
   {
      createC64Ui();
   }
}

void MainWindow::createNesUi()
{
   // If we're not set up for NES target, do so.
   if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
   {
      return;
   }

   // If we're set up for some other UI, tear it down.
   if ( !m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
   {
      destroyC64Ui();
   }

   // Set up compiler for appropriate target.
   CCC65Interface::instance()->updateTargetMachine("nes");

   actionEmulation_Window = new QAction("Emulator",this);
   actionEmulation_Window->setObjectName(QString::fromUtf8("actionEmulation_Window"));
   QIcon icon8;
   icon8.addFile(QString::fromUtf8(":/resources/controller.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionEmulation_Window->setIcon(icon8);

   actionAssembly_Inspector = new QAction("Assembly Browser",this);
   actionAssembly_Inspector->setObjectName(QString::fromUtf8("actionAssembly_Inspector"));
   QIcon icon10;
   icon10.addFile(QString::fromUtf8(":/resources/22_code_inspector.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionAssembly_Inspector->setIcon(icon10);
   actionBreakpoint_Inspector = new QAction("Breakpoints",this);
   actionBreakpoint_Inspector->setObjectName(QString::fromUtf8("actionBreakpoint_Inspector"));
   QIcon icon11;
   icon11.addFile(QString::fromUtf8(":/resources/22_breakpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBreakpoint_Inspector->setIcon(icon11);
   actionGfxCHRMemory_Inspector = new QAction("CHR Memory Visualizer",this);
   actionGfxCHRMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxCHRMemory_Inspector"));
   actionGfxOAMMemory_Inspector = new QAction("OAM Memory Visualizer",this);
   actionGfxOAMMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxOAMMemory_Inspector"));
   actionGfxNameTableNESMemory_Inspector = new QAction("NameTable Visualizer",this);
   actionGfxNameTableNESMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxNameTableNESMemory_Inspector"));
   actionBinCPURAM_Inspector = new QAction("CPU Memory",this);
   actionBinCPURAM_Inspector->setObjectName(QString::fromUtf8("actionBinCPURAM_Inspector"));
   actionBinNameTableNESMemory_Inspector = new QAction("NameTable Memory",this);
   actionBinNameTableNESMemory_Inspector->setObjectName(QString::fromUtf8("actionBinNameTableNESMemory_Inspector"));
   actionBinPPURegister_Inspector = new QAction("Registers",this);
   actionBinPPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinPPURegister_Inspector"));
   actionBinAPURegister_Inspector = new QAction("Registers",this);
   actionBinAPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinAPURegister_Inspector"));
   actionBinCHRMemory_Inspector = new QAction("CHR Memory",this);
   actionBinCHRMemory_Inspector->setObjectName(QString::fromUtf8("actionBinCHRMemory_Inspector"));
   actionBinOAMMemory_Inspector = new QAction("OAM Memory",this);
   actionBinOAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinOAMMemory_Inspector"));
   actionBinPaletteNESMemory_Inspector = new QAction("Palette Memory",this);
   actionBinPaletteNESMemory_Inspector->setObjectName(QString::fromUtf8("actionBinPaletteNESMemory_Inspector"));
   actionBinSRAMMemory_Inspector = new QAction("SRAM Memory",this);
   actionBinSRAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinSRAMMemory_Inspector"));
   actionBinEXRAMMemory_Inspector = new QAction("EXRAM Memory",this);
   actionBinEXRAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinEXRAMMemory_Inspector"));
   actionBinCPURegister_Inspector = new QAction("Registers",this);
   actionBinCPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinCPURegister_Inspector"));
   actionBinMapperMemory_Inspector = new QAction("Mapper Memory",this);
   actionBinMapperMemory_Inspector->setObjectName(QString::fromUtf8("actionBinMapperMemory_Inspector"));
   actionBinROM_Inspector = new QAction("PRG-ROM Memory",this);
   actionBinROM_Inspector->setObjectName(QString::fromUtf8("actionBinROM_Inspector"));
   actionBinCartVRAMMemory_Inspector = new QAction("Extra VRAM Memory",this);
   actionBinCartVRAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinCartVRAMMemory_Inspector"));
   actionPPUInformation_Inspector = new QAction("Information",this);
   actionPPUInformation_Inspector->setObjectName(QString::fromUtf8("actionPPUInformation_Inspector"));
   actionJoypadLogger_Inspector = new QAction("Joypad Logger",this);
   actionJoypadLogger_Inspector->setObjectName(QString::fromUtf8("actionJoypadLogger_Inspector"));
   actionJoypadLogger_Inspector->setEnabled(false);
   actionCodeDataLogger_Inspector = new QAction("Code/Data Log Visualizer",this);
   actionCodeDataLogger_Inspector->setObjectName(QString::fromUtf8("actionCodeDataLogger_Inspector"));
   actionExecution_Visualizer_Inspector = new QAction("Execution Visualizer",this);
   actionExecution_Visualizer_Inspector->setObjectName(QString::fromUtf8("actionExecution_Visualizer_Inspector"));
   actionMapperInformation_Inspector = new QAction("Information",this);
   actionMapperInformation_Inspector->setObjectName(QString::fromUtf8("actionMapperInformation_Inspector"));
   actionAPUInformation_Inspector = new QAction("Information",this);
   actionAPUInformation_Inspector->setObjectName(QString::fromUtf8("actionAPUInformation_Inspector"));
   actionConfigure = new QAction("Set up",this);
   actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));
   actionNTSC = new QAction("NTSC",this);
   actionNTSC->setObjectName(QString::fromUtf8("actionNTSC"));
   actionNTSC->setCheckable(true);
   actionPAL = new QAction("PAL",this);
   actionPAL->setObjectName(QString::fromUtf8("actionPAL"));
   actionPAL->setCheckable(true);
   actionDendy = new QAction("Dendy",this);
   actionDendy->setObjectName(QString::fromUtf8("actionDendy"));
   actionDendy->setCheckable(true);
   actionSquare_1 = new QAction("Square 1",this);
   actionSquare_1->setObjectName(QString::fromUtf8("actionSquare_1"));
   actionSquare_1->setCheckable(true);
   actionSquare_2 = new QAction("Square 2",this);
   actionSquare_2->setObjectName(QString::fromUtf8("actionSquare_2"));
   actionSquare_2->setCheckable(true);
   actionTriangle = new QAction("Triangle",this);
   actionTriangle->setObjectName(QString::fromUtf8("actionTriangle"));
   actionTriangle->setCheckable(true);
   actionNoise = new QAction("Noise",this);
   actionNoise->setObjectName(QString::fromUtf8("actionNoise"));
   actionNoise->setCheckable(true);
   actionDelta_Modulation = new QAction("DMC",this);
   actionDelta_Modulation->setObjectName(QString::fromUtf8("actionDelta_Modulation"));
   actionDelta_Modulation->setCheckable(true);
   actionPulse_1VRC6 = new QAction("Pulse 1",this);
   actionPulse_1VRC6->setObjectName(QString::fromUtf8("actionPulse_1VRC6"));
   actionPulse_1VRC6->setCheckable(true);
   actionPulse_2VRC6 = new QAction("Pulse 2",this);
   actionPulse_2VRC6->setObjectName(QString::fromUtf8("actionPulse_2VRC6"));
   actionPulse_2VRC6->setCheckable(true);
   actionSawtoothVRC6 = new QAction("Sawtooth",this);
   actionSawtoothVRC6->setObjectName(QString::fromUtf8("actionSawtoothVRC6"));
   actionSawtoothVRC6->setCheckable(true);
   actionSquare_1MMC5 = new QAction("Square 1",this);
   actionSquare_1MMC5->setObjectName(QString::fromUtf8("actionSquare_1MMC5"));
   actionSquare_1MMC5->setCheckable(true);
   actionSquare_2MMC5 = new QAction("Square 2",this);
   actionSquare_2MMC5->setObjectName(QString::fromUtf8("actionSquare_2MMC5"));
   actionSquare_2MMC5->setCheckable(true);
   actionDMCMMC5 = new QAction("DMC",this);
   actionDMCMMC5->setObjectName(QString::fromUtf8("actionDMCMMC5"));
   actionDMCMMC5->setCheckable(true);
   actionWave_1N106 = new QAction("Wave 1",this);
   actionWave_1N106->setObjectName(QString::fromUtf8("actionWave_1N106"));
   actionWave_1N106->setCheckable(true);
   actionWave_2N106 = new QAction("Wave 2",this);
   actionWave_2N106->setObjectName(QString::fromUtf8("actionWave_2N106"));
   actionWave_2N106->setCheckable(true);
   actionWave_3N106 = new QAction("Wave 3",this);
   actionWave_3N106->setObjectName(QString::fromUtf8("actionWave_3N106"));
   actionWave_3N106->setCheckable(true);
   actionWave_4N106 = new QAction("Wave 4",this);
   actionWave_4N106->setObjectName(QString::fromUtf8("actionWave_4N106"));
   actionWave_4N106->setCheckable(true);
   actionWave_5N106 = new QAction("Wave 5",this);
   actionWave_5N106->setObjectName(QString::fromUtf8("actionWave_5N106"));
   actionWave_5N106->setCheckable(true);
   actionWave_6N106 = new QAction("Wave 6",this);
   actionWave_6N106->setObjectName(QString::fromUtf8("actionWave_6N106"));
   actionWave_6N106->setCheckable(true);
   actionWave_7N106 = new QAction("Wave 7",this);
   actionWave_7N106->setObjectName(QString::fromUtf8("actionWave_7N106"));
   actionWave_7N106->setCheckable(true);
   actionWave_8N106 = new QAction("Wave 8",this);
   actionWave_8N106->setObjectName(QString::fromUtf8("actionWave_8N106"));
   actionWave_8N106->setCheckable(true);
   actionRun_Test_Suite = new QAction("Run Test Suite",this);
   actionRun_Test_Suite->setObjectName(QString::fromUtf8("actionRun_Test_Suite"));
   action1x = new QAction("1x",this);
   action1x->setObjectName(QString::fromUtf8("action1x"));
   action1x->setShortcut(QKeySequence("Ctrl+1"));
   action1x->setCheckable(true);
   action1_5x = new QAction("1.5x",this);
   action1_5x->setObjectName(QString::fromUtf8("action1_5x"));
   action1_5x->setShortcut(QKeySequence("Ctrl+2"));
   action1_5x->setCheckable(true);
   action2x = new QAction("2x",this);
   action2x->setObjectName(QString::fromUtf8("action2x"));
   action2x->setShortcut(QKeySequence("Ctrl+3"));
   action2x->setCheckable(true);
   action2_5x = new QAction("2.5x",this);
   action2_5x->setObjectName(QString::fromUtf8("action2_5x"));
   action2_5x->setShortcut(QKeySequence("Ctrl+4"));
   action2_5x->setCheckable(true);
   action3x = new QAction("3x",this);
   action3x->setObjectName(QString::fromUtf8("action3x"));
   action3x->setShortcut(QKeySequence("Ctrl+5"));
   action3x->setCheckable(true);
   actionLinear_Interpolation = new QAction("Linear Interpolation",this);
   actionLinear_Interpolation->setObjectName(QString::fromUtf8("actionLinear_Interpolation"));
   actionLinear_Interpolation->setShortcut(QKeySequence("Ctrl+9"));
   actionLinear_Interpolation->setCheckable(true);
   action4_3_Aspect = new QAction("4:3 Aspect",this);
   action4_3_Aspect->setObjectName(QString::fromUtf8("action4_3_Aspect"));
   action4_3_Aspect->setShortcut(QKeySequence("Ctrl+0"));
   action4_3_Aspect->setCheckable(true);
   actionFullscreen = new QAction("Fullscreen",this);
   actionFullscreen->setObjectName(QString::fromUtf8("actionFullscreen"));
   actionFullscreen->setShortcut(QKeySequence("F11"));
   actionFullscreen->setCheckable(true);

   menuCPU_Inspectors = new QMenu("CPU",menuDebugger);
   menuCPU_Inspectors->setObjectName(QString::fromUtf8("menuCPU_Inspectors"));
   menuAPU_Inpsectors = new QMenu("APU",menuDebugger);
   menuAPU_Inpsectors->setObjectName(QString::fromUtf8("menuAPU_Inpsectors"));
   menuPPU_Inspectors = new QMenu("PPU",menuDebugger);
   menuPPU_Inspectors->setObjectName(QString::fromUtf8("menuPPU_Inspectors"));
   menuI_O_Inspectors = new QMenu("I/O",menuDebugger);
   menuI_O_Inspectors->setObjectName(QString::fromUtf8("menuI_O_Inspectors"));
   menuCartridge_Inspectors = new QMenu("Cartridge",menuDebugger);
   menuCartridge_Inspectors->setObjectName(QString::fromUtf8("menuCartridge_Inspectors"));
   menuSystem = new QMenu("System",menuEmulator);
   menuSystem->setObjectName(QString::fromUtf8("menuSystem"));
   menuAudio = new QMenu("Audio",menuEmulator);
   menuAudio->setObjectName(QString::fromUtf8("menuAudio"));
   menuAudioMMC5 = new QMenu("MMC5",menuAudio);
   menuAudioMMC5->setObjectName(QString::fromUtf8("menuAudioMMC5"));
   menuAudioVRC6 = new QMenu("VRC6",menuAudio);
   menuAudioVRC6->setObjectName(QString::fromUtf8("menuAudioVRC6"));
   menuAudioN106 = new QMenu("Namco 106",menuAudio);
   menuAudioN106->setObjectName(QString::fromUtf8("menuAudioN106"));
   menuVideo = new QMenu("Video",menuEmulator);
   menuVideo->setObjectName(QString::fromUtf8("menuVideo"));
   menuVideo->addAction(action1x);
   menuVideo->addAction(action1_5x);
   menuVideo->addAction(action2x);
   menuVideo->addAction(action2_5x);
   menuVideo->addAction(action3x);
   menuVideo->addAction(actionFullscreen);
   menuVideo->addSeparator();
   menuVideo->addAction(actionLinear_Interpolation);
   menuVideo->addAction(action4_3_Aspect);

   menuDebugger->addAction(actionAssembly_Inspector);
   menuDebugger->addAction(actionBreakpoint_Inspector);
   menuDebugger->addSeparator();
   menuDebugger->addAction(actionExecution_Visualizer_Inspector);
   menuDebugger->addAction(actionCodeDataLogger_Inspector);
   menuDebugger->addSeparator();
   menuDebugger->addAction(menuCPU_Inspectors->menuAction());
   menuDebugger->addAction(menuPPU_Inspectors->menuAction());
   menuDebugger->addAction(menuAPU_Inpsectors->menuAction());
   menuDebugger->addAction(menuI_O_Inspectors->menuAction());
   menuDebugger->addAction(menuCartridge_Inspectors->menuAction());
   menuCPU_Inspectors->addAction(actionBinCPURegister_Inspector);
   menuCPU_Inspectors->addSeparator();
   menuCPU_Inspectors->addAction(actionBinCPURAM_Inspector);
   menuAPU_Inpsectors->addAction(actionAPUInformation_Inspector);
   menuAPU_Inpsectors->addAction(actionBinAPURegister_Inspector);
   menuPPU_Inspectors->addAction(actionPPUInformation_Inspector);
   menuPPU_Inspectors->addAction(actionBinPPURegister_Inspector);
   menuPPU_Inspectors->addSeparator();
   menuPPU_Inspectors->addAction(actionBinNameTableNESMemory_Inspector);
   menuPPU_Inspectors->addAction(actionBinPaletteNESMemory_Inspector);
   menuPPU_Inspectors->addAction(actionBinOAMMemory_Inspector);
   menuPPU_Inspectors->addSeparator();
   menuPPU_Inspectors->addAction(actionGfxNameTableNESMemory_Inspector);
   menuPPU_Inspectors->addAction(actionGfxOAMMemory_Inspector);
   menuI_O_Inspectors->addAction(actionJoypadLogger_Inspector);
   menuCartridge_Inspectors->addAction(actionMapperInformation_Inspector);
   menuCartridge_Inspectors->addAction(actionBinMapperMemory_Inspector);
   menuCartridge_Inspectors->addSeparator();
   menuCartridge_Inspectors->addAction(actionBinCHRMemory_Inspector);
   menuCartridge_Inspectors->addAction(actionBinEXRAMMemory_Inspector);
   menuCartridge_Inspectors->addAction(actionBinSRAMMemory_Inspector);
   menuCartridge_Inspectors->addAction(actionBinROM_Inspector);
   menuCartridge_Inspectors->addAction(actionBinCartVRAMMemory_Inspector);
   menuCartridge_Inspectors->addSeparator();
   menuCartridge_Inspectors->addAction(actionGfxCHRMemory_Inspector);
   menuEmulator->addAction(menuSystem->menuAction());
   menuEmulator->addAction(menuVideo->menuAction());
   menuEmulator->addAction(menuAudio->menuAction());
   menuEmulator->addSeparator();
   menuEmulator->addAction(actionRun_Test_Suite);
   menuEmulator->addSeparator();
   menuEmulator->addAction(actionConfigure);
   menuSystem->addAction(actionNTSC);
   menuSystem->addAction(actionPAL);
   menuSystem->addAction(actionDendy);
   menuAudio->addAction(actionSquare_1);
   menuAudio->addAction(actionSquare_2);
   menuAudio->addAction(actionTriangle);
   menuAudio->addAction(actionNoise);
   menuAudio->addAction(actionDelta_Modulation);
   menuAudio->addAction(menuAudioMMC5->menuAction());
   menuAudio->addAction(menuAudioVRC6->menuAction());
   menuAudio->addAction(menuAudioN106->menuAction());
   menuAudioMMC5->addAction(actionSquare_1MMC5);
   menuAudioMMC5->addAction(actionSquare_2MMC5);
   menuAudioMMC5->addAction(actionDMCMMC5);
   menuAudioVRC6->addAction(actionPulse_1VRC6);
   menuAudioVRC6->addAction(actionPulse_2VRC6);
   menuAudioVRC6->addAction(actionSawtoothVRC6);
   menuAudioN106->addAction(actionWave_1N106);
   menuAudioN106->addAction(actionWave_2N106);
   menuAudioN106->addAction(actionWave_3N106);
   menuAudioN106->addAction(actionWave_4N106);
   menuAudioN106->addAction(actionWave_5N106);
   menuAudioN106->addAction(actionWave_6N106);
   menuAudioN106->addAction(actionWave_7N106);
   menuAudioN106->addAction(actionWave_8N106);
   menuView->addAction(actionEmulation_Window);

   debuggerToolBar = new QToolBar("Emulator Control",this);
   debuggerToolBar->setObjectName(QString::fromUtf8("debuggerToolBar"));
   QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(0);
   sizePolicy1.setHeightForWidth(debuggerToolBar->sizePolicy().hasHeightForWidth());
   debuggerToolBar->setSizePolicy(sizePolicy1);
   addToolBar(Qt::TopToolBarArea, debuggerToolBar);

   toolToolbar->addAction(actionEmulation_Window);
   addToolBar(toolToolbar);

   if ( !m_pNESEmulatorThread )
   {
      m_pNESEmulatorThread = new NESEmulatorThread();
   }
   CObjectRegistry::instance()->addObject("Emulator",m_pNESEmulatorThread);

   QObject::connect(this,SIGNAL(startEmulation()),m_pNESEmulatorThread,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),m_pNESEmulatorThread,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(primeEmulator()),m_pNESEmulatorThread,SLOT(primeEmulator()));
   QObject::connect(this,SIGNAL(resetEmulator()),m_pNESEmulatorThread,SLOT(resetEmulator()));
   QObject::connect(this,SIGNAL(adjustAudio(int32_t)),m_pNESEmulatorThread,SLOT(adjustAudio(int32_t)));

   m_pNESEmulator = new NESEmulatorDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pNESEmulator,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pNESEmulator );
   m_pNESEmulator->hide();
   CDockWidgetRegistry::instance()->addWidget ( "Emulator", m_pNESEmulator );

   m_pNESEmulatorControl = new NESEmulatorControl();
   debuggerToolBar->addWidget(m_pNESEmulatorControl);
   debuggerToolBar->show();
   QObject::connect(m_pNESEmulatorControl,SIGNAL(focusEmulator()),this,SLOT(focusEmulator()));

   // Add menu for emulator control.  The emulator control provides menu for itself!  =]
   QAction* firstEmuMenuAction = menuEmulator->actions().at(0);
   foreach ( QAction* action, m_pNESEmulatorControl->menu() )
   {
      menuEmulator->insertAction(firstEmuMenuAction,action);
   }
   menuEmulator->insertSeparator(firstEmuMenuAction);

   m_pBreakpointInspector = new BreakpointDockWidget(nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBreakpointInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pBreakpointInspector );
   m_pBreakpointInspector->hide();
   QObject::connect(m_pBreakpointInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Breakpoints", m_pBreakpointInspector );

   m_pAssemblyInspector = new CodeBrowserDockWidget(nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAssemblyInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pAssemblyInspector );
   m_pAssemblyInspector->hide();
   QObject::connect(m_pAssemblyInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Assembly Browser", m_pAssemblyInspector );

   m_pGfxCHRMemoryInspector = new CHRMEMInspector ();
   m_pGfxCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
   m_pGfxCHRMemoryInspector->setWindowTitle("CHR Memory Visualizer");
   m_pGfxCHRMemoryInspector->setObjectName("chrMemoryVisualizer");
   m_pGfxCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxCHRMemoryInspector );
   m_pGfxCHRMemoryInspector->hide();
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(setStatusBarMessage(QString)),this,SLOT(setStatusBarMessage(QString)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(addStatusBarWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(removeStatusBarWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
   CDockWidgetRegistry::instance()->addWidget ( "CHR Memory Visualizer", m_pGfxCHRMemoryInspector );

   m_pGfxOAMMemoryInspector = new OAMVisualizerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pGfxOAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxOAMMemoryInspector );
   m_pGfxOAMMemoryInspector->hide();
   QObject::connect(m_pGfxOAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "OAM Memory Visualizer", m_pGfxOAMMemoryInspector );

   m_pGfxNameTableMemoryInspector = new NameTableVisualizerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pGfxNameTableMemoryInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pGfxNameTableMemoryInspector );
   m_pGfxNameTableMemoryInspector->hide();
   QObject::connect(m_pGfxNameTableMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Name Table Visualizer", m_pGfxNameTableMemoryInspector );

   m_pJoypadLoggerInspector = new JoypadLoggerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pJoypadLoggerInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pJoypadLoggerInspector );
   m_pJoypadLoggerInspector->hide();
   QObject::connect(m_pJoypadLoggerInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Joypad Logger", m_pJoypadLoggerInspector );

   m_pExecutionVisualizer = new ExecutionVisualizerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pExecutionVisualizer,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionVisualizer );
   m_pExecutionVisualizer->hide();
   QObject::connect(m_pExecutionVisualizer,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Execution Visualizer", m_pExecutionVisualizer );

   m_pCodeDataLoggerInspector = new CodeDataLoggerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pCodeDataLoggerInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pCodeDataLoggerInspector );
   m_pCodeDataLoggerInspector->hide();
   QObject::connect(m_pCodeDataLoggerInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Code/Data Logger Inspector", m_pCodeDataLoggerInspector );

   m_pBinCPURegisterInspector = new RegisterInspectorDockWidget(nesGetCpuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURegisterInspector->setObjectName("cpuRegisterInspector");
   m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
   m_pBinCPURegisterInspector->hide();
   QObject::connect(m_pBinCPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "CPU Register Inspector", m_pBinCPURegisterInspector );

   m_pBinCPURAMInspector = new MemoryInspectorDockWidget(nesGetCpuMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURAMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURAMInspector->setObjectName("cpuMemoryInspector");
   m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
   m_pBinCPURAMInspector->hide();
   QObject::connect(m_pBinCPURAMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "CPU RAM Inspector", m_pBinCPURAMInspector );

   m_pBinROMInspector = new MemoryInspectorDockWidget(nesGetCartridgePRGROMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinROMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinROMInspector->setObjectName("cartPRGROMMemoryInspector");
   m_pBinROMInspector->setWindowTitle("PRG-ROM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinROMInspector );
   m_pBinROMInspector->hide();
   QObject::connect(m_pBinROMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "PRG-ROM Inspector", m_pBinROMInspector );

   m_pBinCartVRAMMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeVRAMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCartVRAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCartVRAMMemoryInspector->setObjectName("cartVRAMMemoryInspector");
   m_pBinCartVRAMMemoryInspector->setWindowTitle("Cartridge VRAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCartVRAMMemoryInspector );
   m_pBinCartVRAMMemoryInspector->hide();
   QObject::connect(m_pBinCartVRAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Cartridge VRAM Inspector", m_pBinCartVRAMMemoryInspector );

   m_pBinNameTableMemoryInspector = new MemoryInspectorDockWidget(nesGetPpuNameTableMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinNameTableMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinNameTableMemoryInspector->setObjectName("ppuNameTableMemoryInspector");
   m_pBinNameTableMemoryInspector->setWindowTitle("NameTable Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinNameTableMemoryInspector );
   m_pBinNameTableMemoryInspector->hide();
   QObject::connect(m_pBinNameTableMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "NameTable Inspector", m_pBinNameTableMemoryInspector );

   m_pBinPPURegisterInspector = new RegisterInspectorDockWidget(nesGetPpuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinPPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinPPURegisterInspector->setObjectName("ppuRegisterInspector");
   m_pBinPPURegisterInspector->setWindowTitle("PPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPURegisterInspector );
   m_pBinPPURegisterInspector->hide();
   QObject::connect(m_pBinPPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "PPU Register Inspector", m_pBinPPURegisterInspector );

   m_pPPUInformationInspector = new PPUInformationDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pPPUInformationInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pPPUInformationInspector );
   m_pPPUInformationInspector->hide();
   QObject::connect(m_pPPUInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "PPU Information", m_pPPUInformationInspector );

   m_pBinAPURegisterInspector = new RegisterInspectorDockWidget(nesGetApuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinAPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinAPURegisterInspector->setObjectName("apuRegisterInspector");
   m_pBinAPURegisterInspector->setWindowTitle("APU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinAPURegisterInspector );
   m_pBinAPURegisterInspector->hide();
   QObject::connect(m_pBinAPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "APU Register Inspector", m_pBinAPURegisterInspector );

   m_pAPUInformationInspector = new APUInformationDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pAPUInformationInspector );
   m_pAPUInformationInspector->hide();
   QObject::connect(m_pAPUInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAPUInformationInspector,SLOT(updateTargetMachine(QString)));
   CDockWidgetRegistry::instance()->addWidget ( "APU Information", m_pAPUInformationInspector );

   m_pBinCHRMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeCHRMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCHRMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCHRMemoryInspector->setObjectName("chrMemoryInspector");
   m_pBinCHRMemoryInspector->setWindowTitle("CHR Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCHRMemoryInspector );
   m_pBinCHRMemoryInspector->hide();
   QObject::connect(m_pBinCHRMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "CHR Memory Inspector", m_pBinCHRMemoryInspector );

   m_pBinOAMMemoryInspector = new RegisterInspectorDockWidget(nesGetPpuOamRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinOAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinOAMMemoryInspector->setObjectName("oamMemoryInspector");
   m_pBinOAMMemoryInspector->setWindowTitle("OAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinOAMMemoryInspector );
   m_pBinOAMMemoryInspector->hide();
   QObject::connect(m_pBinOAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "OAM Memory Inspector", m_pBinOAMMemoryInspector );

   m_pBinPaletteMemoryInspector = new MemoryInspectorDockWidget(nesGetPpuPaletteMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinPaletteMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinPaletteMemoryInspector->setObjectName("ppuPaletteMemoryInspector");
   m_pBinPaletteMemoryInspector->setWindowTitle("Palette Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPaletteMemoryInspector );
   m_pBinPaletteMemoryInspector->hide();
   QObject::connect(m_pBinPaletteMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Palette Memory Inspector", m_pBinPaletteMemoryInspector );

   m_pBinSRAMMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeSRAMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinSRAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinSRAMMemoryInspector->setObjectName("cartSRAMMemoryInspector");
   m_pBinSRAMMemoryInspector->setWindowTitle("Cartridge SRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinSRAMMemoryInspector );
   m_pBinSRAMMemoryInspector->hide();
   QObject::connect(m_pBinSRAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Cartridge SRAM Memory Inspector", m_pBinSRAMMemoryInspector );

   m_pBinEXRAMMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeEXRAMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinEXRAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinEXRAMMemoryInspector->setObjectName("cartEXRAMMemoryInspector");
   m_pBinEXRAMMemoryInspector->setWindowTitle("Cartridge EXRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinEXRAMMemoryInspector );
   m_pBinEXRAMMemoryInspector->hide();
   QObject::connect(m_pBinEXRAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Cartridge EXRAM Memory Inspector", m_pBinEXRAMMemoryInspector );

   m_pMapperInformationInspector = new MapperInformationDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pMapperInformationInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pMapperInformationInspector );
   m_pMapperInformationInspector->hide();
   QObject::connect(m_pMapperInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Cartridge Mapper Information", m_pMapperInformationInspector );

   m_pBinMapperMemoryInspector = new RegisterInspectorDockWidget(nesGetCartridgeRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinMapperMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinMapperMemoryInspector->setObjectName("cartMapperRegisterInspector");
   m_pBinMapperMemoryInspector->setWindowTitle("Cartridge Mapper Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinMapperMemoryInspector );
   m_pBinMapperMemoryInspector->hide();
   QObject::connect(m_pBinMapperMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Cartridge Mapper Register Inspector", m_pBinMapperMemoryInspector );

   // Connect slots for new UI elements.
   QObject::connect(action1x,SIGNAL(triggered()),this,SLOT(action1x_triggered()));
   QObject::connect(action1_5x,SIGNAL(triggered()),this,SLOT(action1_5x_triggered()));
   QObject::connect(action2x,SIGNAL(triggered()),this,SLOT(action2x_triggered()));
   QObject::connect(action2_5x,SIGNAL(triggered()),this,SLOT(action2_5x_triggered()));
   QObject::connect(action3x,SIGNAL(triggered()),this,SLOT(action3x_triggered()));
   QObject::connect(actionLinear_Interpolation,SIGNAL(toggled(bool)),this,SLOT(actionLinear_Interpolation_toggled(bool)));
   QObject::connect(action4_3_Aspect,SIGNAL(toggled(bool)),this,SLOT(action4_3_Aspect_toggled(bool)));
   QObject::connect(actionFullscreen,SIGNAL(toggled(bool)),this,SLOT(actionFullscreen_toggled(bool)));
   QObject::connect(actionSquare_1,SIGNAL(toggled(bool)),this,SLOT(actionSquare_1_toggled(bool)));
   QObject::connect(actionSquare_2,SIGNAL(toggled(bool)),this,SLOT(actionSquare_2_toggled(bool)));
   QObject::connect(actionTriangle,SIGNAL(toggled(bool)),this,SLOT(actionTriangle_toggled(bool)));
   QObject::connect(actionNoise,SIGNAL(toggled(bool)),this,SLOT(actionNoise_toggled(bool)));
   QObject::connect(actionDelta_Modulation,SIGNAL(toggled(bool)),this,SLOT(actionDelta_Modulation_toggled(bool)));
   QObject::connect(actionPulse_1VRC6,SIGNAL(toggled(bool)),this,SLOT(actionPulse_1VRC6_toggled(bool)));
   QObject::connect(actionPulse_2VRC6,SIGNAL(toggled(bool)),this,SLOT(actionPulse_2VRC6_toggled(bool)));
   QObject::connect(actionSawtoothVRC6,SIGNAL(toggled(bool)),this,SLOT(actionSawtoothVRC6_toggled(bool)));
   QObject::connect(actionSquare_1MMC5,SIGNAL(toggled(bool)),this,SLOT(actionSquare_1MMC5_toggled(bool)));
   QObject::connect(actionSquare_2MMC5,SIGNAL(toggled(bool)),this,SLOT(actionSquare_2MMC5_toggled(bool)));
   QObject::connect(actionDMCMMC5,SIGNAL(toggled(bool)),this,SLOT(actionDMCMMC5_toggled(bool)));
   QObject::connect(actionWave_1N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_1N106_toggled(bool)));
   QObject::connect(actionWave_2N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_2N106_toggled(bool)));
   QObject::connect(actionWave_3N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_3N106_toggled(bool)));
   QObject::connect(actionWave_4N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_4N106_toggled(bool)));
   QObject::connect(actionWave_5N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_5N106_toggled(bool)));
   QObject::connect(actionWave_6N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_6N106_toggled(bool)));
   QObject::connect(actionWave_7N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_7N106_toggled(bool)));
   QObject::connect(actionWave_8N106,SIGNAL(toggled(bool)),this,SLOT(actionWave_8N106_toggled(bool)));
   QObject::connect(actionPAL,SIGNAL(triggered()),this,SLOT(actionPAL_triggered()));
   QObject::connect(actionNTSC,SIGNAL(triggered()),this,SLOT(actionNTSC_triggered()));
   QObject::connect(actionDendy,SIGNAL(triggered()),this,SLOT(actionDendy_triggered()));
   QObject::connect(actionAssembly_Inspector,SIGNAL(triggered()),this,SLOT(actionAssembly_Inspector_triggered()));
   QObject::connect(actionBreakpoint_Inspector,SIGNAL(triggered()),this,SLOT(actionBreakpoint_Inspector_triggered()));
   QObject::connect(actionEmulation_Window,SIGNAL(triggered()),this,SLOT(actionEmulation_Window_triggered()));
   QObject::connect(actionRun_Test_Suite,SIGNAL(triggered()),this,SLOT(actionRun_Test_Suite_triggered()));
   QObject::connect(actionConfigure,SIGNAL(triggered()),this,SLOT(actionConfigure_triggered()));
   QObject::connect(actionCodeDataLogger_Inspector,SIGNAL(triggered()),this,SLOT(actionCodeDataLogger_Inspector_triggered()));
   QObject::connect(actionExecution_Visualizer_Inspector,SIGNAL(triggered()),this,SLOT(actionExecution_Visualizer_Inspector_triggered()));
   QObject::connect(actionGfxCHRMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionGfxCHRMemory_Inspector_triggered()));
   QObject::connect(actionGfxOAMMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionGfxOAMMemory_Inspector_triggered()));
   QObject::connect(actionGfxNameTableNESMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionGfxNameTableNESMemory_Inspector_triggered()));
   QObject::connect(actionBinCPURegister_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCPURegister_Inspector_triggered()));
   QObject::connect(actionBinCPURAM_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCPURAM_Inspector_triggered()));
   QObject::connect(actionBinROM_Inspector,SIGNAL(triggered()),this,SLOT(actionBinROM_Inspector_triggered()));
   QObject::connect(actionBinCartVRAMMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCartVRAMMemory_Inspector_triggered()));
   QObject::connect(actionBinNameTableNESMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinNameTableNESMemory_Inspector_triggered()));
   QObject::connect(actionBinCHRMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCHRMemory_Inspector_triggered()));
   QObject::connect(actionBinOAMMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinOAMMemory_Inspector_triggered()));
   QObject::connect(actionBinSRAMMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinSRAMMemory_Inspector_triggered()));
   QObject::connect(actionBinEXRAMMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinEXRAMMemory_Inspector_triggered()));
   QObject::connect(actionBinPaletteNESMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinPaletteNESMemory_Inspector_triggered()));
   QObject::connect(actionBinAPURegister_Inspector,SIGNAL(triggered()),this,SLOT(actionBinAPURegister_Inspector_triggered()));
   QObject::connect(actionBinPPURegister_Inspector,SIGNAL(triggered()),this,SLOT(actionBinPPURegister_Inspector_triggered()));
   QObject::connect(actionBinMapperMemory_Inspector,SIGNAL(triggered()),this,SLOT(actionBinMapperMemory_Inspector_triggered()));
   QObject::connect(actionPPUInformation_Inspector,SIGNAL(triggered()),this,SLOT(actionPPUInformation_Inspector_triggered()));
   QObject::connect(actionAPUInformation_Inspector,SIGNAL(triggered()),this,SLOT(actionAPUInformation_Inspector_triggered()));
   QObject::connect(actionMapperInformation_Inspector,SIGNAL(triggered()),this,SLOT(actionMapperInformation_Inspector_triggered()));
   QObject::connect(actionJoypadLogger_Inspector,SIGNAL(triggered()),this,SLOT(actionJoypadLogger_Inspector_triggered()));

   // Connect snapTo's from various debuggers.
   QObject::connect ( m_pExecutionVisualizer, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pBreakpointInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), m_pBinCPURAMInspector, SLOT(snapToHandler(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), m_pBinSRAMMemoryInspector, SLOT(snapToHandler(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), m_pBinEXRAMMemoryInspector, SLOT(snapToHandler(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), m_pBinROMInspector, SLOT(snapToHandler(QString)) );

   updateFromEmulatorPrefs(true);

   m_targetLoaded = "nes";

   SDL_PauseAudio(0);

   emit updateTargetMachine(m_targetLoaded);
}

void MainWindow::destroyNesUi()
{
   // If we're set up for NES, clear it.
   if ( m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
   {
      return;
   }

   SDL_PauseAudio(1);

   CDockWidgetRegistry::instance()->removeWidget ( "Assembly Browser" );
   CDockWidgetRegistry::instance()->removeWidget ( "Breakpoints" );
   CDockWidgetRegistry::instance()->removeWidget ( "Emulator" );
   CDockWidgetRegistry::instance()->removeWidget ( "CHR Memory Visualizer" );
   CDockWidgetRegistry::instance()->removeWidget ( "OAM Memory Visualizer" );
   CDockWidgetRegistry::instance()->removeWidget ( "Name Table Visualizer" );
   CDockWidgetRegistry::instance()->removeWidget ( "Execution Visualizer" );
   CDockWidgetRegistry::instance()->removeWidget ( "Code/Data Logger Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "CPU Register Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "CPU RAM Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "PRG-ROM Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "NameTable Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "PPU Register Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "PPU Information" );
   CDockWidgetRegistry::instance()->removeWidget ( "APU Register Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "APU Information" );
   CDockWidgetRegistry::instance()->removeWidget ( "CHR Memory Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "OAM Memory Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Palette Memory Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge SRAM Memory Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge EXRAM Memory Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge VRAM Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge Mapper Information" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge Mapper Register Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Cartridge VRAM Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "Joypad Logger" );

   // Properly kill and destroy the thread we created above.
   m_pNESEmulatorThread->blockSignals(true);
   emit pauseEmulation(false);

   delete m_pNESEmulatorThread;
   m_pNESEmulatorThread = NULL;

   CObjectRegistry::instance()->removeObject ( "Emulator" );

   m_pNESEmulator->hide();
   removeDockWidget(m_pNESEmulator);
   delete m_pNESEmulator;
   removeDockWidget(m_pAssemblyInspector);
   delete m_pAssemblyInspector;
   removeDockWidget(m_pBreakpointInspector);
   delete m_pBreakpointInspector;
   removeDockWidget(m_pGfxCHRMemoryInspector);
   delete m_pGfxCHRMemoryInspector;
   removeDockWidget(m_pGfxOAMMemoryInspector);
   delete m_pGfxOAMMemoryInspector;
   removeDockWidget(m_pGfxNameTableMemoryInspector);
   delete m_pGfxNameTableMemoryInspector;
   removeDockWidget(m_pExecutionVisualizer);
   delete m_pExecutionVisualizer;
   removeDockWidget(m_pCodeDataLoggerInspector);
   delete m_pCodeDataLoggerInspector;
   removeDockWidget(m_pBinCPURegisterInspector);
   delete m_pBinCPURegisterInspector;
   removeDockWidget(m_pBinCPURAMInspector);
   delete m_pBinCPURAMInspector;
   removeDockWidget(m_pBinROMInspector);
   delete m_pBinROMInspector;
   removeDockWidget(m_pBinCartVRAMMemoryInspector);
   delete m_pBinCartVRAMMemoryInspector;
   removeDockWidget(m_pBinNameTableMemoryInspector);
   delete m_pBinNameTableMemoryInspector;
   removeDockWidget(m_pBinPPURegisterInspector);
   delete m_pBinPPURegisterInspector;
   removeDockWidget(m_pPPUInformationInspector);
   delete m_pPPUInformationInspector;
   removeDockWidget(m_pBinAPURegisterInspector);
   delete m_pBinAPURegisterInspector;
   removeDockWidget(m_pAPUInformationInspector);
   delete m_pAPUInformationInspector;
   removeDockWidget(m_pBinCHRMemoryInspector);
   delete m_pBinCHRMemoryInspector;
   removeDockWidget(m_pBinOAMMemoryInspector);
   delete m_pBinOAMMemoryInspector;
   removeDockWidget(m_pBinPaletteMemoryInspector);
   delete m_pBinPaletteMemoryInspector;
   removeDockWidget(m_pBinSRAMMemoryInspector);
   delete m_pBinSRAMMemoryInspector;
   removeDockWidget(m_pBinEXRAMMemoryInspector);
   delete m_pBinEXRAMMemoryInspector;
   removeDockWidget(m_pMapperInformationInspector);
   delete m_pMapperInformationInspector;
   removeDockWidget(m_pBinMapperMemoryInspector);
   delete m_pBinMapperMemoryInspector;
   removeDockWidget(m_pJoypadLoggerInspector);
   delete m_pJoypadLoggerInspector;

   menuView->removeAction(actionEmulation_Window);
   toolToolbar->removeAction(actionEmulation_Window);

   delete action1x;
   delete action1_5x;
   delete action2x;
   delete action2_5x;
   delete action3x;
   delete actionLinear_Interpolation;
   delete action4_3_Aspect;
   delete actionFullscreen;
   delete actionEmulation_Window;
   delete actionAssembly_Inspector;
   delete actionBreakpoint_Inspector;
   delete actionGfxCHRMemory_Inspector;
   delete actionGfxOAMMemory_Inspector;
   delete actionGfxNameTableNESMemory_Inspector;
   delete actionBinNameTableNESMemory_Inspector;
   delete actionBinPPURegister_Inspector;
   delete actionBinAPURegister_Inspector;
   delete actionBinCHRMemory_Inspector;
   delete actionBinOAMMemory_Inspector;
   delete actionBinPaletteNESMemory_Inspector;
   delete actionBinSRAMMemory_Inspector;
   delete actionBinEXRAMMemory_Inspector;
   delete actionBinCPURAM_Inspector;
   delete actionBinCPURegister_Inspector;
   delete actionBinMapperMemory_Inspector;
   delete actionBinROM_Inspector;
   delete actionBinCartVRAMMemory_Inspector;
   delete actionPPUInformation_Inspector;
   delete actionJoypadLogger_Inspector;
   delete actionCodeDataLogger_Inspector;
   delete actionExecution_Visualizer_Inspector;
   delete actionMapperInformation_Inspector;
   delete actionAPUInformation_Inspector;
   delete actionConfigure;
   delete actionNTSC;
   delete actionPAL;
   delete actionDendy;
   delete actionSquare_1;
   delete actionSquare_2;
   delete actionTriangle;
   delete actionNoise;
   delete actionDelta_Modulation;
   delete actionPulse_1VRC6;
   delete actionPulse_2VRC6;
   delete actionSawtoothVRC6;
   delete actionSquare_1MMC5;
   delete actionSquare_2MMC5;
   delete actionDMCMMC5;
   delete actionWave_1N106;
   delete actionWave_2N106;
   delete actionWave_3N106;
   delete actionWave_4N106;
   delete actionWave_5N106;
   delete actionWave_6N106;
   delete actionWave_7N106;
   delete actionWave_8N106;
   delete actionRun_Test_Suite;
   delete menuCPU_Inspectors;
   delete menuAPU_Inpsectors;
   delete menuPPU_Inspectors;
   delete menuI_O_Inspectors;
   delete menuCartridge_Inspectors;
   delete menuSystem;
   delete menuVideo;
   delete menuAudioVRC6;
   delete menuAudioN106;
   delete menuAudio;
   delete debuggerToolBar;

   m_targetLoaded = "none";
}

void MainWindow::createC64Ui()
{
   // If we're not set up for C64 target, do so.
   if ( !m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
   {
      return;
   }
   // If we're set up for some other UI, tear it down.
   if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
   {
      destroyNesUi();
   }

   // Set up compiler for appropriate target.
   CCC65Interface::instance()->updateTargetMachine("c64");

   actionAssembly_Inspector = new QAction("Assembly Browser",this);
   actionAssembly_Inspector->setObjectName(QString::fromUtf8("actionAssembly_Inspector"));
   QIcon icon12;
   icon12.addFile(QString::fromUtf8(":/resources/22_code_inspector.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionAssembly_Inspector->setIcon(icon12);
   actionBreakpoint_Inspector = new QAction("Breakpoints",this);
   actionBreakpoint_Inspector->setObjectName(QString::fromUtf8("actionBreakpoint_Inspector"));
   QIcon icon11;
   icon11.addFile(QString::fromUtf8(":/resources/22_breakpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBreakpoint_Inspector->setIcon(icon11);
   actionBinCPURAM_Inspector = new QAction("CPU Memory",this);
   actionBinCPURAM_Inspector->setObjectName(QString::fromUtf8("actionBinCPURAM_Inspector"));
   actionBinCPURegister_Inspector = new QAction("Registers",this);
   actionBinCPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinCPURegister_Inspector"));
   actionBinSIDRegister_Inspector = new QAction("Registers",this);
   actionBinSIDRegister_Inspector->setObjectName(QString::fromUtf8("actionBinSIDRegister_Inspector"));
   actionConfigure = new QAction("Set up",this);
   actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));

   menuCPU_Inspectors = new QMenu("CPU",menuDebugger);
   menuCPU_Inspectors->setObjectName(QString::fromUtf8("menuCPU_Inspectors"));

   menuCPU_Inspectors->addAction(actionBinCPURegister_Inspector);
   menuCPU_Inspectors->addSeparator();
   menuCPU_Inspectors->addAction(actionBinCPURAM_Inspector);

   menuSID_Inspectors = new QMenu("SID",menuDebugger);
   menuSID_Inspectors->setObjectName(QString::fromUtf8("menuSID_Inspectors"));

   menuSID_Inspectors->addAction(actionBinSIDRegister_Inspector);

   menuDebugger->addAction(actionAssembly_Inspector);
   menuDebugger->addAction(actionBreakpoint_Inspector);
   menuDebugger->addSeparator();
   menuDebugger->addAction(menuCPU_Inspectors->menuAction());
   menuDebugger->addAction(menuSID_Inspectors->menuAction());

   menuEmulator->addAction(actionConfigure);

   debuggerToolBar = new QToolBar("Emulator Control",this);
   debuggerToolBar->setObjectName(QString::fromUtf8("debuggerToolBar"));
   QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(0);
   sizePolicy1.setHeightForWidth(debuggerToolBar->sizePolicy().hasHeightForWidth());
   debuggerToolBar->setSizePolicy(sizePolicy1);
   addToolBar(Qt::TopToolBarArea, debuggerToolBar);

   m_pC64EmulatorThread = new C64EmulatorThread();
   CObjectRegistry::instance()->addObject("Emulator",m_pC64EmulatorThread);
   QObject::connect(m_pC64EmulatorThread,SIGNAL(emulatorWantsExit()),this,SLOT(close()));

   QObject::connect(this,SIGNAL(startEmulation()),m_pC64EmulatorThread,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),m_pC64EmulatorThread,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(primeEmulator()),m_pC64EmulatorThread,SLOT(primeEmulator()));
   QObject::connect(this,SIGNAL(resetEmulator()),m_pC64EmulatorThread,SLOT(resetEmulator()));

   m_pC64EmulatorControl = new C64EmulatorControl();
   debuggerToolBar->addWidget(m_pC64EmulatorControl);
   debuggerToolBar->show();

   // Add menu for emulator control.  The emulator control provides menu for itself!  =]
   QAction* firstEmuMenuAction = menuEmulator->actions().at(0);
   menuEmulator->insertActions(firstEmuMenuAction,m_pC64EmulatorControl->menu());
   menuEmulator->insertSeparator(firstEmuMenuAction);

   m_pBreakpointInspector = new BreakpointDockWidget(c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBreakpointInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pBreakpointInspector );
   m_pBreakpointInspector->hide();
   QObject::connect(m_pBreakpointInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Breakpoints", m_pBreakpointInspector );

   m_pAssemblyInspector = new CodeBrowserDockWidget(c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAssemblyInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pAssemblyInspector );
   m_pAssemblyInspector->hide();
   QObject::connect(m_pAssemblyInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "Assembly Browser", m_pAssemblyInspector );

   m_pBinCPURegisterInspector = new RegisterInspectorDockWidget(c64GetCpuRegisterDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURegisterInspector->setObjectName("cpuRegisterInspector");
   m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
   m_pBinCPURegisterInspector->hide();
   QObject::connect(m_pBinCPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "CPU Register Inspector", m_pBinCPURegisterInspector );

   m_pBinCPURAMInspector = new MemoryInspectorDockWidget(c64GetCpuMemoryDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURAMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURAMInspector->setObjectName("cpuMemoryInspector");
   m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
   m_pBinCPURAMInspector->hide();
   QObject::connect(m_pBinCPURAMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "CPU RAM Inspector", m_pBinCPURAMInspector );

   m_pBinSIDRegisterInspector = new RegisterInspectorDockWidget(c64GetSidRegisterDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinSIDRegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinSIDRegisterInspector->setObjectName("sidRegisterInspector");
   m_pBinSIDRegisterInspector->setWindowTitle("SID Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinSIDRegisterInspector );
   m_pBinSIDRegisterInspector->hide();
   QObject::connect(m_pBinSIDRegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::instance()->addWidget ( "SID Register Inspector", m_pBinSIDRegisterInspector );

   // Connect slots for new UI elements.
   QObject::connect(actionConfigure,SIGNAL(triggered()),this,SLOT(actionConfigure_triggered()));
   QObject::connect(actionBinCPURegister_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCPURegister_Inspector_triggered()));
   QObject::connect(actionBinCPURAM_Inspector,SIGNAL(triggered()),this,SLOT(actionBinCPURAM_Inspector_triggered()));
   QObject::connect(actionBinSIDRegister_Inspector,SIGNAL(triggered()),this,SLOT(actionBinSIDRegister_Inspector_triggered()));
   QObject::connect(actionAssembly_Inspector,SIGNAL(triggered()),this,SLOT(actionAssembly_Inspector_triggered()));
   QObject::connect(actionBreakpoint_Inspector,SIGNAL(triggered()),this,SLOT(actionBreakpoint_Inspector_triggered()));

   // Connect snapTo's from various debuggers.
   QObject::connect ( m_pBreakpointInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );

   m_targetLoaded = "c64";

   emit updateTargetMachine(m_targetLoaded);
}

void MainWindow::destroyC64Ui()
{
   // If we're set up for C64, clear it.
   if ( m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
   {
      return;
   }

   CDockWidgetRegistry::instance()->removeWidget ( "Assembly Browser" );
   CDockWidgetRegistry::instance()->removeWidget ( "Breakpoints" );
   CDockWidgetRegistry::instance()->removeWidget ( "CPU Register Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "CPU RAM Inspector" );
   CDockWidgetRegistry::instance()->removeWidget ( "SID Register Inspector" );

   removeDockWidget(m_pAssemblyInspector);
   delete m_pAssemblyInspector;
   removeDockWidget(m_pBreakpointInspector);
   delete m_pBreakpointInspector;
   removeDockWidget(m_pBinCPURegisterInspector);
   delete m_pBinCPURegisterInspector;
   removeDockWidget(m_pBinSIDRegisterInspector);
   delete m_pBinSIDRegisterInspector;
   removeDockWidget(m_pBinCPURAMInspector);
   delete m_pBinCPURAMInspector;
   delete actionAssembly_Inspector;
   delete actionBreakpoint_Inspector;
   delete actionBinCPURAM_Inspector;
   delete actionBinCPURegister_Inspector;
   delete actionBinSIDRegister_Inspector;
   delete actionConfigure;
   delete menuCPU_Inspectors;
   delete menuSID_Inspectors;
   delete debuggerToolBar;

   // Properly kill and destroy the thread we created above.
   m_pC64EmulatorThread->kill();
   m_pC64EmulatorThread->wait();

   delete m_pC64EmulatorThread;
   m_pC64EmulatorThread = NULL;

   CObjectRegistry::instance()->removeObject ( "Emulator" );

   m_targetLoaded = "none";
}

void MainWindow::changeEvent(QEvent* e)
{
   QMainWindow::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         retranslateUi(this);
         break;
      default:
         break;
   }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

bool MainWindow::openAnyFile(QString fileName)
{
   QFileInfo fileInfo;
   bool opened = false;

   fileInfo.setFile(fileName);

   if ( !fileInfo.suffix().compare("nesproject",Qt::CaseInsensitive) )
   {
      if ( nesicideProject->isInitialized() )
      {
         closeProject();
      }
      openNesProject(fileName);
   }
   else if ( !fileInfo.suffix().compare("nes",Qt::CaseInsensitive) )
   {
      if ( nesicideProject->isInitialized() )
      {
         closeProject();
      }
      openNesROM(fileName);
   }
   else if ( !fileInfo.suffix().compare("c64project",Qt::CaseInsensitive) )
   {
      if ( nesicideProject->isInitialized() )
      {
         closeProject();
      }
      openC64Project(fileName);
   }
   else if ( !fileInfo.suffix().compare("c64",Qt::CaseInsensitive) ||
             !fileInfo.suffix().compare("prg",Qt::CaseInsensitive) ||
             !fileInfo.suffix().compare("d64",Qt::CaseInsensitive) )
   {
      if ( nesicideProject->isInitialized() )
      {
         closeProject();
      }
      openC64File(fileName);
   }
   else if ( !fileInfo.suffix().compare("ftm",Qt::CaseInsensitive) )
   {
      m_pProjectModel->getMusicModel()->addExistingMusicFile(fileName);
   }
   else
   {
      openFile(fileName);
   }

   return opened;
}

void MainWindow::dropEvent(QDropEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;
   bool        opened;

   if ( event->mimeData()->hasUrls() )
   {
      output->showPane(OutputPaneDockWidget::Output_General);

      fileUrls = event->mimeData()->urls();

      foreach ( QUrl fileUrl, fileUrls )
      {
         fileName = fileUrl.toLocalFile();

         opened = openAnyFile(fileName);
         if ( opened )
         {
            event->acceptProposedAction();
         }
      }
   }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   if ( (event->key() == Qt::Key_F) &&
        (event->modifiers() == Qt::ControlModifier) )
   {
      m_pSearchBar->show();
      m_pSearchBar->setFocus();
   }
}

void MainWindow::hideEvent(QHideEvent */*event*/)
{
   CDockWidgetRegistry::instance()->saveVisibility();
}

void MainWindow::showEvent(QShowEvent */*event*/)
{
   CDockWidgetRegistry::instance()->restoreVisibility();
}

void MainWindow::projectDataChangesEvent()
{
   m_pProjectModel->setProject(nesicideProject);

   m_pProjectBrowser->layoutChangedEvent();
   m_pProjectBrowser->setVisible(nesicideProject->isInitialized());

   // Enabled/Disable actions based on if we have a project loaded or not
   actionProject_Properties->setEnabled(nesicideProject->isInitialized());
   action_Project_Browser->setEnabled(nesicideProject->isInitialized());
   action_Close_Project->setEnabled(nesicideProject->isInitialized());
   actionCompile_Project->setEnabled(nesicideProject->isInitialized());
   actionManage_Add_Ons->setEnabled(nesicideProject->isInitialized());
   actionSave_Project->setEnabled(nesicideProject->isInitialized());
   actionSave_Project_As->setEnabled(nesicideProject->isInitialized());
   actionClean_Project->setEnabled(nesicideProject->isInitialized());
   actionLoad_In_Emulator->setEnabled(nesicideProject->isInitialized());

   if (tabWidget->currentIndex() >= 0)
   {
      ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->currentWidget());

      if ( projectItem && projectItem->isModified() )
      {
         actionSave_Active_Document->setEnabled(projectItem->isModified());
      }
      else
      {
         actionSave_Active_Document->setEnabled(false);
      }
   }

   setWindowTitle(nesicideProject->getProjectTitle().append("[*] - NESICIDE"));
}

void MainWindow::markProjectDirty(bool dirty)
{
   nesicideProject->setDirty(dirty);
   setWindowModified(dirty);
}

void MainWindow::addStatusBarWidget(QWidget *widget)
{
//   expandableStatusBar->addWidget(widget,100);
   appStatusBar->addWidget(widget);
   widget->show();
}

void MainWindow::removeStatusBarWidget(QWidget *widget)
{
   // For some reason on creation the widget isn't there but it's being removed?
//   expandableStatusBar->addWidget(widget,100);
//   expandableStatusBar->removeWidget(widget);
   appStatusBar->addWidget(widget);
   appStatusBar->removeWidget(widget);
}

void MainWindow::addToolBarWidget(QToolBar *toolBar)
{
   addToolBar(toolBar);
   toolBar->show();
}

void MainWindow::removeToolBarWidget(QToolBar *toolBar)
{
   removeToolBar(toolBar);
}

void MainWindow::addPermanentStatusBarWidget(QWidget *widget)
{
//   expandableStatusBar->addPermanentWidget(widget);
   appStatusBar->addPermanentWidget(widget);
   widget->show();
}

void MainWindow::removePermanentStatusBarWidget(QWidget *widget)
{
//   expandableStatusBar->removeWidget(widget);
   appStatusBar->removeWidget(widget);
}

void MainWindow::setStatusBarMessage(QString message)
{
   appStatusBar->showMessage(message,2000);
}

void MainWindow::on_actionSave_Project_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString   fileName;

   fileName = nesicideProject->getProjectFileName();
   if ( !fileName.compare("(unset)",Qt::CaseInsensitive) )
   {
      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath()+QDir::separator()+nesicideProject->getProjectOutputName()+".nesproject",
                                                        "NES Project (*.nesproject)");
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath()+QDir::separator()+nesicideProject->getProjectOutputName()+".c64project",
                                                        "C64 Project (*.c64project)");
      }
   }

   if (!fileName.isEmpty())
   {
      saveProject(fileName);
   }

   nesicideProject->setDirty(false);

   settings.setValue("LastProject",fileName);
}

void MainWindow::saveEmulatorState(QString fileName)
{
   // Only save the emulator state if the SRAM is dirty.
   if ( nesIsSRAMDirty() )
   {
      QFile file(fileName);

      if ( !file.open( QFile::WriteOnly) )
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to open the save state file for writing.");
         return;
      }

#if defined(XML_SAVE_STATE)
      QDomDocument doc;
      QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
      doc.appendChild(instr);

      if (!m_pNESEmulatorThread->serialize(doc, doc))
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to serialize the save state data.");
         file.close();
      }

      // Create a text stream so we can stream the XML data to the file easily.
      QTextStream ts( &file );

      // Use the standard C++ stream function for streaming the string representation of our XML to
      // our file stream.
      ts << doc.toString();
#else
      if (!m_pNESEmulatorThread->serializeContent(file))
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to serialize the save state data.");
         file.close();
      }
#endif

      // And finally close the file.
      file.close();
   }
}

void MainWindow::saveProject(QString fileName)
{
   QFile file(fileName);

   // Save the project file name in the project...
   nesicideProject->setProjectFileName(fileName);

   if ( !file.open( QFile::WriteOnly) )
   {
      QMessageBox::critical(this, "Error", "An error occured while trying to open the project file for writing.");
      return;
   }

   QDomDocument doc;
   QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
   doc.appendChild(instr);

   if (!nesicideProject->serialize(doc, doc))
   {
      QMessageBox::critical(this, "Error", "An error occured while trying to serialize the project data.");
      file.close();
   }

   // Create a text stream so we can stream the XML data to the file easily.
   QTextStream ts( &file );

   // Use the standard C++ stream function for streaming the string representation of our XML to
   // our file stream.
   ts << doc.toString();

   // And finally close the file.
   file.close();

   // Now save the emulator state if a save state file is specified.
   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      if ( !nesicideProject->getProjectCartridgeSaveStateName().isEmpty() )
      {
         saveEmulatorState(nesicideProject->getProjectCartridgeSaveStateName());
      }
   }

   // Mark the project as not dirty...
   markProjectDirty(false);
}

void MainWindow::on_actionSave_Project_As_triggered()
{
   // Allow the user to select a file name. Note that using the static function produces a native
   // file dialog, while creating an instance of QFileDialog results in a non-native file dialog..
   QString fileName;

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath(),
                                                     "NES Project (*.nesproject)");
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath(),
                                                     "C64 Project (*.c64project)");
   }

   if (!fileName.isEmpty())
   {
      saveProject(fileName);
   }
}

void MainWindow::on_actionProject_Properties_triggered()
{
   ProjectPropertiesDialog dlg;

   if (dlg.exec() == QDialog::Accepted)
   {
      nesicideProject->setDirty(true);
   }

   emit applyProjectProperties();
}

void MainWindow::explodeAddOn(int level,QString projectName,QString addonDirName,QString localDirName)
{
   QDir addonDir(addonDirName);
   QDir localDir;
   QString localDirPath;
   QString projectDirPath;
   QFileInfoList addonFileInfos = addonDir.entryInfoList();

   foreach ( QFileInfo fileInfo, addonFileInfos )
   {
      localDirPath = localDirName;
      projectDirPath = "";

      // level 1 tells us what kind of object is being added
      if ( level != 0 )
      {
         localDirPath += "/";
         localDirPath += fileInfo.fileName();
         if ( level > 1 )
         {
            projectDirPath += "/";
         }
         projectDirPath += fileInfo.fileName();
      }
      localDirPath = localDirPath.replace(QRegExp("_in$"),"");
      localDirPath = localDirPath.replace("_includeInBuild","");
      projectDirPath = projectDirPath.replace(QRegExp("_in$"),"");
      projectDirPath = projectDirPath.replace("_includeInBuild","");

      if ( fileInfo.isDir() )
      {
         explodeAddOn(level+1,projectName,fileInfo.filePath(),localDirPath);
      }
      else
      {
         if ( level > 0 )
         {
            // Save the file locally.
            QFile addonFile(fileInfo.filePath());
            QFile localFile(localDirPath);
            QByteArray addonFileContent;

            if ( addonFile.open(QIODevice::ReadOnly) &&
                 localFile.open(QIODevice::ReadWrite|QIODevice::Truncate) )
            {
               addonFileContent = addonFile.readAll();

               addonFileContent.replace("<!project-title!>",projectName.toUtf8());

               localFile.write(addonFileContent);

               if ( fileInfo.filePath().contains("Source Code") )
               {
                  CSourceItem *source = nesicideProject->getProject()->getSources()->addSourceFile(projectDirPath);
                  if ( fileInfo.filePath().endsWith("_includeInBuild") )
                  {
                     source->setIncludeInBuild(false);
                  }
               }
               else if ( fileInfo.filePath().contains("Custom Rules") )
               {
                  QStringList customRuleFiles = nesicideProject->getMakefileCustomRuleFiles();
                  customRuleFiles += projectDirPath;
                  customRuleFiles.removeDuplicates();
                  nesicideProject->setMakefileCustomRuleFiles(customRuleFiles);
               }
               else
               {
                  // what?
               }
            }

            addonFile.close();
            localFile.close();
         }
      }
   }
}

void MainWindow::explodeTemplate(int level,QString templateName,QString projectName,QString templateDirName,QString localDirName,QString* projectFileName)
{
   QDir templateDir(templateDirName);
   QDir localDir;
   QString localDirTemp;
   QFileInfoList templateFileInfos = templateDir.entryInfoList();

   foreach ( QFileInfo fileInfo, templateFileInfos )
   {
      localDirTemp = localDirName;
      localDirTemp += "/";
      if ( level == 0 )
      {
         localDirTemp += projectName;
         localDirTemp += "/";
      }
      localDir.mkpath(localDirTemp);
      if ( fileInfo.fileName().contains("nesproject_in") )
      {
         localDirTemp += fileInfo.fileName().replace("template",projectName);
      }
      else
      {
         localDirTemp += fileInfo.fileName();
      }
      localDirTemp = localDirTemp.replace(QRegExp("_in$"),"");

      localDirTemp.replace(templateName,projectName);

      if ( fileInfo.isDir() )
      {
         explodeTemplate(level+1,templateName,projectName,fileInfo.filePath(),localDirTemp,projectFileName);
      }
      else
      {
         // Save the file locally.
         QFile templateFile(fileInfo.filePath());
         QFile localFile(localDirTemp);
         QByteArray templateFileContent;

         if ( templateFile.open(QIODevice::ReadOnly) &&
              localFile.open(QIODevice::ReadWrite|QIODevice::Truncate) )
         {
            templateFileContent = templateFile.readAll();

            // If this is the project file, spit out the name and replace <!...!>'s.
            if ( !fileInfo.suffix().compare("nesproject_in",Qt::CaseInsensitive) )
            {
               templateFileContent.replace("<!project-title!>",projectName.toUtf8());
               templateFileContent.replace("<!project-mapper!>",QString::number(nesicideProject->getCartridge()->getMapperNumber()).toUtf8());
               (*projectFileName) = localFile.fileName();
            }
            else if ( !fileInfo.suffix().compare("c64project_in",Qt::CaseInsensitive) )
            {
               templateFileContent.replace("<!project-title!>",projectName.toUtf8());
               templateFileContent.replace("<!project-mapper!>",QString::number(nesicideProject->getCartridge()->getMapperNumber()).toUtf8());
               (*projectFileName) = localFile.fileName();
            }

            localFile.write(templateFileContent);
         }

         templateFile.close();
         localFile.close();
      }
   }
}

void MainWindow::explodeINESHeaderTemplate(QString templateName,QString projectName,QString templateDirName,QString localDirName)
{
   QString templateFileName = ":/templates/NES/"+templateName+"/header.s_in";
   QFileInfo fileInfo = QFileInfo(templateFileName);
   QDir localDir;
   QString localDirTemp;

   localDirTemp = localDirName;
   localDirTemp += "/";
   localDirTemp += projectName;
   localDirTemp += "/";
   localDir.mkpath(localDirTemp);
   localDirTemp += fileInfo.fileName().replace("_in","");

   // Save the file locally.
   QFile templateFile(fileInfo.filePath());
   QFile localFile(localDirTemp);
   QString templateFileContent;

   if ( templateFile.open(QIODevice::ReadOnly) &&
        localFile.open(QIODevice::ReadWrite|QIODevice::Truncate) )
   {
      templateFileContent = templateFile.readAll();

      templateFileContent.replace("<!prg-banks!>",QString::number(nesicideProject->getCartridge()->getPrgRomBanks()->childCount(),16));
      templateFileContent.replace("<!chr-banks!>",QString::number(nesicideProject->getCartridge()->getChrRomBanks()->childCount(),16));
      templateFileContent.replace("<!mapper-low!>",QString::number((nesicideProject->getCartridge()->getMapperNumber()&0x0F)<<4,16));
      templateFileContent.replace("<!mapper-high!>",QString::number(nesicideProject->getCartridge()->getMapperNumber()&0xF0,16));

      localFile.write(templateFileContent.toUtf8());
   }

   templateFile.close();
   localFile.close();
}

void MainWindow::on_actionNew_Project_triggered()
{
   on_action_Close_Project_triggered();

   if ( nesicideProject->isInitialized() )
   {
      return;
   }

   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   NewProjectDialog dlg("New Project","Untitled",settings.value("LastProjectBasePath").toString());

   if (dlg.exec() == QDialog::Accepted)
   {
      m_pProjectBrowser->disableNavigation();

      QDir::setCurrent(dlg.getPath());

      if ( dlg.getTemplateIndex() == 0 )
      {
         // Set project target before initializing project...
         if ( dlg.getTarget() == "Commodore 64" )
         {
            createC64Ui();
         }
         else if ( dlg.getTarget() == "Nintendo Entertainment System" )
         {
            createNesUi();
         }
      }
      else
      {
         nesicideProject->terminateProject();

         QString templateDirName = ":/templates/";
         QString projectFileName;
         QDir projectDir;

         // Now 'open' the new project.
         // Set project target before initializing project...
         if ( dlg.getTarget() == "Commodore 64" )
         {
            templateDirName += "C64/";
            templateDirName += dlg.getTemplate();
            templateDirName += "/";

            // Recursively copy the project content to the local location.
            explodeTemplate(0,dlg.getTemplate(),dlg.getName(),templateDirName,dlg.getPath(),&projectFileName);

            openC64Project(projectFileName);
         }
         else if ( dlg.getTarget() == "Nintendo Entertainment System" )
         {
            templateDirName += "NES/";
            templateDirName += dlg.getTemplate();
            templateDirName += "/";

            // Recursively copy the project content to the local location.
            explodeTemplate(0,dlg.getTemplate(),dlg.getName(),templateDirName,dlg.getPath(),&projectFileName);
            explodeINESHeaderTemplate(dlg.getTemplate(),dlg.getName(),templateDirName,dlg.getPath());

            openNesProject(projectFileName);
         }
      }

      emit applyProjectProperties();

      applyAddOns(dlg.getAddOns());
   }
}

void MainWindow::openNesROM(QString fileName,bool runRom)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QFileInfo fi(fileName);

   // Remove any lingering project content
   m_pProjectBrowser->disableNavigation();
   nesicideProject->terminateProject();

   if ( !fi.exists() ) return;

   createNesUi();

   output->showPane(OutputPaneDockWidget::Output_General);

   // Keep recent file list updated.
   saveRecentFiles(fileName);

   // Clear output
   output->clearAllPanes();

   // Create new project from ROM
   // Set project target before initializing project.
   nesicideProject->setProjectTarget("nes");
   nesicideProject->initializeProject();
   nesicideProject->createProjectFromRom(fileName);

   // Set up some default stuff guessing from the path...
   QFileInfo fileInfo(fileName);
   QDir::setCurrent(fileInfo.path());
   nesicideProject->setProjectTitle(fileInfo.completeBaseName());
   nesicideProject->setProjectLinkerOutputName(fileInfo.completeBaseName()+".prg");
   nesicideProject->setProjectCHRROMOutputName(fileInfo.completeBaseName()+".chr");
   nesicideProject->setProjectCartridgeOutputName(fileInfo.fileName());
   nesicideProject->setProjectCartridgeSaveStateName(fileInfo.completeBaseName()+".sav");
   nesicideProject->setProjectDebugInfoName(fileInfo.completeBaseName()+".dbg");

   // Load debugger info if we can find it.
   CCC65Interface::instance()->captureDebugInfo();

   m_pProjectBrowser->enableNavigation();

   if ( !nesicideProject->getProjectCartridgeSaveStateName().isEmpty() )
   {
      QDomDocument saveDoc;
      QFile saveFile( nesicideProject->getProjectCartridgeSaveStateName() );

      if (saveFile.open(QFile::ReadOnly))
      {
         saveDoc.setContent(saveFile.readAll());
         nesicideProject->setSaveStateDoc(saveDoc);
      }
      saveFile.close();
   }

   emit primeEmulator();
   emit resetEmulator();

   if ( runRom && EnvironmentSettingsDialog::runRomOnLoad() )
   {
      emit startEmulation();
   }

   projectDataChangesEvent();

   // Switch to Debugging mode.
   on_actionDebugging_Mode_triggered();

   settings.setValue("LastProject",fileName);
}

void MainWindow::openC64File(QString fileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QFileInfo fi(fileName);

   // Remove any lingering project content
   m_pProjectBrowser->disableNavigation();
   nesicideProject->terminateProject();

   if ( !fi.exists() ) return;

   // Keep recent file list updated.
   saveRecentFiles(fileName);

   createC64Ui();

   output->showPane(OutputPaneDockWidget::Output_General);

   // Clear output
   output->clearAllPanes();

   // Create new project from ROM
   // Set project target before initializing project.
   nesicideProject->setProjectTarget("c64");
   nesicideProject->initializeProject();
//   nesicideProject->createProjectFromRom(fileName);

   // Set up some default stuff guessing from the path...
   QFileInfo fileInfo(fileName);
   QDir::setCurrent(fileInfo.path());
   nesicideProject->setProjectTitle(fileInfo.completeBaseName());

   if ( !fileInfo.suffix().compare("c64",Qt::CaseInsensitive) ||
        !fileInfo.suffix().compare("prg",Qt::CaseInsensitive) )
   {
      nesicideProject->setProjectOutputName(fileInfo.completeBaseName()+".c64");
      nesicideProject->setProjectLinkerOutputName(fileInfo.completeBaseName()+".c64");
      nesicideProject->setProjectDebugInfoName(fileInfo.completeBaseName()+".dbg");

      // Load debugger info if we can find it.
      CCC65Interface::instance()->captureDebugInfo();
   }
   else if ( !fileInfo.suffix().compare("d64",Qt::CaseInsensitive) )
   {
      nesicideProject->setProjectOutputName(fileInfo.completeBaseName()+".d64");
      nesicideProject->setProjectLinkerOutputName(fileInfo.completeBaseName()+".d64");
   }

   nesicideProject->setProjectCHRROMOutputName("");
   nesicideProject->setProjectCartridgeOutputName("");
   nesicideProject->setProjectCartridgeSaveStateName("");

   m_pProjectBrowser->enableNavigation();

   emit resetEmulator();
   emit primeEmulator();

   projectDataChangesEvent();

   settings.setValue("LastProject",fileName);
}

void MainWindow::on_actionCreate_Project_from_File_triggered()
{
   on_action_Close_Project_triggered();

   if ( nesicideProject->isInitialized() )
   {
      return;
   }

   QString romPath = EnvironmentSettingsDialog::romPath();
   QString selectedFilter;
   QString fileName = QFileDialog::getOpenFileName(this, "Open ROM", romPath, "All Files (*.*);;iNES ROM (*.nes);;Commodore 64 Program (*.c64 *.prg);;Commodore 64 Disk Image (*.d64)",&selectedFilter);

   if (fileName.isEmpty())
   {
      return;
   }

   if ( fileName.endsWith(".nes",Qt::CaseInsensitive) )
   {
      openNesROM(fileName);
   }
   else if ( fileName.endsWith(".d64",Qt::CaseInsensitive) || fileName.endsWith(".prg",Qt::CaseInsensitive) || fileName.endsWith(".c64",Qt::CaseInsensitive) )
   {
      openC64File(fileName);
   }
}

void MainWindow::tabWidget_tabModified(int tab, bool modified)
{
   QList<QAction*> actions = menuWindow->actions();
   QString match;

   match = tabWidget->tabText(tab);
   if ( modified )
   {
      match = match.left(match.length()-1);
   }
   else
   {
      match = match + "*";
   }
   foreach ( QAction* action, actions )
   {
      if ( match == action->text() )
      {
         if ( modified )
         {
            match = match + "*";
         }
         else
         {
            match = match.left(match.length()-1);
         }
         action->setText(match);
      }
   }
   actionSave_Active_Document->setEnabled(modified);
}

void MainWindow::windowMenu_triggered()
{
   QAction* action = dynamic_cast<QAction*>(sender());
   int tab;

   if ( action )
   {
      for ( tab = 0; tab < tabWidget->count(); tab++ )
      {
         if ( tabWidget->tabText(tab) == action->text() )
         {
            tabWidget->setCurrentIndex(tab);
         }
      }
   }
}

void MainWindow::tabWidget_tabAdded(int tab)
{
   menuWindow->clear();

   for ( tab = 0; tab < tabWidget->count(); tab++ )
   {
      QAction* action = menuWindow->addAction(tabWidget->tabText(tab));
      action->setCheckable(true);
      QObject::connect(action,SIGNAL(triggered()),this,SLOT(windowMenu_triggered()));
   }
   menuWindow->setEnabled(true);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
   ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(index));
   int tab;

   menuWindow->clear();

   for ( tab = 0; tab < tabWidget->count(); tab++ )
   {
      QAction* action = menuWindow->addAction(tabWidget->tabText(tab));
      action->setCheckable(true);
      QObject::connect(action,SIGNAL(triggered()),this,SLOT(windowMenu_triggered()));
   }
   menuWindow->setEnabled(tabWidget->count()>0);

   if (projectItem)
   {
      QMessageBox::StandardButton ret = projectItem->onCloseQuery();
      if (ret == QMessageBox::Yes)
      {
         projectItem->onSave();
      }

      if (ret != QMessageBox::Cancel)
      {
         tabWidget->removeTab(index);
         projectItem->onClose();
      }
   }
   else
   {
      tabWidget->removeTab(index);
   }
}

void MainWindow::on_action_Project_Browser_triggered()
{
   action_Project_Browser->setChecked(true);
   m_pProjectBrowser->show();
}

void MainWindow::actionEmulation_Window_triggered()
{
   actionEmulation_Window->setChecked(true);
   m_pNESEmulator->show();
}

void MainWindow::closeEvent ( QCloseEvent* event )
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   bool cancel = false;

   if ( actionCoding_Mode->isChecked() )
   {
      settings.setValue("CodingModeIDEGeometry",saveGeometry());
      settings.setValue("CodingModeIDEState",saveState());
   }
   else
   {
      settings.setValue("DebuggingModeIDEGeometry",saveGeometry());
      settings.setValue("DebuggingModeIDEState",saveState());
   }

   // CP: Force synchronization because we're terminating in OnClose and the settings object
   // can't synchronize to disk if we wait for that.
   settings.sync();

   if (nesicideProject->isInitialized())
   {
      cancel = closeProject();

      CDockWidgetRegistry::instance()->removeWidget ( "Source Navigator" );
      CDockWidgetRegistry::instance()->removeWidget ( "Search Bar" );
      CDockWidgetRegistry::instance()->removeWidget ( "Project Browser" );
      CDockWidgetRegistry::instance()->removeWidget ( "Output" );
      CDockWidgetRegistry::instance()->removeWidget ( "Execution Inspector" );
      CDockWidgetRegistry::instance()->removeWidget ( "Symbol Inspector" );
      CDockWidgetRegistry::instance()->removeWidget ( "Code Profiler" );
   }

   if ( cancel )
   {
      event->ignore();
   }
}

void MainWindow::timerEvent(QTimerEvent */*event*/)
{
// CP: This is part way to doing incremental building
// but needs to check if any files have been modified
// and be less intrusive.
//   on_actionCompile_Project_triggered();
}

void MainWindow::openNesProject(QString fileName,bool runRom)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString errors;
   bool    ok;

   if (QFile::exists(fileName))
   {
      // Keep recent file list updated.
      saveRecentFiles(fileName);

      QDomDocument doc;
      QFile file( fileName );

      if (!file.open(QFile::ReadOnly))
      {
         QMessageBox::critical(this, "Error", "Failed to open the project file.");
         return;
      }

      if (!doc.setContent(file.readAll()))
      {
         QMessageBox::critical(this, "Error", "Failed to parse the project xml data.");
         file.close();
         return;
      }

      file.close();

      m_pProjectBrowser->disableNavigation();

      // Set project target before initializing project.
      nesicideProject->setProjectTarget("nes");
      nesicideProject->initializeProject();
      nesicideProject->setProjectFileName(fileName);

      // Clear output
      output->clearAllPanes();

      // Set up some default stuff guessing from the path...
      QFileInfo fileInfo(fileName);
      QDir::setCurrent(fileInfo.path());

      // Load new project content
      ok = nesicideProject->deserialize(doc,doc,errors);
      if ( !ok )
      {
         QMessageBox::warning(this,"Project Load Error", "The project failed to load.\n\n"+errors);

         nesicideProject->terminateProject();
      }

      // Load ROM if it exists.
      if ( !nesicideProject->getProjectCartridgeOutputName().isEmpty() )
      {
         QDir dir(QDir::currentPath());
         QString romName;
         romName = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectCartridgeOutputName()));

         nesicideProject->createProjectFromRom(romName,true);

         // Load debugger info if we can find it.
         CCC65Interface::instance()->captureDebugInfo();

         if ( !nesicideProject->getProjectCartridgeSaveStateName().isEmpty() )
         {
            QDomDocument saveDoc;
            QFile saveFile( nesicideProject->getProjectCartridgeSaveStateName() );

            if (saveFile.open(QFile::ReadOnly))
            {
               saveDoc.setContent(saveFile.readAll());
               nesicideProject->setSaveStateDoc(saveDoc);
            }
            saveFile.close();
         }

         emit primeEmulator();
         emit resetEmulator();

         if ( runRom && EnvironmentSettingsDialog::runRomOnLoad() )
         {
            emit startEmulation();
         }
      }

      m_pProjectBrowser->enableNavigation();

      settings.setValue("LastProject",fileName);

      projectDataChangesEvent();
   }
   else
   {
      nesicideProject->terminateProject();

      projectDataChangesEvent();
   }
}

void MainWindow::openC64Project(QString fileName,bool run)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString errors;
   bool    ok;

   if (QFile::exists(fileName))
   {
      // Keep recent file list updated.
      saveRecentFiles(fileName);

      QDomDocument doc;
      QFile file( fileName );

      if (!file.open(QFile::ReadOnly))
      {
         QMessageBox::critical(this, "Error", "Failed to open the project file.");
         return;
      }

      if (!doc.setContent(file.readAll()))
      {
         QMessageBox::critical(this, "Error", "Failed to parse the project xml data.");
         file.close();
         return;
      }

      file.close();

      m_pProjectBrowser->disableNavigation();

      // Set project target before initializing project.
      nesicideProject->setProjectTarget("c64");
      nesicideProject->initializeProject();
      nesicideProject->setProjectFileName(fileName);

      // Clear output
      output->clearAllPanes();

      // Set up some default stuff guessing from the path...
      QFileInfo fileInfo(fileName);
      QDir::setCurrent(fileInfo.path());

      // Load new project content
      ok = nesicideProject->deserialize(doc,doc,errors);
      if ( !ok )
      {
         QMessageBox::warning(this,"Project Load Error", "The project failed to load.\n\n"+errors);

         nesicideProject->terminateProject();
      }

      // Load C64 image if it exists.
      if ( !nesicideProject->getProjectLinkerOutputName().isEmpty() )
      {
         QDir dir(QDir::currentPath());
         QString c64Name;
         c64Name = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectLinkerOutputName()));

         // Load debugger info if we can find it.
         CCC65Interface::instance()->captureDebugInfo();

         emit resetEmulator();
      }

      m_pProjectBrowser->enableNavigation();

      settings.setValue("LastProject",fileName);

      projectDataChangesEvent();
   }
   else
   {
      nesicideProject->terminateProject();

      projectDataChangesEvent();
   }
}

void MainWindow::on_actionOpen_Project_triggered()
{
   on_action_Close_Project_triggered();

   if ( nesicideProject->isInitialized() )
   {
      return;
   }

   QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "NES Project (*.nesproject);;Commodore 64 Project (*.c64project)");

   if (fileName.isEmpty())
   {
      return;
   }

   if ( fileName.endsWith(".nesproject",Qt::CaseInsensitive) )
   {
      openNesProject(fileName);
   }
   else
   {
      openC64Project(fileName);
   }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
   ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(index));
   int idx;

   if ( projectItem )
   {
      QList<QAction*> actions = menuEdit->actions();
      for ( idx = actions.count()-1; idx >= 2; idx-- )
      {
         menuEdit->removeAction(actions.at(idx));
      }
      menuEdit->addActions(projectItem->editorMenu().actions());
      if ( projectItem->isModified() )
      {
         actionSave_Active_Document->setEnabled(projectItem->isModified());
      }
      else
      {
         actionSave_Active_Document->setEnabled(false);
      }
   }
}

void MainWindow::on_actionSave_Active_Document_triggered()
{
   ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->currentWidget());

   if (projectItem)
   {
      projectItem->onSave();
   }
}

void MainWindow::on_actionOutput_Window_triggered()
{
   output->show();
}

void MainWindow::on_actionCompile_Project_triggered()
{
   int tab;

   output->showPane(OutputPaneDockWidget::Output_Build);
   emit pauseEmulation(false);

   if ( EnvironmentSettingsDialog::saveAllOnCompile() )
   {
      on_actionSave_Project_triggered();

      // Try to save all opened editors
      for ( tab = 0; tab < tabWidget->count(); tab++ )
      {
         ICenterWidgetItem* item = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(tab));
         if ( item )
         {
            if ( item->isModified() )
            {
               item->onSave();
            }
         }
      }
   }

   emit compile();
}

void MainWindow::compiler_compileStarted()
{
   actionCompile_Project->setEnabled(false);
   actionLoad_In_Emulator->setEnabled(false);
}

void MainWindow::compiler_compileDone(bool /*bOk*/)
{
   actionCompile_Project->setEnabled(true);
   actionLoad_In_Emulator->setEnabled(true);

   projectDataChangesEvent();
}

void MainWindow::on_actionExecution_Inspector_triggered()
{
   m_pExecutionInspector->setVisible(true);
}

void MainWindow::actionExecution_Visualizer_Inspector_triggered()
{
   m_pExecutionVisualizer->setVisible(true);
}

void MainWindow::actionBreakpoint_Inspector_triggered()
{
   m_pBreakpointInspector->setVisible(true);
}

void MainWindow::actionGfxCHRMemory_Inspector_triggered()
{
   m_pGfxCHRMemoryInspector->setVisible(true);
}

void MainWindow::actionJoypadLogger_Inspector_triggered()
{
   m_pJoypadLoggerInspector->setVisible(true);
}

void MainWindow::actionGfxOAMMemory_Inspector_triggered()
{
   m_pGfxOAMMemoryInspector->setVisible(true);
}

void MainWindow::actionGfxNameTableNESMemory_Inspector_triggered()
{
   m_pGfxNameTableMemoryInspector->setVisible(true);
}

void MainWindow::actionBinOAMMemory_Inspector_triggered()
{
   m_pBinOAMMemoryInspector->setVisible(true);
}

void MainWindow::actionBinCPURegister_Inspector_triggered()
{
   m_pBinCPURegisterInspector->setVisible(true);
}

void MainWindow::actionBinCPURAM_Inspector_triggered()
{
   m_pBinCPURAMInspector->setVisible(true);
}

void MainWindow::actionBinROM_Inspector_triggered()
{
   m_pBinROMInspector->setVisible(true);
}

void MainWindow::actionBinCartVRAMMemory_Inspector_triggered()
{
   m_pBinCartVRAMMemoryInspector->setVisible(true);
}

void MainWindow::actionBinNameTableNESMemory_Inspector_triggered()
{
   m_pBinNameTableMemoryInspector->setVisible(true);
}

void MainWindow::actionBinPaletteNESMemory_Inspector_triggered()
{
   m_pBinPaletteMemoryInspector->setVisible(true);
}

void MainWindow::actionBinPPURegister_Inspector_triggered()
{
   m_pBinPPURegisterInspector->setVisible(true);
}

void MainWindow::actionBinAPURegister_Inspector_triggered()
{
   m_pBinAPURegisterInspector->setVisible(true);
}

void MainWindow::actionBinCHRMemory_Inspector_triggered()
{
   m_pBinCHRMemoryInspector->setVisible(true);
}

void MainWindow::actionBinSRAMMemory_Inspector_triggered()
{
   m_pBinSRAMMemoryInspector->setVisible(true);
}

void MainWindow::actionBinEXRAMMemory_Inspector_triggered()
{
   m_pBinEXRAMMemoryInspector->setVisible(true);
}

void MainWindow::actionBinMapperMemory_Inspector_triggered()
{
   m_pBinMapperMemoryInspector->setVisible(true);
}

void MainWindow::actionAssembly_Inspector_triggered()
{
   m_pAssemblyInspector->setVisible(true);
}

void MainWindow::actionCodeDataLogger_Inspector_triggered()
{
   m_pCodeDataLoggerInspector->setVisible(true);
}

void MainWindow::actionPPUInformation_Inspector_triggered()
{
   m_pPPUInformationInspector->setVisible(true);
}

void MainWindow::actionAPUInformation_Inspector_triggered()
{
   m_pAPUInformationInspector->setVisible(true);
}

void MainWindow::actionMapperInformation_Inspector_triggered()
{
   m_pMapperInformationInspector->setVisible(true);
}

void MainWindow::on_actionSymbol_Watch_triggered()
{
   m_pSymbolInspector->setVisible(true);
}

void MainWindow::on_actionCode_Profiler_triggered()
{
   m_pCodeProfiler->setVisible(true);
}

void MainWindow::on_actionSearch_triggered()
{
   output->hide();
   output->show();
   output->resetPane(OutputPaneDockWidget::Output_Search);
   output->showPane(OutputPaneDockWidget::Output_Search);
}

void MainWindow::on_action_About_Nesicide_triggered()
{
   AboutDialog* dlg = new AboutDialog(this);
   dlg->exec();
   delete dlg;
}

bool MainWindow::closeProject()
{
   QList<QAction*> actions = menuWindow->actions();
   QAction* action;
   bool cancel = false;
   int idx;

   // Try to close all opened editors
   for ( idx = tabWidget->count()-1; idx >= 0; idx-- )
   {
      ICenterWidgetItem* item = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(idx));
      if ( item )
      {
         tabWidget->setCurrentWidget(tabWidget->widget(idx));
         QMessageBox::StandardButton ret = item->onCloseQuery();
         if ( ret == QMessageBox::Yes )
         {
            item->onSave();
         }
         tabWidget->removeTab(idx);
      }
   }

   if (nesicideProject->isDirty())
   {
      int result = QMessageBox::question(this,"Save Project?","Your project settings, project content, or debugger content has changed, would you like to save the project?",QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel);
      if ( result == QMessageBox::Yes )
      {
         on_actionSave_Project_triggered();
      }
      else if ( result == QMessageBox::Cancel )
      {
         cancel = true;
      }
   }

   if ( !cancel )
   {
      // Close all inspectors
      CDockWidgetRegistry::instance()->saveVisibility();
      CDockWidgetRegistry::instance()->hideAll();

      m_pSourceNavigator->shutdown();

      // Stop the emulator if it is running
      emit pauseEmulation(false);

      // Now save the emulator state if a save state file is specified.
      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         if ( !nesicideProject->getProjectCartridgeSaveStateName().isEmpty() )
         {
            saveEmulatorState(nesicideProject->getProjectCartridgeSaveStateName());
         }
      }

      // Terminate the project and let the IDE know
      m_pProjectBrowser->disableNavigation();

      foreach ( action, actions )
      {
         QObject::disconnect(action,SIGNAL(triggered()),this,SLOT(windowMenu_triggered()));
         menuWindow->removeAction(action);
      }
      menuWindow->setEnabled(menuWindow->actions().count()>0);

      CCC65Interface::instance()->clear();

      nesicideProject->terminateProject();

      emit primeEmulator();
      emit resetEmulator();

      if ( EnvironmentSettingsDialog::showWelcomeOnStart() )
      {
         tabWidget->addTab(welcome,"Welcome Page");
      }

      // Clear output
      output->clearAllPanes();

      // Let the UI know what's up
      projectDataChangesEvent();

      if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
      {
         destroyNesUi();
      }
      else if ( !m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
      {
         destroyC64Ui();
      }
   }

   return cancel;
}

void MainWindow::on_action_Close_Project_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.setValue("LastProject","");

   closeProject();
}

void MainWindow::actionDendy_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_DENDY);
   actionNTSC->setChecked(false);
   actionPAL->setChecked(false);
   actionDendy->setChecked(true);
   nesSetSystemMode(MODE_DENDY);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::actionNTSC_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_NTSC);
   actionNTSC->setChecked(true);
   actionPAL->setChecked(false);
   actionDendy->setChecked(false);
   nesSetSystemMode(MODE_NTSC);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::actionPAL_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_PAL);
   actionNTSC->setChecked(false);
   actionPAL->setChecked(true);
   actionDendy->setChecked(false);
   nesSetSystemMode(MODE_PAL);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::actionDelta_Modulation_toggled(bool value)
{
   EmulatorPrefsDialog::setDMCEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             0x10);
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3));
   }
}

void MainWindow::actionNoise_toggled(bool value)
{
   EmulatorPrefsDialog::setNoiseEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             0x08|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::actionTriangle_toggled(bool value)
{
   EmulatorPrefsDialog::setTriangleEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             0x04|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::actionSquare_2_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare2Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             0x02|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::actionSquare_1_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(0x01|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::actionSawtoothVRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setSawtoothVRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 0x04);
   }
   else
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1));
   }
}

void MainWindow::actionPulse_2VRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setPulse2VRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 0x02|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
   else
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
}

void MainWindow::actionPulse_1VRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setPulse1VRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(0x01|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
   else
   {
      nesSetVRC6AudioChannelMask((EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
}

void MainWindow::actionDMCMMC5_toggled(bool value)
{
   EmulatorPrefsDialog::setDMCMMC5Enabled(value);
   if ( value )
   {
      nesSetMMC5AudioChannelMask(EmulatorPrefsDialog::getSquare1MMC5Enabled()|
                                 0x04|
                                 (EmulatorPrefsDialog::getDMCMMC5Enabled()<<2));
   }
   else
   {
      nesSetMMC5AudioChannelMask(EmulatorPrefsDialog::getSquare1MMC5Enabled()|
                                 (EmulatorPrefsDialog::getDMCMMC5Enabled()<<2));
   }
}

void MainWindow::actionSquare_2MMC5_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare2MMC5Enabled(value);
   if ( value )
   {
      nesSetMMC5AudioChannelMask(EmulatorPrefsDialog::getSquare1MMC5Enabled()|
                                 (EmulatorPrefsDialog::getSquare2MMC5Enabled()<<1)|
                                 0x04);
   }
   else
   {
      nesSetMMC5AudioChannelMask(EmulatorPrefsDialog::getSquare1MMC5Enabled()|
                                 (EmulatorPrefsDialog::getSquare2MMC5Enabled()<<1));
   }
}

void MainWindow::actionSquare_1MMC5_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1MMC5Enabled(value);
   if ( value )
   {
      nesSetMMC5AudioChannelMask(0x01|
                                 (EmulatorPrefsDialog::getSquare2MMC5Enabled()<<1)|
                                 (EmulatorPrefsDialog::getDMCMMC5Enabled()<<2));
   }
   else
   {
      nesSetMMC5AudioChannelMask((EmulatorPrefsDialog::getSquare2MMC5Enabled()<<1)|
                                 (EmulatorPrefsDialog::getDMCMMC5Enabled()<<2));
   }
}

void MainWindow::actionWave_8N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave8N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 0x80);
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6));
   }
}

void MainWindow::actionWave_7N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave7N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 0x40|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_6N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave6N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 0x20|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_5N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave5N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 0x10|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_4N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave4N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 0x08|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_3N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave3N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 0x04|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_2N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave2N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 0x02|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::actionWave_1N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave1N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask(0x01|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionEnvironment_Settings_triggered()
{
   EnvironmentSettingsDialog dlg;

   int result = dlg.exec();

   if ( result == QDialog::Accepted )
   {
      emit applyEnvironmentSettings();
   }
}

void MainWindow::updateFromEmulatorPrefs(bool initial)
{
   // Synchronize UI elements with changes.
   if ( initial || EmulatorPrefsDialog::systemSettingsChanged() )
   {
      // Set TV standard to use.
      int systemMode = EmulatorPrefsDialog::getTVStandard();
      actionNTSC->setChecked(systemMode==MODE_NTSC);
      actionPAL->setChecked(systemMode==MODE_PAL);
      actionDendy->setChecked(systemMode==MODE_DENDY);
      nesSetSystemMode(systemMode);

      bool breakOnKIL = EmulatorPrefsDialog::getPauseOnKIL();
      nesSetBreakOnKIL(breakOnKIL);
   }

   if ( initial || EmulatorPrefsDialog::audioSettingsChanged() )
   {
      bool square1 = EmulatorPrefsDialog::getSquare1Enabled();
      bool square2 = EmulatorPrefsDialog::getSquare2Enabled();
      bool triangle = EmulatorPrefsDialog::getTriangleEnabled();
      bool noise = EmulatorPrefsDialog::getNoiseEnabled();
      bool dmc = EmulatorPrefsDialog::getDMCEnabled();
      uint32_t mask = ((square1<<0)|(square2<<1)|(triangle<<2)|(noise<<3)|(dmc<<4));

      actionSquare_1->setChecked(square1);
      actionSquare_2->setChecked(square2);
      actionTriangle->setChecked(triangle);
      actionNoise->setChecked(noise);
      actionDelta_Modulation->setChecked(dmc);
      nesSetAudioChannelMask(mask);

      bool pulse1VRC6 = EmulatorPrefsDialog::getPulse1VRC6Enabled();
      bool pulse2VRC6 = EmulatorPrefsDialog::getPulse2VRC6Enabled();
      bool sawtoothVRC6 = EmulatorPrefsDialog::getSawtoothVRC6Enabled();
      mask = ((pulse1VRC6<<0)|(pulse2VRC6<<1)|(sawtoothVRC6<<2));

      actionPulse_1VRC6->setChecked(pulse1VRC6);
      actionPulse_2VRC6->setChecked(pulse2VRC6);
      actionSawtoothVRC6->setChecked(sawtoothVRC6);
      nesSetVRC6AudioChannelMask(mask);

      bool square1MMC5 = EmulatorPrefsDialog::getSquare1MMC5Enabled();
      bool square2MMC5 = EmulatorPrefsDialog::getSquare2MMC5Enabled();
      bool dmcMMC5 = EmulatorPrefsDialog::getDMCMMC5Enabled();
      mask = ((square1MMC5<<0)|(square2MMC5<<1)|(dmcMMC5<<2));

      actionSquare_1MMC5->setChecked(square1MMC5);
      actionSquare_2MMC5->setChecked(square2MMC5);
      actionDMCMMC5->setChecked(dmcMMC5);
      nesSetMMC5AudioChannelMask(mask);

      bool wave1N106 = EmulatorPrefsDialog::getWave1N106Enabled();
      bool wave2N106 = EmulatorPrefsDialog::getWave2N106Enabled();
      bool wave3N106 = EmulatorPrefsDialog::getWave3N106Enabled();
      bool wave4N106 = EmulatorPrefsDialog::getWave4N106Enabled();
      bool wave5N106 = EmulatorPrefsDialog::getWave5N106Enabled();
      bool wave6N106 = EmulatorPrefsDialog::getWave6N106Enabled();
      bool wave7N106 = EmulatorPrefsDialog::getWave7N106Enabled();
      bool wave8N106 = EmulatorPrefsDialog::getWave8N106Enabled();
      mask = ((wave1N106<<0)|(wave2N106<<1)|(wave3N106<<2)|(wave4N106<<3)|
              (wave5N106<<4)|(wave6N106<<5)|(wave7N106<<6)|(wave8N106<<7));

      actionWave_1N106->setChecked(wave1N106);
      actionWave_2N106->setChecked(wave2N106);
      actionWave_3N106->setChecked(wave3N106);
      actionWave_4N106->setChecked(wave4N106);
      actionWave_5N106->setChecked(wave5N106);
      actionWave_6N106->setChecked(wave6N106);
      actionWave_7N106->setChecked(wave7N106);
      actionWave_8N106->setChecked(wave8N106);
      nesSetN106AudioChannelMask(mask);
   }

   if ( initial || EmulatorPrefsDialog::videoSettingsChanged() )
   {
      switch ( EmulatorPrefsDialog::getScalingFactor() )
      {
      case 0:
         // 1x
         action1x_triggered();
         break;
      case 1:
         // 1.5x
         action1_5x_triggered();
         break;
      case 2:
         // 2x
         action2x_triggered();
         break;
      case 3:
         // 2.5x
         action2_5x_triggered();
         break;
      case 4:
         // 3x
         action3x_triggered();
         break;
      }
      actionLinear_Interpolation->setChecked(EmulatorPrefsDialog::getLinearInterpolation());
      m_pNESEmulator->setLinearInterpolation(EmulatorPrefsDialog::getLinearInterpolation());
      action4_3_Aspect->setChecked(EmulatorPrefsDialog::get43Aspect());
      m_pNESEmulator->set43Aspect(EmulatorPrefsDialog::get43Aspect());
   }

   if ( initial || EmulatorPrefsDialog::controllerSettingsChanged() )
   {
      // Set up controllers.
      nesSetControllerType(0,EmulatorPrefsDialog::getControllerType(0));
      nesSetControllerSpecial(0,EmulatorPrefsDialog::getControllerSpecial(0));
      nesSetControllerType(1,EmulatorPrefsDialog::getControllerType(1));
      nesSetControllerSpecial(1,EmulatorPrefsDialog::getControllerSpecial(1));
   }
}

void MainWindow::actionConfigure_triggered()
{
   EmulatorPrefsDialog dlg(nesicideProject->getProjectTarget());

   dlg.exec();

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      updateFromEmulatorPrefs(false);
   }
}

void MainWindow::on_actionOnline_Help_triggered()
{
   tabWidget->addTab(welcome,"Welcome Page");
}

void MainWindow::on_actionLoad_In_Emulator_triggered()
{
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::instance()->getObject("Compiler"));

   output->showPane(OutputPaneDockWidget::Output_Build);

   if ( compiler->assembledOk() )
   {
      actionLoad_In_Emulator->setEnabled(false);

      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         buildTextLogger->write("<b>Loading ROM...</b>");

         if ( !CCC65Interface::instance()->captureINESImage() )
         {
            buildTextLogger->write("<font color='red'><b>Load NES failed.</b></font>");
            return;
         }

         if ( !CCC65Interface::instance()->captureDebugInfo() )
         {
             buildTextLogger->write("<font color='red'><b>Loading debug information failed.</b></font>");
         }

         emit primeEmulator();
         emit resetEmulator();

         buildTextLogger->write("<b>Load complete.</b>");

         // Switch to Debugging mode.
         on_actionDebugging_Mode_triggered();
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         buildTextLogger->write("<b>Loading C64...</b>");

         if ( !CCC65Interface::instance()->captureDebugInfo() )
         {
             buildTextLogger->write("<font color='red'><b>Loading debug information failed.</b></font>");
         }

         emit resetEmulator();

         buildTextLogger->write("<b>Load complete.</b>");
      }
   }
   else
   {
      buildTextLogger->write("<font color='red'><b>Load failed.</b></font>");
   }
}

void MainWindow::actionRun_Test_Suite_triggered()
{
   testSuiteExecutive->show();
}

void MainWindow::on_actionE_xit_triggered()
{
}

void MainWindow::on_actionClean_Project_triggered()
{
   output->showPane(OutputPaneDockWidget::Output_Build);

   emit clean();
}

void MainWindow::openFile(QString file)
{
   QDir dir(QDir::currentPath());
   QString fileName = dir.fromNativeSeparators(dir.filePath(file));
   QFile fileIn(fileName);

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
   {
      CodeEditorForm* editor = new CodeEditorForm(fileName,QString(fileIn.readAll()));

      fileIn.close();

      tabWidget->addTab(editor, fileName);
      tabWidget->setCurrentWidget(editor);
   }
   else
   {
      // CPTODO: fail silently?
   }
}

void MainWindow::actionFullscreen_toggled(bool value)
{
   if ( value )
   {
      m_bEmulatorFloating = m_pNESEmulator->isFloating();
      m_pNESEmulator->setFloating(true);
      m_pNESEmulator->showFullScreen();
      m_pNESEmulator->setFocus();
   }
   else
   {
      m_pNESEmulator->showNormal();
      m_pNESEmulator->setFloating(m_bEmulatorFloating);
      m_pNESEmulator->setFocus();
   }
}

void MainWindow::focusEmulator()
{
   m_pNESEmulator->setFocus();
}

void MainWindow::menuWindow_aboutToShow()
{
   int tab;

   for ( tab = 0; tab < tabWidget->count(); tab++ )
   {
      if ( tab == tabWidget->currentIndex() )
      {
         menuWindow->actions().at(tab)->setChecked(true);
      }
      else
      {
         menuWindow->actions().at(tab)->setChecked(false);
      }
   }
}

void MainWindow::menuEdit_aboutToShow()
{
   ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->currentWidget());
   int idx;

   if ( projectItem )
   {
      QList<QAction*> actions = menuEdit->actions();
      for ( idx = actions.count()-1; idx >= 2; idx-- )
      {
         menuEdit->removeAction(actions.at(idx));
      }
      menuEdit->addActions(projectItem->editorMenu().actions());
   }
   else
   {
      QList<QAction*> actions = menuEdit->actions();
      for ( idx = actions.count()-1; idx >= 2; idx-- )
      {
         menuEdit->removeAction(actions.at(idx));
      }
   }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
   QMessageBox::aboutQt(this);
}

void MainWindow::actionBinSIDRegister_Inspector_triggered()
{
   m_pBinSIDRegisterInspector->setVisible(true);
}

void MainWindow::action1x_triggered()
{
   if ( m_pNESEmulator->isFullScreen() )
   {
      actionFullscreen->setChecked(false);
   }
   EmulatorPrefsDialog::setScalingFactor(0);
   m_pNESEmulator->setScalingFactor(1.0);
   action1x->setChecked(true);
   action1_5x->setChecked(false);
   action2x->setChecked(false);
   action2_5x->setChecked(false);
   action3x->setChecked(false);
}

void MainWindow::action1_5x_triggered()
{
   if ( m_pNESEmulator->isFullScreen() )
   {
      actionFullscreen->setChecked(false);
   }
   EmulatorPrefsDialog::setScalingFactor(1);
   m_pNESEmulator->setScalingFactor(1.5);
   action1x->setChecked(false);
   action1_5x->setChecked(true);
   action2x->setChecked(false);
   action2_5x->setChecked(false);
   action3x->setChecked(false);
}

void MainWindow::action2x_triggered()
{
   if ( m_pNESEmulator->isFullScreen() )
   {
      actionFullscreen->setChecked(false);
   }
   EmulatorPrefsDialog::setScalingFactor(2);
   m_pNESEmulator->setScalingFactor(2.0);
   action1x->setChecked(false);
   action1_5x->setChecked(false);
   action2x->setChecked(true);
   action2_5x->setChecked(false);
   action3x->setChecked(false);
}

void MainWindow::action2_5x_triggered()
{
   if ( m_pNESEmulator->isFullScreen() )
   {
      actionFullscreen->setChecked(false);
   }
   EmulatorPrefsDialog::setScalingFactor(3);
   m_pNESEmulator->setScalingFactor(2.5);
   action1x->setChecked(false);
   action1_5x->setChecked(false);
   action2x->setChecked(false);
   action2_5x->setChecked(true);
   action3x->setChecked(false);
}

void MainWindow::action3x_triggered()
{
   if ( m_pNESEmulator->isFullScreen() )
   {
      actionFullscreen->setChecked(false);
   }
   EmulatorPrefsDialog::setScalingFactor(4);
   m_pNESEmulator->setScalingFactor(3.0);
   action1x->setChecked(false);
   action1_5x->setChecked(false);
   action2x->setChecked(false);
   action2_5x->setChecked(false);
   action3x->setChecked(true);
}

void MainWindow::actionLinear_Interpolation_toggled(bool )
{
   EmulatorPrefsDialog::setLinearInterpolation(actionLinear_Interpolation->isChecked());
   m_pNESEmulator->setLinearInterpolation(actionLinear_Interpolation->isChecked());
}

void MainWindow::action4_3_Aspect_toggled(bool )
{
   EmulatorPrefsDialog::set43Aspect(action4_3_Aspect->isChecked());
   m_pNESEmulator->set43Aspect(action4_3_Aspect->isChecked());
   switch ( EmulatorPrefsDialog::getScalingFactor() )
   {
   case 0:
      action1x->trigger();
      break;
   case 1:
      action1_5x->trigger();
      break;
   case 2:
      action2x->trigger();
      break;
   case 3:
      action2_5x->trigger();
      break;
   case 4:
      action3x->trigger();
      break;
   }
}

void MainWindow::on_actionCoding_Mode_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   if ( actionDebugging_Mode->isChecked() )
   {
      QWidget* widget;
      widget = CDockWidgetRegistry::instance()->getWidget("Output");
      widget->hide();

      settings.setValue("DebuggingModeIDEGeometry",saveGeometry());
      settings.setValue("DebuggingModeIDEState",saveState());

      if ( EnvironmentSettingsDialog::rememberWindowSettings() )
      {
         hide();
         restoreGeometry(settings.value("CodingModeIDEGeometry").toByteArray());
         restoreState(settings.value("CodingModeIDEState").toByteArray());
         show();
      }

      CDockWidgetRegistry::instance()->hideAll();

      widget = CDockWidgetRegistry::instance()->getWidget("Project Browser");
      widget->show();
   }

   actionDebugging_Mode->setChecked(false);
   actionCoding_Mode->setChecked(true);
   appSettings->setAppMode(AppSettings::CodingMode);
}

void MainWindow::on_actionDebugging_Mode_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   if ( actionCoding_Mode->isChecked() )
   {
      QWidget* widget;
      widget = CDockWidgetRegistry::instance()->getWidget("Output");
      widget->hide();

      settings.setValue("CodingModeIDEGeometry",saveGeometry());

      CDockWidgetRegistry::instance()->hideAll();
      if ( EnvironmentSettingsDialog::rememberWindowSettings() )
      {
         hide();
         restoreGeometry(settings.value("DebuggingModeIDEGeometry").toByteArray());
         restoreState(settings.value("DebuggingModeIDEState").toByteArray());
         show();
      }
   }

   CDockWidgetRegistry::instance()->restoreVisibility();
   actionDebugging_Mode->setChecked(true);
   actionCoding_Mode->setChecked(false);

   // Always show emulator
   CDockWidgetRegistry::instance()->getWidget("Emulator")->show();
   appSettings->setAppMode(AppSettings::DebuggingMode);
}

void MainWindow::on_actionExit_triggered()
{
   // Closing the main window kills the app
   close();
}

void MainWindow::applyAddOns(QStringList addOns)
{
   m_pProjectBrowser->disableNavigation();

   foreach ( QString addon_uri, addOns )
   {
      explodeAddOn(0,nesicideProject->getProjectTitle(),addon_uri,QDir::currentPath());
   }

   emit applyProjectProperties();

   m_pProjectBrowser->enableNavigation();
   projectDataChangesEvent();
}

void MainWindow::on_actionManage_Add_Ons_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   NewProjectDialog dlg("Add-Ons",nesicideProject->getProjectTitle(),settings.value("LastProjectBasePath").toString(),true);

   if (dlg.exec() == QDialog::Accepted)
   {
      applyAddOns(dlg.getAddOns());
   }
}
