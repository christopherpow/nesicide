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

// This is the pattern editor class

#pragma once

#include "SoundGen.h"

enum {TRANSPOSE_DEC_NOTES, TRANSPOSE_INC_NOTES, TRANSPOSE_DEC_OCTAVES, TRANSPOSE_INC_OCTAVES};

// Graphical layout of pattern editor

// Top header (channel names etc)
const int HEADER_HEIGHT = 36 /*+ 16*/;
const int HEADER_CHAN_START = /*16*/ 0;
const int HEADER_CHAN_HEIGHT = 36;
// Row column
const int ROW_COL_WIDTH = 32;
const int ROW_HEIGHT = 12;		// 12
// Patterns
const int CHANNEL_WIDTH = 107;
const int COLUMN_SPACING = 4;
const int CHAR_WIDTH = 10;

// Class used by clipboard
class CPatternClipData
{
public:
	CPatternClipData() : pPattern(NULL), Size(0) {
		memset(&ClipInfo, 0, sizeof(ClipInfo));
	}
	CPatternClipData(int Channels, int Rows) {
		memset(&ClipInfo, 0, sizeof(ClipInfo));
		Size = Channels * Rows;
		pPattern = new stChanNote[Size];
	}
	virtual ~CPatternClipData() {
		SAFE_RELEASE_ARRAY(pPattern);
	}

	SIZE_T GetAllocSize() const;	// Get clip data size in bytes
	void ToMem(HGLOBAL hMem);		// Copy structures to memory
	void FromMem(HGLOBAL hMem);		// Copy structures from memory
	
	stChanNote *GetPattern(int Channel, int Row);

public:
	struct {
		int Channels;			// Number of channels
		int Rows;				// Number of rows
		int StartColumn;		// Start column in first channel
		int EndColumn;			// End column in last channel
		struct {				// OLE drag and drop info
			int ChanOffset;
			int RowOffset;
		} OleInfo;
	} ClipInfo;

	stChanNote *pPattern;		// Pattern data
	int Size;					// Pattern data size, in rows * columns
};

// Row color cache
struct RowColorInfo_t {
	COLORREF Note;
	COLORREF Instrument;
	COLORREF Volume;
	COLORREF Effect;
	COLORREF Back;
	COLORREF Shaded;
};

// Cursor position
class CCursorPos {
public:
	CCursorPos();
	CCursorPos(int Row, int Channel, int Column);
	const CCursorPos& operator=(const CCursorPos &pos);
	bool Invalid() const;

public:
	int m_iRow;
	int m_iColumn;
	int m_iChannel;
};

// Selection
class CSelection {
public:
	int GetRowStart() const;
	int GetRowEnd() const;
	int GetColStart() const;
	int GetColEnd() const;
	int GetChanStart() const;
	int GetChanEnd() const;
	bool IsWithin(CCursorPos pos) const;
	bool IsSingleChannel() const;

	void SetStart(CCursorPos pos);
	void SetEnd(CCursorPos pos);

public:
	CCursorPos m_cpStart;
	CCursorPos m_cpEnd;
};

// External classes
class CFamiTrackerDoc;
class CFamiTrackerView;

// CPatternView
class CPatternView {
public:
	static const unsigned int ROW_PLAY_COLOR = 0x400050;

	// Public functions
public:
	CPatternView();
	~CPatternView();

	bool InitView(UINT ClipBoard);
	void ApplyColorScheme();
	void SetDocument(CFamiTrackerDoc *pDoc, CFamiTrackerView *pView);
	void SetWindowSize(int width, int height);
	void Reset();

	void Invalidate(bool bEntire);
	void Modified();
	void AdjustCursor();
	void AdjustCursorChannel();
	bool FullErase() const;

	void DrawScreen(CDC *pDC, CFamiTrackerView *pView);
	void CreateBackground(CDC *pDC, bool bForce);
	void UpdateScreen(CDC *pDC);
	void DrawHeader(CDC *pDC);
	void DrawMeters(CDC *pDC);
	void FastScroll(CDC *pDC, int Rows);

	void SetDPCMState(stDPCMState State);

	void SetFocus(bool bFocus);

	void PaintEditor();
	CRect GetActiveRect() const;

	// Cursor
	void MoveDown(int Step);
	void MoveUp(int Step);
	void MoveLeft();
	void MoveRight();
	void MoveToTop();
	void MoveToBottom();
	void MovePageUp();
	void MovePageDown();
	void NextChannel();
	void PreviousChannel();
	void FirstChannel();
	void LastChannel();
	void MoveChannelLeft();
	void MoveChannelRight();
	void OnHomeKey();
	void OnEndKey();

	void MoveToRow(int Row);
	void MoveToFrame(int Frame);
	void MoveToChannel(int Channel);
	void MoveToColumn(int Column);
	void NextFrame();
	void PreviousFrame();

	void ScrollLeft();
	void ScrollRight();
	void ScrollNextChannel();
	void ScrollPreviousChannel();

	bool StepRow();
	bool StepFrame();
	void JumpToRow(int Row);
	void JumpToFrame(int Frame);

	int GetFrame() const;
	int GetChannel() const;
	int GetRow() const;
	int GetColumn() const;
	int GetPlayFrame() const;
	int GetPlayRow() const;

	// Mouse
	void OnMouseDown(CPoint point);
	void OnMouseUp(CPoint point);
	bool OnMouseHover(UINT nFlags, CPoint point);
	bool OnMouseNcMove();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseDblClk(CPoint point);
	void OnMouseScroll(int Delta);
	void OnMouseRDown(CPoint point);

	bool CancelDragging();

	// Edit: Copy & paste, selection
	CPatternClipData *CopyEntire();
	CPatternClipData *Copy();
	void Cut();
	void PasteEntire(CPatternClipData *pClipData);
	void Paste(CPatternClipData *pClipData);
	void PasteMix(CPatternClipData *pClipData);
	void DeleteSelectionRows(CSelection &selection);
	void DeleteSelection(CSelection &selection);
	void Delete();
	void RemoveSelectedNotes();

	bool IsSelecting() const;
	void SelectAllChannel();
	void SelectAll();

	void Interpolate();
	void Reverse();
	void ReplaceInstrument(int Instrument);

	void ClearSelection();

	void GetVolumeColumn(CString &str) const;

	// Various
	void Transpose(int Type);
	void ScrollValues(int Type);

	void SetHighlight(int Rows, int SecondRows);
	void SetFollowMove(bool bEnable);

	bool IsPlayCursorVisible() const;

	int GetChannelAtPoint(int PointX) const;

	// Scrolling
	void AutoScroll(CPoint point, UINT nFlags);
	bool ScrollTimer();
	void OnVScroll(UINT nSBCode, UINT nPos);
	void OnHScroll(UINT nSBCode, UINT nPos);

	void SetBlockStart();
	void SetBlockEnd();

	int GetChannelWidth(int i) const { return m_iChannelWidths[i]; }
	int GetVisibleWidth() const { return m_iVisibleWidth; }

	CSelection GetSelection() const;
	void SetSelection(CSelection &selection);

	void DragPaste(CPatternClipData *pClipData, CSelection *pDragTarget, bool bMix);

	void ExpandPattern();
	void ShrinkPattern();

#ifdef _DEBUG
	void DrawLog(CDC *pDC);
#endif

	// OLE support
	void BeginDrag(CPatternClipData *pClipData);
	void EndDrag();
	bool PerformDrop(CPatternClipData *pClipData, bool bCopy, bool bCopyMix);
	void UpdateDrag(CPoint point);

	// Private functions
private:
	int  GetRowAtPoint(int PointY) const;
	int  GetColumnAtPoint(int PointX) const;
	bool IsColumnSelected(int Column, int Channel) const;
	int  GetSelectColumn(int Column) const;
	int  GetRealStartColumn(int Column) const;
	int  GetRealEndColumn(int Column) const;
	bool IsSingleChannelSelection() const;
	bool IsInSelection(CCursorPos &Point) const;

	int GetChannelColumns(int Channel) const;

	CCursorPos GetCursorAtPoint(CPoint point) const;

	void ClearRow(CDC *pDC, int Line);
	void DrawPatternArea(CDC *pDC);
	void DrawRow(CDC *pDC, int Row, int Line, int Frame, bool bPreview);

	void DrawCell(int PosX, int Column, int Channel, bool bInvert, stChanNote *pNoteData, CDC *pDC, RowColorInfo_t *pColorInfo);
	void DrawChar(int x, int y, TCHAR c, COLORREF Color, CDC *pDC);
	void DrawNoteColumn(unsigned int PosX, unsigned int PosY, CDC *pDC);
	void DrawInstrumentColumn(unsigned int PosX, unsigned int PosY, CDC *pDC);

	void UpdateVerticalScroll();
	void UpdateHorizontalScroll();

	void IncreaseEffectColumn(int Channel);
	void DecreaseEffectColumn(int Channel);

	// Head
	void DrawChannelNames(CDC *pDC);
	void DrawUnbufferedArea(CDC *pDC);

	// Other
	void DrawChannelStates(CDC *pDC);
	void DrawRegisters(CDC *pDC);

	// Selection routines
	void ResetSelection();
	void SetSelectionStart();
	void UpdateSelection();

	// Other
	int GetCurrentPatternLength(int Frame) const;
	void CalcLayout();

	// Keys
	bool IsShiftPressed() const;
	bool IsControlPressed() const;

private:
	static LPCTSTR DEFAULT_HEADER_FONT;
	static const int DEFAULT_FONT_SIZE;
	static const int DEFAULT_HEADER_FONT_SIZE;

	// Variables
private:
	CFamiTrackerDoc	 *m_pDocument;
	CFamiTrackerView *m_pView;

	UINT m_iClipBoard;

	// Window
	int m_iWinWidth, m_iWinHeight;		// Window height & width
	int	m_iVisibleRows;					// Number of visible rows on screen
	int m_iVisibleFullRows;				// Number of full visible rows on screen

	int m_iPatternHeight;				// Full size of the allocated pattern area
//	int	m_iPatternWidth;

	// Edit cursor
	CCursorPos m_cpCursorPos;			// Cursor position

	int m_iMiddleRow;					// The row in the middle of the editor

	int	m_iCurrentFrame;
	int m_iPatternLength;
	int	m_iPrevPatternLength;
	int	m_iNextPatternLength;
	int m_iPlayPatternLength;

	int m_iChannels;
	int m_iChannelWidths[MAX_CHANNELS];
	int m_iColumns[MAX_CHANNELS];

	// Play cursor
	int m_iPlayRow;
	int m_iPlayFrame;
	bool m_bForcePlayRowUpdate;

	bool m_bFollowMode;					// Follow mode enable/disable

	// Drawing
	int m_iDrawCursorRow;
	int m_iDrawMiddleRow;
	int m_iDrawFrame;
	int m_iFirstChannel;
	int m_iChannelsVisible;
	int m_iWholeChannelsVisible;
	int m_iHighlight;
	int m_iHighlightSecond;
	int m_iPatternWidth;
	int m_iVisibleWidth;
	int m_iRowHeight;

	bool m_bForceFullRedraw;
	bool m_bDrawEntire;

	int	m_iActualLengths[MAX_FRAMES];
	int	m_iNextPreviewFrame[MAX_FRAMES];

	bool m_bHasFocus;
	bool m_bUpdated;
	bool m_bErasedBg;

	int m_iPatternFontSize;

	// Colors
	COLORREF m_colEmptyBg;
	COLORREF m_colSeparator;
	COLORREF m_colHead1, m_colHead2, m_colHead3, m_colHead4;

	// Meters and DPCM
	stDPCMState m_DPCMState;

	int	m_iMouseHoverChan;
	int m_iMouseHoverEffArrow;
	//int m_iMouseClickChan;

	// Selection
	bool m_bSelecting;
	bool m_bCurrentlySelecting;
	bool m_bDragStart;
	bool m_bDragging;
	bool m_bSelectedAll;

	CSelection m_selection;
	CCursorPos m_cpSelCursor;

	// Drag
	CSelection m_selDrag;
	CCursorPos m_cpDragPoint;

	bool m_bSelectionInvalid;
	bool m_bFullRowSelect;

	CPoint m_ptSelStartPoint;

	// Numbers of pixels until selection is initiated
	int m_iDragThresholdX;
	int m_iDragThresholdY;

	// OLE support
	int m_iDragChannels;
	int m_iDragRows;
	int m_iDragStartCol;
	int m_iDragEndCol;

	int m_iDragOffsetChannel;
	int m_iDragOffsetRow;

	// GDI objects
	CDC		*m_pPatternDC;
	CBitmap *m_pPatternBmp;
	CBitmap	m_bmpCache, *m_pOldCacheBmp;
	CFont	m_fontHeader;
	CFont	m_fontPattern;
	CFont	m_fontCourierNew;
	CDC		*m_pHeaderDC;
	CBitmap	*m_pHeaderBmp;

	// Scrolling
	CPoint	m_ptScrollMousePos;
	UINT	m_nScrollFlags;
	int		m_iScrolling;
	int		m_iCurrentHScrollPos;

	// Benchmarking
	int m_iRedraws, m_iFastRedraws, m_iErases, m_iBuffers, m_iCharsDrawn;
};
