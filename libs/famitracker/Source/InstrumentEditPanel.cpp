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
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "InstrumentEditPanel.h"
#include "InstrumentEditDlg.h"

using namespace std;

// CInstrumentEditPanel dialog
//
// Base class for instrument editors
//

IMPLEMENT_DYNAMIC(CInstrumentEditPanel, CDialog)
CInstrumentEditPanel::CInstrumentEditPanel(UINT nIDTemplate, CWnd* pParent) : CDialog(nIDTemplate, pParent), m_bShow(false)
{
}

CInstrumentEditPanel::~CInstrumentEditPanel()
{
}

void CInstrumentEditPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInstrumentEditPanel, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

//COLORREF m_iBGColor = 0xFF0000;

BOOL CInstrumentEditPanel::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

HBRUSH CInstrumentEditPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Find a proper way to get the background color
	//m_iBGColor = GetPixel(pDC->m_hDC, 2, 2);

	if (!theApp.IsThemeActive())
		return hbr;
	
	switch (nCtlColor) {
		case CTLCOLOR_STATIC:
//		case CTLCOLOR_DLG:
			pDC->SetBkMode(TRANSPARENT);
			// TODO: this might fail on some themes?
			//return NULL;
			return GetSysColorBrush(COLOR_3DHILIGHT);
			//return CreateSolidBrush(m_iBGColor);
	}

	return hbr;
}

BOOL CInstrumentEditPanel::PreTranslateMessage(MSG* pMsg)
{
	TCHAR ClassName[256];

	switch (pMsg->message) {
		case WM_KEYDOWN:
			switch (pMsg->wParam) {
				case VK_RETURN:	// Return
					pMsg->wParam = 0;
					OnKeyReturn();
					return TRUE;
				case VK_ESCAPE:	// Esc, close the dialog
					((CInstrumentEditDlg*)GetParent())->DestroyWindow();
					return TRUE;
				default:	// Note keys
					// Make sure the dialog is selected when previewing
            qDebug("GetClassName not used for now...");
//					GetClassName(pMsg->hwnd, ClassName, 256);
//					if (_tcscmp(ClassName, _T("Edit"))) {
//					//if (GetFocus() == this || GetFocus() == GetParent()) {
//					//if (!CDialog::PreTranslateMessage(pMsg)) {
					
//					//if (DYNAMIC_DOWNCAST(CEdit, GetFocus()) == NULL) {
//						// Remove repeated keys
						if ((pMsg->lParam & (1 << 30)) == 0)
							PreviewNote((unsigned char)pMsg->wParam);
						return TRUE;
//					}
					
			}
			break;
		case WM_KEYUP:
			PreviewRelease((unsigned char)pMsg->wParam);
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CInstrumentEditPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Set focus on mouse clicks to enable note preview from keyboard
	SetFocus();
	CDialog::OnLButtonDown(nFlags, point);
}

void CInstrumentEditPanel::OnKeyReturn()
{
	// Empty
}

void CInstrumentEditPanel::OnSetFocus(CWnd* pOldWnd)
{	
	// Kill the default handler to avoid setting focus to a child control
	//Invalidate();
	CDialog::OnSetFocus(pOldWnd);
}

CFamiTrackerDoc *CInstrumentEditPanel::GetDocument() const
{
	// Return selected document
	return static_cast<CInstrumentEditDlg*>(GetParent())->GetDocument();
}

void CInstrumentEditPanel::PreviewNote(unsigned char Key)
{
	CFamiTrackerView::GetView()->PreviewNote(Key);
}

void CInstrumentEditPanel::PreviewRelease(unsigned char Key)
{
	CFamiTrackerView::GetView()->PreviewRelease(Key);
}

//
// CSequenceInstrumentEditPanel
// 
// For dialog panels with sequence editors. Can translate MML strings 
//

IMPLEMENT_DYNAMIC(CSequenceInstrumentEditPanel, CInstrumentEditPanel)

CSequenceInstrumentEditPanel::CSequenceInstrumentEditPanel(UINT nIDTemplate, CWnd* pParent) 
	: CInstrumentEditPanel(nIDTemplate, pParent), m_pSequence(NULL)
{
}

CSequenceInstrumentEditPanel::~CSequenceInstrumentEditPanel()
{
}

void CSequenceInstrumentEditPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSequenceInstrumentEditPanel, CInstrumentEditPanel)
	ON_NOTIFY(NM_RCLICK, IDC_INSTSETTINGS, OnRClickInstSettings)
END_MESSAGE_MAP()

void CSequenceInstrumentEditPanel::mousePressEvent(QMouseEvent *event)
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
   else if ( event->button() == Qt::RightButton )
   {
      NMHDR nmhdr;
      LRESULT result;
      OnRClickInstSettings(&nmhdr,&result);
   }
}

void CSequenceInstrumentEditPanel::PreviewNote(unsigned char Key)
{
	// Skip if MML window has focus
	if (GetDlgItem(IDC_SEQUENCE_STRING) != GetFocus())
		CFamiTrackerView::GetView()->PreviewNote(Key);
}

void CSequenceInstrumentEditPanel::PreviewRelease(unsigned char Key)
{
	CFamiTrackerView::GetView()->PreviewRelease(Key);
}

void CSequenceInstrumentEditPanel::TranslateMML(CString String, CSequence *pSequence, int Max, int Min)
{
	// Takes a string and translates it into a sequence

	int AddedItems = 0;

	// Reset loop points
	pSequence->SetLoopPoint(-1);
	pSequence->SetReleasePoint(-1);

	string str;
	str.assign(CStringA(String));
	istringstream values(str);
	istream_iterator<string> begin(values);
	istream_iterator<string> end;

	while (begin != end && AddedItems < MAX_SEQUENCE_ITEMS) {
		string item = *begin++;

		if (item[0] == '|') {
			// Set loop point
			pSequence->SetLoopPoint(AddedItems);
		}
		else if (item[0] == '/') {
			// Set release point
			pSequence->SetReleasePoint(AddedItems);
		}
		else {
			// Convert to number
			int value = atoi(item.c_str());
			// Check for invalid chars
			if (!(value == 0 && item[0] != '0')) {
				LIMIT(value, Max, Min);
				pSequence->SetItem(AddedItems++, value);
			}
		}
	}

	pSequence->SetItemCount(AddedItems);
}

void CSequenceInstrumentEditPanel::OnRClickInstSettings(NMHDR* pNMHDR, LRESULT* pResult)
{
	POINT oPoint;
	GetCursorPos(&oPoint);

	if (m_pSequence == NULL)
		return;

	// Create a clone option
	CMenu contextMenu;
	contextMenu.CreatePopupMenu();
	contextMenu.AppendMenu(MF_STRING, ID_CLONE_SEQUENCE, _T("Clone sequence"));
	contextMenu.EnableMenuItem(ID_CLONE_SEQUENCE, (m_pSequence->GetItemCount() != 0) ? FALSE : TRUE);
	contextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, oPoint.x, oPoint.y, this);
	contextMenu.DestroyMenu();
}
