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

// FamiTracker.h : main header file for the FamiTracker application
//

#pragma once

#include "cqtmfc.h"

#include "version.h"
// Enable export verification
//#define EXPORT_TEST

//#define LIMIT(v, max, min) if (v > max) v = max; else if (v < min) v = min;
#define LIMIT(v, max, min) v = ((v > max) ? max : ((v < min) ? min : v));//  if (v > max) v = max; else if (v < min) v = min;

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "resource.h"       // main symbols

// Inter-process commands
enum {
	IPC_LOAD = 1,	
	IPC_LOAD_PLAY
};

// Custom command line reader
class CFTCommandLineInfo : public CCommandLineInfo
{
public:
	CFTCommandLineInfo();
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
public:
	bool m_bLog;
	bool m_bExport;
	bool m_bPlay;
	CString m_strExportFile;
	CString m_strExportLogFile;
	CString m_strExportDPCMFile;
};


//class CMIDI;
class CSoundGen;
class CSettings;
class CAccelerator;
class CChannelMap;
class CCustomExporters;

class CMutex;

// CFamiTrackerApp:
// See FamiTracker.cpp for the implementation of this class
//

class CFamiTrackerApp : public CWinApp
{
   Q_OBJECT
public:
   void menuAction_triggered(int id);

   // Constructor
	CFamiTrackerApp();

	//
	// Public functions
	//
public:
	void			LoadSoundConfig();
	void			ReloadColorScheme(void);
	int				GetCPUUsage() const;
	bool			IsThemeActive() const;
	void			CheckSynth();
	void			RemoveSoundGenerator();

	// Tracker player functions
	bool			IsPlaying() const;
	void			ResetPlayer();
	void			SilentEverything();
	void			WaitUntilStopped() const;

	// Get-functions
	CAccelerator	*GetAccelerator() const { return m_pAccel; };
	CSoundGen		*GetSoundGenerator() const { return m_pSoundGenerator; };
//	CMIDI			*GetMIDI() const { return m_pMIDI; };
	CSettings		*GetSettings() const { return m_pSettings; };
	CChannelMap		*GetChannelMap() const { return m_pChannelMap; };
	
	CCustomExporters *GetCustomExporters() const;

	// Try to avoid these
//	CDocument		*GetActiveDocument() const;
//	CView			*GetActiveView() const;

#ifdef EXPORT_TEST
	void			VerifyExport();
#endif /* EXPORT_TEST */

//	//
//	// Private functions
//	//
//private:
//	void CheckAppThemed();
	void ShutDownSynth();
	bool CheckSingleInstance(CFTCommandLineInfo &cmdInfo);
	void RegisterSingleInstance();
	void UnregisterSingleInstance();
//	void CheckNewVersion();
	void CommandLineExport(const CString& fileIn, const CString& fileOut, const CString& fileLog, const CString& fileDPCM);

protected:
	BOOL DoPromptFileName(CString& fileName, CString& filePath, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);

	// Private variables and objects
private:
	// Objects
//	CMIDI			*m_pMIDI;
	CAccelerator	*m_pAccel;					// Keyboard accelerator
	CSoundGen		*m_pSoundGenerator;			// Sound synth & player
	CSettings		*m_pSettings;				// Program settings
	CChannelMap		*m_pChannelMap;

	CCustomExporters *m_customExporters;

	// Single instance stuff
	CMutex			*m_pInstanceMutex;
	HANDLE			m_hWndMapFile;

	bool			m_bThemeActive;

	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();	

	// Implementation
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAppAbout();
	afx_msg void OnTrackerTogglePlay();
	afx_msg void OnTrackerPlay();
	afx_msg void OnTrackerPlayStart();
	afx_msg void OnTrackerPlayCursor();
	afx_msg void OnTrackerPlaypattern();
	afx_msg void OnTrackerStop();
	afx_msg void OnFileOpen();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CFamiTrackerApp theApp;

// Global helper functions
CString LoadDefaultFilter(LPCTSTR Name, LPCTSTR Ext);
