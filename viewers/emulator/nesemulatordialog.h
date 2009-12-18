#ifndef NESEMULATORDIALOG_H
#define NESEMULATORDIALOG_H

#include <QDialog>
#include <QBasicTimer>

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
    void stopEmulation();
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
    void on_stepButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_playButton_clicked();
    void on_resetButton_clicked();

signals:
    void controllerInput(unsigned char* input);
};

#endif // NESEMULATORDIALOG_H
