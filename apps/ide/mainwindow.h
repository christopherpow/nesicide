#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "cnesicideproject.h"
#include "projectbrowserdockwidget.h"
#include "projectpropertiesdialog.h"
#include "newprojectdialog.h"
#include "cbuildertextlogger.h"
#include "ccartridgebuilder.h"
#include "chrmeminspector.h"
#include "oamvisualizerdockwidget.h"
#include "nametablevisualizerdockwidget.h"
#include "executioninspectordockwidget.h"
#include "executionvisualizerdockwidget.h"
#include "memoryinspectordockwidget.h"
#include "registerinspectordockwidget.h"
#include "breakpointdockwidget.h"
#include "codebrowserdockwidget.h"
#include "codedataloggerdockwidget.h"
#include "ppuinformationdockwidget.h"
#include "apuinformationdockwidget.h"
#include "mapperinformationdockwidget.h"
#include "symbolwatchdockwidget.h"
#include "aboutdialog.h"
#include "environmentsettingsdialog.h"
#include "outputpanedockwidget.h"
#include "emulatorprefsdialog.h"
#include "sourcenavigator.h"
#include "codeprofilerdockwidget.h"
#include "joypadloggerdockwidget.h"
#include "searchbar.h"
#include "nesemulatorthread.h"
#include "nesemulatordockwidget.h"
#include "nesemulatorcontrol.h"
#include "c64emulatorthread.h"
#include "c64emulatorcontrol.h"
#include "cexpandablestatusbar.h"

#include "ui_mainwindow.h"

#define MAX_RECENT_FILES 8

class CProjectModel;

namespace Ui
{
class MainWindow;
}

extern OutputPaneDockWidget* output;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
   Q_OBJECT
public:
   MainWindow(CProjectModel* projectModel, QWidget* parent = 0);
   virtual ~MainWindow();

protected:
   void changeEvent(QEvent* e);
   void dragEnterEvent ( QDragEnterEvent* event );
   void dragMoveEvent ( QDragMoveEvent* event );
   void dropEvent ( QDropEvent* event );
   void hideEvent(QHideEvent* event);
   void showEvent(QShowEvent* event);
   void keyPressEvent(QKeyEvent *event);

   // Target UI builders/destroyers.
   void createNesUi();
   void destroyNesUi();
   void createC64Ui();
   void destroyC64Ui();

   // Which target is loaded?
   QString m_targetLoaded;

   // Last activation time (for file-modified-outside-of-ide check)
   QDateTime m_lastActivationChangeTime;

   // Common/reused UI elements.
   CExpandableStatusBar* expandableStatusBar;
   QToolBar *debuggerToolBar;
   QAction *actionBreakpoint_Inspector;
   QAction *actionAssembly_Inspector;
   QMenu *menuCPU_Inspectors;
   QAction *actionBinCPURAM_Inspector;
   QAction *actionBinCPURegister_Inspector;
   RegisterInspectorDockWidget* m_pBinCPURegisterInspector;
   MemoryInspectorDockWidget* m_pBinCPURAMInspector;
   ExecutionInspectorDockWidget* m_pExecutionInspector;
   BreakpointDockWidget* m_pBreakpointInspector;
   CodeBrowserDockWidget* m_pAssemblyInspector;
   SymbolWatchDockWidget* m_pSymbolInspector;
   CodeProfilerDockWidget* m_pCodeProfiler;

   SourceNavigator* m_pSourceNavigator;
   SearchBar* m_pSearchBar;

   // NES-specific UI elements.
   NESEmulatorControl* m_pNESEmulatorControl;
   NESEmulatorThread* m_pNESEmulatorThread;
   NESEmulatorDockWidget* m_pNESEmulator;
   bool m_bEmulatorFloating;
   ExecutionVisualizerDockWidget* m_pExecutionVisualizer;
   CHRMEMInspector* m_pGfxCHRMemoryInspector;
   OAMVisualizerDockWidget* m_pGfxOAMMemoryInspector;
   NameTableVisualizerDockWidget* m_pGfxNameTableMemoryInspector;
   MemoryInspectorDockWidget* m_pBinROMInspector;
   MemoryInspectorDockWidget* m_pBinNameTableMemoryInspector;
   MemoryInspectorDockWidget* m_pBinCHRMemoryInspector;
   MemoryInspectorDockWidget* m_pBinSRAMMemoryInspector;
   MemoryInspectorDockWidget* m_pBinEXRAMMemoryInspector;
   MemoryInspectorDockWidget* m_pBinCartVRAMMemoryInspector;
   RegisterInspectorDockWidget* m_pBinPPURegisterInspector;
   RegisterInspectorDockWidget* m_pBinAPURegisterInspector;
   RegisterInspectorDockWidget* m_pBinOAMMemoryInspector;
   MemoryInspectorDockWidget* m_pBinPaletteMemoryInspector;
   RegisterInspectorDockWidget* m_pBinMapperMemoryInspector;
   CodeDataLoggerDockWidget* m_pCodeDataLoggerInspector;
   PPUInformationDockWidget* m_pPPUInformationInspector;
   APUInformationDockWidget* m_pAPUInformationInspector;
   MapperInformationDockWidget* m_pMapperInformationInspector;
   JoypadLoggerDockWidget* m_pJoypadLoggerInspector;
   QMenu *menuAPU_Inpsectors;
   QMenu *menuPPU_Inspectors;
   QMenu *menuI_O_Inspectors;
   QMenu *menuCartridge_Inspectors;
   QMenu *menuSystem;
   QMenu *menuAudio;
   QMenu *menuAudioMMC5;
   QMenu *menuAudioVRC6;
   QMenu *menuAudioN106;
   QMenu *menuVideo;
   QAction *actionEmulation_Window;
   QAction *actionGfxCHRMemory_Inspector;
   QAction *actionGfxOAMMemory_Inspector;
   QAction *actionGfxNameTableNESMemory_Inspector;
   QAction *actionBinNameTableNESMemory_Inspector;
   QAction *actionBinPPURegister_Inspector;
   QAction *actionBinAPURegister_Inspector;
   QAction *actionBinCHRMemory_Inspector;
   QAction *actionBinOAMMemory_Inspector;
   QAction *actionBinPaletteNESMemory_Inspector;
   QAction *actionBinSRAMMemory_Inspector;
   QAction *actionBinEXRAMMemory_Inspector;
   QAction *actionBinMapperMemory_Inspector;
   QAction *actionBinROM_Inspector;
   QAction *actionBinCartVRAMMemory_Inspector;
   QAction *actionPPUInformation_Inspector;
   QAction *actionJoypadLogger_Inspector;
   QAction *actionCodeDataLogger_Inspector;
   QAction *actionExecution_Visualizer_Inspector;
   QAction *actionMapperInformation_Inspector;
   QAction *actionAPUInformation_Inspector;
   QAction *actionConfigure;
   QAction *actionNTSC;
   QAction *actionPAL;
   QAction *actionDendy;
   QAction *actionSquare_1;
   QAction *actionSquare_2;
   QAction *actionTriangle;
   QAction *actionNoise;
   QAction *actionDelta_Modulation;
   QAction *actionPulse_1VRC6;
   QAction *actionPulse_2VRC6;
   QAction *actionSawtoothVRC6;
   QAction *actionSquare_1MMC5;
   QAction *actionSquare_2MMC5;
   QAction *actionDMCMMC5;
   QAction *actionWave_1N106;
   QAction *actionWave_2N106;
   QAction *actionWave_3N106;
   QAction *actionWave_4N106;
   QAction *actionWave_5N106;
   QAction *actionWave_6N106;
   QAction *actionWave_7N106;
   QAction *actionWave_8N106;
   QAction *actionRun_Test_Suite;
   QAction *action1x;
   QAction *action1_5x;
   QAction *action2x;
   QAction *action2_5x;
   QAction *action3x;
   QAction *actionLinear_Interpolation;
   QAction *action4_3_Aspect;
   QAction *actionFullscreen;

   // C64-specific UI elements.
   C64EmulatorControl* m_pC64EmulatorControl;
   C64EmulatorThread* m_pC64EmulatorThread;
   RegisterInspectorDockWidget* m_pBinSIDRegisterInspector;
   QMenu *menuSID_Inspectors;
   QAction *actionBinSIDRegister_Inspector;
   QStringList m_recentFiles;
   QMenu *m_menuRecentFiles;

   // Other
   int m_periodicTimer;

   // Project data wrappers
   CProjectModel* m_pProjectModel;

private:
   bool openAnyFile(QString fileName);
   void openNesProject(QString fileName,bool runRom=true);
   void openC64Project(QString fileName,bool run=true);
   void saveProject(QString fileName);
   void saveEmulatorState(QString fileName);
   bool closeProject();
   void explodeTemplate(int level,QString templateName,QString projectName,QString templateDirName,QString localDirName,QString* projectFileName);
   void explodeINESHeaderTemplate(QString templateName,QString projectName,QString templateDirName,QString localDirName);
   void updateFromEmulatorPrefs(bool initial);

protected:
   virtual void closeEvent ( QCloseEvent* event );
   virtual void timerEvent(QTimerEvent *event);

signals:
   void checkOpenFiles(QDateTime lastActivationTime);
   void applyProjectProperties();
   void applyEnvironmentSettings();
   void startEmulatorThread();
   void startEmulation();
   void primeEmulator();
   void pauseEmulation(bool show);
   void resetEmulator();
   void adjustAudio(int32_t length);
   void updateTargetMachine(QString target);
   void compile();
   void clean();

public slots:
   void openRecentFile();
   void saveRecentFiles(QString fileName);
   void updateRecentFiles();
   void on_actionExit_triggered();
   void applicationActivationChanged(bool activated);
   void createTarget(QString target);
   void addStatusBarWidget(QWidget* widget);
   void removeStatusBarWidget(QWidget* widget);
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void addPermanentStatusBarWidget(QWidget* widget);
   void removePermanentStatusBarWidget(QWidget* widget);
   void setStatusBarMessage(QString message);
   void openNesROM(QString fileName,bool runRom=true);
   void openC64File(QString fileName);
   void on_actionAbout_Qt_triggered();
   void menuEdit_aboutToShow();
   void menuWindow_aboutToShow();
   void focusEmulator();
   void windowMenu_triggered();
   void markProjectDirty(bool dirty);
   void on_actionClean_Project_triggered();
   void tabWidget_tabAdded(int tab);
   void tabWidget_tabModified(int tab,bool modified);
   void on_actionE_xit_triggered();
   void on_actionLoad_In_Emulator_triggered();
   void on_actionOnline_Help_triggered();
   void projectDataChangesEvent();
   void compiler_compileStarted();
   void compiler_compileDone(bool bOk);
   void on_action_Close_Project_triggered();
   void on_action_About_Nesicide_triggered();
   void on_actionEnvironment_Settings_triggered();
   void on_actionExecution_Inspector_triggered();
   void on_actionSymbol_Watch_triggered();
   void on_actionCode_Profiler_triggered();
   void on_actionSearch_triggered();
   void on_actionCompile_Project_triggered();
   void on_actionOutput_Window_triggered();
   void on_actionSave_Active_Document_triggered();
   void on_tabWidget_currentChanged(int index);
   void on_actionOpen_Project_triggered();
   void on_action_Project_Browser_triggered();
   void on_tabWidget_tabCloseRequested(int index);
   void on_actionCreate_Project_from_File_triggered();
   void on_actionNew_Project_triggered();
   void on_actionProject_Properties_triggered();
   void on_actionSave_Project_As_triggered();
   void on_actionSave_Project_triggered();
   void openFile(QString file);

   // Common/reused UI elements.
   void actionBreakpoint_Inspector_triggered();
   void actionAssembly_Inspector_triggered();
   void actionConfigure_triggered();

   // NES-specific UI elements.
   void actionFullscreen_toggled(bool value);
   void action1x_triggered();
   void action1_5x_triggered();
   void action2x_triggered();
   void action2_5x_triggered();
   void action3x_triggered();
   void actionLinear_Interpolation_toggled(bool );
   void action4_3_Aspect_toggled(bool );
   void actionSquare_1_toggled(bool );
   void actionSquare_2_toggled(bool );
   void actionTriangle_toggled(bool );
   void actionNoise_toggled(bool );
   void actionDelta_Modulation_toggled(bool );
   void actionPulse_1VRC6_toggled(bool );
   void actionPulse_2VRC6_toggled(bool );
   void actionSawtoothVRC6_toggled(bool );
   void actionSquare_1MMC5_toggled(bool );
   void actionSquare_2MMC5_toggled(bool );
   void actionDMCMMC5_toggled(bool );
   void actionWave_1N106_toggled(bool );
   void actionWave_2N106_toggled(bool );
   void actionWave_3N106_toggled(bool );
   void actionWave_4N106_toggled(bool );
   void actionWave_5N106_toggled(bool );
   void actionWave_6N106_toggled(bool );
   void actionWave_7N106_toggled(bool );
   void actionWave_8N106_toggled(bool );
   void actionPAL_triggered();
   void actionNTSC_triggered();
   void actionDendy_triggered();
   void actionRun_Test_Suite_triggered();
   void actionCodeDataLogger_Inspector_triggered();
   void actionExecution_Visualizer_Inspector_triggered();
   void actionGfxCHRMemory_Inspector_triggered();
   void actionGfxOAMMemory_Inspector_triggered();
   void actionGfxNameTableNESMemory_Inspector_triggered();
   void actionBinCPURegister_Inspector_triggered();
   void actionBinCPURAM_Inspector_triggered();
   void actionBinROM_Inspector_triggered();
   void actionBinCartVRAMMemory_Inspector_triggered();
   void actionBinNameTableNESMemory_Inspector_triggered();
   void actionBinCHRMemory_Inspector_triggered();
   void actionBinOAMMemory_Inspector_triggered();
   void actionBinSRAMMemory_Inspector_triggered();
   void actionBinEXRAMMemory_Inspector_triggered();
   void actionBinPaletteNESMemory_Inspector_triggered();
   void actionBinAPURegister_Inspector_triggered();
   void actionBinPPURegister_Inspector_triggered();
   void actionBinMapperMemory_Inspector_triggered();
   void actionPPUInformation_Inspector_triggered();
   void actionAPUInformation_Inspector_triggered();
   void actionMapperInformation_Inspector_triggered();
   void actionJoypadLogger_Inspector_triggered();
   void actionEmulation_Window_triggered();

   // C64-specific UI elements.
   void actionBinSIDRegister_Inspector_triggered();
   void on_actionCoding_Mode_triggered();
   void on_actionDebugging_Mode_triggered();
};

#endif // MAINWINDOW_H
