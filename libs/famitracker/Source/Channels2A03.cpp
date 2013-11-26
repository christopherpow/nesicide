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

// This file handles playing of 2A03 channels

#include <cmath>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "Channels2A03.h"
#include "Settings.h"

CChannelHandler2A03::CChannelHandler2A03() : CChannelHandler()
{
	SetMaxPeriod(0x7FF);
}

void CChannelHandler2A03::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	m_iPostEffect = 0;
	m_iPostEffectParam = 0;
	m_iSweep = 0;
	m_bSweeping = false;
	m_iInitVolume = 0x0F;
	m_bManualVolume = false;

	CChannelHandler::HandleNoteData(pNoteData, EffColumns);

	if (pNoteData->Note != NONE && pNoteData->Note != HALT) {
		if (m_iPostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
			SetupSlide(m_iPostEffect, m_iPostEffectParam);
		else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
			m_iEffect = EF_NONE;
	}
}

void CChannelHandler2A03::HandleCustomEffects(int EffNum, int EffParam)
{
	#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

	if (!CheckCommonEffects(EffNum, EffParam)) {
		// Custom effects
		switch (EffNum) {
			case EF_VOLUME:
				// Kill this maybe?
				m_iInitVolume = EffParam;
				m_bManualVolume = true;
				break;
			case EF_SWEEPUP:
				m_iSweep = 0x88 | (EffParam & 0x77);
				m_iLastPeriod = 0xFFFF;
				m_bSweeping = true;
				break;
			case EF_SWEEPDOWN:
				m_iSweep = 0x80 | (EffParam & 0x77);
				m_iLastPeriod = 0xFFFF;
				m_bSweeping = true;
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

bool CChannelHandler2A03::HandleInstrument(int Instrument, bool Trigger, bool NewInstrument)
{
	CInstrument2A03 *pInstrument = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument);

	if (pInstrument == NULL)
		return false;

	if (pInstrument->GetType() != INST_2A03) {
		pInstrument->Release();
		return false;
	}

	for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i) {
		if (pInstrument->GetSeqIndex(i) != m_iSeqIndex[i] || pInstrument->GetSeqEnable(i) != m_iSeqEnabled[i] || Trigger) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}

	pInstrument->Release();

	return true;
}

void CChannelHandler2A03::HandleEmptyNote()
{
	if (m_bManualVolume)
		m_iSeqVolume = m_iInitVolume;
	
	if (m_bSweeping)
		m_cSweep = m_iSweep;
}

void CChannelHandler2A03::HandleHalt()
{
	CutNote();
}

void CChannelHandler2A03::HandleRelease()
{
	ReleaseNote();
	ReleaseSequences(SNDCHIP_NONE);

	if (!m_bSweeping && (m_cSweep != 0 || m_iSweep != 0)) {
		m_iSweep = 0;
		m_cSweep = 0;
		m_iLastPeriod = 0xFFFF;
	}
	else if (m_bSweeping) {
		m_cSweep = m_iSweep;
		m_iLastPeriod = 0xFFFF;
	}
}

void CChannelHandler2A03::HandleNote(int Note, int Octave)
{
	m_iNote			= RunNote(Octave, Note);
	m_iDutyPeriod	= m_iDefaultDuty;
	m_iSeqVolume	= m_iInitVolume;
	m_bEnabled		= true;

	if (!m_bSweeping && (m_cSweep != 0 || m_iSweep != 0)) {
		m_iSweep = 0;
		m_cSweep = 0;
		m_iLastPeriod = 0xFFFF;
	}
	else if (m_bSweeping) {
		m_cSweep = m_iSweep;
		m_iLastPeriod = 0xFFFF;
	}
}

void CChannelHandler2A03::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();
	
	// Skip when DPCM
	if (m_iChannelID == CHANID_DPCM)
		return;

	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequence(m_iSeqIndex[i], CInstrument2A03::SEQUENCE_TYPES[i]));

	if (m_bGate && m_iSeqEnabled[SEQ_VOLUME] != 0)
		m_bGate = !(m_iSeqEnabled[SEQ_VOLUME] == 0);
}

void CChannelHandler2A03::ResetChannel()
{
	CChannelHandler::ResetChannel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 1 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSquare1Chan::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod(false);
	int Volume = CalculateVolume(15);
	char DutyCycle = (m_iDutyPeriod & 0x03);

	unsigned char HiFreq = (Period & 0xFF);
	unsigned char LoFreq = (Period >> 8);

	WriteRegister(0x4000, (DutyCycle << 6) | 0x30 | Volume);

	if (m_cSweep) {
		if (m_cSweep & 0x80) {
			WriteRegister(0x4001, m_cSweep);
			m_cSweep &= 0x7F;
			WriteRegister(0x4017, 0x80);	// Clear sweep unit
			WriteRegister(0x4017, 0x00);
			WriteRegister(0x4002, HiFreq);
			WriteRegister(0x4003, LoFreq);
			m_iLastPeriod = 0xFFFF;
		}
	}
	else {
		WriteRegister(0x4001, 0x08);
		WriteRegister(0x4017, 0x80);	// Manually execute one APU frame sequence to kill the sweep unit
		WriteRegister(0x4017, 0x00);
		WriteRegister(0x4002, HiFreq);
		
		if (LoFreq != (m_iLastPeriod >> 8))
			WriteRegister(0x4003, LoFreq);
	}

	m_iLastPeriod = Period;

	m_iStatePeriod = Period;
	m_iStateVolume = Volume;
}

void CSquare1Chan::ClearRegisters()
{
	WriteRegister(0x4000, 0x30);
	WriteRegister(0x4001, 0x08);
	WriteRegister(0x4002, 0x00);
	WriteRegister(0x4003, 0x00);
	m_iLastPeriod = 0xFFFF;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 2 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSquare2Chan::RefreshChannel()
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

	WriteRegister(0x4004, (DutyCycle << 6) | 0x30 | Volume);

	if (m_cSweep) {
		if (m_cSweep & 0x80) {
			WriteRegister(0x4005, m_cSweep);
			m_cSweep &= 0x7F;
			WriteRegister(0x4017, 0x80);		// Clear sweep unit
			WriteRegister(0x4017, 0x00);
			WriteRegister(0x4006, HiFreq);
			WriteRegister(0x4007, LoFreq);
			m_iLastPeriod = 0xFFFF;
		}
	}
	else {
		WriteRegister(0x4005, 0x08);
		WriteRegister(0x4017, 0x80);
		WriteRegister(0x4017, 0x00);
		WriteRegister(0x4006, HiFreq);
		
		if (LoFreq != LastLoFreq)
			WriteRegister(0x4007, LoFreq);
	}
}

void CSquare2Chan::ClearRegisters()
{
	WriteRegister(0x4004, 0x30);
	WriteRegister(0x4005, 0x08);
	WriteRegister(0x4006, 0x00);
	WriteRegister(0x4007, 0x00);
	m_iLastPeriod = 0xFFFF;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Triangle 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTriangleChan::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Freq = CalculatePeriod(false);

	unsigned char HiFreq = (Freq & 0xFF);
	unsigned char LoFreq = (Freq >> 8);
	
	if (m_iSeqVolume > 0 && m_iVolume > 0 && m_bGate) {
		WriteRegister(0x4008, 0x81);
		WriteRegister(0x400A, HiFreq);
		WriteRegister(0x400B, LoFreq);
	}
	else
		WriteRegister(0x4008, 0);
}

void CTriangleChan::ClearRegisters()
{
	WriteRegister(0x4008, 0);
	WriteRegister(0x4009, 0);
	WriteRegister(0x400A, 0);
	WriteRegister(0x400B, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Noise
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int CNoiseChan::CalculatePeriod() const
{
	return LimitPeriod(m_iPeriod - GetVibrato() + GetFinePitch() + GetPitch());
}
*/
void CNoiseChan::RefreshChannel()
{
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod(false);
	int Volume = CalculateVolume(15);
	char NoiseMode = (m_iDutyPeriod & 0x01) << 7;

	Period = (Period & 0x0F) ^ 0x0F;

	WriteRegister(0x400C, 0x30 | Volume);
	WriteRegister(0x400D, 0x00);
	WriteRegister(0x400E, NoiseMode | Period);
	WriteRegister(0x400F, 0x00);
}

void CNoiseChan::ClearRegisters()
{
	WriteRegister(0x400C, 0x30);
	WriteRegister(0x400D, 0);
	WriteRegister(0x400E, 0);
	WriteRegister(0x400F, 0);
}

unsigned int CNoiseChan::TriggerNote(int Note)
{
	RegisterKeyState(m_iChannelID, Note);
	return Note;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// DPCM
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CDPCMChan::CDPCMChan(CSampleMem *pSampleMem) : 
	CChannelHandler2A03(), 
	m_pSampleMem(pSampleMem)
{ 
	m_bEnabled = false; 
	m_cDAC = 255;
	m_iRetrigger = 0;
	m_iRetriggerCntr = 0;
	m_bTrigger = false;
};

void CDPCMChan::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	m_iCustomPitch = -1;
	m_iRetrigger = 0;

	if (pNoteData->Note != NONE) {
		m_iNoteCut = 0;
	}

	CChannelHandler::HandleNoteData(pNoteData, EffColumns);
}

void CDPCMChan::HandleCustomEffects(int EffNum, int EffParam)
{
	switch (EffNum) {
	case EF_DAC:
		m_cDAC = EffParam & 0x7F;
		break;
	case EF_SAMPLE_OFFSET:
		m_iOffset = EffParam;
		break;
	case EF_DPCM_PITCH:
		m_iCustomPitch = EffParam;
		break;
	case EF_RETRIGGER:
//			if (NoteData->EffParam[i] > 0) {
			m_iRetrigger = EffParam + 1;
			if (m_iRetriggerCntr == 0)
				m_iRetriggerCntr = m_iRetrigger;
//			}
//			m_iEnableRetrigger = 1;
		break;
	case EF_NOTE_CUT:
		m_iNoteCut = EffParam + 1;
		break;
	}
}

bool CDPCMChan::HandleInstrument(int Instrument, bool Trigger, bool NewInstrument)
{
	// Instruments are accessed in the note routine
	return true;
}

void CDPCMChan::HandleEmptyNote()
{
}

void CDPCMChan::HandleHalt()
{
//	KillChannel();
	CutNote();
}

void CDPCMChan::HandleRelease()
{
	m_bRelease = true;
}

void CDPCMChan::HandleNote(int Note, int Octave)
{
	CInstrument2A03 *pInstrument = (CInstrument2A03*)m_pDocument->GetInstrument(m_iInstrument);

	if (pInstrument == NULL)
		return;

	if (pInstrument->GetType() != INST_2A03) {
		pInstrument->Release();
		return;
	}

	int SampleIndex = pInstrument->GetSample(Octave, Note - 1);

	if (SampleIndex > 0) {

		int Pitch = pInstrument->GetSamplePitch(Octave, Note - 1);

		if (Pitch & 0x80)
			m_iLoop = 0x40;
		else
			m_iLoop = 0;

		if (m_iCustomPitch != -1)
			Pitch = m_iCustomPitch;
	
		m_iLoopOffset = pInstrument->GetSampleLoopOffset(Octave, Note - 1);

		CDSample *pDSample = m_pDocument->GetDSample(SampleIndex - 1);

		int SampleSize = pDSample->SampleSize;

		if (SampleSize > 0) {
			m_pSampleMem->SetMem(pDSample->SampleData, SampleSize);
			m_iPeriod = Pitch & 0x0F;
			m_iSampleLength = (SampleSize >> 4) - (m_iOffset << 2);
			m_iLoopLength = SampleSize - m_iLoopOffset;
			m_bEnabled = true;
			m_bTrigger = true;
			m_bGate = true;

			// Initial delta counter value
			unsigned char Delta = pInstrument->GetSampleDeltaValue(Octave, Note - 1);
			
			if (Delta != 255 && m_cDAC == 255)
				m_cDAC = Delta;

			m_iRetriggerCntr = m_iRetrigger;
		}
	}

	RegisterKeyState(m_iChannelID, -(Note - 1) + (Octave * 12));

	pInstrument->Release();
}

void CDPCMChan::RefreshChannel()
{
	if (m_cDAC != 255) {
		WriteRegister(0x4011, m_cDAC);
		m_cDAC = 255;
	}

	if (m_iRetrigger != 0) {
		m_iRetriggerCntr--;
		if (m_iRetriggerCntr == 0) {
			m_iRetriggerCntr = m_iRetrigger;
			m_bEnabled = true;
			m_bTrigger = true;
		}
	}

	if (m_bRelease) {
		// Release command
		WriteRegister(0x4015, 0x0F);
		m_bEnabled = false;
		m_bRelease = false;
	}

/*	
	if (m_bRelease) {
		// Release loop flag
		m_bRelease = false;
		WriteRegister(0x4010, 0x00 | (m_iPeriod & 0x0F));
		return;
	}
*/	

	if (!m_bEnabled)
		return;

	if (!m_bGate) {
		// Cut sample
		WriteRegister(0x4015, 0x0F);

		if (!theApp.GetSettings()->General.bNoDPCMReset || theApp.IsPlaying()) {
			WriteRegister(0x4011, 0);	// regain full volume for TN
		}

		m_bEnabled = false;		// don't write to this channel anymore
	}
	else if (m_bTrigger) {
		// Start playing the sample
		WriteRegister(0x4010, 0x00 | (m_iPeriod & 0x0F) | m_iLoop);
		WriteRegister(0x4012, m_iOffset);							// load address, start at $C000
		WriteRegister(0x4013, m_iSampleLength);						// length
		WriteRegister(0x4015, 0x0F);
		WriteRegister(0x4015, 0x1F);								// fire sample

		// Loop offset
		if (m_iLoopOffset > 0) {
			WriteRegister(0x4012, m_iLoopOffset);
			WriteRegister(0x4013, m_iLoopLength);
		}

		m_bTrigger = false;
	}
}

void CDPCMChan::ClearRegisters()
{
	WriteRegister(0x4015, 0x0F);

	WriteRegister(0x4010, 0);	
	WriteRegister(0x4011, 0);
	WriteRegister(0x4012, 0);
	WriteRegister(0x4013, 0);

	m_iOffset = 0;
	m_cDAC = 255;
}
