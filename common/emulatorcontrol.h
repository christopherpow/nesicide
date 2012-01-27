#ifndef EMULATORCONTROL_H
#define EMULATORCONTROL_H

#include <QWidget>

namespace Ui {
   class EmulatorControl;
}

class EmulatorControl : public QWidget
{
   Q_OBJECT

public:
   explicit EmulatorControl(QWidget *parent = 0);
   virtual ~EmulatorControl();
   QList<QAction*> menu();

private:
   Ui::EmulatorControl *ui;
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

private slots:
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

#endif // EMULATORCONTROL_H
