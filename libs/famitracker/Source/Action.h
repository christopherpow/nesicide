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

//
// Change MAX_LEVELS in the class CActionHandler if you want more undo levels
//

#include "PatternEditor.h"
#include "FrameEditor.h"

// Base class for action commands
class CAction
{
public:
	CAction(int iAction);
	virtual ~CAction();

	// Save the undo state and perform the action. This method may reject the action by returning false
	virtual bool SaveState(CMainFrame *pMainFrm) = 0;

	// Undo the operation, using the saved state
	virtual void Undo(CMainFrame *pMainFrm) = 0;

	// Redo the operation
	virtual void Redo(CMainFrame *pMainFrm) = 0;

	// Get the action type
	int GetAction() const;

protected:
	int m_iAction;
};

class CPatternEditor;
class CFrameClipData;
class CPatternClipData;

// Stores action objects (a dual-stack, kind of)
class CActionHandler
{
public:
	CActionHandler();
	~CActionHandler();

	void Clear();
	void Push(CAction *pAction);
	CAction *PopUndo();
	CAction *PopRedo();
	CAction *GetLastAction() const;
	int GetUndoLevel() const;
	int GetRedoLevel() const;
	bool CanUndo() const;
	bool CanRedo() const;

public:
	// Levels of undo in the editor. Higher value requires more memory
	static const int MAX_LEVELS = 64;

private:
	CAction *m_pActionStack[MAX_LEVELS + 1];
	int		m_iUndoLevel;
	int		m_iRedoLevel;

};

