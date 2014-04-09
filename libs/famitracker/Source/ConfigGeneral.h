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

#pragma once


// CConfigGeneral dialog

class CConfigGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfigGeneral)

public:
	CConfigGeneral();
	virtual ~CConfigGeneral();

// Dialog Data
	enum { IDD = IDD_CONFIG_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool	m_bWrapCursor;
	bool	m_bWrapFrames;
	bool	m_bFreeCursorEdit;
	bool	m_bPreviewWAV;
	bool	m_bKeyRepeat;
	bool	m_bRowInHex;
	bool	m_bFramePreview;
	int		m_iEditStyle;
	bool	m_bNoDPCMReset;
	bool	m_bNoStepMove;
	int		m_iPageStepSize;
	bool	m_bPullUpDelete;
	bool	m_bBackups;
	bool	m_bSingleInstance;
	int		m_iKeyNoteCut;
	int		m_iKeyNoteRelease;
	int		m_iKeyClear;
	int		m_iKeyRepeat;

	CToolTipCtrl m_wndToolTip;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOptWrapcursor();
	afx_msg void OnBnClickedOptWrapFrames();
	afx_msg void OnBnClickedOptFreecursor();
	afx_msg void OnBnClickedOptWavepreview();
	afx_msg void OnBnClickedOptKeyrepeat();
	afx_msg void OnBnClickedStyle1();
	afx_msg void OnBnClickedStyle2();
	afx_msg void OnBnClickedStyle3();
	afx_msg void OnBnClickedOptHexadecimal();
	afx_msg void OnBnClickedSquarehack();
	afx_msg void OnBnClickedOptFramepreview();
	afx_msg void OnBnClickedOptNodpcmreset();
	afx_msg void OnCbnEditupdatePagelength();
	afx_msg void OnCbnSelendokPagelength();
	afx_msg void OnBnClickedOptNostepmove();
	afx_msg void OnBnClickedOptPullupdelete();
	afx_msg void OnBnClickedOptBackups();
	afx_msg void OnBnClickedOptSingleInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
