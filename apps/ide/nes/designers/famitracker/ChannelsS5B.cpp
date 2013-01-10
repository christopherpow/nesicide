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

// Sunsoft 5B (YM2149/AY-3-8910)

#include <cmath>
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsS5B.h"

//const int CChannelHandlerS5B::SEQ_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_SUNSOFT_NOISE};

CChannelHandlerS5B::CChannelHandlerS5B() : CChannelHandler()
{
	SetMaxPeriod(0xFFF);
}
/*
bool NoteValid(int Note)
{
	return (Note != NONE && Note != HALT && Note != RELEASE);
}
*/
void CChannelHandlerS5B::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	int LastInstrument = m_iInstrument;

	if (!CChannelHandler::CheckNote(pNoteData, INST_S5B))
		return;

	int Note	= pNoteData->Note;
	int Octave	= pNoteData->Octave;
	int Volume	= pNoteData->Vol;

	CInstrumentS5B *pInst = (CInstrumentS5B*)m_pDocument->GetInstrument(m_iInstrument);

	// Instrument
	if ((LastInstrument != m_iInstrument) || (Note != 0 && Note != HALT && Note != RELEASE)) {
		// Setup instrument
		for (int i = 0; i < SEQ_COUNT; ++i) {
			m_iSeqEnabled[i] = pInst->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInst->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}

	// Volume
	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}

	// Note action
	if (Note == HALT) {
		// Halt
		CutNote();
		m_bEnabled = false;
		m_iNote = 0;
	}
	else if (Note == RELEASE) {
		// Release
		ReleaseNote();
	}
	else if (Note != NONE) {
		// New note
		m_iNote	= RunNote(Octave, Note);
		m_iSeqVolume = 0xF;
		m_bEnabled = true;
	}
}

void CChannelHandlerS5B::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();

	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < SEQ_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequence(m_iSeqIndex[i], i));
}

void CChannelHandlerS5B::WriteReg(int Reg, int Value)
{
	m_pAPU->ExternalWrite(0xC000, Reg);
	m_pAPU->ExternalWrite(0xE000, Value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 1
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CS5BChannel1::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod();
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;

	int Volume = CalculateVolume(15);

	unsigned char Noise = 1;//m_iDutyPeriod & 1;
	unsigned char Square = 0;
	unsigned char NoisePeriod = m_iDutyPeriod;

	NoisePeriod = m_iNote & 0x1F;

	WriteReg(7, 0x30 | (Noise ^ 1) << 3 | 0x06 | (Square ^ 1));
	WriteReg(0, LoPeriod);
	WriteReg(1, HiPeriod);
	WriteReg(8, Volume);
	WriteReg(6, NoisePeriod);
	Noise = 0;
}

void CS5BChannel1::ClearRegisters()
{
	WriteReg(7, 0x38);
	WriteReg(8, 0);		// Clear volume
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CS5BChannel2::RefreshChannel()
{
}

void CS5BChannel2::ClearRegisters()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Channel 3
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CS5BChannel3::RefreshChannel()
{
}

void CS5BChannel3::ClearRegisters()
{
}
