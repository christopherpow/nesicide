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

class CFamiTrackerDoc;

struct stSpacingInfo {
	int SpaceCount;
	int SpaceSize;
};

class CPatternCompiler
{
public:
	CPatternCompiler();
	~CPatternCompiler();

	void CleanUp();
	void CompileData(CFamiTrackerDoc *pDoc, int Track, int Pattern, int Channel, unsigned char (*DPCM_LookUp)[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE], unsigned int *iAssignedInstruments);
	bool IsSampleAccessed(unsigned int Index) { return m_bDSamplesAccessed[Index]; };

public:
	unsigned int	GetDataSize() { return m_iDataPointer; };
	unsigned char	GetData(unsigned int i) { ASSERT(i < m_iDataPointer); return m_pData[i]; };
	unsigned char	*GetData() { return m_pData; };
	unsigned int	GetCompressedDataSize() { return m_iCompressedDataPointer; };
	unsigned char	GetCompressedData(unsigned int i) { ASSERT(i < m_iCompressedDataPointer); return m_pCompressedData[i]; };
	bool			EmptyPattern() const;
	unsigned int	GetHash() const;

private:
	unsigned int FindInstrument(int Instrument, unsigned int *pInstList);
	void WriteData(unsigned char Value);
	void DispatchZeroes();
	void AccumulateZero();
	void OptimizeString();
	int	 GetBlockSize(int Position);
	stSpacingInfo ScanNoteLengths(CFamiTrackerDoc *pDoc, int Track, unsigned int StartRow, int Pattern, int Channel);

private:
	unsigned char	*m_pData;
	unsigned char	*m_pCompressedData;
	unsigned int	m_iDataPointer;
	unsigned int	m_iCompressedDataPointer;
	unsigned int	m_iZeroes;
	unsigned int	m_iCurrentDefaultDuration;
	bool			m_bDSamplesAccessed[OCTAVE_RANGE * NOTE_RANGE]; // <- check the range, its not optimal right now
	bool			m_bEmpty;
	unsigned int	m_iHash;
};
