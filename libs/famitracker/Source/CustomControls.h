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

class CMainFrame;

// The instrument list
class CInstrumentList : public CListCtrl {
   Q_OBJECT
public:
	DECLARE_DYNAMIC(CInstrumentList)
protected:
	DECLARE_MESSAGE_MAP()
public:
	CInstrumentList(CMainFrame *pMainFrame);
private:
	CMainFrame *m_pMainFrame;
public:
	afx_msg void OnContextMenu(CWnd*, CPoint);
	afx_msg void OnAddInstrument();
};

// Edit controls that can be enabled by double clicking
class CLockedEdit : public CEdit {
   Q_OBJECT
   // Qt interfaces
protected:
   void mouseDoubleClickEvent(QMouseEvent *event);
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);   
public slots:
   bool eventFilter(QObject *object, QEvent *event);
public:
	DECLARE_DYNAMIC(CLockedEdit)
protected:
	DECLARE_MESSAGE_MAP()
public:
	CLockedEdit() : m_bUpdate(false) {
	};
	bool IsEditable() const;
	bool Update();
	int GetValue() const;
private:
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnSetFocus(CWnd* pOldWnd);
	void OnKillFocus(CWnd* pNewWnd);
private:
	bool m_bUpdate;
	int m_iValue;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileAddsong();
};


// Edit controls that displays a banner when empty
class CBannerEdit : public CEdit {
   Q_OBJECT
   // Qt interfaces
protected:
   void paintEvent(QPaintEvent *event);
   void focusInEvent(QFocusEvent *);
   void focusOutEvent(QFocusEvent *);   
public slots:
   bool eventFilter(QObject *object, QEvent *event);
public:
	DECLARE_DYNAMIC(CBannerEdit)
protected:
	DECLARE_MESSAGE_MAP()
public:
	CBannerEdit(CString txt) : CEdit(), m_strText(txt) {
   };
protected:
	CString m_strText;
protected:
	static const TCHAR BANNER_FONT[];
	static const COLORREF BANNER_COLOR;
public:
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
