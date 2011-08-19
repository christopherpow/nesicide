#ifndef EMULATORPREFSDIALOG_H
#define EMULATORPREFSDIALOG_H

#include <QDialog>

#include "emulator_core.h"

namespace Ui
{
class EmulatorPrefsDialog;
}

class EmulatorPrefsDialog : public QDialog
{
   Q_OBJECT

public:
   explicit EmulatorPrefsDialog(QWidget* parent = 0);
   virtual ~EmulatorPrefsDialog();

   // Interface to retrieve values from QSettings and store them
   // here locally.
   static void readSettings();

   // Query methods
   static bool controllerSettingsChanged() { return controllersUpdated; }
   static bool audioSettingsChanged() { return audioUpdated; }
   static bool videoSettingsChanged() { return videoUpdated; }
   static bool systemSettingsChanged() { return systemUpdated; }

   // Accessors
   static int getControllerType(int port);
   static int getControllerKeyMap(int port,int function);
   static bool getControllerMouseMap(int port,int function);
   static int getControllerSpecial(int port);
   static int getTVStandard();
   static bool getPauseOnKIL();
   static bool getSquare1Enabled();
   static bool getSquare2Enabled();
   static bool getTriangleEnabled();
   static bool getNoiseEnabled();
   static bool getDMCEnabled();
   static int getScalingFactor();

   // Modifiers (only provided for settings that are also found in menus not just in this dialog)
   static void setTVStandard(int standard);
   static void setSquare1Enabled(bool enabled);
   static void setSquare2Enabled(bool enabled);
   static void setTriangleEnabled(bool enabled);
   static void setNoiseEnabled(bool enabled);
   static void setDMCEnabled(bool enabled);
   static void setScalingFactor(int factor);

private:
   Ui::EmulatorPrefsDialog* ui;

   // Interface to store values to QSettings from local storage.
   void writeSettings();

   // Settings data structures.
   static int lastActiveTab;
   static int controllerType[NUM_CONTROLLERS];
   static int standardJoypadKeyMap[NUM_CONTROLLERS][IO_StandardJoypad_MAX];
   static int zapperKeyMap[NUM_CONTROLLERS][IO_Zapper_MAX];
   static bool zapperMouseMap[NUM_CONTROLLERS][IO_Zapper_MAX];
   static int vausArkanoidKeyMap[NUM_CONTROLLERS][IO_Vaus_MAX];
   static bool vausArkanoidMouseMap[NUM_CONTROLLERS][IO_Vaus_MAX];
   static int vausArkanoidTrimPot[NUM_CONTROLLERS];
   static int tvStandard;
   static bool pauseOnKIL;
   static bool square1Enabled;
   static bool square2Enabled;
   static bool triangleEnabled;
   static bool noiseEnabled;
   static bool dmcEnabled;
   static int scalingFactor;

   // Query flags.
   static bool controllersUpdated;
   static bool audioUpdated;
   static bool videoUpdated;
   static bool systemUpdated;

   void updateUi();
   void updateDb();

private slots:
   void on_trimPotVaus_dialMoved(int value);
   void on_controllerTypeComboBox_highlighted(int index);
   void on_controllerPortComboBox_highlighted(int index);
   void on_buttonBox_accepted();
   void on_controllerPortComboBox_currentIndexChanged(int index);
   void on_controllerTypeComboBox_currentIndexChanged(int index );
};

#endif // EMULATORPREFSDIALOG_H
