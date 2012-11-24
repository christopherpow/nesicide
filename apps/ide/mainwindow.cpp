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

#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>

//#include <QSystemStorageInfo>

OutputPaneDockWidget* output = NULL;
ProjectBrowserDockWidget* m_pProjectBrowser = NULL;

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent)
{
   if ( !((QCoreApplication::applicationDirPath().contains("Program Files")) ||
        (QCoreApplication::applicationDirPath().contains("apps/ide"))) ) // Developer builds
   {
      QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
      QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
   }
   if ( !QCoreApplication::applicationDirPath().contains("apps/ide") )
   {
      // Set environment.
      QString envvar = qgetenv("PATH");
      QString envdat;
      envdat = QCoreApplication::applicationDirPath();
      envdat += "/GnuWin32/bin;";
      envdat += QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot/bin;";
      qputenv("PATH",QString(envdat+envvar).toAscii());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot";
      qputenv("CC65_HOME",envdat.toAscii());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot/lib";
      qputenv("LD65_LIB",envdat.toAscii());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot/lib";
      qputenv("LD65_OBJ",envdat.toAscii());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot/asminc";
      qputenv("CC65_ASMINC",envdat.toAscii());

      envdat = QCoreApplication::applicationDirPath();
      envdat += "/cc65-snapshot/asminc";
      qputenv("CC65_INC",envdat.toAscii());

   }

   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

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
   CObjectRegistry::addObject ( "Searcher", searcher );

   // Create the breakpoint watcher thread...
   BreakpointWatcherThread* breakpointWatcher = new BreakpointWatcherThread();
   CObjectRegistry::addObject ( "Breakpoint Watcher", breakpointWatcher );

   // Create the compiler thread...
   CompilerThread* compiler = new CompilerThread();
   QObject::connect(this,SIGNAL(compile()),compiler,SLOT(compile()));
   QObject::connect(this,SIGNAL(clean()),compiler,SLOT(clean()));
   CObjectRegistry::addObject ( "Compiler", compiler );

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

   QObject::connect(menuEdit,SIGNAL(aboutToShow()),this,SLOT(menuEdit_aboutToShow()));

   menuWindow->setEnabled(false);

   // Start with no target loaded.
   m_targetLoaded = "none";

   // Set up common UI elements.
   m_pSourceNavigator = new SourceNavigator();
   compilerToolbar->addWidget(m_pSourceNavigator);
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pSourceNavigator,SLOT(updateTargetMachine(QString)));
   CDockWidgetRegistry::addWidget ( "Source Navigator", m_pSourceNavigator );

   m_pSearchBar = new SearchBar("SearchBar");
   m_pSearchBar->showCloseButton(true);
   centralWidget()->layout()->addWidget(m_pSearchBar);
   m_pSearchBar->hide();
   CDockWidgetRegistry::addWidget ( "Search Bar", m_pSearchBar );

   m_pSearch = new SearchDockWidget();
   addDockWidget(Qt::LeftDockWidgetArea, m_pSearch );
   m_pSearch->hide();
   QObject::connect(m_pSearch, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedSearch_close(bool)));
   CDockWidgetRegistry::addWidget ( "Search", m_pSearch );

   m_pProjectBrowser = new ProjectBrowserDockWidget(tabWidget);
   addDockWidget(Qt::LeftDockWidgetArea, m_pProjectBrowser );
   m_pProjectBrowser->hide();
   QObject::connect(m_pProjectBrowser, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedProjectBrowser_close(bool)));
   CDockWidgetRegistry::addWidget ( "Project", m_pProjectBrowser );

   output = new OutputPaneDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, output );
   output->hide();
   QObject::connect(output, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedOutput_Window_close(bool)));
   QObject::connect(generalTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateGeneralPane(QString)));
   QObject::connect(buildTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateBuildPane(QString)));
   QObject::connect(debugTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateDebugPane(QString)));
   QObject::connect(searchTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateSearchPane(QString)));
   QObject::connect(generalTextLogger,SIGNAL(eraseText()),output,SLOT(eraseGeneralPane()));
   QObject::connect(buildTextLogger,SIGNAL(eraseText()),output,SLOT(eraseBuildPane()));
   QObject::connect(debugTextLogger,SIGNAL(eraseText()),output,SLOT(eraseDebugPane()));
   QObject::connect(searchTextLogger,SIGNAL(eraseText()),output,SLOT(eraseSearchPane()));
   QObject::connect(breakpointWatcher,SIGNAL(showPane(int)),output,SLOT(showPane(int)));
   QObject::connect(m_pSearch, SIGNAL(showPane(int)), output, SLOT(showPane(int)));
   CDockWidgetRegistry::addWidget ( "Output", output );

   generalTextLogger->write("<strong>NESICIDE</strong> Alpha Release");
   generalTextLogger->write("<strong>Plugin Scripting Subsystem:</strong> " + pluginManager->getVersionInfo());

   m_pExecutionInspector = new ExecutionInspectorDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pExecutionInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionInspector );
   m_pExecutionInspector->hide();
   QObject::connect(m_pExecutionInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionInspector_close(bool)));
   QObject::connect(m_pExecutionInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Execution Inspector", m_pExecutionInspector );

   m_pSymbolInspector = new SymbolWatchDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pSymbolInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pSymbolInspector );
   m_pSymbolInspector->hide();
   QObject::connect(m_pSymbolInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedSymbol_Watch_close(bool)));
   QObject::connect(m_pSymbolInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Symbol Inspector", m_pSymbolInspector );

   m_pCodeProfiler = new CodeProfilerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pCodeProfiler,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::LeftDockWidgetArea, m_pCodeProfiler );
   m_pCodeProfiler->hide();
   QObject::connect(m_pCodeProfiler, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCode_Profiler_close(bool)));
   CDockWidgetRegistry::addWidget ( "Code Profiler", m_pCodeProfiler );

   emit updateTargetMachine("none");

   // Connect snapTo's from various debuggers to the central widget.
   QObject::connect ( m_pSourceNavigator, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pCodeProfiler, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pSymbolInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( output, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );

   // Slots for updating status bar.
   QObject::connect ( tabWidget, SIGNAL(addStatusBarWidget(QWidget*)), this, SLOT(addStatusBarWidget(QWidget*)));
   QObject::connect ( tabWidget, SIGNAL(removeStatusBarWidget(QWidget*)), this, SLOT(removeStatusBarWidget(QWidget*)));

   if ( EnvironmentSettingsDialog::showWelcomeOnStart() )
   {
      tabWidget->addTab(tab,"Welcome Page");
      webView->setUrl(QUrl( "http://www.nesicide.com"));
   }
   else
   {
      tabWidget->removeTab(tabWidget->indexOf(tab));
   }

   // Load all plugins.
   pluginManager->doInitScript();
   pluginManager->loadPlugins();

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
   QStringList argv_nes = argv.filter ( QRegExp(".*[.]nes$",Qt::CaseInsensitive) );
   QStringList argv_nesproject = argv.filter ( QRegExp(".*[.]nesproject$",Qt::CaseInsensitive) );
   QStringList argv_c64 = argv.filter ( QRegExp(".*[.](c64|prg|d64)$",Qt::CaseInsensitive) );

   if ( (argv_nes.count() >= 1) &&
        (argv_nesproject.count() >= 1) )
   {
      QMessageBox::information ( 0, "Command Line Error", "Cannot specify a .nes and a .nesproject file to open.\n" );
   }

   if ( argv_nes.count() >= 1 )
   {
      openNesROM(argv_nes.at(0));

      if ( argv_nes.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NES ROM files were specified on the command\n"
                                    "line.  Only the first NES ROM was opened, all others\n"
                                    "were ignored." );
      }
   }
   else if ( argv_nesproject.count() >= 1 )
   {
      openNesProject(argv_nesproject.at(0));

      if ( argv_nesproject.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NESICIDE project files were specified on the command\n"
                                    "line.  Only the first NESICIDE project was opened, all others\n"
                                    "were ignored." );
      }
   }
   else if ( argv_c64.count() >= 1 )
   {
      openC64File(argv_c64.at(0));

      if ( argv_c64.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many C64 files were specified on the command\n"
                                    "line.  Only the first C64 file was opened, all others\n"
                                    "were ignored." );
      }
   }

   projectDataChangesEvent();

   if ( EnvironmentSettingsDialog::rememberWindowSettings() )
   {
      restoreGeometry(settings.value("IDEGeometry").toByteArray());
      restoreState(settings.value("IDEState").toByteArray());
   }

   // Start timer for doing background stuff.
   m_periodicTimer = startTimer(5000);
}

MainWindow::~MainWindow()
{
   BreakpointWatcherThread* breakpointWatcher = dynamic_cast<BreakpointWatcherThread*>(CObjectRegistry::getObject("Breakpoint Watcher"));
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::getObject("Compiler"));
   SearcherThread* searcher = dynamic_cast<SearcherThread*>(CObjectRegistry::getObject("Searcher"));

   killTimer(m_periodicTimer);

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

   delete m_pBreakpointInspector;
   delete m_pExecutionInspector;
   delete m_pAssemblyInspector;
   delete m_pSourceNavigator;
   delete m_pSymbolInspector;
   delete m_pSearch;
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
               closeProject();
               openNesProject(fileInfo.fileName());
            }
         }
      }

      emit checkOpenFiles(m_lastActivationChangeTime);
   }
   else
   {
      if ( EmulatorPrefsDialog::getPauseOnTaskSwitch() )
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
   CCC65Interface::updateTargetMachine("nes");

   actionEmulation_Window = new QAction("Emulator",this);
   actionEmulation_Window->setObjectName(QString::fromUtf8("actionEmulation_Window"));
   actionEmulation_Window->setCheckable(true);
   QIcon icon8;
   icon8.addFile(QString::fromUtf8(":/resources/controller.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionEmulation_Window->setIcon(icon8);

   actionAssembly_Inspector = new QAction("Assembly Browser",this);
   actionAssembly_Inspector->setObjectName(QString::fromUtf8("actionAssembly_Inspector"));
   actionAssembly_Inspector->setCheckable(true);
   QIcon icon10;
   icon10.addFile(QString::fromUtf8(":/resources/22_code_inspector.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionAssembly_Inspector->setIcon(icon10);
   actionBreakpoint_Inspector = new QAction("Breakpoints",this);
   actionBreakpoint_Inspector->setObjectName(QString::fromUtf8("actionBreakpoint_Inspector"));
   actionBreakpoint_Inspector->setCheckable(true);
   QIcon icon11;
   icon11.addFile(QString::fromUtf8(":/resources/22_breakpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBreakpoint_Inspector->setIcon(icon11);
   actionGfxCHRMemory_Inspector = new QAction("CHR Memory Visualizer",this);
   actionGfxCHRMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxCHRMemory_Inspector"));
   actionGfxCHRMemory_Inspector->setCheckable(true);
   QIcon icon12;
   icon12.addFile(QString::fromUtf8(":/resources/22_chr_mem.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionGfxCHRMemory_Inspector->setIcon(icon12);
   actionGfxOAMMemory_Inspector = new QAction("OAM Memory Visualizer",this);
   actionGfxOAMMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxOAMMemory_Inspector"));
   actionGfxOAMMemory_Inspector->setCheckable(true);
   QIcon icon13;
   icon13.addFile(QString::fromUtf8(":/resources/22_preferences-desktop-display-color.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionGfxOAMMemory_Inspector->setIcon(icon13);
   actionGfxNameTableNESMemory_Inspector = new QAction("NameTable Visualizer",this);
   actionGfxNameTableNESMemory_Inspector->setObjectName(QString::fromUtf8("actionGfxNameTableNESMemory_Inspector"));
   actionGfxNameTableNESMemory_Inspector->setCheckable(true);
   actionBinCPURAM_Inspector = new QAction("CPU Memory",this);
   actionBinCPURAM_Inspector->setObjectName(QString::fromUtf8("actionBinCPURAM_Inspector"));
   QIcon icon16;
   icon16.addFile(QString::fromUtf8(":/resources/22_cpu_ram.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinCPURAM_Inspector->setIcon(icon16);
   actionBinCPURAM_Inspector->setCheckable(true);
   actionBinNameTableNESMemory_Inspector = new QAction("NameTable Memory",this);
   actionBinNameTableNESMemory_Inspector->setObjectName(QString::fromUtf8("actionBinNameTableNESMemory_Inspector"));
   actionBinNameTableNESMemory_Inspector->setIcon(icon13);
   actionBinNameTableNESMemory_Inspector->setCheckable(true);
   actionBinPPURegister_Inspector = new QAction("Registers",this);
   actionBinPPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinPPURegister_Inspector"));
   actionBinPPURegister_Inspector->setIcon(icon13);
   actionBinPPURegister_Inspector->setCheckable(true);
   actionBinAPURegister_Inspector = new QAction("Registers",this);
   actionBinAPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinAPURegister_Inspector"));
   actionBinAPURegister_Inspector->setIcon(icon13);
   actionBinAPURegister_Inspector->setCheckable(true);
   actionBinCHRMemory_Inspector = new QAction("CHR Memory",this);
   actionBinCHRMemory_Inspector->setObjectName(QString::fromUtf8("actionBinCHRMemory_Inspector"));
   actionBinCHRMemory_Inspector->setIcon(icon13);
   actionBinCHRMemory_Inspector->setCheckable(true);
   actionBinOAMMemory_Inspector = new QAction("OAM Memory",this);
   actionBinOAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinOAMMemory_Inspector"));
   actionBinOAMMemory_Inspector->setIcon(icon13);
   actionBinOAMMemory_Inspector->setCheckable(true);
   actionBinPaletteNESMemory_Inspector = new QAction("Palette Memory",this);
   actionBinPaletteNESMemory_Inspector->setObjectName(QString::fromUtf8("actionBinPaletteNESMemory_Inspector"));
   actionBinPaletteNESMemory_Inspector->setIcon(icon13);
   actionBinPaletteNESMemory_Inspector->setCheckable(true);
   actionBinSRAMMemory_Inspector = new QAction("SRAM Memory",this);
   actionBinSRAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinSRAMMemory_Inspector"));
   actionBinSRAMMemory_Inspector->setIcon(icon13);
   actionBinSRAMMemory_Inspector->setCheckable(true);
   actionBinEXRAMMemory_Inspector = new QAction("EXRAM Memory",this);
   actionBinEXRAMMemory_Inspector->setObjectName(QString::fromUtf8("actionBinEXRAMMemory_Inspector"));
   actionBinEXRAMMemory_Inspector->setIcon(icon13);
   actionBinEXRAMMemory_Inspector->setCheckable(true);
   actionBinCPURegister_Inspector = new QAction("Registers",this);
   actionBinCPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinCPURegister_Inspector"));
   QIcon icon17;
   icon17.addFile(QString::fromUtf8(":/resources/22_cpu_registers.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinCPURegister_Inspector->setIcon(icon17);
   actionBinCPURegister_Inspector->setCheckable(true);
   actionBinMapperMemory_Inspector = new QAction("Mapper Memory",this);
   actionBinMapperMemory_Inspector->setObjectName(QString::fromUtf8("actionBinMapperMemory_Inspector"));
   QIcon icon18;
   icon18.addFile(QString::fromUtf8(":/resources/22_mapper_memory.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinMapperMemory_Inspector->setIcon(icon18);
   actionBinMapperMemory_Inspector->setCheckable(true);
   actionBinROM_Inspector = new QAction("PRG-ROM Memory",this);
   actionBinROM_Inspector->setObjectName(QString::fromUtf8("actionBinROM_Inspector"));
   QIcon icon19;
   icon19.addFile(QString::fromUtf8(":/resources/22_bin_rom.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinROM_Inspector->setIcon(icon19);
   actionBinROM_Inspector->setCheckable(true);
   actionPPUInformation_Inspector = new QAction("Information",this);
   actionPPUInformation_Inspector->setObjectName(QString::fromUtf8("actionPPUInformation_Inspector"));
   actionPPUInformation_Inspector->setIcon(icon13);
   actionPPUInformation_Inspector->setCheckable(true);
   actionJoypadLogger_Inspector = new QAction("Joypad Logger",this);
   actionJoypadLogger_Inspector->setObjectName(QString::fromUtf8("actionJoypadLogger_Inspector"));
   actionJoypadLogger_Inspector->setCheckable(true);
   actionJoypadLogger_Inspector->setEnabled(false);
   actionCodeDataLogger_Inspector = new QAction("Code/Data Log Visualizer",this);
   actionCodeDataLogger_Inspector->setObjectName(QString::fromUtf8("actionCodeDataLogger_Inspector"));
   actionCodeDataLogger_Inspector->setCheckable(true);
   actionExecution_Visualizer_Inspector = new QAction("Execution Visualizer",this);
   actionExecution_Visualizer_Inspector->setObjectName(QString::fromUtf8("actionExecution_Visualizer_Inspector"));
   actionExecution_Visualizer_Inspector->setCheckable(true);
   actionMapperInformation_Inspector = new QAction("Information",this);
   actionMapperInformation_Inspector->setObjectName(QString::fromUtf8("actionMapperInformation_Inspector"));
   actionMapperInformation_Inspector->setIcon(icon13);
   actionMapperInformation_Inspector->setCheckable(true);
   actionAPUInformation_Inspector = new QAction("Information",this);
   actionAPUInformation_Inspector->setObjectName(QString::fromUtf8("actionAPUInformation_Inspector"));
   actionAPUInformation_Inspector->setIcon(icon13);
   actionAPUInformation_Inspector->setCheckable(true);
   actionPreferences = new QAction("Preferences...",this);
   actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
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
   menuCartridge_Inspectors->addSeparator();
   menuCartridge_Inspectors->addAction(actionGfxCHRMemory_Inspector);
   menuEmulator->addAction(menuSystem->menuAction());
   menuEmulator->addAction(menuVideo->menuAction());
   menuEmulator->addAction(menuAudio->menuAction());
   menuEmulator->addSeparator();
   menuEmulator->addAction(actionRun_Test_Suite);
   menuEmulator->addSeparator();
   menuEmulator->addAction(actionPreferences);
   menuSystem->addAction(actionNTSC);
   menuSystem->addAction(actionPAL);
   menuSystem->addAction(actionDendy);
   menuAudio->addAction(actionSquare_1);
   menuAudio->addAction(actionSquare_2);
   menuAudio->addAction(actionTriangle);
   menuAudio->addAction(actionNoise);
   menuAudio->addAction(actionDelta_Modulation);
   menuView->addSeparator();
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
   toolToolbar->addSeparator();

   m_pNESEmulatorThread = new NESEmulatorThread();
   CObjectRegistry::addObject("Emulator",m_pNESEmulatorThread);

   QObject::connect(this,SIGNAL(startEmulation()),m_pNESEmulatorThread,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),m_pNESEmulatorThread,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(primeEmulator()),m_pNESEmulatorThread,SLOT(primeEmulator()));
   QObject::connect(this,SIGNAL(resetEmulator()),m_pNESEmulatorThread,SLOT(resetEmulator()));
   QObject::connect(this,SIGNAL(adjustAudio(int32_t)),m_pNESEmulatorThread,SLOT(adjustAudio(int32_t)));

   m_pNESEmulator = new NESEmulatorDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pNESEmulator,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pNESEmulator );
   m_pNESEmulator->hide();
   QObject::connect(m_pNESEmulator, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedEmulator_close(bool)));
   CDockWidgetRegistry::addWidget ( "Emulator", m_pNESEmulator );

   m_pNESEmulatorControl = new NESEmulatorControl();
   debuggerToolBar->addWidget(m_pNESEmulatorControl);
   debuggerToolBar->show();
   QObject::connect(m_pNESEmulatorControl,SIGNAL(focusEmulator()),this,SLOT(focusEmulator()));

   // Add menu for emulator control.  The emulator control provides menu for itself!  =]
   QAction* firstEmuMenuAction = menuEmulator->actions().at(0);
   menuEmulator->insertActions(firstEmuMenuAction,m_pNESEmulatorControl->menu());
   menuEmulator->insertSeparator(firstEmuMenuAction);

   m_pBreakpointInspector = new BreakpointDockWidget(nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBreakpointInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pBreakpointInspector );
   m_pBreakpointInspector->hide();
   QObject::connect(m_pBreakpointInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBreakpointInspector_close(bool)));
   QObject::connect(m_pBreakpointInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Breakpoints", m_pBreakpointInspector );

   m_pAssemblyInspector = new CodeBrowserDockWidget(nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAssemblyInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pAssemblyInspector );
   m_pAssemblyInspector->hide();
   QObject::connect(m_pAssemblyInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedAssemblyInspector_close(bool)));
   QObject::connect(m_pAssemblyInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Assembly Browser", m_pAssemblyInspector );

   m_pGfxCHRMemoryInspector = new CHRMEMInspector ();
   m_pGfxCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
   m_pGfxCHRMemoryInspector->setWindowTitle("CHR Memory Visualizer");
   m_pGfxCHRMemoryInspector->setObjectName("chrMemoryVisualizer");
   m_pGfxCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxCHRMemoryInspector );
   m_pGfxCHRMemoryInspector->hide();
   QObject::connect(m_pGfxCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxCHRMemoryInspector_close(bool)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(setStatusBarMessage(QString)),this,SLOT(setStatusBarMessage(QString)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(addStatusBarWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
   QObject::connect(m_pGfxCHRMemoryInspector,SIGNAL(removeStatusBarWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
   CDockWidgetRegistry::addWidget ( "CHR Memory Visualizer", m_pGfxCHRMemoryInspector );

   m_pGfxOAMMemoryInspector = new OAMVisualizerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pGfxOAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxOAMMemoryInspector );
   m_pGfxOAMMemoryInspector->hide();
   QObject::connect(m_pGfxOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxOAMMemoryInspector_close(bool)));
   QObject::connect(m_pGfxOAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "OAM Memory Visualizer", m_pGfxOAMMemoryInspector );

   m_pGfxNameTableMemoryInspector = new NameTableVisualizerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pGfxNameTableMemoryInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pGfxNameTableMemoryInspector );
   m_pGfxNameTableMemoryInspector->hide();
   QObject::connect(m_pGfxNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxNameTableMemoryInspector_close(bool)));
   QObject::connect(m_pGfxNameTableMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Name Table Visualizer", m_pGfxNameTableMemoryInspector );

   m_pJoypadLoggerInspector = new JoypadLoggerDockWidget ();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pJoypadLoggerInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pJoypadLoggerInspector );
   m_pJoypadLoggerInspector->hide();
   QObject::connect(m_pJoypadLoggerInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedJoypadLoggerInspector_close(bool)));
   QObject::connect(m_pJoypadLoggerInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Joypad Logger", m_pJoypadLoggerInspector );

   m_pExecutionVisualizer = new ExecutionVisualizerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pExecutionVisualizer,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionVisualizer );
   m_pExecutionVisualizer->hide();
   QObject::connect(m_pExecutionVisualizer, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionVisualizer_Inspector_close(bool)));
   QObject::connect(m_pExecutionVisualizer,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Execution Visualizer", m_pExecutionVisualizer );

   m_pCodeDataLoggerInspector = new CodeDataLoggerDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pCodeDataLoggerInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pCodeDataLoggerInspector );
   m_pCodeDataLoggerInspector->hide();
   QObject::connect(m_pCodeDataLoggerInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCodeDataLoggerInspector_close(bool)));
   QObject::connect(m_pCodeDataLoggerInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Code/Data Logger Inspector", m_pCodeDataLoggerInspector );

   m_pBinCPURegisterInspector = new RegisterInspectorDockWidget(nesGetCpuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURegisterInspector->setObjectName("cpuRegisterInspector");
   m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
   m_pBinCPURegisterInspector->hide();
   QObject::connect(m_pBinCPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURegisterInspector_close(bool)));
   QObject::connect(m_pBinCPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "CPU Register Inspector", m_pBinCPURegisterInspector );

   m_pBinCPURAMInspector = new MemoryInspectorDockWidget(nesGetCpuMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURAMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURAMInspector->setObjectName("cpuMemoryInspector");
   m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
   m_pBinCPURAMInspector->hide();
   QObject::connect(m_pBinCPURAMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURAMInspector_close(bool)));
   QObject::connect(m_pBinCPURAMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "CPU RAM Inspector", m_pBinCPURAMInspector );

   m_pBinROMInspector = new MemoryInspectorDockWidget(nesGetCartridgePRGROMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinROMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinROMInspector->setObjectName("cartPRGROMMemoryInspector");
   m_pBinROMInspector->setWindowTitle("PRG-ROM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinROMInspector );
   m_pBinROMInspector->hide();
   QObject::connect(m_pBinROMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinROMInspector_close(bool)));
   QObject::connect(m_pBinROMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "PRG-ROM Inspector", m_pBinROMInspector );

   m_pBinNameTableMemoryInspector = new MemoryInspectorDockWidget(nesGetPpuNameTableMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinNameTableMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinNameTableMemoryInspector->setObjectName("ppuNameTableMemoryInspector");
   m_pBinNameTableMemoryInspector->setWindowTitle("NameTable Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinNameTableMemoryInspector );
   m_pBinNameTableMemoryInspector->hide();
   QObject::connect(m_pBinNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinNameTableMemoryInspector_close(bool)));
   QObject::connect(m_pBinNameTableMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "NameTable Inspector", m_pBinNameTableMemoryInspector );

   m_pBinPPURegisterInspector = new RegisterInspectorDockWidget(nesGetPpuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinPPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinPPURegisterInspector->setObjectName("ppuRegisterInspector");
   m_pBinPPURegisterInspector->setWindowTitle("PPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPURegisterInspector );
   m_pBinPPURegisterInspector->hide();
   QObject::connect(m_pBinPPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPPURegisterInspector_close(bool)));
   QObject::connect(m_pBinPPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "PPU Register Inspector", m_pBinPPURegisterInspector );

   m_pPPUInformationInspector = new PPUInformationDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pPPUInformationInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pPPUInformationInspector );
   m_pPPUInformationInspector->hide();
   QObject::connect(m_pPPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedPPUInformationInspector_close(bool)));
   QObject::connect(m_pPPUInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "PPU Information", m_pPPUInformationInspector );

   m_pBinAPURegisterInspector = new RegisterInspectorDockWidget(nesGetApuRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinAPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinAPURegisterInspector->setObjectName("apuRegisterInspector");
   m_pBinAPURegisterInspector->setWindowTitle("APU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinAPURegisterInspector );
   m_pBinAPURegisterInspector->hide();
   QObject::connect(m_pBinAPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinAPURegisterInspector_close(bool)));
   QObject::connect(m_pBinAPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "APU Register Inspector", m_pBinAPURegisterInspector );

   m_pAPUInformationInspector = new APUInformationDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pAPUInformationInspector );
   m_pAPUInformationInspector->hide();
   QObject::connect(m_pAPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedAPUInformationInspector_close(bool)));
   QObject::connect(m_pAPUInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAPUInformationInspector,SLOT(updateTargetMachine(QString)));
   CDockWidgetRegistry::addWidget ( "APU Information", m_pAPUInformationInspector );

   m_pBinCHRMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeCHRMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCHRMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCHRMemoryInspector->setObjectName("chrMemoryInspector");
   m_pBinCHRMemoryInspector->setWindowTitle("CHR Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCHRMemoryInspector );
   m_pBinCHRMemoryInspector->hide();
   QObject::connect(m_pBinCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCHRMemoryInspector_close(bool)));
   QObject::connect(m_pBinCHRMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "CHR Memory Inspector", m_pBinCHRMemoryInspector );

   m_pBinOAMMemoryInspector = new RegisterInspectorDockWidget(nesGetPpuOamRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinOAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinOAMMemoryInspector->setObjectName("oamMemoryInspector");
   m_pBinOAMMemoryInspector->setWindowTitle("OAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinOAMMemoryInspector );
   m_pBinOAMMemoryInspector->hide();
   QObject::connect(m_pBinOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinOAMMemoryInspector_close(bool)));
   QObject::connect(m_pBinOAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "OAM Memory Inspector", m_pBinOAMMemoryInspector );

   m_pBinPaletteMemoryInspector = new MemoryInspectorDockWidget(nesGetPpuPaletteMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinPaletteMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinPaletteMemoryInspector->setObjectName("ppuPaletteMemoryInspector");
   m_pBinPaletteMemoryInspector->setWindowTitle("Palette Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPaletteMemoryInspector );
   m_pBinPaletteMemoryInspector->hide();
   QObject::connect(m_pBinPaletteMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPaletteMemoryInspector_close(bool)));
   QObject::connect(m_pBinPaletteMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Palette Memory Inspector", m_pBinPaletteMemoryInspector );

   m_pBinSRAMMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeSRAMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinSRAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinSRAMMemoryInspector->setObjectName("cartSRAMMemoryInspector");
   m_pBinSRAMMemoryInspector->setWindowTitle("Cartridge SRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinSRAMMemoryInspector );
   m_pBinSRAMMemoryInspector->hide();
   QObject::connect(m_pBinSRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinSRAMMemoryInspector_close(bool)));
   QObject::connect(m_pBinSRAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Cartridge SRAM Memory Inspector", m_pBinSRAMMemoryInspector );

   m_pBinEXRAMMemoryInspector = new MemoryInspectorDockWidget(nesGetCartridgeEXRAMMemoryDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinEXRAMMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinEXRAMMemoryInspector->setObjectName("cartEXRAMMemoryInspector");
   m_pBinEXRAMMemoryInspector->setWindowTitle("Cartridge EXRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinEXRAMMemoryInspector );
   m_pBinEXRAMMemoryInspector->hide();
   QObject::connect(m_pBinEXRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinEXRAMMemoryInspector_close(bool)));
   QObject::connect(m_pBinEXRAMMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Cartridge EXRAM Memory Inspector", m_pBinEXRAMMemoryInspector );

   m_pMapperInformationInspector = new MapperInformationDockWidget();
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pMapperInformationInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::BottomDockWidgetArea, m_pMapperInformationInspector );
   m_pMapperInformationInspector->hide();
   QObject::connect(m_pMapperInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedMapperInformationInspector_close(bool)));
   QObject::connect(m_pMapperInformationInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Cartridge Mapper Information", m_pMapperInformationInspector );

   m_pBinMapperMemoryInspector = new RegisterInspectorDockWidget(nesGetCartridgeRegisterDatabase,nesGetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinMapperMemoryInspector,SLOT(updateTargetMachine(QString)));
   m_pBinMapperMemoryInspector->setObjectName("cartMapperRegisterInspector");
   m_pBinMapperMemoryInspector->setWindowTitle("Cartridge Mapper Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinMapperMemoryInspector );
   m_pBinMapperMemoryInspector->hide();
   QObject::connect(m_pBinMapperMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinMapperMemoryInspector_close(bool)));
   QObject::connect(m_pBinMapperMemoryInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Cartridge Mapper Register Inspector", m_pBinMapperMemoryInspector );

   // Connect slots for new UI elements.
   QObject::connect(action1x,SIGNAL(triggered()),this,SLOT(action1x_triggered()));
   QObject::connect(action1_5x,SIGNAL(triggered()),this,SLOT(action1_5x_triggered()));
   QObject::connect(action2x,SIGNAL(triggered()),this,SLOT(action2x_triggered()));
   QObject::connect(action2_5x,SIGNAL(triggered()),this,SLOT(action2_5x_triggered()));
   QObject::connect(action3x,SIGNAL(triggered()),this,SLOT(action3x_triggered()));
   QObject::connect(actionLinear_Interpolation,SIGNAL(toggled(bool)),this,SLOT(actionLinear_Interpolation_toggled(bool)));
   QObject::connect(action4_3_Aspect,SIGNAL(toggled(bool)),this,SLOT(action4_3_Aspect_toggled(bool)));
   QObject::connect(actionAssembly_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionAssembly_Inspector_toggled(bool)));
   QObject::connect(actionBreakpoint_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBreakpoint_Inspector_toggled(bool)));
   QObject::connect(actionEmulation_Window,SIGNAL(toggled(bool)),this,SLOT(actionEmulation_Window_toggled(bool)));
   QObject::connect(actionFullscreen,SIGNAL(toggled(bool)),this,SLOT(actionFullscreen_toggled(bool)));
   QObject::connect(actionRun_Test_Suite,SIGNAL(triggered()),this,SLOT(actionRun_Test_Suite_triggered()));
   QObject::connect(actionSquare_1,SIGNAL(toggled(bool)),this,SLOT(actionSquare_1_toggled(bool)));
   QObject::connect(actionSquare_2,SIGNAL(toggled(bool)),this,SLOT(actionSquare_2_toggled(bool)));
   QObject::connect(actionTriangle,SIGNAL(toggled(bool)),this,SLOT(actionTriangle_toggled(bool)));
   QObject::connect(actionNoise,SIGNAL(toggled(bool)),this,SLOT(actionNoise_toggled(bool)));
   QObject::connect(actionDelta_Modulation,SIGNAL(toggled(bool)),this,SLOT(actionDelta_Modulation_toggled(bool)));
   QObject::connect(actionPAL,SIGNAL(triggered()),this,SLOT(actionPAL_triggered()));
   QObject::connect(actionNTSC,SIGNAL(triggered()),this,SLOT(actionNTSC_triggered()));
   QObject::connect(actionDendy,SIGNAL(triggered()),this,SLOT(actionDendy_triggered()));
   QObject::connect(actionPreferences,SIGNAL(triggered()),this,SLOT(actionPreferences_triggered()));
   QObject::connect(actionCodeDataLogger_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionCodeDataLogger_Inspector_toggled(bool)));
   QObject::connect(actionExecution_Visualizer_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionExecution_Visualizer_Inspector_toggled(bool)));
   QObject::connect(actionGfxCHRMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionGfxCHRMemory_Inspector_toggled(bool)));
   QObject::connect(actionGfxOAMMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionGfxOAMMemory_Inspector_toggled(bool)));
   QObject::connect(actionGfxNameTableNESMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionGfxNameTableNESMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinCPURegister_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinCPURegister_Inspector_toggled(bool)));
   QObject::connect(actionBinCPURAM_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinCPURAM_Inspector_toggled(bool)));
   QObject::connect(actionBinROM_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinROM_Inspector_toggled(bool)));
   QObject::connect(actionBinNameTableNESMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinNameTableNESMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinCHRMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinCHRMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinOAMMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinOAMMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinSRAMMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinSRAMMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinEXRAMMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinEXRAMMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinPaletteNESMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinPaletteNESMemory_Inspector_toggled(bool)));
   QObject::connect(actionBinAPURegister_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinAPURegister_Inspector_toggled(bool)));
   QObject::connect(actionBinPPURegister_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinPPURegister_Inspector_toggled(bool)));
   QObject::connect(actionBinMapperMemory_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinMapperMemory_Inspector_toggled(bool)));
   QObject::connect(actionPPUInformation_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionPPUInformation_Inspector_toggled(bool)));
   QObject::connect(actionAPUInformation_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionAPUInformation_Inspector_toggled(bool)));
   QObject::connect(actionMapperInformation_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionMapperInformation_Inspector_toggled(bool)));
   QObject::connect(actionJoypadLogger_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionJoypadLogger_Inspector_toggled(bool)));

   // Connect snapTo's from various debuggers.
   QObject::connect ( m_pExecutionVisualizer, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
   QObject::connect ( m_pBreakpointInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );

   updateFromEmulatorPrefs(true);

   m_targetLoaded = "nes";

   emit updateTargetMachine(m_targetLoaded);
}

void MainWindow::destroyNesUi()
{
   // If we're set up for NES, clear it.
   if ( m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
   {
      return;
   }

   CDockWidgetRegistry::removeWidget ( "Assembly Browser" );
   CDockWidgetRegistry::removeWidget ( "Breakpoints" );
   CDockWidgetRegistry::removeWidget ( "Emulator" );
   CDockWidgetRegistry::removeWidget ( "CHR Memory Visualizer" );
   CDockWidgetRegistry::removeWidget ( "OAM Memory Visualizer" );
   CDockWidgetRegistry::removeWidget ( "Name Table Visualizer" );
   CDockWidgetRegistry::removeWidget ( "Execution Visualizer" );
   CDockWidgetRegistry::removeWidget ( "Code/Data Logger Inspector" );
   CDockWidgetRegistry::removeWidget ( "CPU Register Inspector" );
   CDockWidgetRegistry::removeWidget ( "CPU RAM Inspector" );
   CDockWidgetRegistry::removeWidget ( "PRG-ROM Inspector" );
   CDockWidgetRegistry::removeWidget ( "NameTable Inspector" );
   CDockWidgetRegistry::removeWidget ( "PPU Register Inspector" );
   CDockWidgetRegistry::removeWidget ( "PPU Information" );
   CDockWidgetRegistry::removeWidget ( "APU Register Inspector" );
   CDockWidgetRegistry::removeWidget ( "APU Information" );
   CDockWidgetRegistry::removeWidget ( "CHR Memory Inspector" );
   CDockWidgetRegistry::removeWidget ( "OAM Memory Inspector" );
   CDockWidgetRegistry::removeWidget ( "Palette Memory Inspector" );
   CDockWidgetRegistry::removeWidget ( "Cartridge SRAM Memory Inspector" );
   CDockWidgetRegistry::removeWidget ( "Cartridge EXRAM Memory Inspector" );
   CDockWidgetRegistry::removeWidget ( "Cartridge Mapper Information" );
   CDockWidgetRegistry::removeWidget ( "Cartridge Mapper Register Inspector" );
   CDockWidgetRegistry::removeWidget ( "Joypad Logger" );

   // Properly kill and destroy the thread we created above.
   m_pNESEmulatorThread->kill();
   m_pNESEmulatorThread->wait();

   delete m_pNESEmulatorThread;
   m_pNESEmulatorThread = NULL;

   CObjectRegistry::removeObject ( "Emulator" );

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
   delete actionPPUInformation_Inspector;
   delete actionJoypadLogger_Inspector;
   delete actionCodeDataLogger_Inspector;
   delete actionExecution_Visualizer_Inspector;
   delete actionMapperInformation_Inspector;
   delete actionAPUInformation_Inspector;
   delete actionPreferences;
   delete actionNTSC;
   delete actionPAL;
   delete actionDendy;
   delete actionSquare_1;
   delete actionSquare_2;
   delete actionTriangle;
   delete actionNoise;
   delete actionDelta_Modulation;
   delete actionRun_Test_Suite;
   delete menuCPU_Inspectors;
   delete menuAPU_Inpsectors;
   delete menuPPU_Inspectors;
   delete menuI_O_Inspectors;
   delete menuCartridge_Inspectors;
   delete menuSystem;
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
   CCC65Interface::updateTargetMachine("c64");

   actionAssembly_Inspector = new QAction("Assembly Browser",this);
   actionAssembly_Inspector->setObjectName(QString::fromUtf8("actionAssembly_Inspector"));
   actionAssembly_Inspector->setCheckable(true);
   QIcon icon12;
   icon12.addFile(QString::fromUtf8(":/resources/22_code_inspector.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionAssembly_Inspector->setIcon(icon12);
   actionBreakpoint_Inspector = new QAction("Breakpoints",this);
   actionBreakpoint_Inspector->setObjectName(QString::fromUtf8("actionBreakpoint_Inspector"));
   actionBreakpoint_Inspector->setCheckable(true);
   QIcon icon11;
   icon11.addFile(QString::fromUtf8(":/resources/22_breakpoint.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBreakpoint_Inspector->setIcon(icon11);
   actionBinCPURAM_Inspector = new QAction("CPU Memory",this);
   actionBinCPURAM_Inspector->setObjectName(QString::fromUtf8("actionBinCPURAM_Inspector"));
   QIcon icon16;
   icon16.addFile(QString::fromUtf8(":/resources/22_cpu_ram.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinCPURAM_Inspector->setIcon(icon16);
   actionBinCPURAM_Inspector->setCheckable(true);
   actionBinCPURegister_Inspector = new QAction("Registers",this);
   actionBinCPURegister_Inspector->setObjectName(QString::fromUtf8("actionBinCPURegister_Inspector"));
   QIcon icon17;
   icon17.addFile(QString::fromUtf8(":/resources/22_cpu_registers.png"), QSize(), QIcon::Normal, QIcon::Off);
   actionBinCPURegister_Inspector->setIcon(icon17);
   actionBinCPURegister_Inspector->setCheckable(true);
   actionBinSIDRegister_Inspector = new QAction("Registers",this);
   actionBinSIDRegister_Inspector->setObjectName(QString::fromUtf8("actionBinSIDRegister_Inspector"));
   actionBinSIDRegister_Inspector->setIcon(icon17);
   actionBinSIDRegister_Inspector->setCheckable(true);
   actionPreferences = new QAction("Preferences...",this);
   actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));

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

   menuEmulator->addAction(actionPreferences);

   debuggerToolBar = new QToolBar("Emulator Control",this);
   debuggerToolBar->setObjectName(QString::fromUtf8("debuggerToolBar"));
   QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(0);
   sizePolicy1.setHeightForWidth(debuggerToolBar->sizePolicy().hasHeightForWidth());
   debuggerToolBar->setSizePolicy(sizePolicy1);
   addToolBar(Qt::TopToolBarArea, debuggerToolBar);

   m_pC64EmulatorThread = new C64EmulatorThread();
   CObjectRegistry::addObject("Emulator",m_pC64EmulatorThread);
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
   QObject::connect(m_pBreakpointInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBreakpointInspector_close(bool)));
   QObject::connect(m_pBreakpointInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Breakpoints", m_pBreakpointInspector );

   m_pAssemblyInspector = new CodeBrowserDockWidget(c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pAssemblyInspector,SLOT(updateTargetMachine(QString)));
   addDockWidget(Qt::RightDockWidgetArea, m_pAssemblyInspector );
   m_pAssemblyInspector->hide();
   QObject::connect(m_pAssemblyInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedAssemblyInspector_close(bool)));
   QObject::connect(m_pAssemblyInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "Assembly Browser", m_pAssemblyInspector );

   m_pBinCPURegisterInspector = new RegisterInspectorDockWidget(c64GetCpuRegisterDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURegisterInspector->setObjectName("cpuRegisterInspector");
   m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
   m_pBinCPURegisterInspector->hide();
   QObject::connect(m_pBinCPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURegisterInspector_close(bool)));
   QObject::connect(m_pBinCPURegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "CPU Register Inspector", m_pBinCPURegisterInspector );

   m_pBinCPURAMInspector = new MemoryInspectorDockWidget(c64GetCpuMemoryDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinCPURAMInspector,SLOT(updateTargetMachine(QString)));
   m_pBinCPURAMInspector->setObjectName("cpuMemoryInspector");
   m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
   m_pBinCPURAMInspector->hide();
   QObject::connect(m_pBinCPURAMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURAMInspector_close(bool)));
   QObject::connect(m_pBinCPURAMInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "CPU RAM Inspector", m_pBinCPURAMInspector );

   m_pBinSIDRegisterInspector = new RegisterInspectorDockWidget(c64GetSidRegisterDatabase,c64GetBreakpointDatabase());
   QObject::connect(this,SIGNAL(updateTargetMachine(QString)),m_pBinSIDRegisterInspector,SLOT(updateTargetMachine(QString)));
   m_pBinSIDRegisterInspector->setObjectName("sidRegisterInspector");
   m_pBinSIDRegisterInspector->setWindowTitle("SID Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinSIDRegisterInspector );
   m_pBinSIDRegisterInspector->hide();
   QObject::connect(m_pBinSIDRegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinSIDRegisterInspector_close(bool)));
   QObject::connect(m_pBinSIDRegisterInspector,SIGNAL(markProjectDirty(bool)),this,SLOT(markProjectDirty(bool)));
   CDockWidgetRegistry::addWidget ( "SID Register Inspector", m_pBinSIDRegisterInspector );

   m_targetLoaded = "c64";

   emit updateTargetMachine(m_targetLoaded);

   // Connect slots for new UI elements.
   QObject::connect(actionBinCPURegister_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinCPURegister_Inspector_toggled(bool)));
   QObject::connect(actionBinCPURAM_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinCPURAM_Inspector_toggled(bool)));
   QObject::connect(actionBinSIDRegister_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBinSIDRegister_Inspector_toggled(bool)));
   QObject::connect(actionAssembly_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionAssembly_Inspector_toggled(bool)));
   QObject::connect(actionBreakpoint_Inspector,SIGNAL(toggled(bool)),this,SLOT(actionBreakpoint_Inspector_toggled(bool)));

   // Connect snapTo's from various debuggers.
   QObject::connect ( m_pBreakpointInspector, SIGNAL(snapTo(QString)), tabWidget, SLOT(snapToTab(QString)) );
}

void MainWindow::destroyC64Ui()
{
   // If we're set up for C64, clear it.
   if ( m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
   {
      return;
   }

   CDockWidgetRegistry::removeWidget ( "Assembly Browser" );
   CDockWidgetRegistry::removeWidget ( "Breakpoints" );
   CDockWidgetRegistry::removeWidget ( "CPU Register Inspector" );
   CDockWidgetRegistry::removeWidget ( "CPU RAM Inspector" );
   CDockWidgetRegistry::removeWidget ( "SID Register Inspector" );

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
   delete actionPreferences;
   delete menuCPU_Inspectors;
   delete menuSID_Inspectors;
   delete debuggerToolBar;

   // Properly kill and destroy the thread we created above.
   m_pC64EmulatorThread->kill();
   m_pC64EmulatorThread->wait();

   delete m_pC64EmulatorThread;
   m_pC64EmulatorThread = NULL;

   CObjectRegistry::removeObject ( "Emulator" );

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

void MainWindow::dropEvent(QDropEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;
   QFileInfo   fileInfo;
   QStringList extensions;

   if ( event->mimeData()->hasUrls() )
   {
      output->showPane(OutputPaneDockWidget::Output_General);

      fileUrls = event->mimeData()->urls();

      foreach ( QUrl fileUrl, fileUrls )
      {
         fileName = fileUrl.toLocalFile();

         fileInfo.setFile(fileName);

         if ( !fileInfo.suffix().compare("nesproject",Qt::CaseInsensitive) )
         {
            if ( nesicideProject->isInitialized() )
            {
               closeProject();
            }
            openNesProject(fileName);

            event->acceptProposedAction();
         }
         else if ( !fileInfo.suffix().compare("nes",Qt::CaseInsensitive) )
         {
            if ( nesicideProject->isInitialized() )
            {
               closeProject();
            }
            openNesROM(fileName);

            event->acceptProposedAction();
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

            event->acceptProposedAction();
         }
         else
         {
            openFile(fileName);

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

void MainWindow::hideEvent(QHideEvent *event)
{
   CDockWidgetRegistry::saveVisibility();
}

void MainWindow::showEvent(QShowEvent *event)
{
   CDockWidgetRegistry::restoreVisibility();
}

void MainWindow::projectDataChangesEvent()
{
   m_pProjectBrowser->layoutChangedEvent();
   m_pProjectBrowser->setVisible(nesicideProject->isInitialized());
   output->setVisible(nesicideProject->isInitialized());

   // Enabled/Disable actions based on if we have a project loaded or not
   actionNew_Project->setEnabled(!nesicideProject->isInitialized());
   actionCreate_Project_from_File->setEnabled(!nesicideProject->isInitialized());
   actionOpen_Project->setEnabled(!nesicideProject->isInitialized());
   actionProject_Properties->setEnabled(nesicideProject->isInitialized());
   action_Project_Browser->setEnabled(nesicideProject->isInitialized());
   action_Close_Project->setEnabled(nesicideProject->isInitialized());
   actionCompile_Project->setEnabled(nesicideProject->isInitialized());
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
   appStatusBar->addWidget(widget,100);
   widget->show();
}

void MainWindow::removeStatusBarWidget(QWidget *widget)
{
   // For some reason on creation the widget isn't there but it's being removed?
   appStatusBar->addWidget(widget,100);
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
      fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath()+QDir::separator()+nesicideProject->getProjectOutputName()+".nesproject",
                                                     "NESICIDE Project (*.nesproject)");
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
   QString fileName = QFileDialog::getSaveFileName(this, "Save Project", QDir::currentPath(),
                                                  "NESICIDE Project (*.nesproject)");

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

void MainWindow::explodeTemplate(QString templateDirName,QString localDirName,QString* projectFileName)
{
   QDir templateDir(templateDirName);
   QDir localDir;
   QString localDirTemp;
   QFileInfoList templateFileInfos = templateDir.entryInfoList();

   foreach ( QFileInfo fileInfo, templateFileInfos )
   {
      if ( fileInfo.isDir() )
      {
         localDirTemp = localDirName;
         localDirName += fileInfo.fileName();
         localDirName += "/";
         localDir.mkpath(localDirName);
         explodeTemplate(fileInfo.filePath(),localDirName,projectFileName);
         localDirName = localDirTemp;
      }
      else
      {
         // Save the file locally.
         QFile templateFile(fileInfo.filePath());
         QFile localFile(localDirName+fileInfo.fileName());

         if ( templateFile.open(QIODevice::ReadOnly) &&
              localFile.open(QIODevice::ReadWrite|QIODevice::Truncate) )
         {
            localFile.write(templateFile.readAll());
         }

         // If this is the project file, spit out the name.
         if ( !fileInfo.suffix().compare("nesproject",Qt::CaseInsensitive) )
         {
            (*projectFileName) = localFile.fileName();
         }

         templateFile.close();
         localFile.close();
      }
   }
}

void MainWindow::on_actionNew_Project_triggered()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   NewProjectDialog dlg(this,"New Project","Untitled",settings.value("LastProjectBasePath").toString(),true);

   if (dlg.exec() == QDialog::Accepted)
   {
      m_pProjectBrowser->disableNavigation();

      QDir::setCurrent(dlg.getPath());

      if ( dlg.getTemplateIndex() == 0 )
      {
         // Set project target before initializing project...
         if ( dlg.getTarget() == "Commodore 64" )
         {
            nesicideProject->setProjectTarget("c64");
            createC64Ui();
         }
         else if ( dlg.getTarget() == "Nintendo Entertainment System" )
         {
            nesicideProject->setProjectTarget("nes");
            createNesUi();
         }
         nesicideProject->initializeProject();
         nesicideProject->setDirty(true);
         nesicideProject->setProjectTitle(dlg.getName());
      }
      else
      {
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
            explodeTemplate(templateDirName,"",&projectFileName);
         }
         else if ( dlg.getTarget() == "Nintendo Entertainment System" )
         {
            templateDirName += "NES/";
            templateDirName += dlg.getTemplate();
            templateDirName += "/";

            // Recursively copy the project content to the local location.
            explodeTemplate(templateDirName,"",&projectFileName);

            openNesProject(projectFileName);
         }
      }

      m_pProjectBrowser->enableNavigation();
      projectDataChangesEvent();
   }
}

void MainWindow::openNesROM(QString fileName,bool runRom)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   createNesUi();

   output->showPane(OutputPaneDockWidget::Output_General);

   // Remove any lingering project content
   m_pProjectBrowser->disableNavigation();
   nesicideProject->terminateProject();

   // Clear output
   output->clearAllPanes();
   output->show();

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
   CCC65Interface::captureDebugInfo();

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

   actionEmulation_Window->setChecked(true);
   actionEmulation_Window_toggled(true);

   settings.setValue("LastProject",fileName);
}

void MainWindow::openC64File(QString fileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   createC64Ui();

   output->showPane(OutputPaneDockWidget::Output_General);

   // Remove any lingering project content
   m_pProjectBrowser->disableNavigation();
   nesicideProject->terminateProject();

   // Clear output
   output->clearAllPanes();
   output->show();

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
      CCC65Interface::captureDebugInfo();
   }
   else if ( !fileInfo.suffix().compare("d64",Qt::CaseInsensitive) )
   {
      nesicideProject->setProjectOutputName(fileInfo.completeBaseName()+".d64");
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
   QString romPath = EnvironmentSettingsDialog::romPath();
   QString selectedFilter;
   QString fileName = QFileDialog::getOpenFileName(this, "Open ROM", romPath, "All Files (*.*);;iNES ROM (*.nes);;Commodore 64 Program (*.c64 *.prg);;Commodore 64 Disk Image (*.d64)",&selectedFilter);

   if (fileName.isEmpty())
   {
      return;
   }

   if ( selectedFilter.contains("*.nes") )
   {
      openNesROM(fileName);
   }
   else if ( selectedFilter.contains("*.d64") || selectedFilter.contains("*.prg") || selectedFilter.contains("*.c64") )
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
   QList<QAction*> actions = menuWindow->actions();
   QString label = tabWidget->tabText(tab);
   QAction* action;
   bool found = false;

   foreach ( action, actions )
   {
      if ( label == action->text() )
      {
         found = true;
      }
   }
   if ( !found )
   {
      action = menuWindow->addAction(label);
      QObject::connect(action,SIGNAL(triggered()),this,SLOT(windowMenu_triggered()));
   }
   menuWindow->setEnabled(menuWindow->actions().count()>0);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
   ICenterWidgetItem* projectItem = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(index));
   QList<QAction*> actions = menuWindow->actions();
   QString label = tabWidget->tabText(index);
   QAction* action;

   foreach ( action, actions )
   {
      if ( label == action->text() )
      {
         QObject::disconnect(action,SIGNAL(triggered()),this,SLOT(windowMenu_triggered()));
         menuWindow->removeAction(action);
      }
   }
   menuWindow->setEnabled(menuWindow->actions().count()>0);

   if (projectItem)
   {
      if (projectItem->onCloseQuery())
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

void MainWindow::on_action_Project_Browser_toggled(bool visible)
{
   if ( visible )
   {
      m_pProjectBrowser->show();
   }
   else
   {
      m_pProjectBrowser->hide();
   }
}

void MainWindow::actionEmulation_Window_toggled(bool value)
{
   if (value)
   {
      m_pNESEmulator->show();
   }
   else
   {
      m_pNESEmulator->hide();
   }
}

void MainWindow::closeEvent ( QCloseEvent* event )
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.setValue("IDEGeometry",saveGeometry());
   settings.setValue("IDEState",saveState());

   if (nesicideProject->isInitialized())
   {
      closeProject();
   }

   QMainWindow::closeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent *event)
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
      output->show();

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

      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         // Load ROM if it exists.
         if ( !nesicideProject->getProjectCartridgeOutputName().isEmpty() )
         {
            QDir dir(QDir::currentPath());
            QString romName;
            romName = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectCartridgeOutputName()));

            nesicideProject->createProjectFromRom(romName,true);

            // Load debugger info if we can find it.
            CCC65Interface::captureDebugInfo();

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

            actionEmulation_Window->setChecked(true);
            actionEmulation_Window_toggled(true);
         }
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         // Load C64 image if it exists.
         if ( !nesicideProject->getProjectLinkerOutputName().isEmpty() )
         {
            QDir dir(QDir::currentPath());
            QString c64Name;
            c64Name = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectLinkerOutputName()));

            // Load debugger info if we can find it.
            CCC65Interface::captureDebugInfo();

            emit resetEmulator();
         }
      }

      m_pProjectBrowser->enableNavigation();

      settings.setValue("LastProject",fileName);

      projectDataChangesEvent();
   }
}

void MainWindow::on_actionOpen_Project_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "NESICIDE Project (*.nesproject)");

   if (fileName.isEmpty())
   {
      return;
   }

   openNesProject(fileName);
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

void MainWindow::reflectedOutput_Window_close(bool toplevel)
{
   actionOutput_Window->setChecked(toplevel);
}

void MainWindow::on_actionOutput_Window_toggled(bool value)
{
   output->setVisible(value);
}

void MainWindow::reflectedProjectBrowser_close(bool toplevel)
{
   action_Project_Browser->setChecked(toplevel);
}

void MainWindow::on_actionCompile_Project_triggered()
{
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::getObject("Compiler"));
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

void MainWindow::compiler_compileDone(bool bOk)
{
   actionCompile_Project->setEnabled(true);
   actionLoad_In_Emulator->setEnabled(true);

   projectDataChangesEvent();
}

void MainWindow::on_actionExecution_Inspector_toggled(bool value)
{
   m_pExecutionInspector->setVisible(value);
}

void MainWindow::reflectedExecutionInspector_close (bool toplevel)
{
   actionExecution_Inspector->setChecked(toplevel);
}

void MainWindow::actionExecution_Visualizer_Inspector_toggled(bool value)
{
   m_pExecutionVisualizer->setVisible(value);
}

void MainWindow::reflectedExecutionVisualizer_Inspector_close (bool toplevel)
{
   actionExecution_Visualizer_Inspector->setChecked(toplevel);
}

void MainWindow::actionBreakpoint_Inspector_toggled(bool value)
{
   m_pBreakpointInspector->setVisible(value);
}

void MainWindow::reflectedBreakpointInspector_close (bool toplevel)
{
   actionBreakpoint_Inspector->setChecked(toplevel);
}

void MainWindow::actionGfxCHRMemory_Inspector_toggled(bool value)
{
   m_pGfxCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxCHRMemoryInspector_close (bool toplevel)
{
   actionGfxCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionJoypadLogger_Inspector_toggled(bool value)
{
   m_pJoypadLoggerInspector->setVisible(value);
}

void MainWindow::reflectedJoypadLoggerInspector_close (bool toplevel)
{
   actionJoypadLogger_Inspector->setChecked(toplevel);
}

void MainWindow::actionGfxOAMMemory_Inspector_toggled(bool value)
{
   m_pGfxOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxOAMMemoryInspector_close (bool toplevel)
{
   actionGfxOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionGfxNameTableNESMemory_Inspector_toggled(bool value)
{
   m_pGfxNameTableMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxNameTableMemoryInspector_close (bool toplevel)
{
   actionGfxNameTableNESMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinOAMMemory_Inspector_toggled(bool value)
{
   m_pBinOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinOAMMemoryInspector_close (bool toplevel)
{
   actionBinOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinCPURegister_Inspector_toggled(bool value)
{
   m_pBinCPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinCPURegisterInspector_close ( bool toplevel )
{
   actionBinCPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinCPURAM_Inspector_toggled(bool value)
{
   m_pBinCPURAMInspector->setVisible(value);
}

void MainWindow::reflectedBinCPURAMInspector_close (bool toplevel)
{
   actionBinCPURAM_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinROM_Inspector_toggled(bool value)
{
   m_pBinROMInspector->setVisible(value);
}

void MainWindow::reflectedBinROMInspector_close (bool toplevel)
{
   actionBinROM_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinNameTableNESMemory_Inspector_toggled(bool value)
{
   m_pBinNameTableMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinNameTableMemoryInspector_close (bool toplevel)
{
   actionBinNameTableNESMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinPaletteNESMemory_Inspector_toggled(bool value)
{
   m_pBinPaletteMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinPaletteMemoryInspector_close (bool toplevel)
{
   actionBinPaletteNESMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinPPURegister_Inspector_toggled(bool value)
{
   m_pBinPPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinPPURegisterInspector_close ( bool toplevel )
{
   actionBinPPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinAPURegister_Inspector_toggled(bool value)
{
   m_pBinAPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinAPURegisterInspector_close ( bool toplevel )
{
   actionBinAPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinCHRMemory_Inspector_toggled(bool value)
{
   m_pBinCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinCHRMemoryInspector_close ( bool toplevel )
{
   actionBinCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinSRAMMemory_Inspector_toggled(bool value)
{
   m_pBinSRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinSRAMMemoryInspector_close ( bool toplevel )
{
   actionBinSRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinEXRAMMemory_Inspector_toggled(bool value)
{
   m_pBinEXRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinEXRAMMemoryInspector_close ( bool toplevel )
{
   actionBinEXRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionBinMapperMemory_Inspector_toggled(bool value)
{
   m_pBinMapperMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinMapperMemoryInspector_close ( bool toplevel )
{
   actionBinMapperMemory_Inspector->setChecked(toplevel);
}

void MainWindow::actionAssembly_Inspector_toggled(bool value)
{
   m_pAssemblyInspector->setVisible(value);
}

void MainWindow::reflectedEmulator_close ( bool toplevel )
{
   actionEmulation_Window->setChecked(toplevel);
}

void MainWindow::reflectedAssemblyInspector_close ( bool toplevel )
{
   actionAssembly_Inspector->setChecked(toplevel);
}

void MainWindow::actionCodeDataLogger_Inspector_toggled(bool value)
{
   m_pCodeDataLoggerInspector->setVisible(value);
}

void MainWindow::reflectedCodeDataLoggerInspector_close ( bool toplevel )
{
   actionCodeDataLogger_Inspector->setChecked(toplevel);
}

void MainWindow::actionPPUInformation_Inspector_toggled(bool value)
{
   m_pPPUInformationInspector->setVisible(value);
}

void MainWindow::reflectedPPUInformationInspector_close ( bool toplevel )
{
   actionPPUInformation_Inspector->setChecked(toplevel);
}

void MainWindow::actionAPUInformation_Inspector_toggled(bool value)
{
   m_pAPUInformationInspector->setVisible(value);
}

void MainWindow::reflectedAPUInformationInspector_close ( bool toplevel )
{
   actionAPUInformation_Inspector->setChecked(toplevel);
}

void MainWindow::actionMapperInformation_Inspector_toggled(bool value)
{
   m_pMapperInformationInspector->setVisible(value);
}

void MainWindow::reflectedMapperInformationInspector_close ( bool toplevel )
{
   actionMapperInformation_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionSymbol_Watch_toggled(bool value)
{
   m_pSymbolInspector->setVisible(value);
}

void MainWindow::reflectedSymbol_Watch_close ( bool toplevel )
{
   actionSymbol_Watch->setChecked(toplevel);
}

void MainWindow::on_actionCode_Profiler_toggled(bool value)
{
   m_pCodeProfiler->setVisible(value);
}

void MainWindow::reflectedCode_Profiler_close ( bool toplevel )
{
   actionCode_Profiler->setChecked(toplevel);
}

void MainWindow::on_actionSearch_triggered(bool value)
{
   if ( value )
   {
      output->showPane(OutputPaneDockWidget::Output_Search);
      m_pSearch->setVisible(true);
   }
   else
   {
      m_pSearch->setVisible(false);
   }
}

void MainWindow::reflectedSearch_close ( bool toplevel )
{
   actionSearch->setChecked(toplevel);
}

void MainWindow::on_action_About_Nesicide_triggered()
{
   AboutDialog* dlg = new AboutDialog(this);
   dlg->exec();
   delete dlg;
}

void MainWindow::closeProject()
{
   QList<QAction*> actions = menuWindow->actions();
   QAction* action;
   int idx;

   // Close all inspectors
   CDockWidgetRegistry::hideAll();

   // Try to close all opened editors
   for ( idx = tabWidget->count()-1; idx >= 0; idx-- )
   {
      ICenterWidgetItem* item = dynamic_cast<ICenterWidgetItem*>(tabWidget->widget(idx));
      if ( item )
      {
         tabWidget->setCurrentWidget(tabWidget->widget(idx));
         if ( item->onSaveQuery() )
         {
            item->onSave();
         }
         tabWidget->removeTab(idx);
      }
   }

   if (nesicideProject->isDirty())
   {
      int result = QMessageBox::question(this,"Save Project?","Your project settings, project content, or debugger content has changed, would you like to save the project?",QMessageBox::Yes,QMessageBox::No);
      if ( result == QMessageBox::Yes )
      {
         on_actionSave_Project_triggered();
      }
   }

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

   CCC65Interface::clear();

   nesicideProject->terminateProject();

   emit primeEmulator();
   emit resetEmulator();

   if ( EnvironmentSettingsDialog::showWelcomeOnStart() )
   {
      tabWidget->addTab(tab,"Welcome Page");
      webView->setUrl(QUrl( "http://www.nesicide.com"));
   }

   // Clear output
   output->clearAllPanes();
   output->hide();

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
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x10);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x10));
   }
}

void MainWindow::actionNoise_toggled(bool value)
{
   EmulatorPrefsDialog::setNoiseEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x08);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x08));
   }
}

void MainWindow::actionTriangle_toggled(bool value)
{
   EmulatorPrefsDialog::setTriangleEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x04);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x04));
   }
}

void MainWindow::actionSquare_2_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare2Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x02);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x02));
   }
}

void MainWindow::actionSquare_1_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x01);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x01));
   }
}

void MainWindow::on_actionEnvironment_Settings_triggered()
{
   EnvironmentSettingsDialog dlg;

   dlg.exec();

   emit applyEnvironmentSettings();
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
      int mask = ((square1<<0)|(square2<<1)|(triangle<<2)|(noise<<3)|(dmc<<4));

      actionSquare_1->setChecked(square1);
      actionSquare_2->setChecked(square2);
      actionTriangle->setChecked(triangle);
      actionNoise->setChecked(noise);
      actionDelta_Modulation->setChecked(dmc);
      nesSetAudioChannelMask(mask);
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

void MainWindow::actionPreferences_triggered()
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
   tabWidget->addTab(tab,"Welcome Page");
   webView->setUrl(QUrl( "http://www.nesicide.com"));
}

void MainWindow::on_actionLoad_In_Emulator_triggered()
{
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::getObject("Compiler"));

   output->showPane(OutputPaneDockWidget::Output_Build);

   if ( compiler->assembledOk() )
   {
      actionLoad_In_Emulator->setEnabled(false);

      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         buildTextLogger->write("<b>Loading ROM...</b>");

         if ( !CCC65Interface::captureINESImage() )
         {
            buildTextLogger->write("<font color='red'><b>Load NES failed.</b></font>");
            return;
         }

         if ( !CCC65Interface::captureDebugInfo() )
         {
             buildTextLogger->write("<font color='red'><b>Loading debug information failed.</b></font>");
         }

         emit primeEmulator();
         emit resetEmulator();

         buildTextLogger->write("<b>Load complete.</b>");

         actionEmulation_Window->setChecked(true);
         actionEmulation_Window_toggled(true);
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         buildTextLogger->write("<b>Loading C64...</b>");

         if ( !CCC65Interface::captureDebugInfo() )
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
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::getObject("Compiler"));

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

void MainWindow::actionBinSIDRegister_Inspector_toggled(bool value)
{
   m_pBinSIDRegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinSIDRegisterInspector_close(bool toplevel)
{
   actionBinSIDRegister_Inspector->setChecked(toplevel);
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
