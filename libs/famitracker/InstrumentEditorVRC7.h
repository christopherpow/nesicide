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

class CInstrumentEditorVRC7 : public CInstrumentEditPanel
{
   Q_OBJECT
   DECLARE_DYNAMIC(CInstrumentEditorVRC7)
   // Qt stuff
public slots:
   void patch_currentIndexChanged(int index);
   void copy_clicked();
   void paste_clicked();
   void m_am_clicked();
   void m_vib_clicked();
   void m_eq_clicked();
   void m_ksr2_clicked();
   void m_dm_clicked();
   void m_eg_clicked();
   void m_ksl_valueChanged(int val);
   void m_mul_valueChanged(int val);
   void m_ar_valueChanged(int val);
   void m_dr_valueChanged(int val);
   void m_sl_valueChanged(int val);
   void m_rr_valueChanged(int val);
   void c_am_clicked();
   void c_vib_clicked();
   void c_eq_clicked();
   void c_ksr_clicked();
   void c_dm_clicked();
   void c_eg_clicked();
   void c_ksl_valueChanged(int val);
   void c_mul_valueChanged(int val);
   void c_ar_valueChanged(int val);
   void c_dr_valueChanged(int val);
   void c_sl_valueChanged(int val);
   void c_rr_valueChanged(int val);
   void tl_valueChanged(int val);
   void fb_valueChanged(int val);
   
public:
	CInstrumentEditorVRC7(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstrumentEditorVRC7();
	virtual int GetIDD() const { return IDD; };
	virtual TCHAR *GetTitle() const { return _T("Konami VRC7"); };

	virtual void SelectInstrument(int Instrument);

// Dialog Data
	enum { IDD = IDD_INSTRUMENT_VRC7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadCustomPatch();
	void SaveCustomPatch();
	void LoadInternalPatch(int Num);

	void SetupSlider(int Slider, int Max);
	int GetSliderVal(int Slider);
	void SetSliderVal(int Slider, int Value);
	void EnableControls(bool bEnable);

protected:
	//unsigned int m_iInstrument;
	CInstrumentVRC7 *m_pInstrument;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangePatch();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCheckbox();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
};
