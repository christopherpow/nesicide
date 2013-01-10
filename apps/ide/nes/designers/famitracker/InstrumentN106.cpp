/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2010  Jonathan Liss
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

#include "instrument.h"

const int CInstrumentN106::SEQUENCE_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_DUTYCYCLE};

CInstrumentN106::CInstrumentN106()
{
	// Default wave
	const char TRIANGLE_WAVE[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	};

	for (int i = 0; i < SEQUENCE_COUNT; ++i) {
		m_iSeqEnable[i] = 0;
		m_iSeqIndex[i] = 0;
	}

	for (int i = 0; i < MAX_N106_WAVE; ++i) {
		m_iSamples[i] = TRIANGLE_WAVE[i];
	}

	m_iSampleLen = MAX_N106_WAVE;
	m_iSamplePos = 0;
}

CInstrument *CInstrumentN106::Clone()
{
	CInstrumentN106 *pNew = new CInstrumentN106();

	for (int i = 0; i < SEQUENCE_COUNT; i++) {
		pNew->SetSeqEnable(i, GetSeqEnable(i));
		pNew->SetSeqIndex(i, GetSeqIndex(i));
	}

	pNew->SetName(GetName());

	return pNew;
}

//void CInstrumentN106::Store(CDocumentFile *pFile)
//{
//}

//bool CInstrumentN106::Load(CDocumentFile *pDocFile)
//{
//	return false;
//}

//void CInstrumentN106::SaveFile(CFile *pFile, CFamiTrackerDoc *pDoc)
//{
//	AfxMessageBox(_T("Saving N106 instruments is not yet supported"));
//}

//bool CInstrumentN106::LoadFile(CFile *pFile, int iVersion, CFamiTrackerDoc *pDoc)
//{
//	return false;
//}

int CInstrumentN106::CompileSize(CCompiler *pCompiler)
{
	return 0;
}

int CInstrumentN106::Compile(CCompiler *pCompiler, int Index)
{
	return 0;
}

bool CInstrumentN106::CanRelease() const
{
	return false; // TODO
}

int	CInstrumentN106::GetSeqEnable(int Index) const
{
	return m_iSeqEnable[Index];
}

int	CInstrumentN106::GetSeqIndex(int Index) const
{
	return m_iSeqIndex[Index];
}

void CInstrumentN106::SetSeqEnable(int Index, int Value)
{
	m_iSeqEnable[Index] = Value;
}

void CInstrumentN106::SetSeqIndex(int Index, int Value)
{
	m_iSeqIndex[Index] = Value;
}

int CInstrumentN106::GetWaveSize() const
{
	return m_iSampleLen;
}

void CInstrumentN106::SetWaveSize(int size)
{
	m_iSampleLen = size;
}

int CInstrumentN106::GetSample(int index) const
{
	return m_iSamples[index];
}

void CInstrumentN106::SetSample(int index, int sample)
{
	m_iSamples[index] = sample;
}

int CInstrumentN106::GetWavePos() const
{
	return m_iSamplePos;
}

void CInstrumentN106::SetWavePos(int pos)
{
	m_iSamplePos = pos;
}
