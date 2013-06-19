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

// This file handles playing of VRC7 channels

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ChannelHandler.h"
#include "ChannelsVRC7.h"

#define OPL_NOTE_ON 0x10
#define OPL_SUSTAIN_ON 0x20

enum {
	CMD_NONE, 
	CMD_NOTE_ON,
	CMD_NOTE_TRIGGER,
	CMD_NOTE_OFF, 
	CMD_NOTE_HALT,
	CMD_NOTE_RELEASE
};

CChannelHandlerVRC7::CChannelHandlerVRC7() : 
	CChannelHandler(), 
	m_iCommand(0),
	m_iTriggeredNote(0)
{
	m_iVolume = MAX_VOL;
	SetMaxPeriod(2047);
}

void CChannelHandlerVRC7::SetChannelID(int ID)
{
	CChannelHandler::SetChannelID(ID);
	m_iChannel = ID - CHANID_VRC7_CH1;
}

bool bRegsDirty = false;

void CChannelHandlerVRC7::PlayChannelNote(stChanNote *pNoteData, int EffColumns)
{
	int PostEffect = 0, PostEffectParam;
	int Patch = -1;
	CInstrumentVRC7 *pInstrument = NULL;
#if 0
	int Instrument = pNoteData->Instrument;

	if (Instrument != MAX_INSTRUMENTS/* && Instrument != m_iInstrument*/) {
//		m_iLastInstrument = m_iInstrument;
		m_iInstrument = Instrument;
	}
	else
		Instrument = m_iInstrument;

	if (Instrument != MAX_INSTRUMENTS)
		pInstrument = (CInstrumentVRC7*)m_pDocument->GetInstrument(Instrument);
#endif

	if (pNoteData->Instrument != MAX_INSTRUMENTS)
		m_iInstrument = pNoteData->Instrument;

	if (m_iInstrument != MAX_INSTRUMENTS)
		pInstrument = (CInstrumentVRC7*)m_pDocument->GetInstrument(m_iInstrument);

/*
	if (Instrument != MAX_INSTRUMENTS) {
		// Get the instrument
		if ((pInstrument = (CInstrumentVRC7*)m_pDocument->GetInstrument(Instrument)) == NULL)
			return;

		if (pInstrument->GetType() != INST_VRC7)
			return;
	}
*/

	if (pNoteData->Note != NONE) {
		if (pInstrument) {
			// Patch number
			m_iPatch = pInstrument->GetPatch();

			// Load custom parameters
			if (m_iPatch == 0) {
				for (int i = 0; i < 8; ++i)
					m_iRegs[i] = pInstrument->GetCustomReg(i);
			}
		}
	}

	// Evaluate effects
	for (int i = 0; i < EffColumns; ++i) {
		int EffCmd	 = pNoteData->EffNumber[i];
		int EffParam = pNoteData->EffParam[i];

		if (EffCmd == EF_PORTA_DOWN) {
			m_iEffect = EF_PORTA_UP;
			m_iPortaSpeed = EffParam;
		}
		else if (EffCmd == EF_PORTA_UP) {
			m_iEffect = EF_PORTA_DOWN;
			m_iPortaSpeed = EffParam;
		}
		else {
			if (!CheckCommonEffects(EffCmd, EffParam)) {
				switch (EffCmd) {
					case EF_SLIDE_UP:
					case EF_SLIDE_DOWN:
						PostEffect = EffCmd;
						PostEffectParam = EffParam;
						//SetupSlide(EffCmd, EffParam);
						break;
					case EF_DUTY_CYCLE:
						Patch = EffParam;
						break;
/*
					case EF_VRC7_MODULATOR:
						switch (EffParam & 0xF0) {
							case 0x00:	// Amplitude modulation on/off
								break;
							case 0x10:	// Vibrato on/off
								break;
							case 0x20:	// Sustain on/off
								break;
							case 0x30:	// Wave rectification on/off
								break;
							case 0x40:	// Key rate scaling on/off
								break;
							case 0x50:	// Key rate level
								break;
							case 0x60:	// Mult factor
								break;
							case 0x70:	// Attack
								break;
							case 0x80:	// Decay
								break;
							case 0x90:	// Sustain
								break;
							case 0xA0:	// Release
								break;
						}
						break;
					case EF_VRC7_CARRIER:
						break;
					case EF_VRC7_LEVELS:
						if (EffParam & 0x80)	// Feedback
							m_iRegs[0x03] = (m_iRegs[0x03] & 0xF8) | (EffParam & 0x07);
						else
							m_iRegs[0x02] = (m_iRegs[0x02] & 0xC0) | (EffParam & 0x3F);
						bRegsDirty = true;
						break;
						*/
				}
			}
		}
	}

	// Volume
	if (pNoteData->Vol < 0x10) {
		m_iVolume = pNoteData->Vol << VOL_SHIFT;
	}

	if (pNoteData->Note == HALT) {
		// Halt
		m_iCommand = CMD_NOTE_HALT;
		theApp.RegisterKeyState(m_iChannelID, -1);
	}
	else if (pNoteData->Note == RELEASE) {
		// Release
		m_iCommand = CMD_NOTE_RELEASE;
		theApp.RegisterKeyState(m_iChannelID, -1);
	}
	else if (pNoteData->Note != NONE) {

		// Check instrument
//		if (!pInstrument)
//			return;

		if (pInstrument) {
			// Only allow VRC7 instruments
			if (pInstrument->GetType() != INST_VRC7)
				return;
		}

		int OldNote = m_iNote;
		int OldOctave = m_iOctave;

		// Portamento fix
		if (m_iCommand == CMD_NOTE_HALT)
			m_iPeriod = 0;

		// Trigger note
		m_iNote		= CChannelHandler::RunNote(pNoteData->Octave, pNoteData->Note);
		m_bEnabled	= true;
		m_bHold		= true;

//		if (m_iInstrument != m_iLastInstrument /*|| m_iCommand != CMD_NOTE_ON*/)

		if ((m_iEffect != EF_PORTAMENTO || m_iPortaSpeed == 0) || m_iCommand == CMD_NOTE_HALT)
			m_iCommand = CMD_NOTE_TRIGGER;

		if (m_iPortaSpeed > 0 && m_iEffect == EF_PORTAMENTO && m_iCommand != CMD_NOTE_HALT) {

			// Set current frequency to the one with highest octave
			if (m_iOctave > OldOctave) {
				m_iPeriod >>= (m_iOctave - OldOctave);
			}
			else if (OldOctave > m_iOctave) {
				// Do nothing
				m_iPortaTo >>= (OldOctave - m_iOctave);
				m_iOctave = OldOctave;
			}
		}

		/*
		if (pInstrument) {
			// Patch number
			m_iPatch = pInstrument->GetPatch();

			// Load custom parameters
			if (m_iPatch == 0) {
				for (int i = 0; i < 8; ++i)
					m_iRegs[i] = pInstrument->GetCustomReg(i);
			}
		}
		*/
	}
/*
	if (Patch != -1)
		m_iPatch = Patch;
*/

	if (PostEffect && (PostEffect == EF_SLIDE_UP || PostEffect == EF_SLIDE_DOWN)) {

		#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

		m_iPortaSpeed = GET_SLIDE_SPEED(PostEffectParam);
		m_iEffect = PostEffect;

		if (PostEffect == EF_SLIDE_UP)
			m_iNote = m_iNote + (PostEffectParam & 0xF);
		else
			m_iNote = m_iNote - (PostEffectParam & 0xF);

		int OldOctave = m_iOctave;
		m_iPortaTo = TriggerNote(m_iNote);

		if (m_iOctave > OldOctave) {
			m_iPeriod >>= (m_iOctave - OldOctave);
		}
		else if (m_iOctave < OldOctave) {
			m_iPortaTo >>= (OldOctave - m_iOctave);
			m_iOctave = OldOctave;
		}
	}
	else if (pNoteData->Note != NONE && (m_iEffect == EF_SLIDE_DOWN || m_iEffect == EF_SLIDE_UP))
		m_iEffect = EF_NONE;
}

void CChannelHandlerVRC7::ProcessChannel()
{
	// Default effects
	CChannelHandler::ProcessChannel();
}

void CChannelHandlerVRC7::ResetChannel()
{
	CChannelHandler::ResetChannel();
}

unsigned int CChannelHandlerVRC7::TriggerNote(int Note)
{
	m_iTriggeredNote = Note;
	theApp.RegisterKeyState(m_iChannelID, Note);
	if (m_iCommand != CMD_NOTE_TRIGGER && m_iCommand != CMD_NOTE_HALT)
		m_iCommand = CMD_NOTE_ON;
	m_bEnabled = true;
	m_iOctave = Note / 12;
	return GetFnum(Note);
}

unsigned int CChannelHandlerVRC7::GetFnum(int Note)
{
	const int FREQ_TABLE[] = {172, 181, 192, 204, 216, 229, 242, 257, 272, 288, 305, 323};
	return FREQ_TABLE[Note % 12] << 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// VRC7 Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVRC7Channel::RefreshChannel()
{
	int Note;
	int Fnum;		// F-number
	int Bnum;		// Block
	int Patch;
	int Volume;
	
	Note = m_iTriggeredNote;
	Patch = m_iPatch;
	Volume = 15 - ((m_iVolume >> VOL_SHIFT) - GetTremolo());
	Bnum = m_iOctave;
	Fnum = (m_iPeriod >> 2) - GetVibrato() - GetFinePitch();// (m_iFinePitch - 0x80);

	// Write custom instrument
	if (Patch == 0 && (m_iCommand == CMD_NOTE_TRIGGER || bRegsDirty)) {
		for (int i = 0; i < 8; ++i)
			RegWrite(i, m_iRegs[i]);
	}

	bRegsDirty = false;

	int Cmd = 0;

	switch (m_iCommand) {
		case CMD_NOTE_TRIGGER:
			RegWrite(0x20 + m_iChannel, 0);
			m_iCommand = CMD_NOTE_ON;
			Cmd = OPL_NOTE_ON | OPL_SUSTAIN_ON;
			break;
		case CMD_NOTE_ON:
			Cmd = m_bHold ? OPL_NOTE_ON : OPL_SUSTAIN_ON;
			break;
		case CMD_NOTE_HALT:
			Cmd = 0;
			break;
		case CMD_NOTE_RELEASE:
			Cmd = OPL_SUSTAIN_ON;
			break;
	}
	
	// Write frequency
	RegWrite(0x10 + m_iChannel, Fnum & 0xFF);
	
	if (m_iCommand != CMD_NOTE_HALT) {
		// Select volume & patch
		RegWrite(0x30 + m_iChannel, (Patch << 4) | Volume);
	}

	RegWrite(0x20 + m_iChannel, ((Fnum >> 8) & 1) | (Bnum << 1) | Cmd);
}

void CVRC7Channel::ClearRegisters()
{
	for (int i = 0x10; i < 0x30; i += 0x10)
		RegWrite(i + m_iChannel, 0);

	m_iNote = 0;
	m_iEffect = EF_NONE;

	m_iCommand = CMD_NOTE_HALT;

}

void CVRC7Channel::RegWrite(unsigned char Reg, unsigned char Value)
{
	m_pAPU->ExternalWrite(0x9010, Reg);
	m_pAPU->ExternalWrite(0x9030, Value);
}
