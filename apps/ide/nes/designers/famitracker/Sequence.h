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

#include "CustomExporterInterfaces.h"

class CDocumentFile;

/*
** This class is used to store instrument sequences
*/
class CSequence: public CSequenceInterface {
public:
	CSequence();

	void		 Clear();
	signed char	 GetItem(int Index) const;
	unsigned int GetItemCount() const;
	unsigned int GetLoopPoint() const;
	unsigned int GetReleasePoint() const;
	unsigned int GetSetting() const;
	void		 SetItem(int Index, signed char Value);
	void		 SetItemCount(unsigned int Count);
	void		 SetLoopPoint(unsigned int Point);
	void		 SetReleasePoint(unsigned int Point);
	void		 SetSetting(unsigned int Setting);

	//void		 Store(CDocumentFile *pDocFile, int Index, int Type);
 
	void		 Copy(const CSequence *pSeq);

	// Used by instrument editor
	void		 SetPlayPos(int pos);
	int			 GetPlayPos();

private:
	// Sequence data
	unsigned int m_iItemCount;
	unsigned int m_iLoopPoint;
	unsigned int m_iReleasePoint;
	unsigned int m_iSetting;
//	unsigned int m_iItemCountRelease;
	signed char	 m_cValues[MAX_SEQUENCE_ITEMS];
	// Used by instrument editor
	int			 m_iPlaying;
};


// Settings
enum {
	ARP_SETTING_ABSOLUTE = 0,
	ARP_SETTING_FIXED = 1,
	ARP_SETTING_RELATIVE = 2
};

// Sunsoft modes
const int S5B_MODE_SQUARE = 64;
const int S5B_MODE_NOISE = 128;
