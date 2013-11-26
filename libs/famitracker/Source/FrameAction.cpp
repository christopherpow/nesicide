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
#include "MainFrm.h"
#include "PatternEditor.h"
#include "Action.h"
#include "FrameEditor.h"

// CFrameAction ///////////////////////////////////////////////////////////////////
//
// Undo/redo commands for frame editor
//

CFrameAction::CFrameAction(int iAction) : CAction(iAction)
{
	m_pAllPatterns = NULL;
	m_pClipData = NULL;
}

CFrameAction::~CFrameAction()
{
	SAFE_RELEASE_ARRAY(m_pAllPatterns);
	SAFE_RELEASE(m_pClipData);
}

void CFrameAction::SetFrameCount(unsigned int FrameCount)
{
	m_iNewFrameCount = FrameCount;
}

void CFrameAction::SetPattern(unsigned int Pattern)
{
	m_iNewPattern = Pattern;
}

void CFrameAction::SetPatternDelta(int Delta, bool ChangeAll)
{
	m_iPatternDelta = Delta;
	m_bChangeAll = ChangeAll;
}

void CFrameAction::SetPasteData(CFrameClipData *pClipData)
{
	m_pClipData = pClipData;
}

void CFrameAction::SaveFrame(CFamiTrackerDoc *pDoc)
{
	for (unsigned int i = 0; i < pDoc->GetAvailableChannels(); ++i) {
		m_iPatterns[i] = pDoc->GetPatternAtFrame(m_iUndoFramePos, i);
	}
}

void CFrameAction::RestoreFrame(CFamiTrackerDoc *pDoc)
{
	for (unsigned int i = 0; i < pDoc->GetAvailableChannels(); ++i) {
		pDoc->SetPatternAtFrame(m_iUndoFramePos, i, m_iPatterns[i]);
	}
}

void CFrameAction::SetDragInfo(int DragTarget, CFrameClipData *pClipData, bool Remove)
{
	m_iDragTarget = DragTarget;
	m_pClipData = pClipData;
	m_bDragRemove = Remove;
}

void CFrameAction::SaveAllFrames(CFamiTrackerDoc *pDoc)
{
	int Frames = pDoc->GetFrameCount();
	int Channels = pDoc->GetChannelCount();

	m_pAllPatterns = new unsigned int[Frames * Channels];

	for (int i = 0; i < Frames; ++i) {
		for (int j = 0; j < Channels; ++j) {
			m_pAllPatterns[i * Channels + j] = pDoc->GetPatternAtFrame(i, j);
		}
	}

	m_iUndoFrameCount = Frames;
}

void CFrameAction::RestoreAllFrames(CFamiTrackerDoc *pDoc)
{
	pDoc->SetFrameCount(m_iUndoFrameCount);

	int Frames = pDoc->GetFrameCount();
	int Channels = pDoc->GetChannelCount();
	
	for (int i = 0; i < Frames; ++i) {
		for (int j = 0; j < Channels; ++j) {
			pDoc->SetPatternAtFrame(i, j, m_pAllPatterns[i * Channels + j]);
		}
	}
}

int CFrameAction::ClipPattern(int Pattern) const
{
	if (Pattern < 0)
		Pattern = 0;
	if (Pattern > MAX_PATTERN - 1)
		Pattern = MAX_PATTERN - 1;

	return Pattern;
}

void CFrameAction::ClearPatterns(CFamiTrackerDoc *pDoc, int Target)
{
	int Rows = m_pClipData->ClipInfo.Rows;
	int Channels = m_pClipData->ClipInfo.Channels;

	// Clean up the copy to new patterns command
	for (int i = 0; i < Rows; ++i) {
		for (int j = 0; j < Channels; ++j) {
			pDoc->ClearPattern(Target + i, j);
		}
	}

	pDoc->DeleteFrames(Target, Rows);
}

bool CFrameAction::SaveState(CMainFrame *pMainFrm)
{
	// Perform action

	CFrameEditor *pFrameEditor = pMainFrm->GetFrameEditor();
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDocument = pView->GetDocument();

	m_iUndoFramePos = pView->GetSelectedFrame();
	m_iUndoChannelPos = pView->GetSelectedChannel();

	pFrameEditor->GetSelectInfo(m_oSelInfo);

	switch (m_iAction) {
		case ACT_ADD:
			if (!pDocument->InsertFrame(m_iUndoFramePos + 1))
				return false;
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_REMOVE:
			SaveFrame(pDocument);
			if (!pDocument->RemoveFrame(m_iUndoFramePos))
				return false;
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE:
			if (!pDocument->DuplicateFrame(m_iUndoFramePos))
				return false;
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE_PATTERNS:
			if (!pDocument->DuplicatePatterns(m_iUndoFramePos + 1))
				return false;
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_CHANGE_COUNT:
			m_iUndoFrameCount = pDocument->GetFrameCount();
			pDocument->SetFrameCount(m_iNewFrameCount);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pMainFrm->UpdateControls();
			break;
		case ACT_SET_PATTERN:
			m_iOldPattern = pDocument->GetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos);
			pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, m_iNewPattern);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_SET_PATTERN_ALL:
			{
				int Channels = pDocument->GetAvailableChannels();
				for (int i = 0; i < Channels; ++i) {
					m_iPatterns[i] = pDocument->GetPatternAtFrame(m_iUndoFramePos, i);
					pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iNewPattern);
				}
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
		case ACT_CHANGE_PATTERN:
			{
				m_iOldPattern = pDocument->GetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos);
				int NewPattern = ClipPattern(m_iOldPattern + m_iPatternDelta);
				if (NewPattern == m_iOldPattern)
					return false;
				pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, NewPattern);
				pDocument->SetModifiedFlag();
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
		case ACT_CHANGE_PATTERN_ALL:
			{
				int Channels = pDocument->GetAvailableChannels();
				for (int i = 0; i < Channels; ++i) {
					m_iPatterns[i] = pDocument->GetPatternAtFrame(m_iUndoFramePos, i);
					if (m_iPatterns[i] + m_iPatternDelta < 0 || m_iPatterns[i] + m_iPatternDelta >= MAX_FRAMES)
						return false;
				}
				for (int i = 0; i < Channels; ++i)
					pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iPatterns[i] + m_iPatternDelta);
				pDocument->SetModifiedFlag();
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
		case ACT_MOVE_DOWN:
			if (!pDocument->MoveFrameDown(m_iUndoFramePos))
				return false;
			pView->SelectFrame(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_MOVE_UP:
			if (!pDocument->MoveFrameUp(m_iUndoFramePos))
				return false;
			pView->SelectFrame(m_iUndoFramePos - 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_PASTE:
			pFrameEditor->Paste(m_pClipData);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PASTE_NEW:
			pFrameEditor->PasteNew(m_pClipData);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP_MOVE:
			SaveAllFrames(pDocument);
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, m_bDragRemove, false);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pMainFrm->UpdateControls();
			break;
		case ACT_DRAG_AND_DROP_COPY:
			SaveAllFrames(pDocument);
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, false, false);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pMainFrm->UpdateControls();
			break;
		case ACT_DRAG_AND_DROP_COPY_NEW:
			SaveAllFrames(pDocument);
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, false, true);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pMainFrm->UpdateControls();
			break;
		case ACT_DELETE_SELECTION:
			SaveAllFrames(pDocument);
			pDocument->DeleteFrames(m_oSelInfo.iRowStart, m_oSelInfo.iRowEnd - m_oSelInfo.iRowStart + 1);
			pView->SelectFrame(m_oSelInfo.iRowStart);
			pFrameEditor->Clear();
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
	}

	return true;
}

void CFrameAction::Undo(CMainFrame *pMainFrm)
{
	// Undo action

	CFrameEditor *pFrameEditor = pMainFrm->GetFrameEditor();
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDocument = pView->GetDocument();

	m_iRedoFramePos = pView->GetSelectedFrame();
	m_iRedoChannelPos = pView->GetSelectedChannel();

	pView->SelectFrame(m_iUndoFramePos);
	pView->SelectChannel(m_iUndoChannelPos);

	pFrameEditor->SetSelectInfo(m_oSelInfo);

	switch (m_iAction) {
		case ACT_ADD:
			pDocument->RemoveFrame(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_REMOVE:
			pDocument->InsertFrame(m_iUndoFramePos);
			RestoreFrame(pDocument);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE:
			pDocument->RemoveFrame(m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE_PATTERNS:
			for (unsigned int i = 0; i < pDocument->GetAvailableChannels(); ++i) {
				pDocument->ClearPattern(m_iUndoFramePos + 1, i);
			}
			pDocument->RemoveFrame(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_CHANGE_COUNT:
			pDocument->SetFrameCount(m_iUndoFrameCount);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_SET_PATTERN:
			pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, m_iOldPattern);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_SET_PATTERN_ALL:
			for (unsigned int i = 0; i < pDocument->GetAvailableChannels(); ++i) {
				pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iPatterns[i]);
			}
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_CHANGE_PATTERN:
			pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, m_iOldPattern);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_CHANGE_PATTERN_ALL:
			for (unsigned int i = 0; i < pDocument->GetAvailableChannels(); ++i) {
				pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iPatterns[i]);
			}
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_MOVE_DOWN:
			pDocument->MoveFrameUp(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_MOVE_UP:
			pDocument->MoveFrameDown(m_iUndoFramePos - 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_PASTE:
			pDocument->DeleteFrames(m_iUndoFramePos, m_pClipData->ClipInfo.Rows);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PASTE_NEW:
			ClearPatterns(pDocument, m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP_MOVE:
		case ACT_DRAG_AND_DROP_COPY:
		case ACT_DELETE_SELECTION:
			RestoreAllFrames(pDocument);
			pView->SelectFrame(m_oSelInfo.iRowEnd);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_DRAG_AND_DROP_COPY_NEW:
			ClearPatterns(pDocument, m_iDragTarget);
			pView->SelectFrame(m_oSelInfo.iRowEnd);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
	}

	pView->SelectFrame(m_iUndoFramePos);
	pView->SelectChannel(m_iUndoChannelPos);
}

void CFrameAction::Redo(CMainFrame *pMainFrm)
{
	// Redo action

	CFrameEditor *pFrameEditor = pMainFrm->GetFrameEditor();
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDocument = pView->GetDocument();
	
	switch (m_iAction) {
		case ACT_ADD:
			pDocument->InsertFrame(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_REMOVE:
			pDocument->RemoveFrame(m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE:
			pDocument->DuplicateFrame(m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_DUPLICATE_PATTERNS:
			pDocument->DuplicatePatterns(m_iUndoFramePos + 1);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_CHANGE_COUNT:
			pDocument->SetFrameCount(m_iNewFrameCount);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_SET_PATTERN:
			pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, m_iNewPattern);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_SET_PATTERN_ALL:
			{
				int Channels = pDocument->GetAvailableChannels();
				for (int i = 0; i < Channels; ++i) {
					pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iNewPattern);
				}
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
		case ACT_CHANGE_PATTERN:
			pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, m_iOldPattern + m_iPatternDelta);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_CHANGE_PATTERN_ALL:
			{
				int Channels = pDocument->GetAvailableChannels();
				for (int i = 0; i < Channels; ++i) {
					pDocument->SetPatternAtFrame(m_iUndoFramePos, i, m_iPatterns[i] + m_iPatternDelta);
				}
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
		case ACT_MOVE_DOWN:
			pDocument->MoveFrameDown(m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_MOVE_UP:
			pDocument->MoveFrameUp(m_iUndoFramePos);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		case ACT_PASTE:
			pFrameEditor->Paste(m_pClipData);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_PASTE_NEW:
			pFrameEditor->PasteNew(m_pClipData);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			break;
		case ACT_DRAG_AND_DROP_MOVE:
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, m_bDragRemove, false);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_DRAG_AND_DROP_COPY:
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, false, false);
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
		case ACT_DRAG_AND_DROP_COPY_NEW:
			pFrameEditor->PerformDragOperation(m_pClipData, m_iDragTarget, false, true);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			pMainFrm->UpdateControls();
			break;
		case ACT_DELETE_SELECTION:
			pDocument->DeleteFrames(m_oSelInfo.iRowStart, m_oSelInfo.iRowEnd - m_oSelInfo.iRowStart + 1);
			pView->SelectFrame(m_oSelInfo.iRowStart);
			pFrameEditor->Clear();
			pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			pMainFrm->UpdateControls();
			break;
	}

	pView->SelectFrame(m_iRedoFramePos);
	pView->SelectChannel(m_iRedoChannelPos);
}

void CFrameAction::Update(CMainFrame *pMainFrm)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)pMainFrm->GetActiveView();
	CFamiTrackerDoc *pDocument = pView->GetDocument();

	switch (m_iAction) {
		case ACT_CHANGE_COUNT:
			pDocument->SetFrameCount(m_iNewFrameCount);
			pDocument->UpdateAllViews(NULL, CHANGED_FRAMES);
			break;
		// TODO add change pattern 
		case ACT_CHANGE_PATTERN:
			{
				int OldPattern = pDocument->GetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos);
				int NewPattern = ClipPattern(OldPattern + m_iPatternDelta);
				if (NewPattern == OldPattern)
					return;
				pDocument->SetPatternAtFrame(m_iUndoFramePos, m_iUndoChannelPos, NewPattern);
				pDocument->SetModifiedFlag();
				pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);
			}
			break;
	}
}
