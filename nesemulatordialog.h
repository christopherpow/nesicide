#ifndef NESEMULATORDIALOG_H
#define NESEMULATORDIALOG_H

#include <QDialog>

namespace Ui {
    class NESEmulatorDialog;
}

class NESEmulatorDialog : public QDialog {
    Q_OBJECT
public:
    NESEmulatorDialog(QWidget *parent = 0);
    ~NESEmulatorDialog();
    void stopEmulation();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NESEmulatorDialog *ui;

private slots:
    void on_stepButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_playButton_clicked();
};

#endif // NESEMULATORDIALOG_H
