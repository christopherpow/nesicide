/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
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

#include "Action.h"

// Pattern commands
class CPatternAction : public CAction
{
public:
	enum ACTIONS
	{
		ACT_EDIT_NOTE,
		ACT_INSERT_ROW,
		ACT_DELETE_ROW,
		ACT_INCREASE,
		ACT_DECREASE,
		ACT_EDIT_PASTE,
		ACT_EDIT_PASTE_MIX,
		ACT_EDIT_DELETE,
		ACT_EDIT_DELETE_ROWS,
		ACT_TRANSPOSE,
		ACT_SCROLL_VALUES,
		ACT_INTERPOLATE,
		ACT_REVERSE,
		ACT_REPLACE_INSTRUMENT,
		ACT_DRAG_AND_DROP,
		ACT_PATTERN_LENGTH,
		ACT_EXPAND_PATTERN,
		ACT_SHRINK_PATTERN,
		ACT_EXPAND_COLUMNS,
		ACT_SHRINK_COLUMNS
	};

public:
	CPatternAction(int iAction);
	virtual ~CPatternAction();

	bool SaveState(CMainFrame *pMainFrm);
	void Undo(CMainFrame *pMainFrm);
	void Redo(CMainFrame *pMainFrm);

public:
	void SetNote(stChanNote &Note);
	void SetDelete(bool PullUp, bool Back);
	void SetPaste(CPatternClipData *pClipData);
	void SetTranspose(transpose_t Mode);
	void SetScroll(int Scroll);
	void SetInstrument(int Instrument);
	void SetDragAndDrop(const CPatternClipData *pClipData, bool bDelete, bool bMix, const CSelection *pDragTarget);
	void SetPatternLength(int Length);
	void Update(CMainFrame *pMainFrm);
	void SetClickedChannel(int Channel);
	void UpdateCursor(CPatternEditor *pPatternEditor);

private:
	void SaveEntire(CPatternEditor *pPatternEditor);
	void RestoreEntire(CPatternEditor *pPatternEditor);
	void IncreaseRowAction(CFamiTrackerDoc *pDoc);
	void DecreaseRowAction(CFamiTrackerDoc *pDoc);

private:
	stChanNote m_NewNote;
	stChanNote m_OldNote;

	int m_iUndoTrack;
	int m_iUndoFrame;
	int m_iUndoChannel;
	int m_iUndoRow;
	int m_iUndoColumn;
	int m_iUndoColumnCount;

	int m_iRedoTrack;
	int m_iRedoFrame;
	int m_iRedoChannel;
	int m_iRedoRow;
	int m_iRedoColumn;

	int m_iPatternLen;
	int m_iActualPatternLen;

	int m_iNewPatternLen;
	int m_iOldPatternLen;

	bool m_bPullUp;
	bool m_bBack;

	const CPatternClipData *m_pClipData;
	CPatternClipData *m_pUndoClipData;
	
	bool m_bSelecting;
	CSelection m_selection;

	transpose_t m_iTransposeMode;
	int m_iScrollValue;
	int m_iInstrument;

	bool m_bDragDelete;
	bool m_bDragMix;
	CSelection m_dragTarget;

	int m_iClickedChannel;

};
