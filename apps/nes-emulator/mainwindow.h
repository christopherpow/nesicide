#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "nesemulatordockwidget.h"
#include "aboutdialog.h"
#include "emulatorprefsdialog.h"
#include "emulatorcontrol.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(QWidget* parent = 0);
   virtual ~MainWindow();

protected:
   void dragEnterEvent ( QDragEnterEvent* event );
   void dragMoveEvent ( QDragMoveEvent* event );
   void dropEvent ( QDropEvent* event );
   void loadCartridge ( QString fileName );
   void saveEmulatorState(QString fileName);

protected:
   virtual void closeEvent ( QCloseEvent* event );

private:
   Ui::MainWindow* ui;
   NESEmulatorDockWidget* m_pEmulator;
   bool m_bEmulatorFloating;
   EmulatorControl* m_pEmulatorControl;

signals:
   void startEmulation();
   void pauseEmulation(bool show);
   void resetEmulator();

private slots:
   void on_actionAbout_Qt_triggered();
   void on_actionFullscren_toggled(bool value);
   void on_actionDelta_Modulation_toggled(bool );
   void on_actionNoise_toggled(bool );
   void on_actionTriangle_toggled(bool );
   void on_actionSquare_2_toggled(bool );
   void on_actionSquare_1_toggled(bool );
   void on_actionMute_All_toggled(bool );
   void on_actionPreferences_triggered();
   void on_actionAbout_triggered();
   void on_actionPAL_triggered();
   void on_actionNTSC_triggered();
   void on_actionOpen_triggered();
   void on_actionExit_triggered();
};

#endif // MAINWINDOW_H
