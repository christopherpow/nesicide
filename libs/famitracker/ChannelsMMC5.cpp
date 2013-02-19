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

void CChannelHandlerMMC5::PlayChannelNote(stChanNote *NoteData, int EffColumns)
{
	CInstrument2A03 *Inst;
	unsigned int Note, Octave;
	unsigned char Sweep = 0;
	unsigned int Instrument, Volume, LastInstrument;

	int	InitVolume = 0x0F;

	Note		= NoteData->Note;
	Octave		= NoteData->Octave;
	Volume		= NoteData->Vol;
	Instrument	= NoteData->Instrument;

	LastInstrument = m_iInstrument;

	if (Note == HALT || Note == RELEASE) {
		Instrument	= MAX_INSTRUMENTS;
	}

	if (Note == RELEASE)
		m_bRelease = true;
	else if (Note != NONE)
		m_bRelease = false;

	int PostEffect = 0, PostEffectParam = 0;

	// Evaluate effects
	for (int n = 0; n < EffColumns; n++) {
		unsigned char EffNum   = NoteData->EffNumber[n];
		unsigned char EffParam = NoteData->EffParam[n];

		#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

		if (!CheckCommonEffects(EffNum, EffParam)) {
			switch (EffNum) {
				case EF_VOLUME:
					InitVolume = EffParam;
					if (Note == 0)
						m_iSeqVolume = InitVolume;
					break;
				case EF_DUTY_CYCLE:
					m_iDefaultDuty = m_iDutyPeriod = EffParam;
					break;
				case EF_SLIDE_UP:
				case EF_SLIDE_DOWN:
					PostEffect = EffNum;
					PostEffectParam = EffParam;
					SetupSlide(EffNum, EffParam);
					break;
			}
		}
	}

	// Change instrument
	if (Instrument != LastInstrument) {
		if (Instrument == MAX_INSTRUMENTS)
			Instrument = LastInstrument;
		else
			LastInstrument = Instrument;

		if ((Inst = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument)) == NULL)
			return;

		if (Inst->GetType() != INST_2A03)
			return;

		for (int i = 0; i < SEQUENCES; i++) {
			if (m_iSeqIndex[i] != Inst->GetSeqIndex(i)) {
				m_iSeqEnabled[i] = Inst->GetSeqEnable(i);
				m_iSeqIndex[i]	 = Inst->GetSeqIndex(i);
				m_iSeqPointer[i] = 0;
			}
		}

		m_iInstrument = Instrument;
	}
	else {
		if (Instrument == MAX_INSTRUMENTS)
			Instrument = m_iLastInstrument;
		else
			m_iLastInstrument = Instrument;

		if ((Inst = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument)) == NULL)
			return;
		if (Inst->GetType() != INST_2A03)
			return;
	}

	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}

	if (Note == 0) {
		return;
	}
	
	if (Note == HALT) {
		KillChannel();
		return;
	}

	if (!m_bRelease) {

		// Trigger instrument
		for (int i = 0; i < SEQUENCES; i++) {
			m_iSeqEnabled[i]	= Inst->GetSeqEnable(i);
			m_iSeqIndex[i]		= Inst->GetSeqIndex(i);
			m_iSeqPointer[i]	= 0;
		}

		m_iNote			= RunNote(Octave, Note);
		m_iDutyPeriod	= m_iDefaultDuty;
		m_iSeqVolume	= InitVolume;
		m_bEnabled		= true;
	}
	else {
		ReleaseNote();
	}

	if (PostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
		SetupSlide(PostEffect, PostEffectParam);
	else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
		m_iEffect = EF_NONE;
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

	m_pAPU->ExternalWrite(0x5015, 0x03);

	m_pAPU->ExternalWrite(0x5000, (DutyCycle << 6) | 0x30 | Volume);
	m_pAPU->ExternalWrite(0x5002, HiFreq);

	if (LoFreq != LastLoFreq)
		m_pAPU->ExternalWrite(0x5003, LoFreq);
}

void CMMC5Square1Chan::ClearRegisters()
{
	m_pAPU->ExternalWrite(0x5000, 0);
	m_pAPU->ExternalWrite(0x5002, 0);
	m_pAPU->ExternalWrite(0x5003, 0);
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

	m_pAPU->ExternalWrite(0x5015, 0x03);

	m_pAPU->ExternalWrite(0x5004, (DutyCycle << 6) | 0x30 | Volume);
	m_pAPU->ExternalWrite(0x5006, HiFreq);

	if (LoFreq != LastLoFreq)
		m_pAPU->ExternalWrite(0x5007, LoFreq);
}

void CMMC5Square2Chan::ClearRegisters()
{
	m_pAPU->ExternalWrite(0x5004, 0);
	m_pAPU->ExternalWrite(0x5006, 0);
	m_pAPU->ExternalWrite(0x5007, 0);
}
