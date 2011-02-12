#ifndef EMULATORPREFSDIALOG_H
#define EMULATORPREFSDIALOG_H

#include <QDialog>

namespace Ui
{
class EmulatorPrefsDialog;
}

class EmulatorPrefsDialog : public QDialog
{
   Q_OBJECT

public:
   explicit EmulatorPrefsDialog(QWidget* parent = 0);
   ~EmulatorPrefsDialog();

private:
   Ui::EmulatorPrefsDialog* ui;
   void readSettings();
   void writeSettings();

private slots:
   void on_buttonBox_accepted();
   void on_controllerPortComboBox_currentIndexChanged(int index);
   void on_controllerTypeComboBox_currentIndexChanged(int index );
};

#endif // EMULATORPREFSDIALOG_H
