#ifndef EMULATORPREFSDIALOG_H
#define EMULATORPREFSDIALOG_H

#include <QDialog>
#include <QMap>

#include "nes_emulator_core.h"

namespace Ui
{
class EmulatorPrefsDialog;
}

class EmulatorPrefsDialog : public QDialog
{
   Q_OBJECT

public:
   explicit EmulatorPrefsDialog(QString target,QWidget* parent = 0);
   virtual ~EmulatorPrefsDialog();

   // Interface to retrieve values from QSettings and store them
   // here locally.
   static void readSettings();

   // Query methods
   static bool controllerSettingsChanged() { return controllersUpdated; }
   static bool audioSettingsChanged() { return audioUpdated; }
   static bool videoSettingsChanged() { return videoUpdated; }
   static bool systemSettingsChanged() { return systemUpdated; }

   // General accessors
   static bool getPauseOnTaskSwitch();

   // NES accessors
   static int getControllerType(int port);
   static int getControllerKeyMap(int port,int function);
   static bool getControllerMouseMap(int port,int function);
   static int getControllerSpecial(int port);
   static int getTVStandard();
   static bool getPauseOnKIL();
   static bool getSquare1Enabled() { return square1Enabled; }
   static bool getSquare2Enabled() { return square2Enabled; }
   static bool getTriangleEnabled() { return triangleEnabled; }
   static bool getNoiseEnabled() { return noiseEnabled; }
   static bool getDMCEnabled() { return dmcEnabled; }
   static bool getPulse1VRC6Enabled() { return pulse1VRC6Enabled; }
   static bool getPulse2VRC6Enabled() { return pulse2VRC6Enabled; }
   static bool getSawtoothVRC6Enabled() { return sawtoothVRC6Enabled; }
   static bool getSquare1MMC5Enabled() { return square1MMC5Enabled; }
   static bool getSquare2MMC5Enabled() { return square2MMC5Enabled; }
   static bool getDMCMMC5Enabled() { return dmcMMC5Enabled; }
   static bool getWave1N106Enabled() { return wave1N106Enabled; }
   static bool getWave2N106Enabled() { return wave2N106Enabled; }
   static bool getWave3N106Enabled() { return wave3N106Enabled; }
   static bool getWave4N106Enabled() { return wave4N106Enabled; }
   static bool getWave5N106Enabled() { return wave5N106Enabled; }
   static bool getWave6N106Enabled() { return wave6N106Enabled; }
   static bool getWave7N106Enabled() { return wave7N106Enabled; }
   static bool getWave8N106Enabled() { return wave8N106Enabled; }
   static int getScalingFactor();
   static bool getLinearInterpolation();
   static bool get43Aspect();

   // C=64 accessors
   static QString getVICEExecutable();
   static QString getVICEIPAddress();
   static int getVICEMonitorPort();
   static QString getVICEOptions();
   static QString getC64KernalROM();
   static QString getC64BasicROM();
   static QString getC64CharROM();
   static QString getViceInvocation();

   // Modifiers (only provided for settings that are also found in menus not just in this dialog)
   static void setTVStandard(int standard);
   static void setSquare1Enabled(bool enabled);
   static void setSquare2Enabled(bool enabled);
   static void setTriangleEnabled(bool enabled);
   static void setNoiseEnabled(bool enabled);
   static void setDMCEnabled(bool enabled);
   static void setPulse1VRC6Enabled(bool enabled);
   static void setPulse2VRC6Enabled(bool enabled);
   static void setSawtoothVRC6Enabled(bool enabled);
   static void setSquare1MMC5Enabled(bool enabled);
   static void setSquare2MMC5Enabled(bool enabled);
   static void setDMCMMC5Enabled(bool enabled);
   static void setWave1N106Enabled(bool enabled);
   static void setWave2N106Enabled(bool enabled);
   static void setWave3N106Enabled(bool enabled);
   static void setWave4N106Enabled(bool enabled);
   static void setWave5N106Enabled(bool enabled);
   static void setWave6N106Enabled(bool enabled);
   static void setWave7N106Enabled(bool enabled);
   static void setWave8N106Enabled(bool enabled);
   static void setScalingFactor(int factor);
   static void setPauseOnTaskSwitch(bool pause);
   static void setLinearInterpolation(bool enabled);
   static void set43Aspect(bool enabled);

private:
   Ui::EmulatorPrefsDialog* ui;
   QMap<QString,QWidget*> controllerPageMap;
   QString m_targetLoaded;

   // Interface to store values to QSettings from local storage.
   void writeSettings();

   // General data structures.
   static bool pauseOnTaskSwitch;

   // NES settings data structures.
   static int controllerType[NUM_CONTROLLERS];
   static int standardJoypadKeyMap[NUM_CONTROLLERS][IO_StandardJoypad_MAX];
   static int turboJoypadKeyMap[NUM_CONTROLLERS][IO_TurboJoypad_MAX];
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
   static bool pulse1VRC6Enabled;
   static bool pulse2VRC6Enabled;
   static bool sawtoothVRC6Enabled;
   static bool square1MMC5Enabled;
   static bool square2MMC5Enabled;
   static bool dmcMMC5Enabled;
   static bool wave1N106Enabled;
   static bool wave2N106Enabled;
   static bool wave3N106Enabled;
   static bool wave4N106Enabled;
   static bool wave5N106Enabled;
   static bool wave6N106Enabled;
   static bool wave7N106Enabled;
   static bool wave8N106Enabled;
   static int scalingFactor;
   static bool linearInterpolation;
   static bool aspect43;

   // C=64 settings data structures.
   static QString viceExecutable;
   static QString viceIPAddress;
   static int viceMonitorPort;
   static QString viceOptions;
   static QString c64KernalROM;
   static QString c64BasicROM;
   static QString c64CharROM;

   // Query flags.
   static bool controllersUpdated;
   static bool audioUpdated;
   static bool videoUpdated;
   static bool systemUpdated;

   void updateUi();
   void updateDb();
   void updateViceInvocation();

private slots:
   void on_c64CharROMBrowse_clicked();
   void on_c64BasicROMBrowse_clicked();
   void on_c64KernalROMBrowse_clicked();
   void on_controllerTypeComboBox_highlighted(int index);
   void on_controllerPortComboBox_highlighted(int index);
   void on_buttonBox_accepted();
   void on_controllerPortComboBox_currentIndexChanged(int index);
   void on_controllerTypeComboBox_currentIndexChanged(int index );
   void on_viceC64Browse_clicked();
   void on_viceOptions_textChanged();
   void on_viceC64MonitorIPAddress_textChanged(const QString &arg1);
   void on_viceC64MonitorPort_textChanged(const QString &arg1);
   void on_trimPotVaus_sliderMoved(int position);
};

#endif // EMULATORPREFSDIALOG_H
