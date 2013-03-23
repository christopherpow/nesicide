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
#include "Source\ModuleImportDlg.h"


// CModuleImportDlg dialog

IMPLEMENT_DYNAMIC(CModuleImportDlg, CDialog)

CModuleImportDlg::CModuleImportDlg(CFamiTrackerDoc *pDoc)
	: CDialog(CModuleImportDlg::IDD, NULL), m_pImportedDoc(NULL), m_pDocument(pDoc)
{
}

CModuleImportDlg::~CModuleImportDlg()
{
	SAFE_RELEASE(m_pImportedDoc);
}

void CModuleImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModuleImportDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CModuleImportDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModuleImportDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CModuleImportDlg message handlers

BOOL CModuleImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListCtrl *pList = (CListCtrl*) GetDlgItem(IDC_TRACKS);

	pList->InsertColumn(0, _T("Name"), LVCFMT_LEFT, 190);
	pList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

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
