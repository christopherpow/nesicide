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

#include <iterator> 
#include <string>
#include <sstream>
#include <cmath>

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "InstrumentEditPanel.h"
#include "InstrumentEditorN163Wave.h"
#include "MainFrm.h"

using namespace std;

// CInstrumentEditorN163Wave dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorN163Wave, CInstrumentEditPanel)

CInstrumentEditorN163Wave::CInstrumentEditorN163Wave(CWnd* pParent) : CInstrumentEditPanel(CInstrumentEditorN163Wave::IDD, pParent),
	m_pWaveEditor(NULL), 
	m_pInstrument(NULL)
{
//   IDD_INSTRUMENT_N163_WAVE DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
// CP: Put all GROUPBOX items first so their child elements can be created.
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,116
   CGroupBox* mfc1 = new CGroupBox(this);
   mfc1->setTitle("Wave editor");
   CRect r1(CPoint(7,7),CSize(228,116));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave #",IDC_STATIC,244,7,121,35
   CGroupBox* mfc21 = new CGroupBox(this);
   mfc21->setTitle("Wave #");
   CRect r21(CPoint(244,7),CSize(121,35));
   MapDialogRect(&r21);
   mfc21->setGeometry(r21);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "MML string",IDC_STATIC,7,132,358,33
   CGroupBox* mfc7 = new CGroupBox(this);
   mfc7->setTitle("MML string");
   CRect r7(CPoint(7,132),CSize(358,33));
   MapDialogRect(&r7);
   mfc7->setGeometry(r7);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Wave RAM settings",IDC_STATIC,244,45,121,52
   CGroupBox* mfc16 = new CGroupBox(this);
   mfc16->setTitle("Wave RAM settings");
   CRect r16(CPoint(244,45),CSize(121,52));
   MapDialogRect(&r16);
   mfc16->setGeometry(r16);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(this);
   mfc2->setText("Sine");
   CRect r2(CPoint(14,104),CSize(41,14));
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDC_PRESET_SINE,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(presetSine_clicked()));
//       PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,101,41,12
   CButton* mfc3 = new CButton(this);
   mfc3->setText("Triangle");
   CRect r3(CPoint(57,104),CSize(41,14));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_PRESET_TRIANGLE,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),this,SLOT(presetTriangle_clicked()));
//       PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,101,41,12
   CButton* mfc4 = new CButton(this);
   mfc4->setText("Sawtooth");
   CRect r4(CPoint(100,104),CSize(41,14));
   MapDialogRect(&r4);
   mfc4->setGeometry(r4);
   mfcToQtWidget.insert(IDC_PRESET_SAWTOOTH,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(presetSawtooth_clicked()));
//       PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,101,41,12
   CButton* mfc5 = new CButton(this);
   mfc5->setText("50% pulse");
   CRect r5(CPoint(143,104),CSize(41,14));
   MapDialogRect(&r5);
   mfc5->setGeometry(r5);
   mfcToQtWidget.insert(IDC_PRESET_PULSE_50,mfc5);
   QObject::connect(mfc5,SIGNAL(clicked()),this,SLOT(presetPulse50_clicked()));
//       PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,101,41,12
   CButton* mfc6 = new CButton(this);
   mfc6->setText("25% pulse");
   CRect r6(CPoint(186,104),CSize(41,14));
   MapDialogRect(&r6);
   mfc6->setGeometry(r6);
   mfcToQtWidget.insert(IDC_PRESET_PULSE_25,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),this,SLOT(presetPulse25_clicked()));
//       EDITTEXT        IDC_MML,16,145,342,14,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(this);
   CRect r8(CPoint(16,145),CSize(342,14));
   MapDialogRect(&r8);
   mfc8->setGeometry(r8);   
   mfcToQtWidget.insert(IDC_MML,mfc8);
//       PUSHBUTTON      "Copy wave",IDC_COPY_WAVE,240,90,52,14
   CButton* mfc9 = new CButton(this);
   mfc9->setText("Copy wave");
   CRect r9(CPoint(249,105),CSize(50,14));
   MapDialogRect(&r9);
   mfc9->setGeometry(r9);
   mfcToQtWidget.insert(IDC_COPY,mfc9);
   QObject::connect(mfc9,SIGNAL(clicked()),this,SLOT(copy_clicked()));
//       PUSHBUTTON      "Paste wave",IDC_PASTE_WAVE,240,104,52,14
   CButton* mfc10 = new CButton(this);
   mfc10->setText("Paste wave");
   CRect r10(CPoint(308,105),CSize(50,14));
   MapDialogRect(&r10);
   mfc10->setGeometry(r10);
   mfcToQtWidget.insert(IDC_PASTE,mfc10);
   QObject::connect(mfc10,SIGNAL(clicked()),this,SLOT(paste_clicked()));
//       EDITTEXT        IDC_INDEX,253,20,39,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_INDEX_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,281,26,11,14
   CEdit* mfc11 = new CEdit(this);
   CSpinButtonCtrl* mfc12 = new CSpinButtonCtrl(this);
   mfc11->setBuddy(mfc12);
   mfc12->setBuddy(mfc11);
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CRect r11(CPoint(253,20),CSize(39,12));
   CRect r12(CPoint(r11.right-11,r11.top),CSize(11,12));
   MapDialogRect(&r11);
   MapDialogRect(&r12);
   mfc11->setGeometry(r11);   
   mfcToQtWidget.insert(IDC_INDEX,mfc11);
   QObject::connect(mfc11,SIGNAL(textChanged(QString)),this,SLOT(index_textChanged(QString)));
   mfc12->setGeometry(r12);
   mfcToQtWidget.insert(IDC_INDEX_SPIN,mfc12);
   QObject::connect(mfc12,SIGNAL(valueChanged(int)),this,SLOT(indexSpin_valueChanged(int)));
//       LTEXT           "of",IDC_STATIC,300,22,8,8
   CStatic* mfc13 = new CStatic(this);
   mfc13->setText("of");
   CRect r13(CPoint(300,22),CSize(8,8));
   MapDialogRect(&r13);
   mfc13->setGeometry(r13);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_WAVES,316,20,37,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_WAVES_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,342,19,11,14
   CEdit* mfc14 = new CEdit(this);
   CSpinButtonCtrl* mfc15 = new CSpinButtonCtrl(this);
   mfc14->setBuddy(mfc15);
   mfc15->setBuddy(mfc14);
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CRect r14(CPoint(316,20),CSize(37,12));
   CRect r15(CPoint(r14.right-11,r14.top),CSize(11,12));
   MapDialogRect(&r14);
   MapDialogRect(&r15);
   mfc14->setGeometry(r14);   
   mfcToQtWidget.insert(IDC_WAVES,mfc14);
   QObject::connect(mfc14,SIGNAL(textChanged(QString)),this,SLOT(waves_textChanged(QString)));
   mfc15->setGeometry(r15);
   mfcToQtWidget.insert(IDC_WAVES_SPIN,mfc15);
   QObject::connect(mfc15,SIGNAL(valueChanged(int)),this,SLOT(wavesSpin_valueChanged(int)));
//       LTEXT           "Wave size",IDC_STATIC,253,59,34,8
   CStatic* mfc17 = new CStatic(this);
   mfc17->setText("Wave size");
   CRect r17(CPoint(253,59),CSize(34,8));
   MapDialogRect(&r17);
   mfc17->setGeometry(r17);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_WAVE_SIZE,305,57,48,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc18 = new CComboBox(this);
   CRect r18(CPoint(305,57),CSize(48,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r18);
   mfc18->setGeometry(r18);
   mfcToQtWidget.insert(IDC_WAVE_SIZE,mfc18);
   QObject::connect(mfc18,SIGNAL(currentIndexChanged(int)),this,SLOT(waveSize_currentIndexChanged(int)));
//       LTEXT           "Wave position",IDC_STATIC,253,78,46,8
   CStatic* mfc19 = new CStatic(this);
   mfc19->setText("Wave position");
   CRect r19(CPoint(253,78),CSize(46,8));
   MapDialogRect(&r19);
   mfc19->setGeometry(r19);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_WAVE_POS,305,75,48,30,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc20 = new CComboBox(this);
   CRect r20(CPoint(305,75),CSize(48,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r20);
   mfc20->setGeometry(r20);
   mfcToQtWidget.insert(IDC_WAVE_POS,mfc20);
   QObject::connect(mfc20,SIGNAL(currentIndexChanged(int)),this,SLOT(wavePos_currentIndexChanged(int)));
}

CInstrumentEditorN163Wave::~CInstrumentEditorN163Wave()
{
	SAFE_RELEASE(m_pWaveEditor);
}

void CInstrumentEditorN163Wave::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}

void CInstrumentEditorN163Wave::SelectInstrument(int Instrument)
{
	m_pInstrument = (CInstrumentN163*)GetDocument()->GetInstrument(Instrument);

	CComboBox *pSizeBox = (CComboBox*)GetDlgItem(IDC_WAVE_SIZE);
	CComboBox *pPosBox = (CComboBox*)GetDlgItem(IDC_WAVE_POS);

	CString SizeStr;
	SizeStr.Format(_T("%i"), m_pInstrument->GetWaveSize());
	pSizeBox->SelectString(0, SizeStr);

	FillPosBox(m_pInstrument->GetWaveSize());

	CString PosStr;
	PosStr.Format(_T("%i"), m_pInstrument->GetWavePos());
	pPosBox->SetWindowText(PosStr);

	/*
	if (m_pInstrument->GetAutoWavePos()) {
		CheckDlgButton(IDC_POSITION, 1);
		GetDlgItem(IDC_WAVE_POS)->EnableWindow(FALSE);
	}
	else {
		CheckDlgButton(IDC_POSITION, 0);
		GetDlgItem(IDC_WAVE_POS)->EnableWindow(TRUE);
	}
	*/

	if (m_pWaveEditor) {
		m_pWaveEditor->SetInstrument(m_pInstrument);
		m_pWaveEditor->SetLength(m_pInstrument->GetWaveSize());
	}

	CSpinButtonCtrl *pIndexSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_INDEX_SPIN);
	CSpinButtonCtrl *pWavesSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_WAVES_SPIN);

	int WaveCount = m_pInstrument->GetWaveCount();

	pIndexSpin->SetRange(0, WaveCount - 1);
	pIndexSpin->SetPos(0);
	pWavesSpin->SetPos(WaveCount - 1);

	m_iWaveIndex = 0;
}

//BEGIN_MESSAGE_MAP(CInstrumentEditorN163Wave, CInstrumentEditPanel)
//	ON_COMMAND(IDC_PRESET_SINE, OnPresetSine)
//	ON_COMMAND(IDC_PRESET_TRIANGLE, OnPresetTriangle)
//	ON_COMMAND(IDC_PRESET_SAWTOOTH, OnPresetSawtooth)
//	ON_COMMAND(IDC_PRESET_PULSE_50, OnPresetPulse50)
//	ON_COMMAND(IDC_PRESET_PULSE_25, OnPresetPulse25)
//	ON_MESSAGE(WM_USER_WAVE_CHANGED, OnWaveChanged)
//	ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
//	ON_BN_CLICKED(IDC_PASTE, OnBnClickedPaste)
//	ON_CBN_SELCHANGE(IDC_WAVE_SIZE, OnWaveSizeChange)
//	ON_CBN_EDITCHANGE(IDC_WAVE_POS, OnWavePosChange)
//	ON_CBN_SELCHANGE(IDC_WAVE_POS, OnWavePosSelChange)
////	ON_BN_CLICKED(IDC_POSITION, OnPositionClicked)
//	ON_EN_CHANGE(IDC_WAVES, OnWavesChange)
//	ON_EN_CHANGE(IDC_INDEX, OnIndexChange)
//END_MESSAGE_MAP()

void CInstrumentEditorN163Wave::presetSine_clicked()
{
   OnPresetSine();
}

void CInstrumentEditorN163Wave::presetTriangle_clicked()
{
   OnPresetTriangle();
}

void CInstrumentEditorN163Wave::presetSawtooth_clicked()
{
   OnPresetSawtooth();
}

void CInstrumentEditorN163Wave::presetPulse50_clicked()
{
   OnPresetPulse50();
}

void CInstrumentEditorN163Wave::presetPulse25_clicked()
{
   OnPresetPulse25();
}

void CInstrumentEditorN163Wave::copy_clicked()
{
   OnBnClickedCopy();
}

void CInstrumentEditorN163Wave::paste_clicked()
{
   OnBnClickedPaste();
}

void CInstrumentEditorN163Wave::index_textChanged(QString text)
{
   OnIndexChange();
}

void CInstrumentEditorN163Wave::indexSpin_valueChanged(int val)
{
   OnIndexChange();
}

void CInstrumentEditorN163Wave::waves_textChanged(QString text)
{
   OnWavesChange();
}

void CInstrumentEditorN163Wave::wavesSpin_valueChanged(int val)
{
   OnWavesChange();
}

void CInstrumentEditorN163Wave::waveSize_currentIndexChanged(int index)
{
   OnWaveSizeChange();
}

void CInstrumentEditorN163Wave::wavePos_editTextChanged(QString text)
{
   OnWavePosChange();
}

void CInstrumentEditorN163Wave::wavePos_currentIndexChanged(int index)
{
   OnWavePosSelChange();
}

// CInstrumentEditorN163Wave message handlers

BOOL CInstrumentEditorN163Wave::OnInitDialog()
{
	CInstrumentEditPanel::OnInitDialog();

	// Create wave editor
	CRect rect(SX(20), SY(30), 0, 0);
	m_pWaveEditor = new CWaveEditorN163(10, 8, 32, 16);
	m_pWaveEditor->CreateEx(WS_EX_CLIENTEDGE, NULL, _T(""), WS_CHILD | WS_VISIBLE, rect, this);
	m_pWaveEditor->ShowWindow(SW_SHOW);
	m_pWaveEditor->UpdateWindow();

	CComboBox *pSize = (CComboBox*)GetDlgItem(IDC_WAVE_SIZE);

	CSpinButtonCtrl *pIndexSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_INDEX_SPIN);
	CSpinButtonCtrl *pWavesSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_WAVES_SPIN);

	pIndexSpin->SetRange(0, CInstrumentN163::MAX_WAVE_COUNT - 1);
	pWavesSpin->SetRange(0, CInstrumentN163::MAX_WAVE_COUNT - 1);
	
	CComboBox *pWaveSize = (CComboBox*)GetDlgItem(IDC_WAVE_SIZE);

	for (int i = 0; i < CInstrumentN163::MAX_WAVE_SIZE; i += 4) {
		CString txt;
		txt.Format(_T("%i"), i + 4);
		pWaveSize->AddString(txt);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorN163Wave::OnPresetSine()
{
	int size = m_pInstrument->GetWaveSize();

	for (int i = 0; i < size; ++i) {
		float angle = (float(i) * 3.141592f * 2.0f) / float(size) + 0.049087375f;
		int sample = int((sinf(angle) + 1.0f) * 7.5f + 0.5f);
		m_pInstrument->SetSample(m_iWaveIndex, i, sample);
	}

	m_pWaveEditor->WaveChanged();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorN163Wave::OnPresetTriangle()
{
	int size = m_pInstrument->GetWaveSize();

	for (int i = 0; i < size; ++i) {
		int sample = ((i < (size / 2) ? i : ((size - 1) - i))) * (16 / (size / 2));
		m_pInstrument->SetSample(m_iWaveIndex, i, sample);
	}

	m_pWaveEditor->WaveChanged();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorN163Wave::OnPresetPulse50()
{
	int size = m_pInstrument->GetWaveSize();

	for (int i = 0; i < size; ++i) {
		int sample = (i < (size / 2) ? 0 : 15);
		m_pInstrument->SetSample(m_iWaveIndex, i, sample);
	}

	m_pWaveEditor->WaveChanged();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorN163Wave::OnPresetPulse25()
{
	int size = m_pInstrument->GetWaveSize();

	for (int i = 0; i < size; ++i) {
		int sample = (i < (size / 4) ? 0 : 15);
		m_pInstrument->SetSample(m_iWaveIndex, i, sample);
	}

	m_pWaveEditor->WaveChanged();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorN163Wave::OnPresetSawtooth()
{
	int size = m_pInstrument->GetWaveSize();

	for (int i = 0; i < size; ++i) {
		int sample = (i * 16) / size;
		m_pInstrument->SetSample(m_iWaveIndex, i, sample);
	}

	m_pWaveEditor->WaveChanged();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorN163Wave::OnBnClickedCopy()
{
	CString Str;

	int len = m_pInstrument->GetWaveSize();

	//Str.Format(_T("%i, "), m_pInstrument->GetSamplePos());

	// Assemble a MML string
	for (int i = 0; i < len; ++i)
		Str.AppendFormat(_T("%i "), m_pInstrument->GetSample(m_iWaveIndex,i));

	if (!OpenClipboard())
		return;

	EmptyClipboard();

	int size = Str.GetLength() + 1;
	HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, size);
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);  
	strcpy_s(lptstrCopy, size, Str.GetBuffer());
	GlobalUnlock(hMem);
	SetClipboardData(CF_TEXT, hMem);

	CloseClipboard();	
}

void CInstrumentEditorN163Wave::OnBnClickedPaste()
{
	// Copy from clipboard
	if (!OpenClipboard())
		return;

	HANDLE hMem = GetClipboardData(CF_TEXT);
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);
	ParseString(lptstrCopy);
	GlobalUnlock(hMem);
	CloseClipboard();
}

void CInstrumentEditorN163Wave::ParseString(LPTSTR pString)
{
   qDebug("********************************************** STRING FUNCTIONS!!!!!!");
//	string str(pString);

//	// Convert to register values
//	istringstream values(str);
//	istream_iterator<int> begin(values);
//	istream_iterator<int> end;
//	int i;

//	for (i = 0; (i < CInstrumentN163::MAX_WAVE_SIZE) && (begin != end); ++i) {
//		int value = *begin++;
//		if (value >= 0 && value <= 15)
//			m_pInstrument->SetSample(m_iWaveIndex, i, value);
//	}

//	int size = i & 0xFC;
//	if (size < 4)
//		size = 4;
//	m_pInstrument->SetWaveSize(size);

//	CString SizeStr;
//	SizeStr.Format(_T("%i"), size);
//	((CComboBox*)GetDlgItem(IDC_WAVE_SIZE))->SelectString(0, SizeStr);

//	FillPosBox(size);

//	m_pWaveEditor->SetLength(size);
//	m_pWaveEditor->WaveChanged();
}

LRESULT CInstrumentEditorN163Wave::OnWaveChanged(WPARAM wParam, LPARAM lParam)
{
	CString str;
	int Size = m_pInstrument->GetWaveSize();
	for (int i = 0; i < Size; ++i) {
		str.AppendFormat(_T("%i "), m_pInstrument->GetSample(m_iWaveIndex, i));
	}
	SetDlgItemText(IDC_MML, str);
	return 0;
}

void CInstrumentEditorN163Wave::OnWaveSizeChange()
{
	BOOL trans;
	int size = GetDlgItemInt(IDC_WAVE_SIZE, &trans, FALSE);
	size = size & 0xFC;//(size / 4) * 4;
	
	if (size > CInstrumentN163::MAX_WAVE_SIZE)
		size = CInstrumentN163::MAX_WAVE_SIZE;
	if (size < 4)
		size = 4;

	m_pInstrument->SetWaveSize(size);

	FillPosBox(size);

	m_pWaveEditor->SetLength(size);
	m_pWaveEditor->WaveChanged();
}

void CInstrumentEditorN163Wave::OnWavePosChange()
{
	BOOL trans;
	int pos = GetDlgItemInt(IDC_WAVE_POS, &trans, FALSE);
	
	if (pos > 255)
		pos = 255;
	if (pos < 0)
		pos = 0;

	m_pInstrument->SetWavePos(pos);
}

void CInstrumentEditorN163Wave::OnWavePosSelChange()
{
	CString str;
	CComboBox *pPosBox = (CComboBox*)GetDlgItem(IDC_WAVE_POS);
	pPosBox->GetLBText(pPosBox->GetCurSel(), str);

   int pos = _ttoi(str);

	if (pos > 255)
		pos = 255;
	if (pos < 0)
		pos = 0;

	m_pInstrument->SetWavePos(pos);
}

void CInstrumentEditorN163Wave::FillPosBox(int size)
{
	CComboBox *pPosBox = (CComboBox*)GetDlgItem(IDC_WAVE_POS);
	pPosBox->ResetContent();

	CString str;
	for (int i = 0; i < 128; i += size) {
		str.Format(_T("%i"), i);
		pPosBox->AddString(str);
	}
}
/*
void CInstrumentEditorN163Wave::OnPositionClicked()
{
	if (IsDlgButtonChecked(IDC_POSITION)) {
		GetDlgItem(IDC_WAVE_POS)->EnableWindow(FALSE);
		m_pInstrument->SetAutoWavePos(true);
	}
	else {
		GetDlgItem(IDC_WAVE_POS)->EnableWindow(TRUE);
		m_pInstrument->SetAutoWavePos(false);
	}
}
*/
void CInstrumentEditorN163Wave::OnWavesChange()
{
	CSpinButtonCtrl *pIndexSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_INDEX_SPIN);

	int count = GetDlgItemInt(IDC_WAVES) + 1;
	
	if (m_pInstrument != NULL)
		m_pInstrument->SetWaveCount(count);

	pIndexSpin->SetRange(0, count - 1);
	pIndexSpin->RedrawWindow();

	if (pIndexSpin->GetPos() > (count - 1))
		pIndexSpin->SetPos(count - 1);

	if (m_pWaveEditor != NULL) {
		m_pWaveEditor->SetWave(m_iWaveIndex);
		m_pWaveEditor->WaveChanged();
	}
}

void CInstrumentEditorN163Wave::OnIndexChange()
{
	m_iWaveIndex = GetDlgItemInt(IDC_INDEX);

	if (m_pWaveEditor != NULL) {
		m_pWaveEditor->SetWave(m_iWaveIndex);
		m_pWaveEditor->WaveChanged();
	}
}

void CInstrumentEditorN163Wave::OnKeyReturn()
{
	// Parse MML string
	TCHAR text[256];
	GetDlgItemText(IDC_MML, text, 256);
	ParseString(text);
}
