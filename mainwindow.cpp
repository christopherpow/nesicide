#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "inspectorregistry.h"
#include "main.h"

#include <QApplication>
#include <QStringList>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    nesicideProject = new CNesicideProject();
    pluginManager = new CPluginManager();

    ui->setupUi(this);
    projectTreeviewModel = new CProjectTreeViewModel(ui->projectTreeWidget, nesicideProject);
    ui->projectTreeWidget->mdiTabWidget = ui->tabWidget;
    ui->projectTreeWidget->setModel(projectTreeviewModel);

    emulatorDlg = new NESEmulatorDialog();
    emulator->setDialog ( emulatorDlg );
    emulatorDlgTabIdx = -1;

    breakpointWatcher->setDialog ( emulatorDlg );

    projectDataChangesEvent();

    m_pBreakpointInspector = new BreakpointInspector ();
    m_pBreakpointInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBreakpointInspector->setWindowTitle("Breakpoints");
    m_pBreakpointInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
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

    m_pGfxOAMMemoryInspector = new OAMInspector ();
    m_pGfxOAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pGfxOAMMemoryInspector->setWindowTitle("OAM Memory Visualizer");
    m_pGfxOAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pGfxOAMMemoryInspector );
    m_pGfxOAMMemoryInspector->hide();
    QObject::connect(m_pGfxOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxOAMMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "OAM Memory Visualizer", m_pGfxOAMMemoryInspector );

    m_pGfxNameTableMemoryInspector = new NameTableInspector ();
    m_pGfxNameTableMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pGfxNameTableMemoryInspector->setWindowTitle("Name Table Visualizer");
    m_pGfxNameTableMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_pGfxNameTableMemoryInspector );
    m_pGfxNameTableMemoryInspector->hide();
    QObject::connect(m_pGfxNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxNameTableMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Name Table Visualizer", m_pGfxNameTableMemoryInspector );

    m_pExecutionInspector = new ExecutionInspector();
    m_pExecutionInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pExecutionInspector->setWindowTitle("Execution Inspector");
    m_pExecutionInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionInspector );
    m_pExecutionInspector->hide();
    QObject::connect(m_pExecutionInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionInspector_close(bool)));
    InspectorRegistry::addInspector ( "Execution Inspector", m_pExecutionInspector );

    m_pExecutionVisualizer = new ExecutionVisualizer();
    m_pExecutionVisualizer->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pExecutionVisualizer->setWindowTitle("Execution Visualizer");
    m_pExecutionVisualizer->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionVisualizer );
    m_pExecutionVisualizer->hide();
    QObject::connect(m_pExecutionVisualizer, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionVisualizer_Inspector_close(bool)));
    InspectorRegistry::addInspector ( "Execution Visualizer", m_pExecutionVisualizer );

    m_pCodeInspector = new CodeInspector();
    m_pCodeInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pCodeInspector->setWindowTitle("Code Browser");
    m_pCodeInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_pCodeInspector );
    m_pCodeInspector->hide();
    QObject::connect(m_pCodeInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCodeInspector_close(bool)));
    InspectorRegistry::addInspector ( "Code Browser", m_pCodeInspector );

    m_pCodeDataLoggerInspector = new CodeDataLoggerInspector();
    m_pCodeDataLoggerInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pCodeDataLoggerInspector->setWindowTitle("Code/Data Logger Inspector");
    m_pCodeDataLoggerInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_pCodeDataLoggerInspector );
    m_pCodeDataLoggerInspector->hide();
    QObject::connect(m_pCodeDataLoggerInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedCodeDataLoggerInspector_close(bool)));
    InspectorRegistry::addInspector ( "Code/Data Logger Inspector", m_pCodeDataLoggerInspector );

    m_pBinCPURegisterInspector = new RegisterInspector(eMemory_CPUregs);
    m_pBinCPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
    m_pBinCPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
    m_pBinCPURegisterInspector->hide();
    QObject::connect(m_pBinCPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURegisterInspector_close(bool)));
    InspectorRegistry::addInspector ( "CPU Register Inspector", m_pBinCPURegisterInspector );

    m_pBinCPURAMInspector = new MemoryInspector(eMemory_CPU);
    m_pBinCPURAMInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCPURAMInspector->setWindowTitle("CPU RAM Inspector");
    m_pBinCPURAMInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURAMInspector );
    m_pBinCPURAMInspector->hide();
    QObject::connect(m_pBinCPURAMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURAMInspector_close(bool)));
    InspectorRegistry::addInspector ( "CPU RAM Inspector", m_pBinCPURAMInspector );

    m_pBinROMInspector = new MemoryInspector(eMemory_cartROM);
    m_pBinROMInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinROMInspector->setWindowTitle("PRG-ROM Inspector");
    m_pBinROMInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinROMInspector );
    m_pBinROMInspector->hide();
    QObject::connect(m_pBinROMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinROMInspector_close(bool)));
    InspectorRegistry::addInspector ( "PRG-ROM Inspector", m_pBinROMInspector );

    m_pBinNameTableMemoryInspector = new MemoryInspector(eMemory_PPU);
    m_pBinNameTableMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinNameTableMemoryInspector->setWindowTitle("Name Table Inspector");
    m_pBinNameTableMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinNameTableMemoryInspector );
    m_pBinNameTableMemoryInspector->hide();
    QObject::connect(m_pBinNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinNameTableMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Name Table Inspector", m_pBinNameTableMemoryInspector );

    m_pBinPPURegisterInspector = new RegisterInspector(eMemory_PPUregs);
    m_pBinPPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinPPURegisterInspector->setWindowTitle("PPU Register Inspector");
    m_pBinPPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPURegisterInspector );
    m_pBinPPURegisterInspector->hide();
    QObject::connect(m_pBinPPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPPURegisterInspector_close(bool)));
    InspectorRegistry::addInspector ( "PPU Register Inspector", m_pBinPPURegisterInspector );

    m_pPPUInformationInspector = new PPUInformationInspector();
    m_pPPUInformationInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pPPUInformationInspector->setWindowTitle("PPU Information");
    m_pPPUInformationInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pPPUInformationInspector );
    m_pPPUInformationInspector->hide();
    QObject::connect(m_pPPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedPPUInformationInspector_close(bool)));
    InspectorRegistry::addInspector ( "PPU Information", m_pPPUInformationInspector );

    m_pBinAPURegisterInspector = new RegisterInspector(eMemory_IOregs);
    m_pBinAPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinAPURegisterInspector->setWindowTitle("APU Register Inspector");
    m_pBinAPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinAPURegisterInspector );
    m_pBinAPURegisterInspector->hide();
    QObject::connect(m_pBinAPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinAPURegisterInspector_close(bool)));
    InspectorRegistry::addInspector ( "APU Register Inspector", m_pBinAPURegisterInspector );

    m_pAPUInformationInspector = new APUInformationInspector();
    m_pAPUInformationInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pAPUInformationInspector->setWindowTitle("APU Information");
    m_pAPUInformationInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pAPUInformationInspector );
    m_pAPUInformationInspector->hide();
    QObject::connect(m_pAPUInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedAPUInformationInspector_close(bool)));
    InspectorRegistry::addInspector ( "APU Information", m_pAPUInformationInspector );

    m_pBinCHRMemoryInspector = new MemoryInspector(eMemory_cartCHRMEM);
    m_pBinCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCHRMemoryInspector->setWindowTitle("CHR Memory Inspector");
    m_pBinCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCHRMemoryInspector );
    m_pBinCHRMemoryInspector->hide();
    QObject::connect(m_pBinCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCHRMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "CHR Memory Inspector", m_pBinCHRMemoryInspector );

    m_pBinOAMMemoryInspector = new RegisterInspector(eMemory_PPUoam);
    m_pBinOAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinOAMMemoryInspector->setWindowTitle("OAM Memory Inspector");
    m_pBinOAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinOAMMemoryInspector );
    m_pBinOAMMemoryInspector->hide();
    QObject::connect(m_pBinOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinOAMMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "OAM Memory Inspector", m_pBinOAMMemoryInspector );

    m_pBinPaletteMemoryInspector = new MemoryInspector(eMemory_PPUpalette);
    m_pBinPaletteMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinPaletteMemoryInspector->setWindowTitle("Palette Memory Inspector");
    m_pBinPaletteMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinPaletteMemoryInspector );
    m_pBinPaletteMemoryInspector->hide();
    QObject::connect(m_pBinPaletteMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPaletteMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Palette Memory Inspector", m_pBinPaletteMemoryInspector );

    m_pBinSRAMMemoryInspector = new MemoryInspector(eMemory_cartSRAM);
    m_pBinSRAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinSRAMMemoryInspector->setWindowTitle("Cartridge SRAM Memory Inspector");
    m_pBinSRAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinSRAMMemoryInspector );
    m_pBinSRAMMemoryInspector->hide();
    QObject::connect(m_pBinSRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinSRAMMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Cartridge SRAM Memory Inspector", m_pBinSRAMMemoryInspector );

    m_pBinEXRAMMemoryInspector = new MemoryInspector(eMemory_cartEXRAM);
    m_pBinEXRAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinEXRAMMemoryInspector->setWindowTitle("Cartridge EXRAM Memory Inspector");
    m_pBinEXRAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinEXRAMMemoryInspector );
    m_pBinEXRAMMemoryInspector->hide();
    QObject::connect(m_pBinEXRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinEXRAMMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Cartridge EXRAM Memory Inspector", m_pBinEXRAMMemoryInspector );

    m_pMapperInformationInspector = new MapperInformationInspector();
    m_pMapperInformationInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pMapperInformationInspector->setWindowTitle("Cartridge Mapper Information");
    m_pMapperInformationInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pMapperInformationInspector );
    m_pMapperInformationInspector->hide();
    QObject::connect(m_pMapperInformationInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedMapperInformationInspector_close(bool)));
    InspectorRegistry::addInspector ( "Cartridge Mapper Information", m_pMapperInformationInspector );

    m_pBinMapperMemoryInspector = new RegisterInspector(eMemory_cartMapper);
    m_pBinMapperMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinMapperMemoryInspector->setWindowTitle("Cartridge Mapper Register Inspector");
    m_pBinMapperMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinMapperMemoryInspector );
    m_pBinMapperMemoryInspector->hide();
    QObject::connect(m_pBinMapperMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinMapperMemoryInspector_close(bool)));
    InspectorRegistry::addInspector ( "Cartridge Mapper Register Inspector", m_pBinMapperMemoryInspector );

    builderTextLogger.setTextEditControl(ui->compilerOutputTextEdit);
    builderTextLogger.write("<strong>NESICIDE2</strong> Alpha Release");
    builderTextLogger.write("<strong>Plugin Scripting Subsystem:</strong> " + pluginManager->getVersionInfo());

   // Start in NTSC mode for now until we can have it configurable on app entry.
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);

   // Start with all sound channels enabled...
   ui->actionSquare_1->setChecked(true);
   ui->actionSquare_2->setChecked(true);
   ui->actionTriangle->setChecked(true);
   ui->actionNoise->setChecked(true);
   ui->actionDelta_Modulation->setChecked(true);
   ui->actionMute_All->setChecked(false);
   ui->actionNTSC->setChecked(true);
   CNES::VIDEOMODE(MODE_NTSC);

   this->ui->webView->setUrl(QUrl( "http://wiki.nesicide.com/doku.php?id=nesicide_user_manual"));

   QStringList sl_raw = QApplication::arguments();
   QStringList sl_nes = sl_raw.filter ( ".nes" );
   if ( sl_nes.count() >= 1 )
   {
      emulator->pauseEmulation(false);

      nesicideProject->createProjectFromRom(sl_nes.at(0));
      ui->actionEmulation_Window->setChecked(true);
      on_actionEmulation_Window_toggled(true);
      projectDataChangesEvent();

      emulator->resetEmulator();
      emulator->startEmulation();

      if ( sl_nes.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NES ROM files were specified on the command\n"
                                                             "line.  Only the first NES ROM was opened, all others\n"
                                                             "were ignored." );
      }
   }

   // Always call this last
   pluginManager->doInitScript();
}

MainWindow::~MainWindow()
{
    ui->tabWidget->clear();

    delete nesicideProject;
    delete pluginManager;
    delete ui;
    delete projectTreeviewModel;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
   event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
   event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
   QStringList sl = event->mimeData()->formats();
   QByteArray ba = event->mimeData()->data(sl.at(6));

   emulator->pauseEmulation(false);

   nesicideProject->createProjectFromRom(QString(ba));
   ui->actionEmulation_Window->setChecked(true);
   on_actionEmulation_Window_toggled(true);
   projectDataChangesEvent();

   emulator->resetEmulator();
   emulator->startEmulation();

   event->acceptProposedAction();
}

void MainWindow::projectDataChangesEvent()
{
    projectTreeviewModel->layoutChangedEvent();

    // Enabled/Disable actions based on if we have a project loaded or not
    ui->actionNew_Project->setEnabled(!nesicideProject->get_isInitialized());
    ui->actionCreate_Project_from_ROM->setEnabled(!nesicideProject->get_isInitialized());
    ui->actionOpen_Project->setEnabled(!nesicideProject->get_isInitialized());
    ui->action_Close_Project->setEnabled(nesicideProject->get_isInitialized());
    ui->actionCompile_Project->setEnabled(nesicideProject->get_isInitialized());
    ui->actionProject_Properties->setEnabled(nesicideProject->get_isInitialized());
    ui->actionSave_Project->setEnabled(nesicideProject->get_isInitialized());
    ui->actionSave_Project_As->setEnabled(nesicideProject->get_isInitialized());
    ui->actionEmulation_Window->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionExecution_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionExecution_Visualizer_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBreakpoint_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionCode_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionCodeDataLogger_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxCHRMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxOAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxNameTableMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCPURAM_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinROM_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinNameTableMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCHRMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinOAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinPaletteMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinSRAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinEXRAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinPPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinAPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinMapperMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionPPUInformation_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionAPUInformation_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionMapperInformation_Inspector->setEnabled ( nesicideProject->get_isInitialized() );

    if (ui->tabWidget->currentIndex() >= 0)
    {
        IProjectTreeViewItem *projectItem = matchTab(nesicideProject, ui->tabWidget->currentIndex());
        if (projectItem)
        {
            ui->actionSave_Active_Document->setEnabled(((IProjectTreeViewItem *)projectItem)->isDocumentSaveable());
        } else {
            ui->actionSave_Active_Document->setEnabled(false);
        }
    }

    // set up emulator if it needs to be...
    emulator->primeEmulator ();
}

void MainWindow::on_actionSave_Project_triggered()
{
    if (projectFileName.isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                        QString("NESICIDE2 Project (*.nesproject)"));
        if (!fileName.isEmpty())
            saveProject(fileName);
    }
    else
        saveProject(projectFileName);
}

void MainWindow::saveProject(QString fileName)
{
    QFile file(fileName);
    if( !file.open( QFile::WriteOnly) )
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

    projectFileName = fileName;
}

void MainWindow::on_actionSave_Project_As_triggered()
{
    // Allow the user to select a file name. Note that using the static function produces a native
    // file dialog, while creating an instance of QFileDialog results in a non-native file dialog..
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                    QString("NESICIDE2 Project (*.nesproject)"));
    if (!fileName.isEmpty())
        saveProject(fileName);
}

void MainWindow::on_actionProject_Properties_triggered()
{
    ProjectPropertiesDialog *dlg = new ProjectPropertiesDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->set_projectTitle(dlg->getProjectName());
        nesicideProject->get_pointerToListOfProjectPaletteEntries()->clear();
        for (int paletteItemIndex=0; paletteItemIndex<dlg->currentPalette.count(); paletteItemIndex++)
            nesicideProject->get_pointerToListOfProjectPaletteEntries()->append(dlg->currentPalette.at(paletteItemIndex));
        projectDataChangesEvent();

        for (int sourceIndex = 0; sourceIndex < nesicideProject->getProject()->getSources()->childCount(); sourceIndex++)
        {
            CSourceItem *sourceItem = (CSourceItem *)nesicideProject->getProject()->getSources()->child(sourceIndex);
            if (sourceItem->get_sourceName() == dlg->getMainSource())
            {
                nesicideProject->getProject()->setMainSource(sourceItem);
            }
        }
        this->setWindowTitle(nesicideProject->get_projectTitle().prepend("NESICIDE - "));
    }
    delete dlg;
}

void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog *dlg = new NewProjectDialog();
    if (dlg->exec() == QDialog::Accepted)
    {
        ui->projectTreeWidget->setModel(NULL);
        nesicideProject->set_projectTitle(dlg->getProjectTitle());
        nesicideProject->initializeProject();
        ui->projectTreeWidget->setModel(projectTreeviewModel);
        projectDataChangesEvent();
        this->setWindowTitle(nesicideProject->get_projectTitle().prepend("NESICIDE - "));

    }

    delete dlg;
}

void MainWindow::on_actionCreate_Project_from_ROM_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "iNES ROM (*.nes)");
   if (fileName.isEmpty())
      return;

   emulator->pauseEmulation(false);
   nesicideProject->createProjectFromRom(fileName);
   ui->actionEmulation_Window->setChecked(true);
   on_actionEmulation_Window_toggled(true);
   projectDataChangesEvent();
   this->setWindowTitle(nesicideProject->get_projectTitle().prepend("NESICIDE - "));

   emulator->resetEmulator();
   emulator->startEmulation();
}

IProjectTreeViewItem *MainWindow::matchTab(IProjectTreeViewItem *root, int tabIndex)
{
    for (int treeviewItemIndex = 0;
         treeviewItemIndex < ((IProjectTreeViewItem *)root)->childCount(); treeviewItemIndex++)
    {
        IProjectTreeViewItem *item = ((IProjectTreeViewItem *)root)->child(treeviewItemIndex);
        if (!item)
            continue;

        if (((IProjectTreeViewItem *)item)->getTabIndex() == tabIndex)
            return item;

        if (((IProjectTreeViewItem *)item)->childCount() > 0)
        {
            IProjectTreeViewItem *result = matchTab(item, tabIndex);
            if (result)
                return result;
        }
    }

    return (IProjectTreeViewItem *)NULL;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, index);
    if (projectItem)
    {
        if (((IProjectTreeViewItem *)projectItem)->onCloseQuery())
        {
            ui->tabWidget->removeTab(index);
            ((IProjectTreeViewItem *)projectItem)->onClose();
        }
    } else {
        ui->tabWidget->removeTab(index);
        if (index == emulatorDlgTabIdx)
        {
            emulator->pauseEmulation(false);
            ui->actionEmulation_Window->setChecked(false);
            emulatorDlgTabIdx = -1;
        }
    }
}

void MainWindow::on_projectBrowserDockWidget_visibilityChanged(bool visible)
{
    if (!visible)
    {
        if (!ui->projectBrowserDockWidget->isVisibleTo(this))
        {
            ui->action_Project_Browser->setChecked(false);
        }
    } else
        ui->action_Project_Browser->setChecked(visible);
}

void MainWindow::on_action_Project_Browser_toggled(bool visible)
{
    ui->projectBrowserDockWidget->setVisible(visible);
}

void MainWindow::on_actionEmulation_Window_toggled(bool value)
{
    if (value)
    {
        emulatorDlgTabIdx = ui->tabWidget->addTab(emulatorDlg, "Emulation Window");
        ui->tabWidget->setCurrentIndex(emulatorDlgTabIdx);
    }
    else
    {
        ui->tabWidget->removeTab(emulatorDlgTabIdx);
        emulatorDlgTabIdx = -1;
    }
}

void MainWindow::closeEvent ( QCloseEvent * event )
{
   emulator->pauseEmulation(false);

   QWidget::closeEvent(event);
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "NESICIDE2 Project (*.nesproject)");
    if (QFile::exists(fileName))
    {
        QDomDocument doc;
        QFile file( fileName );
        if (!file.open(QFile::ReadOnly))
        {
            QMessageBox::critical(this, "Error", "Failed to open the project file.");
            return;
        }

        if(!doc.setContent(file.readAll()))
        {
            QMessageBox::critical(this, "Error", "Failed to parse the project xml data.");
            file.close();
            return;
        }
        file.close();

        ui->projectTreeWidget->setModel(NULL);

        nesicideProject->deserialize(doc, doc);
        ui->projectTreeWidget->setModel(projectTreeviewModel);

        while (ui->tabWidget->currentIndex() >= 0)
            ui->tabWidget->removeTab(0);

        projectDataChangesEvent();
        projectFileName = fileName;

        this->setWindowTitle(nesicideProject->get_projectTitle().prepend("NESICIDE - "));
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, index);
    if (projectItem)
    {
        ui->actionSave_Active_Document->setEnabled(((IProjectTreeViewItem *)projectItem)->isDocumentSaveable());
    } else {
        ui->actionSave_Active_Document->setEnabled(false);
    }
}

void MainWindow::on_actionSave_Active_Document_triggered()
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, ui->tabWidget->currentIndex());
    if (projectItem)
        ((IProjectTreeViewItem *)projectItem)->onSaveDocument();

}

void MainWindow::on_compilerOutputDockWidget_visibilityChanged(bool visible)
{
    if (!visible)
    {
        if (!ui->compilerOutputTextEdit->isVisibleTo(this))
        {
            ui->actionCompiler_Output->setChecked(false);
        }
    } else
        ui->actionCompiler_Output->setChecked(visible);
}

void MainWindow::on_actionCompiler_Output_toggled(bool value)
{
    ui->compilerOutputDockWidget->setVisible(value);
}


void MainWindow::on_actionCompile_Project_triggered()
{
    CCartridgeBuilder cartridgeBuilder;
    cartridgeBuilder.build();
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
    aboutdialog *dlg = new aboutdialog(this);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_action_Close_Project_triggered()
{
    // Stop the emulator if it is running
    emulator->pauseEmulation(false);
    if (emulatorDlgTabIdx > -1) {
        ui->actionEmulation_Window->toggle();
        emulatorDlgTabIdx = -1;
    }

    // Terminate the project and let the IDE know
    nesicideProject->terminateProject();

    // Remove any tabs
    ui->tabWidget->clear();

    // Close all inspectors
    InspectorRegistry::hideAll();

    // Let the UI know what's up
    projectDataChangesEvent();
    this->setWindowTitle("NESICIDE");
}

void MainWindow::on_actionEmulation_Window_triggered()
{

}

void MainWindow::on_MainWindow_destroyed()
{
    if (nesicideProject->get_isInitialized())
        on_action_Close_Project_triggered();
}

void MainWindow::on_actionNTSC_triggered(bool checked)
{
   checked = checked;
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);
   CNES::VIDEOMODE(MODE_NTSC);

   emulator->resetEmulator();
   emulator->startEmulation();
}

void MainWindow::on_actionPAL_triggered(bool checked)
{
   checked = checked;
   ui->actionNTSC->setChecked(false);
   ui->actionPAL->setChecked(true);
   CNES::VIDEOMODE(MODE_PAL);

   emulator->resetEmulator();
   emulator->startEmulation();
}

void MainWindow::on_actionDelta_Modulation_toggled(bool value)
{
   if ( value )
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x10 );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x10) );
   }
}

void MainWindow::on_actionNoise_toggled(bool value)
{
   if ( value )
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x08 );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x08) );
   }
}

void MainWindow::on_actionTriangle_toggled(bool value)
{
   if ( value )
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x04 );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x04) );
   }
}

void MainWindow::on_actionSquare_2_toggled(bool value)
{
   if ( value )
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x02 );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x02) );
   }
}

void MainWindow::on_actionSquare_1_toggled(bool value)
{
   if ( value )
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x01 );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x01) );
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
      CAPU::SET4015MASK ( CAPU::GET4015MASK()&(~0x1F) );
   }
   else
   {
      CAPU::SET4015MASK ( CAPU::GET4015MASK()|0x1F );
   }
}

void MainWindow::on_actionEnvironment_Settings_triggered()
{
    EnvironmentSettingsDialog *dlg = new EnvironmentSettingsDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        // Todo...
    }
    delete dlg;
}
