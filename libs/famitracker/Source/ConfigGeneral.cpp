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
#include "ConfigGeneral.h"
#include "Settings.h"

// CConfigGeneral dialog

IMPLEMENT_DYNAMIC(CConfigGeneral, CPropertyPage)
CConfigGeneral::CConfigGeneral()
	: CPropertyPage(CConfigGeneral::IDD)
{
}

CConfigGeneral::~CConfigGeneral()
{
}

void CConfigGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigGeneral, CPropertyPage)
	ON_BN_CLICKED(IDC_OPT_WRAPCURSOR, OnBnClickedOptWrapcursor)
	ON_BN_CLICKED(IDC_OPT_WRAPFRAMES, OnBnClickedOptWrapFrames)
	ON_BN_CLICKED(IDC_OPT_FREECURSOR, OnBnClickedOptFreecursor)
	ON_BN_CLICKED(IDC_OPT_WAVEPREVIEW, OnBnClickedOptWavepreview)
	ON_BN_CLICKED(IDC_OPT_KEYREPEAT, OnBnClickedOptKeyrepeat)
	ON_BN_CLICKED(IDC_STYLE1, OnBnClickedStyle1)
	ON_BN_CLICKED(IDC_STYLE2, OnBnClickedStyle2)
	ON_BN_CLICKED(IDC_STYLE3, OnBnClickedStyle3)
	ON_BN_CLICKED(IDC_OPT_HEXROW, OnBnClickedOptHexadecimal)
	ON_BN_CLICKED(IDC_OPT_FRAMEPREVIEW, OnBnClickedOptFramepreview)
	ON_BN_CLICKED(IDC_OPT_NODPCMRESET, OnBnClickedOptNodpcmreset)
	ON_CBN_EDITUPDATE(IDC_PAGELENGTH, OnCbnEditupdatePagelength)
	ON_CBN_SELENDOK(IDC_PAGELENGTH, OnCbnSelendokPagelength)
	ON_BN_CLICKED(IDC_OPT_NOSTEPMOVE, OnBnClickedOptNostepmove)
	ON_BN_CLICKED(IDC_OPT_PULLUPDELETE, OnBnClickedOptPullupdelete)
	ON_BN_CLICKED(IDC_OPT_BACKUPS, OnBnClickedOptBackups)
	ON_BN_CLICKED(IDC_OPT_SINGLEINSTANCE, OnBnClickedOptSingleInstance)
END_MESSAGE_MAP()


// CConfigGeneral message handlers

BOOL CConfigGeneral::OnSetActive()
{
	CheckDlgButton(IDC_OPT_WRAPCURSOR, m_bWrapCursor);
	CheckDlgButton(IDC_OPT_WRAPFRAMES, m_bWrapFrames);
	CheckDlgButton(IDC_OPT_FREECURSOR, m_bFreeCursorEdit);
	CheckDlgButton(IDC_OPT_WAVEPREVIEW, m_bPreviewWAV);
	CheckDlgButton(IDC_OPT_KEYREPEAT, m_bKeyRepeat);
	CheckDlgButton(IDC_OPT_HEXROW, m_bRowInHex);
	CheckDlgButton(IDC_OPT_FRAMEPREVIEW, m_bFramePreview);
	CheckDlgButton(IDC_OPT_NODPCMRESET, m_bNoDPCMReset);
	CheckDlgButton(IDC_OPT_NOSTEPMOVE, m_bNoStepMove);
	CheckDlgButton(IDC_STYLE1, m_iEditStyle == EDIT_STYLE1);
	CheckDlgButton(IDC_STYLE2, m_iEditStyle == EDIT_STYLE2);
	CheckDlgButton(IDC_STYLE3, m_iEditStyle == EDIT_STYLE3);
	CheckDlgButton(IDC_OPT_PULLUPDELETE, m_bPullUpDelete);
	CheckDlgButton(IDC_OPT_BACKUPS, m_bBackups);
	CheckDlgButton(IDC_OPT_SINGLEINSTANCE, m_bSingleInstance);
	SetDlgItemInt(IDC_PAGELENGTH, m_iPageStepSize, FALSE);
	return CPropertyPage::OnSetActive();
}

void CConfigGeneral::OnOK()
{
	CPropertyPage::OnOK();
}

void CConfigGeneral::OnBnClickedOptWrapcursor()
{
	m_bWrapCursor = IsDlgButtonChecked(IDC_OPT_WRAPCURSOR) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptWrapFrames()
{
	m_bWrapFrames = IsDlgButtonChecked(IDC_OPT_WRAPFRAMES) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptFreecursor()
{
	m_bFreeCursorEdit = IsDlgButtonChecked(IDC_OPT_FREECURSOR) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptWavepreview()
{
	m_bPreviewWAV = IsDlgButtonChecked(IDC_OPT_WAVEPREVIEW) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptKeyrepeat()
{
	m_bKeyRepeat = IsDlgButtonChecked(IDC_OPT_KEYREPEAT) != 0;
	SetModified();
}

BOOL CConfigGeneral::OnApply()
{
	// Translate page length
	BOOL Trans;

	m_iPageStepSize = GetDlgItemInt(IDC_PAGELENGTH, &Trans, FALSE);
	
	if (Trans == FALSE)
		m_iPageStepSize = 4;
	else if (m_iPageStepSize > MAX_PATTERN_LENGTH)
		m_iPageStepSize = MAX_PATTERN_LENGTH;

	theApp.GetSettings()->General.bWrapCursor		= m_bWrapCursor;
	theApp.GetSettings()->General.bWrapFrames		= m_bWrapFrames;
	theApp.GetSettings()->General.bFreeCursorEdit	= m_bFreeCursorEdit;
	theApp.GetSettings()->General.bWavePreview		= m_bPreviewWAV;
	theApp.GetSettings()->General.bKeyRepeat		= m_bKeyRepeat;
	theApp.GetSettings()->General.bRowInHex			= m_bRowInHex;
	theApp.GetSettings()->General.iEditStyle		= m_iEditStyle;
	theApp.GetSettings()->General.bFramePreview		= m_bFramePreview;
	theApp.GetSettings()->General.bNoDPCMReset		= m_bNoDPCMReset;
	theApp.GetSettings()->General.bNoStepMove		= m_bNoStepMove;
	theApp.GetSettings()->General.iPageStepSize		= m_iPageStepSize;
	theApp.GetSettings()->General.bPullUpDelete		= m_bPullUpDelete;
	theApp.GetSettings()->General.bBackups			= m_bBackups;
	theApp.GetSettings()->General.bSingleInstance	= m_bSingleInstance;

	theApp.GetSettings()->Keys.iKeyNoteCut			= m_iKeyNoteCut;
	theApp.GetSettings()->Keys.iKeyNoteRelease		= m_iKeyNoteRelease;
	theApp.GetSettings()->Keys.iKeyClear			= m_iKeyClear;
	theApp.GetSettings()->Keys.iKeyRepeat			= m_iKeyRepeat;

	return CPropertyPage::OnApply();
}

BOOL CConfigGeneral::OnInitDialog()
{
	char Text[64];

	CPropertyPage::OnInitDialog();

	m_bWrapCursor		= theApp.GetSettings()->General.bWrapCursor;
	m_bWrapFrames		= theApp.GetSettings()->General.bWrapFrames;
	m_bFreeCursorEdit	= theApp.GetSettings()->General.bFreeCursorEdit;
	m_bPreviewWAV		= theApp.GetSettings()->General.bWavePreview;
	m_bKeyRepeat		= theApp.GetSettings()->General.bKeyRepeat;
	m_bRowInHex			= theApp.GetSettings()->General.bRowInHex;
	m_iEditStyle		= theApp.GetSettings()->General.iEditStyle;
	m_bFramePreview		= theApp.GetSettings()->General.bFramePreview;
	m_bNoDPCMReset		= theApp.GetSettings()->General.bNoDPCMReset;
	m_bNoStepMove		= theApp.GetSettings()->General.bNoStepMove;
	m_iPageStepSize		= theApp.GetSettings()->General.iPageStepSize;
	m_bPullUpDelete		= theApp.GetSettings()->General.bPullUpDelete;
	m_bBackups			= theApp.GetSettings()->General.bBackups;
	m_bSingleInstance	= theApp.GetSettings()->General.bSingleInstance;

	m_iKeyNoteCut		= theApp.GetSettings()->Keys.iKeyNoteCut; 
	m_iKeyNoteRelease	= theApp.GetSettings()->Keys.iKeyNoteRelease; 
	m_iKeyClear			= theApp.GetSettings()->Keys.iKeyClear; 
	m_iKeyRepeat		= theApp.GetSettings()->Keys.iKeyRepeat;

	GetKeyNameText(MapVirtualKey(m_iKeyNoteCut, MAPVK_VK_TO_VSC) << 16, Text, 64);
	SetDlgItemText(IDC_KEY_NOTE_CUT, Text);
	GetKeyNameText(MapVirtualKey(m_iKeyNoteRelease, MAPVK_VK_TO_VSC) << 16, Text, 64);
	SetDlgItemText(IDC_KEY_NOTE_RELEASE, Text);
	GetKeyNameText(MapVirtualKey(m_iKeyClear, MAPVK_VK_TO_VSC) << 16, Text, 64);
	SetDlgItemText(IDC_KEY_CLEAR, Text);
	GetKeyNameText(MapVirtualKey(m_iKeyRepeat, MAPVK_VK_TO_VSC) << 16, Text, 64);
	SetDlgItemText(IDC_KEY_REPEAT, Text);

	EnableToolTips(TRUE);

	m_wndToolTip.Create(this, TTS_ALWAYSTIP);
	m_wndToolTip.Activate(TRUE);

	CWnd *pWndChild = GetWindow(GW_CHILD);
	CString strToolTip;

	while (pWndChild) {
		int nID = pWndChild->GetDlgCtrlID();
		if (strToolTip.LoadString(nID)) {
			m_wndToolTip.AddTool(pWndChild, strToolTip);
		}
		pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigGeneral::OnBnClickedStyle1()
{
	m_iEditStyle = EDIT_STYLE1;
	SetModified();
}

void CConfigGeneral::OnBnClickedStyle2()
{
	m_iEditStyle = EDIT_STYLE2;
	SetModified();
}

void CConfigGeneral::OnBnClickedStyle3()
{
	m_iEditStyle = EDIT_STYLE3;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptHexadecimal()
{
	m_bRowInHex = IsDlgButtonChecked(IDC_OPT_HEXROW) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptFramepreview()
{
	m_bFramePreview = IsDlgButtonChecked(IDC_OPT_FRAMEPREVIEW) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptNodpcmreset()
{
	m_bNoDPCMReset = IsDlgButtonChecked(IDC_OPT_NODPCMRESET) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptNostepmove()
{
	m_bNoStepMove = IsDlgButtonChecked(IDC_OPT_NOSTEPMOVE) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptPullupdelete()
{
	m_bPullUpDelete = IsDlgButtonChecked(IDC_OPT_PULLUPDELETE) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptBackups()
{
	m_bBackups = IsDlgButtonChecked(IDC_OPT_BACKUPS) != 0;
	SetModified();
}

void CConfigGeneral::OnBnClickedOptSingleInstance()
{
	m_bSingleInstance = IsDlgButtonChecked(IDC_OPT_SINGLEINSTANCE) != 0;
	SetModified();
}

void CConfigGeneral::OnCbnEditupdatePagelength()
{
	SetModified();
}

void CConfigGeneral::OnCbnSelendokPagelength()
{
	SetModified();
}

BOOL CConfigGeneral::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		char Text[64];
		int id = GetFocus()->GetDlgCtrlID();
		int key = pMsg->wParam;

		if (key == 27)		// ESC
			key = 0;

		switch (id) {
			case IDC_KEY_NOTE_CUT:
				m_iKeyNoteCut = key;
				break;
			case IDC_KEY_NOTE_RELEASE:
				m_iKeyNoteRelease = key;
				break;
			case IDC_KEY_CLEAR:
				m_iKeyClear = key;
				break;
			case IDC_KEY_REPEAT:
				m_iKeyRepeat = key;
				break;
			default:
				return CPropertyPage::PreTranslateMessage(pMsg);
		}

		GetKeyNameText(key ? pMsg->lParam : 0, Text, 64);
		SetDlgItemText(id, Text);

		SetModified();

		return TRUE;
	}

	m_wndToolTip.RelayEvent(pMsg);

	return CPropertyPage::PreTranslateMessage(pMsg);
}
