/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2012  Jonathan Liss
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

/*
 *  Add new program settings to the SetupSettings function,
 *  three macros are provided for the type of setting you want to add.
 *  (SETTING_INT, SETTING_BOOL, SETTING_STRING)
 *
 */

#include <QSettings>
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "Settings.h"
#include "ColorScheme.h"

#define SETTING_INT(Section, Entry, Default, Variable)	\
	AddSetting(new CSettingInt(_T(Section), _T(Entry), Default, Variable))	\

#define SETTING_BOOL(Section, Entry, Default, Variable)	\
	AddSetting(new CSettingBool(_T(Section), _T(Entry), Default, Variable))	\

#define SETTING_STRING(Section, Entry, Default, Variable)	\
	AddSetting(new CSettingString(_T(Section), _T(Entry), Default, Variable))	\

// CSettings

CSettings::CSettings() : m_iAddedSettings(0)
{
	m_bNamcoMixing = false;

	memset(m_pSettings, 0, sizeof(CSettingBase*) * MAX_SETTINGS);
	SetupSettings();
	ATLTRACE2(atlTraceGeneral, 0, "Settings: Added %d settings\n", m_iAddedSettings);	// debug
}

CSettings::~CSettings()
{
	// Release all settings
	for (int i = 0; i < m_iAddedSettings; ++i) {
		SAFE_RELEASE(m_pSettings[i]);
	}
}

void CSettings::SetupSettings()
{
	//
	// This function defines all settings in the program that are stored in registry
	// All settings are loaded on program start and saved when closing the program
	//

	// The SETTING macros takes four arguments: 
	//
	//  1. Registry section
	//  2. Registry key name
	//  3. Default value
	//  4. A variable that contains the setting, loaded on program startup and saved on shutdown
	//

	// General
	SETTING_INT("General", "Edit style", EDIT_STYLE1, &General.iEditStyle);
	SETTING_INT("General", "Page step size", 4, &General.iPageStepSize);
	SETTING_BOOL("General", "Wrap cursor", true, &General.bWrapCursor);
	SETTING_BOOL("General", "Wrap across frames", true, &General.bWrapFrames);
	SETTING_BOOL("General", "Free cursor edit",	false, &General.bFreeCursorEdit);
	SETTING_BOOL("General", "Wave preview", true, &General.bWavePreview);
	SETTING_BOOL("General", "Key repeat", true, &General.bKeyRepeat);
	SETTING_BOOL("General", "Hex row display", true, &General.bRowInHex);
	SETTING_BOOL("General", "Frame preview", true, &General.bFramePreview);
	SETTING_BOOL("General", "No DPCM reset", false, &General.bNoDPCMReset);
	SETTING_BOOL("General", "No Step moving", false, &General.bNoStepMove);
	SETTING_BOOL("General", "Pattern colors", true, &General.bPatternColor);
	SETTING_BOOL("General", "Delete pull up", false, &General.bPullUpDelete);
	SETTING_BOOL("General", "Backups", false, &General.bBackups);
	SETTING_STRING("General", "Pattern font", FONT_FACE, &General.strFont);
	SETTING_INT("General", "Pattern font size", FONT_SIZE, &General.iFontSize);
	SETTING_BOOL("General", "Single instance", false, &General.bSingleInstance);

	// Keys
	SETTING_INT("Keys", "Note cut",		0x31, &Keys.iKeyNoteCut);
	SETTING_INT("Keys", "Note release", 0xDC, &Keys.iKeyNoteRelease);
	SETTING_INT("Keys", "Clear field",	0xBD, &Keys.iKeyClear);
	SETTING_INT("Keys", "Repeat",		0x00, &Keys.iKeyRepeat);

	// Sound
	SETTING_INT("Sound", "Audio Device", 0, &Sound.iDevice);
	SETTING_INT("Sound", "Sample rate",	44100, &Sound.iSampleRate);
	SETTING_INT("Sound", "Sample size", 16, &Sound.iSampleSize);
	SETTING_INT("Sound", "Buffer length", 40, &Sound.iBufferLength);
	SETTING_INT("Sound", "Bass filter freq", 16, &Sound.iBassFilter);
	SETTING_INT("Sound", "Treble filter freq", 12000, &Sound.iTrebleFilter);
	SETTING_INT("Sound", "Treble filter damping", 24, &Sound.iTrebleDamping);
	SETTING_INT("Sound", "Volume", 100, &Sound.iMixVolume);

	// Midi
	SETTING_INT("MIDI", "Device", 0, &Midi.iMidiDevice);
	SETTING_INT("MIDI", "Out Device", 0, &Midi.iMidiOutDevice);
	SETTING_BOOL("MIDI", "Master sync", false, &Midi.bMidiMasterSync);
	SETTING_BOOL("MIDI", "Key release", false, &Midi.bMidiKeyRelease);
	SETTING_BOOL("MIDI", "Channel map", false, &Midi.bMidiChannelMap);
	SETTING_BOOL("MIDI", "Velocity control", false,	&Midi.bMidiVelocity);
	SETTING_BOOL("MIDI", "Auto Arpeggio", false, &Midi.bMidiArpeggio);

	// Appearance	
	SETTING_INT("Appearance", "Background", DEFAULT_COLOR_SCHEME.BACKGROUND, &Appearance.iColBackground);
	SETTING_INT("Appearance", "Background highlighted", DEFAULT_COLOR_SCHEME.BACKGROUND_HILITE, &Appearance.iColBackgroundHilite);
	SETTING_INT("Appearance", "Background highlighted 2", DEFAULT_COLOR_SCHEME.BACKGROUND_HILITE2, &Appearance.iColBackgroundHilite2);
	SETTING_INT("Appearance", "Pattern text", DEFAULT_COLOR_SCHEME.TEXT_NORMAL, &Appearance.iColPatternText);
	SETTING_INT("Appearance", "Pattern text highlighted", DEFAULT_COLOR_SCHEME.TEXT_HILITE, &Appearance.iColPatternTextHilite);
	SETTING_INT("Appearance", "Pattern text highlighted 2", DEFAULT_COLOR_SCHEME.TEXT_HILITE2, &Appearance.iColPatternTextHilite2);
	SETTING_INT("Appearance", "Pattern instrument", DEFAULT_COLOR_SCHEME.TEXT_INSTRUMENT, &Appearance.iColPatternInstrument);
	SETTING_INT("Appearance", "Pattern volume", DEFAULT_COLOR_SCHEME.TEXT_VOLUME, &Appearance.iColPatternVolume);
	SETTING_INT("Appearance", "Pattern effect", DEFAULT_COLOR_SCHEME.TEXT_EFFECT, &Appearance.iColPatternEffect);
	SETTING_INT("Appearance", "Selection", DEFAULT_COLOR_SCHEME.SELECTION, &Appearance.iColSelection);
	SETTING_INT("Appearance", "Cursor", DEFAULT_COLOR_SCHEME.CURSOR, &Appearance.iColCursor);
	
	// Window position
	SETTING_INT("Window position", "Left", 100, &WindowPos.iLeft);
	SETTING_INT("Window position", "Top", 100, &WindowPos.iTop);
	SETTING_INT("Window position", "Right",	950, &WindowPos.iRight);
	SETTING_INT("Window position", "Bottom", 920, &WindowPos.iBottom);
	SETTING_INT("Window position", "State",	STATE_NORMAL, &WindowPos.iState);

	// Other
	SETTING_INT("Other", "Sample window state", 0, &SampleWinState);
	SETTING_INT("Other", "Frame editor position", 0, &FrameEditPos);

	// Paths
	SETTING_STRING("Paths", "FTM path", _T(""), &Paths[PATH_FTM]);
	SETTING_STRING("Paths", "FTI path", _T(""), &Paths[PATH_FTI]);
	SETTING_STRING("Paths", "NSF path", _T(""), &Paths[PATH_NSF]);
	SETTING_STRING("Paths", "DMC path", _T(""), &Paths[PATH_DMC]);
	SETTING_STRING("Paths", "WAV path", _T(""), &Paths[PATH_WAV]);

	SETTING_STRING("Paths", "Instrument menu", _T(""), &InstrumentMenuPath);

	/*
	SETTING_INT("Sound levels", "2A03", 0, &ChipLevels.iLevel2A03);
	SETTING_INT("Sound levels", "VRC6", 0, &ChipLevels.iLevelVRC6);
	SETTING_INT("Sound levels", "VRC7", 0, &ChipLevels.iLevelVRC7);
	SETTING_INT("Sound levels", "MMC5", 0, &ChipLevels.iLevelMMC5);
	SETTING_INT("Sound levels", "FDS", 0, &ChipLevels.iLevelFDS);
	*/
}

void CSettings::AddSetting(CSettingBase *pSetting)
{
	ASSERT(m_iAddedSettings < MAX_SETTINGS);
	m_pSettings[m_iAddedSettings++] = pSetting;
}


// CSettings member functions

void CSettings::LoadSettings()
{
	for (int i = 0; i < m_iAddedSettings; ++i) {
		m_pSettings[i]->Load();
	}

	m_bNamcoMixing = LoadSetting(_T("Emulation"), _T("Linear Namco mixing"), 0) == 1;
}

void CSettings::SaveSettings()
{
	for (int i = 0; i < m_iAddedSettings; ++i) {
		m_pSettings[i]->Save();
	}
}

void CSettings::DefaultSettings()
{
	for (int i = 0; i < m_iAddedSettings; ++i) {
		m_pSettings[i]->Default();
	}
}

void CSettings::DeleteSettings()
{
	// Delete all settings from registry
//	HKEY hKey = theApp.GetAppRegistryKey();
//	theApp.DelRegTree(hKey, _T(""));
}

void CSettings::SetWindowPos(int Left, int Top, int Right, int Bottom, int State)
{
	WindowPos.iLeft = Left;
	WindowPos.iTop = Top;
	WindowPos.iRight = Right;
	WindowPos.iBottom = Bottom;
	WindowPos.iState = State;
}

CString CSettings::GetPath(unsigned int PathType) const
{
	ASSERT(PathType < PATH_COUNT);
	return Paths[PathType];
}

void CSettings::SetPath(CString PathName, unsigned int PathType)
{
	ASSERT(PathType < PATH_COUNT);

   qDebug("SetPath");
//	// Remove file name if there is a
//	if (PathName.Right(1) == _T("\\") || PathName.Find(_T('\\')) == -1)
//		Paths[PathType] = PathName;
//	else
//		Paths[PathType] = PathName.Left(PathName.ReverseFind(_T('\\')));
}

void CSettings::StoreSetting(CString Section, CString Name, int Value) const
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = Section.GetBuffer();
   key += "/";
   key += Name.GetBuffer();
//   qDebug("StoreSetting");
//   qDebug(key.toAscii().constData());
   
   settings.setValue(key,Value);
}

int CSettings::LoadSetting(CString Section, CString Name, int Default) const
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = Section.GetBuffer();
   key += "/";
   key += Name.GetBuffer();
//   qDebug("LoadSetting");
//   qDebug(key.toAscii().constData());
   
   return settings.value(key,Default).toInt();
}

// Settings types

void CSettingBool::Load()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingBool::Load");
//   qDebug(key.toAscii().constData());
   
   *(bool*)m_pVariable = settings.value(key,QVariant(m_bDefaultValue)).toBool();
}

void CSettingBool::Save()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingBool::Save");
//   qDebug(key.toAscii().constData());
   
   settings.setValue(key,QVariant(*(bool*)m_pVariable));
}

void CSettingBool::Default()
{
	*(bool*)m_pVariable = m_bDefaultValue;
}

void CSettingInt::Load()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingInt::Load");
//   qDebug(key.toAscii().constData());
   
   *(int*)m_pVariable = settings.value(key,QVariant(m_iDefaultValue)).toInt();
}

void CSettingInt::Save()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingInt::Save");
//   qDebug(key.toAscii().constData());
   
   settings.setValue(key,QVariant(*(int*)m_pVariable));
}

void CSettingInt::Default()
{
	*(int*)m_pVariable = m_iDefaultValue;
}

void CSettingString::Load()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingString::Load");
//   qDebug(key.toAscii().constData());
   
   (*(CString*)m_pVariable) = settings.value(key,QString::fromUtf16((const ushort*)m_pDefaultValue)).toString();
}

void CSettingString::Save()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString key;
   key = m_pSection;
   key += "/";
   key += m_pEntry;
//   qDebug("CSettingString::Save");
//   qDebug(key.toAscii().constData());
   
   settings.setValue(key,(*(CString*)m_pVariable).GetBuffer());
}

void CSettingString::Default()
{
	(*(CString*)m_pVariable) = (TCHAR*)m_pDefaultValue;
}
