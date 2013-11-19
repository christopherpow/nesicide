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
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "SoundGen.h"
#include "WavProgressDlg.h"


// CWavProgressDlg dialog

IMPLEMENT_DYNAMIC(CWavProgressDlg, CDialog)

CWavProgressDlg::CWavProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWavProgressDlg::IDD, pParent)
{

}

CWavProgressDlg::~CWavProgressDlg()
{
}

void CWavProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWavProgressDlg, CDialog)
//ON_BN_CLICKED(IDC_CANCEL, &CWavProgressDlg::OnBnClickedCancel)
   ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CWavProgressDlg::cancel_clicked()
{
   OnBnClickedCancel();
}

void CWavProgressDlg::timerEvent(QTimerEvent *event)
{
   int mfcId = mfcTimerId(event->timerId());
   OnTimer(mfcId);
}

// CWavProgressDlg message handlers

void CWavProgressDlg::OnBnClickedCancel()
{
	if (m_pSoundGen->IsRendering()) {
		//m_pSoundGen->StopRendering();
		m_pSoundGen->PostThreadMessage(WM_USER_STOP_RENDER, 0, 0);
	}

	EndDialog(0);
}

void CWavProgressDlg::SetFile(CString File)
{
	m_sFile = File;
}

void CWavProgressDlg::SetOptions(int LengthType, int LengthParam)
{
	// Set song length
	m_iSongEndType = LengthType;
	m_iSongEndParam = LengthParam;
}

BOOL CWavProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString FileStr;

	if (m_sFile.GetLength() == 0)
		return TRUE;

	m_pSoundGen = theApp.GetSoundGenerator();
	m_pProgressBar = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_BAR);
	m_pView = CFamiTrackerView::GetView();
	m_pDoc = CFamiTrackerDoc::GetDoc();

	m_pView->Invalidate();
	m_pView->RedrawWindow();

	m_pProgressBar->SetRange(0, 100);

	// Start rendering
	FileStr.Format(_T("Saving to: %s"), m_sFile.GetString());
	SetDlgItemText(IDC_PROGRESS_FILE, FileStr);

	if (!m_pSoundGen->RenderToFile(m_sFile.GetBuffer(), m_iSongEndType, m_iSongEndParam))
		EndDialog(0);

	m_dwStartTime = GetTickCount();
	SetTimer(0, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWavProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Update progress status
	CString Text;
	int Frame, PercentDone;
	int RenderedTime;
	int FramesToRender;
	bool Done;
	DWORD Time = (GetTickCount() - m_dwStartTime) / 1000;
	
	//Frame = pView->GetSelectedFrame() + 1;
//	FrameCount = m_pDoc->GetFrameCount();

	m_pSoundGen->GetRenderStat(Frame, RenderedTime, Done, FramesToRender);

	if (m_iSongEndType == SONG_LOOP_LIMIT) {
		if (Frame > FramesToRender)
			Frame = FramesToRender;
		PercentDone = (Frame * 100) / FramesToRender;
		Text.Format(_T("Frame: %i / %i (%i%% done) "), Frame, FramesToRender, PercentDone);
	}
	else if (m_iSongEndType == SONG_TIME_LIMIT) {
		int TotalSec, TotalMin;
		int CurrSec, CurrMin;
		TotalSec = m_iSongEndParam % 60;
		TotalMin = m_iSongEndParam / 60;
		CurrSec = RenderedTime % 60;
		CurrMin = RenderedTime / 60;
		PercentDone = (RenderedTime * 100) / m_iSongEndParam;
		Text.Format(_T("Time: %02i:%02i / %02i:%02i (%i%% done) "), CurrMin, CurrSec, TotalMin, TotalSec, PercentDone);
	}

	SetDlgItemText(IDC_PROGRESS_LBL, Text);

	Text.Format(_T("Elapsed time: %02i:%02i"), (Time / 60), (Time % 60));
	SetDlgItemText(IDC_TIME, Text);

	m_pProgressBar->SetPos(PercentDone);

	if (!m_pSoundGen->IsRendering()) {
		SetDlgItemText(IDC_CANCEL, _T("Done"));
		CString title;
		GetWindowText(title);
		title.Append(_T(" finished"));
		SetWindowText(title);
	//	Text.Format("%02i:02i, frame: %i / %i (%i%% done) ", (Time / 60), (Time % 60), FrameCount, FrameCount, 100);
//		Text.Format("Frame: %i / %i (%i%% done) ", FrameCount, FrameCount, 100);
//		Text.Format("%02i:%02i, (%i%% done)", (Time / 60), (Time % 60), 100);
//		SetDlgItemText(IDC_PROGRESS_LBL, Text);
		m_pProgressBar->SetPos(100);
		KillTimer(0);
	}

	CDialog::OnTimer(nIDEvent);
}
