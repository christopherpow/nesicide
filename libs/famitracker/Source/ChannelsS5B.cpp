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

// Sunsoft 5B (YM2149/AY-3-8910)

#include <cmath>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsS5B.h"
#include "SoundGen.h"
#include "APU/APU.h"

// Static member variables, for the shared stuff in 5B
int			  CChannelHandlerS5B::m_iModes		= 0;
int			  CChannelHandlerS5B::m_iNoiseFreq	= 0;
unsigned char CChannelHandlerS5B::m_iEnvFreqHi	= 0;
unsigned char CChannelHandlerS5B::m_iEnvFreqLo	= 0;
int			  CChannelHandlerS5B::m_iEnvType	= 0;
bool		  CChannelHandlerS5B::m_bRegsDirty	= false;

// Class functions

void CChannelHandlerS5B::SetEnvelopeHigh(int Val)
{
	m_iEnvFreqHi = Val;
	m_bRegsDirty = true;
}

void CChannelHandlerS5B::SetEnvelopeLow(int Val)
{
	m_iEnvFreqLo = Val;
	m_bRegsDirty = true;
}

void CChannelHandlerS5B::SetEnvelopeType(int Val)
{
	m_iEnvType = Val;
	m_bRegsDirty = true;
}

void CChannelHandlerS5B::SetMode(int Chan, int Square, int Noise)
{
	int initModes = m_iModes;

	switch (Chan) {
		case 0:
			m_iModes &= 0x36;
			break;
		case 1:
			m_iModes &= 0x2D;
			break;
		case 2:
			m_iModes &= 0x1B;
			break;
	}

	m_iModes |= (Noise << (3 + Chan)) | (Square << Chan);
	
	if (m_iModes != initModes) {
		m_bRegsDirty = true;
	}
}

void CChannelHandlerS5B::SetNoiseFreq(int Freq)
{
	m_iNoiseFreq = Freq;
	m_bRegsDirty = true;
}

void CChannelHandlerS5B::UpdateRegs(CAPU *pAPU)
{
	if (!m_bRegsDirty)
		return;

	// Done only once
	pAPU->ExternalWrite(0xC000, 0x07);
	pAPU->ExternalWrite(0xE000, m_iModes);

	pAPU->ExternalWrite(0xC000, 0x06);
	pAPU->ExternalWrite(0xE000, m_iNoiseFreq);

	pAPU->ExternalWrite(0xC000, 0x0B);
	pAPU->ExternalWrite(0xE000, m_iEnvFreqLo);

	pAPU->ExternalWrite(0xC000, 0x0C);
	pAPU->ExternalWrite(0xE000, m_iEnvFreqHi);

	pAPU->ExternalWrite(0xC000, 0x0D);
	pAPU->ExternalWrite(0xE000, m_iEnvType);

	m_bRegsDirty = false;
}

// Instance functions

CChannelHandlerS5B::CChannelHandlerS5B() : 
	CChannelHandler(0xFFF, 0x0F), 
	m_iNoiseOffset(0), 
	m_bUpdate(false)
{
}

/*
bool NoteValid(int Note)
{
	return (Note != NONE && Note != HALT && Note != RELEASE);
}
*/

void CChannelHandlerS5B::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	CChannelHandler::HandleNoteData(pNoteData, EffColumns);
}

void CChannelHandlerS5B::HandleCustomEffects(int EffNum, int EffParam)
{
	if (!CheckCommonEffects(EffNum, EffParam)) {
		switch (EffNum) {
			case EF_SUNSOFT_ENV_HI:
				SetEnvelopeHigh(EffParam);
				break;
			case EF_SUNSOFT_ENV_LO:
				SetEnvelopeLow(EffParam);
				break;
			case EF_SUNSOFT_ENV_TYPE:
				SetEnvelopeType(EffParam);
				m_bEnvEnable = true;
				m_bUpdate = true;
				break;

				/*
			case EF_SLIDE_UP:
			case EF_SLIDE_DOWN:
				PostEffect = EffCmd;
				PostEffectParam = EffParam;
				SetupSlide(EffCmd, EffParam);
				break;
				*/
		}
	}
}

bool CChannelHandlerS5B::HandleInstrument(int Instrument, bool Trigger, bool NewInstrument)
{
	CFamiTrackerDoc *pDocument = m_pSoundGen->GetDocument();
	CInstrumentContainer<CInstrumentS5B> instContainer(pDocument, Instrument);
	CInstrumentS5B *pInstrument = instContainer();

	if (pInstrument == NULL)
		return false;

	for (int i = 0; i < SEQ_COUNT; ++i) {
		const CSequence *pSequence = pDocument->GetSequence(SNDCHIP_S5B, pInstrument->GetSeqIndex(i), i);
		if (Trigger || !IsSequenceEqual(i, pSequence) || pInstrument->GetSeqEnable(i) > GetSequenceState(i)) {
			if (pInstrument->GetSeqEnable(i) == 1)
				SetupSequence(i, pSequence);
			else
				ClearSequence(i);
		}
	}

	return true;
}

void CChannelHandlerS5B::HandleEmptyNote()
{
}

void CChannelHandlerS5B::HandleCut()
{
	CutNote();
	m_iNote = 0;
}

void CChannelHandlerS5B::HandleRelease()
{
	if (!m_bRelease) {
		ReleaseNote();
		m_bUpdate = true;
	}
}

void CChannelHandlerS5B::HandleNote(int Note, int Octave)
{
	m_iNote	= RunNote(Octave, Note);
	m_iSeqVolume = 0xF;

	m_iDutyPeriod = S5B_MODE_SQUARE;

	m_bUpdate = true;
}

void CChannelHandlerS5B::ProcessChannel()
{
	CFamiTrackerDoc *pDocument = m_pSoundGen->GetDocument();

	// Default effects
	CChannelHandler::ProcessChannel();

	// Sequences
	for (int i = 0; i < SEQ_COUNT; ++i)
		RunSequence(i);
}

void CChannelHandlerS5B::WriteReg(int Reg, int Value)
{
	m_pAPU->ExternalWrite(0xC000, Reg);
	m_pAPU->ExternalWrite(0xE000, Value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 1 
///////////////////////////////////////////////////////////////////////////////////////////////////////////


void CS5BChannel1::RefreshChannel()
{
	if (!m_bUpdate)
		return;

	m_bUpdate = false;

	int Period = CalculatePeriod();
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume();

	unsigned char Noise = (m_iDutyPeriod & S5B_MODE_NOISE) ? 0 : 1;
	unsigned char Square = (m_iDutyPeriod & S5B_MODE_SQUARE) ? 0 : 1;
	unsigned char NoisePeriod = (m_iDutyPeriod + m_iNoiseOffset) & 0x1F;
	unsigned char Envelope = m_bEnvEnable ? 0x10 : 0x00;

	SetMode(0, Square, Noise);

	WriteReg(0, LoPeriod);
	WriteReg(1, HiPeriod);
	WriteReg(8, Volume | Envelope);

	if (!Noise)
		SetNoiseFreq(NoisePeriod);

//	UpdateRegs(m_pAPU);
}

void CS5BChannel1::ClearRegisters()
{
	//WriteReg(7, 0x38);
	WriteReg(8, 0);		// Clear volume
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Square 2 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CS5BChannel2::RefreshChannel()
{
	int Period = CalculatePeriod();
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume();

	unsigned char Noise = (m_iDutyPeriod & S5B_MODE_NOISE) ? 0 : 1;
	unsigned char Square = (m_iDutyPeriod & S5B_MODE_SQUARE) ? 0 : 1;
	unsigned char NoisePeriod = (m_iDutyPeriod + m_iNoiseOffset) & 0x1F;
	unsigned char Envelope = 0;

	//WriteReg(7, 0x30 | (Noise << 3) | 0x06 | Square);

	SetMode(1, Square, Noise);

	WriteReg(2, LoPeriod);
	WriteReg(3, HiPeriod);
	WriteReg(9, Volume);

	if (!Noise)
		SetNoiseFreq(NoisePeriod);
}

void CS5BChannel2::ClearRegisters()
{
	WriteReg(9, 0);		// Clear volume
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Channel 3 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CS5BChannel3::RefreshChannel()
{
/*
	if (!m_bUpdate)
		return;

	m_bUpdate = true;
*/
	int Period = CalculatePeriod();
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume();

	unsigned char Noise = (m_iDutyPeriod & S5B_MODE_NOISE) ? 0 : 1;
	unsigned char Square = (m_iDutyPeriod & S5B_MODE_SQUARE) ? 0 : 1;
	unsigned char NoisePeriod = (m_iDutyPeriod + m_iNoiseOffset) & 0x1F;
	unsigned char Envelope = 0;

	SetMode(2, Square, Noise);

	WriteReg(4, LoPeriod);
	WriteReg(5, HiPeriod);
	WriteReg(10, Volume | Envelope);

	if (!Noise)
		SetNoiseFreq(NoisePeriod);

	UpdateRegs(m_pAPU);
}

void CS5BChannel3::ClearRegisters()
{
	WriteReg(10, 0);		// Clear volume
}

