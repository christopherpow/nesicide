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
** Any permitted reproduction of these routin, in whole or in part,
** must bear this legend.
*/

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrm.h"
#include "PatternEditor.h"
#include "PatternAction.h"

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

void CPatternAction::SetPaste(CPatternClipData *pClipData)
{
	m_pClipData = pClipData;
}

void CPatternAction::SetTranspose(transpose_t Mode)
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

void CPatternAction::SetDragAndDrop(const CPatternClipData *pClipData, bool bDelete, bool bMix, const CSelection *pDragTarget)
{
	m_pClipData		= pClipData;
	m_bDragDelete	= bDelete;
	m_bDragMix		= bMix;
	m_dragTarget	= *pDragTarget;
}

void CPatternAction::SetPatternLength(int Length)
{
	m_iNewPatternLen = Length;
}

void CPatternAction::SetClickedChannel(int Channel)
{
	m_iClickedChannel = Channel;
}

void CPatternAction::SaveEntire(CPatternEditor *pPatternEditor)
{
	// (avoid when possible)
	m_pUndoClipData = pPatternEditor->CopyEntire();
}

void CPatternAction::RestoreEntire(CPatternEditor *pPatternEditor)
{
	pPatternEditor->PasteEntire(m_pUndoClipData);
}

void CPatternAction::IncreaseRowAction(CFamiTrackerDoc *pDoc)
{
	stChanNote Note;
	bool bUpdate = false;
	
	pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &Note);

	switch (m_iUndoColumn) {
		case C_INSTRUMENT1:
		case C_INSTRUMENT2: 
			if (Note.Instrument < MAX_INSTRUMENTS) {
				++Note.Instrument;
				bUpdate = true;
			}
			break;
		case C_VOLUME: 
			if (Note.Vol < 0x10) {
				++Note.Vol;
				bUpdate = true;
			}
			break;
		case C_EFF_NUM: case C_EFF_PARAM1: case C_EFF_PARAM2: 
			if (Note.EffParam[0] != 255) {
				++Note.EffParam[0];
				bUpdate = true;
			}
			break;
		case C_EFF2_NUM: case C_EFF2_PARAM1: case C_EFF2_PARAM2: 
			if (Note.EffParam[1] != 255) {
				++Note.EffParam[1];
				bUpdate = true;
			}
			break;
		case C_EFF3_NUM: case C_EFF3_PARAM1: case C_EFF3_PARAM2: 
			if (Note.EffParam[2] != 255) {
				++Note.EffParam[2];
				bUpdate = true;
			}
			break;
		case C_EFF4_NUM: case C_EFF4_PARAM1: case C_EFF4_PARAM2: 
			if (Note.EffParam[3] != 255) {
				++Note.EffParam[3];
				bUpdate = true;
			}
			break;
	}

	if (bUpdate)
		pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &Note);
}

void CPatternAction::DecreaseRowAction(CFamiTrackerDoc *pDoc)
{
	stChanNote Note;
	bool bUpdate = false;
	
	pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &Note);

	switch (m_iUndoColumn) {
		case C_INSTRUMENT1:
		case C_INSTRUMENT2:
			if (Note.Instrument > 0) {
				--Note.Instrument;
				bUpdate = true;
			}
			break;
		case C_VOLUME: 
			if (Note.Vol > 0) {
				--Note.Vol;
				bUpdate = true;
			}
			break;
		case C_EFF_NUM: case C_EFF_PARAM1: case C_EFF_PARAM2: 
			if (Note.EffParam[0] > 0) {
				--Note.EffParam[0];
				bUpdate = true;
			}
			break;
		case C_EFF2_NUM: case C_EFF2_PARAM1: case C_EFF2_PARAM2: 
			if (Note.EffParam[1] > 0) {
				--Note.EffParam[1];
				bUpdate = true;
			}
			break;
		case C_EFF3_NUM: case C_EFF3_PARAM1: case C_EFF3_PARAM2: 
			if (Note.EffParam[2] > 0) {
				--Note.EffParam[2];
				bUpdate = true;
			}
			break;
		case C_EFF4_NUM: case C_EFF4_PARAM1: case C_EFF4_PARAM2: 					
			if (Note.EffParam[3] > 0) {
				--Note.EffParam[3];
				bUpdate = true;
			}
			break;
	}

	if (bUpdate)
		pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &Note);
}

bool CPatternAction::SaveState(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(pMainFrm->GetActiveView());
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternEditor *pPatternEditor = pView->GetPatternEditor();

	// Save undo cursor position
	m_iUndoTrack	= pMainFrm->GetSelectedTrack();
	m_iUndoFrame	= pPatternEditor->GetFrame();
	m_iUndoChannel  = pPatternEditor->GetChannel();
	m_iUndoRow		= pPatternEditor->GetRow();
	m_iUndoColumn   = pPatternEditor->GetColumn();

	m_iRedoFrame	= pPatternEditor->GetFrame();
	m_iRedoChannel  = pPatternEditor->GetChannel();
	m_iRedoRow		= pPatternEditor->GetRow();
	m_iRedoColumn   = pPatternEditor->GetColumn();

	m_bSelecting = pPatternEditor->IsSelecting();
	m_selection = pPatternEditor->GetSelection();

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			// Edit note
			pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			break;
		case ACT_DELETE_ROW:
			// Delete row
			if (m_bBack && m_iUndoRow == 0)
				return false;
			pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), &m_OldNote);
			break;
		case ACT_INSERT_ROW:
			// Insert empty row
			pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, pDoc->GetPatternLength(m_iUndoTrack) - 1, &m_OldNote);
			break;
		case ACT_EDIT_PASTE:
			// Paste
			m_pUndoClipData = pPatternEditor->CopyEntire();
			break;
		case ACT_EDIT_PASTE_MIX:
			// Paste and mix
			m_pUndoClipData = pPatternEditor->CopyEntire();
			break;
		case ACT_EDIT_DELETE:
			// Delete selection
			SaveEntire(pPatternEditor);
			break;
		case ACT_EDIT_DELETE_ROWS:
			// Delete and pull up selection
			SaveEntire(pPatternEditor);
			break;
		case ACT_TRANSPOSE:
			// Transpose notes
			SaveEntire(pPatternEditor);
			break;
		case ACT_SCROLL_VALUES:
			// Scroll pattern values
			SaveEntire(pPatternEditor);
			break;
		case ACT_INTERPOLATE:
			// Interpolate pattern
			if (!pPatternEditor->IsSelecting())
				return false;
			SaveEntire(pPatternEditor);
			break;
		case ACT_REVERSE:
			// Reverse pattern
			if (!pPatternEditor->IsSelecting())
				return false;
			SaveEntire(pPatternEditor);
			break;
		case ACT_REPLACE_INSTRUMENT:
			// Replace instruments
			if (!pPatternEditor->IsSelecting())
				return false;
			SaveEntire(pPatternEditor);
			break;
		case ACT_INCREASE:
			// Increase action
			pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			break;
		case ACT_DECREASE:
			// Decrease action
			pDoc->GetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			break;
		case ACT_DRAG_AND_DROP:
			// Drag'n'drop
			SaveEntire(pPatternEditor);
			break;
		case ACT_PATTERN_LENGTH:
			// Change pattern length
			m_iOldPatternLen = pDoc->GetPatternLength(m_iUndoTrack);
			break;
		case ACT_EXPAND_PATTERN:
			// Expand pattern
			SaveEntire(pPatternEditor);
			break;
		case ACT_SHRINK_PATTERN:
			// Shrink pattern
			SaveEntire(pPatternEditor);
			break;
		case ACT_EXPAND_COLUMNS:
			// Add effect column
			m_iUndoColumnCount = pDoc->GetEffColumns(m_iUndoTrack, m_iClickedChannel);
			break;
		case ACT_SHRINK_COLUMNS:
			// Remove effect column
			m_iUndoColumnCount = pDoc->GetEffColumns(m_iUndoTrack, m_iClickedChannel);
			break;
#ifdef _DEBUG_
		default:
			AfxMessageBox(_T("TODO Implement action for this command"));
#endif
	}

	// Redo will perform the action
	Redo(pMainFrm);

	return true;
}

void CPatternAction::Undo(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(pMainFrm->GetActiveView());
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternEditor *pPatternEditor = pView->GetPatternEditor();

	pPatternEditor->MoveToFrame(m_iUndoFrame);
	pPatternEditor->MoveToChannel(m_iUndoChannel);
	pPatternEditor->MoveToRow(m_iUndoRow);
	pPatternEditor->MoveToColumn(m_iUndoColumn);

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DELETE_ROW:
			if (m_bPullUp)
				pDoc->InsertRow(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0));
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INSERT_ROW:
			pDoc->PullUp(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow);
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, pDoc->GetPatternLength(m_iUndoTrack) - 1, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE:
		case ACT_EDIT_PASTE_MIX:
		case ACT_EDIT_DELETE:
		case ACT_EDIT_DELETE_ROWS:
		case ACT_TRANSPOSE:
		case ACT_SCROLL_VALUES:
		case ACT_INTERPOLATE:
		case ACT_REVERSE:
		case ACT_REPLACE_INSTRUMENT:
		case ACT_EXPAND_PATTERN:
		case ACT_SHRINK_PATTERN:
			RestoreEntire(pPatternEditor);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INCREASE:
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DECREASE:
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_OldNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP:
			RestoreEntire(pPatternEditor);
			if (m_bSelecting)
				pPatternEditor->SetSelection(m_selection);
			else
				pPatternEditor->ClearSelection();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iUndoTrack, m_iOldPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_EXPAND_COLUMNS:
		case ACT_SHRINK_COLUMNS:
			pDoc->SetEffColumns(m_iUndoTrack, m_iClickedChannel, m_iUndoColumnCount);
			pDoc->UpdateAllViews(NULL, CHANGED_ERASE);
			break;

#ifdef _DEBUG_
		default:
			AfxMessageBox(_T("TODO Undo for this action is not implemented"));
#endif
	}
}

void CPatternAction::Redo(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(pMainFrm->GetActiveView());
	CFamiTrackerDoc *pDoc = pView->GetDocument();
	CPatternEditor *pPatternEditor = pView->GetPatternEditor();

	pPatternEditor->MoveToFrame(m_iUndoFrame);
	pPatternEditor->MoveToChannel(m_iUndoChannel);
	pPatternEditor->MoveToRow(m_iUndoRow);
	pPatternEditor->MoveToColumn(m_iUndoColumn);

	switch (m_iAction) {
		case ACT_EDIT_NOTE:
			pDoc->SetNoteData(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow, &m_NewNote);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DELETE_ROW:
			pDoc->ClearRowField(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0), m_iUndoColumn);
			if (m_bPullUp)
				pDoc->PullUp(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow - (m_bBack ? 1 : 0));
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INSERT_ROW:
			pDoc->InsertRow(m_iUndoTrack, m_iUndoFrame, m_iUndoChannel, m_iUndoRow);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE:
			pPatternEditor->Paste(m_pClipData);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_PASTE_MIX:
			pPatternEditor->PasteMix(m_pClipData);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->DeleteSelection(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EDIT_DELETE_ROWS:
			// Delete and pull up selection
			pPatternEditor->DeleteSelectionRows(m_selection);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_TRANSPOSE:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->Transpose(m_iTransposeMode);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SCROLL_VALUES:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->ScrollValues(m_iScrollValue);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_INTERPOLATE:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->Interpolate();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REVERSE:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->Reverse();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_REPLACE_INSTRUMENT:
			pPatternEditor->SetSelection(m_selection);
			pPatternEditor->ReplaceInstrument(m_iInstrument);
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
			if (m_bSelecting)
				pPatternEditor->SetSelection(m_selection);
			else
				pPatternEditor->ClearSelection();
			if (m_bDragDelete)
				pPatternEditor->Delete();
			pPatternEditor->DragPaste(m_pClipData, &m_dragTarget, m_bDragMix);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iUndoTrack, m_iNewPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_EXPAND_PATTERN:
			pPatternEditor->ExpandPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SHRINK_PATTERN:
			pPatternEditor->ShrinkPattern();
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_EXPAND_COLUMNS:
			pDoc->SetEffColumns(m_iUndoTrack, m_iClickedChannel, m_iUndoColumnCount + 1);
			pDoc->UpdateAllViews(NULL, CHANGED_ERASE);
			break;
		case ACT_SHRINK_COLUMNS:
			pDoc->SetEffColumns(m_iUndoTrack, m_iClickedChannel, m_iUndoColumnCount - 1);
			pDoc->UpdateAllViews(NULL, CHANGED_ERASE);
			break;
#ifdef _DEBUG_
		default:
			AfxMessageBox(_T("TODO: Redo for this action is not implemented"));
#endif
	}

	pPatternEditor->MoveToFrame(m_iRedoFrame);
	pPatternEditor->MoveToChannel(m_iRedoChannel);
	pPatternEditor->MoveToRow(m_iRedoRow);
	pPatternEditor->MoveToColumn(m_iRedoColumn);
}

void CPatternAction::Update(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(pMainFrm->GetActiveView());
	CFamiTrackerDoc *pDoc = pView->GetDocument();

	switch (m_iAction) {
		case ACT_PATTERN_LENGTH:
			pDoc->SetPatternLength(m_iUndoTrack, m_iNewPatternLen);
			pDoc->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
	}
}

void CPatternAction::UpdateCursor(CPatternEditor *pPatternEditor)
{
	// Update the redo cursor position
	m_iRedoFrame	= pPatternEditor->GetFrame();
	m_iRedoChannel  = pPatternEditor->GetChannel();
	m_iRedoRow		= pPatternEditor->GetRow();
	m_iRedoColumn   = pPatternEditor->GetColumn();
}
