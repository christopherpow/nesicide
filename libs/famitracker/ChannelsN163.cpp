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

#include <cmath>
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsN163.h"
#include "SoundGen.h"

const int N163_PITCH_SLIDE_SHIFT = 2;	// Increase amplitude of pitch slides

CChannelHandlerN163::CChannelHandlerN163() : CChannelHandler(),
	m_bLoadWave(false),
	m_iWaveLen(0),
	m_iWaveIndex(0),
	m_iWaveCount(0)
{
	//SetMaxPeriod(0x3FFFF);
	SetMaxPeriod(0xFFFF);
	m_iDutyPeriod = 0;
}

void CChannelHandlerN163::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	int LastInstrument = m_iInstrument;

	if (!CChannelHandler::CheckNote(pNoteData, INST_N163))
		return;

	int Note	= pNoteData->Note;
	int Octave	= pNoteData->Octave;
	int Volume	= pNoteData->Vol;

	CInstrumentN163 *pInstrument = (CInstrumentN163*)m_pDocument->GetInstrument(m_iInstrument);

	if (pInstrument == NULL)
		return;

	int PostEffect = 0, PostEffectParam = 0;

	m_bLoadWave = false;

	// Evaluate effects
	for (int n = 0; n < EffColumns; n++) {
		unsigned char EffNum   = pNoteData->EffNumber[n];
		unsigned char EffParam = pNoteData->EffParam[n];

//		#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

		if (EffNum == EF_PORTA_DOWN) {
			m_iPortaSpeed = EffParam << N163_PITCH_SLIDE_SHIFT;
			m_iEffect = EF_PORTA_UP;
		}
		else if (EffNum == EF_PORTA_UP) {
			m_iPortaSpeed = EffParam << N163_PITCH_SLIDE_SHIFT;
			m_iEffect = EF_PORTA_DOWN;
		}
		else if (EffNum == EF_PORTAMENTO) {
			m_iPortaSpeed = EffParam << N163_PITCH_SLIDE_SHIFT;
			m_iEffect = EF_PORTAMENTO;
		}
		else if (!CheckCommonEffects(EffNum, EffParam)) {
			// Custom effects
			switch (EffNum) {
				case EF_DUTY_CYCLE:
					// Duty effect controls wave
					m_iWaveIndex = EffParam;
					m_bLoadWave = true;
					break;
				case EF_SLIDE_UP:
				case EF_SLIDE_DOWN:
					PostEffect = EffNum;
					PostEffectParam = EffParam;
					SetupSlide(EffNum, EffParam);
					m_iPortaSpeed <<= N163_PITCH_SLIDE_SHIFT;
					break;
			}
		}
	}

	// Volume
	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}

	// Instrument switches
	if (LastInstrument != m_iInstrument || Note > 0 && pInstrument && Note != HALT && Note != RELEASE) {
		// Load new instrument
		for (int i = 0; i < SEQ_COUNT; ++i) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
		m_iWaveLen = pInstrument->GetWaveSize();
		m_iWavePos = /*pInstrument->GetAutoWavePos() ? GetIndex() * 16 :*/ pInstrument->GetWavePos();
		m_iWaveCount = pInstrument->GetWaveCount();
		if (!m_bLoadWave && LastInstrument != m_iInstrument)
			m_iWaveIndex = 0;
		m_bLoadWave = true;
	}

	// Note action
	if (Note == HALT) {
		// Halt
		CutNote();
		m_bEnabled = false;
		m_iNote = 0;
		m_bRelease = false;
	}
	else if (Note == RELEASE) {
		// Release
		ReleaseNote();
		ReleaseSequences(SNDCHIP_N163);
		m_bRelease = true;
	}
	else if (Note != NONE) {
		// New note
		m_iNote	= RunNote(Octave, Note);
		m_bEnabled = true;
		m_iSeqVolume = 0x0F;
		m_bRelease = false;
	}
	else if (Note == NONE) {
		return;
	}

	if (PostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN)) {
		SetupSlide(PostEffect, PostEffectParam);
		m_iPortaSpeed <<= N163_PITCH_SLIDE_SHIFT;
	}
	else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
		m_iEffect = EF_NONE;
}

void CChannelHandlerN163::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();

	if (!m_bEnabled)
		return;

	bool bUpdateWave = m_iSeqEnabled[SEQ_DUTYCYCLE] != 0;

	// Sequences
	for (int i = 0; i < CInstrumentN163::SEQUENCE_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequenceN163(m_iSeqIndex[i], i));

	if (bUpdateWave) {
		m_iWaveIndex = m_iDutyPeriod;
		m_bLoadWave = true;
	}
}

void CChannelHandlerN163::RefreshChannel()
{
	CheckWaveUpdate();

	int Channel = 7 - GetIndex();		// Channel #

	int WaveSize = /*64*/256 - (m_iWaveLen >> 2);

	int Frequency = LimitPeriod(m_iPeriod - ((GetVibrato() + GetFinePitch() + GetPitch()) << 4)) << 2;

	// Compensate for shorter waves
//	Frequency >>= 5 - int(log(double(m_iWaveLen)) / log(2.0));

	int Volume = CalculateVolume(15);

	int ChannelAddrBase = 0x40 + Channel * 8;
	int Channels = m_pDocument->GetNamcoChannels() - 1;

	if (!m_bEnabled)
		Volume = 0;

	if (m_bLoadWave) {
		m_bLoadWave = false;
		LoadWave();
	}

	// Update channel
	WriteData(ChannelAddrBase + 0, Frequency & 0xFF);
	WriteData(ChannelAddrBase + 2, (Frequency >> 8) & 0xFF);
	WriteData(ChannelAddrBase + 4, (WaveSize << 2) | ((Frequency >> 16) & 0x03));
	WriteData(ChannelAddrBase + 6, m_iWavePos);
	WriteData(ChannelAddrBase + 7, (Channels << 4) | Volume);
}

void CChannelHandlerN163::ClearRegisters()
{
	int Channel = GetIndex();
	int ChannelAddrBase = 0x40 + Channel * 8;
	int Channels = m_pDocument->GetNamcoChannels() - 1;

	WriteReg(ChannelAddrBase + 7, (Channels << 4) | 0);

	m_bLoadWave = false;
	m_iDutyPeriod = 0;
}

void CChannelHandlerN163::WriteReg(int Reg, int Value)
{
	m_pAPU->ExternalWrite(0xF800, Reg);
	m_pAPU->ExternalWrite(0x4800, Value);
}

void CChannelHandlerN163::SetAddress(char Addr, bool AutoInc)
{
	m_pAPU->ExternalWrite(0xF800, (AutoInc ? 0x80 : 0) | Addr);
}

void CChannelHandlerN163::WriteData(char Data)
{
	m_pAPU->ExternalWrite(0x4800, Data);
}

void CChannelHandlerN163::WriteData(int Addr, char Data)
{
	SetAddress(Addr, false);
	WriteData(Data);
}

void CChannelHandlerN163::LoadWave()
{
	// Fill the wave RAM
	CInstrumentN163 *pInstrument = (CInstrumentN163*)m_pDocument->GetInstrument(m_iInstrument);

	if (pInstrument == NULL || pInstrument->GetType() != INST_N163)
		return;

	// Start of wave in memory
	int Channel = GetIndex();
	int StartAddr = m_iWavePos >> 1;

	SetAddress(StartAddr, true);

	if (m_iWaveIndex >= m_iWaveCount)
		m_iWaveIndex = m_iWaveCount - 1;

	for (int i = 0; i < m_iWaveLen; i += 2) {
		WriteData((pInstrument->GetSample(m_iWaveIndex, i + 1) << 4) | pInstrument->GetSample(m_iWaveIndex, i));
	}
}

void CChannelHandlerN163::CheckWaveUpdate()
{
	// Check wave changes
	if (theApp.GetSoundGenerator()->HasWaveChanged())
		m_bLoadWave = true;
}
