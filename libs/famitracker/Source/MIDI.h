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

#include <mmsystem.h>

const int MIDI_MSG_NOTE_OFF			= 8;
const int MIDI_MSG_NOTE_ON			= 9;
const int MIDI_MSG_AFTER_TOUCH		= 0xA;
const int MIDI_MSG_CONTROL_CHANGE	= 0xB;
const int MIDI_MSG_PROGRAM_CHANGE	= 0xC;
const int MIDI_MSG_CHANNEL_PRESSURE = 0xD;
const int MIDI_MSG_PITCH_WHEEL		= 0xE;

// CMIDI command target

class CMIDI : public CObject
{
public:
	CMIDI();
	virtual ~CMIDI();

	bool	Init(void);
	void	Shutdown(void);

	bool	OpenDevices(void);
	bool	CloseDevices(void);

	bool	ReadMessage(unsigned char & Message, unsigned char & Channel, unsigned char & Data1, unsigned char & Data2);
	void	WriteNote(unsigned char Channel, unsigned char Note, unsigned char Octave, unsigned char Velocity);
	void	ResetOutput();
	void	ToggleInput();

	int		GetQuantization();

	bool	IsOpened() const;
	bool	IsAvailable() const;

	void	SetInputDevice(int Device, bool MasterSync);
	void	SetOutputDevice(int Device);

	int		GetInputDevice() const;
	int		GetOutputDevice() const;

	// Device enumeration
	int		GetNumInputDevices() const;
	int		GetNumOutputDevices() const;

	void	GetInputDeviceString(int Num, CString &Text) const;
	void	GetOutputDeviceString(int Num, CString &Text) const;

	// Protected functions
protected:
	void	Event(unsigned char Status, unsigned char Data1, unsigned char Data2);
	void	Enqueue(unsigned char MsgType, unsigned char MsgChannel, unsigned char Data1, unsigned char Data2);

	// Constants
protected:
	static const int MAX_QUEUE = 100;

	// Private variables
private:
	int		m_iInDevice;				// MIDI input device
	int		m_iOutDevice;				// MIDI output device

	bool	m_bMasterSync;
	bool	m_bInStarted;

	char	MsgTypeQueue[MAX_QUEUE];
	char	MsgChanQueue[MAX_QUEUE];
	char	Data1Queue[MAX_QUEUE];
	char	Data2Queue[MAX_QUEUE];

	char	Quantization[MAX_QUEUE];

	int		m_iTimingCounter;

	int		m_iQueuePtr;
	int		m_iQueueHead;
	int		m_iQueueTail;

	unsigned char	m_iLastMsgType;
	unsigned char	m_iLastMsgChan;

	HMIDIIN		m_hMIDIIn;
	HMIDIOUT	m_hMIDIOut;

	// Static functions & variables
protected:
	static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	static CMIDI *m_pInstance;
};
