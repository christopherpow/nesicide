#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

private slots:
    void addToolBarWidget(QToolBar* toolBar);
    void removeToolBarWidget(QToolBar* toolBar);
    void editor_modificationChanged(bool m);
    void documentSaved();
    void documentClosed();
};

#endif // MAINWINDOW_H
