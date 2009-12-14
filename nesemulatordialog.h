#ifndef NESEMULATORDIALOG_H
#define NESEMULATORDIALOG_H

#include <QDialog>
#include <QBasicTimer>

#include "nesemulatorrenderer.h"

namespace Ui {
    class NESEmulatorDialog;
}

class NESEmulatorDialog : public QDialog {
    Q_OBJECT
public:
    NESEmulatorDialog(QWidget *parent = 0);
    ~NESEmulatorDialog();
    void stopEmulation();
    CNESEmulatorRenderer *renderer;
    QBasicTimer* timer;

protected:
    void changeEvent(QEvent *e);
    void timerEvent(QTimerEvent *event);

private:
    Ui::NESEmulatorDialog *ui;
    char *imgData;

private slots:
    void on_stepButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_playButton_clicked();
};

#endif // NESEMULATORDIALOG_H
