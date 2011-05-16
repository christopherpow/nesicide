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
    ~EmulatorControl();

private:
    Ui::EmulatorControl *ui;

private slots:
    void on_resetButton_clicked();
    void on_stepPPUButton_clicked();
    void on_stepCPUButton_clicked();
    void on_pauseButton_clicked();
    void on_playButton_clicked();
    void internalPause();
    void internalPlay();
};

#endif // EMULATORCONTROL_H
