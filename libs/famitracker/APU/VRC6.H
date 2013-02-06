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

#ifndef _VRC6_H_
#define _VRC6_H_

#include "external.h"
#include "channel.h"

class CVRC6_Pulse : public CExChannel {
public:
	CVRC6_Pulse(CMixer *pMixer, int ID) : CExChannel(pMixer, SNDCHIP_VRC6, ID) {};
	void Reset();
	void Write(uint16 Address, uint8 Value);
	void Process(int Time);

private:
	uint8	DutyCycle, Volume, Gate, Enabled;
	uint32	Frequency;
	uint8	FreqLow, FreqHigh;
	int32	Counter;
	uint8	DutyCycleCounter;
};

class CVRC6_Sawtooth : public CExChannel {
public:
	CVRC6_Sawtooth(CMixer *pMixer, int ID) : CExChannel(pMixer, SNDCHIP_VRC6, ID) {};
	void Reset();
	void Write(uint16 Address, uint8 Value);
	void Process(int Time);

private:
	uint8	PhaseAccumulator, PhaseInput, Enabled, ResetReg;
	uint32	Frequency;
	uint8	FreqLow, FreqHigh;
	int32	Counter;
};

class CVRC6 : public CExternal {
public:
	CVRC6(CMixer *pMixer);
	virtual ~CVRC6();
	void Reset();
	void Write(uint16 Address, uint8 Value);
	uint8 Read(uint16 Address, bool &Mapped);
	void EndFrame();
	void Process(uint32 Time);

private:
	CVRC6_Pulse		*Pulse1, *Pulse2;
	CVRC6_Sawtooth	*Sawtooth;
};

#endif /* _VRC6_H_ */