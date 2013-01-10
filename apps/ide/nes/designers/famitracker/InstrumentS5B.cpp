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

const int CInstrumentS5B::SEQUENCE_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_DUTYCYCLE};

CInstrumentS5B::CInstrumentS5B()
{
	for (int i = 0; i < SEQUENCE_COUNT; ++i) {
		m_iSeqEnable[i] = 0;
		m_iSeqIndex[i] = 0;
	}
}

CInstrument *CInstrumentS5B::Clone()
{
	return NULL;
}

//void CInstrumentS5B::Store(CDocumentFile *pDocFile)
//{
//}

//bool CInstrumentS5B::Load(CDocumentFile *pDocFile)
//{
//	return false;
//}

//void CInstrumentS5B::SaveFile(CFile *pFile, CFamiTrackerDoc *pDoc)
//{
//	AfxMessageBox(_T("Saving 5B instruments is not yet supported"));
//}

//bool CInstrumentS5B::LoadFile(CFile *pFile, int iVersion, CFamiTrackerDoc *pDoc)
//{
//	return false;
//}

int CInstrumentS5B::CompileSize(CCompiler *pCompiler)
{
	return 0;
}

int CInstrumentS5B::Compile(CCompiler *pCompiler, int Index)
{
	return 0;
}

bool CInstrumentS5B::CanRelease() const
{
	return false; // TODO
}

int	CInstrumentS5B::GetSeqEnable(int Index) const
{
	return m_iSeqEnable[Index];
}

int	CInstrumentS5B::GetSeqIndex(int Index) const
{
	return m_iSeqIndex[Index];
}

void CInstrumentS5B::SetSeqEnable(int Index, int Value)
{
	m_iSeqEnable[Index] = Value;
}

void CInstrumentS5B::SetSeqIndex(int Index, int Value)
{
	m_iSeqIndex[Index] = Value;
}
