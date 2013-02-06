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

#ifndef _FDS_H_
#define _FDS_H_

#include "external.h"
#include "channel.h"

class CFDS : public CExternal, CExChannel {
public:
	CFDS(CMixer *pMixer);
	virtual ~CFDS();
//	void	Init(CMixer *pMixer);
	void	Reset();
	void	Write(uint16 Address, uint8 Value);
	uint8	Read(uint16 Address, bool &Mapped);
	void	EndFrame();
	void	Process(uint32 Time);
private:
	// Volume envelope variables
	uint8	m_iVolumeEnvDisable;			// Volume envelope, 1 = disabled
	uint8	m_iVolumeEnvMode;				// Envelope mode, 1 = increase
	uint8	m_iVolumeEnvSpeed;				// Volume envelope speed

	// Frequency and control
	uint16	m_iFrequency;					// Frequency, 12 bits
	uint8	m_iDisabled;					// Channel disable
	uint8	m_iEnvelopeDisable;				// Envelope disabled

	// Sweep envelope variables
	uint8	m_iSweepEnvDisable;				// Sweep envelope, 1 = enabled
	uint8	m_iSweepEnvMode;				// Sweep envelope mode
	uint8	m_iSweepEnvSpeed;
	int8	m_iSweepBias;

	// Modulation unit variables
	uint8	m_iModTableAddress;				// Modulation table address
	uint16	m_iModFrequency;				// Modulation frequency
	uint8	m_iModDisable;

	uint8	m_iWriteMode;					// Enable writes to the wave table
	uint8	m_iMasterVolume;				// Channel master volume

	uint8	m_iEnvelopeSpeed;				// Envelope speed

	// Counters
	uint16	m_iVolumeEnvCounter;
	uint16	m_iSweepEnvCounter;
	uint16	m_iModCounter;

	uint32	m_iWaveGenCounter;				// Cycle counter for wave gen.

	uint8	m_iVolumeGain;
	uint8	m_iSweepGain;

	uint8	m_iWaveReadPointer;
	int16	m_iModAlt;

	uint8	m_iWaveTable[0x40];		// Waveform	table
	uint8	m_iModTable[0x40];		// Frequency modulation table
};

#endif /* _FDS_H_ */