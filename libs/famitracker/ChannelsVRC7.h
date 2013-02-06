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
// Derived channels, VRC7
//

class CChannelHandlerVRC7 : public CChannelHandler {
public:
	CChannelHandlerVRC7();
	virtual void ProcessChannel();
	virtual void ResetChannel();
	virtual void SetChannelID(int ID);
protected:
	virtual void PlayChannelNote(stChanNote *NoteData, int EffColumns);
	unsigned int TriggerNote(int Note);
protected:
	unsigned int GetFnum(int Note);

protected:
	unsigned char m_iChannel;
	unsigned char m_iPatch;

	char	m_iRegs[8];

	bool	m_bHold;

	int		m_iCommand;
	int		m_iTriggeredNote;
	int		m_iOctave;
};

class CVRC7Channel : public CChannelHandlerVRC7 {
public:
	CVRC7Channel() : CChannelHandlerVRC7() {};
	void RefreshChannel();
protected:
	void ClearRegisters();
private:
	void RegWrite(unsigned char Reg, unsigned char Value);
};
