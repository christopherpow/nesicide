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
#include "Source/CommentsDlg.h"

// CCommentsDlg dialog

LPCTSTR CCommentsDlg::FONT_FACE = _T("Courier");

IMPLEMENT_DYNAMIC(CCommentsDlg, CDialog)

CCommentsDlg::CCommentsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommentsDlg::IDD, pParent)
{

}

CCommentsDlg::~CCommentsDlg()
{
}

void CCommentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CCommentsDlg, CDialog)
//	ON_BN_CLICKED(IDC_OK, &CCommentsDlg::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_CANCEL, &CCommentsDlg::OnBnClickedCancel)
//	ON_WM_SIZE()
//END_MESSAGE_MAP()

void CCommentsDlg::ok_clicked()
{
   OnBnClickedOk();
}

void CCommentsDlg::cancel_clicked()
{
   OnBnClickedCancel();
}

// CCommentsDlg message handlers

void CCommentsDlg::OnBnClickedOk()
{
	CMainFrame *pMainFrame = (CMainFrame*)GetParentFrame();
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
	CString comment;

	GetDlgItemText(IDC_COMMENTS, comment);

	pDoc->SetComment(comment);

	EndDialog(0);
}

void CCommentsDlg::OnBnClickedCancel()
{
	EndDialog(0);
}

void CCommentsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pEdit = GetDlgItem(IDC_COMMENTS);
	CWnd *pOk = GetDlgItem(IDC_OK);
	CWnd *pCancel = GetDlgItem(IDC_CANCEL);
	CWnd *pCheckBox = GetDlgItem(IDC_SHOWONOPEN);

	CRect dlgRect;
	GetClientRect(dlgRect);

	if (pEdit != NULL) {
		dlgRect.bottom -= 39;
		pEdit->MoveWindow(dlgRect);
		CRect buttonRect;
		pOk->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 8);
		buttonRect.MoveToX(dlgRect.right - buttonRect.Width() * 2 - 10);
		pOk->MoveWindow(buttonRect);
		pCancel->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 8);
		buttonRect.MoveToX(dlgRect.right - buttonRect.Width() - 7);
		pCancel->MoveWindow(buttonRect);
		pCheckBox->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 14);
		buttonRect.MoveToX(8);
		pCheckBox->MoveWindow(buttonRect);
	}
}

BOOL CCommentsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *pMainFrame = (CMainFrame*)GetParentFrame();
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
	CString comment = pDoc->GetComment();

	SetDlgItemText(IDC_COMMENTS, comment);

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_COMMENTS);

	CFont *pFont = new CFont();
	
	pFont->CreateFont(12, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, FONT_FACE);

	pEdit->SetFont(pFont);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
