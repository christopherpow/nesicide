#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "nes_emulator_core.h"

#include <QSettings>
#include <QFileDialog>

// Query flags.
bool EmulatorPrefsDialog::controllersUpdated;
bool EmulatorPrefsDialog::audioUpdated;
bool EmulatorPrefsDialog::videoUpdated;
bool EmulatorPrefsDialog::systemUpdated;

// NES settings data structures.
int EmulatorPrefsDialog::controllerType[NUM_CONTROLLERS];
int EmulatorPrefsDialog::standardJoypadKeyMap[NUM_CONTROLLERS][IO_StandardJoypad_MAX];
int EmulatorPrefsDialog::zapperKeyMap[NUM_CONTROLLERS][IO_Zapper_MAX];
bool EmulatorPrefsDialog::zapperMouseMap[NUM_CONTROLLERS][IO_Zapper_MAX];
int EmulatorPrefsDialog::vausArkanoidKeyMap[NUM_CONTROLLERS][IO_Vaus_MAX];
bool EmulatorPrefsDialog::vausArkanoidMouseMap[NUM_CONTROLLERS][IO_Vaus_MAX];
int EmulatorPrefsDialog::vausArkanoidTrimPot[NUM_CONTROLLERS];
int EmulatorPrefsDialog::tvStandard;
bool EmulatorPrefsDialog::pauseOnKIL;
bool EmulatorPrefsDialog::square1Enabled;
bool EmulatorPrefsDialog::square2Enabled;
bool EmulatorPrefsDialog::triangleEnabled;
bool EmulatorPrefsDialog::noiseEnabled;
bool EmulatorPrefsDialog::dmcEnabled;
int EmulatorPrefsDialog::scalingFactor;

// C=64 settings data structures.
QString EmulatorPrefsDialog::viceExecutable;
QString EmulatorPrefsDialog::viceIPAddress;
int EmulatorPrefsDialog::viceMonitorPort;
QString EmulatorPrefsDialog::viceOptions;
QString EmulatorPrefsDialog::c64KernalROM;
QString EmulatorPrefsDialog::c64BasicROM;
QString EmulatorPrefsDialog::c64CharROM;

EmulatorPrefsDialog::EmulatorPrefsDialog(QString target,QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   ui->setupUi(this);

   m_targetLoaded = target;

   readSettings();

   ui->tabWidget->setCurrentIndex(0);
   ui->controllerPortComboBox->setCurrentIndex(0);
   on_controllerPortComboBox_currentIndexChanged(0);

   ui->tvStandard->setCurrentIndex(tvStandard);
   ui->pauseOnKIL->setChecked(pauseOnKIL);

   ui->square1->setChecked(square1Enabled);
   ui->square2->setChecked(square2Enabled);
   ui->triangle->setChecked(triangleEnabled);
   ui->noise->setChecked(noiseEnabled);
   ui->dmc->setChecked(dmcEnabled);

   ui->scalingFactor->setCurrentIndex(scalingFactor);

   ui->viceC64Executable->setText(viceExecutable);
   ui->viceC64MonitorIPAddress->setText(viceIPAddress);
   ui->viceC64MonitorPort->setText(QString::number(viceMonitorPort));
   ui->viceOptions->setText(viceOptions);
   ui->c64KernalROM->setText(c64KernalROM);
   ui->c64BasicROM->setText(c64BasicROM);
   ui->c64CharROM->setText(c64CharROM);

   if ( !target.compare("nes",Qt::CaseInsensitive) )
   {
      ui->tabWidget->removeTab(4);
   }
   else if ( !target.compare("c64",Qt::CaseInsensitive) )
   {
      ui->tabWidget->removeTab(0);
      ui->tabWidget->removeTab(0);
      ui->tabWidget->removeTab(0);
      ui->tabWidget->removeTab(0);
   }
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
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Port"+QString::number(port));
      controllerType[port] = settings.value("Type",QVariant(IO_StandardJoypad)).toInt();
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/StandardJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_StandardJoypad_MAX; function++ )
      {
         standardJoypadKeyMap[port][function] = settings.value("KeyMap"+QString::number(function)).toInt();
      }
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Zapper/Port"+QString::number(port));
      for ( function = 0; function < IO_Zapper_MAX; function++ )
      {
         zapperKeyMap[port][function] = settings.value("KeyMap"+QString::number(function)).toInt();
         zapperMouseMap[port][function] = settings.value("MouseMap"+QString::number(function),QVariant(true)).toBool();
      }
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Vaus(Arkanoid)/Port"+QString::number(port));
      for ( function = 0; function < IO_Vaus_MAX; function++ )
      {
         vausArkanoidKeyMap[port][function] = settings.value("KeyMap"+QString::number(function)).toInt();
         vausArkanoidMouseMap[port][function] = settings.value("MouseMap"+QString::number(function),QVariant(true)).toBool();
      }
      vausArkanoidTrimPot[port] = settings.value("TrimPot",QVariant(0x54)).toInt();
      settings.endGroup();
   }

   settings.beginGroup("EmulatorPreferences/NES/Audio");
   square1Enabled = settings.value("Square1",QVariant(true)).toBool();
   square2Enabled = settings.value("Square2",QVariant(true)).toBool();
   triangleEnabled = settings.value("Triangle",QVariant(true)).toBool();
   noiseEnabled = settings.value("Noise",QVariant(true)).toBool();
   dmcEnabled = settings.value("DMC",QVariant(true)).toBool();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/Video");
   scalingFactor = settings.value("ScalingFactor",0).toInt();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/System");
   tvStandard = settings.value("TVStandard",QVariant(MODE_NTSC)).toInt();
   pauseOnKIL = settings.value("PauseOnKIL",QVariant(true)).toBool();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/C64");
   viceExecutable = settings.value("VICEExecutable",QVariant()).toString();
   viceIPAddress = settings.value("VICEIPAddress",QVariant("127.0.0.1")).toString();
   viceMonitorPort = settings.value("VICEMonitorPort",QVariant(6510)).toInt();
   viceOptions = settings.value("VICEOptions",QVariant()).toString();
   c64KernalROM = settings.value("C64KernalROM").toString();
   c64BasicROM = settings.value("C64BasicROM").toString();
   c64CharROM = settings.value("C64CharROM").toString();
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
   QKeySequence triggerZapperKeySequence(ui->triggerKeyZapper->text());
   QKeySequence ccwVausKeySequence(ui->ccwKeyVaus->text());
   QKeySequence cwVausKeySequence(ui->cwKeyVaus->text());
   QKeySequence triggerVausKeySequence(ui->triggerKeyVaus->text());
   int       port;
   int       function;

   // Set query flags.
   if ( (tvStandard != ui->tvStandard->currentIndex()) ||
        (pauseOnKIL != ui->pauseOnKIL->isChecked()) )
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
         port = ui->controllerPortComboBox->currentIndex();
         triggerZapperKeySequence.fromString(ui->triggerKeyZapper->text());
         zapperKeyMap[port][IO_Zapper_FIRE] = triggerZapperKeySequence[0];
         zapperMouseMap[port][IO_Zapper_FIRE] = ui->mouseFiresZapper->isChecked();
      break;
      case IO_Vaus:
         port = ui->controllerPortComboBox->currentIndex();
         ccwVausKeySequence.fromString(ui->ccwKeyVaus->text());
         vausArkanoidKeyMap[port][IO_Vaus_CCW] = ccwVausKeySequence[0];
         vausArkanoidMouseMap[port][IO_Vaus_CCW] = ui->mouseMovesVaus->isChecked();
         cwVausKeySequence.fromString(ui->cwKeyVaus->text());
         vausArkanoidKeyMap[port][IO_Vaus_CW] = cwVausKeySequence[0];
         vausArkanoidMouseMap[port][IO_Vaus_CW] = ui->mouseMovesVaus->isChecked();
         triggerVausKeySequence.fromString(ui->triggerKeyVaus->text());
         vausArkanoidKeyMap[port][IO_Vaus_FIRE] = triggerVausKeySequence[0];
         vausArkanoidMouseMap[port][IO_Vaus_FIRE] = ui->mouseFiresVaus->isChecked();
         vausArkanoidTrimPot[port] = ui->trimPotVaus->value();
      break;
   }
   tvStandard = ui->tvStandard->currentIndex();
   pauseOnKIL = ui->pauseOnKIL->isChecked();

   square1Enabled = ui->square1->isChecked();
   square2Enabled = ui->square2->isChecked();
   triangleEnabled = ui->triangle->isChecked();
   noiseEnabled = ui->noise->isChecked();
   dmcEnabled = ui->dmc->isChecked();

   scalingFactor = ui->scalingFactor->currentIndex();

   viceExecutable = ui->viceC64Executable->text();
   viceIPAddress = ui->viceC64MonitorIPAddress->text();
   viceMonitorPort = ui->viceC64MonitorPort->text().toInt();
   viceOptions = ui->viceOptions->toPlainText();
   c64KernalROM = ui->c64KernalROM->text();
   c64BasicROM = ui->c64BasicROM->text();
   c64CharROM = ui->c64CharROM->text();

   // Then save locals to QSettings.
   for ( port = 0; port < NUM_CONTROLLERS; port++ )
   {
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Port"+QString::number(port));
      settings.setValue("Type",controllerType[port]);
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/StandardJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_StandardJoypad_MAX; function++ )
      {
         settings.setValue("KeyMap"+QString::number(function),standardJoypadKeyMap[port][function]);
      }
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Zapper/Port"+QString::number(port));
      for ( function = 0; function < IO_Zapper_MAX; function++ )
      {
         settings.setValue("KeyMap"+QString::number(function),zapperKeyMap[port][function]);
         settings.setValue("MouseMap"+QString::number(function),zapperMouseMap[port][function]);
      }
      settings.endGroup();
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/Vaus(Arkanoid)/Port"+QString::number(port));
      for ( function = 0; function < IO_Vaus_MAX; function++ )
      {
         settings.setValue("KeyMap"+QString::number(function),vausArkanoidKeyMap[port][function]);
         settings.setValue("MouseMap"+QString::number(function),vausArkanoidMouseMap[port][function]);
      }
      settings.setValue("TrimPot",vausArkanoidTrimPot[port]);
      settings.endGroup();
   }

   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("Square1",square1Enabled);
   settings.setValue("Square2",square2Enabled);
   settings.setValue("Triangle",triangleEnabled);
   settings.setValue("Noise",noiseEnabled);
   settings.setValue("DMC",dmcEnabled);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/Video");
   settings.setValue("ScalingFactor",scalingFactor);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/System");
   settings.setValue("TVStandard",tvStandard);
   settings.setValue("PauseOnKIL",pauseOnKIL);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/C64");
   settings.setValue("VICEExecutable",viceExecutable);
   settings.setValue("VICEIPAddress",viceIPAddress);
   settings.setValue("VICEMonitorPort",viceMonitorPort);
   settings.setValue("VICEOptions",viceOptions);
   settings.setValue("C64KernalROM",c64KernalROM);
   settings.setValue("C64BasicROM",c64BasicROM);
   settings.setValue("C64CharROM",c64CharROM);
   settings.endGroup();
}

void EmulatorPrefsDialog::updateUi()
{
   int port = ui->controllerPortComboBox->currentIndex();
   int type = ui->controllerTypeComboBox->currentIndex();
   QKeySequence upKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_UP]);
   QKeySequence downKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_DOWN]);
   QKeySequence leftKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_LEFT]);
   QKeySequence rightKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT]);
   QKeySequence aKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_A]);
   QKeySequence bKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_B]);
   QKeySequence selectKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_SELECT]);
   QKeySequence startKeySequence(standardJoypadKeyMap[port][IO_StandardJoypad_START]);
   QKeySequence triggerZapperKeySequence(zapperKeyMap[port][IO_Zapper_FIRE]);
   QKeySequence ccwVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_CCW]);
   QKeySequence cwVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_CW]);
   QKeySequence triggerVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_FIRE]);

   if (type == IO_Disconnected)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(IO_Disconnected);
   }
   else if (type == IO_StandardJoypad)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(IO_StandardJoypad);
      ui->upKey->setText(upKeySequence.toString());
      ui->downKey->setText(downKeySequence.toString());
      ui->leftKey->setText(leftKeySequence.toString());
      ui->rightKey->setText(rightKeySequence.toString());
      ui->aKey->setText(aKeySequence.toString());
      ui->bKey->setText(bKeySequence.toString());
      ui->selectKey->setText(selectKeySequence.toString());
      ui->startKey->setText(startKeySequence.toString());
   }
   else if (type == IO_Zapper)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(IO_Zapper);
      ui->triggerKeyZapper->setText(triggerZapperKeySequence.toString());
      ui->mouseFiresZapper->setChecked(zapperMouseMap[port][IO_Zapper_FIRE]);
   }
   else if (type == IO_Vaus)
   {
      ui->controllerKeysStackedWidget->setCurrentIndex(IO_Vaus);
      ui->ccwKeyVaus->setText(ccwVausKeySequence.toString());
      ui->cwKeyVaus->setText(cwVausKeySequence.toString());
      // Note, both CCW and CW rotation are controlled by mouse, not either/or.
      ui->mouseMovesVaus->setChecked(vausArkanoidMouseMap[port][IO_Vaus_CCW]);
      ui->triggerKeyVaus->setText(triggerVausKeySequence.toString());
      ui->mouseFiresVaus->setChecked(vausArkanoidMouseMap[port][IO_Vaus_FIRE]);
      ui->trimPotVaus->setValue(vausArkanoidTrimPot[port]);
      on_trimPotVaus_dialMoved(vausArkanoidTrimPot[port]);
   }
}

void EmulatorPrefsDialog::updateDb()
{
   int port = ui->controllerPortComboBox->currentIndex();

   if ( ui->controllerTypeComboBox->currentIndex() == IO_StandardJoypad )
   {
      QKeySequence upKeySequence(ui->upKey->text());
      QKeySequence downKeySequence(ui->downKey->text());
      QKeySequence leftKeySequence(ui->leftKey->text());
      QKeySequence rightKeySequence(ui->rightKey->text());
      QKeySequence aKeySequence(ui->aKey->text());
      QKeySequence bKeySequence(ui->bKey->text());
      QKeySequence selectKeySequence(ui->selectKey->text());
      QKeySequence startKeySequence(ui->startKey->text());
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
   }
   else if ( ui->controllerTypeComboBox->currentIndex() == IO_Zapper )
   {
      QKeySequence triggerZapperKeySequence(ui->triggerKeyZapper->text());
      triggerZapperKeySequence.fromString(ui->triggerKeyZapper->text());
      zapperKeyMap[port][IO_Zapper_FIRE] = triggerZapperKeySequence[0];
      zapperMouseMap[port][IO_Zapper_FIRE] = ui->mouseFiresZapper->isChecked();
   }
   else if ( ui->controllerTypeComboBox->currentIndex() == IO_Vaus )
   {
      QKeySequence ccwVausKeySequence(ui->ccwKeyVaus->text());
      QKeySequence cwVausKeySequence(ui->cwKeyVaus->text());
      QKeySequence triggerVausKeySequence(ui->triggerKeyVaus->text());
      ccwVausKeySequence.fromString(ui->ccwKeyVaus->text());
      vausArkanoidKeyMap[port][IO_Vaus_CCW] = ccwVausKeySequence[0];
      cwVausKeySequence.fromString(ui->cwKeyVaus->text());
      vausArkanoidKeyMap[port][IO_Vaus_CW] = cwVausKeySequence[0];
      // Note, both CCW and CW rotation are controlled by mouse, not either/or.
      vausArkanoidMouseMap[port][IO_Vaus_CCW] = ui->mouseMovesVaus->isChecked();
      vausArkanoidMouseMap[port][IO_Vaus_CW] = ui->mouseMovesVaus->isChecked();
      triggerVausKeySequence.fromString(ui->triggerKeyVaus->text());
      vausArkanoidKeyMap[port][IO_Vaus_FIRE] = triggerVausKeySequence[0];
      vausArkanoidMouseMap[port][IO_Vaus_FIRE] = ui->mouseFiresVaus->isChecked();
      vausArkanoidTrimPot[port] = ui->trimPotVaus->value();
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
      updateDb();
   }
}

void EmulatorPrefsDialog::on_controllerTypeComboBox_highlighted(int index)
{
   if ( index != ui->controllerTypeComboBox->currentIndex() )
   {
      updateDb();
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
         keyMap = zapperKeyMap[port][function];
      break;
      case IO_Vaus:
         keyMap = vausArkanoidKeyMap[port][function];
      break;
   }
   return keyMap;
}

bool EmulatorPrefsDialog::getControllerMouseMap(int port,int function)
{
   bool mouseMap = false;
   switch ( controllerType[port] )
   {
      case IO_Disconnected:
      break;
      case IO_StandardJoypad:
      break;
      case IO_Zapper:
         mouseMap = zapperMouseMap[port][function];
      break;
      case IO_Vaus:
         mouseMap = vausArkanoidMouseMap[port][function];
      break;
   }
   return mouseMap;
}

int EmulatorPrefsDialog::getControllerSpecial(int port)
{
   int special = -1;
   switch ( controllerType[port] )
   {
      case IO_Disconnected:
      break;
      case IO_StandardJoypad:
      break;
      case IO_Zapper:
      break;
      case IO_Vaus:
         special = vausArkanoidTrimPot[port];
      break;
   }
   return special;
}

int EmulatorPrefsDialog::getTVStandard()
{
   return tvStandard;
}

bool EmulatorPrefsDialog::getPauseOnKIL()
{
   return pauseOnKIL;
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

void EmulatorPrefsDialog::on_trimPotVaus_dialMoved(int value)
{
   QString str;
   str.sprintf("$%02X-$%02X",value,value+0xA0);
   ui->trimPotValueVaus->setText(str);
}

QString EmulatorPrefsDialog::getVICEExecutable()
{
   return viceExecutable;
}

QString EmulatorPrefsDialog::getVICEIPAddress()
{
   return viceIPAddress;
}

int EmulatorPrefsDialog::getVICEMonitorPort()
{
   return viceMonitorPort;
}

QString EmulatorPrefsDialog::getVICEOptions()
{
   return viceOptions;
}

void EmulatorPrefsDialog::on_viceC64Browse_clicked()
{
   QString value = QFileDialog::getExistingDirectory(this,"VICE Executables Path",ui->viceC64Executable->text());

   if ( !value.isEmpty() )
   {
      ui->viceC64Executable->setText(value);
#if defined(Q_WS_WIN)
      ui->c64KernalROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"kernal");
      ui->c64BasicROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"basic");
      ui->c64CharROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"chargen");
#else
      ui->c64KernalROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"Kernal.rom");
      ui->c64BasicROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"Basic.rom");
      ui->c64CharROM->setText(value+QDir::separator()+"C64"+QDir::separator()+"Char.rom");
#endif
   }
}

void EmulatorPrefsDialog::on_c64KernalROMBrowse_clicked()
{
   QString value = QFileDialog::getOpenFileName(this,"Commodore 64 Kernal ROM",ui->c64KernalROM->text());

   if ( !value.isEmpty() )
   {
      ui->c64KernalROM->setText(value);
   }
}

void EmulatorPrefsDialog::on_c64BasicROMBrowse_clicked()
{
   QString value = QFileDialog::getOpenFileName(this,"Commodore 64 BASIC ROM",ui->c64BasicROM->text());

   if ( !value.isEmpty() )
   {
      ui->c64BasicROM->setText(value);
   }
}

void EmulatorPrefsDialog::on_c64CharROMBrowse_clicked()
{
   QString value = QFileDialog::getOpenFileName(this,"Commodore 64 Character ROM",ui->c64CharROM->text());

   if ( !value.isEmpty() )
   {
      ui->c64CharROM->setText(value);
   }
}

QString EmulatorPrefsDialog::getC64KernalROM()
{
   return c64KernalROM;
}

QString EmulatorPrefsDialog::getC64BasicROM()
{
   return c64BasicROM;
}

QString EmulatorPrefsDialog::getC64CharROM()
{
   return c64CharROM;
}
