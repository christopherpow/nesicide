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

// Famicom disk sound

#include <cmath>
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsFDS.h"
#include "SoundGen.h"

CChannelHandlerFDS::CChannelHandlerFDS() : CChannelHandler() 
{ 
	SetMaxPeriod(0xFFF);

	m_iSeqEnabled[SEQ_VOLUME] = 0;
	m_iSeqEnabled[SEQ_ARPEGGIO] = 0;
	m_iSeqEnabled[SEQ_PITCH] = 0;

	memset(m_iModTable, 0, 32);

	m_bResetMod = false;
}

void CChannelHandlerFDS::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	CInstrumentFDS *pInstrument = NULL;
	int PostEffect = 0, PostEffectParam;
	int EffModDepth = -1;
	int EffModSpeedHi = -1, EffModSpeedLo = -1;

	if (!CChannelHandler::CheckNote(pNoteData, INST_FDS))
		return;

	int Note	= pNoteData->Note;
	int Octave	= pNoteData->Octave;
	int Volume	= pNoteData->Vol;

	// Read volume
	if (Volume < 0x10) {
		m_iVolume = Volume << VOL_SHIFT;
	}

	if (m_iInstrument != MAX_INSTRUMENTS) {
		// Get instrument
		pInstrument = dynamic_cast<CInstrumentFDS*>(m_pDocument->GetInstrument(m_iInstrument));
	}

	if (pNoteData->Note == RELEASE)
		m_bRelease = true;
	else if (pNoteData->Note != NONE)
		m_bRelease = false;

	// Evaluate effects
	for (int i = 0; i < EffColumns; ++i) {
		unsigned char EffNum   = pNoteData->EffNumber[i];
		unsigned char EffParam = pNoteData->EffParam[i];

		if (EffNum == EF_PORTA_DOWN) {
			m_iPortaSpeed = EffParam;
			m_iEffect = EF_PORTA_UP;
		}
		else if (EffNum == EF_PORTA_UP) {
			m_iPortaSpeed = EffParam;
			m_iEffect = EF_PORTA_DOWN;
		}
		else if (!CheckCommonEffects(EffNum, EffParam)) {
			// Custom effects
			switch (EffNum) {
				case EF_SLIDE_UP:
				case EF_SLIDE_DOWN:
					PostEffect = EffNum;
					PostEffectParam = EffParam;
					SetupSlide(EffNum, EffParam);
					break;
				case EF_FDS_MOD_DEPTH:
					EffModDepth = EffParam & 0x3F;
					break;
				case EF_FDS_MOD_SPEED_HI:
					EffModSpeedHi = EffParam & 0x0F;
					break;
				case EF_FDS_MOD_SPEED_LO:
					EffModSpeedLo = EffParam;
					break;
			}
		}
	}

	// Load the instrument, only when a new instrument is loaded?
	if (Note != HALT && Note != RELEASE && m_iLastInstrument != m_iInstrument && pInstrument) {
		// TODO: check this in nsf
		FillWaveRAM(pInstrument);
		//if (pInstrument->GetModulationEnable())
			FillModulationTable(pInstrument);
	}

	if (Note == HALT) {
		CutNote();
		m_bEnabled = false;
//		m_iNote = 0x80;
	}
	else if (Note == RELEASE) {
		ReleaseNote();

		CChannelHandler::ReleaseSequence(SEQ_VOLUME, m_pVolumeSeq);
		CChannelHandler::ReleaseSequence(SEQ_ARPEGGIO, m_pArpeggioSeq);
		CChannelHandler::ReleaseSequence(SEQ_PITCH, m_pPitchSeq);
	}
	else if (Note != NONE) {

		if (pInstrument) {
			// Check instrument type
			if (pInstrument->GetType() != INST_FDS)
				return;
		}

		// Trigger a new note
		m_iNote	= RunNote(Octave, Note);
		m_bEnabled = true;
		m_bResetMod = true;
		m_iLastInstrument = m_iInstrument;

		m_iSeqVolume = 0x1F;

		if (pInstrument) {
			m_pVolumeSeq = pInstrument->GetVolumeSeq();
			m_pArpeggioSeq = pInstrument->GetArpSeq();
			m_pPitchSeq = pInstrument->GetPitchSeq();

			m_iSeqEnabled[SEQ_VOLUME] = (m_pVolumeSeq->GetItemCount() > 0) ? 1 : 0;
			m_iSeqPointer[SEQ_VOLUME] = 0;

			m_iSeqEnabled[SEQ_ARPEGGIO] = (m_pArpeggioSeq->GetItemCount() > 0) ? 1 : 0;
			m_iSeqPointer[SEQ_ARPEGGIO] = 0;

			m_iSeqEnabled[SEQ_PITCH] = (m_pPitchSeq->GetItemCount() > 0) ? 1 : 0;
			m_iSeqPointer[SEQ_PITCH] = 0;

//			if (pInstrument->GetModulationEnable()) {
				m_iModulationSpeed = pInstrument->GetModulationSpeed();
				m_iModulationDepth = pInstrument->GetModulationDepth();
				m_iModulationDelay = pInstrument->GetModulationDelay();
//			}
		}

		if (PostEffect && (m_iEffect == EF_SLIDE_UP || m_iEffect == EF_SLIDE_DOWN))
			SetupSlide(PostEffect, PostEffectParam);
		else if (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP)
			m_iEffect = EF_NONE;
	}

	if (EffModDepth != -1)
		m_iModulationDepth = EffModDepth;

	if (EffModSpeedHi != -1)
		m_iModulationSpeed = (m_iModulationSpeed & 0xFF) | (EffModSpeedHi << 8);

	if (EffModSpeedLo != -1)
		m_iModulationSpeed = (m_iModulationSpeed & 0xF00) | EffModSpeedLo;
}

void CChannelHandlerFDS::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();	

	// Sequences
	if (m_iSeqEnabled[SEQ_VOLUME])
		CChannelHandler::RunSequence(SEQ_VOLUME, m_pVolumeSeq);

	if (m_iSeqEnabled[SEQ_ARPEGGIO])
		CChannelHandler::RunSequence(SEQ_ARPEGGIO, m_pArpeggioSeq);

	if (m_iSeqEnabled[SEQ_PITCH])
		CChannelHandler::RunSequence(SEQ_PITCH, m_pPitchSeq);
}

void CChannelHandlerFDS::RefreshChannel()
{
	CheckWaveUpdate();	

	int Frequency = CalculatePeriod(true);
	unsigned char LoFreq = Frequency & 0xFF;
	unsigned char HiFreq = (Frequency >> 8) & 0x0F;

	unsigned char ModFreqLo = m_iModulationSpeed & 0xFF;
	unsigned char ModFreqHi = (m_iModulationSpeed >> 8) & 0x0F;

	unsigned char Volume = CalculateVolume(32);

	//if (m_iNote == 0x80)
	if (!m_bEnabled)
		Volume = 0;

	// Write frequency
	m_pAPU->ExternalWrite(0x4082, LoFreq);
	m_pAPU->ExternalWrite(0x4083, HiFreq);

	// Write volume, disable envelope
	m_pAPU->ExternalWrite(0x4080, 0x80 | Volume);

	if (m_bResetMod)
		m_pAPU->ExternalWrite(0x4085, 0);

	m_bResetMod = false;

	// Update modulation unit
	if (m_iModulationDelay == 0) {
		// Modulation frequency
		m_pAPU->ExternalWrite(0x4086, ModFreqLo);
		m_pAPU->ExternalWrite(0x4087, ModFreqHi);

		// Sweep depth, disable sweep envelope
		m_pAPU->ExternalWrite(0x4084, 0x80 | m_iModulationDepth); 
	}
	else {
		// Delayed modulation
		m_pAPU->ExternalWrite(0x4087, 0x80);
		m_iModulationDelay--;
	}

}

void CChannelHandlerFDS::ClearRegisters()
{	
	// Clear gain
	m_pAPU->ExternalWrite(0x4090, 0x00);

	// Clear volume
	m_pAPU->ExternalWrite(0x4080, 0x80);

	// Silence channel
	m_pAPU->ExternalWrite(0x4083, 0x80);

	// Default speed
	m_pAPU->ExternalWrite(0x408A, 0xFF);

	// Disable modulation
	m_pAPU->ExternalWrite(0x4087, 0x80);

	m_iSeqVolume = 0x20;

//	m_iNote = 0x80;
	m_bEnabled = false;

//	m_iLastInstrument = MAX_INSTRUMENTS;
//	m_iInstrument = 0;
}

void CChannelHandlerFDS::FillWaveRAM(CInstrumentFDS *pInst)
{
	// Fills the 64 byte waveform table
	// Enable write for waveform RAM
	m_pAPU->ExternalWrite(0x4089, 0x80);

	// This is the time the loop takes in NSF code
	AddCycles(1088);

	// Wave ram
	for (int i = 0; i < 0x40; ++i)
		m_pAPU->ExternalWrite(0x4040 + i, pInst->GetSample(i));

	// Disable write for waveform RAM, master volume = full
	m_pAPU->ExternalWrite(0x4089, 0x00);
}

void CChannelHandlerFDS::FillModulationTable(CInstrumentFDS *pInst)
{
	// Fills the 32 byte modulation table


	bool bNew(true);

	for (int i = 0; i < 32; ++i) {
		if (m_iModTable[i] != pInst->GetModulation(i)) {
			bNew = true;
			break;
		}
	}

	if (bNew) {
		// Copy table
		for (int i = 0; i < 32; ++i)
			m_iModTable[i] = pInst->GetModulation(i);

		// Disable modulation
		m_pAPU->ExternalWrite(0x4087, 0x80);
		// Reset modulation table pointer, set bias to zero
		m_pAPU->ExternalWrite(0x4085, 0x00);
		// Fill the table
		for (int i = 0; i < 32; ++i)
			m_pAPU->ExternalWrite(0x4088, m_iModTable[i]);
	}
}

void CChannelHandlerFDS::CheckWaveUpdate()
{
	// Check wave changes
	if (m_iInstrument != MAX_INSTRUMENTS && theApp.GetSoundGenerator()->HasWaveChanged()) {
		CInstrumentFDS *pInst = dynamic_cast<CInstrumentFDS*>(m_pDocument->GetInstrument(m_iInstrument));
		if (pInst != NULL && pInst->GetType() == INST_FDS) {
			// Realtime update
			m_iModulationSpeed = pInst->GetModulationSpeed();
			m_iModulationDepth = pInst->GetModulationDepth();
			FillWaveRAM(pInst);
			FillModulationTable(pInst);
		}
	}	
}
