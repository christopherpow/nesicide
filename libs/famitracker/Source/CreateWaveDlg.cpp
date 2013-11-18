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
#include "CreateWaveDlg.h"
#include "WavProgressDlg.h"
#include "SoundGen.h"
#include "TrackerChannel.h"

const int MAX_LOOP_TIMES = 99;
const int MAX_PLAY_TIME	 = (99 * 60) + 0;

// CCreateWaveDlg dialog

IMPLEMENT_DYNAMIC(CCreateWaveDlg, CDialog)

CCreateWaveDlg::CCreateWaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateWaveDlg::IDD, pParent)
{
}

CCreateWaveDlg::~CCreateWaveDlg()
{
}

void CCreateWaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreateWaveDlg, CDialog)
//ON_BN_CLICKED(IDC_BEGIN, &CCreateWaveDlg::OnBnClickedBegin)
//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP, &CCreateWaveDlg::OnDeltaposSpinLoop)
//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, &CCreateWaveDlg::OnDeltaposSpinTime)
	ON_BN_CLICKED(IDC_BEGIN, OnBnClickedBegin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP, OnDeltaposSpinLoop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, OnDeltaposSpinTime)
END_MESSAGE_MAP()

void CCreateWaveDlg::begin_clicked()
{
   OnBnClickedBegin();
}

void CCreateWaveDlg::cancel_clicked()
{
   OnCancel();
}

void CCreateWaveDlg::spinLoop_valueChanged(int arg1,int arg2)
{
}

void CCreateWaveDlg::spinTime_valueChanged(int arg1,int arg2)
{
}

int CCreateWaveDlg::GetFrameLoopCount()
{
	int Frames = GetDlgItemInt(IDC_TIMES);

	if (Frames < 1)
		Frames = 1;
	if (Frames > MAX_LOOP_TIMES)
		Frames = MAX_LOOP_TIMES;

	return Frames;
}

int CCreateWaveDlg::GetTimeLimit()
{
	int Minutes, Seconds, Time;
	TCHAR str[256];

	GetDlgItemText(IDC_SECONDS, str, 256);
	_stscanf(str, _T("%u:%u"), &Minutes, &Seconds);
	Time = (Minutes * 60) + (Seconds % 60);

	if (Time < 1)
		Time = 1;
	if (Time > MAX_PLAY_TIME)
		Time = MAX_PLAY_TIME;

	return Time;
}

// CCreateWaveDlg message handlers

void CCreateWaveDlg::OnBnClickedBegin()
{
	RENDER_END EndType;
	int EndParam;

	CFamiTrackerDoc *pDoc = CFamiTrackerDoc::GetDoc();

	CString FileName = pDoc->GetFileTitle();

	CWavProgressDlg ProgressDlg;
	CFileDialog SaveDialog(FALSE, _T("wav"), FileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Microsoft PCM files (*.wav)|*.wav|All files (*.*)|*.*||"));

	// Close this dialog
	EndDialog(0);

	// Ask for file location
	if (SaveDialog.DoModal() == IDCANCEL)
		return;
	
	// Save
	if (IsDlgButtonChecked(IDC_RADIO_LOOP)) {
		EndType = SONG_LOOP_LIMIT;
		EndParam = GetFrameLoopCount();
	}
	else if (IsDlgButtonChecked(IDC_RADIO_TIME)) {
		EndType = SONG_TIME_LIMIT;
		EndParam = GetTimeLimit();
	}

	CFamiTrackerView *pView = CFamiTrackerView::GetView();

	pView->UnmuteAllChannels();

	// Mute selected channels
	for (int i = 0; i < m_ctlChannelList.GetCount(); ++i) {
		if (m_ctlChannelList.GetCheck(i) == 0)
			pView->ToggleChannel(i);
	}

//	m_sFileName = SaveDialog.GetPathName();
	ProgressDlg.SetFile(SaveDialog.GetPathName().GetString());
	ProgressDlg.SetOptions(EndType, EndParam);
	ProgressDlg.DoModal();

	// Unmute all channels
	pView->UnmuteAllChannels();
}

BOOL CCreateWaveDlg::OnInitDialog()
{
	CheckDlgButton(IDC_RADIO_LOOP, BST_CHECKED);
	CheckDlgButton(IDC_RADIO_TIME, BST_UNCHECKED);

	SetDlgItemText(IDC_TIMES, _T("1"));
	SetDlgItemText(IDC_SECONDS, _T("01:00"));

	m_ctlChannelList.SubclassDlgItem(IDC_CHANNELS, this);

	m_ctlChannelList.ResetContent();
	m_ctlChannelList.SetCheckStyle(BS_AUTOCHECKBOX);

	CFamiTrackerDoc *pDoc = CFamiTrackerDoc::GetDoc();

	for (int i = 0; i < pDoc->GetChannelCount(); ++i) {
		m_ctlChannelList.AddString(pDoc->GetChannel(i)->GetChannelName());
		m_ctlChannelList.SetCheck(i, 1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateWaveDlg::ShowDialog()
{
	CDialog::DoModal();
}

void CCreateWaveDlg::OnDeltaposSpinLoop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int Times = GetFrameLoopCount() - pNMUpDown->iDelta;

	if (Times < 1)
		Times = 1;
	if (Times > MAX_LOOP_TIMES)
		Times = MAX_LOOP_TIMES;

	SetDlgItemInt(IDC_TIMES, Times);
	CheckDlgButton(IDC_RADIO_LOOP, BST_CHECKED);
	CheckDlgButton(IDC_RADIO_TIME, BST_UNCHECKED);
	*pResult = 0;
}

void CCreateWaveDlg::OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int Minutes, Seconds;
	int Time = GetTimeLimit() - pNMUpDown->iDelta;
	CString str;

	if (Time < 1)
		Time = 1;
	if (Time > MAX_PLAY_TIME)
		Time = MAX_PLAY_TIME;

	Seconds = Time % 60;
	Minutes = Time / 60;

	str.Format(_T("%02i:%02i"), Minutes, Seconds);
	SetDlgItemText(IDC_SECONDS, str);
	CheckDlgButton(IDC_RADIO_LOOP, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_TIME, BST_CHECKED);
	*pResult = 0;
}
