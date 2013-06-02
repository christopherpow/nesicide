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

//	CFileSoundDialog

CDMCFileSoundDialog::CDMCFileSoundDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize) 
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
   QObject::connect(_qtd,SIGNAL(fileSelected(QString)),this,SLOT(fileSelected(QString)));
}

void CDMCFileSoundDialog::fileSelected(QString file)
{
   OnFileNameChange();
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

void CInstrumentEditorDPCM::contextMenuEvent(QContextMenuEvent *event)
{
   NMHDR nmhdr;
   LRESULT result;

   if ( GetDlgItem(IDC_TABLE)->toQWidget()->geometry().contains(event->pos()) )
   {
      OnNMRClickTable(&nmhdr,&result);
   }
   else if ( GetDlgItem(IDC_SAMPLE_LIST)->toQWidget()->geometry().contains(event->pos()) )
   {
      OnNMRClickSampleList(&nmhdr,&result);
   }
}

void CInstrumentEditorDPCM::keyPressEvent(QKeyEvent *event)
{
   MSG msg;
   msg.hwnd = (HWND)this;
   msg.message = WM_KEYDOWN;
   msg.wParam = event->key();
   PreTranslateMessage(&msg);
}

void CInstrumentEditorDPCM::keyReleaseEvent(QKeyEvent *event)
{
   MSG msg;
   msg.hwnd = (HWND)this;
   msg.message = WM_KEYUP;
   msg.wParam = event->key();
   PreTranslateMessage(&msg);
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
	m_pTableListCtrl->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_pSampleListCtrl = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_SAMPLE_LIST));
	m_pSampleListCtrl->DeleteAllItems();
	m_pSampleListCtrl->InsertColumn(0, _T("#"), LVCFMT_LEFT, 22);
	m_pSampleListCtrl->InsertColumn(1, _T("Name"), LVCFMT_LEFT, 88);
	m_pSampleListCtrl->InsertColumn(2, _T("Size"), LVCFMT_LEFT, 39);
	m_pSampleListCtrl->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

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

	Text.Format(_T("%02i - %s"), Sample, (LPCTSTR)m_pTableListCtrl->GetItemText(m_pTableListCtrl->GetSelectionMark(), 2));

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
			PopupMenu.AppendMenu(MF_STRING, i + 2, A2T(pDSample->Name));
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
