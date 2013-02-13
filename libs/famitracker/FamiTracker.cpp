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


//#include "Exception.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrame.h"
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
   m_pSettings = new CSettings();
   m_pSettings->LoadSettings();
   m_pSoundGenerator = new CSoundGen();
   m_pChannelMap = new CChannelMap();
}

CFamiTrackerApp::~CFamiTrackerApp()
{
   delete m_pSoundGenerator;
   delete m_pChannelMap;
   delete m_pSettings;
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

// Silences everything
void CFamiTrackerApp::SilentEverything()
{
   GetSoundGenerator()->SilentAll();
   CFamiTrackerView::GetView()->MakeSilent();
}

void CFamiTrackerApp::RegisterKeyState(int Channel, int Note)
{
   qDebug("::RegisterKeyState");
}

void CFamiTrackerApp::OnTrackerStop()
{
}

CFamiTrackerApp theApp;

