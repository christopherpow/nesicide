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


// CWavProgressDlg dialog

class CWavProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CWavProgressDlg)

public:
	CWavProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWavProgressDlg();

	void SetFile(CString File);
	void SetOptions(int LengthType, int LengthParam);

// Dialog Data
	enum { IDD = IDD_WAVE_PROGRESS };

protected:

	CSoundGen			*m_pSoundGen;
	CProgressCtrl		*m_pProgressBar;
	CFamiTrackerView	*m_pView;
	CFamiTrackerDoc		*m_pDoc;
	DWORD				m_dwStartTime;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CString m_sFile;
	int m_iSongEndType, m_iSongEndParam;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
