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

#pragma once

// CSettings command target

enum EDIT_STYLES {
	EDIT_STYLE1 = 0,		// FT2
	EDIT_STYLE2 = 1,		// ModPlug
	EDIT_STYLE3 = 2			// IT
};

enum WIN_STATES {
	STATE_NORMAL,
	STATE_MAXIMIZED
};

enum PATHS {
	PATH_FTM,
	PATH_FTI,
	PATH_NSF,
	PATH_DMC,
	PATH_WAV,

	PATH_COUNT
};


// Base class for settings, pure virtual
class CSettingBase {
public:
	CSettingBase(LPCTSTR pSection, LPCTSTR pEntry, void *pVariable) : m_pSection(pSection), m_pEntry(pEntry), m_pVariable(pVariable) {};
	virtual void Load() = 0;
	virtual void Save() = 0;
	virtual void Default() = 0;
	LPCTSTR GetSection() const { return m_pSection; };
protected:
	LPCTSTR m_pSection;
	LPCTSTR m_pEntry;
	void *m_pVariable;
};

// Bool setting
class CSettingBool : public CSettingBase {
public:
	CSettingBool(LPCTSTR pSection, LPCTSTR pEntry, bool defaultVal, void *pVar) : CSettingBase(pSection, pEntry, pVar), m_bDefaultValue(defaultVal) {};
	virtual void Load();
	virtual void Save();
	virtual void Default();
protected:
	bool m_bDefaultValue;
};

// Integer setting
class CSettingInt : public CSettingBase {
public:
	CSettingInt(LPCTSTR pSection, LPCTSTR pEntry, int defaultVal, void *pVar) : CSettingBase(pSection, pEntry, pVar), m_iDefaultValue(defaultVal) {};
	virtual void Load();
	virtual void Save();
	virtual void Default();
protected:
	int m_iDefaultValue;
};

// String setting
class CSettingString : public CSettingBase {
public:
	CSettingString(LPCTSTR pSection, LPCTSTR pEntry, LPCTSTR pDefault, void *pVar) : CSettingBase(pSection, pEntry, pVar), m_pDefaultValue(pDefault) {};
	virtual void Load();
	virtual void Save();
	virtual void Default();
protected:
	LPCTSTR m_pDefaultValue;
};

// Settings collection
class CSettings : public CObject
{
public:
	CSettings();
	virtual ~CSettings();

	void	LoadSettings();
	void	SaveSettings();
	void	DefaultSettings();
	void	DeleteSettings();
	void	SetWindowPos(int Left, int Top, int Right, int Bottom, int State);

	void	StoreSetting(CString Section, CString Name, int Value) const;
	int		LoadSetting(CString Section, CString Name, int Default) const;

	CString GetPath(unsigned int PathType) const;
	void	SetPath(CString PathName, unsigned int PathType);

public:
	// Local cache of all settings (all public)

	struct {
		bool	bWrapCursor;
		bool	bWrapFrames;
		bool	bFreeCursorEdit;
		bool	bWavePreview;
		bool	bKeyRepeat;
		bool	bRowInHex;
		bool	bFramePreview;
		int		iEditStyle;
		bool	bNoDPCMReset;
		bool	bNoStepMove;
		int		iPageStepSize;
		CString	strFont;
		bool	bPatternColor;
		bool	bPullUpDelete;
		bool	bBackups;
		int		iFontSize;
		bool	bSingleInstance;
	} General;

	struct {
		int		iDevice;
		int		iSampleRate;
		int		iSampleSize;
		int		iBufferLength;
		int		iBassFilter;
		int		iTrebleFilter;
		int		iTrebleDamping;
		int		iMixVolume;
	} Sound;

	struct {
		int		iMidiDevice;
		int		iMidiOutDevice;
		bool	bMidiMasterSync;
		bool	bMidiKeyRelease;
		bool	bMidiChannelMap;
		bool	bMidiVelocity;
		bool	bMidiArpeggio;
	} Midi;

	struct {
		int		iColBackground;
		int		iColBackgroundHilite;
		int		iColBackgroundHilite2;
		int		iColPatternText;
		int		iColPatternTextHilite;
		int		iColPatternTextHilite2;
		int		iColPatternInstrument;
		int		iColPatternVolume;
		int		iColPatternEffect;
		int		iColSelection;
		int		iColCursor;
	} Appearance;

	struct {
		int		iLeft;
		int		iTop;
		int		iRight;
		int		iBottom;
		int		iState;
	} WindowPos;

	struct {
		int		iKeyNoteCut;
		int		iKeyNoteRelease;
		int		iKeyClear;
		int		iKeyRepeat;
	} Keys;

	// Other
	int SampleWinState;
	int FrameEditPos;

	struct {
		int		iLevel2A03;
		int		iLevelVRC6;
		int		iLevelVRC7;
		int		iLevelMMC5;
		int		iLevelFDS;
	} ChipLevels;

	bool m_bNamcoMixing;

	CString InstrumentMenuPath;

private:
	void AddSetting(CSettingBase *pSetting);
	void SetupSettings();

private:
	static const int MAX_SETTINGS = 128;

private:
	CSettingBase *m_pSettings[MAX_SETTINGS];
	int m_iAddedSettings;

private:
	// Paths
	CString Paths[PATH_COUNT];
};


