#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "nesemulatordockwidget.h"
#include "nesemulatorthread.h"
#include "aboutdialog.h"
#include "emulatorprefsdialog.h"
#include "emulatorcontrol.h"

#define MAX_RECENT_FILES 8

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
   static QWidget* me() { return _me; }

protected:
   void dragEnterEvent ( QDragEnterEvent* event );
   void dragMoveEvent ( QDragMoveEvent* event );
   void dropEvent ( QDropEvent* event );
   void loadCartridge ( QString fileName );
   void saveEmulatorState(QString fileName);

protected:
   virtual void closeEvent ( QCloseEvent* event );

private:
   static QWidget* _me;
   Ui::MainWindow* ui;
   NESEmulatorDockWidget* m_pEmulator;
   bool m_bEmulatorFloating;
   EmulatorControl* m_pEmulatorControl;
   NESEmulatorThread* m_pNESEmulatorThread;
   QRect ncRect;
   QStringList m_recentFiles;
   QList<QAction*> m_recentFileActions;

private:
   void updateFromEmulatorPrefs(bool initial);

signals:
   void primeEmulator(CCartridge* pCartridge);
   void startEmulation();
   void pauseEmulation(bool show);
   void resetEmulator();

private slots:
   void openRecentFile();
   void saveRecentFiles(QString fileName);
   void updateRecentFiles();
   void on_action4_3_Aspect_toggled(bool );
   void on_actionLinear_Interpolation_toggled(bool );
   void on_action3x_triggered();
   void on_action2_5x_triggered();
   void on_action2x_triggered();
   void on_action1_5x_triggered();
   void on_action1x_triggered();
   void on_actionDendy_triggered();
   void applicationActivationChanged(bool activated);
   void on_actionAbout_Qt_triggered();
   void on_actionFullscreen_toggled(bool value);
   void on_actionDelta_Modulation_toggled(bool );
   void on_actionNoise_toggled(bool );
   void on_actionTriangle_toggled(bool );
   void on_actionSquare_2_toggled(bool );
   void on_actionSquare_1_toggled(bool );
   void on_actionConfigure_triggered();
   void on_actionAbout_triggered();
   void on_actionPAL_triggered();
   void on_actionNTSC_triggered();
   void on_actionOpen_triggered();
   void on_actionExit_triggered();
   void on_actionSawtoothVRC6_toggled(bool arg1);
   void on_actionPulse_2VRC6_toggled(bool arg1);
   void on_actionPulse_1VRC6_toggled(bool arg1);
   void on_actionWave_8N106_toggled(bool arg1);
   void on_actionWave_7N106_toggled(bool arg1);
   void on_actionWave_6N106_toggled(bool arg1);
   void on_actionWave_5N106_toggled(bool arg1);
   void on_actionWave_4N106_toggled(bool arg1);
   void on_actionWave_3N106_toggled(bool arg1);
   void on_actionWave_2N106_toggled(bool arg1);
   void on_actionWave_1N106_toggled(bool arg1);
};

#endif // MAINWINDOW_H
