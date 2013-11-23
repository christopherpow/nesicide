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
#include "MainFrm.h"
#include "ModulePropertiesDlg.h"
#include "ChannelMap.h"
#include "ModuleImportDlg.h"

LPCTSTR TRACK_FORMAT = _T("#%02i %s");

// CModulePropertiesDlg dialog

//
// Contains song list editor and expansion chip selector
//

IMPLEMENT_DYNAMIC(CModulePropertiesDlg, CDialog)
CModulePropertiesDlg::CModulePropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModulePropertiesDlg::IDD, pParent)
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

void CModulePropertiesDlg::songList_itemSelectionChanged()
{
   CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_SONGLIST);
   NMLISTVIEW nmlv;
   LRESULT result;
   
   nmlv.uChanged = LVIF_STATE;
   nmlv.iItem = pList->currentIndex().row();
   nmlv.iSubItem = pList->currentIndex().column();
   nmlv.uNewState = LCTRL_CHECKBOX_STATE|LVNI_SELECTED;
   nmlv.uOldState = 0;
   
   OnLvnItemchangedSonglist((NMHDR*)&nmlv,&result);
}

void CModulePropertiesDlg::songAdd_clicked()
{
   OnBnClickedSongAdd();
}

void CModulePropertiesDlg::songRemove_clicked()
{
   OnBnClickedSongRemove();
}

void CModulePropertiesDlg::songUp_clicked()
{
   OnBnClickedSongUp();
}

void CModulePropertiesDlg::songDown_clicked()
{
   OnBnClickedSongDown();
}

void CModulePropertiesDlg::songImport_clicked()
{
   OnBnClickedSongImport();
}

void CModulePropertiesDlg::songName_textEdited(QString text)
{
   OnEnChangeSongname();
}

void CModulePropertiesDlg::expansion_currentIndexChanged(int index)
{
   OnCbnSelchangeExpansion();
}

void CModulePropertiesDlg::ok_clicked()
{
   OnBnClickedOk();
}

void CModulePropertiesDlg::cancel_clicked()
{
   OnCancel();
}

void CModulePropertiesDlg::channels_valueChanged(int value)
{
   OnHScroll(SB_HORZ,value,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_CHANNELS)));
}

// CModulePropertiesDlg message handlers

BOOL CModulePropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Get active document
	CFrameWnd *pFrameWnd = static_cast<CFrameWnd*>(GetParent());
	m_pDocument = static_cast<CFamiTrackerDoc*>(pFrameWnd->GetActiveDocument());

	m_pSongList = (CListCtrl*)GetDlgItem(IDC_SONGLIST);
	m_pSongList->InsertColumn(0, _T("Songs"), 0, 150);
	m_pSongList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	FillSongList();

	// Expansion chips
	CComboBox *pChipBox = (CComboBox*)GetDlgItem(IDC_EXPANSION);
	int ExpChip = m_pDocument->GetExpansionChip();
	CChannelMap *pChannelMap = theApp.GetChannelMap();

	for (int i = 0; i < pChannelMap->GetChipCount(); ++i)
		pChipBox->AddString(pChannelMap->GetChipName(i));

	pChipBox->SetCurSel(pChannelMap->GetChipIndex(ExpChip));

	// Vibrato 
	CComboBox *pVibratoBox = (CComboBox*)GetDlgItem(IDC_VIBRATO);
	pVibratoBox->SetCurSel((m_pDocument->GetVibratoStyle() == VIBRATO_NEW) ? 0 : 1);

	// Namco channel count
	CSliderCtrl *pChanSlider = (CSliderCtrl*)GetDlgItem(IDC_CHANNELS);
	pChanSlider->SetRange(1, 8);
	if (ExpChip == SNDCHIP_N163) {
		int Channels = m_pDocument->GetNamcoChannels();
		pChanSlider->SetPos(Channels);
		pChanSlider->EnableWindow(TRUE);
		CString text;
		text.Format(_T("Channels: %i"), Channels);
		SetDlgItemText(IDC_CHANNELS_NR, text);
	}
	else {
		pChanSlider->SetPos(0);
		pChanSlider->EnableWindow(FALSE);
		SetDlgItemText(IDC_CHANNELS_NR, _T("Channels: N/A"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CModulePropertiesDlg::OnBnClickedOk()
{
	CComboBox *pExpansionChipBox = (CComboBox*)GetDlgItem(IDC_EXPANSION);
	CMainFrame *pMainFrame = (CMainFrame*)GetParentFrame();

	// Expansion chip
	unsigned int iExpansionChip = theApp.GetChannelMap()->GetChipIdent(pExpansionChipBox->GetCurSel());
	unsigned int iChannels = ((CSliderCtrl*)GetDlgItem(IDC_CHANNELS))->GetPos();

	if (m_pDocument->GetExpansionChip() != iExpansionChip || m_pDocument->GetNamcoChannels() != iChannels) {
		m_pDocument->SetNamcoChannels(iChannels);
		m_pDocument->SelectExpansionChip(iExpansionChip);
	}

	// Vibrato 
	CComboBox *pVibratoBox = (CComboBox*)GetDlgItem(IDC_VIBRATO);
	m_pDocument->SetVibratoStyle((pVibratoBox->GetCurSel() == 0) ? VIBRATO_NEW : VIBRATO_OLD);

	if (pMainFrame->GetSelectedTrack() != m_iSelectedSong)
		m_pDocument->SelectTrack(m_iSelectedSong);

	OnOK();
}

void CModulePropertiesDlg::OnBnClickedSongAdd()
{
	CString TrackTitle;

	// Try to add a track
	if (!m_pDocument->AddTrack())
		return;
	
	// New track is always the last one
	int NewTrack = m_pDocument->GetTrackCount() - 1;
	
	TrackTitle.Format(TRACK_FORMAT, NewTrack, m_pDocument->GetTrackTitle(NewTrack));
	m_pSongList->InsertItem(NewTrack, TrackTitle);

	SelectSong(NewTrack);
}

void CModulePropertiesDlg::OnBnClickedSongRemove()
{
	ASSERT(m_iSelectedSong != -1);

	CString TrackTitle;

	unsigned Count = m_pDocument->GetTrackCount();

	if (Count == 1)
		return; // Single track

	// Display warning first
	if (AfxMessageBox(IDS_SONG_DELETE, MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
		return;

	m_pSongList->DeleteItem(m_iSelectedSong);
	m_pDocument->RemoveTrack(m_iSelectedSong);

	Count = m_pDocument->GetTrackCount();	// Get new track count

	// Redraw track list
	for (unsigned int i = 0; i < Count; ++i) {
		TrackTitle.Format(_T("#%02i %s"), i + 1, m_pDocument->GetTrackTitle(i));
		m_pSongList->SetItemText(i, 0, TrackTitle);
	}

	if (m_iSelectedSong == Count)
		SelectSong(m_iSelectedSong - 1);
	else
		SelectSong(m_iSelectedSong);
}

void CModulePropertiesDlg::OnBnClickedSongUp()
{
	CString Text;
	int Song = m_iSelectedSong;

	if (Song == 0)
		return;

	m_pDocument->MoveTrackUp(Song);

	Text.Format(TRACK_FORMAT, Song + 1, m_pDocument->GetTrackTitle(Song));
	m_pSongList->SetItemText(Song, 0, Text);
	Text.Format(TRACK_FORMAT, Song, m_pDocument->GetTrackTitle(Song - 1));
	m_pSongList->SetItemText(Song - 1, 0, Text);

	SelectSong(Song - 1);
}

void CModulePropertiesDlg::OnBnClickedSongDown()
{
	CString Text;
	int Song = m_iSelectedSong;

	if (Song == (m_pDocument->GetTrackCount() - 1))
		return;

	m_pDocument->MoveTrackDown(Song);

	Text.Format(TRACK_FORMAT, Song + 1, m_pDocument->GetTrackTitle(Song));
	m_pSongList->SetItemText(Song, 0, Text);
	Text.Format(TRACK_FORMAT, Song + 2, m_pDocument->GetTrackTitle(Song + 1));
	m_pSongList->SetItemText(Song + 1, 0, Text);

	SelectSong(Song + 1);
}

void CModulePropertiesDlg::OnEnChangeSongname()
{
	CString Text, Title;
	CEdit *pName = (CEdit*)GetDlgItem(IDC_SONGNAME);

	if (m_iSelectedSong == -1)
		return;

	pName->GetWindowText(Text);

	Title.Format(TRACK_FORMAT, m_iSelectedSong + 1, (LPCTSTR)Text);

	m_pSongList->SetItemText(m_iSelectedSong, 0, Title);
	m_pDocument->SetTrackTitle(m_iSelectedSong, Text.GetBuffer());
}

void CModulePropertiesDlg::SelectSong(int Song)
{
	ASSERT(Song >= 0);

	m_pSongList->SetItemState(Song, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_pSongList->EnsureVisible(Song, FALSE);
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
	CComboBox *pExpansionChipBox = (CComboBox*)GetDlgItem(IDC_EXPANSION);
	CMainFrame *pMainFrame = (CMainFrame*)GetParentFrame();
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(IDC_CHANNELS);

	// Expansion chip
	unsigned int iExpansionChip = theApp.GetChannelMap()->GetChipIdent(pExpansionChipBox->GetCurSel());

	if (iExpansionChip == SNDCHIP_N163) {
		pSlider->EnableWindow(TRUE);
		int Channels = m_pDocument->GetNamcoChannels();
		pSlider->SetPos(Channels);
		CString text;
		text.Format(_T("Channels: %i"), Channels);
		SetDlgItemText(IDC_CHANNELS_NR, text);
	}
	else {
		pSlider->EnableWindow(FALSE);
		SetDlgItemText(IDC_CHANNELS_NR, _T("Channels: N/A"));
	}
}

void CModulePropertiesDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(IDC_CHANNELS);

	CString text;
	text.Format(_T("Channels: %i"),  pSlider->GetPos());
	SetDlgItemText(IDC_CHANNELS_NR, text);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CModulePropertiesDlg::OnLvnItemchangedSonglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVNI_SELECTED)) {
		int Song = pNMLV->iItem;

		m_iSelectedSong = Song;

		CEdit *pName = (CEdit*)GetDlgItem(IDC_SONGNAME);
		pName->SetWindowText(CString(m_pDocument->GetTrackTitle(Song)));

		UpdateSongButtons();
    }

	*pResult = 0;
}

void CModulePropertiesDlg::FillSongList()
{
	CString Text;

	m_pSongList->DeleteAllItems();

	// Song editor
	int Songs = m_pDocument->GetTrackCount();

	for (int i = 0; i < Songs; ++i) {
		Text.Format(TRACK_FORMAT, i + 1, m_pDocument->GetTrackTitle(i));	// start counting songs from 1
		m_pSongList->InsertItem(i, Text);
	}

	// Select first song when dialog is displayed
	SelectSong(0);
}
