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

#include "MainFrm.h"

class CFamiTrackerDoc;
class CFamiTrackerView;

// CFrameEditor

namespace Ui {
class CFrameEditor;
}

class CFrameEditor : public CWnd
{
   Q_OBJECT
	DECLARE_DYNAMIC(CFrameEditor)
public:
	CFrameEditor(CMainFrame *pMainFrm);
	virtual ~CFrameEditor();

	void AssignDocument(CFamiTrackerDoc *pDoc, CFamiTrackerView *pView);
	void EnableInput();
	bool InputEnabled() const;

//	bool Translate(HWND hWnd, MSG *pMsg) const;

protected:
   void paintEvent(QPaintEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseDoubleClickEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *event);
   void keyPressEvent(QKeyEvent *event);
   void resizeEvent(QResizeEvent *event);
   void timerEvent(QTimerEvent *event);
   void focusInEvent(QFocusEvent *event) { event->ignore(); }
   void focusOutEvent(QFocusEvent *);
   void leaveEvent(QEvent *);
   
private:
   Ui::CFrameEditor *ui;
	void CreateGdiObjects();

public:
	static const int FIXED_WIDTH = 51;			// The left-most column width
	static const int FRAME_ITEM_WIDTH = 20;		// Channel width 
	static const int ROW_HEIGHT = 15;			// Height of rows
	static const int TOP_OFFSET = 3;			// Top offset

	static const TCHAR DEFAULT_FONT[];
	
private:
	// GDI objects
	CFont	m_Font;
	CBitmap m_bmpBack;
	CDC		m_dcBack;

	CMainFrame		 *m_pMainFrame;
	CFamiTrackerDoc  *m_pDocument;
	CFamiTrackerView *m_pView;

	int m_iHiglightLine;
	int m_iFirstChannel;
	int m_iCursorPos;
	int m_iNewPattern;
	bool m_bInputEnable;
	bool m_bCursor;
	int m_iCopiedValues[MAX_CHANNELS];
	int m_iFramesVisible;

//	HACCEL m_hAccel;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFrameCopy();
	afx_msg void OnFramePaste();
	afx_msg void OnModuleInsertFrame();
	afx_msg void OnModuleRemoveFrame();
	afx_msg void OnModuleDuplicateFrame();
	afx_msg void OnModuleDuplicateFramePatterns();
	afx_msg void OnModuleMoveFrameDown();
	afx_msg void OnModuleMoveFrameUp();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   
public slots:
   void updateViews(long hint);
private slots:
   void on_verticalScrollBar_actionTriggered(int action);
   void on_horizontalScrollBar_actionTriggered(int action);
public: 
   // Reimplements of CWnd functionality that unfortunately can't easily be moved there...
   void SetScrollRange(
      int nBar,
      int nMinPos,
      int nMaxPos,
      BOOL bRedraw = TRUE 
   );
   int SetScrollPos(
      int nBar,
      int nPos,
      BOOL bRedraw = TRUE 
   );
};


