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
#include "InstrumentEditPanel.h"
#include "InstrumentEditorDPCM.h"
#include "SampleEditorDlg.h"
#include "PCMImport.h"
#include "Settings.h"
#include "SoundGen.h"

const TCHAR *CInstrumentEditorDPCM::KEY_NAMES[] = {_T("C"), _T("C#"), _T("D"), _T("D#"), _T("E"), _T("F"), _T("F#"), _T("G"), _T("G#"), _T("A"), _T("A#"), _T("B")};

// Derive a new class from CFileDialog with implemented preview of DMC files

class CDMCFileSoundDialog : public CFileDialog
{
public:
	CDMCFileSoundDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL, DWORD dwSize = 0);
	virtual ~CDMCFileSoundDialog();

	static const int DEFAULT_PREVIEW_PITCH = 15;

protected:
	virtual void OnFileNameChange();
	CString m_strLastFile;
};

//	CFileSoundDialog

CDMCFileSoundDialog::CDMCFileSoundDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize) 
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
}

CDMCFileSoundDialog::~CDMCFileSoundDialog()
{
	// Stop any possible playing sound
	//PlaySound(NULL, NULL, SND_NODEFAULT | SND_SYNC);
}

void CDMCFileSoundDialog::OnFileNameChange()
{
	// Preview DMC file
	if (!GetFileExt().CompareNoCase(_T("dmc")) && theApp.GetSettings()->General.bWavePreview) {
		DWORD dwAttrib = GetFileAttributes(GetPathName());
		if (!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) && GetPathName() != m_strLastFile) {
			CFile file(GetPathName(), CFile::modeRead);
			ULONGLONG size = file.GetLength();
			size = min(size, CDSample::MAX_SIZE);
			CDSample *pSample = new CDSample((int)size);
			file.Read(pSample->SampleData, (int)size);
			theApp.GetSoundGenerator()->PreviewSample(pSample, 0, DEFAULT_PREVIEW_PITCH);
			file.Close();
			m_strLastFile = GetPathName();
		}
	}
	
	CFileDialog::OnFileNameChange();
}

// CInstrumentDPCM dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorDPCM, CInstrumentEditPanel)
CInstrumentEditorDPCM::CInstrumentEditorDPCM(CWnd* pParent) : CInstrumentEditPanel(CInstrumentEditorDPCM::IDD, pParent)
{
//   IDD_INSTRUMENT_DPCM DIALOGEX 0, 0, 372, 174
   CRect rect(CPoint(0,0),CSize(372,174));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
//   GROUPBOX        "Loaded samples",IDC_STATIC,192,7,173,160
   CGroupBox* mfc9 = new CGroupBox(this);
   mfc9->setTitle("Loaded samples");
   CRect r9(CPoint(192,7),CSize(173,160));
   MapDialogRect(&r9);
   mfc9->setGeometry(r9);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   GROUPBOX        "Assigned samples",IDC_STATIC,7,7,179,160
   CGroupBox* mfc10 = new CGroupBox(this);
   mfc10->setTitle("Assigned samples");
   CRect r10(CPoint(7,7),CSize(179,160));
   MapDialogRect(&r10);
   mfc10->setGeometry(r10);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_OCTAVE,138,30,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc1 = new CComboBox(this);
   CRect r1(CPoint(138,30),CSize(42,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   mfcToQtWidget.insert(IDC_OCTAVE,mfc1);
   QObject::connect(mfc1,SIGNAL(currentIndexChanged(int)),this,SLOT(octave_currentIndexChanged(int)));
//   LTEXT           "Octave",IDC_STATIC,138,19,30,10
   CStatic* mfc2 = new CStatic(this);
   CRect r2(CPoint(138,19),CSize(30,10));
   MapDialogRect(&r2);
   mfc2->Create(_T("Octave"),WS_VISIBLE,r2,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   COMBOBOX        IDC_SAMPLES,15,148,117,125,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc3 = new CComboBox(this);
   CRect r3(CPoint(15,148),CSize(117,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_SAMPLES,mfc3);
   QObject::connect(mfc3,SIGNAL(currentIndexChanged(int)),this,SLOT(samples_currentIndexChanged(int)));
//   PUSHBUTTON      "Unload",IDC_UNLOAD,312,35,47,14
   CButton* mfc4 = new CButton(this);
   CRect r4(CPoint(312,35),CSize(47,14));
   MapDialogRect(&r4);
   mfc4->Create(_T("Unload"),WS_VISIBLE,r4,this,IDC_UNLOAD);
   mfcToQtWidget.insert(IDC_UNLOAD,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(unload_clicked()));
//   DEFPUSHBUTTON   "Load",IDC_LOAD,312,19,47,14
   CButton* mfc5 = new CButton(this);
   CRect r5(CPoint(312,19),CSize(47,14));
   MapDialogRect(&r5);
   mfc5->Create(_T("Unload"),WS_VISIBLE,r5,this,IDC_LOAD);
   mfc5->setDefault(true);
   mfcToQtWidget.insert(IDC_LOAD,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),this,SLOT(load_clicked()));
//   PUSHBUTTON      "Save",IDC_SAVE,312,51,47,14
   CButton* mfc6 = new CButton(this);
   CRect r6(CPoint(312,51),CSize(47,14));
   MapDialogRect(&r6);
   mfc6->Create(_T("Save"),WS_VISIBLE,r6,this,IDC_SAVE);
   mfcToQtWidget.insert(IDC_SAVE,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),this,SLOT(save_clicked()));
//   PUSHBUTTON      "Import",IDC_IMPORT,312,67,47,14
   CButton* mfc7 = new CButton(this);
   CRect r7(CPoint(312,67),CSize(47,14));
   MapDialogRect(&r7);
   mfc7->Create(_T("Import"),WS_VISIBLE,r7,this,IDC_IMPORT);
   mfcToQtWidget.insert(IDC_IMPORT,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),this,SLOT(import_clicked()));
//   LTEXT           "Space used 16 / 16 kb",IDC_SPACE,198,151,166,9
   CStatic* mfc8 = new CStatic(this);
   CRect r8(CPoint(198,151),CSize(166,9));
   MapDialogRect(&r8);
   mfc8->Create(_T("Space used 16 / 16 kb"),WS_VISIBLE,r8,this,IDC_SPACE);
   mfcToQtWidget.insert(IDC_SPACE,mfc8);
//   COMBOBOX        IDC_PITCH,138,58,42,53,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc11 = new CComboBox(this);
   CRect r11(CPoint(138,58),CSize(42,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r11);
   mfc11->setGeometry(r11);
   mfcToQtWidget.insert(IDC_PITCH,mfc11);
   QObject::connect(mfc11,SIGNAL(currentIndexChanged(int)),this,SLOT(pitch_currentIndexChanged(int)));
//   LTEXT           "Pitch",IDC_STATIC,138,48,30,8
   CStatic* mfc12 = new CStatic(this);
   CRect r12(CPoint(138,48),CSize(30,8));
   MapDialogRect(&r12);
   mfc12->Create(_T("Pitch"),WS_VISIBLE,r12,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_TABLE,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,15,19,117,125
   CListCtrl* mfc13 = new CListCtrl(this);
   mfc13->setSelectionMode(QAbstractItemView::SingleSelection);
   mfc13->setSelectionBehavior(QAbstractItemView::SelectRows);
//   mfc13->verticalScrollBar()->hide();
//   mfc13->horizontalScrollBar()->hide();
   CRect r13(CPoint(15,19),CSize(117,125));
   MapDialogRect(&r13);
   mfc13->setGeometry(r13);
   mfcToQtWidget.insert(IDC_TABLE,mfc13);
   QObject::connect(mfc13,SIGNAL(itemSelectionChanged()),this,SLOT(table_itemSelectionChanged()));
   QObject::connect(mfc13,SIGNAL(cellClicked(int,int)),this,SLOT(table_cellClicked(int,int)));
   QObject::connect(mfc13,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(table_cellDoubleClicked(int,int)));
//   CONTROL         "",IDC_SAMPLE_LIST,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,198,19,108,125
   CListCtrl* mfc14 = new CListCtrl(this);
   mfc14->setSelectionMode(QAbstractItemView::SingleSelection);
   mfc14->setSelectionBehavior(QAbstractItemView::SelectRows);
//   mfc14->verticalScrollBar()->hide();
//   mfc14->horizontalScrollBar()->hide();
   CRect r14(CPoint(198,19),CSize(108,125));
   MapDialogRect(&r14);
   mfc14->setGeometry(r14);
   mfcToQtWidget.insert(IDC_SAMPLE_LIST,mfc14);
   QObject::connect(mfc14,SIGNAL(itemSelectionChanged()),this,SLOT(sampleList_itemSelectionChanged()));
   QObject::connect(mfc14,SIGNAL(cellClicked(int,int)),this,SLOT(sampleList_cellClicked(int,int)));
   QObject::connect(mfc14,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(sampleList_cellDoubleClicked(int,int)));
//   CONTROL         "Loop",IDC_LOOP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,138,75,42,9
   CButton* mfc15 = new CButton(this);
   CRect r15(CPoint(138,75),CSize(42,9));
   MapDialogRect(&r15);
   mfc15->Create(_T("Loop"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r15,this,IDC_LOOP);
   mfcToQtWidget.insert(IDC_LOOP,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),this,SLOT(loop_clicked()));
//   PUSHBUTTON      "<-",IDC_ADD,138,130,42,14
   CButton* mfc16 = new CButton(this);
   CRect r16(CPoint(138,130),CSize(42,14));
   MapDialogRect(&r16);
   mfc16->Create(_T("<-"),WS_VISIBLE,r16,this,IDC_ADD);
   mfcToQtWidget.insert(IDC_ADD,mfc16);
   QObject::connect(mfc16,SIGNAL(clicked()),this,SLOT(add_clicked()));
//   PUSHBUTTON      "->",IDC_REMOVE,138,146,42,14
   CButton* mfc17 = new CButton(this);
   CRect r17(CPoint(138,146),CSize(42,14));
   MapDialogRect(&r17);
   mfc17->Create(_T("->"),WS_VISIBLE,r17,this,IDC_REMOVE);
   mfcToQtWidget.insert(IDC_REMOVE,mfc17);
   QObject::connect(mfc15,SIGNAL(clicked()),this,SLOT(remove_clicked()));
//   EDITTEXT        IDC_LOOP_POINT,138,106,42,13,ES_AUTOHSCROLL | NOT WS_VISIBLE
   CEdit* mfc18 = new CEdit(this);
   CRect r18(CPoint(138,106),CSize(42,13));
   MapDialogRect(&r18);
   mfc18->Create(ES_AUTOHSCROLL,r18,this,IDC_LOOP_POINT);
   mfcToQtWidget.insert(IDC_LOOP_POINT,mfc18);
   QObject::connect(mfc18,SIGNAL(textChanged(QString)),this,SLOT(loopPoint_textChanged(QString)));
//   PUSHBUTTON      "Edit",IDC_EDIT,312,83,47,14
   CButton* mfc19 = new CButton(this);
   CRect r19(CPoint(312,83),CSize(47,14));
   MapDialogRect(&r19);
   mfc19->Create(_T("Edit"),WS_VISIBLE,r19,this,IDC_EDIT);
   mfcToQtWidget.insert(IDC_EDIT,mfc19);
   QObject::connect(mfc19,SIGNAL(clicked()),this,SLOT(edit_clicked()));
//   PUSHBUTTON      "Preview",IDC_PREVIEW,312,99,47,14
   CButton* mfc20 = new CButton(this);
   CRect r20(CPoint(312,99),CSize(47,14));
   MapDialogRect(&r20);
   mfc20->Create(_T("Preview"),WS_VISIBLE,r20,this,IDC_PREVIEW);
   mfcToQtWidget.insert(IDC_PREVIEW,mfc20);
   QObject::connect(mfc20,SIGNAL(clicked()),this,SLOT(preview_clicked()));
//   LTEXT           "Loop offset",IDC_STATIC,138,94,42,10,NOT WS_VISIBLE
   CStatic* mfc21 = new CStatic(this);
   CRect r21(CPoint(138,94),CSize(42,10));
   MapDialogRect(&r21);
   mfc21->Create(_T("Loop offset"),0,r21,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
}

CInstrumentEditorDPCM::~CInstrumentEditorDPCM()
{
}

void CInstrumentEditorDPCM::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CInstrumentEditorDPCM, CInstrumentEditPanel)
//	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
//	ON_BN_CLICKED(IDC_UNLOAD, OnBnClickedUnload)
//	ON_NOTIFY(NM_CLICK, IDC_SAMPLE_LIST, OnNMClickSampleList)
//	ON_BN_CLICKED(IDC_IMPORT, OnBnClickedImport)
//	ON_CBN_SELCHANGE(IDC_OCTAVE, OnCbnSelchangeOctave)
//	ON_CBN_SELCHANGE(IDC_PITCH, OnCbnSelchangePitch)
//	ON_NOTIFY(NM_CLICK, IDC_TABLE, OnNMClickTable)
//	ON_CBN_SELCHANGE(IDC_SAMPLES, OnCbnSelchangeSamples)
//	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
//	ON_BN_CLICKED(IDC_LOOP, OnBnClickedLoop)
//	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
//	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
//	ON_EN_CHANGE(IDC_LOOP_POINT, &CInstrumentEditorDPCM::OnEnChangeLoopPoint)
//	ON_BN_CLICKED(IDC_EDIT, &CInstrumentEditorDPCM::OnBnClickedEdit)
//	ON_NOTIFY(NM_DBLCLK, IDC_SAMPLE_LIST, &CInstrumentEditorDPCM::OnNMDblclkSampleList)
//	ON_BN_CLICKED(IDC_PREVIEW, &CInstrumentEditorDPCM::OnBnClickedPreview)
//	ON_NOTIFY(NM_RCLICK, IDC_SAMPLE_LIST, &CInstrumentEditorDPCM::OnNMRClickSampleList)
//	ON_NOTIFY(NM_RCLICK, IDC_TABLE, &CInstrumentEditorDPCM::OnNMRClickTable)
//	ON_NOTIFY(NM_DBLCLK, IDC_TABLE, &CInstrumentEditorDPCM::OnNMDblclkTable)
//END_MESSAGE_MAP()

void CInstrumentEditorDPCM::load_clicked()
{
   OnBnClickedLoad();
}

void CInstrumentEditorDPCM::unload_clicked()
{
   OnBnClickedUnload();
}

void CInstrumentEditorDPCM::sampleList_itemSelectionChanged()
{
}

void CInstrumentEditorDPCM::sampleList_cellClicked(int row, int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnNMClickSampleList(&nmhdr,&result);
}

void CInstrumentEditorDPCM::sampleList_cellDoubleClicked(int row, int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnNMDblclkSampleList(&nmhdr,&result);
}

void CInstrumentEditorDPCM::import_clicked()
{
   OnBnClickedImport();
}

void CInstrumentEditorDPCM::octave_currentIndexChanged(int index)
{
   OnCbnSelchangeOctave();
}

void CInstrumentEditorDPCM::pitch_currentIndexChanged(int index)
{
   OnCbnSelchangePitch();
}

void CInstrumentEditorDPCM::table_itemSelectionChanged()
{
}

void CInstrumentEditorDPCM::table_cellClicked(int row, int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnNMClickTable(&nmhdr,&result);
}

void CInstrumentEditorDPCM::table_cellDoubleClicked(int row, int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnNMDblclkTable(&nmhdr,&result);
}

void CInstrumentEditorDPCM::samples_currentIndexChanged(int index)
{
   OnCbnSelchangeSamples();
}

void CInstrumentEditorDPCM::save_clicked()
{
   OnBnClickedSave();
}

void CInstrumentEditorDPCM::loop_clicked()
{
   OnBnClickedLoop();
}

void CInstrumentEditorDPCM::add_clicked()
{
   OnBnClickedAdd();
}

void CInstrumentEditorDPCM::remove_clicked()
{
   OnBnClickedRemove();
}

void CInstrumentEditorDPCM::loopPoint_textChanged(QString str)
{
   OnEnChangeLoopPoint();
}

void CInstrumentEditorDPCM::edit_clicked()
{
   OnBnClickedEdit();
}

void CInstrumentEditorDPCM::preview_clicked()
{
   OnBnClickedPreview();
}

// CInstrumentDPCM message handlers

BOOL CInstrumentEditorDPCM::OnInitDialog()
{
	CInstrumentEditPanel::OnInitDialog();

	CComboBox *pPitch, *pOctave;
	CString Text;

	m_iOctave = 3;
	m_iSelectedKey = -1;

	pPitch	= reinterpret_cast<CComboBox*>(GetDlgItem(IDC_PITCH));
	pOctave = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_OCTAVE));

	m_pTableListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));
	m_pTableListCtrl->DeleteAllItems();
	m_pTableListCtrl->InsertColumn(0, _T("Key"), LVCFMT_LEFT, 30);
	m_pTableListCtrl->InsertColumn(1, _T("Pitch"), LVCFMT_LEFT, 35);
	m_pTableListCtrl->InsertColumn(2, _T("Sample"), LVCFMT_LEFT, 90);
   qDebug("CListCtrl::SendMessage");
//	m_pTableListCtrl->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));
	m_pSampleListCtrl->DeleteAllItems();
	m_pSampleListCtrl->InsertColumn(0, _T("#"), LVCFMT_LEFT, 22);
	m_pSampleListCtrl->InsertColumn(1, _T("Name"), LVCFMT_LEFT, 88);
	m_pSampleListCtrl->InsertColumn(2, _T("Size"), LVCFMT_LEFT, 39);
   qDebug("CListCtrl::SendMessage");
//	m_pSampleListCtrl->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

//	m_pSampleListCtrl->OnFile

	for (int i = 0; i < 16; ++i) {
		Text.Format(_T("%i"), i);
		pPitch->AddString(Text);
	}

	pPitch->SetCurSel(15);

	for (int i = 0; i < OCTAVE_RANGE; ++i) {
		Text.Format(_T("%i"), i);
		pOctave->AddString(Text);
	}

	pOctave->SetCurSel(3);
	CheckDlgButton(IDC_LOOP, 0);
	m_pTableListCtrl->DeleteAllItems();

	for (int i = 0; i < 12; ++i)
		m_pTableListCtrl->InsertItem(i, KEY_NAMES[i], 0);

	BuildSampleList();
	m_iSelectedSample = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorDPCM::BuildKeyList()
{
	for (int i = 0; i < 12; ++i) {
		UpdateKey(i);
	}
}

void CInstrumentEditorDPCM::UpdateKey(int Index)
{
	char Name[256];

	m_pTableListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));

	if (m_pInstrument->GetSample(m_iOctave, Index) > 0) {
		int Item = m_pInstrument->GetSample(m_iOctave, Index) - 1;
		int Pitch = m_pInstrument->GetSamplePitch(m_iOctave, Index);

		if (GetDocument()->GetSampleSize(Item) == 0) {
			strcpy(Name, "(n/a)");
		}
		else {
			GetDocument()->GetSampleName(Item, Name);
		}
		m_pTableListCtrl->SetItemText(Index, 2, Name);
		sprintf(Name, "%i %s", Pitch & 0x0F, (Pitch & 0x80) ? "L" : "");
		m_pTableListCtrl->SetItemText(Index, 1, Name);
	}
	else {
		sprintf(Name, "(no sample)");
		m_pTableListCtrl->SetItemText(Index, 2, Name);
		sprintf(Name, "-");
		m_pTableListCtrl->SetItemText(Index, 1, Name);
	}
}

void CInstrumentEditorDPCM::BuildSampleList()
{
	CString	Text;
	CComboBox *pSampleBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_SAMPLES));

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));

	m_pSampleListCtrl->DeleteAllItems();
	pSampleBox->ResetContent();

	unsigned int Size(0), Index(0);

	pSampleBox->AddString(_T("(no sample)"));

	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		CDSample *pDSample = GetDocument()->GetDSample(i);
		if (pDSample->SampleSize > 0) {
			Text.Format(_T("%i"), i);
			m_pSampleListCtrl->InsertItem(Index, Text);
			Text.Format(_T("%s"), pDSample->Name);
			m_pSampleListCtrl->SetItemText(Index, 1, Text);
			Text.Format(_T("%i"), pDSample->SampleSize);
			m_pSampleListCtrl->SetItemText(Index, 2, Text);
			Text.Format(_T("%02i - %s"), i, pDSample->Name);
			pSampleBox->AddString(Text);
			Size += pDSample->SampleSize;
			++Index;
		}
	}

	Text.Format(IDS_DPCM_SPACE_FORMAT, Size / 0x400, (MAX_SAMPLE_SPACE - Size) / 0x400, MAX_SAMPLE_SPACE / 0x400);
	SetDlgItemText(IDC_SPACE, Text);
}

// When saved in NSF, the samples has to be aligned at even 6-bits addresses
//#define ADJUST_FOR_STORAGE(x) (((x >> 6) + (x & 0x3F ? 1 : 0)) << 6)
// TODO: I think I was wrong
#define ADJUST_FOR_STORAGE(x) (x)

bool CInstrumentEditorDPCM::LoadSample(CString FilePath, CString FileName)
{
	CFile SampleFile;

	if (!SampleFile.Open(FilePath, CFile::modeRead)) {
		AfxMessageBox(IDS_FILE_OPEN_ERROR);
		return false;
	}

	CDSample *pNewSample = new CDSample();

	if (pNewSample != NULL) {
      strcpy(pNewSample->Name, (char*)FileName.GetString());
		int Size = (int)SampleFile.GetLength();
		int AddSize = 0;
		// Clip file if too large
		Size = min(Size, CDSample::MAX_SIZE);
		// Make sure size is compatible with DPCM hardware
		if ((Size & 0xF) != 1) {
			AddSize = 0x10 - ((Size + 0x0F) & 0x0F);
		}
		pNewSample->SampleSize = Size + AddSize;
		pNewSample->SampleData = new char[Size + AddSize];
		SampleFile.Read(pNewSample->SampleData, Size);
		memset(pNewSample->SampleData + Size, 0xAA, AddSize);
		if (!InsertSample(pNewSample))
			return false;
	}
	
	SampleFile.Close();
	BuildSampleList();

	return true;
}

bool CInstrumentEditorDPCM::InsertSample(CDSample *pNewSample)
{	
	int FreeSlot = GetDocument()->GetFreeDSample();

	// Out of sample slots
	if (FreeSlot == -1) {
		delete pNewSample;
		return false;
	}

	CDSample *pFreeSample = GetDocument()->GetDSample(FreeSlot);

	int Size = GetDocument()->GetTotalSampleSize();
	
	if ((Size + pNewSample->SampleSize) > MAX_SAMPLE_SPACE) {
		CString message;
		message.Format(IDS_OUT_OF_SAMPLEMEM, MAX_SAMPLE_SPACE / 1024);
		AfxMessageBox(message, MB_ICONERROR);
	}
	else {
		strcpy(pFreeSample->Name, pNewSample->Name);
		pFreeSample->SampleSize = pNewSample->SampleSize;
		pFreeSample->SampleData = new char[pNewSample->SampleSize];
		memcpy(pFreeSample->SampleData, pNewSample->SampleData, pNewSample->SampleSize);
	}

	delete pNewSample;

	return true;
}

void CInstrumentEditorDPCM::OnBnClickedLoad()
{
	CDMCFileSoundDialog OpenFileDialog(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER, _T("Delta modulated samples (*.dmc)|*.dmc|All files|*.*||"));

//	OpenFileDialog.m_pOFN->lpstrInitialDir = theApp.GetSettings()->GetPath(PATH_DMC);

	if (OpenFileDialog.DoModal() == IDCANCEL)
		return;

	theApp.GetSettings()->SetPath(OpenFileDialog.GetPathName(), PATH_DMC);

	if (OpenFileDialog.GetFileName().GetLength() == 0) {
		// Multiple files
		POSITION Pos = OpenFileDialog.GetStartPosition();
		while (Pos) {
			CString Path = OpenFileDialog.GetNextPathName(Pos);
			CString FileName = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1);
			LoadSample(Path, FileName);
		}
	}
	else {
		// Single file
      LoadSample(OpenFileDialog.GetPathName(), OpenFileDialog.GetFileName());
	}
}

void CInstrumentEditorDPCM::OnBnClickedUnload()
{
	CListCtrl *pListBox = (CListCtrl*)GetDlgItem(IDC_SAMPLE_LIST);
	int nItem(-1), SelCount, Index;
	char ItemName[256];

	if (m_iSelectedSample == MAX_DSAMPLES)
		return;
	
	if (!(SelCount = pListBox->GetSelectedCount()))
		return;

	for (int i = 0; i < SelCount; i++) {
		nItem = pListBox->GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);
		pListBox->GetItemText(nItem, 0, ItemName, 256);
		Index = atoi(ItemName);
		GetDocument()->RemoveDSample(Index);
	}

	BuildSampleList();
}

void CInstrumentEditorDPCM::OnNMClickSampleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));

	if (m_pSampleListCtrl->GetItemCount() == 0)
		return;

	int Index = m_pSampleListCtrl->GetSelectionMark();
	TCHAR ItemName[256];
	m_pSampleListCtrl->GetItemText(Index, 0, ItemName, 256);
	//sscanf(ItemName, "%i", &m_iSelectedSample);
	m_iSelectedSample = _ttoi(ItemName);

	*pResult = 0;
}

void CInstrumentEditorDPCM::OnBnClickedImport()
{
	CPCMImport	ImportDialog;
	CDSample	*pImported;

	if (!(pImported = ImportDialog.ShowDialog()))
		return;

	InsertSample(pImported);
	BuildSampleList();
}

void CInstrumentEditorDPCM::OnCbnSelchangeOctave()
{
	CComboBox *m_OctaveBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_OCTAVE));
	m_iOctave = m_OctaveBox->GetCurSel();
	BuildKeyList();
}

void CInstrumentEditorDPCM::OnCbnSelchangePitch()
{
	CComboBox *m_pPitchBox	= reinterpret_cast<CComboBox*>(GetDlgItem(IDC_PITCH));

	if (m_iSelectedKey == -1)
		return;

	int Pitch = m_pPitchBox->GetCurSel();

	if (IsDlgButtonChecked(IDC_LOOP))
		Pitch |= 0x80;

	m_pInstrument->SetSamplePitch(m_iOctave, m_iSelectedKey, Pitch);

	UpdateKey(m_iSelectedKey);
}

void CInstrumentEditorDPCM::OnNMClickTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	CComboBox *pSampleBox, *pPitchBox;
	CString Text;

	m_pTableListCtrl	= reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));
	m_iSelectedKey		= m_pTableListCtrl->GetSelectionMark();

	int Sample			= m_pInstrument->GetSample(m_iOctave, m_iSelectedKey) - 1;
	int Pitch			= m_pInstrument->GetSamplePitch(m_iOctave, m_iSelectedKey);

	m_pTableListCtrl	= static_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));
	pSampleBox			= static_cast<CComboBox*>(GetDlgItem(IDC_SAMPLES));
	pPitchBox			= static_cast<CComboBox*>(GetDlgItem(IDC_PITCH));

	Text.Format(_T("%02i - %s"), Sample, m_pTableListCtrl->GetItemText(m_pTableListCtrl->GetSelectionMark(), 2));

	if (Sample != -1)
		pSampleBox->SelectString(0, Text);
	else
		pSampleBox->SetCurSel(0);
	
	if (Sample > 0)
		pPitchBox->SetCurSel(Pitch & 0x0F);

	if (Pitch & 0x80)
		CheckDlgButton(IDC_LOOP, 1);
	else
		CheckDlgButton(IDC_LOOP, 0);

	*pResult = 0;
}

void CInstrumentEditorDPCM::OnCbnSelchangeSamples()
{
	CComboBox *m_pSampleBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_SAMPLES));
	CComboBox *pPitchBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_PITCH));
	
	int PrevSample = m_pInstrument->GetSample(m_iOctave, m_iSelectedKey);
	int Sample = m_pSampleBox->GetCurSel();

	if (Sample > 0) {
		char Name[256];
		m_pSampleBox->GetLBText(Sample, Name);
		
		Name[2] = 0;
		if (Name[0] == _T('0')) {
			Name[0] = Name[1];
			Name[1] = 0;
		}

		Sample = atoi(Name);
		Sample++;

		if (PrevSample == 0) {
			int Pitch = pPitchBox->GetCurSel();
			m_pInstrument->SetSamplePitch(m_iOctave, m_iSelectedKey, Pitch);
		}
	}

	m_pInstrument->SetSample(m_iOctave, m_iSelectedKey, Sample);

	UpdateKey(m_iSelectedKey);
}

CDSample *CInstrumentEditorDPCM::GetSelectedSample()
{
	CDSample *pSample;
	TCHAR	 Text[256];

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));

	int Index = m_pSampleListCtrl->GetSelectionMark();

	if (Index == -1)
		return NULL;

	m_pSampleListCtrl->GetItemText(Index, 0, Text, 256);
	Index = _tstoi(Text);
	
	pSample = GetDocument()->GetDSample(Index);

	return pSample;
}

void CInstrumentEditorDPCM::OnBnClickedSave()
{
	CString		Path;
	CFile		SampleFile;
	CDSample	*DSample;
	TCHAR		Text[256];

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));

	int Index = m_pSampleListCtrl->GetSelectionMark();

	if (Index == -1)
		return;

	m_pSampleListCtrl->GetItemText(Index, 0, Text, 256);
	Index = _tstoi(Text);
	
	DSample = GetDocument()->GetDSample(Index);

	if (DSample->SampleSize == 0)
		return;

	CFileDialog SaveFileDialog(FALSE, _T("dmc"), (LPCTSTR)DSample->Name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Delta modulation samples (*.dmc)|*.dmc|All files|*.*||"));
	
	SaveFileDialog.m_pOFN->lpstrInitialDir = theApp.GetSettings()->GetPath(PATH_DMC);

	if (SaveFileDialog.DoModal() == IDCANCEL)
		return;

	theApp.GetSettings()->SetPath(SaveFileDialog.GetPathName(), PATH_DMC);

	Path = SaveFileDialog.GetPathName();

	if (!SampleFile.Open(Path, CFile::modeWrite | CFile::modeCreate)) {
		AfxMessageBox(IDS_FILE_OPEN_ERROR);
		return;
	}

	SampleFile.Write(DSample->SampleData, DSample->SampleSize);
	SampleFile.Close();
}

void CInstrumentEditorDPCM::OnBnClickedLoop()
{
	int Pitch = m_pInstrument->GetSamplePitch(m_iOctave, m_iSelectedKey) & 0x0F;

	if (IsDlgButtonChecked(IDC_LOOP))
		Pitch |= 0x80;

	m_pInstrument->SetSamplePitch(m_iOctave, m_iSelectedKey, Pitch);

	UpdateKey(m_iSelectedKey);
}

void CInstrumentEditorDPCM::SelectInstrument(int Instrument)
{
	CInstrument2A03 *pInst = (CInstrument2A03*)GetDocument()->GetInstrument(Instrument);
	m_pInstrument = pInst;
	BuildKeyList();
}

BOOL CInstrumentEditorDPCM::PreTranslateMessage(MSG* pMsg)
{
	if (IsWindowVisible()) {
		switch (pMsg->message) {
			case WM_KEYDOWN:
				if (pMsg->wParam == 27)	// Esc
					break;
				// Select DPCM channel
				CFamiTrackerView::GetView()->SelectChannel(4);
				CFamiTrackerView::GetView()->PreviewNote((unsigned char)pMsg->wParam);
				return TRUE;
			case WM_KEYUP:
				CFamiTrackerView::GetView()->PreviewRelease((unsigned char)pMsg->wParam);
				return TRUE;
		}
	}

	return CInstrumentEditPanel::PreTranslateMessage(pMsg);
}

void CInstrumentEditorDPCM::OnBnClickedAdd()
{
	// Add sample to key list	
	CComboBox *pPitchBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_PITCH));

	int Pitch = pPitchBox->GetCurSel();

	if (GetDocument()->GetSampleSize(m_iSelectedSample) > 0) {
		m_pInstrument->SetSample(m_iOctave, m_iSelectedKey, m_iSelectedSample + 1);
		m_pInstrument->SetSamplePitch(m_iOctave, m_iSelectedKey, Pitch);
		UpdateKey(m_iSelectedKey);
	}

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));
	m_pTableListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));

	if (m_iSelectedKey < 12 && m_iSelectedSample < MAX_DSAMPLES) {
		m_pSampleListCtrl->SetItemState(m_iSelectedSample, 0, LVIS_FOCUSED | LVIS_SELECTED);
		m_pTableListCtrl->SetItemState(m_iSelectedKey, 0, LVIS_FOCUSED | LVIS_SELECTED);
		if (m_iSelectedSample < m_pSampleListCtrl->GetItemCount() - 1)
			m_iSelectedSample++;
		m_iSelectedKey++;
		m_pSampleListCtrl->SetSelectionMark(m_iSelectedSample);
		m_pTableListCtrl->SetSelectionMark(m_iSelectedKey);
		m_pSampleListCtrl->SetItemState(m_iSelectedSample, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_pTableListCtrl->SetItemState(m_iSelectedKey, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

void CInstrumentEditorDPCM::OnBnClickedRemove()
{
	// Remove sample from key list
	m_pInstrument->SetSample(m_iOctave, m_iSelectedKey, 0);
	UpdateKey(m_iSelectedKey);

	if (m_iSelectedKey > 0) {
		m_pTableListCtrl->SetItemState(m_iSelectedKey, 0, LVIS_FOCUSED | LVIS_SELECTED);
		m_iSelectedKey--;
		m_pTableListCtrl->SetSelectionMark(m_iSelectedKey);
		m_pTableListCtrl->SetItemState(m_iSelectedKey, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}


void CInstrumentEditorDPCM::OnEnChangeLoopPoint()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CInstrumentEditPanel::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	/*
	int Pitch = GetDlgItemInt(IDC_LOOP_POINT, 0, 0);
	m_pInstrument->SetSampleLoopOffset(m_iOctave, m_iSelectedKey, Pitch);
	*/
}

void CInstrumentEditorDPCM::OnBnClickedEdit()
{
	CDSample *pSample = GetSelectedSample();

	if (pSample == NULL)
		return;

	CSampleEditorDlg Editor(this, pSample);

	INT_PTR nRes = Editor.DoModal();
	
	if (nRes == IDOK) {
		// Save edited sample
		Editor.CopySample(pSample);
	}

	// Update sample list
	BuildSampleList();
}

void CInstrumentEditorDPCM::OnNMDblclkSampleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Behaviour when double clicking the sample list
	OnBnClickedPreview();
	*pResult = 0;
}

void CInstrumentEditorDPCM::OnBnClickedPreview()
{
	CDSample *pSample = GetSelectedSample();

	if (pSample == NULL)
		return;

	theApp.GetSoundGenerator()->PreviewSample(pSample, 0, 15);
}

void CInstrumentEditorDPCM::OnNMRClickSampleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu *PopupMenu, PopupMenuBar;
	CPoint point;

	GetCursorPos(&point);
	PopupMenuBar.LoadMenu(IDR_SAMPLES_POPUP);
	PopupMenu = PopupMenuBar.GetSubMenu(0);
	PopupMenu->SetDefaultItem(IDC_PREVIEW);
	PopupMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);

	*pResult = 0;
}

void CInstrumentEditorDPCM::OnNMRClickTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// Create a popup menu for key list with samples

	CDSample *pDSample;
	CMenu PopupMenu;
	CPoint point;

	m_pTableListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));
	m_iSelectedKey	 = m_pTableListCtrl->GetSelectionMark();

	GetCursorPos(&point);
	PopupMenu.CreatePopupMenu();
	PopupMenu.AppendMenu(MF_STRING, 1, _T("(no sample)"));

	// Fill menu
	for (int i = 0; i < MAX_DSAMPLES; i++) {
		pDSample = GetDocument()->GetDSample(i);
		if (pDSample->SampleSize > 0) {
			PopupMenu.AppendMenu(MF_STRING, i + 2, pDSample->Name);
		}
	}

	UINT Result = PopupMenu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);

	if (Result == 1) {
		// Remove sample
		m_pInstrument->SetSample(m_iOctave, m_iSelectedKey, 0);
		UpdateKey(m_iSelectedKey);
	}
	else if (Result > 1) {
		// Add sample
		CComboBox *pPitchBox = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_PITCH));
		int Pitch = pPitchBox->GetCurSel();
		m_pInstrument->SetSample(m_iOctave, m_iSelectedKey, Result - 1);
		m_pInstrument->SetSamplePitch(m_iOctave, m_iSelectedKey, Pitch);
		UpdateKey(m_iSelectedKey);
	}

	*pResult = 0;
}

void CInstrumentEditorDPCM::OnNMDblclkTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// Preview sample from key table

	int Sample = m_pInstrument->GetSample(m_iOctave, m_iSelectedKey);

	if (Sample == 0)
		return;

	CDSample *pSample = GetDocument()->GetDSample(Sample - 1);
	int Pitch = m_pInstrument->GetSamplePitch(m_iOctave, m_iSelectedKey) & 0x0F;

	m_pTableListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_TABLE));

	if (pSample == NULL || pSample->SampleSize == 0 || m_pTableListCtrl->GetItemText(m_iSelectedKey, 2) == _T("(no sample)"))
		return;

	theApp.GetSoundGenerator()->PreviewSample(pSample, 0, Pitch);

	*pResult = 0;
}
