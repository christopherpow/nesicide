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

#pragma once

//
// Derived channels, 5B
//

class CChannelHandlerS5B : public CChannelHandler {
public:
	CChannelHandlerS5B();
	virtual void ProcessChannel();

protected:
	virtual void HandleNoteData(stChanNote *pNoteData, int EffColumns);
	virtual void HandleCustomEffects(int EffNum, int EffParam);
	virtual bool HandleInstrument(int Instrument, bool Trigger, bool NewInstrument);
	virtual void HandleEmptyNote();
	virtual void HandleHalt();
	virtual void HandleRelease();
	virtual void HandleNote(int Note, int Octave);

protected:
	void WriteReg(int Reg, int Value);

protected:
	int m_iNoiseOffset;
	bool m_bEnvEnable;

	bool m_bUpdate;

//	void RunSequence(int Index, CSequence *pSequence);
/*
	unsigned char m_cSweep;
	unsigned char m_cDutyCycle, m_iDefaultDuty;

	int ModEnable[SEQ_COUNT];
	int	ModIndex[SEQ_COUNT];
	int	ModDelay[SEQ_COUNT];
	int	ModPointer[SEQ_COUNT];
	*/

};

// Channel 1
class CS5BChannel1 : public CChannelHandlerS5B {
public:
	CS5BChannel1() : CChannelHandlerS5B() { m_iDefaultDuty = 0; m_bEnabled = false; };
	void RefreshChannel();
protected:
	void ClearRegisters();
};

// Channel 2
class CS5BChannel2 : public CChannelHandlerS5B {
public:
	CS5BChannel2() : CChannelHandlerS5B() { m_iDefaultDuty = 0; m_bEnabled = false; };
	void RefreshChannel();
protected:
	void ClearRegisters();
};

// Channel 3
class CS5BChannel3 : public CChannelHandlerS5B {
public:
	CS5BChannel3() : CChannelHandlerS5B() { m_iDefaultDuty = 0; m_bEnabled = false; };
	void RefreshChannel();
protected:
	void ClearRegisters();
};
