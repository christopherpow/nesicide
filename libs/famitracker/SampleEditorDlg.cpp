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
#include "SoundGen.h"
#include "SampleEditorDlg.h"

//
// The DPCM sample editor
//

// CSampleEditorDlg dialog

IMPLEMENT_DYNAMIC(CSampleEditorDlg, CDialog)

CSampleEditorDlg::CSampleEditorDlg(CWnd* pParent /*=NULL*/, CDSample *pSample)
	: CDialog(CSampleEditorDlg::IDD, pParent), m_pSampleView(NULL)
{
	// Create a copy of the sample
	m_pSample = new CDSample(*pSample);
	m_pOriginalSample = pSample;
	m_pSoundGen = theApp.GetSoundGenerator();
   
//   IDD_SAMPLE_EDITOR DIALOGEX 0, 0, 481, 255
   CRect rect(CPoint(0,0),CSize(481,255));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
//   DEFPUSHBUTTON   "OK",IDOK,372,234,50,14
   CButton* mfc1 = new CButton(this);
   mfc1->setText("OK");
   mfc1->setDefault(true);
   CRect r1(CPoint(372,234),CSize(50,14));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   mfcToQtWidget.insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),this,SLOT(ok_clicked()));
//   PUSHBUTTON      "Cancel",IDCANCEL,424,234,50,14
   CButton* mfc2 = new CButton(this);
   mfc2->setText("Cancel");
   CRect r2(CPoint(424,234),CSize(50,14));
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(cancel_clicked()));
//   CONTROL         "",IDC_SAMPLE,"Static",SS_WHITERECT | SS_NOTIFY | SS_SUNKEN,7,7,467,204
   CStatic *mfc3 = new CStatic(this);
   CRect r3(CPoint(7,7),CSize(467,204));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_SAMPLE,mfc3);
//   PUSHBUTTON      "Play",IDC_PLAY,67,234,50,14
   CButton* mfc4 = new CButton(this);
   mfc4->setText("Play");
   CRect r4(CPoint(67,234),CSize(50,14));
   MapDialogRect(&r4);
   mfc4->setGeometry(r4);
   mfcToQtWidget.insert(IDC_PLAY,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(play_clicked()));
//   LTEXT           "Offset: 0, Pos: 0",IDC_POS,7,217,79,11,SS_SUNKEN
   CStatic* mfc5 = new CStatic(this);
   mfc5->setText("Offset: 0, Pos: 0");
   CRect r5(CPoint(7,217),CSize(79,11));
   MapDialogRect(&r5);
   mfc5->setGeometry(r5);
   mfcToQtWidget.insert(IDC_POS,mfc5);
//   PUSHBUTTON      "Delete",IDC_DELETE,239,234,50,14
   CButton* mfc6 = new CButton(this);
   mfc6->setText("Delete");
   CRect r6(CPoint(239,234),CSize(50,14));
   MapDialogRect(&r6);
   mfc6->setGeometry(r6);
   mfcToQtWidget.insert(IDC_DELETE,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),this,SLOT(delete_clicked()));
//   CONTROL         "",IDC_PITCH,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,143,236,79,12
   CSliderCtrl* mfc7 = new CSliderCtrl(this);
   CRect r7(CPoint(143,236),CSize(79,12));
   MapDialogRect(&r7);
   mfc7->setGeometry(r7);
   mfcToQtWidget.insert(IDC_PITCH,mfc7);
   QObject::connect(mfc7,SIGNAL(valueChanged(int)),this,SLOT(pitch_valueChanged(int)));
//   PUSHBUTTON      "Tilt",IDC_TILT,292,234,50,14
   CButton* mfc8 = new CButton(this);
   mfc8->setText("Tilt");
   CRect r8(CPoint(292,234),CSize(50,14));
   MapDialogRect(&r8);
   mfc8->setGeometry(r8);
   mfcToQtWidget.insert(IDC_TILT,mfc8);
   QObject::connect(mfc8,SIGNAL(clicked()),this,SLOT(tilt_clicked()));
//   LTEXT           "0 bytes",IDC_INFO,88,217,87,11,SS_SUNKEN
   CStatic* mfc9 = new CStatic(this);
   mfc9->setText("0 bytes");
   CRect r9(CPoint(88,217),CSize(87,11));
   MapDialogRect(&r9);
   mfc9->setGeometry(r9);
   mfcToQtWidget.insert(IDC_INFO,mfc9);
//   LTEXT           "Pitch",IDC_STATIC_PITCH,123,237,19,11
   CStatic* mfc10 = new CStatic(this);
   mfc10->setText("Pitch");
   CRect r10(CPoint(123,237),CSize(19,11));
   MapDialogRect(&r10);
   mfc10->setGeometry(r10);
   mfcToQtWidget.insert(IDC_STATIC_PITCH,mfc10);
//   CONTROL         "Start from 64",IDC_DELTASTART,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,238,57,10
   CCheckBox* mfc11 = new CCheckBox(this);
   mfc11->setText("Start from 64");
   CRect r11(CPoint(7,238),CSize(57,10));
   MapDialogRect(&r11);
   mfc11->setGeometry(r11);
   mfcToQtWidget.insert(IDC_DELTASTART,mfc11);
   QObject::connect(mfc11,SIGNAL(clicked()),this,SLOT(deltaStart_clicked()));
}

CSampleEditorDlg::~CSampleEditorDlg()
{
	SAFE_RELEASE(m_pSampleView);
	SAFE_RELEASE(m_pSample);
}

void CSampleEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//BEGIN_MESSAGE_MAP(CSampleEditorDlg, CDialog)
//	ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_PLAY, &CSampleEditorDlg::OnBnClickedPlay)
//	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_DELETE, &CSampleEditorDlg::OnBnClickedDelete)
//	ON_BN_CLICKED(IDC_DELTASTART, &CSampleEditorDlg::OnBnClickedDeltastart)
//	ON_WM_KEYDOWN()
//	ON_BN_CLICKED(IDC_TILT, &CSampleEditorDlg::OnBnClickedTilt)
//END_MESSAGE_MAP()


// CSampleEditorDlg message handlers

BOOL CSampleEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

   m_pSampleView = new CSampleView();
	m_pSampleView->SubclassDlgItem(IDC_SAMPLE, this);
	m_pSampleView->CalculateSample(m_pSample, IsDlgButtonChecked(IDC_DELTASTART) ? 64 : 0);
	m_pSampleView->UpdateInfo();

	CSliderCtrl *pitch = (CSliderCtrl*)GetDlgItem(IDC_PITCH);
	pitch->SetRange(0, 15);
	pitch->SetPos(15);

	MoveControls();

	// A timer for the flashing start cursor
	SetTimer(1, 500, NULL);

	CString title;
	GetWindowText(title);
	title.AppendFormat(_T(" [%s]"), m_pSample->Name);
	SetWindowText(title);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSampleEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	MoveControls();
}

void CSampleEditorDlg::MoveControls()
{
	CRect rect;
	GetClientRect(&rect);

	if (m_pSampleView) {
		rect.top++;
		rect.left++;
		rect.bottom -= 80;
		rect.right -= 2;
		m_pSampleView->MoveWindow(rect);
		m_pSampleView->Invalidate();
	}

	CWnd *control;
	CRect controlRect;

	GetClientRect(&rect);

	if (control = GetDlgItem(IDC_PLAY)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(10, rect.bottom - 30);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_POS)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(5, rect.bottom - 75);
		controlRect.InflateRect(0, 0, 2, 2);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_INFO)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(130, rect.bottom - 75);
		controlRect.InflateRect(0, 0, 2, 2);
		controlRect.right = rect.right - 5;
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_STATIC_PITCH)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(93, rect.bottom - 27);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_PITCH)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(120, rect.bottom - 30);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_DELTASTART)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(10, rect.bottom - 50);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_DELETE)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(250, rect.bottom - 30);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDC_TILT)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(330, rect.bottom - 30);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDCANCEL)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(rect.right - controlRect.right - 10, rect.bottom - 30);
		control->MoveWindow(controlRect);
	}

	if (control = GetDlgItem(IDOK)) {
		control->GetClientRect(&controlRect);
		controlRect.MoveToXY(rect.right - controlRect.right - 10, rect.bottom - 55);
		control->MoveWindow(controlRect);
	}

	Invalidate();
	RedrawWindow();
}

void CSampleEditorDlg::OnBnClickedPlay()
{
	if (m_pSample->SampleSize == 0)
		return;

	int Pitch = ((CSliderCtrl*)GetDlgItem(IDC_PITCH))->GetPos();
	m_pSoundGen->WriteAPU(0x4011, IsDlgButtonChecked(IDC_DELTASTART) ? 64 : 0);
	m_pSoundGen->PreviewSample(m_pSample, m_pSampleView->GetStartOffset(), Pitch);
	// Wait for sample to play (at most 400ms)
	DWORD time = GetTickCount() + 400;
	while (m_pSoundGen->PreviewDone() == true || GetTickCount() > time);
	// Start play cursor timer
	SetTimer(0, 10, NULL);
}

void CSampleEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Update play cursor

	switch (nIDEvent) {
		case 0: {
			// Play cursor
			stDPCMState state = m_pSoundGen->GetDPCMState();

			// Pos is in bytes
			int Pos = state.SamplePos /*<< 6*/;

			if (m_pSoundGen->PreviewDone()) {
				KillTimer(0);
				Pos = -1;
			}

			m_pSampleView->DrawPlayCursor(Pos);
		}
		case 1: {
			// Start cursor
			if (m_pSoundGen->PreviewDone()) {
				static bool bDraw = false;
				if (!bDraw)
					m_pSampleView->DrawStartCursor();
				else
					m_pSampleView->DrawPlayCursor(-1);
				bDraw = !bDraw;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CSampleEditorDlg::OnBnClickedDelete()
{
	int SelStart = m_pSampleView->GetSelStart();
	int SelEnd = m_pSampleView->GetSelEnd();

	if (SelStart == SelEnd)
		return;

	int StartSample = m_pSampleView->GetBlock(SelStart + 1) * 16;
	int EndSample = m_pSampleView->GetBlock(SelEnd + 1) * 16;
	int Diff = EndSample - StartSample;

	for (int i = 0; i < Diff; ++i) {
		if ((EndSample + i) < (int)m_pSample->SampleSize)
			m_pSample->SampleData[StartSample + i] = m_pSample->SampleData[EndSample + i];
	}

	m_pSample->SampleSize -= Diff;

	// Reallocate
	// TODO: fix this, it crashes
	/*
	char *pData = new char[m_pSample->SampleSize];
	memcpy(pData, m_pSample->SampleData, m_pSample->SampleSize);
	delete [] m_pSample->SampleData;
	m_pSample->SampleData = pData;
	*/

	UpdateSampleView();
}

void CSampleEditorDlg::OnBnClickedDeltastart()
{
	UpdateSampleView();
}

void CSampleEditorDlg::UpdateSampleView()
{
	m_pSampleView->CalculateSample(m_pSample, IsDlgButtonChecked(IDC_DELTASTART) ? 64 : 0);
	m_pSampleView->UpdateInfo();
	m_pSampleView->Invalidate();
	m_pSampleView->RedrawWindow();
}

void CSampleEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: this never gets called
	if (nChar == VK_DELETE) {
		OnBnClickedDelete();
	}
	if (nChar == VK_HOME) {
		m_pSampleView->OnHome();
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


// CSampleView control

IMPLEMENT_DYNAMIC(CSampleView, CStatic)

//BEGIN_MESSAGE_MAP(CSampleView, CStatic)
//	ON_WM_PAINT()
//	ON_WM_ERASEBKGND()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_SIZE()
//END_MESSAGE_MAP()

CScrollBar* m_sbScrollBar;

CSampleView::CSampleView() : 
	m_iSelStart(-1), 
	m_iSelEnd(-1), 
	m_iStartCursor(0),
	m_pSamples(NULL),
	m_bClicked(false)
{
   m_pSolidPen = new CPen(PS_SOLID, 1, (COLORREF)0);
	m_pDashedPen = new CPen(PS_DASH, 1, (COLORREF)0x00);
	m_pGrayDashedPen = new CPen(PS_DASHDOT, 1, (COLORREF)0xF0F0F0);
	m_pDarkGrayDashedPen = new CPen(PS_DASHDOT, 1, (COLORREF)0xE0E0E0);
   
   _qt->installEventFilter(this);
   
   m_sbScrollBar = new CScrollBar;
}

CSampleView::~CSampleView()
{
	SAFE_RELEASE_ARRAY(m_pSamples);

	SAFE_RELEASE(m_pSolidPen);
	SAFE_RELEASE(m_pDashedPen);
	SAFE_RELEASE(m_pGrayDashedPen);
	SAFE_RELEASE(m_pDarkGrayDashedPen);
}

bool CSampleView::eventFilter(QObject *object, QEvent *event)
{
   if ( event->type() == QEvent::Show )
   {
      showEvent(dynamic_cast<QShowEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::ShowToParent )
   {
      showEvent(dynamic_cast<QShowEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Hide )
   {
      hideEvent(dynamic_cast<QHideEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Move )
   {
      moveEvent(dynamic_cast<QMoveEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Paint )
   {
      paintEvent(dynamic_cast<QPaintEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::FocusIn )
   {
      focusInEvent(dynamic_cast<QFocusEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::FocusOut )
   {
      focusOutEvent(dynamic_cast<QFocusEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonPress )
   {
      mousePressEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonRelease )
   {
      mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseButtonDblClick )
   {
      mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::MouseMove )
   {
      mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Wheel )
   {
      wheelEvent(dynamic_cast<QWheelEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::Resize )
   {
      resizeEvent(dynamic_cast<QResizeEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::KeyPress )
   {
      keyPressEvent(dynamic_cast<QKeyEvent*>(event));
      return true;
   }
   if ( event->type() == QEvent::KeyRelease )
   {
      keyReleaseEvent(dynamic_cast<QKeyEvent*>(event));
      return true;
   }
   qDebug("eventFilter: unhandled %d object %s", event->type(), object->objectName().toAscii().constData());
   return false;
}

void CSampleView::OnPaint()
{
	int ScrollBarHeight = 0;

	CPaintDC dc(this); // device context for painting

   // Create scroll bar
	if (m_sbScrollBar->m_hWnd == NULL) {
		CRect rect;
		GetClientRect(&rect);
		m_sbScrollBar->Create(SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rect, this, 1);
		m_sbScrollBar->EnableWindow(0);
	}

	CRect sbRect;
	m_sbScrollBar->GetClientRect(&sbRect);
	ScrollBarHeight = sbRect.bottom - sbRect.top;

	GetClientRect(&m_clientRect);

	m_clientRect.bottom -= ScrollBarHeight;

	int MaxY = m_clientRect.bottom - 2;
	int MaxX = m_clientRect.right - 2;
	int x, y;

   qDebug("DeleteDC");
//	if (m_dcCopy.m_hDC != NULL)
//		m_dcCopy.DeleteDC();

   qDebug("DeleteObject");
//	if (m_bmpCopy.m_hObject != NULL)
//		m_bmpCopy.DeleteObject();

	m_bmpCopy.CreateCompatibleBitmap(&dc, m_clientRect.Width(), m_clientRect.Height());
	m_dcCopy.CreateCompatibleDC(&dc);
	m_dcCopy.SelectObject(&m_bmpCopy);
	m_dcCopy.FillSolidRect(m_clientRect, 0xFFFFFF);
	m_dcCopy.SetViewportOrg(1, 1);

	if (m_iSize == 0) {
		m_dcCopy.TextOut(10, 10, CString(_T("No sample")));
		dc.BitBlt(0, 0, m_clientRect.Width(), m_clientRect.Height(), &m_dcCopy, 0, 0, SRCCOPY);
		return;
	}

	double Step = double(m_iSize) / double(MaxX);	// Samples / pixel

	m_dSampleStep = Step;
	m_iBlockSize = (MaxX * (8 * 16)) / m_iSize;

	CPen *oldPen = m_dcCopy.SelectObject(m_pSolidPen);

	// Block markers
	m_dcCopy.SelectObject(m_pGrayDashedPen);
	m_dcCopy.SetBkMode(TRANSPARENT);
	int Blocks = (m_iSize / (8 * 16));
	if (Blocks < (MaxX / 2)) {
		for (int i = 1; i < Blocks; ++i) {
			x = int((i * 128) / m_dSampleStep) - 1;
			if (i % 4 == 0)
				m_dcCopy.SelectObject(m_pDarkGrayDashedPen);
			else
				m_dcCopy.SelectObject(m_pGrayDashedPen);
			m_dcCopy.MoveTo(x, 0);
			m_dcCopy.LineTo(x, MaxY);
		}
	}

	// Selection, each step is 16 bytes, or 128 samples
	if (m_iSelStart != m_iSelEnd) {
		m_dcCopy.FillSolidRect(m_iSelStart, 0, m_iSelEnd - m_iSelStart, MaxY, 0xFF80A0);
//		copy.FillSolidRect(m_iSelStart, 0, 1, MaxY, 0xFF6080);
//		copy.FillSolidRect(m_iSelEnd, 0, 1, MaxY, 0xFF6080);
	}

	// Draw the sample
	y = (m_pSamples[0] * MaxY) / 127;
	m_dcCopy.MoveTo(0, y);
	m_dcCopy.SelectObject(m_pSolidPen);

	for (x = 0; x < MaxX; ++x) {
		y = (m_pSamples[(x * m_iSize) / MaxX] * MaxY) / 127;
		m_dcCopy.LineTo(x, y);
	}

	m_dcCopy.SetViewportOrg(0, 0);
	m_dcCopy.SelectObject(oldPen);

	dc.BitBlt(0, 0, m_clientRect.Width(), m_clientRect.Height(), &m_dcCopy, 0, 0, SRCCOPY);
}

BOOL CSampleView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CSampleView::OnMouseMove(UINT nFlags, CPoint point)
{
	double Sample = double(point.x) * m_dSampleStep;
	int Offset = int(Sample / (8.0 * 64.0));
	int Pos = int(Sample / (8.0 * 16.0));

	if (!m_iSize)
		return;

	if (point.y > m_clientRect.bottom)
		return;

	if (nFlags & MK_LBUTTON && m_iSelStart != -1) {
		//m_iSelEnd = int((point.x + m_iBlockSize / 2) / m_iBlockSize) * m_iBlockSize;
		int Block = GetBlock(point.x + m_iBlockSize / 2);
		m_iSelEnd = GetPixel(Block);
		Invalidate();
		RedrawWindow();
	}

	CString Text;
	Text.Format(_T("Offset: %i, Pos: %i"), Offset, Pos);
	GetParent()->SetDlgItemText(IDC_POS, Text);

	CStatic::OnMouseMove(nFlags, point);
}

void CSampleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_iSize)
		return;

	if (point.y > m_clientRect.bottom)
		return;

	int Block = GetBlock(point.x);
	m_iSelStart = GetPixel(Block);
	m_iSelEnd = m_iSelStart;
	Invalidate();
	RedrawWindow();
	m_bClicked = true;
	CStatic::OnLButtonDown(nFlags, point);
}

void CSampleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Sets the start cursor

	if (!m_iSize)
		return;

	if (!m_bClicked)
		return;

	m_bClicked = false;

	double Sample = double(point.x) * m_dSampleStep;
	int Offset = int(Sample / (8.0 * 64.0));

	if (m_iSelEnd == m_iSelStart) {
		m_iStartCursor = Offset;
		m_iSelStart = m_iSelEnd = -1;
		DrawStartCursor();
	}
	else {
		if (m_iSelEnd < m_iSelStart) {
			int Temp = m_iSelEnd;
			m_iSelEnd = m_iSelStart;
			m_iSelStart = Temp;
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void CSampleView::DrawPlayCursor(int Pos)
{
	CDC *pDC = GetDC();
	int x = int(((double(Pos + m_iStartCursor) * 64.0) * 8.0) / m_dSampleStep);

	pDC->BitBlt(0, 0, m_clientRect.Width(), m_clientRect.Height(), &m_dcCopy, 0, 0, SRCCOPY);

	if (Pos != -1) {
		CPen *oldPen = pDC->SelectObject(m_pDashedPen);
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, m_clientRect.bottom);
		pDC->SelectObject(oldPen);
	}

	ReleaseDC(pDC);
}

void CSampleView::DrawStartCursor()
{
	CDC *pDC = GetDC();
	int x = int((double(m_iStartCursor) * 8.0 * 64.0) / m_dSampleStep);

	pDC->BitBlt(0, 0, m_clientRect.Width(), m_clientRect.Height(), &m_dcCopy, 0, 0, SRCCOPY);

	if (m_iStartCursor != -1) {
		CPen *oldPen = pDC->SelectObject(m_pDashedPen);
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, m_clientRect.bottom);
		pDC->SelectObject(oldPen);
	}

	ReleaseDC(pDC);
}

void CSampleView::CalculateSample(CDSample *pSample, int Start)
{
	int Samples = pSample->SampleSize;
	int Size = Samples * 8;

	SAFE_RELEASE_ARRAY(m_pSamples);

	if (pSample->SampleSize == 0) {
		m_iSize = 0;
		m_iStartCursor = 0;
		m_iSelStart = m_iSelEnd = -1;
		return;
	}

	m_pSamples = new int[Size];
	m_iSize = Size;

	int cntr = Start;

	for (int i = 0; i < Size; ++i) {
		if (pSample->SampleData[i / 8] & (1 << (i % 8))) {
			if (cntr < 126)
				cntr += 2;
		}
		else {
			if (cntr > 1)
				cntr -= 2;
		}
		m_pSamples[i] = cntr;
	}	

	m_iSelStart = m_iSelEnd = -1;
	m_iStartCursor = 0;
}

int CSampleView::GetBlock(int Pixel) const
{
	double Sample = double(Pixel) * m_dSampleStep;
	int Pos = int(Sample / (8.0 * 16.0));
	return Pos;
}

int CSampleView::GetPixel(int Block) const
{
	return int((double(Block) * 128.0) / m_dSampleStep);
}

void CSampleView::UpdateInfo()
{
	if (!m_iSize)
		return;
	CString Text;
	Text.Format(_T("Delta end pos: %i, Size: %i bytes"), m_pSamples[m_iSize - 1], m_iSize / 8);
	GetParent()->SetDlgItemText(IDC_INFO, Text);
}

void CSampleEditorDlg::OnBnClickedTilt()
{
	int SelStart = m_pSampleView->GetSelStart();
	int SelEnd = m_pSampleView->GetSelEnd();

	if (SelStart == SelEnd)
		return;

	int StartSample = m_pSampleView->GetBlock(SelStart + 1) * 16;
	int EndSample = m_pSampleView->GetBlock(SelEnd + 1) * 16;
	int Diff = EndSample - StartSample;

	int Nr = 10;
	int Step = (Diff * 8) / Nr;
	int Cntr = rand() % Step;

	for (int i = StartSample; i < EndSample; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (++Cntr == Step) {
				m_pSample->SampleData[i] &= (0xFF ^ (1 << j));
				Cntr = 0;
			}
		}
	}

	UpdateSampleView();
}

void CSampleView::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (m_sbScrollBar->m_hWnd != NULL) {
		CRect clientRect, scrollRect;
		GetClientRect(&clientRect);
		m_sbScrollBar->GetClientRect(&scrollRect);
		scrollRect.right = clientRect.right;
		int height = scrollRect.Height();
		scrollRect.top = clientRect.bottom - height;
		scrollRect.bottom = scrollRect.top + height;
		m_sbScrollBar->MoveWindow(&scrollRect);
	}
}

void CSampleView::OnHome()
{
	m_iStartCursor = 0;
}

void CSampleEditorDlg::CopySample(CDSample *pTarget)
{
	pTarget->Allocate(m_pSample->SampleSize, m_pSample->SampleData);
}

void CSampleView::paintEvent(QPaintEvent *)
{
   OnPaint();
}

void CSampleView::mouseMoveEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   OnMouseMove(flags,point);
   update();
}

void CSampleView::mousePressEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      OnLButtonDown(flags,point);
   }
   update();
}

void CSampleView::mouseReleaseEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      OnLButtonUp(flags,point);
   }
   update();
}

void CSampleView::resizeEvent(QResizeEvent *event)
{
   OnSize(0,event->size().width(),event->size().height());
}

void CSampleEditorDlg::timerEvent(QTimerEvent *event)
{
   int mfcId = mfcTimerId(event->timerId());
   OnTimer(mfcId);
}

void CSampleEditorDlg::ok_clicked()
{
   OnOK();
}

void CSampleEditorDlg::cancel_clicked()
{
   OnCancel();
}

void CSampleEditorDlg::play_clicked()
{
   OnBnClickedPlay();
}

void CSampleEditorDlg::delete_clicked()
{
   OnBnClickedDelete();
}

void CSampleEditorDlg::deltaStart_clicked()
{
   OnBnClickedDeltastart();
}

void CSampleEditorDlg::tilt_clicked()
{
   OnBnClickedTilt();
}

void CSampleEditorDlg::resizeEvent(QResizeEvent *event)
{
   OnSize(0,event->size().width(),event->size().height());
}
