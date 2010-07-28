#ifndef NESEMULATORDIALOG_H
#define NESEMULATORDIALOG_H

#include <QDialog>

#include "cnesicidecommon.h"
#include "nesemulatorrenderer.h"

namespace Ui {
    class NESEmulatorDialog;
}

class NESEmulatorDialog : public QDialog {
    Q_OBJECT
public:
    NESEmulatorDialog(QWidget *parent = 0);
    ~NESEmulatorDialog();
    CNESEmulatorRenderer *renderer;

protected:
    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::NESEmulatorDialog *ui;
    char *imgData;
    unsigned char m_joy [ NUM_JOY ];

private slots:
    void on_stepCPUButton_clicked();
    void on_stepPPUButton_clicked();
    void on_pauseButton_clicked();
    void on_playButton_clicked();
    void on_resetButton_clicked();
    void internalPause(bool);
    void internalPauseWithoutShow();
    void internalPlay();
    void renderData();
};

#endif // NESEMULATORDIALOG_H
