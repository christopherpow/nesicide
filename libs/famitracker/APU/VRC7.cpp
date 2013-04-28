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

#include "../stdafx.h"
#include <memory>
#include "APU.h"
#include "VRC7.h"

const float  CVRC7::AMPLIFY	  = 2.88f;		// Mixing amplification, VRC7 patch 14 is 4,88 times stronger than a 50% square @ v=15
const uint32 CVRC7::OPL_CLOCK = 3579545;	// Clock frequency

CVRC7::CVRC7(CMixer *pMixer) : CExternal(pMixer), m_pBuffer(NULL), m_pOPLLInt(NULL), m_fVolume(1.0f)
{
	Reset();
}

CVRC7::~CVRC7()
{
	if (m_pOPLLInt != NULL) {
		OPLL_delete(m_pOPLLInt);
		m_pOPLLInt = NULL;
	}

	SAFE_RELEASE_ARRAY(m_pBuffer);
}

void CVRC7::Reset()
{
	m_iBufferPtr = 0;
	m_iTime = 0;
}

void CVRC7::SetSampleSpeed(uint32 SampleRate, double ClockRate, uint32 FrameRate)
{
	if (m_pOPLLInt != NULL) {
		OPLL_delete(m_pOPLLInt);
		m_pOPLLInt = NULL;
	}

	m_pOPLLInt = OPLL_new(OPL_CLOCK, SampleRate);

	OPLL_reset(m_pOPLLInt);
	OPLL_reset_patch(m_pOPLLInt, 1);

	m_iMaxSamples = (SampleRate / FrameRate) * 2;	// Allow some overflow

	SAFE_RELEASE_ARRAY(m_pBuffer);
	m_pBuffer = new int16[m_iMaxSamples];
	memset(m_pBuffer, 0, sizeof(int16) * m_iMaxSamples);
}

void CVRC7::SetVolume(float Volume)
{
	m_fVolume = Volume * AMPLIFY;
}

void CVRC7::Write(uint16 Address, uint8 Value)
{
	switch (Address) {
		case 0x9010:
			m_iSoundReg = Value;
			break;
		case 0x9030:
			OPLL_writeReg(m_pOPLLInt, m_iSoundReg, Value);
			break;
	}
}

uint8 CVRC7::Read(uint16 Address, bool &Mapped)
{
	return 0;
}

void CVRC7::EndFrame()
{
	uint32 WantSamples = m_pMixer->GetMixSampleCount(m_iTime);

	static int32 LastSample = 0;

	// Generate VRC7 samples
	while (m_iBufferPtr < WantSamples) {
		int32 Sample = int(float(OPLL_calc(m_pOPLLInt)) * m_fVolume);
		m_pBuffer[m_iBufferPtr++] = int16((Sample + LastSample) >> 1);
		LastSample = Sample;
	}

	m_pMixer->MixSamples((blip_sample_t*)m_pBuffer, WantSamples);

	m_iBufferPtr -= WantSamples;
	m_iTime = 0;
}

void CVRC7::Process(uint32 Time)
{
	// This cannot run in sync, fetch all samples at end of frame instead
	m_iTime += Time;
}
