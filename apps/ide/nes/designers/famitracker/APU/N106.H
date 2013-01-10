/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2010  Jonathan Liss
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

#ifndef _N106_H_
#define _N106_H_

#include "external.h"
#include "Channel.h"

class CMixer;

class CN106Chan : public CExChannel {
public:
	CN106Chan(CMixer *pMixer, int ID, uint8 *pWaveData);
	virtual ~CN106Chan();
	void Reset();
	void Process(uint32 Time, uint8 ChannelsActive);
	void Write(uint16 Address, uint8 Value);
private:
	uint8	m_iFreqs[3];
	uint32	m_iCounter, m_iFrequency;
	uint8	m_iVolume;
	uint8	m_iWavePtr, m_iWaveLength, m_iWaveOffset;
	uint8	*m_pWaveData;
};

class CN106 : public CExternal {
public:
	CN106(CMixer *pMixer);
	virtual ~CN106();
	void Reset();
	void Process(uint32 Time);
	void EndFrame();
	void Write(uint16 Address, uint8 Value);
	uint8 Read(uint16 Address, bool &Mapped);
private:
	CN106Chan	*m_pChannels[8];
	uint8		*m_pWaveData;
	uint8		ExpandAddr;
	uint8		m_iChansInUse;
};

#endif /* _N106_H_ */