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

#include "stdafx.h"
#include "FamiTrackerDoc.h"
#include "InstrumentEditPanel.h"
#include "SequenceEditor.h"

class CInstrumentEditor2A03 : public CSequenceInstrumentEditPanel
{
   Q_OBJECT
   // Qt stuff
public slots:
   void instSettings_itemSelectionChanged();
   void sequenceSpin_valueChanged(int oldVal,int newVal);
   void menuAction_triggered(int id);
public: // For some reason MOC doesn't like the protection specification inside DECLARE_DYNAMIC
   
   DECLARE_DYNAMIC(CInstrumentEditor2A03)
   
public:
	CInstrumentEditor2A03(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstrumentEditor2A03();
	virtual int GetIDD() const { return IDD; };
	virtual TCHAR *GetTitle() const { return _T("2A03 settings"); };

	// Public
	virtual void SelectInstrument(int Instrument);
	virtual void SetSequenceString(CString Sequence, bool Changed);

// Dialog Data
	enum { IDD = IDD_INSTRUMENT_INTERNAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnKeyReturn();

	void SelectSequence(int Sequence, int Type);
	void TranslateMML(CString String, int Max, int Min);

protected:
	static LPCTSTR INST_SETTINGS[CInstrument2A03::SEQUENCE_COUNT];

protected:
	CWnd				*m_pParentWin;
	CSequenceEditor		*m_pSequenceEditor;
	CInstrument2A03		*m_pInstrument;

	unsigned int		m_iSelectedSetting;	 // In settings list

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedInstsettings(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSeqIndex();
	afx_msg void OnBnClickedFreeSeq();
	virtual BOOL DestroyWindow();
	afx_msg void OnCloneSequence();
};
