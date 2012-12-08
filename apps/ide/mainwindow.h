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
#include "searchdockwidget.h"

#include "ui_mainwindow.h"

namespace Ui
{
class MainWindow;
}

extern OutputPaneDockWidget* output;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
   Q_OBJECT
public:
   MainWindow(QWidget* parent = 0);
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
   SearchDockWidget* m_pSearch;

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
   QAction *actionPPUInformation_Inspector;
   QAction *actionJoypadLogger_Inspector;
   QAction *actionCodeDataLogger_Inspector;
   QAction *actionExecution_Visualizer_Inspector;
   QAction *actionMapperInformation_Inspector;
   QAction *actionAPUInformation_Inspector;
   QAction *actionPreferences;
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

   // Other
   int m_periodicTimer;

private:
   void openNesProject(QString fileName,bool runRom=true);
   void saveProject(QString fileName);
   void saveEmulatorState(QString fileName);
   void closeProject();
   void explodeTemplate(QString templateDirName,QString localDirName,QString* projectFileName);
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

private slots:
   void applicationActivationChanged(bool activated);
   void createTarget(QString target);
   void addStatusBarWidget(QWidget* widget);
   void removeStatusBarWidget(QWidget* widget);
   void setStatusBarMessage(QString message);
   void openNesROM(QString fileName,bool runRom=true);
   void openC64File(QString fileName);
   void on_actionAbout_Qt_triggered();
   void menuEdit_aboutToShow();
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
   void on_actionExecution_Inspector_toggled(bool );
   void on_actionSymbol_Watch_toggled(bool );
   void on_actionCode_Profiler_toggled(bool );
   void on_actionSearch_triggered(bool );
   void on_actionCompile_Project_triggered();
   void on_actionOutput_Window_toggled(bool );
   void on_actionSave_Active_Document_triggered();
   void on_tabWidget_currentChanged(int index);
   void on_actionOpen_Project_triggered();
   void on_action_Project_Browser_toggled(bool );
   void on_tabWidget_tabCloseRequested(int index);
   void on_actionCreate_Project_from_File_triggered();
   void on_actionNew_Project_triggered();
   void on_actionProject_Properties_triggered();
   void on_actionSave_Project_As_triggered();
   void on_actionSave_Project_triggered();
   void openFile(QString file);

   // Common/reused UI elements.
   void actionBreakpoint_Inspector_toggled(bool );
   void reflectedBreakpointInspector_close(bool toplevel);
   void actionAssembly_Inspector_toggled(bool );
   void reflectedAssemblyInspector_close(bool toplevel);
   void actionPreferences_triggered();

   // NES-specific UI elements.
   void actionFullscreen_toggled(bool value);
   void action1x_triggered();
   void action1_5x_triggered();
   void action2x_triggered();
   void action2_5x_triggered();
   void action3x_triggered();
   void actionLinear_Interpolation_toggled(bool );
   void action4_3_Aspect_toggled(bool );
   void actionRun_Test_Suite_triggered();
   void actionSquare_1_toggled(bool );
   void actionSquare_2_toggled(bool );
   void actionTriangle_toggled(bool );
   void actionNoise_toggled(bool );
   void actionDelta_Modulation_toggled(bool );
   void actionPulse_1VRC6_toggled(bool );
   void actionPulse_2VRC6_toggled(bool );
   void actionSawtoothVRC6_toggled(bool );
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
   void actionCodeDataLogger_Inspector_toggled(bool );
   void actionExecution_Visualizer_Inspector_toggled(bool );
   void actionGfxCHRMemory_Inspector_toggled(bool );
   void actionGfxOAMMemory_Inspector_toggled(bool );
   void actionGfxNameTableNESMemory_Inspector_toggled(bool );
   void actionBinCPURegister_Inspector_toggled(bool );
   void actionBinCPURAM_Inspector_toggled(bool );
   void actionBinROM_Inspector_toggled(bool );
   void actionBinNameTableNESMemory_Inspector_toggled(bool );
   void actionBinCHRMemory_Inspector_toggled(bool );
   void actionBinOAMMemory_Inspector_toggled(bool );
   void actionBinSRAMMemory_Inspector_toggled(bool );
   void actionBinEXRAMMemory_Inspector_toggled(bool );
   void actionBinPaletteNESMemory_Inspector_toggled(bool );
   void actionBinAPURegister_Inspector_toggled(bool );
   void actionBinPPURegister_Inspector_toggled(bool );
   void actionBinMapperMemory_Inspector_toggled(bool );
   void actionPPUInformation_Inspector_toggled(bool );
   void actionAPUInformation_Inspector_toggled(bool );
   void actionMapperInformation_Inspector_toggled(bool );
   void actionJoypadLogger_Inspector_toggled(bool );
   void actionEmulation_Window_toggled(bool );
   void reflectedEmulator_close(bool toplevel);
   void reflectedCodeDataLoggerInspector_close(bool toplevel);
   void reflectedExecutionInspector_close(bool toplevel);
   void reflectedExecutionVisualizer_Inspector_close(bool toplevel);
   void reflectedGfxCHRMemoryInspector_close(bool toplevel);
   void reflectedGfxNameTableMemoryInspector_close(bool toplevel);
   void reflectedGfxOAMMemoryInspector_close(bool toplevel);
   void reflectedBinCPURegisterInspector_close(bool toplevel);
   void reflectedBinCPURAMInspector_close(bool toplevel);
   void reflectedBinROMInspector_close(bool toplevel);
   void reflectedBinNameTableMemoryInspector_close(bool toplevel);
   void reflectedBinOAMMemoryInspector_close(bool toplevel);
   void reflectedBinCHRMemoryInspector_close(bool toplevel);
   void reflectedBinPaletteMemoryInspector_close(bool toplevel);
   void reflectedBinSRAMMemoryInspector_close(bool toplevel);
   void reflectedBinEXRAMMemoryInspector_close(bool toplevel);
   void reflectedBinPPURegisterInspector_close(bool toplevel);
   void reflectedBinAPURegisterInspector_close(bool toplevel);
   void reflectedBinMapperMemoryInspector_close(bool toplevel);
   void reflectedPPUInformationInspector_close(bool toplevel);
   void reflectedAPUInformationInspector_close(bool toplevel);
   void reflectedMapperInformationInspector_close(bool toplevel);
   void reflectedJoypadLoggerInspector_close(bool toplevel);
   void reflectedSymbol_Watch_close(bool toplevel);
   void reflectedCode_Profiler_close(bool toplevel);
   void reflectedSearch_close(bool toplevel);
   void reflectedOutput_Window_close(bool toplevel);
   void reflectedProjectBrowser_close(bool toplevel);

   // C64-specific UI elements.
   void actionBinSIDRegister_Inspector_toggled(bool value);
   void reflectedBinSIDRegisterInspector_close(bool toplevel);
};

#endif // MAINWINDOW_H
