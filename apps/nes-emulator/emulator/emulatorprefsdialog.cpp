#include "emulatorprefsdialog.h"
#include "ui_emulatorprefsdialog.h"

#include "nes_emulator_core.h"

#include <QSettings>

// Query flags.
bool EmulatorPrefsDialog::controllersUpdated;
bool EmulatorPrefsDialog::audioUpdated;
bool EmulatorPrefsDialog::videoUpdated;
bool EmulatorPrefsDialog::systemUpdated;

// General data structures.
bool EmulatorPrefsDialog::pauseOnTaskSwitch;

// NES settings data structures.
int EmulatorPrefsDialog::controllerType[NUM_CONTROLLERS];
int EmulatorPrefsDialog::standardJoypadKeyMap[NUM_CONTROLLERS][IO_StandardJoypad_MAX];
int EmulatorPrefsDialog::turboJoypadKeyMap[NUM_CONTROLLERS][IO_TurboJoypad_MAX];
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
bool EmulatorPrefsDialog::linearInterpolation;
bool EmulatorPrefsDialog::aspect43;

EmulatorPrefsDialog::EmulatorPrefsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EmulatorPrefsDialog)
{
   ui->setupUi(this);

   controllerPageMap.insert("(Disconnected)",ui->noControllerPage);
   controllerPageMap.insert("Standard Controller",ui->standardControllerPage);
   controllerPageMap.insert("Turbo Controller",ui->turboControllerPage);
   controllerPageMap.insert("Zapper",ui->zapperPage);
   controllerPageMap.insert("Vaus (Arkanoid)",ui->vausPage);

   readSettings();

   ui->tabWidget->setCurrentIndex(0);

   ui->pauseOnTaskSwitch->setChecked(pauseOnTaskSwitch);

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
   ui->linearInterpolation->setChecked(linearInterpolation);
   ui->aspect43->setChecked(aspect43);
}

EmulatorPrefsDialog::~EmulatorPrefsDialog()
{
   delete ui;
}

void EmulatorPrefsDialog::readSettings()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int       port;
   int       function;

   // Nothing's been updated yet.
   controllersUpdated = false;
   audioUpdated = false;
   videoUpdated = false;
   systemUpdated = false;

   settings.beginGroup("EmulatorPreferences/General");
   pauseOnTaskSwitch = settings.value("pauseOnTaskSwitch",QVariant(true)).toBool();
   settings.endGroup();

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
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/TurboJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_TurboJoypad_MAX; function++ )
      {
         turboJoypadKeyMap[port][function] = settings.value("KeyMap"+QString::number(function)).toInt();
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
   scalingFactor = settings.value("EMUScalingFactor",0).toInt();
   linearInterpolation = settings.value("LinearInterpolation",true).toBool();
   aspect43 = settings.value("EMU43Aspect",true).toBool();
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/System");
   tvStandard = settings.value("TVStandard",QVariant(MODE_NTSC)).toInt();
   pauseOnKIL = settings.value("PauseOnKIL",QVariant(true)).toBool();
   settings.endGroup();
}

void EmulatorPrefsDialog::writeSettings()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QKeySequence upKeyStandardSequence(ui->upKeyStandard->text());
   QKeySequence downKeyStandardSequence(ui->downKeyStandard->text());
   QKeySequence leftKeyStandardSequence(ui->leftKeyStandard->text());
   QKeySequence rightKeyStandardSequence(ui->rightKeyStandard->text());
   QKeySequence aKeyStandardSequence(ui->aKeyStandard->text());
   QKeySequence bKeyStandardSequence(ui->bKeyStandard->text());
   QKeySequence selectKeyStandardSequence(ui->selectKeyStandard->text());
   QKeySequence startKeyStandardSequence(ui->startKeyStandard->text());
   QKeySequence upKeyTurboSequence(ui->upKeyTurbo->text());
   QKeySequence downKeyTurboSequence(ui->downKeyTurbo->text());
   QKeySequence leftKeyTurboSequence(ui->leftKeyTurbo->text());
   QKeySequence rightKeyTurboSequence(ui->rightKeyTurbo->text());
   QKeySequence aKeyTurboSequence(ui->aKeyTurbo->text());
   QKeySequence bKeyTurboSequence(ui->bKeyTurbo->text());
   QKeySequence aTurboKeyTurboSequence(ui->aTurboKeyTurbo->text());
   QKeySequence bTurboKeyTurboSequence(ui->bTurboKeyTurbo->text());
   QKeySequence selectKeyTurboSequence(ui->selectKeyTurbo->text());
   QKeySequence startKeyTurboSequence(ui->startKeyTurbo->text());
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
      upKeyStandardSequence.fromString(ui->upKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_UP] = upKeyStandardSequence[0];
      downKeyStandardSequence.fromString(ui->downKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_DOWN] = downKeyStandardSequence[0];
      leftKeyStandardSequence.fromString(ui->leftKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_LEFT] = leftKeyStandardSequence[0];
      rightKeyStandardSequence.fromString(ui->rightKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT] = rightKeyStandardSequence[0];
      aKeyStandardSequence.fromString(ui->aKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_A] = aKeyStandardSequence[0];
      bKeyStandardSequence.fromString(ui->bKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_B] = bKeyStandardSequence[0];
      selectKeyStandardSequence.fromString(ui->selectKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_SELECT] = selectKeyStandardSequence[0];
      startKeyStandardSequence.fromString(ui->startKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_START] = startKeyStandardSequence[0];
   break;
   case IO_TurboJoypad:
      port = ui->controllerPortComboBox->currentIndex();
      upKeyTurboSequence.fromString(ui->upKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_UP] = upKeyTurboSequence[0];
      downKeyTurboSequence.fromString(ui->downKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_DOWN] = downKeyTurboSequence[0];
      leftKeyTurboSequence.fromString(ui->leftKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_LEFT] = leftKeyTurboSequence[0];
      rightKeyTurboSequence.fromString(ui->rightKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_RIGHT] = rightKeyTurboSequence[0];
      aKeyTurboSequence.fromString(ui->aKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_A] = aKeyTurboSequence[0];
      bKeyTurboSequence.fromString(ui->bKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_B] = bKeyTurboSequence[0];
      aTurboKeyTurboSequence.fromString(ui->aTurboKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_ATURBO] = aTurboKeyTurboSequence[0];
      bTurboKeyTurboSequence.fromString(ui->bTurboKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_BTURBO] = bTurboKeyTurboSequence[0];
      selectKeyTurboSequence.fromString(ui->selectKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_SELECT] = selectKeyTurboSequence[0];
      startKeyTurboSequence.fromString(ui->startKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_START] = startKeyTurboSequence[0];
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

   pauseOnTaskSwitch = ui->pauseOnTaskSwitch->isChecked();

   tvStandard = ui->tvStandard->currentIndex();
   pauseOnKIL = ui->pauseOnKIL->isChecked();

   square1Enabled = ui->square1->isChecked();
   square2Enabled = ui->square2->isChecked();
   triangleEnabled = ui->triangle->isChecked();
   noiseEnabled = ui->noise->isChecked();
   dmcEnabled = ui->dmc->isChecked();

   scalingFactor = ui->scalingFactor->currentIndex();
   linearInterpolation = ui->linearInterpolation->isChecked();
   aspect43 = ui->aspect43->isChecked();

   // Then save locals to QSettings.
   settings.beginGroup("EmulatorPreferences/General");
   settings.setValue("pauseOnTaskSwitch",QVariant(pauseOnTaskSwitch));
   settings.endGroup();

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
      settings.beginGroup("EmulatorPreferences/NES/ControllerConfig/TurboJoypad/Port"+QString::number(port));
      for ( function = 0; function < IO_TurboJoypad_MAX; function++ )
      {
         settings.setValue("KeyMap"+QString::number(function),turboJoypadKeyMap[port][function]);
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
   settings.setValue("EMUScalingFactor",scalingFactor);
   settings.setValue("LinearInterpolation",linearInterpolation);
   settings.setValue("EMU43Aspect",aspect43);
   settings.endGroup();

   settings.beginGroup("EmulatorPreferences/NES/System");
   settings.setValue("TVStandard",tvStandard);
   settings.setValue("PauseOnKIL",pauseOnKIL);
   settings.endGroup();
}

void EmulatorPrefsDialog::updateUi()
{
   int port = ui->controllerPortComboBox->currentIndex();
   int type = ui->controllerTypeComboBox->currentIndex();
   QKeySequence upKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_UP]);
   QKeySequence downKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_DOWN]);
   QKeySequence leftKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_LEFT]);
   QKeySequence rightKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT]);
   QKeySequence aKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_A]);
   QKeySequence bKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_B]);
   QKeySequence selectKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_SELECT]);
   QKeySequence startKeyStandardSequence(standardJoypadKeyMap[port][IO_StandardJoypad_START]);
   QKeySequence upKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_UP]);
   QKeySequence downKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_DOWN]);
   QKeySequence leftKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_LEFT]);
   QKeySequence rightKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_RIGHT]);
   QKeySequence aKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_A]);
   QKeySequence bKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_B]);
   QKeySequence aTurboKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_ATURBO]);
   QKeySequence bTurboKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_BTURBO]);
   QKeySequence selectKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_SELECT]);
   QKeySequence startKeyTurboSequence(turboJoypadKeyMap[port][IO_TurboJoypad_START]);
   QKeySequence triggerZapperKeySequence(zapperKeyMap[port][IO_Zapper_FIRE]);
   QKeySequence ccwVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_CCW]);
   QKeySequence cwVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_CW]);
   QKeySequence triggerVausKeySequence(vausArkanoidKeyMap[port][IO_Vaus_FIRE]);

   ui->controllerKeysStackedWidget->setCurrentWidget(controllerPageMap.value(ui->controllerTypeComboBox->currentText()));

   if (type == IO_StandardJoypad)
   {
      ui->upKeyStandard->setText(upKeyStandardSequence.toString());
      ui->downKeyStandard->setText(downKeyStandardSequence.toString());
      ui->leftKeyStandard->setText(leftKeyStandardSequence.toString());
      ui->rightKeyStandard->setText(rightKeyStandardSequence.toString());
      ui->aKeyStandard->setText(aKeyStandardSequence.toString());
      ui->bKeyStandard->setText(bKeyStandardSequence.toString());
      ui->selectKeyStandard->setText(selectKeyStandardSequence.toString());
      ui->startKeyStandard->setText(startKeyStandardSequence.toString());
   }
   else if (type == IO_TurboJoypad)
   {
      ui->upKeyTurbo->setText(upKeyTurboSequence.toString());
      ui->downKeyTurbo->setText(downKeyTurboSequence.toString());
      ui->leftKeyTurbo->setText(leftKeyTurboSequence.toString());
      ui->rightKeyTurbo->setText(rightKeyTurboSequence.toString());
      ui->aKeyTurbo->setText(aKeyTurboSequence.toString());
      ui->bKeyTurbo->setText(bKeyTurboSequence.toString());
      ui->aTurboKeyTurbo->setText(aTurboKeyTurboSequence.toString());
      ui->bTurboKeyTurbo->setText(bTurboKeyTurboSequence.toString());
      ui->selectKeyTurbo->setText(selectKeyTurboSequence.toString());
      ui->startKeyTurbo->setText(startKeyTurboSequence.toString());
   }
   else if (type == IO_Zapper)
   {
      ui->triggerKeyZapper->setText(triggerZapperKeySequence.toString());
      ui->mouseFiresZapper->setChecked(zapperMouseMap[port][IO_Zapper_FIRE]);
   }
   else if (type == IO_Vaus)
   {
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
      QKeySequence upKeyStandardSequence(ui->upKeyStandard->text());
      QKeySequence downKeyStandardSequence(ui->downKeyStandard->text());
      QKeySequence leftKeyStandardSequence(ui->leftKeyStandard->text());
      QKeySequence rightKeyStandardSequence(ui->rightKeyStandard->text());
      QKeySequence aKeyStandardSequence(ui->aKeyStandard->text());
      QKeySequence bKeyStandardSequence(ui->bKeyStandard->text());
      QKeySequence selectKeyStandardSequence(ui->selectKeyStandard->text());
      QKeySequence startKeyStandardSequence(ui->startKeyStandard->text());
      upKeyStandardSequence.fromString(ui->upKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_UP] = upKeyStandardSequence[0];
      downKeyStandardSequence.fromString(ui->downKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_DOWN] = downKeyStandardSequence[0];
      leftKeyStandardSequence.fromString(ui->leftKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_LEFT] = leftKeyStandardSequence[0];
      rightKeyStandardSequence.fromString(ui->rightKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_RIGHT] = rightKeyStandardSequence[0];
      aKeyStandardSequence.fromString(ui->aKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_A] = aKeyStandardSequence[0];
      bKeyStandardSequence.fromString(ui->bKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_B] = bKeyStandardSequence[0];
      selectKeyStandardSequence.fromString(ui->selectKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_SELECT] = selectKeyStandardSequence[0];
      startKeyStandardSequence.fromString(ui->startKeyStandard->text());
      standardJoypadKeyMap[port][IO_StandardJoypad_START] = startKeyStandardSequence[0];
   }
   else if ( ui->controllerTypeComboBox->currentIndex() == IO_TurboJoypad )
   {
      QKeySequence upKeyTurboSequence(ui->upKeyTurbo->text());
      QKeySequence downKeyTurboSequence(ui->downKeyTurbo->text());
      QKeySequence leftKeyTurboSequence(ui->leftKeyTurbo->text());
      QKeySequence rightKeyTurboSequence(ui->rightKeyTurbo->text());
      QKeySequence aKeyTurboSequence(ui->aKeyTurbo->text());
      QKeySequence bKeyTurboSequence(ui->bKeyTurbo->text());
      QKeySequence aTurboKeyTurboSequence(ui->aTurboKeyTurbo->text());
      QKeySequence bTurboKeyTurboSequence(ui->bTurboKeyTurbo->text());
      QKeySequence selectKeyTurboSequence(ui->selectKeyTurbo->text());
      QKeySequence startKeyTurboSequence(ui->startKeyTurbo->text());
      upKeyTurboSequence.fromString(ui->upKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_UP] = upKeyTurboSequence[0];
      downKeyTurboSequence.fromString(ui->downKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_DOWN] = downKeyTurboSequence[0];
      leftKeyTurboSequence.fromString(ui->leftKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_LEFT] = leftKeyTurboSequence[0];
      rightKeyTurboSequence.fromString(ui->rightKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_RIGHT] = rightKeyTurboSequence[0];
      aKeyTurboSequence.fromString(ui->aKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_A] = aKeyTurboSequence[0];
      bKeyTurboSequence.fromString(ui->bKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_B] = bKeyTurboSequence[0];
      aTurboKeyTurboSequence.fromString(ui->aTurboKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_ATURBO] = aTurboKeyTurboSequence[0];
      bTurboKeyTurboSequence.fromString(ui->bTurboKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_BTURBO] = bTurboKeyTurboSequence[0];
      selectKeyTurboSequence.fromString(ui->selectKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_SELECT] = selectKeyTurboSequence[0];
      startKeyTurboSequence.fromString(ui->startKeyTurbo->text());
      turboJoypadKeyMap[port][IO_TurboJoypad_START] = startKeyTurboSequence[0];
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

bool EmulatorPrefsDialog::getPauseOnTaskSwitch()
{
   return pauseOnTaskSwitch;
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
   case IO_TurboJoypad:
      keyMap = turboJoypadKeyMap[port][function];
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

void EmulatorPrefsDialog::setPauseOnTaskSwitch(bool pause)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   pauseOnTaskSwitch = pause;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/General");
   settings.setValue("pauseOnTaskSwitch",pauseOnTaskSwitch);
   settings.endGroup();
}

void EmulatorPrefsDialog::setTVStandard(int standard)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   tvStandard = standard;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/System");
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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   square1Enabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("Square1",square1Enabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setSquare2Enabled(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   square2Enabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("Square2",square2Enabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setTriangleEnabled(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   triangleEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("Triangle",triangleEnabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setNoiseEnabled(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   noiseEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("Noise",noiseEnabled);
   settings.endGroup();
}

void EmulatorPrefsDialog::setDMCEnabled(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   dmcEnabled = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Audio");
   settings.setValue("DMC",dmcEnabled);
   settings.endGroup();
}

int EmulatorPrefsDialog::getScalingFactor()
{
   return scalingFactor;
}

void EmulatorPrefsDialog::setScalingFactor(int factor)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   scalingFactor = factor;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Video");
   settings.setValue("EMUScalingFactor",scalingFactor);
   settings.endGroup();
}

bool EmulatorPrefsDialog::getLinearInterpolation()
{
   return linearInterpolation;
}

void EmulatorPrefsDialog::setLinearInterpolation(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   linearInterpolation = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Video");
   settings.setValue("LinearInterpolation",linearInterpolation);
   settings.endGroup();
}

bool EmulatorPrefsDialog::get43Aspect()
{
   return aspect43;
}

void EmulatorPrefsDialog::set43Aspect(bool enabled)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Update local storage first.
   aspect43 = enabled;

   // Now write to QSettings.
   settings.beginGroup("EmulatorPreferences/NES/Video");
   settings.setValue("EMU43Aspect",aspect43);
   settings.endGroup();
}

void EmulatorPrefsDialog::on_trimPotVaus_dialMoved(int value)
{
   QString str;
   str.sprintf("$%02X-$%02X",value,value+0xA0);
   ui->trimPotValueVaus->setText(str);
}
