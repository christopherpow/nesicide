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

#ifndef _MIXER_H_
#define _MIXER_H_

#include "../common.h"
#include "../Blip_Buffer/Blip_Buffer.h"

enum CHAN_IDS {
	CHANID_SQUARE1,
	CHANID_SQUARE2,
	CHANID_TRIANGLE,
	CHANID_NOISE,
	CHANID_DPCM,

	CHANID_VRC6_PULSE1,
	CHANID_VRC6_PULSE2,
	CHANID_VRC6_SAWTOOTH,

	CHANID_MMC5_SQUARE1,
	CHANID_MMC5_SQUARE2,
	CHANID_MMC5_VOICE,

	CHANID_N163_CHAN1,
	CHANID_N163_CHAN2,
	CHANID_N163_CHAN3,
	CHANID_N163_CHAN4,
	CHANID_N163_CHAN5,
	CHANID_N163_CHAN6,
	CHANID_N163_CHAN7,
	CHANID_N163_CHAN8,

	CHANID_FDS,

	CHANID_VRC7_CH1,
	CHANID_VRC7_CH2,
	CHANID_VRC7_CH3,
	CHANID_VRC7_CH4,
	CHANID_VRC7_CH5,
	CHANID_VRC7_CH6,

	CHANID_S5B_CH1,
	CHANID_S5B_CH2,
	CHANID_S5B_CH3,

	CHANNELS		/* Total number of channels */
};

class CMixer
{
	public:
		CMixer();
		~CMixer();

		void	ExternalSound(int Chip);
		void	AddValue(int ChanID, int Chip, int Value, int AbsValue, int FrameCycles);
		void	UpdateSettings(int LowCut,	int HighCut, int HighDamp, int OverallVol);

		bool	AllocateBuffer(unsigned int Size, uint32 SampleRate, uint8 NrChannels);
		void	SetClockRate(uint32 Rate);
		void	ClearBuffer();
		int		FinishBuffer(int t);
		int		SamplesAvail() const;

		void	MixSamples(blip_sample_t *pBuffer, uint32 Count);
		uint32	GetMixSampleCount(int t) const;

		void	AddSample(int ChanID, int Value);

		int		ReadBuffer(int Size, void *Buffer, bool Stereo);

		int32	GetChanOutput(uint8 Chan) const;

		void	SetChipLevel(int Chip, float Level);

		uint32	ResampleDuration(uint32 Time) const;

		void	SetNamcoMixing(bool bLinear);
		void	SetNamcoVolume(float fVol);

	private:
		inline double CalcPin1(double Val1, double Val2);
		inline double CalcPin2(double Val1, double Val2, double Val3);

		void MixInternal1(int Time);
		void MixInternal2(int Time);
		void MixN163(int Value, int Time);
		void MixFDS(int Value, int Time);
		void MixVRC6(int Value, int Time);
		void MixMMC5(int Value, int Time);
		void MixS5B(int Value, int Time);

		void StoreChannelLevel(int Channel, int Value);

		// Blip buffer synths
		Blip_Synth<blip_good_quality, -500>		Synth2A03SS;
		Blip_Synth<blip_good_quality, -500>		Synth2A03TND;
		Blip_Synth<blip_good_quality, -500>		SynthVRC6;
		Blip_Synth<blip_good_quality, -130>		SynthMMC5;	
		Blip_Synth<blip_good_quality, -1600>	SynthN163;
		Blip_Synth<blip_good_quality, -3500>	SynthFDS;
		Blip_Synth<blip_good_quality, -2000>	SynthS5B;
		

		// Blip buffer object
		Blip_Buffer	BlipBuffer;

		// Random variables
		int32		*m_pSampleBuffer;

		int32		m_iChannels[CHANNELS];
		uint8		m_iExternalChip;
		uint32		m_iSampleRate;

		float		m_fChannelLevels[CHANNELS];
		uint32		m_iChanLevelFallOff[CHANNELS];

		int			m_iLowCut;
		int			m_iHighCut;
		int			m_iHighDamp;
		int			m_iOverallVol;

		float		m_fDamping;

		float		m_fLevel2A03;
		float		m_fLevelVRC6;
		float		m_fLevelMMC5;
		float		m_fLevelFDS;

		bool		m_bNamcoMixing;
};

#endif /* _MIXER_H_ */
