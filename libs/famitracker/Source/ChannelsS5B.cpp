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

// Sunsoft 5B (YM2149/AY-3-8910)

#include <cmath>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsS5B.h"

//const int CChannelHandlerS5B::SEQ_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_SUNSOFT_NOISE};

static int gModes = 0;
static int gNoiseFreq = 0;
static unsigned char gEnvFreqHi = 0;
static unsigned char gEnvFreqLo = 0;
static int gEnvType = 0;
static bool gRegsDirty = false;

static void SetEnvelopeHigh(int Val)
{
	gEnvFreqHi = Val;
	gRegsDirty = true;
}

static void SetEnvelopeLow(int Val)
{
	gEnvFreqLo = Val;
	gRegsDirty = true;
}

static void SetEnvelopeType(int Val)
{
	gEnvType = Val;
	gRegsDirty = true;
}

static void SetMode(int Chan, int Square, int Noise)
{
	int initModes = gModes;

	switch (Chan) {
		case 0:
			gModes &= 0x36;
			break;
		case 1:
			gModes &= 0x2D;
			break;
		case 2:
			gModes &= 0x1B;
			break;
	}

	gModes |= (Noise << (3 + Chan)) | (Square << Chan);
	
	if (gModes != initModes) {
		gRegsDirty = true;
	}
}

static void SetNoiseFreq(int Freq)
{
	gNoiseFreq = Freq;
	gRegsDirty = true;
}

static void UpdateRegs(CAPU *pAPU)
{
	if (!gRegsDirty)
		return;

	// Done only once
	pAPU->ExternalWrite(0xC000, 0x07);
	pAPU->ExternalWrite(0xE000, gModes);

	pAPU->ExternalWrite(0xC000, 0x06);
	pAPU->ExternalWrite(0xE000, gNoiseFreq);

	pAPU->ExternalWrite(0xC000, 0x0B);
	pAPU->ExternalWrite(0xE000, gEnvFreqLo);

	pAPU->ExternalWrite(0xC000, 0x0C);
	pAPU->ExternalWrite(0xE000, gEnvFreqHi);

	pAPU->ExternalWrite(0xC000, 0x0D);
	pAPU->ExternalWrite(0xE000, gEnvType);

	gRegsDirty = false;
}

CChannelHandlerS5B::CChannelHandlerS5B() : CChannelHandler(), m_iNoiseOffset(0), m_bUpdate(false)
{
	SetMaxPeriod(0xFFF);
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
	CInstrumentS5B *pInstrument = (CInstrumentS5B*)m_pDocument->GetInstrument(m_iInstrument);

	if (pInstrument == NULL)
		return false;

	if (pInstrument->GetType() != INST_S5B) {
		pInstrument->Release();
		return false;
	}

	for (int i = 0; i < SEQ_COUNT; ++i) {
		if (pInstrument->GetSeqIndex(i) != m_iSeqIndex[i] || pInstrument->GetSeqEnable(i) != m_iSeqEnabled[i] || Trigger) {
			m_iSeqEnabled[i] = pInstrument->GetSeqEnable(i);
			m_iSeqIndex[i]	 = pInstrument->GetSeqIndex(i);
			m_iSeqPointer[i] = 0;
		}
	}

	pInstrument->Release();

	return true;
}

void CChannelHandlerS5B::HandleEmptyNote()
{
}

void CChannelHandlerS5B::HandleHalt()
{
	CutNote();
	m_bEnabled = false;
	m_iNote = 0;
}

void CChannelHandlerS5B::HandleRelease()
{
	ReleaseNote();
	m_bUpdate = true;
}

void CChannelHandlerS5B::HandleNote(int Note, int Octave)
{
	m_iNote	= RunNote(Octave, Note);
	m_iSeqVolume = 0xF;
	m_bEnabled = true;

	m_iDutyPeriod = S5B_MODE_SQUARE;

	m_bUpdate = true;
}

void CChannelHandlerS5B::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();

	if (!m_bEnabled)
		return;

	// Sequences
	for (int i = 0; i < SEQ_COUNT; ++i)
		CChannelHandler::RunSequence(i, m_pDocument->GetSequence(SNDCHIP_S5B, m_iSeqIndex[i], i));
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
//	if (!m_bEnabled)
//		return;

	if (!m_bUpdate)
		return;

	m_bUpdate = false;

	int Period = CalculatePeriod(false);
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume(15);

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

//	m_bEnabled = false;
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
	if (!m_bEnabled)
		return;

	int Period = CalculatePeriod(false);
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume(15);

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
	//if (!m_bEnabled)
	//	return;
/*
	if (!m_bUpdate)
		return;

	m_bUpdate = true;
*/
	int Period = CalculatePeriod(false);
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume(15);

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
