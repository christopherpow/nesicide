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
** Any permitted reproduction of these routin, in whole or in part,
** must bear this legend.
*/

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrame.h"
#include "PatternEditor.h"
#include "Action.h"

// CPatternAction /////////////////////////////////////////////////////////////////
//
// Undo/redo commands for pattern editor
//

// TODO: optimize the cases where it's not necessary to store the whole pattern

CPatternAction::CPatternAction(int iAction) : 
	CAction(iAction), 
	m_pClipData(NULL), 
	m_pUndoClipData(NULL)
{
}

CPatternAction::~CPatternAction()
{
	SAFE_RELEASE(m_pClipData);
	SAFE_RELEASE(m_pUndoClipData);
}

void CPatternAction::SetNote(stChanNote &Note)
{
	m_NewNote = Note;
}

void CPatternAction::SetDelete(bool PullUp, bool Back)
{
	m_bPullUp = PullUp;
	m_bBack = Back;
}

void CPatternAction::SetPaste(stClipData *pClipData, int Mode)
{
	m_pClipData = new stClipData;
	memcpy(m_pClipData, pClipData, sizeof(stClipData));
	m_iPasteMode = Mode;
}

void CPatternAction::SetSelection(CSelection *pSelection)
{
//	m_pSelection = new CSelection();
//	memcpy(m_pSelection, pSelection, sizeof(CSelection));
}

void CPatternAction::SetTranspose(int Mode)
{
	m_iTransposeMode = Mode;
}

void CPatternAction::SetScroll(int Scroll)
{
	m_iScrollValue = Scroll;
}

void CPatternAction::SetInstrument(int Instrument)
{
	m_iInstrument = Instrument;
}

void CPatternAction::SetDragAndDrop(stClipData *pClipData, bool bDelete, bool bMix, CSelection *pDragTarget)
{
	m_pClipData = new stClipData;
	memcpy(m_pClipData, pClipData, sizeof(stClipData));

	m_bDragDelete	= bDelete;
	m_bDragMix		= bMix;
	m_dragTarget	= *pDragTarget;
}

void CPatternAction::SetPatternLength(int Length)
{
	m_iNewPatternLen = Length;
}

void CPatternAction::SaveEntire(CPatternView *pPatternView)
{
	// (avoid when possible)
	m_pUndoClipData = new stClipData;
	pPatternView->CopyEntire(m_pUndoClipData);
}

void CPatternAction::SaveSelection(CPatternView *pPatternView)
{
	// Save pattern selection
	m_pUndoClipData = new stClipData;
	pPatternView->Copy(m_pUndoClipData);
}

void CPatternAction::RestoreEntire(CPatternView *pPatternView)
{
	pPatternView->PasteEntire(m_pUndoClipData);
}

void CPatternAction::RestoreSelection(CPatternView *pPatternView)
{
	pPatternView->Paste(m_pUndoClipData);
}

void CPatternAction::IncreaseRowAction(CFamiTrackerDoc *pDoc)
{
	switch (m_iUndoColumn) {
		case C_INSTRUMENT1:
		case C_INSTRUMENT2: 
			pDoc->IncreaseInstrument(m_iUndoFrame, m_iUndoChannel, m_iUndoRow); 	
			break;
		case C_VOLUME: 
			pDoc->IncreaseVolume(m_iUndoFrame, m_iUndoChannel, m_iUndoRow);	
			break;
		case C_EFF_NUM: case C_EFF_PARAM1: case C_EFF_PARAM2: 
			pDoc->IncreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 0);
			break;
		case C_EFF2_NUM: case C_EFF2_PARAM1: case C_EFF2_PARAM2: 
			pDoc->IncreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 1);
			break;
		case C_EFF3_NUM: case C_EFF3_PARAM1: case C_EFF3_PARAM2: 
			pDoc->IncreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 2);
			break;
		case C_EFF4_NUM: case C_EFF4_PARAM1: case C_EFF4_PARAM2: 
			pDoc->IncreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 3);
			break;
	}
}

void CPatternAction::DecreaseRowAction(CFamiTrackerDoc *pDoc)
{
	switch (m_iUndoColumn) {
		case C_INSTRUMENT1:
		case C_INSTRUMENT2:
			pDoc->DecreaseInstrument(m_iUndoFrame, m_iUndoChannel, m_iUndoRow); 	
			break;
		case C_VOLUME: 
			pDoc->DecreaseVolume(m_iUndoFrame, m_iUndoChannel, m_iUndoRow);	
			break;
		case C_EFF_NUM: case C_EFF_PARAM1: case C_EFF_PARAM2: 
			pDoc->DecreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 0);
			break;
		case C_EFF2_NUM: case C_EFF2_PARAM1: case C_EFF2_PARAM2: 
			pDoc->DecreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 1);
			break;
		case C_EFF3_NUM: case C_EFF3_PARAM1: case C_EFF3_PARAM2: 
			pDoc->DecreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 2); 
			break;
		case C_EFF4_NUM: case C_EFF4_PARAM1: case C_EFF4_PARAM2: 					
			pDoc->DecreaseEffect(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, 3); 
			break;
	}
}

bool CPatternAction::SaveState(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternView *pPatternView = pView->GetPatternView();

	m_iUndoFrame	= pPatternView->GetFrame();
	m_iUndoChannel  = pPatternView->GetChannel();
	m_iUndoRow		= pPatternView->GetRow();
	m_iUndoColumn   = pPatternView->GetColumn();

	m_iUndoColumnCount = pDoc->GetEffColumns(m_iUndoChannel);

	m_selection = pPatternView->GetSelection();

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			// Edit note
			pDoc->GetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_NewNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DELETE_ROW:
			// Delete row
			if (m_bBack && m_iUndoRow == 0)
				return false;
			pDoc->GetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), &m_OldNote);
			pDoc->ClearRowField(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), m_iUndoColumn);
			if (m_bPullUp)
				pDoc->PullUp(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0));
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INSERT_ROW:
			// Insert empty row
			pDoc->GetNoteData(m_iUndoFrame, m_iUndoChannel, pDoc->GetPatternLength() - 1, &m_OldNote);
			pDoc->InsertRow(m_iUndoFrame, m_iUndoChannel, m_iUndoRow);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE:
			// Paste
			m_pUndoClipData = new stClipData;
			pPatternView->CopyEntire(m_pUndoClipData);
			if (m_iPasteMode == PASTE_MODE_MIX)
				pPatternView->PasteMix(m_pClipData);
			else
				pPatternView->Paste(m_pClipData);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE:
			// Delete selection
			SaveEntire(pPatternView);
			pPatternView->DeleteSelection(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE_ROWS:
			// Delete and pull up selection
			SaveEntire(pPatternView);
			pPatternView->DeleteSelectionRows(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_TRANSPOSE:
			// Transpose notes
			SaveEntire(pPatternView);
			pPatternView->Transpose(m_iTransposeMode);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SCROLL_VALUES:
			// Scroll pattern values
			SaveEntire(pPatternView);
			pPatternView->ScrollValues(m_iScrollValue);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INTERPOLATE:
			// Interpolate pattern
			if (!pPatternView->IsSelecting())
				return false;
			SaveEntire(pPatternView);
			pPatternView->Interpolate();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REVERSE:
			// Reverse pattern
			if (!pPatternView->IsSelecting())
				return false;
			SaveEntire(pPatternView);
			pPatternView->Reverse();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REPLACE_INSTRUMENT:
			// Replace instruments
			if (!pPatternView->IsSelecting())
				return false;
			SaveEntire(pPatternView);
			pPatternView->ReplaceInstrument(m_iInstrument);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INCREASE:
			// Increase action
			pDoc->GetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			IncreaseRowAction(pDoc);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DECREASE:
			// Decrease action
			pDoc->GetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			DecreaseRowAction(pDoc);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP:
			// Drag'n'drop
			SaveEntire(pPatternView);
			if (m_bDragDelete)
				pPatternView->Delete();
			pPatternView->DragPaste(m_pClipData, &m_dragTarget, m_bDragMix);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PATTERN_LENGTH:
			m_iOldPatternLen = pDoc->GetPatternLength();
			pDoc->SetPatternLength(m_iNewPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EXPAND_PATTERN:
			SaveEntire(pPatternView);
			pPatternView->ExpandPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SHRINK_PATTERN:
			SaveEntire(pPatternView);
			pPatternView->ShrinkPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
	}

	return true;
}

void CPatternAction::Undo(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternView *pPatternView = pView->GetPatternView();

	m_iRedoFrame	= pPatternView->GetFrame();
	m_iRedoChannel  = pPatternView->GetChannel();
	m_iRedoRow		= pPatternView->GetRow();
	m_iRedoColumn   = pPatternView->GetColumn();
	m_iRedoColumnCount = pDoc->GetEffColumns(m_iRedoChannel);

	if (m_iRedoColumnCount != m_iUndoColumnCount)
		pDoc->SetEffColumns(m_iUndoChannel, m_iUndoColumnCount);

	pPatternView->MoveToFrame(m_iUndoFrame);
	pPatternView->MoveToChannel(m_iUndoChannel);
	pPatternView->MoveToRow(m_iUndoRow);
	pPatternView->MoveToColumn(m_iUndoColumn);

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DELETE_ROW:
			if (m_bPullUp)
				pDoc->InsertRow(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0));
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INSERT_ROW:
			pDoc->PullUp(m_iUndoFrame, m_iUndoChannel, m_iUndoRow);
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, pDoc->GetPatternLength() - 1, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE:
		case ACT_EDIT_DELETE:
		case ACT_EDIT_DELETE_ROWS:
		case ACT_TRANSPOSE:
		case ACT_SCROLL_VALUES:
		case ACT_INTERPOLATE:
		case ACT_REVERSE:
		case ACT_REPLACE_INSTRUMENT:
		case ACT_EXPAND_PATTERN:
		case ACT_SHRINK_PATTERN:
			RestoreEntire(pPatternView);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INCREASE:
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DECREASE:
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP:
			RestoreEntire(pPatternView);
			pPatternView->SetSelection(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iOldPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
#ifdef _DEBUG_
		default:
			AfxMessageBox("Undo not implemented");
#endif
	}
}

void CPatternAction::Redo(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternView *pPatternView = pView->GetPatternView();

	pPatternView->MoveToFrame(m_iUndoFrame);
	pPatternView->MoveToChannel(m_iUndoChannel);
	pPatternView->MoveToRow(m_iUndoRow);
	pPatternView->MoveToColumn(m_iUndoColumn);

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			pDoc->SetNoteData(m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_NewNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DELETE_ROW:
			pDoc->ClearRowField(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), m_iUndoColumn);
			if (m_bPullUp)
				pDoc->PullUp(m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0));
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INSERT_ROW:
			pDoc->InsertRow(m_iUndoFrame, m_iUndoChannel, m_iUndoRow);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE:
			if (m_iPasteMode == PASTE_MODE_MIX)
				pPatternView->PasteMix(m_pClipData);
			else
				pPatternView->Paste(m_pClipData);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE:
			pPatternView->SetSelection(m_selection);
			pPatternView->DeleteSelection(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE_ROWS:
			// Delete and pull up selection
			pPatternView->DeleteSelectionRows(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_TRANSPOSE:
			pPatternView->SetSelection(m_selection);
			pPatternView->Transpose(m_iTransposeMode);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SCROLL_VALUES:
			pPatternView->SetSelection(m_selection);
			pPatternView->ScrollValues(m_iScrollValue);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INTERPOLATE:
			pPatternView->SetSelection(m_selection);
			pPatternView->Interpolate();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REVERSE:
			pPatternView->SetSelection(m_selection);
			pPatternView->Reverse();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REPLACE_INSTRUMENT:
			pPatternView->SetSelection(m_selection);
			pPatternView->ReplaceInstrument(m_iInstrument);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INCREASE:
			IncreaseRowAction(pDoc);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DECREASE:
			DecreaseRowAction(pDoc);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP:
			pPatternView->SetSelection(m_selection);
			if (m_bDragDelete)
				pPatternView->Delete();
			pPatternView->DragPaste(m_pClipData, &m_dragTarget, m_bDragMix);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iNewPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_EXPAND_PATTERN:
			pPatternView->ExpandPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SHRINK_PATTERN:
			pPatternView->ShrinkPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
#ifdef _DEBUG_
		default:
			AfxMessageBox("Redo not implemented");
#endif
	}

	pPatternView->MoveToFrame(m_iRedoFrame);
	pPatternView->MoveToChannel(m_iRedoChannel);
	pPatternView->MoveToRow(m_iRedoRow);
	pPatternView->MoveToColumn(m_iRedoColumn);
}

void CPatternAction::Update(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDoc = pView->GetDocument();

	switch (m_iAction) {
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iNewPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
	}
}
