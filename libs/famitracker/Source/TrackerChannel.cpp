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

#include "stdafx.h"
#include "FamiTrackerDoc.h"
#include "TrackerChannel.h"

/*
 * This class serves as the interface between the UI and the sound player for each channel
 * Thread synchronization should be done here
 *
 */

CTrackerChannel::CTrackerChannel(const TCHAR *pName, const int iChip, const int iID) : 
	m_pChannelName(pName),
	m_iChip(iChip),
	m_iChannelID(iID),
	m_iColumnCount(0),
	m_bNewNote(false),
	m_iPitch(0)
{
}

CTrackerChannel::~CTrackerChannel(void)
{
}

const TCHAR *CTrackerChannel::GetChannelName() const
{
	return m_pChannelName;
}

const char CTrackerChannel::GetChip() const
{
	return m_iChip;
}

const int CTrackerChannel::GetID() const
{
	return m_iChannelID;
}

const int CTrackerChannel::GetColumnCount() const
{
	return m_iColumnCount;
}

void CTrackerChannel::SetColumnCount(int Count)
{
	m_iColumnCount = Count;
}

void CTrackerChannel::SetNote(stChanNote Note)
{
	m_NoteLock.Lock();
	m_Note = Note;
	m_bNewNote = true;
	m_NoteLock.Unlock();
}

stChanNote CTrackerChannel::GetNote()
{
	stChanNote Note;
	m_NoteLock.Lock();
	m_bNewNote = false;
	Note = m_Note;
	m_NoteLock.Unlock();
	return Note;
}

bool CTrackerChannel::NewNoteData()
{
	bool bNewNote;
	m_NoteLock.Lock();
	bNewNote = m_bNewNote;
	m_NoteLock.Unlock();
	return bNewNote;
}

void CTrackerChannel::Reset()
{
	m_NoteLock.Lock();
	m_bNewNote = false;
	m_NoteLock.Unlock();
}

void CTrackerChannel::SetVolumeMeter(int Value)
{
	m_iVolumeMeter = Value;
}

int CTrackerChannel::GetVolumeMeter() const
{
	return m_iVolumeMeter;
}

void CTrackerChannel::SetPitch(int Pitch)
{
	m_iPitch = Pitch;
}

int CTrackerChannel::GetPitch() const
{
	return m_iPitch;
}
/*
int CTrackerChannel::GetEffect(int Letter) const
{
	if (m_iChip == SNDCHIP_FDS) {
	}

	return 
}
*/
