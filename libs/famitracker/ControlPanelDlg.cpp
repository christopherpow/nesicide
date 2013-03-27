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
** Any permitted reproduction of these routin, in whole or in part,
** must bear this legend.
*/


#include "stdafx.h"
#include "resource.h"
#include "ControlPanelDlg.h"

// This class is mainly used to forward messages to the parent window

// CControlPanelDlg dialog

IMPLEMENT_DYNAMIC(CControlPanelDlg, CDialog)

CControlPanelDlg::CControlPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlPanelDlg::IDD, pParent)
{
//   IDD_MAINFRAME DIALOGEX 0, 0, 413, 126
   CRect rect(CPoint(0,0),CSize(413,126));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());

//       GROUPBOX        "Song settings",IDC_STATIC,7,5,78,71
   CGroupBox* mfc1 = new CGroupBox(this);
   CRect r1(CPoint(7,5),CSize(78,71));
   MapDialogRect(&r1);
   mfc1->Create(_T("Song settings"),WS_VISIBLE,r1,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Edit settings",IDC_STATIC,7,78,78,43,0,WS_EX_TRANSPARENT
   CGroupBox* mfc14 = new CGroupBox(this);
   CRect r14(CPoint(7,78),CSize(78,43));
   MapDialogRect(&r14);
   mfc14->Create(_T("Edit settings"),WS_VISIBLE,r14,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Song information",IDC_STATIC,91,5,96,61
   CGroupBox* mfc19 = new CGroupBox(this);
   CRect r19(CPoint(91,5),CSize(96,61));
   MapDialogRect(&r19);
   mfc19->Create(_T("Song information"),WS_VISIBLE,r19,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Songs",IDC_STATIC,91,94,96,27
   CGroupBox* mfc23 = new CGroupBox(this);
   CRect r23(CPoint(91,94),CSize(96,27));
   MapDialogRect(&r23);
   mfc23->Create(_T("Songs"),WS_VISIBLE,r23,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       LTEXT           "Speed",IDC_STATIC,13,17,29,10
   CStatic* mfc2 = new CStatic(this);
   CRect r2(CPoint(13,17),CSize(29,10));
   MapDialogRect(&r2);
   mfc2->Create(_T("Speed"),WS_VISIBLE,r2,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_SPEED,43,15,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER
//       CONTROL         "",IDC_SPEED_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,15,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc3 = new CEdit(this);
   CSpinButtonCtrl* mfc4 = new CSpinButtonCtrl(this);
   CRect r3(CPoint(43,15),CSize(38,12));
   CRect r4(CPoint(r3.right-11,r3.top),CSize(11,12));
   MapDialogRect(&r3);
   MapDialogRect(&r4);
   mfc3->Create(ES_AUTOHSCROLL | ES_READONLY | ES_NUMBER | WS_VISIBLE,r3,this,IDC_SPEED);
   mfc3->setBuddy(mfc4);
   mfcToQtWidget.insert(IDC_SPEED,mfc3);
   QObject::connect(mfc3,SIGNAL(textChanged(QString)),this,SLOT(speed_textChanged(QString)));
   mfc4->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r4,this,IDC_SPEED_SPIN);
   mfc4->setBuddy(mfc3);
   mfcToQtWidget.insert(IDC_SPEED_SPIN,mfc4);
   QObject::connect(mfc4,SIGNAL(valueChanged(int)),this,SLOT(speedSpin_valueChanged(int)));   
//       LTEXT           "Tempo",IDC_STATIC,13,31,29,10
   CStatic* mfc5 = new CStatic(this);
   CRect r5(CPoint(13,31),CSize(29,10));
   MapDialogRect(&r5);
   mfc5->Create(_T("Tempo"),WS_VISIBLE,r5,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_TEMPO,43,29,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_TEMPO_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,28,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc6 = new CEdit(this);
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(this);
   CRect r6(CPoint(43,29),CSize(38,12));
   CRect r7(CPoint(r6.right-11,r6.top),CSize(11,12));
   MapDialogRect(&r6);
   MapDialogRect(&r7);
   mfc6->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r6,this,IDC_TEMPO);
   mfc6->setBuddy(mfc7);
   mfcToQtWidget.insert(IDC_TEMPO,mfc6);
   QObject::connect(mfc6,SIGNAL(textChanged(QString)),this,SLOT(tempo_textChanged(QString)));
   mfc7->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r7,this,IDC_TEMPO_SPIN);
   mfc7->setBuddy(mfc6);
   mfcToQtWidget.insert(IDC_TEMPO_SPIN,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),this,SLOT(tempoSpin_valueChanged(int)));   
//       LTEXT           "Rows",IDC_STATIC,13,45,29,10
   CStatic* mfc8 = new CStatic(this);
   CRect r8(CPoint(13,45),CSize(29,10));
   MapDialogRect(&r8);
   mfc8->Create(_T("Rows"),WS_VISIBLE,r8,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_ROWS,43,43,38,12,ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN
//       CONTROL         "",IDC_ROWS_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,43,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc9 = new CEdit(this);
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(this);
   CRect r9(CPoint(43,43),CSize(38,12));
   CRect r10(CPoint(r9.right-11,r9.top),CSize(11,12));
   MapDialogRect(&r9);
   MapDialogRect(&r10);
   mfc9->Create(ES_AUTOHSCROLL | ES_READONLY | ES_WANTRETURN | WS_VISIBLE,r9,this,IDC_ROWS);
   mfc9->setBuddy(mfc10);
   mfcToQtWidget.insert(IDC_ROWS,mfc9);
   QObject::connect(mfc9,SIGNAL(textChanged(QString)),this,SLOT(rows_textChanged(QString)));
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,this,IDC_ROWS_SPIN);
   mfc10->setBuddy(mfc9);
   mfcToQtWidget.insert(IDC_ROWS_SPIN,mfc10);
   QObject::connect(mfc10,SIGNAL(valueChanged(int)),this,SLOT(rowsSpin_valueChanged(int)));   
//       LTEXT           "Frames",IDC_STATIC,13,59,29,10
   CStatic* mfc11 = new CStatic(this);
   CRect r11(CPoint(13,59),CSize(29,10));
   MapDialogRect(&r11);
   mfc11->Create(_T("Frames"),WS_VISIBLE,r11,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_FRAMES,43,57,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_FRAME_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,58,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc12 = new CEdit(this);
   CSpinButtonCtrl* mfc13 = new CSpinButtonCtrl(this);
   CRect r12(CPoint(43,57),CSize(38,12));
   CRect r13(CPoint(r12.right-11,r12.top),CSize(11,12));
   MapDialogRect(&r12);
   MapDialogRect(&r13);
   mfc12->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r12,this,IDC_FRAMES);
   mfc12->setBuddy(mfc13);
   mfcToQtWidget.insert(IDC_FRAMES,mfc12);
   QObject::connect(mfc12,SIGNAL(textChanged(QString)),this,SLOT(frames_textChanged(QString)));
   mfc13->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r13,this,IDC_FRAME_SPIN);
   mfc13->setBuddy(mfc12);
   mfcToQtWidget.insert(IDC_FRAME_SPIN,mfc13);
   QObject::connect(mfc13,SIGNAL(valueChanged(int)),this,SLOT(framesSpin_valueChanged(int)));   
//       LTEXT           "Step",IDC_STATIC,13,91,29,10,0,WS_EX_TRANSPARENT
   CStatic* mfc15 = new CStatic(this);
   CRect r15(CPoint(13,91),CSize(29,10));
   MapDialogRect(&r15);
   mfc15->Create(_T("Step"),WS_VISIBLE,r15,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       EDITTEXT        IDC_KEYSTEP,43,90,38,12,ES_AUTOHSCROLL | ES_READONLY
//       CONTROL         "",IDC_KEYSTEP_SPIN,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,81,88,11,13
   // CP: Note, we fake a MFC "spin-box" separate control by placing it over it's "buddy" and connecting signals appropriately
   // to mimic the buddy relationship.
   CEdit* mfc16 = new CEdit(this);
   CSpinButtonCtrl* mfc17 = new CSpinButtonCtrl(this);
   CRect r16(CPoint(43,90),CSize(38,12));
   CRect r17(CPoint(r16.right-11,r16.top),CSize(11,12));
   MapDialogRect(&r16);
   MapDialogRect(&r17);
   mfc16->Create(ES_AUTOHSCROLL | ES_READONLY | WS_VISIBLE,r16,this,IDC_KEYSTEP);
   mfc16->setBuddy(mfc17);
   mfcToQtWidget.insert(IDC_KEYSTEP,mfc16);
   QObject::connect(mfc16,SIGNAL(textChanged(QString)),this,SLOT(keyStep_textChanged(QString)));
   mfc17->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r17,this,IDC_KEYSTEP_SPIN);
   mfc17->setBuddy(mfc16);
   mfcToQtWidget.insert(IDC_KEYSTEP_SPIN,mfc17);
   QObject::connect(mfc17,SIGNAL(valueChanged(int)),this,SLOT(keyStepSpin_valueChanged(int)));   
//       CONTROL         "Key repetition",IDC_KEYREPEAT,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,13,105,68,9,WS_EX_TRANSPARENT
   CButton* mfc18 = new CButton(this);
   CRect r18(CPoint(13,105),CSize(68,9));
   MapDialogRect(&r18);
   mfc18->Create(_T("Key repetition"),BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,r18,this,IDC_KEYREPEAT);
   mfcToQtWidget.insert(IDC_KEYREPEAT,mfc18);
   QObject::connect(mfc18,SIGNAL(clicked()),this,SLOT(keyRepeat_clicked()));
//       EDITTEXT        IDC_SONG_NAME,97,18,83,12,ES_AUTOHSCROLL
   CEdit* mfc20 = new CEdit(this);
   CRect r20(CPoint(97,18),CSize(83,12));
   MapDialogRect(&r20);
   mfc20->Create(ES_AUTOHSCROLL | WS_VISIBLE,r20,this,IDC_SONG_NAME);
   mfcToQtWidget.insert(IDC_SONG_NAME,mfc20);
   QObject::connect(mfc20,SIGNAL(textChanged(QString)),this,SLOT(songName_textChanged(QString)));
//       EDITTEXT        IDC_SONG_ARTIST,97,33,83,12,ES_AUTOHSCROLL
   CEdit* mfc21 = new CEdit(this);
   CRect r21(CPoint(97,33),CSize(83,12));
   MapDialogRect(&r21);
   mfc21->Create(ES_AUTOHSCROLL | WS_VISIBLE,r21,this,IDC_SONG_ARTIST);
   mfcToQtWidget.insert(IDC_SONG_ARTIST,mfc21);
   QObject::connect(mfc21,SIGNAL(textChanged(QString)),this,SLOT(songArtist_textChanged(QString)));
//       EDITTEXT        IDC_SONG_COPYRIGHT,97,49,83,12,ES_AUTOHSCROLL
   CEdit* mfc22 = new CEdit(this);
   CRect r22(CPoint(97,49),CSize(83,12));
   MapDialogRect(&r22);
   mfc22->Create(ES_AUTOHSCROLL | WS_VISIBLE,r22,this,IDC_SONG_COPYRIGHT);
   mfcToQtWidget.insert(IDC_SONG_COPYRIGHT,mfc22);
   QObject::connect(mfc22,SIGNAL(textChanged(QString)),this,SLOT(songCopyright_textChanged(QString)));
//       COMBOBOX        IDC_SUBTUNE,97,104,84,85,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc24 = new CComboBox(this);
   CRect r24(CPoint(97,104),CSize(84,85));
   MapDialogRect(&r24);
   mfc24->Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,r24,this,IDC_SUBTUNE);
   mfcToQtWidget.insert(IDC_SUBTUNE,mfc24);
   QObject::connect(mfc24,SIGNAL(currentIndexChanged(int)),this,SLOT(subtune_currentIndexChanged(int)));
//       CONTROL         "",IDC_INSTRUMENTS,"SysListView32",LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP,193,5,202,103
   CListCtrl* mfc25 = new CListCtrl(this);
   CRect r25(CPoint(193,5),CSize(202,103));
   MapDialogRect(&r25);
   mfc25->Create(LVS_LIST | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r25,this,IDC_INSTRUMENTS);
   mfcToQtWidget.insert(IDC_INSTRUMENTS,mfc25);
   QObject::connect(mfc25,SIGNAL(cellClicked(int,int)),this,SLOT(instruments_cellClicked(int,int)));
   QObject::connect(mfc25,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(instruments_cellDoubleClicked(int,int)));
//       EDITTEXT        IDC_INSTNAME,324,109,71,12,ES_AUTOHSCROLL
   CEdit* mfc26 = new CEdit(this);
   CRect r26(CPoint(324,109),CSize(71,12));
   MapDialogRect(&r26);
   mfc26->Create(ES_AUTOHSCROLL | WS_VISIBLE,r26,this,IDC_INSTNAME);
   mfcToQtWidget.insert(IDC_INSTNAME,mfc26);
   QObject::connect(mfc26,SIGNAL(textChanged(QString)),this,SLOT(instName_textChanged(QString)));
}

CControlPanelDlg::~CControlPanelDlg()
{
}

void CControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CControlPanelDlg, CDialog)
//END_MESSAGE_MAP()


// CControlPanelDlg message handlers

BOOL CControlPanelDlg::PreTranslateMessage(MSG* pMsg)
{
	UpdateDialogControls(this, TRUE);
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CControlPanelDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Route command messages to main frame
//	if (nID != IDC_KEYSTEP_SPIN) {
		if (m_pMainFrame->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
//		if (GetParent()->GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
//			return TRUE;
//	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CControlPanelDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// Route notify messages to main frame
	//*pResult = GetParent()->GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CControlPanelDlg::SetFrameParent(CWnd *pMainFrm)
{
	m_pMainFrame = pMainFrm;
}
