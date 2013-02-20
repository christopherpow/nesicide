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


#include "stdafx.h"
//#include "Exception.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrm.h"
//#include "AboutDlg.h"
#include "TrackerChannel.h"
//#include "MIDI.h"
#include "SoundGen.h"
//#include "Accelerator.h"
#include "Settings.h"
#include "ChannelMap.h"
//#include "CustomExporters.h"

CFamiTrackerApp::CFamiTrackerApp()
{
}

CFamiTrackerApp::~CFamiTrackerApp()
{
}

void CFamiTrackerApp::ReloadColorScheme(void)
{
   qDebug("ReloadColorScheme");
//	// Main window
//	CMainFrame *pMainFrm = dynamic_cast<CMainFrame*>(GetMainWnd());

//	if (pMainFrm != NULL)
//		pMainFrm->SetupColors();

//	// Notify all views	
//	POSITION TemplatePos = GetFirstDocTemplatePosition();
//	CDocTemplate *pDocTemplate = GetNextDocTemplate(TemplatePos);
//	POSITION DocPos = pDocTemplate->GetFirstDocPosition();

//	while (CDocument* pDoc = pDocTemplate->GetNextDoc(DocPos)) {
//		POSITION ViewPos = pDoc->GetFirstViewPosition();
//		while (CView *pView = pDoc->GetNextView(ViewPos)) {
//			static_cast<CFamiTrackerView*>(pView)->SetupColors();
//		}
//	}
}

// Player interface

// Silences everything
void CFamiTrackerApp::SilentEverything()
{
   GetSoundGenerator()->SilentAll();
   CFamiTrackerView::GetView()->MakeSilent();
}

void CFamiTrackerApp::RegisterKeyState(int Channel, int Note)
{
   CFamiTrackerView *pView = CFamiTrackerView::GetView();

	if (pView)
		pView->RegisterKeyState(Channel, Note);
}

// CFamiTrackerApp message handlers

void CFamiTrackerApp::OnTrackerPlay()
{
	// Play
   qDebug("theApp.play");
	if (m_pSoundGenerator)
		m_pSoundGenerator->StartPlayer(MODE_PLAY);
}

void CFamiTrackerApp::OnTrackerPlaypattern()
{
	// Loop pattern
	if (m_pSoundGenerator)
		m_pSoundGenerator->StartPlayer(MODE_PLAY_REPEAT);
}

void CFamiTrackerApp::OnTrackerPlayStart()
{
	// Play from start of song
	if (m_pSoundGenerator)
		m_pSoundGenerator->StartPlayer(MODE_PLAY_START);
}

void CFamiTrackerApp::OnTrackerPlayCursor()
{
	// Play from cursor
	if (m_pSoundGenerator)
		m_pSoundGenerator->StartPlayer(MODE_PLAY_CURSOR);
}

void CFamiTrackerApp::OnTrackerTogglePlay()
{
	if (m_pSoundGenerator) {
		if (m_pSoundGenerator->IsPlaying())
			OnTrackerStop();
		else
			OnTrackerPlay();
	}
}

void CFamiTrackerApp::OnTrackerStop()
{
	// Stop tracker
	if (m_pSoundGenerator)
		m_pSoundGenerator->StopPlayer();

   qDebug("m_pMIDI->ResetOutput");
//	m_pMIDI->ResetOutput();
}

// Player interface

bool CFamiTrackerApp::IsPlaying() const
{
	if (m_pSoundGenerator)
		return m_pSoundGenerator->IsPlaying();

	return false;
}

void CFamiTrackerApp::ResetPlayer()
{
	if (m_pSoundGenerator)
		m_pSoundGenerator->ResetPlayer();
}

void CFamiTrackerApp::RemoveSoundGenerator()
{
	// Sound generator object is deleted
	m_pSoundGenerator = NULL;
}

BOOL CFamiTrackerApp::InitInstance()
{
//	// InitCommonControls() is required on Windows XP if an application
//	// manifest specifies use of ComCtl32.dll version 6 or later to enable
//	// visual styles.  Otherwise, any window creation will fail.
//	InitCommonControls();

	CWinApp::InitInstance();
/*
	if (!AfxOleInit()) {
		TRACE0("OLE initialization failed\n");
	}
*/
//	// Standard initialization
//	// If you are not using these features and wish to reduce the size
//	// of your final executable, you should remove from the following
//	// the specific initialization routines you do not need
//	// Change the registry key under which our settings are stored
//	// TODO: You should modify this string to be something appropriate
//	// such as the name of your company or organization
//	SetRegistryKey(_T(""));
//	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Load program settings
	m_pSettings = new CSettings();
	m_pSettings->LoadSettings();

//	if (CheckSingleInstance())
//		return FALSE;

//	//who: added by Derek Andrews <derek.george.andrews@gmail.com>
//	//why: Load all custom exporter plugins on startup.
	
//	TCHAR pathToPlugins[MAX_PATH];
//	GetModuleFileName(NULL, pathToPlugins, MAX_PATH);
//	PathRemoveFileSpec(pathToPlugins);
//	PathAppend(pathToPlugins, _T("\\Plugins"));
//	m_customExporters = new CCustomExporters( pathToPlugins );

//	// Load custom accelerator
//	m_pAccel = new CAccelerator();
//	m_pAccel->LoadShortcuts(m_pSettings);
//	m_pAccel->Setup();

//	// Create the MIDI interface
//	m_pMIDI = new CMIDI();

	// Create sound generator
	m_pSoundGenerator = new CSoundGen();

	// Create channel map
	m_pChannelMap = new CChannelMap();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME, RUNTIME_CLASS(CFamiTrackerDoc), RUNTIME_CLASS(CMainFrame), RUNTIME_CLASS(CFamiTrackerView));

	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

   qDebug("Until we call OnFileNew...do this...");
   m_pMainWnd = m_pDocTemplate->m_pFrameWnd;
   
//	// Determine windows version
//    OSVERSIONINFO osvi;

//    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
//    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

//    GetVersionEx(&osvi);

//	// Work-around to enable file type registration in windows vista/7
//	if (osvi.dwMajorVersion >= 6) { 
//		HKEY HKCU;
//		long res_reg = ::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Classes"), &HKCU);
//		if(res_reg == ERROR_SUCCESS)
//			res_reg = RegOverridePredefKey(HKEY_CLASSES_ROOT, HKCU);
//	}

//	// Enable DDE Execute open
//	EnableShellOpen();

//	// Skip this if in wip/beta mode
//#if /*!defined(WIP) &&*/ !defined(_DEBUG)
//	// Add shell options
//	RegisterShellFileTypes(TRUE);
//	// Add an option to play files
//	CString strPathName, strTemp, strFileTypeId;
//	AfxGetModuleShortFileName(AfxGetInstanceHandle(), strPathName);
//	CString strOpenCommandLine = strPathName;
//	strOpenCommandLine += _T(" /play \"%1\"");
//	if (pDocTemplate->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId) && !strFileTypeId.IsEmpty()) {
//		strTemp.Format(_T("%s\\shell\\play\\%s"), (LPCTSTR)strFileTypeId, _T("command"));
//		AfxRegSetValue(HKEY_CLASSES_ROOT, strTemp, REG_SZ, strOpenCommandLine, lstrlen(strOpenCommandLine) * sizeof(TCHAR));
//	}
//#endif
	
//	// Parse command line for standard shell commands, DDE, file open + some custom ones
//	CFTCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

//	// Dispatch commands specified on the command line.  Will return FALSE if
//	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
//	if (!ProcessShellCommand(cmdInfo)) {
//		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister) {
//			// Also clear settings from registry when unregistering
//			m_pSettings->DeleteSettings();
//		}
//		return FALSE;
//	}

//	// Move root key back to default
//	if (osvi.dwMajorVersion >= 6) { 
//		::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
//	}

//	// The one and only window has been initialized, so show and update it
//	m_pMainWnd->ShowWindow(SW_SHOW);
//	m_pMainWnd->UpdateWindow();
//	// call DragAcceptFiles only if there's a suffix
//	//  In an SDI app, this should occur after ProcessShellCommand
//	// Enable drag/drop open
//	m_pMainWnd->DragAcceptFiles();

//	// This object is used to indicate if the sound synth thread locks up
//	m_hAliveCheck = CreateEvent(NULL, TRUE, FALSE, NULL);
//	// Used to awake the sound generator thread in case of lockup
//	m_hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	// Start sound generator thread, initially suspended
	if (!m_pSoundGenerator->CreateThread(CREATE_SUSPENDED)) {
		// If failed, restore and save default settings
		m_pSettings->DefaultSettings();
		m_pSettings->SaveSettings();
		// Show message and quit
//		AfxMessageBox(IDS_SOUNDGEN_ERROR, MB_ICONERROR);
		return FALSE;
	}

	// Initialize the sound interface, also resumes the thread
	if (!m_pSoundGenerator->InitializeSound(0,0,0)) {//m_pMainWnd->m_hWnd, m_hAliveCheck, m_hNotificationEvent)) {
		// If failed, restore and save default settings
		m_pSettings->DefaultSettings();
		m_pSettings->SaveSettings();
		// Quit program
//		AfxMessageBox(IDS_START_ERROR, MB_ICONERROR);
		return FALSE;
	}

//	// Initialize midi unit
//	m_pMIDI->Init();

//	// Check if the application is themed
//	CheckAppThemed();
	
//	if (cmdInfo.m_bPlay)
//		theApp.OnTrackerPlay();

	// Initialization is done
	TRACE0("App: InitInstance done\n");

//	// Save the main window handle
//	RegisterSingleInstance();

//#ifndef _DEBUG
//	// WIP
//	m_pMainWnd->GetMenu()->GetSubMenu(2)->RemoveMenu(ID_MODULE_CHANNELS, MF_BYCOMMAND);
//	m_pMainWnd->GetMenu()->GetSubMenu(2)->RemoveMenu(ID_MODULE_COMMENTS, MF_BYCOMMAND);
//#endif

	return TRUE;
}

int CFamiTrackerApp::ExitInstance()
{
	// Close program
	// The document is already closed at this point (and detached from sound player)

	TRACE("App: ExitInstance started\n");

//	UnregisterSingleInstance();

//	ShutDownSynth();

//	if (m_pMIDI) {
//		m_pMIDI->Shutdown();
//		delete m_pMIDI;
//		m_pMIDI = NULL;
//	}

//	if (m_pAccel) {
//		m_pAccel->SaveShortcuts(m_pSettings);
//		m_pAccel->Shutdown();
//		delete m_pAccel;
//		m_pAccel = NULL;
//	}

	if (m_pSettings) {
		m_pSettings->SaveSettings();
		delete m_pSettings;
		m_pSettings = NULL;
	}

//	if (m_customExporters) {
//		delete m_customExporters;
//		m_customExporters = NULL;
//	}

	if (m_pChannelMap) {
		delete m_pChannelMap;
		m_pChannelMap = NULL;
	}

	TRACE0("App: ExitInstance done\n");

	return CWinApp::ExitInstance();
}

CFamiTrackerApp theApp;

CWinApp* AfxGetApp() { return &theApp; }
CFrameWnd* AfxGetMainWnd() { return theApp.m_pMainWnd; }
