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
#include "SequenceEditor.h"
#include "InstrumentEditorS5B.h"

LPCTSTR CInstrumentEditorS5B::INST_SETTINGS_S5B[] = {
	_T("Volume"), 
	_T("Arpeggio"), 
	_T("Pitch"), 
	_T("Hi-pitch"), 
	_T("Noise / mode")
};

// CInstrumentEditorS5B dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorS5B, CSequenceInstrumentEditPanel)
CInstrumentEditorS5B::CInstrumentEditorS5B(CWnd* pParent) : CSequenceInstrumentEditPanel(CInstrumentEditorS5B::IDD, pParent),
	m_pParentWin(pParent),
	m_pInstrument(NULL),
	m_pSequence(NULL),
	m_pSequenceEditor(NULL),
	m_iSelectedSetting(0)
{
//   IDD_INSTRUMENT_INTERNAL DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
//   GROUPBOX        "Sequence editor",IDC_STATIC,120,7,245,158
   CGroupBox* mfc5 = new CGroupBox(this);
   CRect r5(CPoint(120,7),CSize(245,158));
   MapDialogRect(&r5);
   mfc5->Create(_T("Sequence editor"),WS_VISIBLE,r5,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   GROUPBOX        "Instrument settings",IDC_STATIC,7,7,107,158,0,WS_EX_TRANSPARENT
   CGroupBox* mfc6 = new CGroupBox(this);
   CRect r6(CPoint(7,7),CSize(107,158));
   MapDialogRect(&r6);
   mfc6->Create(_T("Instrument settings"),WS_VISIBLE,r6,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   CONTROL         "",IDC_INSTSETTINGS,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,12,18,96,109,WS_EX_TRANSPARENT
   CListCtrl* mfc1 = new CListCtrl(this);
   CRect r1(CPoint(12,18),CSize(96,109));
   MapDialogRect(&r1);
   mfc1->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r1,this,IDC_INSTSETTINGS);
   mfcToQtWidget.insert(IDC_INSTSETTINGS,mfc1);
   QObject::connect(mfc1,SIGNAL(itemSelectionChanged()),this,SLOT(instSettings_itemSelectionChanged()));
//   CONTROL         "Sequence #",IDC_STATIC,"Static",SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP,12,149,53,10,WS_EX_TRANSPARENT
   CStatic* mfc2 = new CStatic(this);
   CRect r2(CPoint(12,149),CSize(53,10));
   MapDialogRect(&r2);
   mfc2->Create(_T("Sequence #"),SS_LEFTNOWORDWRAP | SS_CENTERIMAGE | WS_GROUP | WS_VISIBLE,r2,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//   EDITTEXT        IDC_SEQ_INDEX,69,147,39,12,ES_AUTOHSCROLL | ES_NUMBER
//   CONTROL         "",IDC_SEQUENCE_SPIN,"msctls_updown32",UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,66,153,11,9
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc3 = new CEdit(this);
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(this);
   CRect r3(CPoint(69,147),CSize(39,12));
   CRect r4(CPoint(r3.right-11,r3.top),CSize(11,12));
   MapDialogRect(&r3);
   MapDialogRect(&r4);
   mfc3->Create(ES_AUTOHSCROLL | ES_NUMBER | WS_VISIBLE,r3,this,IDC_SEQ_INDEX);
   mfc3->setBuddy(mfc4);
   mfcToQtWidget.insert(IDC_SEQ_INDEX,mfc3);
   QObject::connect(mfc3,SIGNAL(textChanged(QString)),this,SLOT(seqIndex_textChanged(QString)));
   mfc4->Create(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,this,IDC_SEQUENCE_SPIN);
   mfc4->setBuddy(mfc3);
   mfcToQtWidget.insert(IDC_SEQUENCE_SPIN,mfc4);
   QObject::connect(mfc4,SIGNAL(valueChanged(int)),this,SLOT(sequenceSpin_valueChanged(int)));   
//   PUSHBUTTON      "Select next empty slot",IDC_FREE_SEQ,12,129,96,15
   CButton* mfc7 = new CButton(this);
   CRect r7(CPoint(12,129),CSize(96,15));
   MapDialogRect(&r7);
   mfc7->Create(_T("Select next empty slot"),WS_VISIBLE,r7,this,IDC_FREE_SEQ);
   mfcToQtWidget.insert(IDC_FREE_SEQ,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),this,SLOT(freeSeq_clicked()));
//   EDITTEXT        IDC_SEQUENCE_STRING,126,149,232,13,ES_AUTOHSCROLL
   CEdit* mfc8 = new CEdit(this);
   CRect r8(CPoint(126,149),CSize(232,13));
   MapDialogRect(&r8);
   mfc8->Create(ES_AUTOHSCROLL | WS_VISIBLE,r8,this,IDC_SEQUENCE_STRING);
   mfcToQtWidget.insert(IDC_SEQUENCE_STRING,mfc8);
}

CInstrumentEditorS5B::~CInstrumentEditorS5B()
{
	SAFE_RELEASE(m_pSequenceEditor);
}

void CInstrumentEditorS5B::DoDataExchange(CDataExchange* pDX)
{
	CSequenceInstrumentEditPanel::DoDataExchange(pDX);
}

void CInstrumentEditorS5B::SelectInstrument(int Instrument)
{
	CInstrumentS5B *pInst = (CInstrumentS5B*)GetDocument()->GetInstrument(Instrument);
	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_INSTSETTINGS);

	m_pInstrument = NULL;

	// Update instrument setting list
	for (int i = 0; i < SEQ_COUNT; ++i) {
		CString IndexStr;
		IndexStr.Format(_T("%i"), pInst->GetSeqIndex(i));
		pList->SetCheck(i, pInst->GetSeqEnable(i));
		pList->SetItemText(i, 1, IndexStr);
	} 

	// Setting text box
	SetDlgItemInt(IDC_SEQ_INDEX, pInst->GetSeqIndex(m_iSelectedSetting));

	m_pInstrument = pInst;

	// Select new sequence
	SelectSequence(pInst->GetSeqIndex(m_iSelectedSetting), m_iSelectedSetting);
}

void CInstrumentEditorS5B::SelectSequence(int Sequence, int Type)
{
	// Selects the current sequence in the sequence editor
	m_pSequence = GetDocument()->GetSequence(SNDCHIP_S5B, Sequence, Type);
	m_pSequenceEditor->SelectSequence(m_pSequence, Type, INST_S5B);
}

void CInstrumentEditorS5B::TranslateMML(CString String, int Max, int Min)
{
	CSequenceInstrumentEditPanel::TranslateMML(String, m_pSequence, Max, Min);

	// Update editor
	m_pSequenceEditor->RedrawWindow();

	// Register a document change
	GetDocument()->SetModifiedFlag();

	// Enable setting
	((CListCtrl*)GetDlgItem(IDC_INSTSETTINGS))->SetCheck(m_iSelectedSetting, 1);
}

void CInstrumentEditorS5B::SetSequenceString(CString Sequence, bool Changed)
{
	// Update sequence string
	SetDlgItemText(IDC_SEQUENCE_STRING, Sequence);
	// If the sequence was changed, assume the user wants to enable it
	if (Changed) {
		((CListCtrl*)GetDlgItem(IDC_INSTSETTINGS))->SetCheck(m_iSelectedSetting, 1);
	}
}

//BEGIN_MESSAGE_MAP(CInstrumentEditorS5B, CInstrumentEditPanel)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INSTSETTINGS, OnLvnItemchangedInstsettings)	
//	ON_EN_CHANGE(IDC_SEQ_INDEX, OnEnChangeSeqIndex)
//	ON_BN_CLICKED(IDC_FREE_SEQ, OnBnClickedFreeSeq)
//END_MESSAGE_MAP()

void CInstrumentEditorS5B::instSettings_itemSelectionChanged()
{
   CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_INSTSETTINGS);
   NMLISTVIEW nmlv;
   LRESULT result;
   
   nmlv.uChanged = LVIF_STATE;
   nmlv.iItem = pList->currentIndex().row();
   nmlv.iSubItem = pList->currentIndex().column();
   nmlv.uNewState = LCTRL_CHECKBOX_STATE|LVNI_SELECTED;
   nmlv.uOldState = 0;
   
   OnLvnItemchangedInstsettings((NMHDR*)&nmlv,&result);
}

void CInstrumentEditorS5B::freeSeq_clicked()
{
   OnBnClickedFreeSeq();
}

void CInstrumentEditorS5B::sequenceSpin_valueChanged(int val)
{
   // Update the "buddy"
   SetDlgItemInt(IDC_SEQ_INDEX,val);
   OnEnChangeSeqIndex();
}

void CInstrumentEditorS5B::seqIndex_textChanged(QString)
{
   OnEnChangeSeqIndex();
}

// CInstrumentSettings message handlers

BOOL CInstrumentEditorS5B::OnInitDialog()
{
	CInstrumentEditPanel::OnInitDialog();

	// Instrument settings
	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_INSTSETTINGS);
	pList->DeleteAllItems();
	pList->InsertColumn(0, _T(""), LVCFMT_LEFT, 26);
	pList->InsertColumn(1, _T("#"), LVCFMT_LEFT, 30);
	pList->InsertColumn(2, _T("Effect name"), LVCFMT_LEFT, 84);
   qDebug("CListCtrl::SendMessage");
//	pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	
	for (int i = SEQ_COUNT - 1; i > -1; i--) {
		pList->InsertItem(0, _T(""), 0);
		pList->SetCheck(0, 0);
		pList->SetItemText(0, 1, _T("0"));
		pList->SetItemText(0, 2, INST_SETTINGS_S5B[i]);
	}

	pList->SetItemState(m_iSelectedSetting, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	SetDlgItemInt(IDC_SEQ_INDEX, m_iSelectedSetting);

	CSpinButtonCtrl *pSequenceSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SEQUENCE_SPIN);
	pSequenceSpin->SetRange(0, MAX_SEQUENCES - 1);

	CRect rect(190 - 2, 30 - 2, CSequenceEditor::SEQUENCE_EDIT_WIDTH, CSequenceEditor::SEQUENCE_EDIT_HEIGHT);
	
	m_pSequenceEditor = new CSequenceEditor(GetDocument());	
	m_pSequenceEditor->CreateEditor(this, rect);
	m_pSequenceEditor->ShowWindow(SW_SHOW);
	m_pSequenceEditor->SetMaxValues(MAX_VOLUME, MAX_DUTY);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorS5B::OnLvnItemchangedInstsettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_INSTSETTINGS);

	if (pNMLV->uChanged & LVIF_STATE && m_pInstrument != NULL) {
		// Selected new setting
		if (pNMLV->uNewState & LVNI_SELECTED || pNMLV->uNewState & LCTRL_CHECKBOX_STATE) {
			m_iSelectedSetting = pNMLV->iItem;
			int Sequence = m_pInstrument->GetSeqIndex(m_iSelectedSetting);
			SetDlgItemInt(IDC_SEQ_INDEX, Sequence);
			SelectSequence(Sequence, m_iSelectedSetting);
			pList->SetSelectionMark(m_iSelectedSetting);
			pList->SetItemState(m_iSelectedSetting, LVIS_SELECTED, LVIS_SELECTED);
		}

		// Changed checkbox
		switch(pNMLV->uNewState & LCTRL_CHECKBOX_STATE) {
			case LCTRL_CHECKBOX_CHECKED:
				m_pInstrument->SetSeqEnable(m_iSelectedSetting, 1);
				break;
			case LCTRL_CHECKBOX_UNCHECKED:
				m_pInstrument->SetSeqEnable(m_iSelectedSetting, 0);
				break;
		}
	}

	*pResult = 0;
}

void CInstrumentEditorS5B::OnEnChangeSeqIndex()
{
	// Selected sequence changed
	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_INSTSETTINGS);
	int Index = GetDlgItemInt(IDC_SEQ_INDEX);

	if (Index < 0)
		Index = 0;
	if (Index > (MAX_SEQUENCES - 1))
		Index = (MAX_SEQUENCES - 1);
	
	if (m_pInstrument != NULL) {
		// Update list
		CString Text;
		Text.Format(_T("%i"), Index);
		pList->SetItemText(m_iSelectedSetting, 1, Text);

		if (m_pInstrument->GetSeqIndex(m_iSelectedSetting) != Index)
			m_pInstrument->SetSeqIndex(m_iSelectedSetting, Index);

		SelectSequence(Index, m_iSelectedSetting);
	}
}

void CInstrumentEditorS5B::OnBnClickedFreeSeq()
{
	int FreeIndex = GetDocument()->GetFreeSequenceS5B(m_iSelectedSetting);
	SetDlgItemInt(IDC_SEQ_INDEX, FreeIndex, FALSE);	// Things will update automatically by changing this
}

BOOL CInstrumentEditorS5B::DestroyWindow()
{
	m_pSequenceEditor->DestroyWindow();
	return CDialog::DestroyWindow();
}

void CInstrumentEditorS5B::OnKeyReturn()
{
	// Translate the sequence text string to a sequence
	CString Text;
	GetDlgItemText(IDC_SEQUENCE_STRING, Text);

	switch (m_iSelectedSetting) {
		case SEQ_VOLUME:
			TranslateMML(Text, MAX_VOLUME, 0);
			break;
		case SEQ_ARPEGGIO:
			TranslateMML(Text, 96, -96);
			break;
		case SEQ_PITCH:
			TranslateMML(Text, 126, -127);
			break;
		case SEQ_HIPITCH:
			TranslateMML(Text, 126, -127);
			break;
		case SEQ_DUTYCYCLE:
			TranslateMML(Text, 31, 0);
			break;
	}
}
