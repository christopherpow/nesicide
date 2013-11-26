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

// MMC5 file

#include <cmath>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsMMC5.h"

const int CChannelHandlerMMC5::SEQ_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_DUTYCYCLE};

CChannelHandlerMMC5::CChannelHandlerMMC5() : CChannelHandler()
{
	SetMaxPeriod(0x7FF);	// same as 2A03
}

void CChannelHandlerMMC5::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	m_iPostEffect = 0;
	m_iPostEffectParam = 0;
	m_bManualVolume = false;
	m_iInitVolume = 0x0F;

	CChannelHandler::HandleNoteData(pNoteData, EffColumns);

	if (pNoteData->Note != NONE && pNoteData->Note != HALT) {
		if (m_iPostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
			SetupSlide(m_iPostEffect, m_iPostEffectParam);
		else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
			m_iEffect = EF_NONE;
	}
}

void CChannelHandlerMMC5::HandleCustomEffects(int EffNum, int EffParam)
{
	if (!CheckCommonEffects(EffNum, EffParam)) {
		switch (EffNum) {
			case EF_VOLUME:
				m_iInitVolume = EffParam;
				m_bManualVolume = true;
				break;
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

bool CChannelHandlerMMC5::HandleInstrument(int Instrument, bool Trigger, bool NewInstrument)
{
	CInstrument2A03 *pInstrument = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument);

	if (pInstrument == NULL)
		return false;

	if (pInstrument->GetType() != INST_2A03) {
		pInstrument->Release();
		return false;
	}

	for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i) {
		if (m_iSeqIndex[i] != pInstrument->GetSeqIndex(i) || pInstrument->GetSeqEnable(i) != m_iSeqEnabled[i] || Trigger) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}
	
	pInstrument->Release();

	return true;
}

void CChannelHandlerMMC5::HandleEmptyNote()
{
}

void CChannelHandlerMMC5::HandleHalt()
{
	CutNote();
}

void CChannelHandlerMMC5::HandleRelease()
{
	ReleaseNote();
	ReleaseSequences(SNDCHIP_NONE);
}

void CChannelHandlerMMC5::HandleNote(int Note, int Octave)
{
	m_iNote		  = RunNote(Octave, Note);
	m_iDutyPeriod = m_iDefaultDuty;
	m_iSeqVolume  = m_iInitVolume;
	m_bEnabled	  = true;
}

void CChannelHandlerMMC5::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();
	
	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < SEQUENCES; ++i)
		RunSequence(i, m_pDocument->GetSequence(m_iSeqIndex[i], SEQ_TYPES[i]));
}

void CChannelHandlerMMC5::ResetChannel()
{
	CChannelHandler::ResetChannel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 1 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMMC5Square1Chan::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod(false);
	int Volume = CalculateVolume(15);
	char DutyCycle = (m_iDutyPeriod & 0x03);

	unsigned char HiFreq		= (Period & 0xFF);
	unsigned char LoFreq		= (Period >> 8);
	unsigned char LastLoFreq	= (m_iLastPeriod >> 8);

	m_iLastPeriod = Period;

	WriteExternalRegister(0x5015, 0x03);

	WriteExternalRegister(0x5000, (DutyCycle << 6) | 0x30 | Volume);
	WriteExternalRegister(0x5002, HiFreq);

	if (LoFreq != LastLoFreq)
		WriteExternalRegister(0x5003, LoFreq);
}

void CMMC5Square1Chan::ClearRegisters()
{
	WriteExternalRegister(0x5000, 0);
	WriteExternalRegister(0x5002, 0);
	WriteExternalRegister(0x5003, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 2 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMMC5Square2Chan::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod(false);
	int Volume = CalculateVolume(15);
	char DutyCycle = (m_iDutyPeriod & 0x03);

	unsigned char HiFreq		= (Period & 0xFF);
	unsigned char LoFreq		= (Period >> 8);
	unsigned char LastLoFreq	= (m_iLastPeriod >> 8);

	m_iLastPeriod = Period;

	WriteExternalRegister(0x5015, 0x03);

	WriteExternalRegister(0x5004, (DutyCycle << 6) | 0x30 | Volume);
	WriteExternalRegister(0x5006, HiFreq);

	if (LoFreq != LastLoFreq)
		WriteExternalRegister(0x5007, LoFreq);
}

void CMMC5Square2Chan::ClearRegisters()
{
	WriteExternalRegister(0x5004, 0);
	WriteExternalRegister(0x5006, 0);
	WriteExternalRegister(0x5007, 0);
}
