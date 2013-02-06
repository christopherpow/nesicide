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
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "Channels2A03.h"

#ifdef _DEBUG
void ClearLog();
void AddLog(int Item);
#endif

CChannelHandler2A03::CChannelHandler2A03() : CChannelHandler()
{
	SetMaxPeriod(0x7FF);
}

void CChannelHandler2A03::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	CInstrument2A03 *pInstrument = NULL;
	unsigned int Note, Octave;
	unsigned char Sweep = 0;
	unsigned int Instrument, Volume, LastInstrument;
	bool Sweeping = false;

	int	InitVolume = 0x0F;

	Note		= pNoteData->Note;
	Octave		= pNoteData->Octave;
	Volume		= pNoteData->Vol;
	Instrument	= pNoteData->Instrument;

	LastInstrument = m_iInstrument;

	if (Note == HALT || Note == RELEASE) {
		Instrument	= MAX_INSTRUMENTS;
	}

	if (Note == RELEASE)
		m_bRelease = true;
	else if (Note != NONE)
		m_bRelease = false;

	if (Note != NONE) {
		m_iNoteCut = 0;
	}

	int PostEffect = 0, PostEffectParam = 0;

	// Evaluate effects
	for (int n = 0; n < EffColumns; n++) {
		unsigned char EffNum   = pNoteData->EffNumber[n];
		unsigned char EffParam = pNoteData->EffParam[n];

		#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

		if (!CheckCommonEffects(EffNum, EffParam)) {
			// Custom effects
			switch (EffNum) {
				case EF_VOLUME:
					// Kill this maybe?
					InitVolume = EffParam;
					if (Note == 0)
						m_iSeqVolume = InitVolume;
					break;
				case EF_SWEEPUP:
					Sweep = 0x88 | (EffParam & 0x77);
					m_iLastPeriod = 0xFFFF;
					Sweeping = true;
					break;
				case EF_SWEEPDOWN:
					Sweep = 0x80 | (EffParam & 0x77);
					m_iLastPeriod = 0xFFFF;
					Sweeping = true;
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
	
	// Volume column
	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}
	
	// Change instrument
 	if (Instrument != LastInstrument /*|| (m_iLastInstrument == MAX_INSTRUMENTS && Instrument != MAX_INSTRUMENTS)*/) {
		if (Instrument == MAX_INSTRUMENTS)
			Instrument = LastInstrument;
		else
			LastInstrument = Instrument;

		if ((pInstrument = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument)) == NULL)
			return;

		if (pInstrument->GetType() != INST_2A03)
			return;

		for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i) {
			if (m_iSeqIndex[i] != pInstrument->GetSeqIndex(i) || pInstrument->GetSeqEnable(i) == 0) {
				m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
				m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
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

		if (Instrument < MAX_INSTRUMENTS) {
			if ((pInstrument = (CInstrument2A03*)m_pDocument->GetInstrument(Instrument)) == NULL)
				return;
			if (pInstrument->GetType() != INST_2A03)
				return;
		}
	}

	if (Note == NONE) {
		if (Sweeping)
			m_cSweep = Sweep;
		// No note specified, stop here
		return;
	}
	
	if (Note == HALT) {
		CutNote();
		return;
	}

	if (!Sweeping && (m_cSweep != 0 || Sweep != 0)) {
		Sweep = 0;
		m_cSweep = 0;
		m_iLastPeriod = 0xFFFF;
	}
	else if (Sweeping) {
		m_cSweep = Sweep;
		m_iLastPeriod = 0xFFFF;
	}

	if (!m_bRelease) {

		if (pInstrument == NULL)
			return;

		// Trigger instrument
		for (int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; ++i) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}

		m_iNote			= RunNote(Octave, Note);
		m_iDutyPeriod	= m_iDefaultDuty;
		m_iSeqVolume	= InitVolume;
		m_bEnabled		= true;
	}
	else {
		ReleaseNote();
		ReleaseSequences(SNDCHIP_NONE);
	}

	if (PostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
		SetupSlide(PostEffect, PostEffectParam);
	else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
		m_iEffect = EF_NONE;
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

#ifdef _DEBUG
	ClearLog();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 1 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSquare1Chan::RefreshChannel()
{
	if (!m_bEnabled){
#ifdef _DEBUG
		AddLog(0);
#endif
		return;
	}

	int Period = CalculatePeriod(false);
	int Volume = CalculateVolume(15);
	char DutyCycle = (m_iDutyPeriod & 0x03);

	unsigned char HiFreq = (Period & 0xFF);
	unsigned char LoFreq = (Period >> 8);

#ifdef _DEBUG
	AddLog(Period);
#endif

	m_pAPU->Write(0x4000, (DutyCycle << 6) | 0x30 | Volume);

	if (m_cSweep) {
		if (m_cSweep & 0x80) {
			m_pAPU->Write(0x4001, m_cSweep);
			m_cSweep &= 0x7F;
			m_pAPU->Write(0x4017, 0x80);	// Clear sweep unit
			m_pAPU->Write(0x4017, 0x00);
			m_pAPU->Write(0x4002, HiFreq);
			m_pAPU->Write(0x4003, LoFreq);
			m_iLastPeriod = 0xFFFF;
		}
	}
	else {
		m_pAPU->Write(0x4001, 0x08);
		m_pAPU->Write(0x4017, 0x80);	// Manually execute one APU frame sequence to kill the sweep unit
		m_pAPU->Write(0x4017, 0x00);
		m_pAPU->Write(0x4002, HiFreq);
		
		if (LoFreq != (m_iLastPeriod >> 8))
			m_pAPU->Write(0x4003, LoFreq);
	}

	m_iLastPeriod = Period;
}

void CSquare1Chan::ClearRegisters()
{
	m_pAPU->Write(0x4000, 0x30);
	m_pAPU->Write(0x4001, 0x08);
	m_pAPU->Write(0x4002, 0x00);
	m_pAPU->Write(0x4003, 0x00);
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

	m_pAPU->Write(0x4004, (DutyCycle << 6) | 0x30 | Volume);

	if (m_cSweep) {
		if (m_cSweep & 0x80) {
			m_pAPU->Write(0x4005, m_cSweep);
			m_cSweep &= 0x7F;
			m_pAPU->Write(0x4017, 0x80);		// Clear sweep unit
			m_pAPU->Write(0x4017, 0x00);
			m_pAPU->Write(0x4006, HiFreq);
			m_pAPU->Write(0x4007, LoFreq);
			m_iLastPeriod = 0xFFFF;
		}
	}
	else {
		m_pAPU->Write(0x4005, 0x08);
		m_pAPU->Write(0x4017, 0x80);
		m_pAPU->Write(0x4017, 0x00);
		m_pAPU->Write(0x4006, HiFreq);
		
		if (LoFreq != LastLoFreq)
			m_pAPU->Write(0x4007, LoFreq);
	}
}

void CSquare2Chan::ClearRegisters()
{
	m_pAPU->Write(0x4004, 0x30);
	m_pAPU->Write(0x4005, 0x08);
	m_pAPU->Write(0x4006, 0x00);
	m_pAPU->Write(0x4007, 0x00);
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
	
	if (m_iSeqVolume > 0) {
		m_pAPU->Write(0x4008, 0x81);
		m_pAPU->Write(0x400A, HiFreq);
		m_pAPU->Write(0x400B, LoFreq);
	}
	else
		m_pAPU->Write(0x4008, 0);
}

void CTriangleChan::ClearRegisters()
{
	m_pAPU->Write(0x4008, 0);
//	m_pAPU->Write(0x4009, 0);		// these had to be disabled as the triangle generator is now better (more accurate)
//	m_pAPU->Write(0x400A, 0);
//	m_pAPU->Write(0x400B, 0);
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

	m_pAPU->Write(0x400C, 0x30 | Volume);
	m_pAPU->Write(0x400D, 0x00);
	m_pAPU->Write(0x400E, NoiseMode | Period);
	m_pAPU->Write(0x400F, 0x00);
}

void CNoiseChan::ClearRegisters()
{
	m_pAPU->Write(0x400C, 0x30);
	m_pAPU->Write(0x400D, 0);
	m_pAPU->Write(0x400E, 0);
	m_pAPU->Write(0x400F, 0);	
}

unsigned int CNoiseChan::TriggerNote(int Note)
{
	theApp.RegisterKeyState(m_iChannelID, Note);
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
};

void CDPCMChan::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	unsigned int Note, Octave, SampleIndex, LastInstrument;
	int EffPitch = -1;
	CInstrument2A03 *Inst;

	Note	= pNoteData->Note;
	Octave	= pNoteData->Octave;

	m_iRetrigger = 0;

	if (Note != NONE) {
		m_iNoteCut = 0;
	}

	for (int i = 0; i < EffColumns; i++) {
		switch (pNoteData->EffNumber[i]) {
		case EF_DAC:
			m_cDAC = pNoteData->EffParam[i] & 0x7F;
			break;
		case EF_SAMPLE_OFFSET:
			m_iOffset = pNoteData->EffParam[i];
			break;
		case EF_DPCM_PITCH:
			EffPitch = pNoteData->EffParam[i];
			break;
		case EF_RETRIGGER:
//			if (NoteData->EffParam[i] > 0) {
				m_iRetrigger = pNoteData->EffParam[i] + 1;
				if (m_iRetriggerCntr == 0)
					m_iRetriggerCntr = m_iRetrigger;
//			}
//			m_iEnableRetrigger = 1;
			break;
		case EF_NOTE_CUT:
			m_iNoteCut = pNoteData->EffParam[i] + 1;
			break;
		}
	}

	if (Note == 0)
		return;

	if (Note == RELEASE) {
		m_bRelease = true;
		return;
	}
	else
		m_bRelease = false;

	if (Note == HALT) {
		KillChannel();
		return;
	}

	LastInstrument = m_iInstrument;

	if (pNoteData->Instrument != 0x40)
		m_iInstrument = pNoteData->Instrument;

	if ((Inst = (CInstrument2A03*)m_pDocument->GetInstrument(m_iInstrument)) == NULL)
		return;

	if (Inst->GetType() != INST_2A03)
		return;

	// Change instrument
	if (pNoteData->Instrument != m_iLastInstrument) {
		if (pNoteData->Instrument == MAX_INSTRUMENTS)
			pNoteData->Instrument = LastInstrument;
		else
			LastInstrument = pNoteData->Instrument;

		m_iInstrument = pNoteData->Instrument;
	}
	else {
		if (pNoteData->Instrument == MAX_INSTRUMENTS)
			pNoteData->Instrument = m_iLastInstrument;
		else
			m_iLastInstrument = pNoteData->Instrument;
	}

	SampleIndex = Inst->GetSample(Octave, Note - 1);

	if (SampleIndex > 0) {
		int Pitch = Inst->GetSamplePitch(Octave, Note - 1);
		if (Pitch & 0x80)
			m_iLoop = 0x40;
		else
			m_iLoop = 0;

		if (EffPitch != -1)
			Pitch = EffPitch;
	
		m_iLoopOffset = Inst->GetSampleLoopOffset(Octave, Note - 1);

		CDSample *DSample = m_pDocument->GetDSample(SampleIndex - 1);

		int SampleSize = DSample->SampleSize;

		if (SampleSize > 0) {
			m_pSampleMem->SetMem(DSample->SampleData, SampleSize);
			Length = SampleSize;		// this will be adjusted
			m_iPeriod = Pitch & 0x0F;
			m_iSampleLength = (SampleSize >> 4) - (m_iOffset << 2);
			m_iLoopLength = SampleSize - m_iLoopOffset;
			m_bEnabled = true;

			m_iRetriggerCntr = m_iRetrigger;
		}
	}

	theApp.RegisterKeyState(m_iChannelID, (Note - 1) + (Octave * 12));
}

void CDPCMChan::RefreshChannel()
{
	if (m_cDAC != 255) {
		m_pAPU->Write(0x4011, m_cDAC);
		m_cDAC = 255;
	}

	if (m_iRetrigger != 0) {
		m_iRetriggerCntr--;
		if (m_iRetriggerCntr == 0) {
			m_iRetriggerCntr = m_iRetrigger;
			m_bEnabled = true;
		}
	}

	if (m_bRelease) {
		m_pAPU->Write(0x4015, 0x0F);
		m_bEnabled = false;
		m_bRelease = false;
	}

/*	
	if (m_bRelease) {
		// Release loop flag
		m_bRelease = false;
		m_pAPU->Write(0x4010, 0x00 | (m_iPeriod & 0x0F));
		return;
	}
*/	

	if (!m_bEnabled)
		return;

	m_pAPU->Write(0x4010, 0x00 | (m_iPeriod & 0x0F) | m_iLoop);
	m_pAPU->Write(0x4012, m_iOffset);							// load address, start at $C000
	m_pAPU->Write(0x4013, m_iSampleLength);						// length
	m_pAPU->Write(0x4015, 0x0F);
	m_pAPU->Write(0x4015, 0x1F);								// fire sample

	// Loop offset
	if (m_iLoopOffset > 0) {
		m_pAPU->Write(0x4012, m_iLoopOffset);
		m_pAPU->Write(0x4013, m_iLoopLength);
	}

	m_bEnabled = false;		// don't write to this channel anymore
}

void CDPCMChan::ClearRegisters()
{
qDebug("::ClearRegisters");
//	m_pAPU->Write(0x4015, 0x0F);
//	m_pAPU->Write(0x4010, 0);

//	if (!theApp.GetSettings()->General.bNoDPCMReset || theApp.IsPlaying()) {
//		m_pAPU->Write(0x4011, 0);		// regain full volume for TN
//	}

//	m_pAPU->Write(0x4012, 0);
//	m_pAPU->Write(0x4013, 0);

//	m_iOffset = 0;
//	m_cDAC = 255;
}
