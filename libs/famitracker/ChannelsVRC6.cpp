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

// This file handles playing of VRC6 channels

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsVRC6.h"

CChannelHandlerVRC6::CChannelHandlerVRC6() : CChannelHandler() 
{
	SetMaxPeriod(0xFFF);
}

void CChannelHandlerVRC6::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	CInstrumentVRC6 *pInstrument;
	int PostEffect = 0, PostEffectParam;

	int LastInstrument = m_iInstrument;

	if (!CChannelHandler::CheckNote(pNoteData, INST_VRC6))
		return;

	unsigned int Note, Octave;
	unsigned int Volume;

	Note	= pNoteData->Note;
	Octave	= pNoteData->Octave;
	Volume	= pNoteData->Vol;

	if (Note != 0)
		m_bRelease = false;
	else {
		if (pNoteData->Instrument != MAX_INSTRUMENTS)
			m_iInstrument = pNoteData->Instrument;
	}

	if (Note == RELEASE) {
		m_bRelease = true;
		m_iInstrument	= LastInstrument;
	}
	else if (Note == HALT) {
		m_iInstrument	= LastInstrument;
	}

	// Evaluate effects
	for (int n = 0; n < EffColumns; n++) {
		int EffCmd	 = pNoteData->EffNumber[n];
		int EffParam = pNoteData->EffParam[n];

		if (!CheckCommonEffects(EffCmd, EffParam)) {
			switch (EffCmd) {
				case EF_DUTY_CYCLE:
					m_iDefaultDuty = m_iDutyPeriod = EffParam;
					break;
				case EF_SLIDE_UP:
				case EF_SLIDE_DOWN:
					PostEffect = EffCmd;
					PostEffectParam = EffParam;
					SetupSlide(EffCmd, EffParam);
					break;
			}
		}
	}

	pInstrument = (CInstrumentVRC6*)m_pDocument->GetInstrument(m_iInstrument);

	if (!pInstrument)
		return;

	if (LastInstrument != m_iInstrument || Note > 0 && Note != HALT && Note != RELEASE) {
		// Setup instrument
		for (int i = 0; i < CInstrumentVRC6::SEQUENCE_COUNT; ++i) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}

	// Get volume
	if (Volume < 0x10)
		m_iVolume = Volume << VOL_SHIFT;

	if (Note == HALT) {
		KillChannel();
		return;
	}

	// No note
	if (!Note)
		return;

	if (!m_bRelease) {
		// Get the note
		m_iNote				= RunNote(Octave, Note);
		m_iSeqVolume		= 0xF;
		m_iDutyPeriod		= m_iDefaultDuty;
		m_bEnabled			= true;
		m_iLastInstrument	= m_iInstrument;
	}
	else {
		ReleaseNote();
		ReleaseSequences(SNDCHIP_VRC6);
	}

	if (PostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
		SetupSlide(PostEffect, PostEffectParam);
	else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
		m_iEffect = EF_NONE;
}

void CChannelHandlerVRC6::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();

	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < CInstrumentVRC6::SEQUENCE_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequence(SNDCHIP_VRC6, m_iSeqIndex[i], CInstrumentVRC6::SEQUENCE_TYPES[i]));
}

void CChannelHandlerVRC6::ResetChannel()
{
	CChannelHandler::ResetChannel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VRC6 Square 1
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVRC6Square1::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	unsigned int Period = CalculatePeriod(false);
	unsigned int Volume = CalculateVolume(15);
	unsigned char DutyCycle = m_iDutyPeriod << 4;

	unsigned char HiFreq = (Period & 0xFF);
	unsigned char LoFreq = (Period >> 8);

	m_pAPU->ExternalWrite(0x9000, DutyCycle | Volume);
	m_pAPU->ExternalWrite(0x9001, HiFreq);
	m_pAPU->ExternalWrite(0x9002, 0x80 | LoFreq);
}

void CVRC6Square1::ClearRegisters()
{
	m_pAPU->ExternalWrite(0x9000, 0);
	m_pAPU->ExternalWrite(0x9001, 0);
	m_pAPU->ExternalWrite(0x9002, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VRC6 Square 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVRC6Square2::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	unsigned int Period = CalculatePeriod(false);
	unsigned int Volume = CalculateVolume(15);
	unsigned char DutyCycle = m_iDutyPeriod << 4;

	unsigned char HiFreq = (Period & 0xFF);
	unsigned char LoFreq = (Period >> 8);

	m_pAPU->ExternalWrite(0xA000, DutyCycle | Volume);
	m_pAPU->ExternalWrite(0xA001, HiFreq);
	m_pAPU->ExternalWrite(0xA002, 0x80 | LoFreq);
}

void CVRC6Square2::ClearRegisters()
{
	m_pAPU->ExternalWrite(0xA000, 0);
	m_pAPU->ExternalWrite(0xA001, 0);
	m_pAPU->ExternalWrite(0xA002, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VRC6 Sawtooth
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVRC6Sawtooth::RefreshChannel()
{
	if (!m_bEnabled) 
		return;

	unsigned int Period = CalculatePeriod(false);

	unsigned char HiFreq = (Period & 0xFF);
	unsigned char LoFreq = (Period >> 8);

	unsigned int TremVol = GetTremolo();
	unsigned int Volume = (m_iSeqVolume * (m_iVolume >> VOL_SHIFT)) / 15 - TremVol;

	Volume = (Volume << 1) | ((m_iDutyPeriod & 1) << 5);

	if (Volume < 0)
		Volume = 0;
	if (Volume > 63)
		Volume = 63;

	if (m_iSeqVolume > 0 && m_iVolume > 0 && Volume == 0)
		Volume = 1;

	m_pAPU->ExternalWrite(0xB000, Volume);
	m_pAPU->ExternalWrite(0xB001, HiFreq);
	m_pAPU->ExternalWrite(0xB002, 0x80 | LoFreq);
}

void CVRC6Sawtooth::ClearRegisters()
{
	m_pAPU->ExternalWrite(0xB000, 0);
	m_pAPU->ExternalWrite(0xB001, 0);
	m_pAPU->ExternalWrite(0xB002, 0);
}
