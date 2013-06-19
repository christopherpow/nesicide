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

const int MAX_VOL = 0x7F;
const int VOL_SHIFT = 3;

//enum {SEQ_RUN, SEQ_DISABLED, SEQ_RELEASE, SEQ_WAIT, SEQ_HALT};

class CAPU;

// TODO: A lot of cleanup is needed in these files!

//
// Base class for channel renderers
//
class CChannelHandler {
public:
	CChannelHandler();

	void PlayNote(stChanNote *NoteData, int EffColumns);		// Plays a note, calls the derived classes

	// TODO: use these eventually
	void CutNote();													// Called on note cut commands
	void ReleaseNote();												// Called on note release commands

	// Public functions
	void InitChannel(CAPU *pAPU, int *pVibTable, CFamiTrackerDoc *pDoc);
	void KillChannel();
	void MakeSilent();
	void Arpeggiate(unsigned int Note);

	void SetVibratoStyle(int Style);

	//
	// Public virtual functions
	//
public:
	virtual void ProcessChannel() = 0;							// Run the instrument and effects
	virtual void RefreshChannel() = 0;							// Update channel registers
	virtual void ResetChannel();								// Resets all default state variables

	virtual void SetNoteTable(unsigned int *NoteLookupTable);
	virtual void UpdateSequencePlayPos() {};
	virtual void SetPitch(int Pitch);

	virtual void SetChannelID(int ID) { m_iChannelID = ID; }

	// 
	// Internal virtual functions
	//
protected:
	virtual void PlayChannelNote(stChanNote *NoteData, int EffColumns) = 0; // Plays a note
	virtual void ClearRegisters() = 0;										// Clear channel registers
	virtual	unsigned int TriggerNote(int Note);

	// For sequence
	virtual void RunSequence(int Index, CSequence *pSequence);		// Default sequence handler
	virtual CSequence *GetSequence(int Index, int Type);

	virtual int GetPitch() const;

	int LimitPeriod(int Period) const;
	int LimitVolume(int Volume) const;
	void SetMaxPeriod(int Period);

	void ReleaseSequences(int Chip);
	void ReleaseSequence(int Index, CSequence *pSeq);

	int CalculatePeriod(bool InvertPitch) const;
	int CalculateVolume(int Limit) const;

	//
	// Internal functions
	//
protected:
	int RunNote(int Octave, int Note);

	void SetupSlide(int Type, int EffParam);

	bool CheckNote(stChanNote *pNoteData, int InstrumentType);

	bool CheckCommonEffects(unsigned char EffCmd, unsigned char EffParam);
	bool HandleDelay(stChanNote *NoteData, int EffColumns);

	int GetVibrato() const;
	int GetTremolo() const;
	int GetFinePitch() const;

	void AddCycles(int count);

	void PeriodAdd(int Step);
	void PeriodRemove(int Step);

	void LinearAdd(int Step);
	void LinearRemove(int Step);

private:
	void UpdateNoteCut();
	void UpdateDelay();
	void UpdateVolumeSlide();
	void UpdateVibratoTremolo();
	void UpdateEffects();


	// Shared variables
protected:
	// Channel variables
	int					m_iChannelID;				// Channel ID
	int					m_iVibratoStyle;

	// General
	bool				m_bEnabled;
	bool				m_bRelease;							// Note released
	unsigned int		m_iInstrument, m_iLastInstrument;	// Instrument
	int					m_iNote;							// Active note
	int					m_iPeriod, m_iLastPeriod;			// Channel period
	char				m_iVolume;							// Volume
	char				m_iDutyPeriod;

	int					m_iPeriodPart;

	// Delay effect variables
	bool				m_bDelayEnabled;
	unsigned char		m_cDelayCounter;
	unsigned int		m_iDelayEffColumns;		
	stChanNote			m_cnDelayed;

	// Vibrato & tremolo
	unsigned int		m_iVibratoDepth, m_iVibratoSpeed, m_iVibratoPhase;
	unsigned int		m_iTremoloDepth, m_iTremoloSpeed, m_iTremoloPhase;

	unsigned char		m_iEffect;		// arpeggio & portamento
	unsigned char		m_cArpeggio, m_cArpVar;
	int					m_iPortaTo, m_iPortaSpeed;

	unsigned char		m_iNoteCut;					// Note cut effect
	unsigned int		m_iFinePitch;				// Fine pitch effect
	unsigned char		m_iDefaultDuty;				// Duty effect
	unsigned char		m_iVolSlide;				// Volume slide effect

	// Sequences
	int					m_iSeqEnabled[SEQ_COUNT];
	int					m_iSeqPointer[SEQ_COUNT];
	int					m_iSeqIndex[SEQ_COUNT];

	unsigned int		m_iSeqVolume;				// Current sequence volume

	// Misc 
	CAPU				*m_pAPU;
	CFamiTrackerDoc		*m_pDocument;

	unsigned int		*m_pNoteLookupTable;		// Note->period table
	int					*m_pVibratoTable;			// Vibrato table

	int					m_iPitch;					// Used by the pitch wheel

	bool				m_bGate;

	// TODO: sort and rename
	unsigned int		InitVol;
	unsigned int		Length;

	// Private variables
private:
	int m_iMaxPeriod;				// Used to limit period register

};
