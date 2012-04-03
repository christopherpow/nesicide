#ifndef C64EMULATORCONTROL_H
#define C64EMULATORCONTROL_H

#include <QWidget>

namespace Ui {
   class C64EmulatorControl;
}

class C64EmulatorControl : public QWidget
{
   Q_OBJECT

public:
   explicit C64EmulatorControl(QWidget *parent = 0);
   virtual ~C64EmulatorControl();
   QList<QAction*> menu();

private:
   Ui::C64EmulatorControl *ui;
   bool debugging;

signals:
   void startEmulation();
   void pauseEmulation(bool show);
   void stepCPUEmulation();
   void stepOverCPUEmulation();
   void stepOutCPUEmulation();
   void resetEmulator();

private slots:
   void on_debugButton_toggled(bool checked);
   void on_stepOutButton_clicked();
   void on_stepOverButton_clicked();
   void on_resetButton_clicked();
   void on_stepCPUButton_clicked();
   void on_pauseButton_clicked();
   void on_playButton_clicked();
   void internalPause();
   void internalPlay();
};

#endif // C64EMULATORCONTROL_H
