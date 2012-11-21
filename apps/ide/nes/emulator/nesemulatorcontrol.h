#ifndef NESEMULATORCONTROL_H
#define NESEMULATORCONTROL_H

#include <QWidget>

namespace Ui {
   class NESEmulatorControl;
}

class NESEmulatorControl : public QWidget
{
   Q_OBJECT

public:
   explicit NESEmulatorControl(QWidget *parent = 0);
   virtual ~NESEmulatorControl();
   QList<QAction*> menu();

private:
   Ui::NESEmulatorControl *ui;
   bool debugging;

signals:
   void focusEmulator();
   void startEmulation();
   void pauseEmulation(bool show);
   void stepCPUEmulation();
   void stepOverCPUEmulation();
   void stepOutCPUEmulation();
   void stepPPUEmulation();
   void advanceFrame();
   void resetEmulator();
   void softResetEmulator();

private slots:
   void on_softButton_clicked();
   void on_debugButton_toggled(bool checked);
   void on_stepOutButton_clicked();
   void on_stepOverButton_clicked();
   void on_frameAdvance_clicked();
   void on_resetButton_clicked();
   void on_stepPPUButton_clicked();
   void on_stepCPUButton_clicked();
   void on_pauseButton_clicked();
   void on_playButton_clicked();
   void internalPause();
   void internalPlay();
};

#endif // NESEMULATORCONTROL_H
