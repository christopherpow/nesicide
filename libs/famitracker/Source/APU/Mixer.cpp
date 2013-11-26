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

/*

 This will mix and synthesize the APU audio using blargg's blip-buffer

 Mixing of internal audio relies on Blargg's findings

 Mixing of external channles are based on my own research:

 VRC6 (Madara): 
	Pulse channels has the same amplitude as internal-
    pulse channels on equal volume levels.

 FDS: 
	Square wave @ v = $1F: 2.4V
	  			  v = $0F: 1.25V
	(internal square wave: 1.0V)

 MMC5 (just breed): 
	2A03 square @ v = $0F: 760mV (the cart attenuates internal channels a little)
	MMC5 square @ v = $0F: 900mV

 VRC7:
	2A03 Square  @ v = $0F: 300mV (the cart attenuates internal channels a lot)
	VRC7 Patch 5 @ v = $0F: 900mV
	Did some more tests and found patch 14 @ v=15 to be 13.77dB stronger than a 50% square @ v=15

 ---

 N163 & 5B are still unknown

*/

#include "../stdafx.h"
#include <memory>
#include <cmath>
#include "Mixer.h"
#include "APU.h"
#include "emu2413.h"
#include "emu2149.h"

//#define LINEAR_MIXING

static const double AMP_2A03 = 400.0;

static const float LEVEL_FALL_OFF_RATE	= 0.6f;
static const int   LEVEL_FALL_OFF_DELAY = 3;

CMixer::CMixer()
{
	memset(m_iChannels, 0, sizeof(int32) * CHANNELS);
	memset(m_fChannelLevels, 0, sizeof(float) * CHANNELS);
	memset(m_iChanLevelFallOff, 0, sizeof(uint32) * CHANNELS);

	m_fLevel2A03 = 1.0f;
	m_fLevelVRC6 = 1.0f;
	m_fLevelMMC5 = 1.0f;
	m_fLevelFDS = 1.0f;
   
   m_iLowCut = 0;
   m_iHighCut = 0;
   m_iHighDamp = 0;
   m_iOverallVol = 0;

	m_bNamcoMixing = false;
}

CMixer::~CMixer()
{
}

inline double CMixer::CalcPin1(double Val1, double Val2)
{
	// Mix the output of APU audio pin 1: square
	//

	if ((Val1 + Val2) > 0)
		return 95.88 / ((8128.0 / (Val1 + Val2)) + 100.0);

	return 0;
}

inline double CMixer::CalcPin2(double Val1, double Val2, double Val3)
{
	// Mix the output of APU audio pin 2: triangle, noise and DPCM
	//

	if ((Val1 + Val2 + Val3) > 0)
		return 159.79 / ((1.0 / ((Val1 / 8227.0) + (Val2 / 12241.0) + (Val3 / 22638.0))) + 100.0);

	return 0;
}

void CMixer::ExternalSound(int Chip)
{
	m_iExternalChip = Chip;
	UpdateSettings(m_iLowCut, m_iHighCut, m_iHighDamp, m_iOverallVol);
}

void CMixer::SetNamcoMixing(bool bLinear)
{
	m_bNamcoMixing = bLinear;
}

void CMixer::SetChipLevel(int Chip, float Level)
{
	switch (Chip) {
		case SNDCHIP_NONE:
			m_fLevel2A03 = Level;
			break;
		case SNDCHIP_VRC6:
			m_fLevelVRC6 = Level;
			break;
		case SNDCHIP_MMC5:
			m_fLevelMMC5 = Level;
			break;
		case SNDCHIP_FDS:
			m_fLevelFDS = Level;
			break;
	}
}

void CMixer::UpdateSettings(int LowCut,	int HighCut, int HighDamp, int OverallVol)
{
	float fVolume = float(OverallVol) / 100.0f;

	m_fDamping = 1.0f;

	if (m_iExternalChip & SNDCHIP_VRC7)
		// Decrease the internal audio when VRC7 is enabled to increase the headroom
		m_fDamping *= 0.34f;
	else
		m_fDamping *= 1.0f;

	if (m_iExternalChip & SNDCHIP_N163)
		m_fDamping *= 0.9f;
	else
		m_fDamping *= 1.0f;

	fVolume *= m_fDamping;

	// Blip-buffer filtering
	BlipBuffer.bass_freq(LowCut);

	blip_eq_t eq(-HighDamp, HighCut, m_iSampleRate);

	Synth2A03SS.treble_eq(eq);
	Synth2A03TND.treble_eq(eq);
	SynthVRC6.treble_eq(eq);
	SynthMMC5.treble_eq(eq);
	SynthS5B.treble_eq(eq);

	double n163_treble = 24;
	long n163_rolloff = 12000;

	if (HighDamp > n163_treble)
		n163_treble = HighDamp;

	if (n163_rolloff > HighCut)
		n163_rolloff = HighCut;

	blip_eq_t eq_n163(-n163_treble, n163_rolloff, m_iSampleRate);
	SynthN163.treble_eq(eq_n163);

	// Checked against hardware
	blip_eq_t fds_eq(-48, 1000, m_iSampleRate);
	SynthFDS.treble_eq(fds_eq);
	Synth2A03SS.volume(fVolume * m_fLevel2A03);
	Synth2A03TND.volume(fVolume * m_fLevel2A03);
	SynthVRC6.volume(fVolume * 3.98333f * m_fLevelVRC6);
	SynthFDS.volume(fVolume * 1.00f * m_fLevelFDS);
	SynthMMC5.volume(fVolume * 1.18421f * m_fLevelMMC5);
	
	// Not checked
	SynthN163.volume(fVolume * 1.1f * (m_bNamcoMixing ? 0.8f : 1.0f));
	SynthS5B.volume(fVolume * 1.0f);

	m_iLowCut = LowCut;
	m_iHighCut = HighCut;
	m_iHighDamp = HighDamp;
	m_iOverallVol = OverallVol;
}

void CMixer::SetNamcoVolume(float fVol)
{
	float fVolume = fVol * float(m_iOverallVol) / 100.0f;

	SynthN163.volume(fVolume * 1.1f * (m_bNamcoMixing ? 0.8f : 1.0f));
}

void CMixer::MixSamples(blip_sample_t *pBuffer, uint32 Count)
{
	// For VRC7
	BlipBuffer.mix_samples(pBuffer, Count);
}

uint32 CMixer::GetMixSampleCount(int t) const
{
	return BlipBuffer.count_samples(t);
}

bool CMixer::AllocateBuffer(unsigned int BufferLength, uint32 SampleRate, uint8 NrChannels)
{
	m_iSampleRate = SampleRate;
	BlipBuffer.sample_rate(SampleRate, (BufferLength * 1000 * 2) / SampleRate);
	return true;
}

void CMixer::SetClockRate(uint32 Rate)
{
	// Change the clockrate
	BlipBuffer.clock_rate(Rate);
}

void CMixer::ClearBuffer()
{
	BlipBuffer.clear();
}

int CMixer::SamplesAvail() const
{	
	return (int)BlipBuffer.samples_avail();
}

int CMixer::FinishBuffer(int t)
{
	BlipBuffer.end_frame(t);

	// Get channel levels for VRC7
	for (int i = 0; i < 6; ++i)
		StoreChannelLevel(CHANID_VRC7_CH1 + i, OPLL_getchanvol(i));

	// Get channel levels for Sunsoft
	for (int i = 0; i < 3; ++i)
		StoreChannelLevel(CHANID_S5B_CH1 + i, PSG_getchanvol(i));

	for (int i = 0; i < CHANNELS; ++i) {
		if (m_iChanLevelFallOff[i] > 0)
			m_iChanLevelFallOff[i]--;
		else {
			if (m_fChannelLevels[i] > 0) {
				m_fChannelLevels[i] -= LEVEL_FALL_OFF_RATE;
				if (m_fChannelLevels[i] < 0)
					m_fChannelLevels[i] = 0;
			}
		}
	}

	// Return number of samples available
	return BlipBuffer.samples_avail();
}

//
// Mixing
//

void CMixer::MixInternal1(int Time)
{
	static double LastSum;
	double Sum, Delta;

#ifdef LINEAR_MIXING
	SumL = ((m_iChannels[CHANID_SQUARE1].Left + m_iChannels[CHANID_SQUARE2].Left) * 0.00752) * InternalVol;
	SumR = ((m_iChannels[CHANID_SQUARE1].Right + m_iChannels[CHANID_SQUARE2].Right) *  0.00752) * InternalVol;
#else
	Sum = CalcPin1(m_iChannels[CHANID_SQUARE1], m_iChannels[CHANID_SQUARE2]);
#endif

	Delta = (Sum - LastSum) * AMP_2A03;
	Synth2A03SS.offset(Time, (int)Delta, &BlipBuffer);
	LastSum = Sum;
}

void CMixer::MixInternal2(int Time)
{
	static double LastSum;
	double Sum, Delta;

#ifdef LINEAR_MIXING
	SumL = ((0.00851 * m_iChannels[CHANID_TRIANGLE].Left + 0.00494 * m_iChannels[CHANID_NOISE].Left + 0.00335 * m_iChannels[CHANID_DPCM].Left)) * InternalVol;
	SumR = ((0.00851 * m_iChannels[CHANID_TRIANGLE].Right + 0.00494 * m_iChannels[CHANID_NOISE].Right + 0.00335 * m_iChannels[CHANID_DPCM].Right)) * InternalVol;
#else
	Sum = CalcPin2(m_iChannels[CHANID_TRIANGLE], m_iChannels[CHANID_NOISE], m_iChannels[CHANID_DPCM]);
#endif

	Delta = (Sum - LastSum) * AMP_2A03;
	Synth2A03TND.offset(Time, (int)Delta, &BlipBuffer);
	LastSum = Sum;
}

void CMixer::MixN163(int Value, int Time)
{
	SynthN163.offset(Time, Value, &BlipBuffer);
}

void CMixer::MixFDS(int Value, int Time)
{
	SynthFDS.offset(Time, Value, &BlipBuffer);
}

void CMixer::MixVRC6(int Value, int Time)
{
	SynthVRC6.offset(Time, Value, &BlipBuffer);
}

void CMixer::MixMMC5(int Value, int Time)
{
	SynthMMC5.offset(Time, Value, &BlipBuffer);
}

void CMixer::MixS5B(int Value, int Time)
{
	SynthS5B.offset(Time, Value, &BlipBuffer);
}

void CMixer::AddValue(int ChanID, int Chip, int Value, int AbsValue, int FrameCycles)
{
	// Add sound to mixer
	//
	
	int Delta = Value - m_iChannels[ChanID];
	StoreChannelLevel(ChanID, AbsValue);
	m_iChannels[ChanID] = Value;

	switch (Chip) {
		case SNDCHIP_NONE:
			switch (ChanID) {
				case CHANID_SQUARE1:
				case CHANID_SQUARE2:
					MixInternal1(FrameCycles);
					break;
				case CHANID_TRIANGLE:
				case CHANID_NOISE:
				case CHANID_DPCM:
					MixInternal2(FrameCycles);
					break;
			}
			break;
		case SNDCHIP_N163:
			MixN163(Value, FrameCycles);
			break;
		case SNDCHIP_FDS:
			MixFDS(Value, FrameCycles);
			break;
		case SNDCHIP_MMC5:
			MixMMC5(Delta, FrameCycles);
			break;
		case SNDCHIP_VRC6:
			MixVRC6(Value, FrameCycles);
			break;
	}
}

int CMixer::ReadBuffer(int Size, void *Buffer, bool Stereo)
{
	return BlipBuffer.read_samples((blip_sample_t*)Buffer, Size);
}

int32 CMixer::GetChanOutput(uint8 Chan) const
{
	return (int32)m_fChannelLevels[Chan];
}

void CMixer::StoreChannelLevel(int Channel, int Value)
{
	int AbsVol = abs(Value);

	// Adjust channel levels for some channels
	if (Channel == CHANID_VRC6_SAWTOOTH)
		AbsVol = (AbsVol * 3) / 4;

	if (Channel == CHANID_DPCM)
		AbsVol /= 8;

	if (Channel == CHANID_FDS)
		AbsVol = AbsVol / 38;

	if (Channel >= CHANID_N163_CHAN1 && Channel <= CHANID_N163_CHAN8) {
		AbsVol /= 15;
		Channel = (7 - (Channel - CHANID_N163_CHAN1)) + CHANID_N163_CHAN1;
	}

	if (Channel >= CHANID_VRC7_CH1 && Channel <= CHANID_VRC7_CH6) {
		AbsVol = (int)(logf((float)AbsVol) * 3.0f);
	}

	if (Channel >= CHANID_S5B_CH1 && Channel <= CHANID_S5B_CH3) {
		AbsVol /= 16;
		// TODO: log -> linear conversion
	}

	if (float(AbsVol) >= m_fChannelLevels[Channel]) {
		m_fChannelLevels[Channel] = float(AbsVol);
		m_iChanLevelFallOff[Channel] = LEVEL_FALL_OFF_DELAY;
	}
}

uint32 CMixer::ResampleDuration(uint32 Time) const
{
	return (uint32)BlipBuffer.resampled_duration((blip_time_t)Time);
}
