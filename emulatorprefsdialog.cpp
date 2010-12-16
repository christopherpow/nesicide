#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "emulator_core.h"

EmulatorPrefsDialog::EmulatorPrefsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   ui->setupUi(this);
}

EmulatorPrefsDialog::~EmulatorPrefsDialog()
{
   delete ui;
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
