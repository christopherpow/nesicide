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

// CModuleImportDlg dialog

class CModuleImportDlg : public CDialog
{
   Q_OBJECT
   // Qt interfaces
public slots:
   void ok_clicked();
   void cancel_clicked();
public: // For some reason MOC doesn't like the protection specification inside DECLARE_DYNAMIC
   
	DECLARE_DYNAMIC(CModuleImportDlg)

public:
	CModuleImportDlg(CFamiTrackerDoc *pDoc);
	virtual ~CModuleImportDlg();

// Dialog Data
	enum { IDD = IDD_IMPORT };

public:
	bool LoadFile(CString Path, CFamiTrackerDoc *pDoc);

private:
	CFamiTrackerDoc *m_pDocument;
	CFamiTrackerDoc *m_pImportedDoc;

	int m_iInstrumentTable[MAX_INSTRUMENTS];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool ImportInstruments();
	bool ImportTracks();

protected:
	CCheckListBox m_ctlTrackList;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
