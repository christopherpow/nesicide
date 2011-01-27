#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "inspectorregistry.h"
#include "cpluginmanager.h"

#include "main.h"

#include "pasm_lib.h"

#include <QApplication>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>

OutputPaneDockWidget* output = NULL;
ProjectBrowserDockWidget* projectBrowser = NULL;

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   QSettings settings;
   
   QObject::connect(this, SIGNAL(destroyed()), this, SLOT(handle_MainWindow_destroyed()));

   QObject::connect(compiler, SIGNAL(compileStarted()), this, SLOT(compiler_compileStarted()));
   QObject::connect(compiler, SIGNAL(compileDone(bool)), this, SLOT(compiler_compileDone(bool)));
   
   generalTextLogger = new CTextLogger();
   buildTextLogger = new CTextLogger();
   debugTextLogger = new CTextLogger();

   nesicideProject = new CNesicideProject();

   ui->setupUi(this);

   emulatorDlg = new NESEmulatorDockWidget();
   addDockWidget(Qt::RightDockWidgetArea, emulatorDlg );
   emulatorDlg->hide();
   QObject::connect(emulatorDlg, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedEmulator_close(bool)));
   InspectorRegistry::addInspector ( "Emulator", emulatorDlg );
   
   QObject::connect(emulator, SIGNAL(cartridgeLoaded()), this, SLOT(projectDataChangesEvent()));

   m_pSourceNavigator = new SourceNavigator(ui->tabWidget);
   ui->compilerToolbar->addWidget(m_pSourceNavigator);

   projectBrowser = new ProjectBrowserDockWidget(ui->tabWidget,m_pSourceNavigator);
   addDockWidget(Qt::LeftDockWidgetArea, projectBrowser );
   projectBrowser->hide();
   QObject::connect(projectBrowser, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedProjectBrowser_close(bool)));
   InspectorRegistry::addInspector ( "Project", projectBrowser );
   
   output = new OutputPaneDockWidget(ui->tabWidget);
   addDockWidget(Qt::BottomDockWidgetArea, output );
   output->hide();
   QObject::connect(output, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedOutput_Window_close(bool)));
   QObject::connect(generalTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateGeneralPane(QString)));
   QObject::connect(buildTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateBuildPane(QString)));
   QObject::connect(debugTextLogger,SIGNAL(updateText(QString)),output,SLOT(updateDebugPane(QString)));
   QObject::connect(generalTextLogger,SIGNAL(eraseText()),output,SLOT(eraseGeneralPane()));
   QObject::connect(buildTextLogger,SIGNAL(eraseText()),output,SLOT(eraseBuildPane()));
   QObject::connect(debugTextLogger,SIGNAL(eraseText()),output,SLOT(eraseDebugPane()));
   QObject::connect(breakpointWatcher,SIGNAL(showPane(int)),output,SLOT(showPane(int)));
   InspectorRegistry::addInspector ( "Output", output );

   generalTextLogger->write("<strong>NESICIDE2</strong> Alpha Release");
   generalTextLogger->write("<strong>Plugin Scripting Subsystem:</strong> " + pluginManager->getVersionInfo());

   m_pBreakpointInspector = new BreakpointDockWidget ();
   addDockWidget(Qt::BottomDockWidgetArea, m_pBreakpointInspector );
   m_pBreakpointInspector->hide();
   QObject::connect(m_pBreakpointInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBreakpointInspector_close(bool)));
   InspectorRegistry::addInspector ( "Breakpoints", m_pBreakpointInspector );

   m_pGfxCHRMemoryInspector = new CHRMEMInspector ();
   m_pGfxCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
   m_pGfxCHRMemoryInspector->setWindowTitle("CHR Memory Visualizer");
   m_pGfxCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxCHRMemoryInspector );
   m_pGfxCHRMemoryInspector->hide();
   QObject::connect(m_pGfxCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxCHRMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "CHR Memory Visualizer", m_pGfxCHRMemoryInspector );

   m_pGfxOAMMemoryInspector = new OAMVisualizerDockWidget ();
   addDockWidget(Qt::BottomDockWidgetArea, m_pGfxOAMMemoryInspector );
   m_pGfxOAMMemoryInspector->hide();
   QObject::connect(m_pGfxOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxOAMMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "OAM Memory Visualizer", m_pGfxOAMMemoryInspector );

   m_pGfxNameTableMemoryInspector = new NameTableVisualizerDockWidget ();
   addDockWidget(Qt::RightDockWidgetArea, m_pGfxNameTableMemoryInspector );
   m_pGfxNameTableMemoryInspector->hide();
   QObject::connect(m_pGfxNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxNameTableMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "Name Table Visualizer", m_pGfxNameTableMemoryInspector );

   m_pExecutionInspector = new ExecutionInspectorDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionInspector );
   m_pExecutionInspector->hide();
   QObject::connect(m_pExecutionInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionInspector_close(bool)));
   InspectorRegistry::addInspector ( "Execution Inspector", m_pExecutionInspector );

   m_pExecutionVisualizer = new ExecutionVisualizerDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionVisualizer );
   m_pExecutionVisualizer->hide();
   QObject::connect(m_pExecutionVisualizer, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionVisualizer_Inspector_close(bool)));
   InspectorRegistry::addInspector ( "Execution Visualizer", m_pExecutionVisualizer );

   m_pCodeInspector = new CodeBrowserDockWidget();
   addDockWidget(Qt::RightDockWidgetArea, m_pCodeInspector );
   m_pCodeInspector->hide();
   QObject::connect(m_pCodeInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCodeInspector_close(bool)));
   InspectorRegistry::addInspector ( "Code Browser", m_pCodeInspector );

   m_pCodeDataLoggerInspector = new CodeDataLoggerDockWidget();
   addDockWidget(Qt::RightDockWidgetArea, m_pCodeDataLoggerInspector );
   m_pCodeDataLoggerInspector->hide();
   QObject::connect(m_pCodeDataLoggerInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCodeDataLoggerInspector_close(bool)));
   InspectorRegistry::addInspector ( "Code/Data Logger Inspector", m_pCodeDataLoggerInspector );

   m_pBinCPURegisterInspector = new RegisterInspectorDockWidget(eMemory_CPUregs);
   m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
   m_pBinCPURegisterInspector->hide();
   QObject::connect(m_pBinCPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURegisterInspector_close(bool)));
   InspectorRegistry::addInspector ( "CPU Register Inspector", m_pBinCPURegisterInspector );

   m_pBinCPURAMInspector = new MemoryInspectorDockWidget(eMemory_CPU);
   m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
   m_pBinCPURAMInspector->hide();
   QObject::connect(m_pBinCPURAMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURAMInspector_close(bool)));
   InspectorRegistry::addInspector ( "CPU RAM Inspector", m_pBinCPURAMInspector );

   m_pBinROMInspector = new MemoryInspectorDockWidget(eMemory_cartROM);
   m_pBinROMInspector->setWindowTitle("PRG-ROM Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinROMInspector );
   m_pBinROMInspector->hide();
   QObject::connect(m_pBinROMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinROMInspector_close(bool)));
   InspectorRegistry::addInspector ( "PRG-ROM Inspector", m_pBinROMInspector );

   m_pBinNameTableMemoryInspector = new MemoryInspectorDockWidget(eMemory_PPU);
   m_pBinNameTableMemoryInspector->setWindowTitle("NameTable Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinNameTableMemoryInspector );
   m_pBinNameTableMemoryInspector->hide();
   QObject::connect(m_pBinNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinNameTableMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "NameTable Inspector", m_pBinNameTableMemoryInspector );

   m_pBinPPURegisterInspector = new RegisterInspectorDockWidget(eMemory_PPUregs);
   m_pBinPPURegisterInspector->setWindowTitle("PPU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPURegisterInspector );
   m_pBinPPURegisterInspector->hide();
   QObject::connect(m_pBinPPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPPURegisterInspector_close(bool)));
   InspectorRegistry::addInspector ( "PPU Register Inspector", m_pBinPPURegisterInspector );

   m_pPPUInformationInspector = new PPUInformationDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pPPUInformationInspector );
   m_pPPUInformationInspector->hide();
   QObject::connect(m_pPPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedPPUInformationInspector_close(bool)));
   InspectorRegistry::addInspector ( "PPU Information", m_pPPUInformationInspector );

   m_pBinAPURegisterInspector = new RegisterInspectorDockWidget(eMemory_IOregs);
   m_pBinAPURegisterInspector->setWindowTitle("APU Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinAPURegisterInspector );
   m_pBinAPURegisterInspector->hide();
   QObject::connect(m_pBinAPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinAPURegisterInspector_close(bool)));
   InspectorRegistry::addInspector ( "APU Register Inspector", m_pBinAPURegisterInspector );

   m_pAPUInformationInspector = new APUInformationDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pAPUInformationInspector );
   m_pAPUInformationInspector->hide();
   QObject::connect(m_pAPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedAPUInformationInspector_close(bool)));
   InspectorRegistry::addInspector ( "APU Information", m_pAPUInformationInspector );

   m_pBinCHRMemoryInspector = new MemoryInspectorDockWidget(eMemory_cartCHRMEM);
   m_pBinCHRMemoryInspector->setWindowTitle("CHR Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinCHRMemoryInspector );
   m_pBinCHRMemoryInspector->hide();
   QObject::connect(m_pBinCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCHRMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "CHR Memory Inspector", m_pBinCHRMemoryInspector );

   m_pBinOAMMemoryInspector = new RegisterInspectorDockWidget(eMemory_PPUoam);
   m_pBinOAMMemoryInspector->setWindowTitle("OAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinOAMMemoryInspector );
   m_pBinOAMMemoryInspector->hide();
   QObject::connect(m_pBinOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinOAMMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "OAM Memory Inspector", m_pBinOAMMemoryInspector );

   m_pBinPaletteMemoryInspector = new MemoryInspectorDockWidget(eMemory_PPUpalette);
   m_pBinPaletteMemoryInspector->setWindowTitle("Palette Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinPaletteMemoryInspector );
   m_pBinPaletteMemoryInspector->hide();
   QObject::connect(m_pBinPaletteMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPaletteMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "Palette Memory Inspector", m_pBinPaletteMemoryInspector );

   m_pBinSRAMMemoryInspector = new MemoryInspectorDockWidget(eMemory_cartSRAM);
   m_pBinSRAMMemoryInspector->setWindowTitle("Cartridge SRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinSRAMMemoryInspector );
   m_pBinSRAMMemoryInspector->hide();
   QObject::connect(m_pBinSRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinSRAMMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "Cartridge SRAM Memory Inspector", m_pBinSRAMMemoryInspector );

   m_pBinEXRAMMemoryInspector = new MemoryInspectorDockWidget(eMemory_cartEXRAM);
   m_pBinEXRAMMemoryInspector->setWindowTitle("Cartridge EXRAM Memory Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinEXRAMMemoryInspector );
   m_pBinEXRAMMemoryInspector->hide();
   QObject::connect(m_pBinEXRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinEXRAMMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "Cartridge EXRAM Memory Inspector", m_pBinEXRAMMemoryInspector );

   m_pMapperInformationInspector = new MapperInformationDockWidget();
   addDockWidget(Qt::BottomDockWidgetArea, m_pMapperInformationInspector );
   m_pMapperInformationInspector->hide();
   QObject::connect(m_pMapperInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedMapperInformationInspector_close(bool)));
   InspectorRegistry::addInspector ( "Cartridge Mapper Information", m_pMapperInformationInspector );

   m_pBinMapperMemoryInspector = new RegisterInspectorDockWidget(eMemory_cartMapper);
   m_pBinMapperMemoryInspector->setWindowTitle("Cartridge Mapper Register Inspector");
   addDockWidget(Qt::BottomDockWidgetArea, m_pBinMapperMemoryInspector );
   m_pBinMapperMemoryInspector->hide();
   QObject::connect(m_pBinMapperMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinMapperMemoryInspector_close(bool)));
   InspectorRegistry::addInspector ( "Cartridge Mapper Register Inspector", m_pBinMapperMemoryInspector );

   // Start in NTSC mode for now until we can have it configurable on app entry.
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);
   nesSetSystemMode(MODE_NTSC);

   // Start with all sound channels enabled...
   ui->actionSquare_1->setChecked(true);
   ui->actionSquare_2->setChecked(true);
   ui->actionTriangle->setChecked(true);
   ui->actionNoise->setChecked(true);
   ui->actionDelta_Modulation->setChecked(true);
   ui->actionMute_All->setChecked(false);

   if ( settings.value("showWelcomeOnStart",QVariant(true)) == QVariant(true) )
   {
      ui->tabWidget->addTab(ui->tab,"Welcome Page");
      ui->webView->setUrl(QUrl( "http://wiki.nesicide.com/doku.php?id=nesicide_user_manual"));
   }
   else
   {
      ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tab));
   }

   QStringList argv = QApplication::arguments();
   QStringList argv_nes = argv.filter ( QRegExp("*.nes",Qt::CaseInsensitive,QRegExp::Wildcard) );
   QStringList argv_nesproject = argv.filter ( QRegExp("*.nesproject",Qt::CaseInsensitive,QRegExp::Wildcard) );

   if ( (argv_nes.count() >= 1) && 
        (argv_nesproject.count() >= 1) )
   {
      QMessageBox::information ( 0, "Command Line Error", "Cannot specify a .nes and a .nesproject file to open.\n" );
   }
   
   if ( argv_nes.count() >= 1 )
   {
      openROM(argv_nes.at(0));

      if ( argv_nes.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NES ROM files were specified on the command\n"
                                    "line.  Only the first NES ROM was opened, all others\n"
                                    "were ignored." );
      }
   }
   else if ( argv_nesproject.count() >= 1 )
   {
      openProject(argv_nesproject.at(0));

      if ( argv_nesproject.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NESICIDE project files were specified on the command\n"
                                    "line.  Only the first NESICIDE project was opened, all others\n"
                                    "were ignored." );
      }
   }
   
   projectDataChangesEvent();

   // Always call this last
   pluginManager->doInitScript();
   pluginManager->loadPlugins();
}

MainWindow::~MainWindow()
{
   ui->tabWidget->clear();

   delete generalTextLogger;
   delete buildTextLogger;
   delete debugTextLogger;

   delete nesicideProject;
   delete pluginManager;
   delete ui;
   
   delete m_pBreakpointInspector;
   delete m_pGfxCHRMemoryInspector;
   delete m_pGfxOAMMemoryInspector;
   delete m_pGfxNameTableMemoryInspector;
   delete m_pExecutionInspector;
   delete m_pExecutionVisualizer;
   delete m_pCodeInspector;
   delete m_pCodeDataLoggerInspector;
   delete m_pBinCPURegisterInspector;
   delete m_pBinCPURAMInspector;
   delete m_pBinROMInspector;
   delete m_pBinNameTableMemoryInspector;
   delete m_pBinPPURegisterInspector;
   delete m_pPPUInformationInspector;
   delete m_pBinAPURegisterInspector;
   delete m_pAPUInformationInspector;
   delete m_pBinCHRMemoryInspector;
   delete m_pBinOAMMemoryInspector;
   delete m_pBinPaletteMemoryInspector;
   delete m_pBinSRAMMemoryInspector;
   delete m_pBinEXRAMMemoryInspector; 
   delete m_pMapperInformationInspector;
   delete m_pBinMapperMemoryInspector;
   delete m_pSourceNavigator;
}

void MainWindow::changeEvent(QEvent* e)
{
   QMainWindow::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;
      
   if ( event->mimeData()->hasUrls() )
   {
      fileUrls = event->mimeData()->urls();
      
      fileName = fileUrls.at(0).toLocalFile();
      
      if ( ((!fileName.contains(".nesproject",Qt::CaseInsensitive)) && (fileName.contains(".nes",Qt::CaseInsensitive))) ||
           (fileName.contains(".nesproject",Qt::CaseInsensitive)) )
      {   
         event->acceptProposedAction();
      }
   }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;
      
   if ( event->mimeData()->hasUrls() )
   {
      fileUrls = event->mimeData()->urls();
      
      fileName = fileUrls.at(0).toLocalFile();
      
      if ( ((!fileName.contains(".nesproject",Qt::CaseInsensitive)) && (fileName.contains(".nes",Qt::CaseInsensitive))) ||
           (fileName.contains(".nesproject",Qt::CaseInsensitive)) )
      {   
         event->acceptProposedAction();
      }
   }
}

void MainWindow::dropEvent(QDropEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;
   
   if ( event->mimeData()->hasUrls() )
   {
      output->showPane(OutputPaneDockWidget::Output_General);
            
      fileUrls = event->mimeData()->urls();
      
      fileName = fileUrls.at(0).toLocalFile();
      
      if ( fileName.contains(".nesproject",Qt::CaseInsensitive) )
      {
         if ( nesicideProject->isInitialized() )
         {
            on_action_Close_Project_triggered();
         }
         openProject(fileName);
         
         event->acceptProposedAction();
      }
      else if ( fileName.contains(".nes",Qt::CaseInsensitive) )
      {   
         openROM(fileName);
         
         event->acceptProposedAction();
      }
   }
}

void MainWindow::hideEvent(QHideEvent *event)
{
   InspectorRegistry::saveVisibility();
}

void MainWindow::showEvent(QShowEvent *event)
{
   InspectorRegistry::restoreVisibility();
}

void MainWindow::projectDataChangesEvent()
{
   projectBrowser->layoutChangedEvent();
   projectBrowser->setVisible(nesicideProject->isInitialized());
   output->setVisible(nesicideProject->isInitialized());

   // Enabled/Disable actions based on if we have a project loaded or not
   ui->actionNew_Project->setEnabled(!nesicideProject->isInitialized());
   ui->actionCreate_Project_from_ROM->setEnabled(!nesicideProject->isInitialized());
   ui->actionOpen_Project->setEnabled(!nesicideProject->isInitialized());
   ui->actionProject_Properties->setEnabled(nesicideProject->isInitialized());
   ui->action_Project_Browser->setEnabled(nesicideProject->isInitialized());
   ui->action_Close_Project->setEnabled(nesicideProject->isInitialized());
   ui->actionCompile_Project->setEnabled(nesicideProject->isInitialized());
   ui->actionSave_Project->setEnabled(nesicideProject->isInitialized());
   ui->actionSave_Project_As->setEnabled(nesicideProject->isInitialized());
   
   // Enabled/Disable actions based on if we have a project loaded or not and a cartridge loaded in the emulator
   ui->actionEmulation_Window->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionExecution_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionExecution_Visualizer_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBreakpoint_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionCode_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionCodeDataLogger_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionGfxCHRMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionGfxOAMMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionGfxNameTableMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinCPURegister_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinCPURAM_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinROM_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinNameTableMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinCHRMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinOAMMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinPaletteMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinSRAMMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinEXRAMMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinPPURegister_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinAPURegister_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionBinMapperMemory_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionPPUInformation_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionAPUInformation_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );
   ui->actionMapperInformation_Inspector->setEnabled ( nesicideProject->isInitialized() && nesROMIsLoaded() );

   // Enabled/Disable actions based on if we have a project loaded or not and a good compile
   ui->actionRun_In_Emulator->setEnabled ( nesicideProject->isInitialized() && compiler->assembledOk() );

   if (ui->tabWidget->currentIndex() >= 0)
   {
      IProjectTreeViewItem* projectItem = matchTab(ui->tabWidget->currentWidget());

      if (projectItem)
      {
         ui->actionSave_Active_Document->setEnabled(((IProjectTreeViewItem*)projectItem)->isDocumentSaveable());
      }
      else
      {
         ui->actionSave_Active_Document->setEnabled(false);
      }
   }

   setWindowTitle(nesicideProject->getProjectTitle().prepend("NESICIDE - "));
}

void MainWindow::on_actionSave_Project_triggered()
{
   if (projectFileName.isEmpty())
   {
      projectFileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                     QString("NESICIDE Project (*.nesproject)"));
   }
   
   if (!projectFileName.isEmpty())
   {
      saveProject();
   }
}

void MainWindow::saveProject()
{
   QFile file(projectFileName);

   if ( !file.open( QFile::WriteOnly) )
   {
      QMessageBox::critical(this, "Error", "An error occured while trying to open the project file for writing.");
      projectFileName.clear();
      return;
   }

   QDomDocument doc;
   QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
   doc.appendChild(instr);

   if (!nesicideProject->serialize(doc, doc))
   {
      QMessageBox::critical(this, "Error", "An error occured while trying to serialize the project data.");
      file.close();
      projectFileName.clear();
   }

   // Create a text stream so we can stream the XML data to the file easily.
   QTextStream ts( &file );

   // Use the standard C++ stream function for streaming the string representation of our XML to
   // our file stream.
   ts << doc.toString();

   // And finally close the file.
   file.close();
}

void MainWindow::on_actionSave_Project_As_triggered()
{
   // Allow the user to select a file name. Note that using the static function produces a native
   // file dialog, while creating an instance of QFileDialog results in a non-native file dialog..
   projectFileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                  QString("NESICIDE Project (*.nesproject)"));

   if (!projectFileName.isEmpty())
   {
      saveProject();
   }
}

void MainWindow::on_actionProject_Properties_triggered()
{
   ProjectPropertiesDialog* dlg = new ProjectPropertiesDialog(this);

   if (dlg->exec() == QDialog::Accepted)
   {
      emulator->pauseEmulation(false);
   
      nesicideProject->setProjectTitle(dlg->getProjectName());
      nesicideProject->getProjectPaletteEntries()->clear();

      for (int paletteItemIndex=0; paletteItemIndex<dlg->currentPalette.count(); paletteItemIndex++)
      {
         nesicideProject->getProjectPaletteEntries()->append(dlg->currentPalette.at(paletteItemIndex));
      }
      
      nesicideProject->getCartridge()->setMapperNumber(dlg->getMapperNumber());
      nesicideProject->getCartridge()->setMirrorMode(dlg->getMirrorMode());
      
      emulator->primeEmulator();
      emulator->resetEmulator();

      projectDataChangesEvent();

      for (int sourceIndex = 0; sourceIndex < nesicideProject->getProject()->getSources()->childCount(); sourceIndex++)
      {
         CSourceItem* sourceItem = (CSourceItem*)nesicideProject->getProject()->getSources()->child(sourceIndex);

         if (sourceItem->name() == dlg->getMainSource())
         {
            nesicideProject->getProject()->setMainSource(sourceItem);
         }
      }
   }

   delete dlg;
}

void MainWindow::on_actionNew_Project_triggered()
{
   NewProjectDialog* dlg = new NewProjectDialog();

   if (dlg->exec() == QDialog::Accepted)
   {
      projectBrowser->disableNavigation();
      nesicideProject->setProjectTitle(dlg->getProjectTitle());
      nesicideProject->initializeProject();
      projectBrowser->enableNavigation();
      projectDataChangesEvent();
   }
   delete dlg;
}

void MainWindow::openROM(QString fileName)
{
   output->showPane(OutputPaneDockWidget::Output_General);

   emulator->pauseEmulation(false);
   
   // Remove any lingering project content
   projectBrowser->disableNavigation();
   nesicideProject->terminateProject();
   
   // Clear output
   output->clearAllPanes();
   output->show();
    
   // Create new project from ROM
   nesicideProject->createProjectFromRom(fileName);

   projectBrowser->enableNavigation();
   
   emulator->primeEmulator();
   emulator->resetEmulator();
//   emulator->startEmulation();
   
   compiler->reset();
   
   pasm_initialize();
   
   projectDataChangesEvent();
   
   ui->actionEmulation_Window->setChecked(true);
   on_actionEmulation_Window_toggled(true);
}

void MainWindow::on_actionCreate_Project_from_ROM_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "iNES ROM (*.nes)");

   if (fileName.isEmpty())
   {
      return;
   }

   openROM(fileName);
}

IProjectTreeViewItem* MainWindow::matchTab(QWidget* pTab)
{
   IProjectTreeViewItemIterator iter(nesicideProject);
   IProjectTreeViewItem*        item = NULL;
   
   while ( iter.current() )
   {
      if ( iter.current()->tab() == pTab )
      {
         item = iter.current();
         break;
      }
      
      iter.next();
   }

   return item;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
   IProjectTreeViewItem* projectItem = matchTab(ui->tabWidget->widget(index));

   if (projectItem)
   {
      if (((IProjectTreeViewItem*)projectItem)->onCloseQuery())
      {
         ui->tabWidget->removeTab(index);
         ((IProjectTreeViewItem*)projectItem)->onClose();
      }
   }
   else
   {
      ui->tabWidget->removeTab(index);
   }
}

void MainWindow::on_action_Project_Browser_toggled(bool visible)
{
   if ( visible )
   {
      projectBrowser->show();
   }
   else
   {
      projectBrowser->hide();
   }
}

void MainWindow::on_actionEmulation_Window_toggled(bool value)
{
   if (value)
   {
      emulatorDlg->show();
   }
   else
   {
      emulatorDlg->hide();
   }
}

void MainWindow::closeEvent ( QCloseEvent* event )
{
   emulator->pauseEmulation(false);

   QWidget::closeEvent(event);
}

void MainWindow::openProject(QString fileName)
{
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

      projectBrowser->disableNavigation();

      nesicideProject->initializeProject();
      
      // Clear output
      output->clearAllPanes();
      output->show();

      // Load new project content
      nesicideProject->deserialize(doc, doc);

      projectBrowser->enableNavigation();
   
      compiler->reset();
      
      pasm_initialize();
      
      projectDataChangesEvent();
      
      projectFileName = fileName;
   }
}

void MainWindow::on_actionOpen_Project_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "NESICIDE Project (*.nesproject)");
   
   if (fileName.isEmpty())
   {
      return;
   }
   
   openProject(fileName);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
   IProjectTreeViewItem* projectItem = matchTab(ui->tabWidget->widget(index));

   if (projectItem)
   {
      ui->actionSave_Active_Document->setEnabled(projectItem->isDocumentSaveable());
   }
   else
   {
      ui->actionSave_Active_Document->setEnabled(false);
   }
}

void MainWindow::on_actionSave_Active_Document_triggered()
{
   IProjectTreeViewItem* projectItem = matchTab(ui->tabWidget->currentWidget());

   if (projectItem)
   {
      ((IProjectTreeViewItem*)projectItem)->onSaveDocument();
   }

}

void MainWindow::reflectedOutput_Window_close(bool toplevel)
{
   ui->actionOutput_Window->setChecked(toplevel);
}

void MainWindow::on_actionOutput_Window_toggled(bool value)
{
   output->setVisible(value);
}

void MainWindow::reflectedProjectBrowser_close(bool toplevel)
{
   ui->action_Project_Browser->setChecked(toplevel);
}

void MainWindow::on_actionCompile_Project_triggered()
{
   QSettings settings;
   output->showPane(OutputPaneDockWidget::Output_Build);
   emulator->pauseEmulation(false);
   
   if ( settings.value("saveAllOnCompile",QVariant(true)) == QVariant(true) )
   {
      saveProject();
   }
   compiler->assemble();
}

void MainWindow::compiler_compileStarted()
{
   ui->actionCompile_Project->setEnabled(false);
   ui->actionRun_In_Emulator->setEnabled(false);
}

void MainWindow::compiler_compileDone(bool bOk)
{
   ui->actionCompile_Project->setEnabled(true);
   
   projectDataChangesEvent();
}

void MainWindow::on_actionExecution_Inspector_toggled(bool value)
{
   m_pExecutionInspector->setVisible(value);
}

void MainWindow::reflectedExecutionInspector_close (bool toplevel)
{
   ui->actionExecution_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionExecution_Visualizer_Inspector_toggled(bool value)
{
   m_pExecutionVisualizer->setVisible(value);
}

void MainWindow::reflectedExecutionVisualizer_Inspector_close (bool toplevel)
{
   ui->actionExecution_Visualizer_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBreakpoint_Inspector_toggled(bool value)
{
   m_pBreakpointInspector->setVisible(value);
}

void MainWindow::reflectedBreakpointInspector_close (bool toplevel)
{
   ui->actionBreakpoint_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxCHRMemory_Inspector_toggled(bool value)
{
   m_pGfxCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxCHRMemoryInspector_close (bool toplevel)
{
   ui->actionGfxCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxOAMMemory_Inspector_toggled(bool value)
{
   m_pGfxOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxOAMMemoryInspector_close (bool toplevel)
{
   ui->actionGfxOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxNameTableMemory_Inspector_toggled(bool value)
{
   m_pGfxNameTableMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxNameTableMemoryInspector_close (bool toplevel)
{
   ui->actionGfxNameTableMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinOAMMemory_Inspector_toggled(bool value)
{
   m_pBinOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinOAMMemoryInspector_close (bool toplevel)
{
   ui->actionBinOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCPURegister_Inspector_toggled(bool value)
{
   m_pBinCPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinCPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinCPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCPURAM_Inspector_toggled(bool value)
{
   m_pBinCPURAMInspector->setVisible(value);
}

void MainWindow::reflectedBinCPURAMInspector_close (bool toplevel)
{
   ui->actionBinCPURAM_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinROM_Inspector_toggled(bool value)
{
   m_pBinROMInspector->setVisible(value);
}

void MainWindow::reflectedBinROMInspector_close (bool toplevel)
{
   ui->actionBinROM_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinNameTableMemory_Inspector_toggled(bool value)
{
   m_pBinNameTableMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinNameTableMemoryInspector_close (bool toplevel)
{
   ui->actionBinNameTableMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinPaletteMemory_Inspector_toggled(bool value)
{
   m_pBinPaletteMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinPaletteMemoryInspector_close (bool toplevel)
{
   ui->actionBinPaletteMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinPPURegister_Inspector_toggled(bool value)
{
   m_pBinPPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinPPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinPPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinAPURegister_Inspector_toggled(bool value)
{
   m_pBinAPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinAPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinAPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCHRMemory_Inspector_toggled(bool value)
{
   m_pBinCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinCHRMemoryInspector_close ( bool toplevel )
{
   ui->actionBinCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinSRAMMemory_Inspector_toggled(bool value)
{
   m_pBinSRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinSRAMMemoryInspector_close ( bool toplevel )
{
   ui->actionBinSRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinEXRAMMemory_Inspector_toggled(bool value)
{
   m_pBinEXRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinEXRAMMemoryInspector_close ( bool toplevel )
{
   ui->actionBinEXRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinMapperMemory_Inspector_toggled(bool value)
{
   m_pBinMapperMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinMapperMemoryInspector_close ( bool toplevel )
{
   ui->actionBinMapperMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionCode_Inspector_toggled(bool value)
{
   m_pCodeInspector->setVisible(value);
}

void MainWindow::reflectedEmulator_close ( bool toplevel )
{
   ui->actionEmulation_Window->setChecked(toplevel);
}

void MainWindow::reflectedCodeInspector_close ( bool toplevel )
{
   ui->actionCode_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionCodeDataLogger_Inspector_toggled(bool value)
{
   m_pCodeDataLoggerInspector->setVisible(value);
}

void MainWindow::reflectedCodeDataLoggerInspector_close ( bool toplevel )
{
   ui->actionCodeDataLogger_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionPPUInformation_Inspector_toggled(bool value)
{
   m_pPPUInformationInspector->setVisible(value);
}

void MainWindow::reflectedPPUInformationInspector_close ( bool toplevel )
{
   ui->actionPPUInformation_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionAPUInformation_Inspector_toggled(bool value)
{
   m_pAPUInformationInspector->setVisible(value);
}

void MainWindow::reflectedAPUInformationInspector_close ( bool toplevel )
{
   ui->actionAPUInformation_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionMapperInformation_Inspector_toggled(bool value)
{
   m_pMapperInformationInspector->setVisible(value);
}

void MainWindow::reflectedMapperInformationInspector_close ( bool toplevel )
{
   ui->actionMapperInformation_Inspector->setChecked(toplevel);
}

void MainWindow::on_action_About_Nesicide_triggered()
{
   AboutDialog* dlg = new AboutDialog(this);
   dlg->exec();
   delete dlg;
}

void MainWindow::on_action_Close_Project_triggered()
{
   QSettings settings;
   
   // Stop the emulator if it is running
   emulator->pauseEmulation(false);

   // Terminate the project and let the IDE know
   projectBrowser->disableNavigation();
   
   nesicideProject->terminateProject();
   
   emulator->primeEmulator();
   emulator->resetEmulator();
   
   compiler->reset();
   
   m_pSourceNavigator->shutdown();
   
   pasm_initialize();

   // Remove any tabs
   ui->tabWidget->clear();
   
   if ( settings.value("showWelcomeOnStart",QVariant(true)) == QVariant(true) )
   {
      ui->tabWidget->addTab(ui->tab,"Welcome Page");
      ui->webView->setUrl(QUrl( "http://wiki.nesicide.com/doku.php?id=nesicide_user_manual"));
   }
      
   // Clear output
   output->clearAllPanes();
   output->hide();

   // Close all inspectors
   InspectorRegistry::hideAll();

   // Let the UI know what's up
   projectDataChangesEvent();
}

void MainWindow::handle_MainWindow_destroyed()
{
   if (nesicideProject->isInitialized())
   {
      on_action_Close_Project_triggered();
   }
}

void MainWindow::on_actionNTSC_triggered()
{
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);
   nesSetSystemMode(MODE_NTSC);

   emulator->resetEmulator();
   emulator->startEmulation();
}

void MainWindow::on_actionPAL_triggered()
{
   ui->actionNTSC->setChecked(false);
   ui->actionPAL->setChecked(true);
   nesSetSystemMode(MODE_PAL);

   emulator->resetEmulator();
   emulator->startEmulation();
}

void MainWindow::on_actionDelta_Modulation_toggled(bool value)
{
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x10);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x10));
   }
}

void MainWindow::on_actionNoise_toggled(bool value)
{
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x08);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x08));
   }
}

void MainWindow::on_actionTriangle_toggled(bool value)
{
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x04);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x04));
   }
}

void MainWindow::on_actionSquare_2_toggled(bool value)
{
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x02);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x02));
   }
}

void MainWindow::on_actionSquare_1_toggled(bool value)
{
   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x01);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x01));
   }
}

void MainWindow::on_actionMute_All_toggled(bool value)
{
   ui->actionSquare_1->setChecked(!value);
   ui->actionSquare_2->setChecked(!value);
   ui->actionTriangle->setChecked(!value);
   ui->actionNoise->setChecked(!value);
   ui->actionDelta_Modulation->setChecked(!value);

   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x1F));
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x1F);
   }
}

void MainWindow::on_actionEnvironment_Settings_triggered()
{
   EnvironmentSettingsDialog* dlg = new EnvironmentSettingsDialog(this);

   if (dlg->exec() == QDialog::Accepted)
   {
      // Todo...
   }

   delete dlg;
}

void MainWindow::on_actionPreferences_triggered()
{
   EmulatorPrefsDialog* dlg =  new EmulatorPrefsDialog();

   if (dlg->exec() == QDialog::Accepted)
   {
      // Todo...
   }

   delete dlg;
}

void MainWindow::on_actionOnline_Help_triggered()
{
   ui->tabWidget->addTab(ui->tab,"Welcome Page");
   ui->webView->setUrl(QUrl( "http://wiki.nesicide.com/doku.php?id=nesicide_user_manual"));    
}

void MainWindow::on_actionRun_In_Emulator_triggered()
{
   if ( compiler->assembledOk() )
   {
      emulator->primeEmulator();
      emulator->resetEmulator();
      emulator->pauseEmulation(true);
//      emulator->startEmulation();
      
      ui->actionEmulation_Window->setChecked(true);
      on_actionEmulation_Window_toggled(true);
   }
    
}
