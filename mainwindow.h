#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "cnesicideproject.h"
#include "cprojecttreeviewmodel.h"
#include "projectpropertiesdialog.h"
#include "newprojectdialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CNesicideProject *nesicideProject;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    void projectDataChangesEvent();
    CProjectTreeViewModel *projectTreeviewModel;

private slots:
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
