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
#include "ccartridgebuilder.h"

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
    CHRMEMInspector* m_pCHRMEMInspector;
    OAMInspector* m_pOAMInspector;
    NameTableInspector* m_pNameTableInspector;
    ExecutionInspector* m_pExecutionInspector;
    MemoryInspector* m_pCPUMemoryInspector;
    MemoryInspector* m_pPPUMemoryInspector;
    MemoryInspector* m_pCHRMemoryInspector;
    MemoryInspector* m_pPPURegisterInspector;
    MemoryInspector* m_pAPURegisterInspector;
    void saveProject(QString fileName);

protected:
    virtual void closeEvent ( QCloseEvent * event );

private slots:
    void on_actionAPURegister_Inspector_toggled(bool );
    void on_actionPPURegister_Inspector_toggled(bool );
    void oamInspector_close(bool toplevel);
    void ntInspector_close(bool toplevel);
    void gfxchrmemInspector_close(bool toplevel);
    void binchrmemInspector_close(bool toplevel);
    void exeInspector_close(bool toplevel);
    void cpumemInspector_close(bool toplevel);
    void ppumemInspector_close(bool toplevel);
    void ppuregInspector_close(bool toplevel);
    void apuregInspector_close(bool toplevel);
    void on_actionCHRMEM_Inspector_toggled(bool );
    void on_actionOAM_Inspector_toggled(bool );
    void on_actionNameTable_Inspector_toggled(bool );
    void on_actionExecution_Inspector_toggled(bool );
    void on_actionCPUMemory_Inspector_toggled(bool );
    void on_actionPPUMemory_Inspector_toggled(bool );
    void on_actionCHRMemory_Inspector_toggled(bool );
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
