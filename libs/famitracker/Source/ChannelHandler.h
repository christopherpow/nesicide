/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
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

class CAPU;

// TODO: Break out sequence functions to a new class

enum seq_state_t {
	SEQ_STATE_DISABLED,
	SEQ_STATE_RUNNING,
	SEQ_STATE_END,
	SEQ_STATE_HALT
};

//
// Base class for channel renderers
//
class CChannelHandler {
public:
	CChannelHandler();
	virtual ~CChannelHandler();

	void PlayNote(stChanNote *pNoteData, int EffColumns);		// Plays a note, calls the derived classes

	// Public functions
	void InitChannel(CAPU *pAPU, int *pVibTable, CSoundGen *pSoundGen);
	void Arpeggiate(unsigned int Note);

	// Channel state
	int GetStatePeriod() const { return m_iStatePeriod; };
	int GetStateVolume() const { return m_iStateVolume; };

	void DocumentPropertiesChange(CFamiTrackerDoc *pDoc);

	//
	// Public virtual functions
	//
public:
	virtual void ProcessChannel() = 0;							// Run the instrument and effects
	virtual void RefreshChannel() = 0;							// Update channel registers
	virtual void ResetChannel();								// Resets all state variables to default

	virtual void SetNoteTable(unsigned int *NoteLookupTable);
	virtual void UpdateSequencePlayPos() {};
	virtual void SetPitch(int Pitch);

	virtual void SetChannelID(int ID) { m_iChannelID = ID; }

	// 
	// Internal virtual functions
	//
protected:
	virtual void ClearRegisters() = 0;							// Clear channel registers
	virtual	unsigned int TriggerNote(int Note);

	virtual void HandleNoteData(stChanNote *pNoteData, int EffColumns);

	// Pure virtual functions for handling notes
	virtual void HandleCustomEffects(int EffNum, int EffParam) = 0;
	virtual bool HandleInstrument(int Instrument, bool Trigger, bool NewInstrument) = 0;
	virtual void HandleEmptyNote() = 0;
	virtual void HandleCut() = 0;
	virtual void HandleRelease() = 0;
	virtual void HandleNote(int Note, int Octave) = 0;

	virtual void SetupSlide(int Type, int EffParam);

	virtual int CalculatePeriod() const;
	virtual int CalculateVolume() const;

	// 
	// Internal non-virtual functions
	//
protected:
	void CutNote();												// Called on note cut commands
	void ReleaseNote();											// Called on note release commands

	int LimitPeriod(int Period) const;
	int LimitVolume(int Volume) const;

	void SetMaxPeriod(int Period);
	void SetMaxVolume(int Volume);

	void RegisterKeyState(int Note);

	int RunNote(int Octave, int Note);

	int GetPitch() const;

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

	bool IsActive() const;

	void WriteRegister(uint16 Reg, uint8 Value);
	void WriteExternalRegister(uint16 Reg, uint8 Value);

	// Sequence functions
protected:
	void RunSequence(int Index, const CSequence *pSequence);
	void ClearSequences();
	void ReleaseSequences(int Chip);
	void ReleaseSequence(int Index, CSequence *pSeq);

private:
	void UpdateNoteCut();
	void UpdateDelay();
	void UpdateVolumeSlide();
	void UpdateTargetVolumeSlide();
	void UpdateVibratoTremolo();
	void UpdateEffects();

public:
	// Range for the pitch wheel command (in semitones)
	static const int PITCH_WHEEL_RANGE = 6;

	static const int VOL_COLUMN_SHIFT = 3;
	static const int VOL_COLUMN_MAX = 0x7F;

	// Shared variables
protected:
	// Channel variables
	int				m_iChannelID;					// Channel ID

	// General
	bool			m_bRelease;						// Note released flag
	bool			m_bGate;						// Note gate flag
	unsigned int	m_iInstrument,					// Instrument
					m_iLastInstrument;
	int				m_iNote;						// Active note
	int				m_iPeriod,
					m_iLastPeriod;					// Channel period
	int				m_iVolume;						// Volume
	char			m_iDutyPeriod;

	int				m_iPeriodPart;					// Used by linear slides

	bool			m_bNewVibratoMode;
	bool			m_bLinearPitch;

	// Delay effect variables
	bool			m_bDelayEnabled;
	unsigned char	m_cDelayCounter;
	unsigned int	m_iDelayEffColumns;		
	stChanNote		m_cnDelayed;

	// Vibrato & tremolo
	unsigned int	m_iVibratoDepth,
					m_iVibratoSpeed,
					m_iVibratoPhase;

	unsigned int	m_iTremoloDepth, 
					m_iTremoloSpeed, 
					m_iTremoloPhase;

	unsigned char	m_iEffect;						// arpeggio & portamento
	unsigned char	m_iArpeggio;
	unsigned char	m_iArpState;
	int				m_iPortaTo;
	int				m_iPortaSpeed;

	unsigned char	m_iNoteCut;						// Note cut effect
	unsigned int	m_iFinePitch;					// Fine pitch effect
	unsigned char	m_iDefaultDuty;					// Duty effect
	unsigned char	m_iVolSlide;					// Volume slide effect

	// Misc 
	CAPU			*m_pAPU;
	//CFamiTrackerDoc	*m_pDocument;
	CSoundGen		*m_pSoundGen;

	unsigned int	*m_pNoteLookupTable;			// Note->period table
	int				*m_pVibratoTable;				// Vibrato table

	int				m_iPitch;						// Used by the pitch wheel

	int				m_iStatePeriod;
	int				m_iStateVolume;

	// Sequence variables
protected:
	seq_state_t		m_iSeqState[SEQ_COUNT];
	int				m_iSeqPointer[SEQ_COUNT];
	int				m_iSeqIndex[SEQ_COUNT];

	unsigned int	m_iSeqVolume;					// Current sequence volume

	// Private variables
private:
	int				m_iMaxPeriod;					// Used to limit period register
	int				m_iMaxVolume;					// Max channel volume

};

// Channel handler for channels with frequency registers
class CChannelHandlerInverted : public CChannelHandler {
protected:
	virtual void SetupSlide(int Type, int EffParam);
	virtual int CalculatePeriod() const;
};
