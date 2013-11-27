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

class CFamiTrackerView;

// CInstrumentDPCM dialog

class CInstrumentEditorDPCM : public CInstrumentEditPanel
{
   Q_OBJECT
   // Qt interfaces
protected:
   void contextMenuEvent(QContextMenuEvent *event);
public slots:
   void load_clicked();
   void unload_clicked();
   void sampleList_itemSelectionChanged();
   void sampleList_cellClicked(int row, int column);
   void sampleList_cellDoubleClicked(int row, int column);
   void import_clicked();
   void octave_currentIndexChanged(int index);
   void pitch_currentIndexChanged(int index);
   void table_itemSelectionChanged();
   void table_cellClicked(int row, int column);
   void table_cellDoubleClicked(int row, int column);
   void samples_currentIndexChanged(int index);
   void save_clicked();
   void loop_clicked();
   void add_clicked();
   void remove_clicked();
   void loopPoint_textEdited(QString str);
   void edit_clicked();
   void preview_clicked();
   void menuAction_triggered(int id);
protected:
   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
public: // For some reason MOC doesn't like the protection specification inside DECLARE_DYNAMIC
   
	DECLARE_DYNAMIC(CInstrumentEditorDPCM)

public:
	CInstrumentEditorDPCM(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstrumentEditorDPCM();
	virtual int GetIDD() const { return IDD; };
	virtual TCHAR *GetTitle() const { return _T("DPCM samples"); };

// Dialog Data
	enum { IDD = IDD_INSTRUMENT_DPCM };

	// Overloaded
	void SelectInstrument(int Instrument);

protected:
	static const char *KEY_NAMES[];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void BuildKeyList();
	void BuildSampleList();
	void UpdateKey(int Index);
	bool LoadSample(const CString &FilePath, const CString &FileName);
	bool InsertSample(CDSample *pNewSample);

	CDSample *GetSelectedSample();

protected:
	CListCtrl	*m_pTableListCtrl;
	CListCtrl	*m_pSampleListCtrl;

	CInstrument2A03	*m_pInstrument;

	int	m_iSelectedSample;
	int	m_iOctave, m_iSelectedKey;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedUnload();
	afx_msg void OnNMClickSampleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedImport();
	afx_msg void OnCbnSelchangeOctave();
	afx_msg void OnCbnSelchangePitch();
	afx_msg void OnLvnItemchangedTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeSamples();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoop();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSawhack();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnEnChangeLoopPoint();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnNMDblclkSampleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPreview();
	afx_msg void OnNMRClickSampleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeDeltaCounter();
	afx_msg void OnDeltaposDeltaSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
