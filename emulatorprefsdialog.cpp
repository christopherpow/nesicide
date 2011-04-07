#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "emulator_core.h"

#include <QSettings>

EmulatorPrefsDialog::EmulatorPrefsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   QSettings settings;
   int32_t*  configString;
   int32_t   configSize;

   ui->setupUi(this);

   ui->tabWidget->setCurrentIndex(0);
   ui->controllerPortComboBox->setCurrentIndex(0);

   settings.beginGroup("EmulatorPreferences");
   if ( !settings.contains("ControllerConfig") )
   {
      settings.beginGroup("ControllerConfig/Port0");
      settings.setValue("Type",QVariant(IO_StandardJoypad).toInt());
      settings.endGroup();
      settings.beginGroup("ControllerConfig/Port1");
      settings.setValue("Type",QVariant(IO_Zapper).toInt());
      settings.endGroup();
      settings.beginGroup("ControllerConfig");
      if ( !settings.contains("StandardJoypad/Port0") )
      {
         settings.beginGroup("StandardJoypad/Port0");
         nesGetControllerConfig(0,IO_StandardJoypad,&configString,&configSize);
         settings.setValue("ConfigString",QVariant(QByteArray((char*)configString,configSize)));
         settings.endGroup();
      }
      if ( !settings.contains("StandardJoypad/Port1") )
      {
         settings.beginGroup("StandardJoypad/Port1");
         nesGetControllerConfig(1,IO_StandardJoypad,&configString,&configSize);
         settings.setValue("ConfigString",QVariant(QByteArray((char*)configString,configSize)));
         settings.endGroup();
      }
      if ( !settings.contains("Zapper") )
      {
      }
      settings.endGroup();
   }
   settings.endGroup();

   readSettings();

   updateUi();
}

EmulatorPrefsDialog::~EmulatorPrefsDialog()
{
   delete ui;
}

void EmulatorPrefsDialog::readSettings()
{
   QSettings  settings;
   QByteArray configString;
   int        configSize;
   int32_t*   configItems;

   if ( ui->controllerPortComboBox->currentIndex() == 0 )
   {
      if ( ui->controllerTypeComboBox->currentIndex() == IO_StandardJoypad )
      {
         configString = settings.value("EmulatorPreferences/ControllerConfig/StandardJoypad/Port0").toByteArray();
         configItems = (int32_t*)configString.constData();
         ui->upKey->setText(QString(QChar(configItems[idxJOY_UP])));
         ui->downKey->setText(QString(QChar(configItems[idxJOY_DOWN])));
         ui->leftKey->setText(QString(QChar(configItems[idxJOY_LEFT])));
         ui->rightKey->setText(QString(QChar(configItems[idxJOY_RIGHT])));
         ui->aKey->setText(QString(QChar(configItems[idxJOY_A])));
         ui->bKey->setText(QString(QChar(configItems[idxJOY_B])));
         ui->selectKey->setText(QString(QChar(configItems[idxJOY_SELECT])));
         ui->startKey->setText(QString(QChar(configItems[idxJOY_START])));
      }
   }
   else if ( ui->controllerPortComboBox->currentIndex() == 1 )
   {
      if ( ui->controllerTypeComboBox->currentIndex() == IO_StandardJoypad )
      {
         configString = settings.value("EmulatorPreferences/ControllerConfig/StandardJoypad/Port1").toByteArray();
         configItems = (int32_t*)configString.constData();
         ui->upKey->setText(QString(QChar(configItems[idxJOY_UP])));
         ui->downKey->setText(QString(QChar(configItems[idxJOY_DOWN])));
         ui->leftKey->setText(QString(QChar(configItems[idxJOY_LEFT])));
         ui->rightKey->setText(QString(QChar(configItems[idxJOY_RIGHT])));
         ui->aKey->setText(QString(QChar(configItems[idxJOY_A])));
         ui->bKey->setText(QString(QChar(configItems[idxJOY_B])));
         ui->selectKey->setText(QString(QChar(configItems[idxJOY_SELECT])));
         ui->startKey->setText(QString(QChar(configItems[idxJOY_START])));
      }
   }
}

void EmulatorPrefsDialog::writeSettings()
{
}

void EmulatorPrefsDialog::on_controllerTypeComboBox_currentIndexChanged(int index)
{
   QString settingsGroup = "EmulatorPreferences/ControllerConfig/Port"+QString::number(ui->controllerPortComboBox->currentIndex());
   QSettings settings;

   settings.beginGroup(settingsGroup);
   settings.setValue("Type",QVariant(index).toInt());
   settings.endGroup();

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

   readSettings();

   updateUi();
}

void EmulatorPrefsDialog::on_controllerPortComboBox_currentIndexChanged(int index)
{
   readSettings();

   updateUi();
}

void EmulatorPrefsDialog::on_buttonBox_accepted()
{
   writeSettings();
}

void EmulatorPrefsDialog::updateUi()
{
   QSettings settings;

   if ( ui->controllerPortComboBox->currentIndex() == 0 )
   {
      settings.beginGroup("EmulatorPreferences/ControllerConfig/Port0");
      ui->ControllerKeysStackedWidget->setCurrentIndex(settings.value("Type").toInt());
      if ( settings.value("Type").toInt() == IO_StandardJoypad )
      {
      }
      else if ( settings.value("Type").toInt() == IO_Zapper )
      {
      }
      settings.endGroup();
   }
   else if ( ui->controllerPortComboBox->currentIndex() == 1 )
   {
      settings.beginGroup("EmulatorPreferences/ControllerConfig/Port1");
      ui->ControllerKeysStackedWidget->setCurrentIndex(settings.value("Type").toInt());
      if ( settings.value("Type").toInt() == IO_StandardJoypad )
      {
      }
      else if ( settings.value("Type").toInt() == IO_Zapper )
      {
      }
      settings.endGroup();
   }
}
