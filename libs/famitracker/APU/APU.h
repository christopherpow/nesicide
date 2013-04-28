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

#ifndef _APU_H_
#define _APU_H_

#include <QObject>

//#define LOGGING

#include "../common.h"
#include "Mixer.h"

const uint8 SNDCHIP_NONE  = 0;
const uint8 SNDCHIP_VRC6  = 1;			// Konami VRCVI
const uint8 SNDCHIP_VRC7  = 2;			// Konami VRCVII
const uint8 SNDCHIP_FDS	  = 4;			// Famicom Disk Sound
const uint8 SNDCHIP_MMC5  = 8;			// Nintendo MMC5
const uint8 SNDCHIP_N163  = 16;			// Namco N-106
const uint8 SNDCHIP_S5B	  = 32;			// Sunsoft 5B

enum {MACHINE_NTSC, MACHINE_PAL};

// External classes
class CSquare;
class CTriangle;
class CNoise;
class CDPCM;

class CVRC6;
class CVRC7;
class CFDS;
class CMMC5;
class CN163;
class CS5B;

class CExternal;

class CAPU : public QObject {
   Q_OBJECT
public:
	CAPU(ICallback *pCallback, CSampleMem *pSampleMem);
	~CAPU();

	void	Reset();
	void	Process();
	void	AddTime(int32 Cycles);

	uint8	Read4015();
	void	Write4017(uint8 Value);
	void	Write4015(uint8 Value);
	void	Write(uint16 Address, uint8 Value);

	void	SetExternalSound(uint8 Chip);
	void	ExternalWrite(uint16 Address, uint8 Value);
	uint8	ExternalRead(uint16 Address);
	
	void	ChangeMachine(int Machine);
	bool	SetupSound(int SampleRate, int NrChannels, int Speed);
	void	SetupMixer(int LowCut, int HighCut, int HighDamp, int Volume) const;

	int32	GetVol(uint8 Chan) const;
	uint8	GetSamplePos() const;
	uint8	GetDeltaCounter() const;
	bool	DPCMPlaying() const;
	uint8	GetReg(int Chip, int Reg) const;

	void	SetChipLevel(int Chip, int Level);

	void	SetNamcoMixing(bool bLinear);

#ifdef LOGGING
	void	Log();
#endif

public:
	static const uint8	LENGTH_TABLE[];
	static const uint32	BASE_FREQ_NTSC;
	static const uint32	BASE_FREQ_PAL;
	static const uint8	FRAME_RATE_NTSC;
	static const uint8	FRAME_RATE_PAL;

private:
	static const int SEQUENCER_PERIOD;
	
private:
	inline void Clock_240Hz();
	inline void	Clock_120Hz();
	inline void	Clock_60Hz();
	inline void	ClockSequence();

	void EndFrame();
		
	void LogExternalWrite(uint16 Address, uint8 Value);

private:
	CMixer		*m_pMixer;
	ICallback	*m_pParent;

	// Internal channels
	CSquare		*m_pSquare1;
	CSquare		*m_pSquare2;
	CTriangle	*m_pTriangle;
	CNoise		*m_pNoise;
	CDPCM		*m_pDPCM;

	// Expansion chips
	CVRC6		*m_pVRC6;
	CMMC5		*m_pMMC5;
	CFDS		*m_pFDS;
	CN163		*m_pN163;
	CVRC7		*m_pVRC7;
	CS5B		*m_pS5B;

	uint8		m_iExternalSoundChip;				// External sound chip, if used

	uint32		m_iFramePeriod;						// Cycles per frame
	uint32		m_iFrameCycles;						// Cycles emulated from start of frame
	uint32		m_iSequencerClock;						// Clock for frame sequencer
	uint8		m_iFrameSequence;					// Frame sequence
	uint8		m_iFrameMode;						// 4 or 5-steps frame sequence

	uint32		m_iFrameCycleCount;
	uint32		m_iFrameClock;
	uint32		m_iCyclesToRun;						// Number of cycles to process

	uint32		m_iSoundBufferSamples;				// Size of buffer, in samples
	bool		m_bStereoEnabled;					// If stereo is enabled

	uint32		m_iSampleSizeShift;					// To convert samples to bytes
	uint32		m_iSoundBufferSize;					// Size of buffer, in samples
	uint32		m_iBufferPointer;					// Fill pos in buffer
	int16		*m_pSoundBuffer;					// Sound transfer buffer

	uint8		m_iRegs[0x20];
	uint8		m_iRegsVRC6[0x10];
	uint8		m_iRegsFDS[0x10];

	float		m_fLevelVRC7;
	float		m_fLevelS5B;

#ifdef LOGGING
	CFile		  *m_pLog;
	int			  m_iFrame = 0;
	unsigned char m_iRegs[32];
#endif

};

#endif /* _APU_H_ */
