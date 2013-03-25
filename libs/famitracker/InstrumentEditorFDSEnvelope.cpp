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
#include <cmath>
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "InstrumentEditPanel.h"
#include "SequenceEditor.h"
#include "InstrumentEditorFDSEnvelope.h"

// CInstrumentEditorFDSEnvelope dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorFDSEnvelope, CSequenceInstrumentEditPanel)

CInstrumentEditorFDSEnvelope::CInstrumentEditorFDSEnvelope(CWnd* pParent) : CSequenceInstrumentEditPanel(CInstrumentEditorFDSEnvelope::IDD, pParent),
	m_pInstrument(NULL), 
	m_pSequenceEditor(NULL),
	m_iSelectedType(0)
{
//   IDD_INSTRUMENT_FDS_ENVELOPE DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());   

//       EDITTEXT        IDC_SEQUENCE_STRING,75,151,290,14,ES_AUTOHSCROLL
   CEdit* mfc1 = new CEdit(this);
   CRect r1(CPoint(75,151),CSize(290,14));
   MapDialogRect(&r1);
   mfc1->Create(ES_AUTOHSCROLL | WS_VISIBLE,r1,this,IDC_SEQUENCE_STRING);
   mfcToQtWidget.insert(IDC_SEQUENCE_STRING,mfc1);
//       COMBOBOX        IDC_TYPE,8,151,63,30,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(this);
   CRect r2(CPoint(8,151),CSize(63,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDC_TYPE,mfc2);
//   IDD_INSTRUMENT_FDS_ENVELOPE DLGINIT
//   BEGIN
//       IDC_TYPE, 0x403, 7, 0
//   0x6f56, 0x756c, 0x656d, "\000" 
//       IDC_TYPE, 0x403, 9, 0
//   0x7241, 0x6570, 0x6767, 0x6f69, "\000" 
//       IDC_TYPE, 0x403, 6, 0
//   0x6950, 0x6374, 0x0068, 
//       0
//   END
   mfc2->AddString(_T("Volume"));
   mfc2->AddString(_T("Arpeggio"));
   mfc2->AddString(_T("Pitch"));
   QObject::connect(mfc2,SIGNAL(currentIndexChanged(int)),this,SLOT(type_currentIndexChanged(int)));
}

CInstrumentEditorFDSEnvelope::~CInstrumentEditorFDSEnvelope()
{
	SAFE_RELEASE(m_pSequenceEditor);
}

void CInstrumentEditorFDSEnvelope::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}

void CInstrumentEditorFDSEnvelope::SelectInstrument(int Instrument)
{
	CInstrumentFDS *pInst = (CInstrumentFDS*)GetDocument()->GetInstrument(Instrument);
	m_pInstrument = pInst;
	LoadSequence();
}


//BEGIN_MESSAGE_MAP(CInstrumentEditorFDSEnvelope, CInstrumentEditPanel)
//	ON_CBN_SELCHANGE(IDC_TYPE, &CInstrumentEditorFDSEnvelope::OnCbnSelchangeType)
//END_MESSAGE_MAP()

void CInstrumentEditorFDSEnvelope::type_currentIndexChanged(int index)
{
   OnCbnSelchangeType();
}

// CInstrumentEditorFDSEnvelope message handlers

BOOL CInstrumentEditorFDSEnvelope::OnInitDialog()
{
	CInstrumentEditPanel::OnInitDialog();

	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(10, 10, 10, 45);

	m_pSequenceEditor = new CSequenceEditor(GetDocument());
	m_pSequenceEditor->CreateEditor(this, rect);
	m_pSequenceEditor->SetMaxValues(MAX_VOLUME, 0);

	((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorFDSEnvelope::SetSequenceString(CString Sequence, bool Changed)
{
	SetDlgItemText(IDC_SEQUENCE_STRING, Sequence);
}

void CInstrumentEditorFDSEnvelope::OnCbnSelchangeType()
{
	CComboBox *pTypeBox = (CComboBox*)GetDlgItem(IDC_TYPE);
	m_iSelectedType = pTypeBox->GetCurSel();
	LoadSequence();
}

void CInstrumentEditorFDSEnvelope::LoadSequence()
{
	switch (m_iSelectedType) {
		case SEQ_VOLUME:
			m_pSequenceEditor->SelectSequence(m_pInstrument->GetVolumeSeq(), SEQ_VOLUME, INST_FDS);	
			break;
		case SEQ_ARPEGGIO:
			m_pSequenceEditor->SelectSequence(m_pInstrument->GetArpSeq(), SEQ_ARPEGGIO, INST_FDS);
			break;
		case SEQ_PITCH:
			m_pSequenceEditor->SelectSequence(m_pInstrument->GetPitchSeq(), SEQ_PITCH, INST_FDS);
			break;
	}
}

void CInstrumentEditorFDSEnvelope::OnKeyReturn()
{
	CString string;

	GetDlgItemText(IDC_SEQUENCE_STRING, string);

	if (m_iSelectedType == SEQ_VOLUME) {
		TranslateMML(string, m_pInstrument->GetVolumeSeq(), MAX_VOLUME, 0);
	}
	else if (m_iSelectedType == SEQ_ARPEGGIO) {
		TranslateMML(string, m_pInstrument->GetArpSeq(), 96, -96);
	}
	else if (m_iSelectedType == SEQ_PITCH) {
		TranslateMML(string, m_pInstrument->GetPitchSeq(), 126, -127);
	}

	// Update editor
	m_pSequenceEditor->RedrawWindow();

	// Register a document change
	GetDocument()->SetModifiedFlag();
}
