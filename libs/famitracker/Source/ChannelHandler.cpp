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

//
// This is the base class for all classes that takes care of 
// translating notes to channel register writes.
//

#include <algorithm>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "SoundGen.h"
#include "ChannelHandler.h"
#include "APU/APU.h"

/*
 * Class CChannelHandler
 *
 */

CChannelHandler::CChannelHandler(int MaxPeriod, int MaxVolume) : 
	m_iChannelID(0), 
	m_iInstrument(0), 
	m_iLastInstrument(MAX_INSTRUMENTS),
	m_pNoteLookupTable(NULL),
	m_pVibratoTable(NULL),
	m_pAPU(NULL),
	m_iPitch(0),
	m_iNote(0),
	m_iSeqVolume(0),
	m_iDefaultDuty(0),
	m_iDutyPeriod(0),
	m_iMaxPeriod(MaxPeriod),
	m_iMaxVolume(MaxVolume),
	m_bGate(false),
	m_bNewVibratoMode(false),
	m_bLinearPitch(false),
	m_bPeriodUpdated(false),
	m_bVolumeUpdate(false)
{ 
}

CChannelHandler::~CChannelHandler()
{
}

void CChannelHandler::InitChannel(CAPU *pAPU, int *pVibTable, CSoundGen *pSoundGen)
{
	// Called from main thread

	m_pAPU = pAPU;
	m_pVibratoTable = pVibTable;
	m_pSoundGen = pSoundGen;

	m_bDelayEnabled = false;

	m_iEffect = 0;

	DocumentPropertiesChanged(pSoundGen->GetDocument());

	ResetChannel();
}

void CChannelHandler::DocumentPropertiesChanged(CFamiTrackerDoc *pDoc)
{
	m_bNewVibratoMode = (pDoc->GetVibratoStyle() == VIBRATO_NEW);
	m_bLinearPitch = pDoc->GetLinearPitch();
}

int CChannelHandler::LimitPeriod(int Period) const
{
	Period = std::min(Period, m_iMaxPeriod);
	Period = std::max(Period, 0);
	return Period;
}

int CChannelHandler::LimitVolume(int Volume) const
{
	Volume = std::min(Volume, 15);
	Volume = std::max(Volume, 0);
	return Volume;
}

void CChannelHandler::SetPitch(int Pitch)
{
	// Pitch ranges from -511 to +512
	m_iPitch = Pitch;
	if (m_iPitch == 512)
		m_iPitch = 511;
}

int CChannelHandler::GetPitch() const 
{ 
	if (m_iPitch != 0 && m_iNote != 0 && m_pNoteLookupTable != NULL) {
		// Interpolate pitch
		int LowNote  = std::max(m_iNote - PITCH_WHEEL_RANGE, 0);
		int HighNote = std::min(m_iNote + PITCH_WHEEL_RANGE, 95);
		int Freq	 = m_pNoteLookupTable[m_iNote];
		int Lower	 = m_pNoteLookupTable[LowNote];
		int Higher	 = m_pNoteLookupTable[HighNote];
		int Pitch	 = (m_iPitch < 0) ? (Freq - Lower) : (Higher - Freq);
		return (Pitch * m_iPitch) / 511;
	}

	return 0;
}

void CChannelHandler::Arpeggiate(unsigned int Note)
{
	SetPeriod(TriggerNote(Note));
}

void CChannelHandler::ResetChannel()
{
	// Resets the channel states (volume, instrument & duty)
	// Clears channel registers

	// Instrument 
	m_iInstrument		= MAX_INSTRUMENTS;
	m_iLastInstrument	= MAX_INSTRUMENTS;

	// Volume 
	m_iVolume			= VOL_COLUMN_MAX;

	m_iDefaultDuty		= 0;
	m_iSeqVolume		= 0;

	// Period
	m_iPeriod			= 0;
	m_iLastPeriod		= 0xFFFF;
	m_iPeriodPart		= 0;

	// Effect states
	m_iPortaSpeed		= 0;
	m_iPortaTo			= 0;
	m_iArpeggio			= 0;
	m_iArpState			= 0;
	m_iVibratoSpeed		= 0;
	m_iVibratoPhase		= !m_bNewVibratoMode ? 48 : 0;
	m_iTremoloSpeed		= 0;
	m_iTremoloPhase		= 0;
	m_iFinePitch		= 0x80;
	m_iPeriod			= 0;
	m_iVolSlide			= 0;
	m_bDelayEnabled		= false;
	m_iNoteCut			= 0;
	m_iVibratoDepth		= 0;
	m_iTremoloDepth		= 0;

	// States
	m_bRelease			= false;
	m_bGate				= false;

	RegisterKeyState(-1);

	// Clear channel registers
	ClearRegisters();

	ClearSequences();
}

// Handle common things before letting the channels play the notes
void CChannelHandler::PlayNote(stChanNote *pNoteData, int EffColumns)
{
	ASSERT (pNoteData != NULL);

	// Handle delay commands
	if (HandleDelay(pNoteData, EffColumns))
		return;

	// Handle global effects
	m_pSoundGen->EvaluateGlobalEffects(pNoteData, EffColumns);

	// Let the channel play
	HandleNoteData(pNoteData, EffColumns);
}

void CChannelHandler::HandleNoteData(stChanNote *pNoteData, int EffColumns)
{
	int LastInstrument = m_iInstrument;
	int Instrument = pNoteData->Instrument;
	int Note = pNoteData->Note;

	// Clear the note cut effect
	if (pNoteData->Note != NONE) {
		m_iNoteCut = 0;
	}

	// Effects
	for (int n = 0; n < EffColumns; n++) {
		unsigned char EffNum   = pNoteData->EffNumber[n];
		unsigned char EffParam = pNoteData->EffParam[n];
		HandleCustomEffects(EffNum, EffParam);
	}

	// Volume
	if (pNoteData->Vol < 0x10) {
		m_iVolume = pNoteData->Vol << VOL_COLUMN_SHIFT;
	}

	// Instrument
	if (Note == HALT || Note == RELEASE) 
		Instrument = MAX_INSTRUMENTS;	// Ignore instrument for release and halt commands

	if (Instrument != MAX_INSTRUMENTS)
		m_iInstrument = Instrument;

	bool Trigger = (Note != NONE) && (Note != HALT) && (Note != RELEASE);
	bool NewInstrument = (m_iInstrument != LastInstrument) || (m_iInstrument == MAX_INSTRUMENTS);

	if (m_iInstrument == MAX_INSTRUMENTS) {
		// No instrument selected, default to 0
		m_iInstrument = m_pSoundGen->GetDefaultInstrument();
	}

	if (NewInstrument || Trigger) {
		if (!HandleInstrument(m_iInstrument, Trigger, NewInstrument))
			return;
	}

	if (NewInstrument || Trigger) {
		if (!HandleInstrument(m_iInstrument, Trigger, NewInstrument))
			return;
	}

	// Clear release flag
	if (pNoteData->Note != RELEASE && pNoteData->Note != NONE) {
		m_bRelease = false;
	}

	// Note
	switch (pNoteData->Note) {
		case NONE:
			HandleEmptyNote();
			break;
		case HALT:
			HandleCut();
			break;
		case RELEASE:
			HandleRelease();
			break;
		default:
			HandleNote(pNoteData->Note, pNoteData->Octave);
			break;
	}
}

void CChannelHandler::SetNoteTable(unsigned int *pNoteLookupTable)
{
	// Installs the note lookup table
	m_pNoteLookupTable = pNoteLookupTable;
}

int CChannelHandler::TriggerNote(int Note)
{
	Note = std::min(Note, NOTE_COUNT - 1);
	Note = std::max(Note, 0);

	// Trigger a note, return note period
	RegisterKeyState(Note);

	if (!m_pNoteLookupTable)
		return Note;

	return m_pNoteLookupTable[Note];
}

void CChannelHandler::CutNote()
{
	// Cut currently playing note

	RegisterKeyState(-1);

	m_bGate = false;
	m_iPeriod = 0;
	m_iPortaTo = 0;
}

void CChannelHandler::ReleaseNote()
{
	// Release currently playing note

	RegisterKeyState(-1);

	m_bRelease = true;
}

int CChannelHandler::RunNote(int Octave, int Note)
{
	// Run the note and handle portamento
	int NewNote = MIDI_NOTE(Octave, Note);
	int NesFreq = TriggerNote(NewNote);

	if (m_iPortaSpeed > 0 && m_iEffect == EF_PORTAMENTO) {
		if (m_iPeriod == 0)
			m_iPeriod = NesFreq;
		m_iPortaTo = NesFreq;
	}
	else
		m_iPeriod = NesFreq;

	m_bGate = true;

	return NewNote;
}

void CChannelHandler::SetupSlide(int Type, int EffParam)
{
	#define GET_SLIDE_SPEED(x) (((x & 0xF0) >> 3) + 1)

	m_iPortaSpeed = GET_SLIDE_SPEED(EffParam);
	m_iEffect = Type;

	if (Type == EF_SLIDE_UP)
		m_iNote = m_iNote + (EffParam & 0xF);
	else
		m_iNote = m_iNote - (EffParam & 0xF);

	m_iPortaTo = TriggerNote(m_iNote);
}

bool CChannelHandler::CheckCommonEffects(unsigned char EffCmd, unsigned char EffParam)
{
	// Handle common effects for all channels

	switch (EffCmd) {
		case EF_PORTAMENTO:
			m_iPortaSpeed = EffParam;
			m_iEffect = EF_PORTAMENTO;
			if (!EffParam)
				m_iPortaTo = 0;
			break;
		case EF_VIBRATO:
			m_iVibratoDepth = (EffParam & 0x0F) << 4;
			m_iVibratoSpeed = EffParam >> 4;
			if (!EffParam)
				m_iVibratoPhase = !m_bNewVibratoMode ? 48 : 0;
			break;
		case EF_TREMOLO:
			m_iTremoloDepth = (EffParam & 0x0F) << 4;
			m_iTremoloSpeed = EffParam >> 4;
			if (!EffParam)
				m_iTremoloPhase = 0;
			break;
		case EF_ARPEGGIO:
			m_iArpeggio = EffParam;
			m_iEffect = EF_ARPEGGIO;
			break;
		case EF_PITCH:
			m_iFinePitch = EffParam;
			break;
		case EF_PORTA_DOWN:
			m_iPortaSpeed = EffParam;
			m_iEffect = EF_PORTA_DOWN;
			break;
		case EF_PORTA_UP:
			m_iPortaSpeed = EffParam;
			m_iEffect = EF_PORTA_UP;
			break;
		case EF_VOLUME_SLIDE:
			m_iVolSlide = EffParam;
			break;
		case EF_NOTE_CUT:
			m_iNoteCut = EffParam + 1;
			break;
//		case EF_TARGET_VOLUME_SLIDE:
			// TODO implement
//			break;
		default:
			return false;
	}
	
	return true;
}

bool CChannelHandler::HandleDelay(stChanNote *pNoteData, int EffColumns)
{
	// Handle note delay, Gxx

	if (m_bDelayEnabled) {
		m_bDelayEnabled = false;
		HandleNoteData(&m_cnDelayed, m_iDelayEffColumns);
	}
	
	// Check delay
	for (int i = 0; i < EffColumns; ++i) {
		if (pNoteData->EffNumber[i] == EF_DELAY && pNoteData->EffParam[i] > 0) {
			m_bDelayEnabled = true;
			m_cDelayCounter = pNoteData->EffParam[i];
			m_iDelayEffColumns = EffColumns;
			memcpy(&m_cnDelayed, pNoteData, sizeof(stChanNote));

			// Only one delay/row is allowed. Remove global effects
			for (int j = 0; j < EffColumns; ++j) {
				switch (m_cnDelayed.EffNumber[j]) {
					case EF_DELAY:
						m_cnDelayed.EffNumber[j] = EF_NONE;
						m_cnDelayed.EffParam[j] = 0;
						break;
					case EF_JUMP:
						m_pSoundGen->SetJumpPattern(m_cnDelayed.EffParam[j]);
						m_cnDelayed.EffNumber[j] = EF_NONE;
						m_cnDelayed.EffParam[j] = 0;
						break;
					case EF_SKIP:
						m_pSoundGen->SetSkipRow(m_cnDelayed.EffParam[j]);
						m_cnDelayed.EffNumber[j] = EF_NONE;
						m_cnDelayed.EffParam[j] = 0;
						break;
				}
			}
			return true;
		}
	}

	return false;
}

void CChannelHandler::UpdateNoteCut()
{
	// Note cut ()
	if (m_iNoteCut > 0) {
		m_iNoteCut--;
		if (m_iNoteCut == 0) {
			HandleCut();
		}
	}
}

void CChannelHandler::UpdateDelay()
{
	// Delay (Gxx)
	if (m_bDelayEnabled) {
		if (!m_cDelayCounter) {
			m_bDelayEnabled = false;
			PlayNote(&m_cnDelayed, m_iDelayEffColumns);
		}
		else
			m_cDelayCounter--;
	}
}

void CChannelHandler::UpdateVolumeSlide()
{
	// Volume slide (Axx)
	m_iVolume -= (m_iVolSlide & 0x0F);
	if (m_iVolume < 0)
		m_iVolume = 0;

	m_iVolume += (m_iVolSlide & 0xF0) >> 4;
	if (m_iVolume > VOL_COLUMN_MAX)
		m_iVolume = VOL_COLUMN_MAX;
}

void CChannelHandler::UpdateTargetVolumeSlide()
{
	// TODO implement
}

void CChannelHandler::UpdateVibratoTremolo()
{
	// Vibrato and tremolo
	m_iVibratoPhase = (m_iVibratoPhase + m_iVibratoSpeed) & 63;
	m_iTremoloPhase = (m_iTremoloPhase + m_iTremoloSpeed) & 63;
}

void CChannelHandler::LinearAdd(int Step)
{
	m_iPeriod = (m_iPeriod << 5) | m_iPeriodPart;
	int value = (m_iPeriod * Step) / 512;
	if (value == 0)
		value = 1;
	m_iPeriod += value;
	m_iPeriodPart = m_iPeriod & 0x1F;
	m_iPeriod >>= 5;
}

void CChannelHandler::LinearRemove(int Step)
{
	m_iPeriod = (m_iPeriod << 5) | m_iPeriodPart;
	int value = (m_iPeriod * Step) / 512;
	if (value == 0)
		value = 1;
	m_iPeriod -= value;
	m_iPeriodPart = m_iPeriod & 0x1F;
	m_iPeriod >>= 5;
}

void CChannelHandler::PeriodAdd(int Step)
{
	if (m_bLinearPitch)
		LinearAdd(Step);
	else
		SetPeriod(GetPeriod() + Step);
}

void CChannelHandler::PeriodRemove(int Step)
{
	if (m_bLinearPitch)
		LinearRemove(Step);
	else
		SetPeriod(GetPeriod() - Step);
}

void CChannelHandler::UpdateEffects()
{
	// Handle other effects
	switch (m_iEffect) {
		case EF_ARPEGGIO:
			if (m_iArpeggio != 0) {
				switch (m_iArpState) {
					case 0:
						SetPeriod(TriggerNote(m_iNote));
						break;
					case 1:
						SetPeriod(TriggerNote(m_iNote + (m_iArpeggio >> 4)));
						if ((m_iArpeggio & 0x0F) == 0)
							++m_iArpState;
						break;
					case 2:
						SetPeriod(TriggerNote(m_iNote + (m_iArpeggio & 0x0F)));
						break;
				}
				m_iArpState = (m_iArpState + 1) % 3;
			}
			break;
		case EF_PORTAMENTO:
			// Automatic portamento
			if (m_iPortaSpeed > 0 && m_iPortaTo > 0) {
				if (m_iPeriod > m_iPortaTo) {
					PeriodRemove(m_iPortaSpeed);
					if (m_iPeriod < m_iPortaTo)
						SetPeriod(m_iPortaTo);
				}
				else if (m_iPeriod < m_iPortaTo) {
					PeriodAdd(m_iPortaSpeed);
					if (m_iPeriod > m_iPortaTo)
						SetPeriod(m_iPortaTo);
				}
			}
			break;
		case EF_SLIDE_UP:
			if (m_iPortaSpeed > 0) {
				PeriodRemove(m_iPortaSpeed);
				if (m_iPeriod < m_iPortaTo) {
					SetPeriod(m_iPortaTo);
					m_iPortaTo = 0;
					m_iEffect = EF_NONE;
				}
			}
			break;
		case EF_SLIDE_DOWN:
			if (m_iPortaSpeed > 0) {
				PeriodAdd(m_iPortaSpeed);
				if (m_iPeriod > m_iPortaTo) {
					SetPeriod(m_iPortaTo);
					m_iPortaTo = 0;
					m_iEffect = EF_NONE;
				}
			}
			break;
		case EF_PORTA_DOWN:
			if (GetPeriod() > 0)
				PeriodAdd(m_iPortaSpeed);
			break;
		case EF_PORTA_UP:
			if (GetPeriod() > 0)
				PeriodRemove(m_iPortaSpeed);
			break;
	}
}

void CChannelHandler::ProcessChannel()
{
	// Run all default and common channel processing
	// This gets called each frame
	//

	UpdateDelay();
	UpdateNoteCut();
	UpdateVolumeSlide();
	UpdateVibratoTremolo();
	UpdateEffects();
}

bool CChannelHandler::IsActive() const
{
	return m_bGate;
}

bool CChannelHandler::IsReleasing() const
{
	return m_bRelease;
}

int CChannelHandler::GetVibrato() const
{
	// Vibrato offset (4xx)
	int VibFreq;

	if ((m_iVibratoPhase & 0xF0) == 0x00)
		VibFreq = m_pVibratoTable[m_iVibratoDepth + m_iVibratoPhase];
	else if ((m_iVibratoPhase & 0xF0) == 0x10)
		VibFreq = m_pVibratoTable[m_iVibratoDepth + 15 - (m_iVibratoPhase - 16)];
	else if ((m_iVibratoPhase & 0xF0) == 0x20)
		VibFreq = -m_pVibratoTable[m_iVibratoDepth + (m_iVibratoPhase - 32)];
	else if ((m_iVibratoPhase & 0xF0) == 0x30)
		VibFreq = -m_pVibratoTable[m_iVibratoDepth + 15 - (m_iVibratoPhase - 48)];

	if (!m_bNewVibratoMode) {
		VibFreq += m_pVibratoTable[m_iVibratoDepth + 15] + 1;
		VibFreq >>= 1;
	}

	if (m_bLinearPitch)
		VibFreq = (GetPeriod() * VibFreq) / 128;

	return VibFreq;
}

int CChannelHandler::GetTremolo() const
{
	// Tremolo offset (7xx)
	int TremVol = 0;
	int Phase = m_iTremoloPhase >> 1;

	if ((Phase & 0xF0) == 0x00)
		TremVol = m_pVibratoTable[m_iTremoloDepth + Phase];
	else if ((Phase & 0xF0) == 0x10)
		TremVol = m_pVibratoTable[m_iTremoloDepth + 15 - (Phase - 16)];

	return (TremVol >> 1);
}

int CChannelHandler::GetFinePitch() const
{
	// Fine pitch setting (Pxx)
	return (0x80 - m_iFinePitch);
}

int CChannelHandler::CalculatePeriod() const 
{
	return LimitPeriod(GetPeriod() - GetVibrato() + GetFinePitch() + GetPitch());
}

int CChannelHandler::CalculateVolume() const
{
	// Volume calculation
	int Volume = m_iVolume >> VOL_COLUMN_SHIFT;

	Volume = (m_iSeqVolume * Volume) / 15 - GetTremolo();
	Volume = std::max(Volume, 0);
	Volume = std::min(Volume, m_iMaxVolume);

	if (m_iSeqVolume > 0 && m_iVolume > 0 && Volume == 0)
		Volume = 1;

	if (!m_bGate)
		Volume = 0;

	return Volume;
}

void CChannelHandler::AddCycles(int count)
{
	m_pSoundGen->AddCycles(count);
}

void CChannelHandler::WriteRegister(uint16 Reg, uint8 Value)
{
	m_pAPU->Write(Reg, Value);
	m_pSoundGen->WriteRegister(Reg, Value);
}

void CChannelHandler::WriteExternalRegister(uint16 Reg, uint8 Value)
{
	m_pAPU->ExternalWrite(Reg, Value);
	m_pSoundGen->WriteExternalRegister(Reg, Value);
}

void CChannelHandler::RegisterKeyState(int Note)
{
	m_pSoundGen->RegisterKeyState(m_iChannelID, Note);
}

void CChannelHandler::SetVolume(int Volume)
{
	m_iSeqVolume = Volume;
}

void CChannelHandler::SetPeriod(int Period)
{
	m_iPeriod = LimitPeriod(Period);
	m_bPeriodUpdated = true;
}

int CChannelHandler::GetPeriod() const
{
	return m_iPeriod;
}

void CChannelHandler::SetNote(int Note)
{
	m_iNote = Note;
}

int CChannelHandler::GetNote() const
{
	return m_iNote;
}

void CChannelHandler::SetDutyPeriod(int Period)
{
	m_iDutyPeriod = Period;
}

/*
 * Class CChannelHandlerInverted
 *
 */

void CChannelHandlerInverted::SetupSlide(int Type, int EffParam)
{
	CChannelHandler::SetupSlide(Type, EffParam);

	// Invert slide effects
	if (m_iEffect == EF_SLIDE_DOWN)
		m_iEffect = EF_SLIDE_UP;
	else
		m_iEffect = EF_SLIDE_DOWN;
}

int CChannelHandlerInverted::CalculatePeriod() const 
{
	return LimitPeriod(GetPeriod() + GetVibrato() - GetFinePitch() - GetPitch());
}

/*
 * Class CSequenceHandler
 *
 */

CSequenceHandler::CSequenceHandler()
{
	ClearSequences();
}

// Sequence routines

void CSequenceHandler::SetupSequence(int Index, const CSequence *pSequence)
{
	m_iSeqState[Index]	 = SEQ_STATE_RUNNING;
	m_iSeqPointer[Index] = 0;
	m_pSequence[Index]	 = pSequence;
}

void CSequenceHandler::ClearSequence(int Index)
{
	m_iSeqState[Index]	 = SEQ_STATE_DISABLED;
	m_iSeqPointer[Index] = 0;
	m_pSequence[Index]	 = NULL;
}

void CSequenceHandler::UpdateSequenceRunning(int Index, const CSequence *pSequence)
{
	int Value = pSequence->GetItem(m_iSeqPointer[Index]);

	switch (Index) {
		// Volume modifier
		case SEQ_VOLUME:
			SetVolume(Value);
			break;
		// Arpeggiator
		case SEQ_ARPEGGIO:
			switch (pSequence->GetSetting()) {
				case ARP_SETTING_ABSOLUTE:
					SetPeriod(TriggerNote(GetNote() + Value));
					break;
				case ARP_SETTING_FIXED:
					SetPeriod(TriggerNote(Value));
					break;
				case ARP_SETTING_RELATIVE:
					SetNote(GetNote() + Value);
					SetPeriod(TriggerNote(GetNote()));
					break;
			}
			break;
		// Pitch
		case SEQ_PITCH:
			SetPeriod(GetPeriod() + Value);
			break;
		// Hi-pitch
		case SEQ_HIPITCH:
			SetPeriod(GetPeriod() + (Value << 4));
			break;
		// Duty cycling
		case SEQ_DUTYCYCLE:
			SetDutyPeriod(Value);
			break;
	}

	++m_iSeqPointer[Index];

	int Release = pSequence->GetReleasePoint();
	int Items = pSequence->GetItemCount();
	int Loop = pSequence->GetLoopPoint();

	if (m_iSeqPointer[Index] == (Release + 1) || m_iSeqPointer[Index] >= Items) {
		// End point reached
		if (Loop != -1 && !(IsReleasing() && Release != -1)) {
			m_iSeqPointer[Index] = Loop;
		}
		else {
			if (m_iSeqPointer[Index] >= Items) {
				// End of sequence 
				m_iSeqState[Index] = SEQ_STATE_END;
			}
			else if (!IsReleasing()) {
				// Waiting for release
				--m_iSeqPointer[Index];
			}
		}
	}

	theApp.GetSoundGenerator()->SetSequencePlayPos(pSequence, m_iSeqPointer[Index]);
}

void CSequenceHandler::UpdateSequenceEnd(int Index, const CSequence *pSequence)
{
	switch (Index) {
		case SEQ_ARPEGGIO:
			if (pSequence->GetSetting() == ARP_SETTING_FIXED) {
				SetPeriod(TriggerNote(GetNote()));
			}
			break;
	}

	m_iSeqState[Index] = SEQ_STATE_HALT;

	theApp.GetSoundGenerator()->SetSequencePlayPos(pSequence, -1);
}

void CSequenceHandler::RunSequence(int Index)
{
	const CSequence *pSequence = m_pSequence[Index];

	if (!pSequence || pSequence->GetItemCount() == 0 || !IsActive())
		return;

	switch (m_iSeqState[Index]) {
		case SEQ_STATE_RUNNING:
			UpdateSequenceRunning(Index, pSequence);
			break;
		case SEQ_STATE_END:
			UpdateSequenceEnd(Index, pSequence);
			break;
		case SEQ_STATE_DISABLED:
		case SEQ_STATE_HALT:
			// Do nothing
			break;
	}
}

void CSequenceHandler::ClearSequences()
{
	for (int i = 0; i < SEQ_COUNT; ++i) {
		m_iSeqState[i]	 = SEQ_STATE_DISABLED;
		m_iSeqPointer[i] = 0;
		m_pSequence[i]	 = NULL;
	}
}

void CSequenceHandler::ReleaseSequences()
{
	for (int i = 0; i < SEQ_COUNT; ++i) {
		if (m_iSeqState[i] == SEQ_STATE_RUNNING || m_iSeqState[i] == SEQ_STATE_END) {
			ReleaseSequence(i, m_pSequence[i]);
		}
	}
}

void CSequenceHandler::ReleaseSequence(int Index, const CSequence *pSeq)
{
	int ReleasePoint = pSeq->GetReleasePoint();

	if (ReleasePoint != -1) {
		m_iSeqPointer[Index] = ReleasePoint;
		m_iSeqState[Index] = SEQ_STATE_RUNNING;
	}
}

bool CSequenceHandler::IsSequenceEqual(int Index, const CSequence *pSequence) const
{
	return pSequence == m_pSequence[Index];
}

seq_state_t CSequenceHandler::GetSequenceState(int Index) const
{
	return m_iSeqState[Index];
}
