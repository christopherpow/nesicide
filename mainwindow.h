#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "cnesicideproject.h"
#include "projectbrowserdockwidget.h"
#include "projectpropertiesdialog.h"
#include "newprojectdialog.h"
#include "nesemulatordockwidget.h"
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
#include "aboutdialog.h"
#include "environmentsettingsdialog.h"
#include "outputpanedockwidget.h"
#include "emulatorprefsdialog.h"
#include "sourcenavigator.h"

namespace Ui
{
class MainWindow;
}

extern OutputPaneDockWidget* output;

class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
   MainWindow(QWidget* parent = 0);
   ~MainWindow();
   NESEmulatorDockWidget* emulatorDlg;
   void openROM(QString fileName);
protected:
   void changeEvent(QEvent* e);
   void dragEnterEvent ( QDragEnterEvent* event );
   void dragMoveEvent ( QDragMoveEvent* event );
   void dropEvent ( QDropEvent* event );
   void hideEvent(QHideEvent* event);
   void showEvent(QShowEvent* event);

private:
   Ui::MainWindow* ui;
   IProjectTreeViewItem* matchTab(QWidget* pTab);
   QString projectFileName;
   void openProject(QString fileName);
   void saveProject();

   // Debugger inspector dockables
   ExecutionInspectorDockWidget* m_pExecutionInspector;
   ExecutionVisualizerDockWidget* m_pExecutionVisualizer;
   RegisterInspectorDockWidget* m_pBinCPURegisterInspector;
   CHRMEMInspector* m_pGfxCHRMemoryInspector;
   OAMVisualizerDockWidget* m_pGfxOAMMemoryInspector;
   NameTableVisualizerDockWidget* m_pGfxNameTableMemoryInspector;
   MemoryInspectorDockWidget* m_pBinCPURAMInspector;
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
   BreakpointDockWidget* m_pBreakpointInspector;
   CodeBrowserDockWidget* m_pCodeInspector;
   CodeDataLoggerDockWidget* m_pCodeDataLoggerInspector;
   PPUInformationDockWidget* m_pPPUInformationInspector;
   APUInformationDockWidget* m_pAPUInformationInspector;
   MapperInformationDockWidget* m_pMapperInformationInspector;

   SourceNavigator* m_pSourceNavigator;

protected:
   virtual void closeEvent ( QCloseEvent* event );

private slots:
   void on_actionRun_Test_Suite_triggered();
   void on_actionLoad_In_Emulator_triggered();
   void on_actionOnline_Help_triggered();
   void projectDataChangesEvent();
   void on_actionPreferences_triggered();
   void compiler_compileStarted();
   void compiler_compileDone(bool bOk);
   void on_actionEnvironment_Settings_triggered();
   void on_actionMute_All_toggled(bool );
   void on_actionSquare_1_toggled(bool );
   void on_actionSquare_2_toggled(bool );
   void on_actionTriangle_toggled(bool );
   void on_actionNoise_toggled(bool );
   void on_actionDelta_Modulation_toggled(bool );
   void on_actionPAL_triggered();
   void on_actionNTSC_triggered();
   void handle_MainWindow_destroyed();
   void on_action_Close_Project_triggered();
   void on_action_About_Nesicide_triggered();
   void on_actionCode_Inspector_toggled(bool );
   void on_actionCodeDataLogger_Inspector_toggled(bool );
   void on_actionExecution_Inspector_toggled(bool );
   void on_actionExecution_Visualizer_Inspector_toggled(bool );
   void on_actionBreakpoint_Inspector_toggled(bool );
   void on_actionGfxCHRMemory_Inspector_toggled(bool );
   void on_actionGfxOAMMemory_Inspector_toggled(bool );
   void on_actionGfxNameTableMemory_Inspector_toggled(bool );
   void on_actionBinCPURegister_Inspector_toggled(bool );
   void on_actionBinCPURAM_Inspector_toggled(bool );
   void on_actionBinROM_Inspector_toggled(bool );
   void on_actionBinNameTableMemory_Inspector_toggled(bool );
   void on_actionBinCHRMemory_Inspector_toggled(bool );
   void on_actionBinOAMMemory_Inspector_toggled(bool );
   void on_actionBinSRAMMemory_Inspector_toggled(bool );
   void on_actionBinEXRAMMemory_Inspector_toggled(bool );
   void on_actionBinPaletteMemory_Inspector_toggled(bool );
   void on_actionBinAPURegister_Inspector_toggled(bool );
   void on_actionBinPPURegister_Inspector_toggled(bool );
   void on_actionBinMapperMemory_Inspector_toggled(bool );
   void on_actionPPUInformation_Inspector_toggled(bool );
   void on_actionAPUInformation_Inspector_toggled(bool );
   void on_actionMapperInformation_Inspector_toggled(bool );
   void reflectedEmulator_close(bool toplevel);
   void reflectedCodeInspector_close(bool toplevel);
   void reflectedCodeDataLoggerInspector_close(bool toplevel);
   void reflectedExecutionInspector_close(bool toplevel);
   void reflectedExecutionVisualizer_Inspector_close(bool toplevel);
   void reflectedBreakpointInspector_close(bool toplevel);
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
   void reflectedOutput_Window_close(bool toplevel);
   void reflectedProjectBrowser_close(bool toplevel);
   void on_actionCompile_Project_triggered();
   void on_actionOutput_Window_toggled(bool );
   void on_actionSave_Active_Document_triggered();
   void on_tabWidget_currentChanged(int index);
   void on_actionOpen_Project_triggered();
   void on_actionEmulation_Window_toggled(bool );
   void on_action_Project_Browser_toggled(bool );
   void on_tabWidget_tabCloseRequested(int index);
   void on_actionCreate_Project_from_ROM_triggered();
   void on_actionNew_Project_triggered();
   void on_actionProject_Properties_triggered();
   void on_actionSave_Project_As_triggered();
   void on_actionSave_Project_triggered();
};

#endif // MAINWINDOW_H
