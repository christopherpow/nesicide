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

// CSampleView control

class CSampleView : public CStatic {
	DECLARE_DYNAMIC(CSampleView)
protected:
	DECLARE_MESSAGE_MAP()
public:
	CSampleView();
	virtual ~CSampleView();

	void	DrawPlayCursor(int Pos);
	void	DrawStartCursor();
	void	CalculateSample(CDSample *pSample, int Start);
	void	UpdateInfo();
	void	OnHome();

	int		GetStartOffset() const { return m_iStartCursor; };
	int		GetSelStart() const { return (m_iSelStart < m_iSelEnd) ? m_iSelStart : m_iSelEnd; };
	int		GetSelEnd() const { return (m_iSelStart > m_iSelEnd) ? m_iSelStart : m_iSelEnd; };
	int		GetBlock(int Pixel) const;
	int		GetPixel(int Block) const;

private:
	int *m_pSamples;

	int m_iSize;
	int m_iBlockSize;
	int m_iSelStart;
	int m_iSelEnd;
	int m_iStartCursor;
	double m_dSampleStep;
	bool m_bClicked;

	CRect	m_clientRect;
	CDC		m_dcCopy;
	CBitmap m_bmpCopy;

	CPen *m_pSolidPen;
	CPen *m_pDashedPen;
	CPen *m_pGrayDashedPen;
	CPen *m_pDarkGrayDashedPen;

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

// CSampleEditorDlg dialog

class CSampleEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CSampleEditorDlg)

public:
	CSampleEditorDlg(CWnd* pParent = NULL, CDSample *pSample = NULL);   // standard constructor
	virtual ~CSampleEditorDlg();

	void CopySample(CDSample *pTarget);

// Dialog Data
	enum { IDD = IDD_SAMPLE_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void MoveControls();
	void UpdateSampleView();

	CDSample	*m_pSample;
	CDSample	*m_pOriginalSample;
	CSampleView *m_pSampleView;
	CSoundGen	*m_pSoundGen;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedDelete();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedDeltastart();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedTilt();
};
