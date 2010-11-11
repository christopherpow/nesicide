#ifndef EMULATORPREFSDIALOG_H
#define EMULATORPREFSDIALOG_H

#include <QDialog>

namespace Ui {
    class EmulatorPrefsDialog;
}

class EmulatorPrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmulatorPrefsDialog(QWidget *parent = 0);
    ~EmulatorPrefsDialog();

private:
    Ui::EmulatorPrefsDialog *ui;

private slots:
    void on_controllerTypeComboBox_currentIndexChanged(int index );
};

#endif // EMULATORPREFSDIALOG_H
