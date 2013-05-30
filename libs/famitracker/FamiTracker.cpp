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
#include "Accelerator.h"
#include "Settings.h"
#include "ChannelMap.h"
#include "CustomExporters.h"

// Single instance-stuff
const TCHAR FT_SHARED_MUTEX_NAME[]	= _T("FamiTrackerMutex");	// Name of global mutex
const TCHAR FT_SHARED_MEM_NAME[]	= _T("FamiTrackerWnd");		// Name of global memory area
const DWORD	SHARED_MEM_SIZE			= 256;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef SVN_BUILD
#pragma message("Building SVN build...")
#endif

// CFamiTrackerApp

//BEGIN_MESSAGE_MAP(CFamiTrackerApp, CWinApp)
//	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
//	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
//	ON_COMMAND(ID_TRACKER_PLAY, OnTrackerPlay)
//	ON_COMMAND(ID_TRACKER_PLAY_START, OnTrackerPlayStart)
//	ON_COMMAND(ID_TRACKER_PLAY_CURSOR, OnTrackerPlayCursor)
////	ON_COMMAND(ID_TRACKER_PLAY, OnTrackerPlay)
//	ON_COMMAND(ID_TRACKER_STOP, OnTrackerStop)
//	ON_COMMAND(ID_TRACKER_TOGGLE_PLAY, OnTrackerTogglePlay)
//	ON_COMMAND(ID_TRACKER_PLAYPATTERN, OnTrackerPlaypattern)
//#ifdef UPDATE_CHECK
//	ON_COMMAND(ID_HELP_CHECKFORNEWVERSIONS, CheckNewVersion)
//#endif
//END_MESSAGE_MAP()

// Include this for windows xp style in visual studio 2005 or later
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

// CFamiTrackerApp construction

CFamiTrackerApp::CFamiTrackerApp() :
//   m_bThemeActive(false),
//	m_pMIDI(NULL),
//	m_pAccel(NULL),
	m_pSettings(NULL),
	m_pSoundGenerator(NULL),
	m_pChannelMap(NULL),
	m_customExporters(NULL),
	m_hAliveCheck(NULL),
	m_hNotificationEvent(NULL),
//	m_hWndMapFile(NULL),
	m_pInstanceMutex(NULL)
{
}

// The one and only CFamiTrackerApp object
CFamiTrackerApp theApp;
CWinApp* ptrToTheApp = &theApp;

// CFamiTrackerApp initialization

BOOL CFamiTrackerApp::InitInstance(QMainWindow* parent)
{
   // Hook Qt to this MFC app...
   qtMainWindow = parent;
   
//   // InitCommonControls() is required on Windows XP if an application
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

	if (CheckSingleInstance())
		return FALSE;

	//who: added by Derek Andrews <derek.george.andrews@gmail.com>
	//why: Load all custom exporter plugins on startup.
	
	TCHAR pathToPlugins[MAX_PATH];
	GetModuleFileName(NULL, pathToPlugins, MAX_PATH);
	PathRemoveFileSpec(pathToPlugins);
	PathAppend(pathToPlugins, _T("\\Plugins"));
	m_customExporters = new CCustomExporters( pathToPlugins );

	// Load custom accelerator
	m_pAccel = new CAccelerator();
	m_pAccel->LoadShortcuts(m_pSettings);
	m_pAccel->Setup();

   qDebug("NO MIDI SUPPORT YET...");
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
	
	// Parse command line for standard shell commands, DDE, file open + some custom ones
	CFTCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo)) {
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister) {
			// Also clear settings from registry when unregistering
			m_pSettings->DeleteSettings();
		}
		return FALSE;
	}

//	// Move root key back to default
//	if (osvi.dwMajorVersion >= 6) { 
//		::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
//	}

	// The one and only window has been initialized, so show and update it
//	m_pMainWnd->ShowWindow(SW_SHOW);
//	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// This object is used to indicate if the sound synth thread locks up
	m_hAliveCheck = CreateEvent(NULL, TRUE, FALSE, NULL);
	// Used to awake the sound generator thread in case of lockup
	m_hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	// Start sound generator thread, initially suspended
	if (!m_pSoundGenerator->CreateThread(CREATE_SUSPENDED)) {
		// If failed, restore and save default settings
		m_pSettings->DefaultSettings();
		m_pSettings->SaveSettings();
		// Show message and quit
		AfxMessageBox(IDS_SOUNDGEN_ERROR, MB_ICONERROR);
		return FALSE;
	}

	// Initialize the sound interface, also resumes the thread
	if (!m_pSoundGenerator->InitializeSound(m_pMainWnd->m_hWnd, m_hAliveCheck, m_hNotificationEvent)) {
		// If failed, restore and save default settings
		m_pSettings->DefaultSettings();
		m_pSettings->SaveSettings();
		// Quit program
		AfxMessageBox(IDS_START_ERROR, MB_ICONERROR);
		return FALSE;
	}

   qDebug("NO MIDI SUPPORT YET...");
//	// Initialize midi unit
//	m_pMIDI->Init();

//	// Check if the application is themed
//	CheckAppThemed();
	
	if (cmdInfo.m_bPlay)
		theApp.OnTrackerPlay();

	// Initialization is done
	TRACE0("App: InitInstance done\n");

	// Save the main window handle
	RegisterSingleInstance();

#ifndef _DEBUG
	// WIP
   qDebug("GetMenu to remove WIP menus");
//	m_pMainWnd->GetMenu()->GetSubMenu(2)->RemoveMenu(ID_MODULE_CHANNELS, MF_BYCOMMAND);
//	m_pMainWnd->GetMenu()->GetSubMenu(2)->RemoveMenu(ID_MODULE_COMMENTS, MF_BYCOMMAND);
#endif

	return TRUE;
}

int CFamiTrackerApp::ExitInstance()
{
	// Close program
	// The document is already closed at this point (and detached from sound player)

	TRACE("App: ExitInstance started\n");

	UnregisterSingleInstance();

	ShutDownSynth();

   qDebug("NO MIDI SUPPORT YET...");
//	if (m_pMIDI) {
//		m_pMIDI->Shutdown();
//		delete m_pMIDI;
//		m_pMIDI = NULL;
//	}

	if (m_pAccel) {
		m_pAccel->SaveShortcuts(m_pSettings);
		m_pAccel->Shutdown();
		delete m_pAccel;
		m_pAccel = NULL;
	}

	if (m_pSettings) {
		m_pSettings->SaveSettings();
		delete m_pSettings;
		m_pSettings = NULL;
	}

	if (m_customExporters) {
		delete m_customExporters;
		m_customExporters = NULL;
	}

	if (m_pChannelMap) {
		delete m_pChannelMap;
		m_pChannelMap = NULL;
	}

	TRACE0("App: ExitInstance done\n");

	return CWinApp::ExitInstance();
}

BOOL CFamiTrackerApp::PreTranslateMessage(MSG* pMsg)
{
	if (CWinApp::PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	else if (m_pMainWnd != NULL && m_pAccel != NULL) {
		if (m_pAccel->Translate(m_pMainWnd->m_hWnd, pMsg)) {
			return TRUE;
		}
	}

	return FALSE;
	/*
	
	if (m_pAccel->Translate(m_pMainWnd->m_hWnd, pMsg)) {
		return TRUE;
	}

	return CWinApp::PreTranslateMessage(pMsg);
	*/
}

//void CFamiTrackerApp::CheckAppThemed()
//{
//	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	
//	if (hinstDll) {
//		typedef BOOL (*ISAPPTHEMEDPROC)();
//		ISAPPTHEMEDPROC pIsAppThemed;
//		pIsAppThemed = (ISAPPTHEMEDPROC) ::GetProcAddress(hinstDll, "IsAppThemed");

//		if(pIsAppThemed)
//			m_bThemeActive = (pIsAppThemed() == TRUE);

//		::FreeLibrary(hinstDll);
//	}
//}

bool CFamiTrackerApp::IsThemeActive() const
{ 
	return m_bThemeActive; 
}

void CFamiTrackerApp::ShutDownSynth()
{
   qDebug("ShutDownSynth");
//	// Shut down sound generator
//	if (!m_pSoundGenerator)
//		return;

//	// Save a handle to the thread since the object will delete itself
//	HANDLE hThread = m_pSoundGenerator->m_hThread;

//	if (!hThread) {
//		// Object was found but thread not created
//		delete m_pSoundGenerator;
//		m_pSoundGenerator = NULL;
//		return;
//	}

//	// Resume if thread was suspended
//	m_pSoundGenerator->ResumeThread();

//	// Send quit message
//	m_pSoundGenerator->PostThreadMessage(WM_QUIT, 0, 0);
//	SetEvent(m_hNotificationEvent);

//	// Wait for thread to exit, timout = 1s
//	DWORD dwResult = ::WaitForSingleObject(hThread, 1000);

//	if (dwResult != WAIT_OBJECT_0 && m_pSoundGenerator != NULL) {
//		TRACE0("App: Closing the sound generator thread failed\n");
//#ifdef _DEBUG
//		AfxMessageBox(_T("Error: Could not close sound generator thread"));
//#endif
//		// Unclean exit
//		return;
//	}

//	// Object is auto-deleted
//	ASSERT(m_pSoundGenerator == NULL);

//	// Close handles
//	CloseHandle(m_hNotificationEvent);
//	CloseHandle(m_hAliveCheck);

//	m_hNotificationEvent = NULL;
//	m_hAliveCheck = NULL;

//	TRACE0("App: Sound generator has closed\n");
}

void CFamiTrackerApp::RemoveSoundGenerator()
{
	// Sound generator object is deleted
	m_pSoundGenerator = NULL;
}

CCustomExporters* CFamiTrackerApp::GetCustomExporters(void) const
{
	return m_customExporters;
}

void CFamiTrackerApp::RegisterSingleInstance()
{
   qDebug("RegisterSingleInstance");
//	// Create a memory area with this app's window handle
//	if (!GetSettings()->General.bSingleInstance)
//		return;

//	m_hWndMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEM_SIZE, FT_SHARED_MEM_NAME);

//	if (m_hWndMapFile != NULL) {
//		LPTSTR pBuf = (LPTSTR) MapViewOfFile(m_hWndMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_SIZE);
//		if (pBuf != NULL) { 
//			// Create a string of main window handle
//			_itot((int)GetMainWnd()->m_hWnd, pBuf, 10);
//			UnmapViewOfFile(pBuf);
//		}
//	}
}

void CFamiTrackerApp::UnregisterSingleInstance()
{	
   qDebug("UnregisterSingleInstance");
//	// Close shared memory area
//	if (m_hWndMapFile) {
//		CloseHandle(m_hWndMapFile);
//		m_hWndMapFile = NULL;
//	}

//	if (m_pInstanceMutex) {
//		delete m_pInstanceMutex;
//		m_pInstanceMutex = NULL;
//	}
}

bool CFamiTrackerApp::CheckSingleInstance()
{	
	// Returns true if program should close
	
	if (!GetSettings()->General.bSingleInstance)
		return false;

   qDebug("CheckSingleInstance");
//	m_pInstanceMutex = new CMutex(FALSE, FT_SHARED_MUTEX_NAME);

//	if (GetLastError() == ERROR_ALREADY_EXISTS) {
//		// Another instance detected, get window handle
//		HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, FT_SHARED_MEM_NAME);
//		if (hMapFile != NULL) {	
//			LPCTSTR pBuf = (LPTSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_SIZE);
//			if (pBuf != NULL) {
//				// Get window handle
//				HWND hWnd = (HWND)_ttoi(pBuf);
//				if (hWnd != NULL) {
//					// Get file name
//					CFTCommandLineInfo cmdInfo;
//					ParseCommandLine(cmdInfo);
//					LPTSTR pFilePath = cmdInfo.m_strFileName.GetBuffer();
//					// We have the window handle & file, send a message to open the file
//					COPYDATASTRUCT data;
//					data.dwData = cmdInfo.m_bPlay ? IPC_LOAD_PLAY : IPC_LOAD;
//					data.cbData = (DWORD)((_tcslen(pFilePath) + 1) * sizeof(TCHAR));
//					data.lpData = pFilePath;
//					DWORD result;
//					SendMessageTimeout(hWnd, WM_COPYDATA, NULL, (LPARAM)&data, SMTO_NORMAL, 100, &result);
//					UnmapViewOfFile(pBuf);
//					CloseHandle(hMapFile);
//					TRACE("Found another instance, shutting down\n");
//					// Then close the program
//					return true;
//				}

//				UnmapViewOfFile(pBuf);
//			}
//			CloseHandle(hMapFile);
//		}
//	}
	
	return false;
}

////////////////////////////////////////////////////////
//  Things that belongs to the synth are kept below!  //
////////////////////////////////////////////////////////

// Load sound configuration
void CFamiTrackerApp::LoadSoundConfig()
{
	GetSoundGenerator()->LoadSettings();
	SetEvent(m_hNotificationEvent);
	((CFrameWnd*)GetMainWnd())->SetMessageText(IDS_NEW_SOUND_CONFIG);
}

// Silences everything
void CFamiTrackerApp::SilentEverything()
{
	GetSoundGenerator()->SilentAll();
	CFamiTrackerView::GetView()->MakeSilent();
}

void CFamiTrackerApp::CheckSynth() 
{
	// Monitor performance
	static DWORD LastTime;

	if (!m_hAliveCheck)
		return;

	if (LastTime == 0)
		LastTime = GetTickCount();

	// Wait for signals from the player thread
   qDebug("WaitForSingleObject??");
//	if (WaitForSingleObject(m_hAliveCheck, 0) == WAIT_OBJECT_0) {	// return immediately
//		if ((GetTickCount() - LastTime) > 1000) {
//			((CMainFrame*) GetMainWnd())->SetMessageText(AFX_IDS_IDLEMESSAGE);
//		}
//		LastTime = GetTickCount();
//	}
//	else {
//		// Timeout after 1 s
//		if ((GetTickCount() - LastTime) > 1000) {
//			// Display message
//			((CMainFrame*) GetMainWnd())->SetMessageText(IDS_SOUND_FAIL);
//		}
//	}

	ResetEvent(m_hAliveCheck);
}

int CFamiTrackerApp::GetCPUUsage() const
{
   qDebug("GetCPUUsage");
//	// Calculate CPU usage
//	const int THREAD_COUNT = 2;
//	static FILETIME KernelLastTime[THREAD_COUNT], UserLastTime[THREAD_COUNT];
//	const HANDLE hThreads[THREAD_COUNT] = {m_hThread, m_pSoundGenerator->m_hThread};
	unsigned int TotalTime = 0;

//	for (int i = 0; i < THREAD_COUNT; ++i) {
//		FILETIME CreationTime, ExitTime, KernelTime, UserTime;
//		GetThreadTimes(hThreads[i], &CreationTime, &ExitTime, &KernelTime, &UserTime);
//		TotalTime += (KernelTime.dwLowDateTime - KernelLastTime[i].dwLowDateTime) / 1000;
//		TotalTime += (UserTime.dwLowDateTime - UserLastTime[i].dwLowDateTime) / 1000;
//		KernelLastTime[i] = KernelTime;
//		UserLastTime[i]	= UserTime;
//	}

	return TotalTime;
}

void CFamiTrackerApp::ReloadColorScheme(void)
{
	// Main window
	CMainFrame *pMainFrm = dynamic_cast<CMainFrame*>(GetMainWnd());

	if (pMainFrm != NULL)
		pMainFrm->SetupColors();

	// Notify all views	
	POSITION TemplatePos = GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemplate = GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pDocTemplate->GetFirstDocPosition();

	while (CDocument* pDoc = pDocTemplate->GetNextDoc(DocPos)) {
		POSITION ViewPos = pDoc->GetFirstViewPosition();
		while (CView *pView = pDoc->GetNextView(ViewPos)) {
			static_cast<CFamiTrackerView*>(pView)->SetupColors();
		}
	}
}

void CFamiTrackerApp::RegisterKeyState(int Channel, int Note)
{
	CFamiTrackerView *pView = CFamiTrackerView::GetView();

	if (pView)
		pView->RegisterKeyState(Channel, Note);
}

// App command to run the about dialog
void CFamiTrackerApp::OnAppAbout()
{
   qDebug("OnAppAbout");
//	CAboutDlg aboutDlg;
//	aboutDlg.DoModal();
}

// CFamiTrackerApp message handlers

void CFamiTrackerApp::OnTrackerPlay()
{
	// Play
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

// Active document & view

// TODO remove this
/*
CDocument *CFamiTrackerApp::GetActiveDocument() const
{
	CFrameWnd *pFrameWnd = dynamic_cast<CFrameWnd*>(m_pMainWnd);

	if (!pFrameWnd)
		return NULL;

	return pFrameWnd->GetActiveDocument();
}
*/
// TODO remove this
/*
CView *CFamiTrackerApp::GetActiveView() const
{
	CFrameWnd *pFrameWnd = dynamic_cast<CFrameWnd*>(m_pMainWnd);

	if (!pFrameWnd)
		return NULL;
	
	return pFrameWnd->GetActiveView();
}
*/
// File load/save

void CFamiTrackerApp::OnFileOpen() 
{
	// Overloaded in order to save the file path
	CString newName, path;

	// Get path
	path = m_pSettings->GetPath(PATH_FTM) + _T("\\");
	newName = _T("");

	if (!DoPromptFileName(newName, path, AFX_IDS_OPENFILE, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
		return; // open cancelled

	// Save path
	m_pSettings->SetPath(newName, PATH_FTM);
	((CFrameWnd*)GetMainWnd())->SetMessageText(_T("Loading file..."));
	AfxGetApp()->OpenDocumentFile(newName);
	((CFrameWnd*)GetMainWnd())->SetMessageText(_T("Done"));
}

BOOL CFamiTrackerApp::DoPromptFileName(CString& fileName, CString& filePath, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
	// Copied from MFC
	CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);

	CString title;
	ENSURE(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;

	if (pTemplate == NULL) {
		POSITION pos = GetFirstDocTemplatePosition();
		while (pos != NULL) {
			CString strFilterExt;
			CString strFilterName;
			pTemplate = GetNextDocTemplate(pos);
			pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt);
			pTemplate->GetDocString(strFilterName, CDocTemplate::filterName);

			// Add extension
			strFilter += strFilterName;
			strFilter += (TCHAR)'\0';
			strFilter += _T("*");
			strFilter += strFilterExt;
			strFilter += (TCHAR)'\0';
			dlgFile.m_ofn.nMaxCustFilter++;
		}
	}

	// Select first filter
	dlgFile.m_ofn.nFilterIndex = 1;

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrInitialDir = filePath.GetBuffer(_MAX_PATH);
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	filePath.ReleaseBuffer();

	return nResult == IDOK;
}

// Various global helper functions

CString LoadDefaultFilter(LPCTSTR Name, LPCTSTR Ext)
{
	// Loads a single filter string including the all files option
	CString filter;
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));

	filter = Name;
	filter += _T(" (*");
	filter += Ext;
	filter += _T(")");
	filter += allFilter;

	return filter;
}

/**
 * CFTCommandLineInfo, a custom command line parser
 *
 */

CFTCommandLineInfo::CFTCommandLineInfo() : CCommandLineInfo(), 
	m_bLog(false), 
	m_bExport(false), 
	m_bPlay(false),
	m_strExportFile(_T(""))
{
}

void CFTCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag) {
		// Export file (/export or /e)
		if (!_tcsicmp(pszParam, _T("export")) || !_tcsicmp(pszParam, _T("e"))) {
			m_bExport = true;
			return;
		}
		// Auto play (/play or /p)
		else if (!_tcsicmp(pszParam, _T("play")) || !_tcsicmp(pszParam, _T("p"))) {
			m_bPlay = true;
			return;
		}
		// Enable register logger (/log), available in debug mode only
		else if (!_tcsicmp(pszParam, _T("log"))) {
#ifdef _DEBUG
			m_bLog = true;
			return;
#endif
		}
	}
	else {
		// Store NSF name
		if (m_strFileName.GetLength() != 0 && m_bExport == true) {
			m_strExportFile = CString(pszParam);
			return;
		}
	}

	// Call default implementation
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

