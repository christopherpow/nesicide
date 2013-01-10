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

#include <cmath>
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsN106.h"

CChannelHandlerN106::CChannelHandlerN106() : CChannelHandler(),
	m_bLoadWave(false),
	m_iWaveLen(0),
	m_pInstrument(NULL)
{
	SetMaxPeriod(0x3FFFF);
}

void CChannelHandlerN106::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	int LastInstrument = m_iInstrument;

	if (!CChannelHandler::CheckNote(pNoteData, INST_N106))
		return;

	int Note	= pNoteData->Note;
	int Octave	= pNoteData->Octave;
	int Volume	= pNoteData->Vol;

	CInstrumentN106 *pInst = (CInstrumentN106*)m_pDocument->GetInstrument(m_iInstrument);

	m_pInstrument = pInst;

	// Volume
	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}

	// Instrument switches
	if (LastInstrument != m_iInstrument || Note > 0 && pInst) {
		// Load new instrument
		for (int i = 0; i < SEQ_COUNT; ++i) {
			m_iSeqEnabled[i] = pInst->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInst->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
		m_iWaveLen = pInst->GetWaveSize();
		m_iWavePos = pInst->GetWavePos();
		m_bLoadWave = true;
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
		m_bEnabled = true;
		m_iSeqVolume = 0x0F;
	}

}

void CChannelHandlerN106::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();

	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < SEQ_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequenceN106(m_iSeqIndex[i], i));
}

void CChannelHandlerN106::RefreshChannel()
{
	int Channel = GetIndex();		// Channel #

	int WaveSize = 8 - (m_iWaveLen >> 2);

	int Frequency = CalculatePeriod();
	int Volume = CalculateVolume(15);

	int ChannelAddrBase = 0x40 + Channel * 8;
	int Channels = 7;

	if (!m_bEnabled)
		Volume = 0;

	if (m_bLoadWave) {
		m_bLoadWave = false;
		LoadWave();
	}

	// Update channel
	SetAddress(ChannelAddrBase, true);

	WriteData(Frequency & 0xFF);
	WriteData(0);
	WriteData((Frequency >> 8) & 0xFF);
	WriteData(0);
	WriteData((WaveSize << 2) | ((Frequency >> 16) & 0x03));
	WriteData(0);
	WriteData(m_iWavePos);
	WriteData((Channels << 4) | Volume);
}

void CChannelHandlerN106::ClearRegisters()
{
	int Channel = GetIndex();
	int ChannelAddrBase = 0x40 + Channel * 8;
	int Channels = 7;

	WriteReg(ChannelAddrBase + 7, (Channels << 4) | 0);
}

void CChannelHandlerN106::WriteReg(int Reg, int Value)
{
	m_pAPU->ExternalWrite(0xF800, Reg);
	m_pAPU->ExternalWrite(0x4800, Value);
}

void CChannelHandlerN106::SetAddress(char Addr, bool AutoInc)
{
	m_pAPU->ExternalWrite(0xF800, (AutoInc ? 0x80 : 0) | Addr);
}

void CChannelHandlerN106::WriteData(char Data)
{
	m_pAPU->ExternalWrite(0x4800, Data);
}

void CChannelHandlerN106::LoadWave()
{
	// Fill the wave RAM
	ASSERT(m_pInstrument != NULL);

	// Start of wave in memory
	int Channel = GetIndex();
	int StartAddr = m_iWavePos >> 1;

	SetAddress(StartAddr, true);

	for (int i = 0, w; i < m_iWaveLen; ++i) {
		if (i & 1) {
			w |= m_pInstrument->GetSample(i) << 4;
			WriteData(w);
		}
		else
			w = m_pInstrument->GetSample(i);
	}
}
