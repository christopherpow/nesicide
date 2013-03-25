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
//   IDD_PROPERTIES DIALOGEX 0, 0, 213, 259
   CRect rect(CPoint(0,0),CSize(213,259));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());

//       GROUPBOX        "Song editor",IDC_STATIC,7,7,199,147
   CGroupBox* mfc1 = new CGroupBox(this);
   mfc1->setTitle("Song editor");
   CRect r1(CPoint(7,7),CSize(199,147));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Expansion sound",IDC_STATIC,7,162,127,30
   CGroupBox* mfc10 = new CGroupBox(this);
   mfc10->setTitle("Expansion sound");
   CRect r10(CPoint(7,162),CSize(127,30));
   MapDialogRect(&r10);
   mfc10->setGeometry(r10);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Vibrato",IDC_STATIC,7,194,199,31
   CGroupBox* mfc12 = new CGroupBox(this);
   mfc12->setTitle("Vibrato");
   CRect r12(CPoint(7,194),CSize(199,31));
   MapDialogRect(&r12);
   mfc12->setGeometry(r12);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Channels: 0",IDC_CHANNELS_NR,141,162,65,30
   CGroupBox* mfc17 = new CGroupBox(this);
   mfc17->setTitle("Channels: 0");
   CRect r17(CPoint(141,162),CSize(65,30));
   MapDialogRect(&r17);
   mfc17->setGeometry(r17);
   mfcToQtWidget.insert(IDC_CHANNELS_NR,mfc17);
//       CONTROL         "",IDC_SONGLIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,14,18,120,114
   CListCtrl* mfc2 = new CListCtrl(this);
   mfc2->setSelectionMode(QAbstractItemView::SingleSelection);
   mfc2->setSelectionBehavior(QAbstractItemView::SelectRows);
//   mfc2->verticalScrollBar()->hide();
//   mfc2->horizontalScrollBar()->hide();
   CRect r2(CPoint(14,18),CSize(120,114));
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDC_SONGLIST,mfc2);
   QObject::connect(mfc2,SIGNAL(itemSelectionChanged()),this,SLOT(songList_itemSelectionChanged()));
//       PUSHBUTTON      "Add",IDC_SONG_ADD,138,18,60,14
   CButton* mfc3 = new CButton(this);
   CRect r3(CPoint(138,18),CSize(60,14));
   MapDialogRect(&r3);
   mfc3->Create(_T("Add"),WS_VISIBLE,r3,this,IDC_SONG_ADD);
   mfcToQtWidget.insert(IDC_SONG_ADD,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),this,SLOT(songAdd_clicked()));
//       PUSHBUTTON      "Remove",IDC_SONG_REMOVE,138,35,60,14
   CButton* mfc4 = new CButton(this);
   CRect r4(CPoint(138,35),CSize(60,14));
   MapDialogRect(&r4);
   mfc4->Create(_T("Remove"),WS_VISIBLE,r4,this,IDC_SONG_REMOVE);
   mfcToQtWidget.insert(IDC_SONG_REMOVE,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(songRemove_clicked()));
//       PUSHBUTTON      "Move up",IDC_SONG_UP,138,52,60,14
   CButton* mfc5 = new CButton(this);
   CRect r5(CPoint(138,52),CSize(60,14));
   MapDialogRect(&r5);
   mfc5->Create(_T("Move up"),WS_VISIBLE,r5,this,IDC_SONG_UP);
   mfcToQtWidget.insert(IDC_SONG_UP,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),this,SLOT(songUp_clicked()));
//       PUSHBUTTON      "Move down",IDC_SONG_DOWN,138,69,60,14
   CButton* mfc6 = new CButton(this);
   CRect r6(CPoint(138,69),CSize(60,14));
   MapDialogRect(&r6);
   mfc6->Create(_T("Move down"),WS_VISIBLE,r6,this,IDC_SONG_DOWN);
   mfcToQtWidget.insert(IDC_SONG_DOWN,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),this,SLOT(songDown_clicked()));
//       PUSHBUTTON      "Import file",IDC_SONG_IMPORT,138,86,60,14
   CButton* mfc7 = new CButton(this);
   CRect r7(CPoint(138,86),CSize(60,14));
   MapDialogRect(&r7);
   mfc7->Create(_T("Import file"),WS_VISIBLE,r7,this,IDC_SONG_IMPORT);
   mfcToQtWidget.insert(IDC_SONG_IMPORT,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),this,SLOT(songImport_clicked()));
//       LTEXT           " Title",IDC_STATIC,14,135,17,12,SS_CENTERIMAGE
   CStatic* mfc8 = new CStatic(this);
   mfc8->setText(" Title");
   CRect r8(CPoint(14,135),CSize(17,12));
   MapDialogRect(&r8);
   mfc8->Create(_T(" Title"),SS_CENTERIMAGE | WS_VISIBLE,r8,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SONGNAME,34,135,100,12,ES_AUTOHSCROLL
   CEdit* mfc9 = new CEdit(this);
   CRect r9(CPoint(34,135),CSize(100,12));
   MapDialogRect(&r9);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,r9,this,IDC_SONGNAME); 
   mfcToQtWidget.insert(IDC_SONGNAME,mfc9);
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),this,SLOT(songName_textChanged(QString)));
//       COMBOBOX        IDC_EXPANSION,14,173,113,61,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(this);
   CRect r11(CPoint(14,173),CSize(113,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r11);
   mfc11->setGeometry(r11);
   mfcToQtWidget.insert(IDC_EXPANSION,mfc11);
   QObject::connect(mfc11,SIGNAL(currentIndexChanged(int)),this,SLOT(expansion_currentIndexChanged(int)));
//       COMBOBOX        IDC_VIBRATO,14,204,184,61,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc13 = new CComboBox(this);
   CRect r13(CPoint(14,204),CSize(184,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r13);
   mfc13->setGeometry(r13);
   mfcToQtWidget.insert(IDC_VIBRATO,mfc13);
//   IDD_PROPERTIES DLGINIT
//   BEGIN
//       IDC_VIBRATO, 0x403, 27, 0
//   0x654e, 0x2077, 0x7473, 0x6c79, 0x2065, 0x6228, 0x6e65, 0x2064, 0x7075, 
//   0x2620, 0x6420, 0x776f, 0x296e, "\000" 
//       IDC_VIBRATO, 0x403, 20, 0
//   0x6c4f, 0x2064, 0x7473, 0x6c79, 0x2065, 0x6228, 0x6e65, 0x2064, 0x7075, 
//   0x0029, 
//       0
//   END
   mfc13->AddString(_T("Old style (bend up)"));
   mfc13->AddString(_T("New style (bend up & down)"));
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,231,199,1
   qDebug("horzline not implemented");
//       DEFPUSHBUTTON   "OK",IDOK,95,238,53,14
   CButton* mfc15 = new CButton(this);
   CRect r15(CPoint(95,238),CSize(53,14));
   MapDialogRect(&r15);
   mfc15->Create(_T("OK"),WS_VISIBLE,r15,this,IDOK);
   mfc15->setDefault(true);
   mfcToQtWidget.insert(IDOK,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),this,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,153,238,53,14
   CButton* mfc16 = new CButton(this);
   CRect r16(CPoint(153,238),CSize(53,14));
   MapDialogRect(&r16);
   mfc16->Create(_T("Cancel"),WS_VISIBLE,r16,this,IDCANCEL);
   mfcToQtWidget.insert(IDCANCEL,mfc16);
   QObject::connect(mfc16,SIGNAL(clicked()),this,SLOT(cancel_clicked()));
//       CONTROL         "",IDC_CHANNELS,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,147,173,51,13
   CSliderCtrl* mfc18 = new CSliderCtrl(this);
   CRect r18(CPoint(147,173),CSize(51,13));
   MapDialogRect(&r18);
   mfc18->setGeometry(r18);
   mfcToQtWidget.insert(IDC_CHANNELS,mfc18);
   QObject::connect(mfc18,SIGNAL(valueChanged(int)),this,SLOT(channels_valueChanged(int)));
}

CModulePropertiesDlg::~CModulePropertiesDlg()
{
}

void CModulePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CModulePropertiesDlg, CDialog)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
//	ON_BN_CLICKED(IDC_SONG_ADD, &CModulePropertiesDlg::OnBnClickedSongAdd)
//	ON_BN_CLICKED(IDC_SONG_REMOVE, &CModulePropertiesDlg::OnBnClickedSongRemove)
//	ON_BN_CLICKED(IDC_SONG_UP, &CModulePropertiesDlg::OnBnClickedSongUp)
//	ON_BN_CLICKED(IDC_SONG_DOWN, &CModulePropertiesDlg::OnBnClickedSongDown)
//	ON_EN_CHANGE(IDC_SONGNAME, &CModulePropertiesDlg::OnEnChangeSongname)
//	ON_BN_CLICKED(IDC_SONG_IMPORT, &CModulePropertiesDlg::OnBnClickedSongImport)
//	ON_CBN_SELCHANGE(IDC_EXPANSION, &CModulePropertiesDlg::OnCbnSelchangeExpansion)
//	ON_WM_HSCROLL()
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SONGLIST, &CModulePropertiesDlg::OnLvnItemchangedSonglist)
//END_MESSAGE_MAP()

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

void CModulePropertiesDlg::songName_textChanged(QString text)
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
   qDebug("CListCtrl::SendMessage not implemented");
//	m_pSongList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

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

	Title.Format(TRACK_FORMAT, m_iSelectedSong + 1, Text);

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
