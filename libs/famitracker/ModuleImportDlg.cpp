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
#include "ModuleImportDlg.h"


// CModuleImportDlg dialog

IMPLEMENT_DYNAMIC(CModuleImportDlg, CDialog)

CModuleImportDlg::CModuleImportDlg(CFamiTrackerDoc *pDoc)
	: CDialog(CModuleImportDlg::IDD, NULL), m_pImportedDoc(NULL), m_pDocument(pDoc)
{
//   IDD_IMPORT DIALOGEX 0, 0, 161, 209
   CRect rect(CPoint(0,0),CSize(161,209));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());

//       GROUPBOX        "Tracks",IDC_STATIC,7,7,147,137
   CGroupBox* mfc3 = new CGroupBox(this);
   mfc3->setTitle("Tracks");
   CRect r3(CPoint(7,7),CSize(147,137));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Options",IDC_STATIC,7,148,147,29
   CGroupBox* mfc6 = new CGroupBox(this);
   mfc6->setTitle("Options");
   CRect r6(CPoint(7,148),CSize(147,29));
   MapDialogRect(&r6);
   mfc6->setGeometry(r6);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,51,188,50,14
   CButton* mfc1 = new CButton(this);
   mfc1->setText("OK");
   mfc1->setDefault(true);
   CRect r1(CPoint(51,188),CSize(50,14));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   mfcToQtWidget.insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),this,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,104,188,50,14
   CButton* mfc2 = new CButton(this);
   mfc2->setText("Cancel");
   CRect r2(CPoint(104,188),CSize(50,14));
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(cancel_clicked()));
//       CONTROL         "Include instruments",IDC_INSTRUMENTS,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,161,116,10
   CCheckBox* mfc4 = new CCheckBox(this);
   mfc4->setText("Include instruments");
   CRect r4(CPoint(14,161),CSize(116,10));
   MapDialogRect(&r4);
   mfc4->setGeometry(r4);
   mfcToQtWidget.insert(IDC_INSTRUMENTS,mfc4);
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,183,147,1
   qDebug("horzline not implemented");
//       CONTROL         "",IDC_TRACKS,"SysListView32",LVS_REPORT | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,14,18,133,120
   CListCtrl* mfc7 = new CListCtrl(this);
   mfc7->setSelectionMode(QAbstractItemView::SingleSelection);
   mfc7->setSelectionBehavior(QAbstractItemView::SelectRows);
//   mfc7->verticalScrollBar()->hide();
//   mfc7->horizontalScrollBar()->hide();
   CRect r7(CPoint(14,18),CSize(133,120));
   MapDialogRect(&r7);
   mfc7->setGeometry(r7);
   mfcToQtWidget.insert(IDC_TRACKS,mfc7);
}

CModuleImportDlg::~CModuleImportDlg()
{
	SAFE_RELEASE(m_pImportedDoc);
}

void CModuleImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CModuleImportDlg, CDialog)
//	ON_BN_CLICKED(IDOK, &CModuleImportDlg::OnBnClickedOk)
//	ON_BN_CLICKED(IDCANCEL, &CModuleImportDlg::OnBnClickedCancel)
//END_MESSAGE_MAP()

void CModuleImportDlg::ok_clicked()
{
   OnBnClickedOk();
}

void CModuleImportDlg::cancel_clicked()
{
   OnBnClickedCancel();
}

// CModuleImportDlg message handlers

BOOL CModuleImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_TRACKS);

	pList->InsertColumn(0, _T("Name"), LVCFMT_LEFT, 190);
   qDebug("CListCtrl::SendMessage not implemented");
//	pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	int TrackCount = m_pImportedDoc->GetTrackCount();
	
	for (int i = 0; i < TrackCount; ++i) {
		CString str;
		str.Format(_T("#%i %s"), i, m_pImportedDoc->GetTrackTitle(i));
		pList->InsertItem(i, str);
		pList->SetCheck(i, TRUE);
	}

	CheckDlgButton(IDC_INSTRUMENTS, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CModuleImportDlg::OnBnClickedOk()
{
	if (!(ImportInstruments() && ImportTracks()))
		AfxMessageBox(IDS_IMPORT_FAILED, MB_ICONERROR);

	OnOK();
}

void CModuleImportDlg::OnBnClickedCancel()
{
	OnCancel();
}

bool CModuleImportDlg::LoadFile(CString Path, CFamiTrackerDoc *pDoc)
{	
	m_pImportedDoc = pDoc->LoadImportFile(Path);

	// Check if load failed
	if (m_pImportedDoc == NULL)
		return false;

	// Check expansion chip match
	if (m_pImportedDoc->GetExpansionChip() != m_pDocument->GetExpansionChip()) {
		AfxMessageBox(IDS_IMPORT_CHIP_MISMATCH);
		SAFE_RELEASE(m_pImportedDoc);
		return false;
	}

	return true;
}

bool CModuleImportDlg::ImportInstruments()
{
	if (IsDlgButtonChecked(IDC_INSTRUMENTS) == BST_CHECKED) {
		// Import instruments
		if (!m_pDocument->ImportInstruments(m_pImportedDoc, m_iInstrumentTable))
			return false;
	}
	else {
		// No instrument translation
		for (int i = 0; i < MAX_INSTRUMENTS; ++i)
			m_iInstrumentTable[i] = i;
	}

	return true;
}

bool CModuleImportDlg::ImportTracks()
{
	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_TRACKS);

	for (unsigned int i = 0; i < m_pImportedDoc->GetTrackCount(); ++i) {
		if (pList->GetCheck(i) == TRUE) {
			// Import track
			if (!m_pDocument->ImportTrack(i, m_pImportedDoc, m_iInstrumentTable))
				return false;
		}
	}

	// Rebuild instrument list
	m_pDocument->SetModifiedFlag();
	m_pDocument->UpdateAllViews(NULL, UPDATE_INSTRUMENTS);
	m_pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);

	return true;
}
