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
#include "SpeedDlg.h"

const int RATE_MIN = 25;
const int RATE_MAX = 400;

// CSpeedDlg dialog

IMPLEMENT_DYNAMIC(CSpeedDlg, CDialog)
CSpeedDlg::CSpeedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedDlg::IDD, pParent)
{
//   IDD_SPEED DIALOGEX 0, 0, 196, 44
   CRect rect(CPoint(0,0),CSize(196,44));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
//       DEFPUSHBUTTON   "OK",IDOK,139,7,50,14
   CButton* mfc1 = new CButton(this);
   CRect r1(CPoint(139,7),CSize(50,14));
   MapDialogRect(&r1);
   mfc1->Create(_T("OK"),WS_VISIBLE,r1,this,IDOK);
   mfc1->setDefault(true);
   mfcToQtWidget.insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),this,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,139,23,50,14
   CButton* mfc2 = new CButton(this);
   CRect r2(CPoint(139,23),CSize(50,14));
   MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,this,IDCANCEL);
   mfcToQtWidget.insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(cancel_clicked()));
//       CONTROL         "",IDC_SPEED_SLD,"msctls_trackbar32",WS_TABSTOP,7,7,101,16
   CSliderCtrl* mfc3 = new CSliderCtrl(this);
   CRect r3(CPoint(7,7),CSize(101,16));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_SPEED_SLD,mfc3);
   QObject::connect(mfc3,SIGNAL(valueChanged(int)),this,SLOT(speedSld_valueChanged(int)));
//       LTEXT           "60 Hz",IDC_SPEED,112,11,26,12
   CStatic* mfc4 = new CStatic(this);
   CRect r4(CPoint(112,11),CSize(26,12));
   MapDialogRect(&r4);
   mfc4->Create(_T("60 Hz"),WS_VISIBLE,r4,this,IDC_SPEED);
   mfcToQtWidget.insert(IDC_SPEED,mfc4);
}

CSpeedDlg::~CSpeedDlg()
{
}

void CSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CSpeedDlg, CDialog)
//	ON_WM_HSCROLL()
//	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
//END_MESSAGE_MAP()

void CSpeedDlg::ok_clicked()
{
   OnOK();
}

void CSpeedDlg::cancel_clicked()
{
   OnBnClickedCancel();
}

void CSpeedDlg::speedSld_valueChanged(int value)
{
   OnHScroll(SB_HORZ,value,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_SPEED_SLD)));
}

// CSpeedDlg message handlers


int CSpeedDlg::GetSpeedFromDlg(int InitialSpeed)
{
	m_iSpeed = InitialSpeed;
	CDialog::DoModal();
	return m_iSpeed;
}

void CSpeedDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	m_iSpeed = ((CSliderCtrl*)pScrollBar)->GetPos();
	CString String;
	String.Format(_T("%i Hz"), m_iSpeed );
	SetDlgItemText(IDC_SPEED, String);
}

BOOL CSpeedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl *Slider = (CSliderCtrl*)GetDlgItem(IDC_SPEED_SLD);
	CString String;

	// TODO: Program will crash if speed is set below 25Hz, I don't know why
	Slider->SetRange(RATE_MIN, RATE_MAX);
	Slider->SetPos(m_iSpeed);

	String.Format(_T("%i Hz"), m_iSpeed);
	SetDlgItemText(IDC_SPEED, String);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeedDlg::OnBnClickedCancel()
{
	m_iSpeed = 0;
	OnCancel();
}
