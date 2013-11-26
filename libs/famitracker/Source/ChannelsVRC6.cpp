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

void CChannelHandlerVRC6::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	m_iPostEffect = 0;
	m_iPostEffectParam = 0;

	CChannelHandler::HandleNoteData(pNoteData, EffColumns);

	if (pNoteData->Note != NONE && pNoteData->Note != HALT) {
		if (m_iPostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
			SetupSlide(m_iPostEffect, m_iPostEffectParam);
		else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
			m_iEffect = EF_NONE;
	}
}

void CChannelHandlerVRC6::HandleCustomEffects(int EffNum, int EffParam)
{
	if (!CheckCommonEffects(EffNum, EffParam)) {
		switch (EffNum) {
			case EF_DUTY_CYCLE:
				m_iDefaultDuty = m_iDutyPeriod = EffParam;
				break;
			case EF_SLIDE_UP:
			case EF_SLIDE_DOWN:
				m_iPostEffect = EffNum;
				m_iPostEffectParam = EffParam;
				SetupSlide(EffNum, EffParam);
				break;
		}
	}
}

bool CChannelHandlerVRC6::HandleInstrument(int Instrument, bool Trigger, bool NewInstrument)
{
	CInstrumentVRC6 *pInstrument = (CInstrumentVRC6*)m_pDocument->GetInstrument(Instrument);

	if (!pInstrument)
		return false;

	if (pInstrument->GetType() != INST_VRC6) {
		pInstrument->Release();
		return false;
	}

	// Setup instrument
	for (int i = 0; i < CInstrumentVRC6::SEQUENCE_COUNT; ++i) {
		if (pInstrument->GetSeqIndex(i) != m_iSeqIndex[i] || pInstrument->GetSeqEnable(i) != m_iSeqEnabled[i] || Trigger) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}

	pInstrument->Release();

	return true;
}

void CChannelHandlerVRC6::HandleEmptyNote()
{
}

void CChannelHandlerVRC6::HandleHalt()
{
	CutNote();
}

void CChannelHandlerVRC6::HandleRelease()
{
	ReleaseNote();
	ReleaseSequences(SNDCHIP_VRC6);
}

void CChannelHandlerVRC6::HandleNote(int Note, int Octave)
{
	m_iNote		  = RunNote(Octave, Note);
	m_iSeqVolume  = 0x0F;
	m_iDutyPeriod = m_iDefaultDuty;
	m_bEnabled	  = true;
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

	WriteExternalRegister(0x9000, DutyCycle | Volume);
	WriteExternalRegister(0x9001, HiFreq);
	WriteExternalRegister(0x9002, 0x80 | LoFreq);
}

void CVRC6Square1::ClearRegisters()
{
	WriteExternalRegister(0x9000, 0);
	WriteExternalRegister(0x9001, 0);
	WriteExternalRegister(0x9002, 0);
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

	WriteExternalRegister(0xA000, DutyCycle | Volume);
	WriteExternalRegister(0xA001, HiFreq);
	WriteExternalRegister(0xA002, 0x80 | LoFreq);
}

void CVRC6Square2::ClearRegisters()
{
	WriteExternalRegister(0xA000, 0);
	WriteExternalRegister(0xA001, 0);
	WriteExternalRegister(0xA002, 0);
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
	int Volume = (m_iSeqVolume * (m_iVolume >> VOL_SHIFT)) / 15 - TremVol;

	Volume = (Volume << 1) | ((m_iDutyPeriod & 1) << 5);

	if (Volume < 0)
		Volume = 0;
	if (Volume > 63)
		Volume = 63;

	if (m_iSeqVolume > 0 && m_iVolume > 0 && Volume == 0)
		Volume = 1;

	if (!m_bGate)
		Volume = 0;

	WriteExternalRegister(0xB000, Volume);
	WriteExternalRegister(0xB001, HiFreq);
	WriteExternalRegister(0xB002, 0x80 | LoFreq);
}

void CVRC6Sawtooth::ClearRegisters()
{
	WriteExternalRegister(0xB000, 0);
	WriteExternalRegister(0xB001, 0);
	WriteExternalRegister(0xB002, 0);
}
