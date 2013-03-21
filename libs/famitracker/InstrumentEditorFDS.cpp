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
#include "InstrumentEditorFDS.h"
#include "MainFrm.h"

using namespace std;

// CInstrumentEditorFDS dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorFDS, CInstrumentEditPanel)

CInstrumentEditorFDS::CInstrumentEditorFDS(CWnd* pParent) : CInstrumentEditPanel(CInstrumentEditorFDS::IDD, pParent),
	m_pWaveEditor(NULL), 
	m_pModSequenceEditor(NULL), 
	m_pInstrument(NULL)
{
//   IDD_INSTRUMENT_FDS DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
// CP: Put all GROUPBOX items first so their child elements can be created.
//       GROUPBOX        "Wave editor",IDC_STATIC,7,7,228,111
   CGroupBox* mfc1 = new CGroupBox(this);
   mfc1->setTitle("Wave editor");
   CRect r1(CPoint(7,7),CSize(228,111));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Frequency modulation",IDC_STATIC,240,7,124,79
   CGroupBox* mfc16 = new CGroupBox(this);
   mfc16->setTitle("Frequency modulation");
   CRect r16(CPoint(240,7),CSize(124,79));
   MapDialogRect(&r16);
   mfc16->setGeometry(r16);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       PUSHBUTTON      "Sine",IDC_PRESET_SINE,14,101,41,12
   CButton* mfc2 = new CButton(this);
   mfc2->setText("Sine");
   CRect r2(CPoint(14,101),CSize(41,12));
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDC_PRESET_SINE,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(presetSine_clicked()));
//       PUSHBUTTON      "Triangle",IDC_PRESET_TRIANGLE,57,101,41,12
   CButton* mfc3 = new CButton(this);
   mfc3->setText("Triangle");
   CRect r3(CPoint(57,101),CSize(41,12));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_PRESET_TRIANGLE,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),this,SLOT(presetTriangle_clicked()));
//       PUSHBUTTON      "Sawtooth",IDC_PRESET_SAWTOOTH,100,101,41,12
   CButton* mfc4 = new CButton(this);
   mfc4->setText("Sawtooth");
   CRect r4(CPoint(100,101),CSize(41,12));
   MapDialogRect(&r4);
   mfc4->setGeometry(r4);
   mfcToQtWidget.insert(IDC_PRESET_SAWTOOTH,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(presetSawtooth_clicked()));
//       LTEXT           "Modulation rate",IDC_STATIC,246,24,51,8
   CStatic* mfc5 = new CStatic(this);
   mfc5->setText("Modulation rate");
   CRect r5(CPoint(246,24),CSize(51,8));
   MapDialogRect(&r5);
   mfc5->setGeometry(r5);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_RATE,318,22,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_RATE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_NOTHOUSANDS,344,25,11,14
   CEdit* mfc6 = new CEdit(this);
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(this);
   mfc6->setBuddy(mfc7);
   mfc7->setBuddy(mfc6);
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CRect r6(CPoint(318,22),CSize(37,14));
   CRect r7(CPoint(r6.right-11,r6.top),CSize(11,14));
   MapDialogRect(&r6);
   MapDialogRect(&r7);
   mfc6->setGeometry(r6);   
   mfcToQtWidget.insert(IDC_MOD_RATE,mfc6);
   QObject::connect(mfc6,SIGNAL(textChanged(QString)),this,SLOT(modRate_textChanged(QString)));
   mfc7->setGeometry(r7);
   mfcToQtWidget.insert(IDC_MOD_RATE_SPIN,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),this,SLOT(modRateSpin_valueChanged(int)));
//       LTEXT           "Modulation depth",IDC_STATIC,246,46,56,8
   CStatic* mfc8 = new CStatic(this);
   mfc8->setText("Modulation depth");
   CRect r8(CPoint(246,46),CSize(56,8));
   MapDialogRect(&r8);
   mfc8->setGeometry(r8);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DEPTH,318,43,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_DEPTH_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,43,11,14
   CEdit* mfc9 = new CEdit(this);
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(this);
   mfc9->setBuddy(mfc10);
   mfc10->setBuddy(mfc9);
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CRect r9(CPoint(318,43),CSize(37,14));
   CRect r10(CPoint(r9.right-11,r9.top),CSize(11,14));
   MapDialogRect(&r9);
   MapDialogRect(&r10);
   mfc9->setGeometry(r9);   
   mfcToQtWidget.insert(IDC_MOD_DEPTH,mfc9);
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),this,SLOT(modDepth_textChanged(QString)));
   mfc10->setGeometry(r10);
   mfcToQtWidget.insert(IDC_MOD_DEPTH_SPIN,mfc10);
   QObject::connect(mfc10,SIGNAL(valueChanged(int)),this,SLOT(modDepthSpin_valueChanged(int)));
//       LTEXT           "Modulation delay",IDC_STATIC,246,68,55,8
   CStatic* mfc11 = new CStatic(this);
   mfc11->setText("Modulation delay");
   CRect r11(CPoint(246,68),CSize(55,8));
   MapDialogRect(&r11);
   mfc11->setGeometry(r11);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_MOD_DELAY,318,65,37,14,ES_AUTOHSCROLL | ES_NUMBER
//       CONTROL         "",IDC_MOD_DELAY_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,344,63,11,14
   CEdit* mfc12 = new CEdit(this);
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(this);
   mfc12->setBuddy(mfc13);
   mfc13->setBuddy(mfc12);
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CRect r12(CPoint(318,65),CSize(37,14));
   CRect r13(CPoint(r12.right-11,r12.top),CSize(11,14));
   MapDialogRect(&r12);
   MapDialogRect(&r13);
   mfc12->setGeometry(r12);   
   mfcToQtWidget.insert(IDC_MOD_DELAY,mfc12);
   QObject::connect(mfc12,SIGNAL(textChanged(QString)),this,SLOT(modDelay_textChanged(QString)));
   mfc13->setGeometry(r13);
   mfcToQtWidget.insert(IDC_MOD_DELAY_SPIN,mfc13);
   QObject::connect(mfc13,SIGNAL(valueChanged(int)),this,SLOT(modDelaySpin_valueChanged(int)));
//       PUSHBUTTON      "Flat",IDC_MOD_PRESET_FLAT,318,131,46,12
   CButton* mfc14 = new CButton(this);
   mfc14->setText("Flat");
   CRect r14(CPoint(318,131),CSize(46,12));
   MapDialogRect(&r14);
   mfc14->setGeometry(r14);
   mfcToQtWidget.insert(IDC_MOD_PRESET_FLAT,mfc14);
   QObject::connect(mfc14,SIGNAL(clicked()),this,SLOT(modPresetFlat_clicked()));
//       PUSHBUTTON      "Sine",IDC_MOD_PRESET_SINE,318,148,46,12
   CButton* mfc15 = new CButton(this);
   mfc15->setText("Sine");
   CRect r15(CPoint(318,148),CSize(46,12));
   MapDialogRect(&r15);
   mfc15->setGeometry(r15);
   mfcToQtWidget.insert(IDC_MOD_PRESET_SINE,mfc15);
   QObject::connect(mfc15,SIGNAL(clicked()),this,SLOT(modPresetSine_clicked()));
//       PUSHBUTTON      "Copy wave",IDC_COPY_WAVE,240,90,52,14
   CButton* mfc17 = new CButton(this);
   mfc17->setText("Copy wave");
   CRect r17(CPoint(240,90),CSize(52,14));
   MapDialogRect(&r17);
   mfc17->setGeometry(r17);
   mfcToQtWidget.insert(IDC_COPY_WAVE,mfc17);
   QObject::connect(mfc17,SIGNAL(clicked()),this,SLOT(copyWave_clicked()));
//       PUSHBUTTON      "Paste wave",IDC_PASTE_WAVE,240,104,52,14
   CButton* mfc18 = new CButton(this);
   mfc18->setText("Paste wave");
   CRect r18(CPoint(240,104),CSize(52,14));
   MapDialogRect(&r18);
   mfc18->setGeometry(r18);
   mfcToQtWidget.insert(IDC_PASTE_WAVE,mfc18);
   QObject::connect(mfc18,SIGNAL(clicked()),this,SLOT(pasteWave_clicked()));
//       PUSHBUTTON      "Copy table",IDC_COPY_TABLE,305,90,50,14
   CButton* mfc19 = new CButton(this);
   mfc19->setText("Copy table");
   CRect r19(CPoint(305,90),CSize(50,14));
   MapDialogRect(&r19);
   mfc19->setGeometry(r19);
   mfcToQtWidget.insert(IDC_COPY_TABLE,mfc19);
   QObject::connect(mfc19,SIGNAL(clicked()),this,SLOT(copyTable_clicked()));
//       PUSHBUTTON      "Paste table",IDC_PASTE_TABLE,305,104,50,14
   CButton* mfc20 = new CButton(this);
   mfc20->setText("Paste table");
   CRect r20(CPoint(305,104),CSize(50,14));
   MapDialogRect(&r20);
   mfc20->setGeometry(r20);
   mfcToQtWidget.insert(IDC_PASTE_TABLE,mfc20);
   QObject::connect(mfc20,SIGNAL(clicked()),this,SLOT(pasteTable_clicked()));
//       PUSHBUTTON      "50% pulse",IDC_PRESET_PULSE_50,143,101,41,12
   CButton* mfc21 = new CButton(this);
   mfc21->setText("50% pulse");
   CRect r21(CPoint(143,101),CSize(41,12));
   MapDialogRect(&r21);
   mfc21->setGeometry(r21);
   mfcToQtWidget.insert(IDC_PRESET_PULSE_50,mfc21);
   QObject::connect(mfc21,SIGNAL(clicked()),this,SLOT(presetPulse50_clicked()));
//       PUSHBUTTON      "25% pulse",IDC_PRESET_PULSE_25,186,101,41,12
   CButton* mfc22 = new CButton(this);
   mfc22->setText("25% pulse");
   CRect r22(CPoint(186,101),CSize(41,12));
   MapDialogRect(&r22);
   mfc22->setGeometry(r22);
   mfcToQtWidget.insert(IDC_PRESET_PULSE_25,mfc22);
   QObject::connect(mfc22,SIGNAL(clicked()),this,SLOT(presetPulse25_clicked()));
}

CInstrumentEditorFDS::~CInstrumentEditorFDS()
{
	SAFE_RELEASE(m_pModSequenceEditor);
	SAFE_RELEASE(m_pWaveEditor);
}

void CInstrumentEditorFDS::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}

void CInstrumentEditorFDS::SelectInstrument(int Instrument)
{
	m_pInstrument = (CInstrumentFDS*)GetDocument()->GetInstrument(Instrument);

	if (m_pWaveEditor)
		m_pWaveEditor->SetInstrument(m_pInstrument);

	if (m_pModSequenceEditor)
		m_pModSequenceEditor->SetInstrument(m_pInstrument);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_RATE_SPIN))->SetPos(m_pInstrument->GetModulationSpeed());
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_DEPTH_SPIN))->SetPos(m_pInstrument->GetModulationDepth());
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_DELAY_SPIN))->SetPos(m_pInstrument->GetModulationDelay());

//	CheckDlgButton(IDC_ENABLE_FM, m_pInstrument->GetModulationEnable() ? 1 : 0);

	EnableModControls(m_pInstrument->GetModulationEnable());
}


//BEGIN_MESSAGE_MAP(CInstrumentEditorFDS, CInstrumentEditPanel)
//	ON_COMMAND(IDC_PRESET_SINE, OnPresetSine)
//	ON_COMMAND(IDC_PRESET_TRIANGLE, OnPresetTriangle)
//	ON_COMMAND(IDC_PRESET_SAWTOOTH, OnPresetSawtooth)
//	ON_COMMAND(IDC_PRESET_PULSE_50, OnPresetPulse50)
//	ON_COMMAND(IDC_PRESET_PULSE_25, OnPresetPulse25)
//	ON_COMMAND(IDC_MOD_PRESET_FLAT, OnModPresetFlat)
//	ON_COMMAND(IDC_MOD_PRESET_SINE, OnModPresetSine)
//	ON_WM_VSCROLL()
//	ON_EN_CHANGE(IDC_MOD_RATE, OnModRateChange)
//	ON_EN_CHANGE(IDC_MOD_DEPTH, OnModDepthChange)
//	ON_EN_CHANGE(IDC_MOD_DELAY, OnModDelayChange)
//	ON_BN_CLICKED(IDC_COPY_WAVE, &CInstrumentEditorFDS::OnBnClickedCopyWave)
//	ON_BN_CLICKED(IDC_PASTE_WAVE, &CInstrumentEditorFDS::OnBnClickedPasteWave)
//	ON_BN_CLICKED(IDC_COPY_TABLE, &CInstrumentEditorFDS::OnBnClickedCopyTable)
//	ON_BN_CLICKED(IDC_PASTE_TABLE, &CInstrumentEditorFDS::OnBnClickedPasteTable)
////	ON_BN_CLICKED(IDC_ENABLE_FM, &CInstrumentEditorFDS::OnBnClickedEnableFm)
//	ON_MESSAGE(WM_USER + 1, OnModChanged)
//END_MESSAGE_MAP()

// CInstrumentEditorFDS message handlers

BOOL CInstrumentEditorFDS::OnInitDialog()
{
	CInstrumentEditPanel::OnInitDialog();

	// Create wave editor
	CRect rect(SX(20), SY(30), 0, 0);
	m_pWaveEditor = new CWaveEditorFDS(5, 2, 64, 64);
	m_pWaveEditor->CreateEx(WS_EX_CLIENTEDGE, NULL, _T(""), WS_CHILD | WS_VISIBLE, rect, this);
	m_pWaveEditor->ShowWindow(SW_SHOW);
	m_pWaveEditor->UpdateWindow();

	// Create modulation sequence editor
	rect = CRect(SX(10), SY(200), 0, 0);
	m_pModSequenceEditor = new CModSequenceEditor();
	m_pModSequenceEditor->CreateEx(WS_EX_CLIENTEDGE, NULL, _T(""), WS_CHILD | WS_VISIBLE, rect, this);
	m_pModSequenceEditor->ShowWindow(SW_SHOW);
	m_pModSequenceEditor->UpdateWindow();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_RATE_SPIN))->SetRange(0, 4095);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_DEPTH_SPIN))->SetRange(0, 63);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MOD_DELAY_SPIN))->SetRange(0, 255);
/*
	CSliderCtrl *pModSlider;
	pModSlider = (CSliderCtrl*)GetDlgItem(IDC_MOD_FREQ);
	pModSlider->SetRange(0, 0xFFF);
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorFDS::OnPresetSine()
{
	for (int i = 0; i < 64; ++i) {
		float angle = (float(i) * 3.141592f * 2.0f) / 64.0f + 0.049087375f;
		int sample = int((sinf(angle) + 1.0f) * 31.5f + 0.5f);
		m_pInstrument->SetSample(i, sample);
	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnPresetTriangle()
{
	for (int i = 0; i < 64; ++i) {
		int sample = (i < 32 ? i << 1 : (63 - i) << 1);
		m_pInstrument->SetSample(i, sample);
	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnPresetPulse50()
{
	for (int i = 0; i < 64; ++i) {
		int sample = (i < 32 ? 0 : 63);
		m_pInstrument->SetSample(i, sample);
	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnPresetPulse25()
{
	for (int i = 0; i < 64; ++i) {
		int sample = (i < 16 ? 0 : 63);
		m_pInstrument->SetSample(i, sample);
	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnPresetSawtooth()
{
	for (int i = 0; i < 64; ++i) {
		int sample = i;
		m_pInstrument->SetSample(i, sample);
	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnModPresetFlat()
{
	for (int i = 0; i < 32; ++i) {
		m_pInstrument->SetModulation(i, 0);
	}

	m_pModSequenceEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnModPresetSine()
{
	for (int i = 0; i < 8; ++i) {
		m_pInstrument->SetModulation(i, 7);
		m_pInstrument->SetModulation(i + 8, 1);
		m_pInstrument->SetModulation(i + 16, 1);
		m_pInstrument->SetModulation(i + 24, 7);
	}

	m_pInstrument->SetModulation(7, 0);
	m_pInstrument->SetModulation(8, 0);
	m_pInstrument->SetModulation(9, 0);

	m_pInstrument->SetModulation(23, 0);
	m_pInstrument->SetModulation(24, 0);
	m_pInstrument->SetModulation(25, 0);

	m_pModSequenceEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int ModSpeed = GetDlgItemInt(IDC_MOD_RATE);
	int ModDepth = GetDlgItemInt(IDC_MOD_DEPTH);
	int ModDelay = GetDlgItemInt(IDC_MOD_DELAY);

	m_pInstrument->SetModulationSpeed(ModSpeed);
}

void CInstrumentEditorFDS::OnModRateChange()
{
	if (m_pInstrument) {
		int ModSpeed = GetDlgItemInt(IDC_MOD_RATE);
		LIMIT(ModSpeed, 4095, 0);
		m_pInstrument->SetModulationSpeed(ModSpeed);
	}
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnModDepthChange()
{
	if (m_pInstrument) {
		int ModDepth = GetDlgItemInt(IDC_MOD_DEPTH);
		LIMIT(ModDepth, 63, 0);
		m_pInstrument->SetModulationDepth(ModDepth);
	}
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnModDelayChange()
{
	if (m_pInstrument) {
		int ModDelay = GetDlgItemInt(IDC_MOD_DELAY);
		LIMIT(ModDelay, 255, 0);
		m_pInstrument->SetModulationDelay(ModDelay);
	}
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::PreviewNote(unsigned char Key)
{
	CFamiTrackerView::GetView()->PreviewNote(Key);
}

void CInstrumentEditorFDS::OnBnClickedCopyWave()
{
	CString Str;

	// Assemble a MML string
	for (int i = 0; i < 64; ++i)
		Str.AppendFormat(_T("%i "), m_pInstrument->GetSample(i));

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

void CInstrumentEditorFDS::OnBnClickedPasteWave()
{
	// Copy from clipboard
	if (!OpenClipboard())
		return;

	HANDLE hMem = GetClipboardData(CF_TEXT);

	if (!hMem) {
		CloseClipboard();
		return;
	}

	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);
	
	if (!lptstrCopy) {
		CloseClipboard();
		return;
	}
   
   qDebug("string stuff not found/implemented yet");
//	string str(lptstrCopy);
//	GlobalUnlock(hMem);
//	CloseClipboard();

//	// Convert to register values
//	istringstream values(str);
//	istream_iterator<int> begin(values);
//	istream_iterator<int> end;

//	for (int i = 0; (i < 64) && (begin != end); ++i) {
//		int value = *begin++;
//		if (value >= 0 && value <= 63)
//			m_pInstrument->SetSample(i, value);
//	}

	m_pWaveEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnBnClickedCopyTable()
{
	CString Str;

	// Assemble a MML string
	for (int i = 0; i < 32; ++i)
		Str.AppendFormat(_T("%i "), m_pInstrument->GetModulation(i));

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

void CInstrumentEditorFDS::OnBnClickedPasteTable()
{
	// Copy from clipboard
	if (!OpenClipboard())
		return;

	HANDLE hMem = GetClipboardData(CF_TEXT);

	if (!hMem) {
		CloseClipboard();
		return;
	}

	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);

	if (!lptstrCopy) {
		CloseClipboard();
		return;
	}

   qDebug("string stuff not found/implemented yet");
//	string str(lptstrCopy);
//	GlobalUnlock(hMem);
//	CloseClipboard();

//	// Convert to register values
//	istringstream values(str);
//	istream_iterator<int> begin(values);
//	istream_iterator<int> end;

//	for (int i = 0; (i < 32) && (begin != end); ++i) {
//		int value = *begin++;
//		if (value >= 0 && value <= 7)
//			m_pInstrument->SetModulation(i, value);
//	}

	m_pModSequenceEditor->RedrawWindow();
	theApp.GetSoundGenerator()->WaveChanged();
}

void CInstrumentEditorFDS::OnBnClickedEnableFm()
{
	/*
	UINT button = IsDlgButtonChecked(IDC_ENABLE_FM);

	EnableModControls(button == 1);
	*/
}

void CInstrumentEditorFDS::EnableModControls(bool enable)
{
	if (!enable) {
		GetDlgItem(IDC_MOD_RATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOD_DEPTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOD_DELAY)->EnableWindow(FALSE);
//		m_pInstrument->SetModulationEnable(false);
	}
	else {
		GetDlgItem(IDC_MOD_RATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOD_DEPTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOD_DELAY)->EnableWindow(TRUE);
//		m_pInstrument->SetModulationEnable(true);
	}
}

LRESULT CInstrumentEditorFDS::OnModChanged(WPARAM wParam, LPARAM lParam)
{
	theApp.GetSoundGenerator()->WaveChanged();
	return 0;
}

void CInstrumentEditorFDS::presetSine_clicked()
{
   OnPresetSine();
}

void CInstrumentEditorFDS::presetTriangle_clicked()
{
   OnPresetTriangle();
}

void CInstrumentEditorFDS::presetSawtooth_clicked()
{
   OnPresetSawtooth();
}

void CInstrumentEditorFDS::modRate_textChanged(QString text)
{
   OnModRateChange();
}

void CInstrumentEditorFDS::modRateSpin_valueChanged(int val)
{
   // Update the "buddy"
   SetDlgItemInt(IDC_MOD_RATE,val);
   OnModRateChange();
}

void CInstrumentEditorFDS::modDepth_textChanged(QString text)
{
   OnModDepthChange();
}

void CInstrumentEditorFDS::modDepthSpin_valueChanged(int val)
{
   // Update the "buddy"
   SetDlgItemInt(IDC_MOD_DEPTH,val);
   OnModDepthChange();
}

void CInstrumentEditorFDS::modDelay_textChanged(QString text)
{
   OnModDelayChange();
}

void CInstrumentEditorFDS::modDelaySpin_valueChanged(int val)
{
   // Update the "buddy"
   SetDlgItemInt(IDC_MOD_DELAY,val);
   OnModDelayChange();
}

void CInstrumentEditorFDS::modPresetFlat_clicked()
{
   OnModPresetFlat();
}

void CInstrumentEditorFDS::modPresetSine_clicked()
{
   OnModPresetSine();
}

void CInstrumentEditorFDS::copyWave_clicked()
{
   OnBnClickedCopyWave();
}

void CInstrumentEditorFDS::pasteWave_clicked()
{
   OnBnClickedPasteWave();
}

void CInstrumentEditorFDS::copyTable_clicked()
{
   OnBnClickedCopyTable();
}

void CInstrumentEditorFDS::pasteTable_clicked()
{
   OnBnClickedPasteTable();
}

void CInstrumentEditorFDS::presetPulse50_clicked()
{
   OnPresetPulse50();
}

void CInstrumentEditorFDS::presetPulse25_clicked()
{
   OnPresetPulse25();
}
