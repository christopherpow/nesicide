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
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrm.h"
#include "CustomControls.h"

/*

 Contains some custom GUI controls.

 * CInstrumentList
   - The instrument list, extended to contain a context menu
 
 * CBannerEdit
   - An edit box that displays a banner when no text is present
 
 * CLockedEdit
   - An edit box that is locked for editing, unlocked with double-click

*/

///
/// CInstrumentList
///

// This class takes care of the context menu message

IMPLEMENT_DYNAMIC(CInstrumentList, CListCtrl)

BEGIN_MESSAGE_MAP(CInstrumentList, CListCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// TODO: expand this class

CInstrumentList::CInstrumentList(CMainFrame *pMainFrame) : m_pMainFrame(pMainFrame)
{
}

void CInstrumentList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int Instrument(0);
	TCHAR Text[256];

	if (GetSelectionMark() != -1) {
		// Select the instrument
		GetItemText(GetSelectionMark(), 0, Text, 256);
		_stscanf(Text, _T("%X"), &Instrument);
		CFamiTrackerDoc::GetDoc()->GetInstrumentName(Instrument, Text);
		CFamiTrackerView::GetView()->SetInstrument(Instrument);
		// TODO: fix??
		//m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(Text);
	}

	// Display the popup menu
	CMenu *pPopupMenu, PopupMenuBar;
	PopupMenuBar.LoadMenu(IDR_INSTRUMENT_POPUP);
	pPopupMenu = PopupMenuBar.GetSubMenu(0);
	// Route the menu messages to mainframe
	pPopupMenu->TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_pMainFrame);

	// Return focus to pattern editor
	m_pMainFrame->GetActiveView()->SetFocus();
}

///
/// CBannerEdit
///

const TCHAR CBannerEdit::BANNER_FONT[]	 = _T("Tahoma");
const COLORREF CBannerEdit::BANNER_COLOR = 0x808080;

// Used to display a banner in edit boxes

IMPLEMENT_DYNAMIC(CBannerEdit, CEdit)

BEGIN_MESSAGE_MAP(CBannerEdit, CEdit)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CBannerEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);
	Invalidate();
	RedrawWindow(NULL, NULL, RDW_ERASE);
}

void CBannerEdit::OnPaint()
{
	CEdit::OnPaint();

	// Overlay some text
	CString str;
	GetWindowText(str);

	// only if empty and not in focus
	if (str.GetLength() > 0 || GetFocus() == this)
		return;

	CDC *dc(GetDC());
	CFont font;

	font.CreateFont(12, 0, 0, 0, 0, TRUE, FALSE, FALSE, 0, 0, 0, 0, 0, BANNER_FONT);
	CFont *pOldFont = dc->SelectObject(&font);

	dc->SetTextColor(BANNER_COLOR);
	dc->TextOut(2, 1, m_strText);
	dc->SelectObject(pOldFont);

	ReleaseDC(dc);
}


void CBannerEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	// Limit string size to 31 chars
	CString text;
	GetWindowText(text);
	if (text.GetLength() > 31)
		text = text.Left(31);
	SetWindowText(text);
}

///
/// CLockedEdit
///

// This class takes care of locking/unlocking edit boxes by double clicking

IMPLEMENT_DYNAMIC(CLockedEdit, CEdit)

BEGIN_MESSAGE_MAP(CLockedEdit, CEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

bool CLockedEdit::IsEditable() const
{
	return !((GetWindowLong(m_hWnd, GWL_STYLE) & ES_READONLY) == ES_READONLY);
}

bool CLockedEdit::Update()
{
	bool ret_val(m_bUpdate);
	m_bUpdate = false;
	return ret_val;
}

int CLockedEdit::GetValue() const
{
	return m_iValue;
}

void CLockedEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bUpdate = false;
	if (IsEditable())
		SetSel(0, -1);	// select all
	else {
		SendMessage(EM_SETREADONLY, FALSE);
		SetFocus();
		SetSel(0, -1);
	}
}

void CLockedEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	if (!IsEditable())
		CFamiTrackerView::GetView()->SetFocus();
}

void CLockedEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	CString Text;
	if (!IsEditable())
		return;
	GetWindowText(Text);
	m_iValue = _ttoi(Text);
	m_bUpdate = true;
	SendMessage(EM_SETREADONLY, TRUE);
}

BOOL CLockedEdit::PreTranslateMessage(MSG* pMsg)
{
	// For some reason OnKeyDown won't work
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		CFamiTrackerView::GetView()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}
