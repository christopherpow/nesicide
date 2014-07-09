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
   - The instrument list
 
 * CBannerEdit
   - An edit box that displays a banner when no text is present
 
 * CLockedEdit
   - An edit box that is locked for editing, unlocked with double-click

*/

///
/// CInstrumentList
///

// This class takes care of handling the instrument list, 
// since mapping of instruments list to instruments are not 1:1

IMPLEMENT_DYNAMIC(CInstrumentList, CListCtrl)

BEGIN_MESSAGE_MAP(CInstrumentList, CListCtrl)
	ON_WM_CONTEXTMENU()
//   ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, &CInstrumentList::OnLvnBeginlabeledit)
//   ON_NOTIFY_REFLECT(NM_CLICK, &CInstrumentList::OnNMClick)
//   ON_NOTIFY_REFLECT(LVN_KEYDOWN, &CInstrumentList::OnLvnKeydown)
//   ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CInstrumentList::OnLvnEndlabeledit)
//   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CInstrumentList::OnLvnItemchanged)
//   ON_NOTIFY_REFLECT(NM_DBLCLK, &CInstrumentList::OnNMDblclk)
   ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
   ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
   ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
   ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
   ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
END_MESSAGE_MAP()

CInstrumentList::CInstrumentList(CMainFrame *pMainFrame) : m_pMainFrame(pMainFrame)
{
}

int CInstrumentList::GetInstrumentIndex(int Selection) const
{
	// Get the instrument number from an item in the list (Selection = list index)
	int Instrument;
	TCHAR Text[256];

	if (Selection == -1)
		return -1;

	GetItemText(Selection, 0, Text, 256);
	_stscanf(Text, _T("%X"), &Instrument);

	return Instrument;
}

int CInstrumentList::FindInstrument(int Index)
{
	// Find the instrument item from the list (Index = instrument number)
	CString Txt;
	Txt.Format(_T("%02X"), Index);

	LVFINDINFO info;
	info.flags = LVFI_PARTIAL | LVFI_STRING;
	info.psz = Txt;

	return FindItem(&info);
}

void CInstrumentList::SelectInstrument(int Index)
{
	// Highlight a specified instrument (Index = instrument number)	
	int ListIndex = FindInstrument(Index);
	SetSelectionMark(ListIndex);
	SetItemState(ListIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	EnsureVisible(ListIndex, FALSE);
}

void CInstrumentList::SelectNextItem()
{
	// Select next instrument
	int SelIndex = GetSelectionMark();
	int Count = GetItemCount();
	if (SelIndex < (Count - 1)) {
		int Slot = GetInstrumentIndex(SelIndex + 1);
		m_pMainFrame->SelectInstrument(Slot);
	}
}

void CInstrumentList::SelectPreviousItem()
{
	// Select previous instrument
	int SelIndex = GetSelectionMark();
	if (SelIndex > 0) {
		int Slot = GetInstrumentIndex(SelIndex - 1);
		m_pMainFrame->SelectInstrument(Slot);
	}
}

void CInstrumentList::InsertInstrument(int Index)
{
	// Inserts an instrument in the list (Index = instrument number)
	CFamiTrackerDoc *pDoc = CFamiTrackerDoc::GetDoc();

	if (!pDoc->IsInstrumentUsed(Index))
		return;

	char Name[CInstrument::INST_NAME_MAX];
	pDoc->GetInstrumentName(Index, Name);
	int Type = pDoc->GetInstrumentType(Index);

	// Name is of type index - name
	CString Text;
	Text.Format(_T("%02X - %s"), Index, A2T(Name));
	InsertItem(Index, Text, Type - 1);
}

void CInstrumentList::RemoveInstrument(int Index)
{
	// Remove an instrument from the list (Index = instrument number)
	int Selection = FindInstrument(Index);
	if (Selection != -1)
		DeleteItem(Selection);
}

void CInstrumentList::SetInstrumentName(int Index, TCHAR *pName)
{
	// Update instrument name in the list
	int ListIndex = GetSelectionMark();
	CString Name;
	Name.Format(_T("%02X - %s"), Index, pName);
	SetItemText(ListIndex, 0, Name);
}

void CInstrumentList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int Instrument = GetInstrumentIndex(GetSelectionMark());

	// Select the instrument
	if (Instrument != -1)
		m_pMainFrame->SelectInstrument(Instrument);

	// Display the popup menu
	CMenu *pPopupMenu, PopupMenuBar;
	PopupMenuBar.LoadMenu(IDR_INSTRUMENT_POPUP);
	pPopupMenu = PopupMenuBar.GetSubMenu(0);
	// Route the menu messages to mainframe
	pPopupMenu->TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_pMainFrame);

	// Return focus to pattern editor
	m_pMainFrame->GetActiveView()->SetFocus();
}

void CInstrumentList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	// Selection changed
//	if (pNMLV->uNewState & LVIS_SELECTED)
//		m_pMainFrame->SelectInstrument(GetInstrumentIndex(pNMLV->iItem));

	*pResult = 0;
}

void CInstrumentList::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CInstrumentList::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CInstrumentList::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// Select instrument
	m_pMainFrame->SelectInstrument(GetInstrumentIndex(pNMItemActivate->iItem));

	// Move focus to pattern editor 
	m_pMainFrame->GetActiveView()->SetFocus();

	*pResult = 0;
}

void CInstrumentList::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	// Empty

	*pResult = 0;
}

void CInstrumentList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// Double-click = instrument editor
	m_pMainFrame->OpenInstrumentEditor();

	*pResult = 0;
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

	dc->SetBkColor(dc->GetPixel(4, 4));
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
