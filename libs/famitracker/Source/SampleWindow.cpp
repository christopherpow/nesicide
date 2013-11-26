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
#include "SampleWindow.h"
#include "FFT/Fft.h"
#include "resource.h"
#include "Settings.h"
#include "SWSampleScope.h"
#include "SWSpectrum.h"
#include "SWLogo.h"

// Thread entry helper

static UINT ThreadProcFunc(LPVOID pParam)
{
	CSampleWindow *pObj = (CSampleWindow*)pParam;

	if (pObj == NULL || !pObj->IsKindOf(RUNTIME_CLASS(CSampleWindow)))
		return 1;

	return pObj->ThreadProc();
}

// CSampleWindow

IMPLEMENT_DYNAMIC(CSampleWindow, CWnd)

CSampleWindow::CSampleWindow() :
	m_iCurrentState(0), 
	m_bThreadRunning(false), 
	m_iBufferSize(0),
	m_pBuffer1(NULL),
	m_pBuffer2(NULL)
{
}

CSampleWindow::~CSampleWindow()
{
	for (int i = 0; i < 4; ++i) {
		SAFE_RELEASE(m_pStates[i]);
	}

	SAFE_RELEASE_ARRAY(m_pBuffer1);
	SAFE_RELEASE_ARRAY(m_pBuffer2);
}

BEGIN_MESSAGE_MAP(CSampleWindow, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CSampleWindow::paintEvent(QPaintEvent *)
{
// CP: OnPaint passes true insteaad of false, since the painting
// is apparently done outside of OnPaint.  We'll cheat.
   CDC dc(this);
   m_pStates[m_iCurrentState]->Draw(&dc, false);
}

void CSampleWindow::mousePressEvent(QMouseEvent *event)
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
}

void CSampleWindow::mouseReleaseEvent(QMouseEvent *event)
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
   if ( event->button() == Qt::RightButton )
   {
      OnRButtonUp(flags,point);
   }
}

void CSampleWindow::mouseDoubleClickEvent(QMouseEvent *event)
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
      OnLButtonDblClk(flags,point);
   }
}

// State methods

void CSampleWindow::NextState()
{
	m_csBuffer.Lock();
	m_iCurrentState = (m_iCurrentState + 1) % STATE_COUNT;
	m_csBuffer.Unlock();

	Invalidate();

	theApp.GetSettings()->SampleWinState = m_iCurrentState;
}

// CSampleWindow message handlers

void CSampleWindow::SetSampleRate(int SampleRate)
{
	if (m_hWnd) {
		m_pStates[m_iCurrentState]->SetSampleRate(SampleRate);
	}
}

void CSampleWindow::FlushSamples(int *pSamples, int Count)
{
	if (!m_bThreadRunning)
		return;

	if (Count != m_iBufferSize) {
		m_csBuffer.Lock();
		SAFE_RELEASE_ARRAY(m_pBuffer1);
		SAFE_RELEASE_ARRAY(m_pBuffer2);
		m_pBuffer1 = new int[Count];
		m_pBuffer2 = new int[Count];
		m_iBufferSize = Count;
		m_pFillBuffer = m_pBuffer1;
		m_csBuffer.Unlock();
	}

	m_csBufferSelect.Lock();
	memcpy(m_pFillBuffer, pSamples, sizeof(int) * Count);
	m_csBufferSelect.Unlock();

	SetEvent(m_hNewSamples);
}

UINT CSampleWindow::ThreadProc()
{
	m_bThreadRunning = true;

	TRACE0("Visualizer: Started thread\n");

	while (WaitForSingleObject(m_hNewSamples, INFINITE) == WAIT_OBJECT_0 && m_bThreadRunning) {
		
		// Switch buffers
		m_csBufferSelect.Lock();

		int *pDrawBuffer = m_pFillBuffer;

		if (m_pFillBuffer == m_pBuffer1)
			m_pFillBuffer = m_pBuffer2;
		else
			m_pFillBuffer = m_pBuffer1;

		m_csBufferSelect.Unlock();

		// Draw
		m_csBuffer.Lock();

		CDC *pDC = GetDC();

		m_pStates[m_iCurrentState]->SetSampleData(pDrawBuffer, m_iBufferSize);
		m_pStates[m_iCurrentState]->Draw(pDC, false);

		ReleaseDC(pDC);

		m_csBuffer.Unlock();
	}

	CloseHandle(m_hNewSamples);

	TRACE0("Visualizer: Closed thread\n");

	return 0;
}

BOOL CSampleWindow::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	m_pStates[0] = new CSWSampleScope(false);
	m_pStates[1] = new CSWSampleScope(true);
	m_pStates[2] = new CSWSpectrum();
	m_pStates[3] = new CSWLogo();
	
	// This is saved
	m_iCurrentState = theApp.GetSettings()->SampleWinState;

	for (int i = 0; i < STATE_COUNT; ++i) {
		m_pStates[i]->Activate();
	}

	// Create an event used to signal that new samples are available
	m_hNewSamples = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create a worker thread
	AfxBeginThread(&ThreadProcFunc, (LPVOID)this);

	return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CSampleWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	NextState();
	CWnd::OnLButtonDown(nFlags, point);
}

void CSampleWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	NextState();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CSampleWindow::OnPaint()
{
	m_csBuffer.Lock();

	CPaintDC dc(this); // device context for painting
	m_pStates[m_iCurrentState]->Draw(&dc, true);

	m_csBuffer.Unlock();
}

void CSampleWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMenu PopupMenuBar;
	PopupMenuBar.LoadMenu(IDR_SAMPLE_WND_POPUP);

	CMenu *pPopupMenu = PopupMenuBar.GetSubMenu(0);

	CPoint menuPoint;
	CRect rect;

	GetWindowRect(rect);

	menuPoint.x = rect.left + point.x;
	menuPoint.y = rect.top + point.y;

	UINT menuIds[] = {ID_POPUP_SAMPLEGRAPH1, ID_POPUP_SAMPLEGRAPH2, ID_POPUP_SPECTRUMANALYZER, ID_POPUP_NOTHING};

	pPopupMenu->CheckMenuItem(menuIds[m_iCurrentState], MF_BYCOMMAND | MF_CHECKED);

	UINT Result = pPopupMenu->TrackPopupMenu(TPM_RETURNCMD, menuPoint.x, menuPoint.y, this);

	m_csBuffer.Lock();

	switch (Result) {
		case ID_POPUP_SAMPLESCOPE1:
			m_iCurrentState = 0;
			break;
		case ID_POPUP_SAMPLESCOPE2:
			m_iCurrentState = 1;
			break;
		case ID_POPUP_SPECTRUMANALYZER:
			m_iCurrentState = 2;
			break;
		case ID_POPUP_NOTHING:
			m_iCurrentState = 3;
			break;
	}

	m_csBuffer.Unlock();

	Invalidate();
	theApp.GetSettings()->SampleWinState = m_iCurrentState;

	CWnd::OnRButtonUp(nFlags, point);
}

void CSampleWindow::OnDestroy()
{
	// Shut down worker thread
	m_bThreadRunning = false;
	SetEvent(m_hNewSamples);

	CWnd::OnDestroy();
}
