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

#include "cqtmfc.h"
#include "resource.h"

#include "FamiTrackerDoc.h"

// CModulePropertiesDlg dialog

class CModulePropertiesDlg : public CDialog
{
   Q_OBJECT
   // Qt interfaces
public slots:
   void songList_itemSelectionChanged();
   void songAdd_clicked();
   void songRemove_clicked();
   void songUp_clicked();
   void songDown_clicked();
   void songImport_clicked();
   void songName_textChanged(QString text);
   void expansion_currentIndexChanged(int index);
   void ok_clicked();
   void cancel_clicked();
   void channels_valueChanged(int value);
public:
	DECLARE_DYNAMIC(CModulePropertiesDlg)

private:
	void SelectSong(int Song);
	void UpdateSongButtons();
	
	unsigned int m_iSelectedSong;
	CListCtrl *m_pSongList;

	CFamiTrackerDoc *m_pDocument;

public:
	CModulePropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModulePropertiesDlg();

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void FillSongList();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSongAdd();
	afx_msg void OnBnClickedSongRemove();
	afx_msg void OnBnClickedSongUp();
	afx_msg void OnBnClickedSongDown();
	afx_msg void OnEnChangeSongname();
	afx_msg void OnClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSongImport();
	afx_msg void OnCbnSelchangeExpansion();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLvnItemchangedSonglist(NMHDR *pNMHDR, LRESULT *pResult);
};
