#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "cnesicideproject.h"
#include "cprojecttreeviewmodel.h"
#include "projectpropertiesdialog.h"
#include "newprojectdialog.h"
#include "nesemulatordialog.h"
#include "cbuildertextlogger.h"
#include "ccartridgebuilder.h"
#include "chrmeminspector.h"
#include "oaminspector.h"
#include "nametableinspector.h"
#include "executioninspector.h"
#include "memoryinspector.h"
#include "registerinspector.h"
#include "breakpointinspector.h"
#include "codeinspector.h"
#include "ppuinformationinspector.h"
#include "aboutdialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    NESEmulatorDialog *emulatorDlg;
    int emulatorDlgTabIdx;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    void projectDataChangesEvent();
    CProjectTreeViewModel *projectTreeviewModel;
    IProjectTreeViewItem *matchTab(IProjectTreeViewItem *root, int tabIndex);
    QString projectFileName;
    void saveProject(QString fileName);

    // Debugger inspector dockables
    ExecutionInspector* m_pExecutionInspector;
    RegisterInspector* m_pBinCPURegisterInspector;
    CHRMEMInspector* m_pGfxCHRMemoryInspector;
    OAMInspector* m_pGfxOAMMemoryInspector;
    NameTableInspector* m_pGfxNameTableMemoryInspector;
    MemoryInspector* m_pBinCPURAMInspector;
    MemoryInspector* m_pBinROMInspector;
    MemoryInspector* m_pBinNameTableMemoryInspector;
    MemoryInspector* m_pBinCHRMemoryInspector;
    MemoryInspector* m_pBinSRAMMemoryInspector;
    MemoryInspector* m_pBinEXRAMMemoryInspector;
    RegisterInspector* m_pBinPPURegisterInspector;
    RegisterInspector* m_pBinAPURegisterInspector;
    RegisterInspector* m_pBinOAMMemoryInspector;
    MemoryInspector* m_pBinPaletteMemoryInspector;
    RegisterInspector* m_pBinMapperMemoryInspector;
    BreakpointInspector* m_pBreakpointInspector;
    CodeInspector* m_pCodeInspector;
    PPUInformationInspector* m_pPPUInformationInspector;

protected:
    virtual void closeEvent ( QCloseEvent * event );

private slots:
    void on_MainWindow_destroyed();
    void on_actionEmulation_Window_triggered();
    void on_action_Close_Project_triggered();
    void on_action_About_Nesicide_triggered();
    void on_actionCode_Inspector_toggled(bool );
    void on_actionExecution_Inspector_toggled(bool );
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
    void reflectedCodeInspector_close(bool toplevel);
    void reflectedExecutionInspector_close(bool toplevel);
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
    void on_actionCompile_Project_triggered();
    void on_actionCompiler_Output_toggled(bool );
    void on_compilerOutputDockWidget_visibilityChanged(bool visible);
    void on_actionSave_Active_Document_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionOpen_Project_triggered();
    void on_actionEmulation_Window_toggled(bool );
    void on_action_Project_Browser_toggled(bool );
    void on_projectBrowserDockWidget_visibilityChanged(bool visible);
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionCreate_Project_from_ROM_triggered();
    void on_actionNew_Project_triggered();
    void on_actionProject_Properties_triggered();
    void on_actionSave_Project_As_triggered();
    void on_actionSave_Project_triggered();
};

#endif // MAINWINDOW_H
