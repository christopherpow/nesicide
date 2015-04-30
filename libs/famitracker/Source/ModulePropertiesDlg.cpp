/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
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
#include "MainFrm.h"
#include "ModulePropertiesDlg.h"
#include "ChannelMap.h"
#include "ModuleImportDlg.h"
#include "SoundGen.h"

LPCTSTR TRACK_FORMAT = _T("#%02i %s");

// CModulePropertiesDlg dialog

//
// Contains song list editor and expansion chip selector
//

IMPLEMENT_DYNAMIC(CModulePropertiesDlg, CDialog)
CModulePropertiesDlg::CModulePropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModulePropertiesDlg::IDD, pParent), m_iSelectedSong(0), m_pDocument(NULL)
{
}

CModulePropertiesDlg::~CModulePropertiesDlg()
{
}

void CModulePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModulePropertiesDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
//ON_BN_CLICKED(IDC_SONG_ADD, &CModulePropertiesDlg::OnBnClickedSongAdd)
//ON_BN_CLICKED(IDC_SONG_REMOVE, &CModulePropertiesDlg::OnBnClickedSongRemove)
//ON_BN_CLICKED(IDC_SONG_UP, &CModulePropertiesDlg::OnBnClickedSongUp)
//ON_BN_CLICKED(IDC_SONG_DOWN, &CModulePropertiesDlg::OnBnClickedSongDown)
//ON_EN_CHANGE(IDC_SONGNAME, &CModulePropertiesDlg::OnEnChangeSongname)
//ON_BN_CLICKED(IDC_SONG_IMPORT, &CModulePropertiesDlg::OnBnClickedSongImport)
//ON_CBN_SELCHANGE(IDC_EXPANSION, &CModulePropertiesDlg::OnCbnSelchangeExpansion)
   ON_BN_CLICKED(IDC_SONG_ADD, OnBnClickedSongAdd)
   ON_BN_CLICKED(IDC_SONG_REMOVE, OnBnClickedSongRemove)
   ON_BN_CLICKED(IDC_SONG_UP, OnBnClickedSongUp)
   ON_BN_CLICKED(IDC_SONG_DOWN, OnBnClickedSongDown)
   ON_EN_CHANGE(IDC_SONGNAME, OnEnChangeSongname)
   ON_BN_CLICKED(IDC_SONG_IMPORT, OnBnClickedSongImport)
   ON_CBN_SELCHANGE(IDC_EXPANSION, OnCbnSelchangeExpansion)
	ON_WM_HSCROLL()
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_SONGLIST, &CModulePropertiesDlg::OnLvnItemchangedSonglist)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_SONGLIST, OnLvnItemchangedSonglist)
END_MESSAGE_MAP()


// CModulePropertiesDlg message handlers

BOOL CModulePropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Get active document
	CFrameWnd *pFrameWnd = static_cast<CFrameWnd*>(GetParent());
	m_pDocument = static_cast<CFamiTrackerDoc*>(pFrameWnd->GetActiveDocument());

	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	pSongList->InsertColumn(0, _T("Songs"), 0, 150);
	pSongList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	FillSongList();

	// Expansion chips
	CComboBox *pChipBox = static_cast<CComboBox*>(GetDlgItem(IDC_EXPANSION));
	int ExpChip = m_pDocument->GetExpansionChip();
	CChannelMap *pChannelMap = theApp.GetChannelMap();

	for (int i = 0; i < pChannelMap->GetChipCount(); ++i)
		pChipBox->AddString(pChannelMap->GetChipName(i));

	pChipBox->SetCurSel(pChannelMap->GetChipIndex(ExpChip));

	// Vibrato 
	CComboBox *pVibratoBox = static_cast<CComboBox*>(GetDlgItem(IDC_VIBRATO));
	pVibratoBox->SetCurSel((m_pDocument->GetVibratoStyle() == VIBRATO_NEW) ? 0 : 1);

	// Namco channel count
	CSliderCtrl *pChanSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_CHANNELS));
	pChanSlider->SetRange(1, 8);
	CString channelsStr;
	channelsStr.LoadString(IDS_PROPERTIES_CHANNELS);
	if (ExpChip == SNDCHIP_N163) {
		int Channels = m_pDocument->GetNamcoChannels();
		pChanSlider->SetPos(Channels);
		pChanSlider->EnableWindow(TRUE);
		channelsStr.AppendFormat(_T(" %i"), Channels);
	}
	else {
		pChanSlider->SetPos(0);
		pChanSlider->EnableWindow(FALSE);
		channelsStr.Append(_T(" N/A"));
	}
	SetDlgItemText(IDC_CHANNELS_NR, channelsStr);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CModulePropertiesDlg::OnBnClickedOk()
{
	CComboBox *pExpansionChipBox = static_cast<CComboBox*>(GetDlgItem(IDC_EXPANSION));
	CMainFrame *pMainFrame = static_cast<CMainFrame*>(GetParentFrame());

	// Expansion chip
	unsigned int iExpansionChip = theApp.GetChannelMap()->GetChipIdent(pExpansionChipBox->GetCurSel());
	unsigned int iChannels = static_cast<CSliderCtrl*>(GetDlgItem(IDC_CHANNELS))->GetPos();

	if (m_pDocument->GetExpansionChip() != iExpansionChip || m_pDocument->GetNamcoChannels() != iChannels) {
		m_pDocument->SetNamcoChannels(iChannels);
		m_pDocument->SelectExpansionChip(iExpansionChip);
		m_pDocument->UpdateAllViews(NULL, UPDATE_PROPERTIES);
	}

	// Vibrato 
	CComboBox *pVibratoBox = static_cast<CComboBox*>(GetDlgItem(IDC_VIBRATO));
	m_pDocument->SetVibratoStyle((pVibratoBox->GetCurSel() == 0) ? VIBRATO_NEW : VIBRATO_OLD);

	if (pMainFrame->GetSelectedTrack() != m_iSelectedSong)
		pMainFrame->SelectTrack(m_iSelectedSong);

	pMainFrame->UpdateControls();

	theApp.GetSoundGenerator()->DocumentPropertiesChanged(m_pDocument);

	OnOK();
}

void CModulePropertiesDlg::OnBnClickedSongAdd()
{
	CString TrackTitle;

	// Try to add a track
	int NewTrack = m_pDocument->AddTrack();

	if (NewTrack == -1)
		return;
	
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);

	TrackTitle.Format(TRACK_FORMAT, NewTrack, m_pDocument->GetTrackTitle(NewTrack).GetString());
	static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST))->InsertItem(NewTrack, TrackTitle);

	SelectSong(NewTrack);
}

void CModulePropertiesDlg::OnBnClickedSongRemove()
{
	ASSERT(m_iSelectedSong != -1);

	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	CMainFrame *pMainFrame = static_cast<CMainFrame*>(GetParentFrame());
	unsigned Count = m_pDocument->GetTrackCount();
	CString TrackTitle;

	if (Count == 1)
		return; // Single track

	// Display warning first
	if (AfxMessageBox(IDS_SONG_DELETE, MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
		return;

	pSongList->DeleteItem(m_iSelectedSong);
	m_pDocument->RemoveTrack(m_iSelectedSong);
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);

	Count = m_pDocument->GetTrackCount();	// Get new track count

	// Redraw track list
	for (unsigned int i = 0; i < Count; ++i) {
		TrackTitle.Format(_T("#%02i %s"), i + 1, m_pDocument->GetTrackTitle(i).GetString());
		pSongList->SetItemText(i, 0, TrackTitle);
	}

	if (m_iSelectedSong == Count)
		SelectSong(m_iSelectedSong - 1);
	else
		SelectSong(m_iSelectedSong);
}

void CModulePropertiesDlg::OnBnClickedSongUp()
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	int Song = m_iSelectedSong;
	CString Text;

	if (Song == 0)
		return;

	m_pDocument->MoveTrackUp(Song);
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);

	Text.Format(TRACK_FORMAT, Song + 1, m_pDocument->GetTrackTitle(Song).GetString());
	pSongList->SetItemText(Song, 0, Text);
	Text.Format(TRACK_FORMAT, Song, m_pDocument->GetTrackTitle(Song - 1).GetString());
	pSongList->SetItemText(Song - 1, 0, Text);

	SelectSong(Song - 1);
}

void CModulePropertiesDlg::OnBnClickedSongDown()
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	int Song = m_iSelectedSong;
	CString Text;

	if (Song == (m_pDocument->GetTrackCount() - 1))
		return;

	m_pDocument->MoveTrackDown(Song);
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);

	Text.Format(TRACK_FORMAT, Song + 1, m_pDocument->GetTrackTitle(Song).GetString());
	pSongList->SetItemText(Song, 0, Text);
	Text.Format(TRACK_FORMAT, Song + 2, m_pDocument->GetTrackTitle(Song + 1).GetString());
	pSongList->SetItemText(Song + 1, 0, Text);

	SelectSong(Song + 1);
}

void CModulePropertiesDlg::OnEnChangeSongname()
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	CEdit *pName = static_cast<CEdit*>(GetDlgItem(IDC_SONGNAME));
	CString Text, Title;

	if (m_iSelectedSong == -1)
		return;

	pName->GetWindowText(Text);

   Title.Format(TRACK_FORMAT, m_iSelectedSong + 1, (LPCTSTR)Text);

	pSongList->SetItemText(m_iSelectedSong, 0, Title);
	m_pDocument->SetTrackTitle(m_iSelectedSong, Text);
	m_pDocument->UpdateAllViews(NULL, UPDATE_TRACK);
}

void CModulePropertiesDlg::SelectSong(int Song)
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	ASSERT(Song >= 0);

	pSongList->SetItemState(Song, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	pSongList->EnsureVisible(Song, FALSE);
}

void CModulePropertiesDlg::UpdateSongButtons()
{
	unsigned TrackCount = m_pDocument->GetTrackCount();

	GetDlgItem(IDC_SONG_REMOVE)->EnableWindow((TrackCount == 1) ? FALSE : TRUE);
	GetDlgItem(IDC_SONG_ADD)->EnableWindow((TrackCount == MAX_TRACKS) ? FALSE : TRUE);
	GetDlgItem(IDC_SONG_DOWN)->EnableWindow((m_iSelectedSong == (TrackCount - 1)) ? FALSE : TRUE);
	GetDlgItem(IDC_SONG_UP)->EnableWindow((m_iSelectedSong == 0) ? FALSE : TRUE);
	GetDlgItem(IDC_SONG_IMPORT)->EnableWindow((TrackCount == MAX_TRACKS) ? FALSE : TRUE);
}

void CModulePropertiesDlg::OnBnClickedSongImport()
{
	CModuleImportDlg importDlg(m_pDocument);

	// TODO use string table
	CFileDialog OpenFileDlg(TRUE, _T("ftm"), 0, OFN_HIDEREADONLY, _T("FamiTracker files (*.ftm)|*.ftm|All files (*.*)|*.*||"), theApp.GetMainWnd(), 0);

	if (OpenFileDlg.DoModal() == IDCANCEL)
		return;

	if (importDlg.LoadFile(OpenFileDlg.GetPathName(), m_pDocument) == false)
		return;

	importDlg.DoModal();

	FillSongList();
}

void CModulePropertiesDlg::OnCbnSelchangeExpansion()
{
	CComboBox *pExpansionChipBox = static_cast<CComboBox*>(GetDlgItem(IDC_EXPANSION));
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_CHANNELS));

	// Expansion chip
	unsigned int iExpansionChip = theApp.GetChannelMap()->GetChipIdent(pExpansionChipBox->GetCurSel());

	CString channelsStr;
	channelsStr.LoadString(IDS_PROPERTIES_CHANNELS);
	if (iExpansionChip == SNDCHIP_N163) {
		pSlider->EnableWindow(TRUE);
		int Channels = m_pDocument->GetNamcoChannels();
		pSlider->SetPos(Channels);
		channelsStr.AppendFormat(_T(" %i"), Channels);
	}
	else {
		pSlider->EnableWindow(FALSE);
		channelsStr.Append(_T(" N/A"));
	}
	SetDlgItemText(IDC_CHANNELS_NR, channelsStr);
}

void CModulePropertiesDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_CHANNELS));

	CString text;
	text.LoadString(IDS_PROPERTIES_CHANNELS);
	text.AppendFormat(_T(" %i"),  pSlider->GetPos());
	SetDlgItemText(IDC_CHANNELS_NR, text);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CModulePropertiesDlg::OnLvnItemchangedSonglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVNI_SELECTED)) {
		int Song = pNMLV->iItem;

		m_iSelectedSong = Song;

		CEdit *pName = static_cast<CEdit*>(GetDlgItem(IDC_SONGNAME));
		pName->SetWindowText(CString(m_pDocument->GetTrackTitle(Song).GetString()));

		UpdateSongButtons();
    }

	*pResult = 0;
}

void CModulePropertiesDlg::FillSongList()
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));
	CString Text;

	pSongList->DeleteAllItems();

	// Song editor
	int Songs = m_pDocument->GetTrackCount();

	for (int i = 0; i < Songs; ++i) {
		Text.Format(TRACK_FORMAT, i + 1, m_pDocument->GetTrackTitle(i).GetString());	// start counting songs from 1
		pSongList->InsertItem(i, Text);
	}

	// Select first song when dialog is displayed
	SelectSong(0);
}

BOOL CModulePropertiesDlg::PreTranslateMessage(MSG* pMsg)
{
	CListCtrl *pSongList = static_cast<CListCtrl*>(GetDlgItem(IDC_SONGLIST));

	if (GetFocus() == pSongList) {
		if(pMsg->message == WM_KEYDOWN) {
			switch (pMsg->wParam) {
				case VK_DELETE:
					// Delete song
					if (m_iSelectedSong != -1) {
						OnBnClickedSongRemove();
					}
					break;
				case VK_INSERT:
					// Insert song
					OnBnClickedSongAdd();
					break;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
