/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
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
#include "SoundGen.h"

const int CChannelHandlerMMC5::SEQ_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_DUTYCYCLE};

CChannelHandlerMMC5::CChannelHandlerMMC5() : CChannelHandler(0x7FF, 0x0F)
{
}

void CChannelHandlerMMC5::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	m_iPostEffect = 0;
	m_iPostEffectParam = 0;
	m_bManualVolume = false;
	m_iInitVolume = 0x0F;

	CChannelHandler::HandleNoteData(pNoteData, EffColumns);

	if (pNoteData->Note != NONE && pNoteData->Note != HALT && pNoteData->Note != RELEASE) {
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
	CFamiTrackerDoc *pDocument = m_pSoundGen->GetDocument();
	CInstrumentContainer<CInstrument2A03> instContainer(pDocument, Instrument);
	CInstrument2A03 *pInstrument = instContainer();

	if (pInstrument == NULL)
		return false;

	for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i) {
		const CSequence *pSequence = pDocument->GetSequence(SNDCHIP_NONE, pInstrument->GetSeqIndex(i), i);
		if (Trigger || !IsSequenceEqual(i, pSequence) || pInstrument->GetSeqEnable(i) > GetSequenceState(i)) {
			if (pInstrument->GetSeqEnable(i) == 1)
				SetupSequence(i, pSequence);
			else 
				ClearSequence(i);
		}
	}

	return true;
}

void CChannelHandlerMMC5::HandleEmptyNote()
{
}

void CChannelHandlerMMC5::HandleCut()
{
	CutNote();
}

void CChannelHandlerMMC5::HandleRelease()
{
	if (!m_bRelease) {
		ReleaseNote();
		ReleaseSequences();
	}
}

void CChannelHandlerMMC5::HandleNote(int Note, int Octave)
{
	m_iNote		  = RunNote(Octave, Note);
	m_iDutyPeriod = m_iDefaultDuty;
	m_iSeqVolume  = m_iInitVolume;
}

void CChannelHandlerMMC5::ProcessChannel()
{
	CFamiTrackerDoc *pDocument = m_pSoundGen->GetDocument();

	// Default effects
	CChannelHandler::ProcessChannel();

	// Sequences
	for (int i = 0; i < SEQUENCES; ++i)
		RunSequence(i);
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
	int Period = CalculatePeriod();
	int Volume = CalculateVolume();
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
	int Period = CalculatePeriod();
	int Volume = CalculateVolume();
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
