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
#include "chrrominspector.h"

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
    CHRROMInspector* m_pPPUInspector;
    void saveProject(QString fileName);

protected:
    virtual void closeEvent ( QCloseEvent * event );

private slots:
    void on_actionCompile_Project_triggered();
    void on_actionCompiler_Output_toggled(bool );
    void on_compilerOutputDockWidget_visibilityChanged(bool visible);
    void on_actionSave_Active_Document_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionOpen_Project_triggered();
    void on_MainWindow_destroyed();
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
