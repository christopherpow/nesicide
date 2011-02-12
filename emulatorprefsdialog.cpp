#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "emulator_core.h"

#include <QSettings>

EmulatorPrefsDialog::EmulatorPrefsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   QSettings settings;
   
   ui->setupUi(this);
   
   ui->tabWidget->setCurrentIndex(0);
   ui->controllerPortComboBox->setCurrentIndex(0);
   
   readSettings();
}

EmulatorPrefsDialog::~EmulatorPrefsDialog()
{
   delete ui;
}

void EmulatorPrefsDialog::readSettings()
{   
   QSettings settings;

   // Load controller settings for each port...
   settings.beginGroup("emulatorPreferences/controllerConfig");
   ui->controllerTypeComboBox->setCurrentIndex(settings.value("port1controllerType",QVariant(IO_StandardJoypad)).toInt());
   ui->upKey->setText(settings.value("port1upKey",QVariant('W')).toChar());
   ui->downKey->setText(settings.value("port1downKey",QVariant('S')).toChar());
   ui->leftKey->setText(settings.value("port1leftKey",QVariant('A')).toChar());
   ui->rightKey->setText(settings.value("port1rightKey",QVariant('D')).toChar());
   ui->aKey->setText(settings.value("port1aKey",QVariant('.')).toChar());
   ui->bKey->setText(settings.value("port1bKey",QVariant('/')).toChar());
   ui->selectKey->setText(settings.value("port1selectKey",QVariant('[')).toChar());
   ui->startKey->setText(settings.value("port1startKey",QVariant(']')).toChar());
   settings.endGroup();
}

void EmulatorPrefsDialog::writeSettings()
{
   QSettings settings;
   
   // Store controller settings for each port...
   settings.beginGroup("emulatorPreferences/controllerConfig");
   settings.setValue("port1controllerType",QVariant(ui->controllerTypeComboBox->currentIndex()));
   settings.setValue("port1upKey",QVariant(ui->upKey->text()));
   settings.setValue("port1downKey",QVariant(ui->downKey->text()));
   settings.setValue("port1leftKey",QVariant(ui->leftKey->text()));
   settings.setValue("port1rightKey",QVariant(ui->rightKey->text()));
   settings.setValue("port1aKey",QVariant(ui->aKey->text()));
   settings.setValue("port1bKey",QVariant(ui->bKey->text()));
   settings.setValue("port1selectKey",QVariant(ui->selectKey->text()));
   settings.setValue("port1startKey",QVariant(ui->startKey->text()));
   settings.endGroup();
}

void EmulatorPrefsDialog::on_controllerTypeComboBox_currentIndexChanged(int index)
{
   if (index == IO_Disconnected)
   {
      ui->ControllerKeysStackedWidget->setCurrentIndex(0);
   }
   else if (index == IO_StandardJoypad)
   {
      ui->ControllerKeysStackedWidget->setCurrentIndex(1);
   }
   else if (index == IO_Zapper)
   {
      ui->ControllerKeysStackedWidget->setCurrentIndex(2);
   }
}

void EmulatorPrefsDialog::on_controllerPortComboBox_currentIndexChanged(int index)
{
    
}

void EmulatorPrefsDialog::on_buttonBox_accepted()
{
   writeSettings();
}
