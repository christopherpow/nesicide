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
// Derived channels, MMC5
//

class CChannelHandlerMMC5 : public CChannelHandler {
public:
	CChannelHandlerMMC5();
	virtual void ProcessChannel();
	virtual void ResetChannel();

protected:
	virtual void HandleNoteData(stChanNote *pNoteData, int EffColumns);
	virtual void HandleCustomEffects(int EffNum, int EffParam);
	virtual bool HandleInstrument(int Instrument, bool Trigger, bool NewInstrument);
	virtual void HandleEmptyNote();
	virtual void HandleHalt();
	virtual void HandleRelease();
	virtual void HandleNote(int Note, int Octave);

protected:
	static const int SEQUENCES = 5;
	static const int SEQ_TYPES[];
protected:
	int m_iPostEffect;
	int m_iPostEffectParam;
	int m_iInitVolume;
	bool m_bManualVolume;
};

// Square 1
class CMMC5Square1Chan : public CChannelHandlerMMC5 {
public:
	CMMC5Square1Chan() : CChannelHandlerMMC5() {};
	void RefreshChannel();
protected:
	void ClearRegisters();
};

// Square 2
class CMMC5Square2Chan : public CChannelHandlerMMC5 {
public:
	CMMC5Square2Chan() : CChannelHandlerMMC5() {};
	void RefreshChannel();
protected:
	void ClearRegisters();
};
