#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "emulator_core.h"

#include <QSettings>

// Query flags.
bool EmulatorPrefsDialog::controllersUpdated;
bool EmulatorPrefsDialog::audioUpdated;
bool EmulatorPrefsDialog::videoUpdated;
bool EmulatorPrefsDialog::systemUpdated;

// Settings data structures.
int EmulatorPrefsDialog::lastActiveTab;
int EmulatorPrefsDialog::controllerType[NUM_CONTROLLERS];
int EmulatorPrefsDialog::standardJoypadKeyMap[NUM_CONTROLLERS][IO_StandardJoypad_MAX];
int EmulatorPrefsDialog::tvStandard;
bool EmulatorPrefsDialog::square1Enabled;
bool EmulatorPrefsDialog::square2Enabled;
bool EmulatorPrefsDialog::triangleEnabled;
bool EmulatorPrefsDialog::noiseEnabled;
bool EmulatorPrefsDialog::dmcEnabled;
int EmulatorPrefsDialog::scalingFactor;

EmulatorPrefsDialog::EmulatorPrefsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   ui->setupUi(this);

   readSettings();

   ui->tabWidget->setCurrentIndex(0);
   ui->controllerPortComboBox->setCurrentIndex(0);
   on_controllerPortComboBox_currentIndexChanged(0);

   ui->tvStandard->setCurrentIndex(tvStandard);

   ui->square1->setChecked(square1Enabled);
   ui->square2->setChecked(square2Enabled);
   ui->triangle->setChecked(triangleEnabled);
   ui->noise->setChecked(noiseEnabled);
   ui->dmc->setChecked(dmcEnabled);

   ui->scalingFactor->setCurrentIndex(scalingFactor);

   ui->tabWidget->setCurrentIndex(lastActiveTab);
}

EmulatorPrefsDialog::~EmulatorPrefsDialog()
{
   delete ui;
}

void EmulatorPrefsDialog::readSettings()
{
   QSettings settings;
   int       port;
   int       function;

   // Nothing's been updated yet.
   controllersUpdated = false;
   audioUpdated = false;
   videoUpdated = false;
   systemUpdated = false;

   for ( port = 0; port < NUM_CONTROLLERS; port++ )
   {
      settings.beginGroup("EmulatorPreferences/ControllerConfig/Port"+QString::number(port));
      controllerType[port] = settings.value("Type",QVariant(IO_StandardJoypad)).toInt();
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/ControllerConfig/StandardJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_StandardJoypad_MAX; function++ )
      {
         standardJoypadKeyMap[port][function] = settings.value("KeyMap"+QString::number(function)).toInt();
      }
      settings.endGroup();
   }

   settings.beginGroup("EmulatorPreferences/Audio");
   square1Enabled = settings.value("Square1",QVariant(true)).toBool();
   square2Enabled = settings.value("Square2",QVariant(true)).toBool();
   triangleEnabled = settings.value("Triangle",QVariant(true)).toBool();
   noiseEnabled = settings.value("Noise",QVariant(true)).toBool();
   dmcEnabled = settings.value("DMC",QVariant(true)).toBool();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/Video");
   scalingFactor = settings.value("ScalingFactor",0).toInt();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/System");
   tvStandard = settings.value("TVStandard",QVariant(MODE_NTSC)).toInt();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences");
   lastActiveTab = settings.value("LastActiveTab",0).toInt();
   settings.endGroup();
}

void EmulatorPrefsDialog::writeSettings()
{
   QSettings settings;
   QKeySequence upKeySequence(ui->upKey->text());
   QKeySequence downKeySequence(ui->downKey->text());
   QKeySequence leftKeySequence(ui->leftKey->text());
   QKeySequence rightKeySequence(ui->rightKey->text());
   QKeySequence aKeySequence(ui->aKey->text());
   QKeySequence bKeySequence(ui->bKey->text());
   QKeySequence selectKeySequence(ui->selectKey->text());
   QKeySequence startKeySequence(ui->startKey->text());
   int       port;
   int       function;

   // Set query flags.
   if ( tvStandard != ui->tvStandard->currentIndex() )
   {
      systemUpdated = true;
   }
   if ( (square1Enabled != ui->square1->isChecked()) ||
        (square2Enabled != ui->square2->isChecked()) ||
        (triangleEnabled != ui->triangle->isChecked()) ||
        (noiseEnabled != ui->noise->isChecked()) ||
        (dmcEnabled != ui->dmc->isChecked()) )
   {
      audioUpdated = true;
   }
   if ( scalingFactor != ui->scalingFactor->currentIndex() )
   {
      videoUpdated = true;
   }
   controllersUpdated = true;
   lastActiveTab = ui->tabWidget->currentIndex();

   // Save UI to locals first.
   switch ( ui->controllerTypeComboBox->currentIndex() )
   {
      case IO_Disconnected:
      break;
      case IO_StandardJoypad:
         port = ui->controllerPortComboBox->currentIndex();
         upKeySequence.fromString(ui->upKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_UP] = upKeySequence[0];
         downKeySequence.fromString(ui->downKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_DOWN] = downKeySequence[0];
         leftKeySequence.fromString(ui->leftKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_LEFT] = leftKeySequence[0];
         rightKeySequence.fromString(ui->rightKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT] = rightKeySequence[0];
         aKeySequence.fromString(ui->aKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_A] = aKeySequence[0];
         bKeySequence.fromString(ui->bKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_B] = bKeySequence[0];
         selectKeySequence.fromString(ui->selectKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_SELECT] = selectKeySequence[0];
         startKeySequence.fromString(ui->startKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_START] = startKeySequence[0];
      break;
      case IO_Zapper:
      break;
   }
   tvStandard = ui->tvStandard->currentIndex();

   square1Enabled = ui->square1->isChecked();
   square2Enabled = ui->square2->isChecked();
   triangleEnabled = ui->triangle->isChecked();
   noiseEnabled = ui->noise->isChecked();
   dmcEnabled = ui->dmc->isChecked();

   scalingFactor = ui->scalingFactor->currentIndex();

   // Then save locals to QSettings.
   for ( port = 0; port < NUM_CONTROLLERS; port++ )
   {
      settings.beginGroup("EmulatorPreferences/ControllerConfig/Port"+QString::number(port));
      settings.setValue("Type",controllerType[port]);
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/ControllerConfig/StandardJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_StandardJoypad_MAX; function++ )
      {
         settings.setValue("KeyMap"+QString::number(function),standardJoypadKeyMap[port][function]);
      }
      settings.endGroup();
   }

   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("Square1",square1Enabled);
   settings.setValue("Square2",square2Enabled);
   settings.setValue("Triangle",triangleEnabled);
   settings.setValue("Noise",noiseEnabled);
   settings.setValue("DMC",dmcEnabled);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/Video");
   settings.setValue("ScalingFactor",scalingFactor);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/System");
   settings.setValue("TVStandard",tvStandard);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences");
   settings.setValue("LastActiveTab",lastActiveTab);
   settings.endGroup();
}

void EmulatorPrefsDialog::updateUi()
{
   int port = ui->controllerPortComboBox->currentIndex();
   int type = ui->controllerTypeComboBox->currentIndex();

   if (type == IO_Disconnected)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(0);
   }
   else if (type == IO_StandardJoypad)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(1);
      ui->upKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_UP]).toString());
      ui->downKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_DOWN]).toString());
      ui->leftKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_LEFT]).toString());
      ui->rightKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT]).toString());
      ui->aKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_A]).toString());
      ui->bKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_B]).toString());
      ui->selectKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_SELECT]).toString());
      ui->startKey->setText(QKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_START]).toString());
   }
   else if (type == IO_Zapper)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(2);
   }
}

void EmulatorPrefsDialog::on_controllerTypeComboBox_currentIndexChanged(int index)
{
   int port = ui->controllerPortComboBox->currentIndex();

   controllerType[port] = index;

   updateUi();
}

void EmulatorPrefsDialog::on_controllerPortComboBox_currentIndexChanged(int index)
{
   ui->controllerTypeComboBox->setCurrentIndex(controllerType[index]);

   updateUi();
}

void EmulatorPrefsDialog::on_controllerPortComboBox_highlighted(int index)
{
   int port = ui->controllerPortComboBox->currentIndex();

   if ( index != port )
   {
      if ( ui->controllerTypeComboBox->currentIndex() == IO_StandardJoypad )
      {
         QKeySequence upKeySequence(ui->upKey->text());
         upKeySequence.fromString(ui->upKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_UP] = upKeySequence[0];
         QKeySequence downKeySequence(ui->upKey->text());
         downKeySequence.fromString(ui->downKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_DOWN] = downKeySequence[0];
         QKeySequence leftKeySequence(ui->upKey->text());
         leftKeySequence.fromString(ui->leftKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_LEFT] = leftKeySequence[0];
         QKeySequence rightKeySequence(ui->upKey->text());
         rightKeySequence.fromString(ui->rightKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT] = rightKeySequence[0];
         QKeySequence aKeySequence(ui->upKey->text());
         aKeySequence.fromString(ui->aKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_A] = aKeySequence[0];
         QKeySequence bKeySequence(ui->upKey->text());
         bKeySequence.fromString(ui->bKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_B] = bKeySequence[0];
         QKeySequence selectKeySequence(ui->upKey->text());
         selectKeySequence.fromString(ui->selectKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_SELECT] = selectKeySequence[0];
         QKeySequence startKeySequence(ui->upKey->text());
         startKeySequence.fromString(ui->startKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_START] = startKeySequence[0];
      }
   }
}

void EmulatorPrefsDialog::on_controllerTypeComboBox_highlighted(int index)
{
   int port = ui->controllerPortComboBox->currentIndex();

   if ( index != ui->controllerTypeComboBox->currentIndex() )
   {
      if ( ui->controllerTypeComboBox->currentIndex() == IO_StandardJoypad )
      {
         QKeySequence upKeySequence(ui->upKey->text());
         upKeySequence.fromString(ui->upKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_UP] = upKeySequence[0];
         QKeySequence downKeySequence(ui->upKey->text());
         downKeySequence.fromString(ui->downKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_DOWN] = downKeySequence[0];
         QKeySequence leftKeySequence(ui->upKey->text());
         leftKeySequence.fromString(ui->leftKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_LEFT] = leftKeySequence[0];
         QKeySequence rightKeySequence(ui->upKey->text());
         rightKeySequence.fromString(ui->rightKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT] = rightKeySequence[0];
         QKeySequence aKeySequence(ui->upKey->text());
         aKeySequence.fromString(ui->aKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_A] = aKeySequence[0];
         QKeySequence bKeySequence(ui->upKey->text());
         bKeySequence.fromString(ui->bKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_B] = bKeySequence[0];
         QKeySequence selectKeySequence(ui->upKey->text());
         selectKeySequence.fromString(ui->selectKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_SELECT] = selectKeySequence[0];
         QKeySequence startKeySequence(ui->upKey->text());
         startKeySequence.fromString(ui->startKey->text());
         standardJoypadKeyMap[port][IO_StandardJoypad_START] = startKeySequence[0];
      }
   }
}

void EmulatorPrefsDialog::on_buttonBox_accepted()
{
   writeSettings();
}

int EmulatorPrefsDialog::getControllerType(int port)
{
   return controllerType[port];
}

int EmulatorPrefsDialog::getControllerKeyMap(int port,int function)
{
   int keyMap = -1;
   switch ( controllerType[port] )
   {
      case IO_Disconnected:
      break;
      case IO_StandardJoypad:
         keyMap = standardJoypadKeyMap[port][function];
      break;
      case IO_Zapper:
      break;
   }
   return keyMap;
}

int EmulatorPrefsDialog::getTVStandard()
{
   return tvStandard;
}

void EmulatorPrefsDialog::setTVStandard(int standard)
{
   QSettings settings;

   // Update local storage first.
   tvStandard = standard;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/System");
   settings.setValue("TVStandard",tvStandard);
   settings.endGroup();
}

bool EmulatorPrefsDialog::getSquare1Enabled()
{
   return square1Enabled;
}

bool EmulatorPrefsDialog::getSquare2Enabled()
{
   return square2Enabled;
}

bool EmulatorPrefsDialog::getTriangleEnabled()
{
   return triangleEnabled;
}

bool EmulatorPrefsDialog::getNoiseEnabled()
{
   return noiseEnabled;
}

bool EmulatorPrefsDialog::getDMCEnabled()
{
   return dmcEnabled;
}

void EmulatorPrefsDialog::setSquare1Enabled(bool enabled)
{
   QSettings settings;

   // Update local storage first.
   square1Enabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("Square1",square1Enabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setSquare2Enabled(bool enabled)
{
   QSettings settings;

   // Update local storage first.
   square2Enabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("Square2",square2Enabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setTriangleEnabled(bool enabled)
{
   QSettings settings;

   // Update local storage first.
   triangleEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("Triangle",triangleEnabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setNoiseEnabled(bool enabled)
{
   QSettings settings;

   // Update local storage first.
   noiseEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("Noise",noiseEnabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setDMCEnabled(bool enabled)
{
   QSettings settings;

   // Update local storage first.
   dmcEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Audio");
   settings.setValue("DMC",dmcEnabled);
   settings.endGroup();
}

int EmulatorPrefsDialog::getScalingFactor()
{
   return scalingFactor+1; // This is used as a multiplier but is zero-based.
}

void EmulatorPrefsDialog::setScalingFactor(int factor)
{
   QSettings settings;

   // Update local storage first.
   scalingFactor = factor;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/Video");
   settings.setValue("ScalingFactor",scalingFactor);
   settings.endGroup();
}
