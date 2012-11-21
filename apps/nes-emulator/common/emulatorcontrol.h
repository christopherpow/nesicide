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
   EmulatorControl(QWidget *parent = 0);
   virtual ~EmulatorControl();
   QList<QAction*> menu();

private:
   Ui::EmulatorControl *ui;

signals:
   void startEmulation();
   void pauseEmulation(bool show);
   void softResetEmulator();
   void resetEmulator();

private slots:
   void on_softButton_clicked();
   void on_resetButton_clicked();
   void on_pauseButton_clicked();
   void on_playButton_clicked();
   void internalPause();
   void internalPlay();
};

#endif // EMULATORCONTROL_H
