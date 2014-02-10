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

/*
 * The sequence setting button. Used only by arpeggio at the moment
 */

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "Sequence.h"
#include "SequenceEditor.h"
#include "GraphEditor.h"
#include "InstrumentEditPanel.h"
#include "SequenceSetting.h"

// Arpeggio menu
enum {MENU_ARP_ABSOLUTE = 500, MENU_ARP_RELATIVE, MENU_ARP_FIXED};

IMPLEMENT_DYNAMIC(CSequenceSetting, CWnd)

CSequenceSetting::CSequenceSetting(CWnd *pParent) 
	: CWnd(), m_pParent(pParent), m_pSequence(NULL)
{
}

CSequenceSetting::~CSequenceSetting()
{
}

BEGIN_MESSAGE_MAP(CSequenceSetting, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(MENU_ARP_ABSOLUTE, OnMenuArpAbsolute)
	ON_COMMAND(MENU_ARP_RELATIVE, OnMenuArpRelative)
	ON_COMMAND(MENU_ARP_FIXED, OnMenuArpFixed)
END_MESSAGE_MAP()

int mode = 0;

void CSequenceSetting::Setup(CFont *pFont)
{
	m_menuPopup.CreatePopupMenu();

	m_menuPopup.AppendMenu(MF_STRING, MENU_ARP_ABSOLUTE, _T("Absolute"));
	m_menuPopup.AppendMenu(MF_STRING, MENU_ARP_RELATIVE, _T("Relative"));
	m_menuPopup.AppendMenu(MF_STRING, MENU_ARP_FIXED, _T("Fixed"));

	m_pFont = pFont;
}

void CSequenceSetting::OnPaint()
{
	CPaintDC dc(this);
	bool bDraw(false);

	CRect rect;
	GetClientRect(&rect);

	if (m_iType == SEQ_ARPEGGIO)
		bDraw = true;

	int mode = m_pSequence->GetSetting();

	if (bDraw) {
		const COLORREF BG_COLOR = 0x202020;

		dc.FillSolidRect(rect, BG_COLOR);
		dc.DrawEdge(rect, EDGE_BUMP, BF_RECT);

		dc.SelectObject(m_pFont);

		dc.SetTextColor(0xFFFFFF);
		dc.SetBkColor(BG_COLOR);

		LPCTSTR MODES[] = {_T("Absolute"), _T("Fixed"), _T("Relative")};

		rect.top += 2;
		dc.DrawText(MODES[mode], _tcslen(MODES[mode]), rect, DT_CENTER);
	}
	else {
		dc.FillSolidRect(rect, 0xFFFFFF);
	}
}

void CSequenceSetting::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect(rect);

	if (m_iType == SEQ_ARPEGGIO) {

		switch (m_pSequence->GetSetting()) {
			case ARP_SETTING_ABSOLUTE:
				m_menuPopup.CheckMenuItem(MENU_ARP_ABSOLUTE, MF_CHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_RELATIVE, MF_UNCHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_FIXED, MF_UNCHECKED | MF_BYCOMMAND);
				break;
			case ARP_SETTING_RELATIVE:
				m_menuPopup.CheckMenuItem(MENU_ARP_ABSOLUTE, MF_UNCHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_RELATIVE, MF_CHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_FIXED, MF_UNCHECKED | MF_BYCOMMAND);
				break;
			case ARP_SETTING_FIXED:
				m_menuPopup.CheckMenuItem(MENU_ARP_ABSOLUTE, MF_UNCHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_RELATIVE, MF_UNCHECKED | MF_BYCOMMAND);
				m_menuPopup.CheckMenuItem(MENU_ARP_FIXED, MF_CHECKED | MF_BYCOMMAND);
				break;
		}

		m_menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x + rect.left, point.y + rect.top, this);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CSequenceSetting::SelectSequence(CSequence *pSequence, int Type, int InstrumentType)
{
	m_pSequence = pSequence;
	m_iType		= Type;
	m_iInstType = InstrumentType;

	UpdateWindow();
	RedrawWindow();
}

void CSequenceSetting::OnMenuArpAbsolute()
{
	m_pSequence->SetSetting(ARP_SETTING_ABSOLUTE);
	((CSequenceEditor*)m_pParent)->ChangedSetting();
}

void CSequenceSetting::OnMenuArpRelative()
{
	m_pSequence->SetSetting(ARP_SETTING_RELATIVE);
	((CSequenceEditor*)m_pParent)->ChangedSetting();
}

void CSequenceSetting::OnMenuArpFixed()
{
	m_pSequence->SetSetting(ARP_SETTING_FIXED);
	((CSequenceEditor*)m_pParent)->ChangedSetting();

	// Prevent invalid sequence items
	for (unsigned int i = 0; i < m_pSequence->GetItemCount(); ++i) {
		int Item = m_pSequence->GetItem(i);
		if (Item < 0)
			Item = 0;
		m_pSequence->SetItem(i, Item);
	}
}
