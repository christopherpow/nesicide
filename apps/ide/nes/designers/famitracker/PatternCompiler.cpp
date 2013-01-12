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

#include "FamiTrackerDoc.h"
#include "PatternCompiler.h"
#include "TrackerChannel.h"

//
// CPatternCompiler - Compress patterns to strings for the NSF code
//

/*

 Pattern byte layout: 

 00h - 7Fh : Notes, where 00h = rest, 7Fh = Note cut
 80h - DFh : Commands, defined in the command table
 E0h - EFh : Quick instrument switches, E0h = instrument 0, EFh = instrument 15
 F0h - FFh : Volume changes, F0h = volume 0, FFh = volume 15

 Each row entry is ended by a note and the duration of the note,
 if fixed duration is enabled then duration is omitted.

*/

// Define commands, the available range is 0-48
#define DEF_CMD(x) ((x << 1) | 0x80)

// Command table
const unsigned char CMD_INSTRUMENT			= DEF_CMD(0);	// 80h
const unsigned char CMD_EFF_SPEED			= DEF_CMD(1);	// 82h
const unsigned char CMD_EFF_JUMP			= DEF_CMD(2);	// ...
const unsigned char CMD_EFF_SKIP			= DEF_CMD(3);
const unsigned char CMD_EFF_HALT			= DEF_CMD(4);
const unsigned char CMD_EFF_VOLUME			= DEF_CMD(5);
const unsigned char CMD_EFF_PORTAMENTO		= DEF_CMD(6);
const unsigned char CMD_EFF_PORTAUP			= DEF_CMD(7);
const unsigned char CMD_EFF_PORTADOWN		= DEF_CMD(8);
const unsigned char CMD_EFF_SWEEP			= DEF_CMD(9);
const unsigned char CMD_EFF_ARPEGGIO		= DEF_CMD(10);
const unsigned char CMD_EFF_VIBRATO			= DEF_CMD(11);
const unsigned char CMD_EFF_TREMOLO			= DEF_CMD(12);
const unsigned char CMD_EFF_PITCH			= DEF_CMD(13);
const unsigned char CMD_EFF_DELAY			= DEF_CMD(14);
const unsigned char CMD_EFF_DAC				= DEF_CMD(15);
const unsigned char CMD_EFF_DUTY			= DEF_CMD(16);
const unsigned char CMD_EFF_OFFSET			= DEF_CMD(17);
const unsigned char CMD_EFF_SLIDE_UP		= DEF_CMD(18);
const unsigned char CMD_EFF_SLIDE_DOWN		= DEF_CMD(19);
const unsigned char CMD_EFF_VOL_SLIDE		= DEF_CMD(20);
const unsigned char CMD_EFF_NOTE_CUT		= DEF_CMD(21);
const unsigned char CMD_EFF_RETRIGGER		= DEF_CMD(22);
const unsigned char CMD_EFF_DPCM_PITCH		= DEF_CMD(23);

const unsigned char CMD_SET_DURATION		= DEF_CMD(24);	// B0h
const unsigned char CMD_RESET_DURATION		= DEF_CMD(25);	// B2h

const unsigned char CMD_EFF_FDS_MOD_DEPTH	= DEF_CMD(26);
const unsigned char CMD_EFF_FDS_MOD_RATE_HI = DEF_CMD(27);
const unsigned char CMD_EFF_FDS_MOD_RATE_LO = DEF_CMD(28);

const unsigned char CMD_EFF_VRC7_PATCH		= CMD_EFF_FDS_MOD_DEPTH;	// TODO: hack, fix this

const unsigned char CMD_LOOP_POINT			= DEF_CMD(26);	// Currently not in use


#define OPTIMIZE_DURATIONS		// Remove note durations when possible
#define QUICK_INST				// Remove instrument switch command for instrument 0 - 15

CPatternCompiler::CPatternCompiler() :
	m_iDataPointer(0),
	m_iCompressedDataPointer(0),
	m_pData(NULL),
	m_pCompressedData(NULL)
{
	memset(m_bDSamplesAccessed, 0, sizeof(bool) * MAX_DSAMPLES);
}

CPatternCompiler::~CPatternCompiler()
{
	SAFE_RELEASE_ARRAY(m_pData);
	SAFE_RELEASE_ARRAY(m_pCompressedData);
}

void CPatternCompiler::CleanUp()
{
	SAFE_RELEASE_ARRAY(m_pData);
	SAFE_RELEASE_ARRAY(m_pCompressedData);

	m_iDataPointer = 0;
	m_pData = new unsigned char[0x1000];

	m_iCompressedDataPointer = 0;
	m_pCompressedData = new unsigned char[0x1000];
}

void CPatternCompiler::CompileData(CFamiTrackerDoc *pDoc, int Track, int Pattern, int Channel, unsigned char (*DPCM_LookUp)[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE], unsigned int *iAssignedInstruments)
{
	unsigned int iPatternLen;
	unsigned char NESNote, Note, Octave, Instrument, LastInstrument, Volume;
	unsigned char Effect, EffParam;
	unsigned int DPCMInst = 0;
	bool Action;
	stSpacingInfo SpaceInfo;

	int EffColumns = pDoc->GetEffColumns(Track, Channel) + 1;
	int Chip = pDoc->GetExpansionChip();

	stChanNote ChanNote;

	CleanUp();

	// Global init
	m_bEmpty = true;
	m_iHash = 0;

	m_iZeroes = 0;
	m_iCurrentDefaultDuration = 0xFF;

	// Local init
	iPatternLen = pDoc->GetPatternLength(Track);
	LastInstrument = 0x41;

	Instrument = 0;

	for (unsigned int i = 0; i < iPatternLen; ++i) {
		pDoc->GetDataAtPattern(Track, Pattern, Channel, i, &ChanNote);

		Note		= ChanNote.Note;
		Octave		= ChanNote.Octave;
		Instrument	= FindInstrument(ChanNote.Instrument, iAssignedInstruments);
		Volume		= ChanNote.Vol;
		Action		= false;

		CTrackerChannel *pTrackerChannel = pDoc->GetChannel(Channel);
		int ChanID = pTrackerChannel->GetID();
		int ChipID = pTrackerChannel->GetChip();

		// Check for delays, must come first
		for (int j = 0; j < EffColumns; ++j) {
			Effect	 = ChanNote.EffNumber[j];
			EffParam = ChanNote.EffParam[j];
			if (Effect == EF_DELAY && EffParam > 0) {
				DispatchZeroes();
				Action = true;
				WriteData(CMD_EFF_DELAY);
				WriteData(EffParam);
			}
		}

#ifdef OPTIMIZE_DURATIONS

		// Determine length of space between notes
		SpaceInfo = ScanNoteLengths(pDoc, Track, i, Pattern, Channel);

		if (SpaceInfo.SpaceCount > 2) {
			if (SpaceInfo.SpaceSize != m_iCurrentDefaultDuration && SpaceInfo.SpaceCount != 0xFF) {
				// Enable compressed durations
				DispatchZeroes();
				m_iCurrentDefaultDuration = SpaceInfo.SpaceSize;
				WriteData(CMD_SET_DURATION);
				WriteData(m_iCurrentDefaultDuration);
			}
		}
		else {
			if (m_iCurrentDefaultDuration != 0xFF && m_iCurrentDefaultDuration != SpaceInfo.SpaceSize) {
				// Disable compressed durations
				DispatchZeroes();
				m_iCurrentDefaultDuration = 0xFF;
				WriteData(CMD_RESET_DURATION);
			}
		}
		
#endif
/*
		if (SpaceInfo.SpaceCount > 2 && SpaceInfo.SpaceSize != CurrentDefaultDuration) {
			CurrentDefaultDuration = SpaceInfo.SpaceSize;
			WriteData(CMD_SET_DURATION);
			WriteData(CurrentDefaultDuration);
		}
		else if (SpaceInfo.SpaceCount < 2 && SpaceInfo.SpaceSize == CurrentDefaultDuration) {
		}
		else 
*/
		if (Instrument != LastInstrument && Instrument < 0x40 && Note != HALT && Note != RELEASE) {

			LastInstrument = Instrument;
			// Write instrument change command
			//if (Channel < InstrChannels) {
			if (ChanID != CHANID_DPCM) {		// Skip DPCM
				DispatchZeroes();
#ifdef QUICK_INST
				if (Instrument < 0x10) {
					WriteData(0xE0 | Instrument);
				}
				else {
					WriteData(CMD_INSTRUMENT);
					WriteData(Instrument << 1);
				}
#else
				WriteData(CMD_INSTRUMENT);
				WriteData(Instrument << 1);
#endif
				Action = true;
			}
			else {
				DPCMInst = ChanNote.Instrument;
			}
		}
#ifdef OPTIMIZE_DURATIONS
		else if (Instrument == LastInstrument && Instrument < 0x40) {
			if (ChanID != CHANID_DPCM) {
				DispatchZeroes();
				Action = true;
			}
		}
#endif

		if (Note == 0) {
			NESNote = 0xFF;
		}
		else if (Note == HALT) {
			NESNote = 0x7F - 1;
		}
		else if (Note == RELEASE) {
			NESNote = 0x7F - 2;
		}
		else {
			if (ChanID == CHANID_DPCM) {
				// 2A03 DPCM
				int LookUp = (*DPCM_LookUp)[DPCMInst][Octave][Note - 1];
				if (LookUp > 0) {
					NESNote = LookUp << 1;
					int Sample = ((CInstrument2A03*)pDoc->GetInstrument(DPCMInst))->GetSample(Octave, Note - 1) - 1;
					m_bDSamplesAccessed[Sample] = true;
				}
				else
					NESNote = 0xFF;		// Invalid sample, skip
			}
			else if (ChanID == CHANID_NOISE) {
				// 2A03 Noise
				NESNote = (Note - 1) + (Octave * 12);
				if (NESNote == 0)
					NESNote += 0x10;
			}
			else
				// All other channels
				NESNote = (Note - 1) + (Octave * 12);
		}

		for (unsigned int j = 0; j < (pDoc->GetEffColumns(Track, Channel) + 1); ++j) {
			Effect	 = ChanNote.EffNumber[j];
			EffParam = ChanNote.EffParam[j];
			
			if (Effect > 0) {
				DispatchZeroes();
				Action = true;
			}

			switch (Effect) {
				case EF_SPEED:
					WriteData(CMD_EFF_SPEED);
					if (EffParam == 0)
						WriteData(1);			// NSF halts if 0 is exported
					else
						WriteData(EffParam);
					break;
				case EF_JUMP:
					WriteData(CMD_EFF_JUMP);
					WriteData(EffParam + 1);
					break;
				case EF_SKIP:
					WriteData(CMD_EFF_SKIP);
					WriteData(EffParam + 1);
					break;
				case EF_HALT:
					WriteData(CMD_EFF_HALT);
					WriteData(EffParam);
					break;
				case EF_VOLUME:
					// TODO: remove this
					if (Channel < 5) {
						WriteData(CMD_EFF_VOLUME);
						WriteData(EffParam);
					}
					break;
				case EF_PORTAMENTO:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_PORTAMENTO);
						WriteData(EffParam);
					}
					break;
				case EF_PORTA_UP:
					if (ChanID != CHANID_DPCM) {
						if (ChipID == SNDCHIP_FDS || ChipID == SNDCHIP_VRC7 || ChipID == SNDCHIP_N163)
							WriteData(CMD_EFF_PORTADOWN);	// Pitch is inverted for these chips
						else
							WriteData(CMD_EFF_PORTAUP);
						WriteData(EffParam);
					}
					break;
				case EF_PORTA_DOWN:
					if (ChanID != CHANID_DPCM) {
						if (ChipID == SNDCHIP_FDS || ChipID == SNDCHIP_VRC7 || ChipID == SNDCHIP_N163)
							WriteData(CMD_EFF_PORTAUP);
						else
							WriteData(CMD_EFF_PORTADOWN);
						WriteData(EffParam);
					}
					break;
					/*
				case EF_PORTAOFF:
					if (Channel < 5) {
						WriteData(CMD_EFF_PORTAOFF);
						//WriteData(EffParam);
					}
					break;*/
				case EF_SWEEPUP:
					if (ChanID < CHANID_TRIANGLE) {
						WriteData(CMD_EFF_SWEEP);
						WriteData(0x88 | (EffParam & 0x77));	// Calculate sweep
					}
					break;
				case EF_SWEEPDOWN:
					if (ChanID < CHANID_TRIANGLE) {
						WriteData(CMD_EFF_SWEEP);
						WriteData(0x80 | (EffParam & 0x77));	// Calculate sweep
					}
					break;
				case EF_ARPEGGIO:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_ARPEGGIO);
						WriteData(EffParam);
					}
					break;
				case EF_VIBRATO:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_VIBRATO);
						//WriteData(EffParam);
						WriteData((EffParam & 0xF) << 4 | (EffParam >> 4));
					}
					break;
				case EF_TREMOLO:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_TREMOLO);
//						WriteData(EffParam & 0xF7);
						WriteData((EffParam & 0xF) << 4 | (EffParam >> 4));
					}
					break;
				case EF_PITCH:
					if (ChanID != CHANID_DPCM) {
						switch (ChipID) {
							case SNDCHIP_VRC7:
							case SNDCHIP_FDS:
							case SNDCHIP_N163:
								EffParam = (char)(256 - (int)EffParam);
								if (EffParam == 0)
									EffParam = 0xFF;
								break;
						}
						WriteData(CMD_EFF_PITCH);
						WriteData(EffParam);
					}
					break;
				case EF_DAC:
					if (ChanID == CHANID_DPCM) {
						WriteData(CMD_EFF_DAC);
						WriteData(EffParam & 0x7F);
					}
					break;
				case EF_DUTY_CYCLE:
					if (ChipID == SNDCHIP_VRC7) {
						WriteData(CMD_EFF_VRC7_PATCH);
						WriteData(EffParam << 4);
					}
					else if (ChanID != CHANID_TRIANGLE && ChanID != CHANID_DPCM) {	// Not triangle and dpcm
						WriteData(CMD_EFF_DUTY);
						WriteData(EffParam);
					}
					break;
				case EF_SAMPLE_OFFSET:
					if (ChanID == CHANID_DPCM) {	// DPCM
						WriteData(CMD_EFF_OFFSET);
						WriteData(EffParam);
					}
					break;
				case EF_SLIDE_UP:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_SLIDE_UP);
						WriteData(EffParam);
					}
					break;
				case EF_SLIDE_DOWN:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_SLIDE_DOWN);
						WriteData(EffParam);
					}
					break;
				case EF_VOLUME_SLIDE:
					if (ChanID != CHANID_DPCM) {
						WriteData(CMD_EFF_VOL_SLIDE);
						WriteData(EffParam);
					}
					break;
				case EF_NOTE_CUT:
					WriteData(CMD_EFF_NOTE_CUT);
					WriteData(EffParam + 1);
					break;
				case EF_RETRIGGER:
					if (ChanID == CHANID_DPCM) {
						WriteData(CMD_EFF_RETRIGGER);
						WriteData(EffParam + 1);
					}
					break;
				case EF_DPCM_PITCH:
					if (ChanID == CHANID_DPCM) {
						WriteData(CMD_EFF_DPCM_PITCH);
						WriteData(EffParam);
					}
					break;
				// FDS
				case EF_FDS_MOD_DEPTH:
					if (ChanID == CHANID_FDS) {
						WriteData(CMD_EFF_FDS_MOD_DEPTH);
						WriteData(EffParam);
					}
					break;
				case EF_FDS_MOD_SPEED_HI:
					if (ChanID == CHANID_FDS) {
						WriteData(CMD_EFF_FDS_MOD_RATE_HI);
						WriteData(EffParam & 0x0F);
					}
					break;
				case EF_FDS_MOD_SPEED_LO:
					if (ChanID == CHANID_FDS) {
						WriteData(CMD_EFF_FDS_MOD_RATE_LO);
						WriteData(EffParam);
					}
					break;
			}
		}

		// Volume command
		if (Volume < 0x10) {
			unsigned char Vol = Volume;
			//if (pDoc->GetChannelType(Channel) == SNDCHIP_VRC7) {
			//if (pTrackerChannel->GetChip() == SNDCHIP_VRC7) {
				// VRC7 volumes are inverted
				//Vol ^= 0x0F;
			//}
			DispatchZeroes();
			WriteData(0xF0 | Vol);
			Action = true;			// Terminate command
		} 

		if (NESNote == 0xFF) {
			if (Action) {
				// A instrument/effect command was issued but no new note, write rest command
				WriteData(0);
				m_bEmpty = false;
			}
			AccumulateZero();
		}
		else {
			// Write note command
			DispatchZeroes();
			WriteData(NESNote + 1);
			AccumulateZero();
			m_bEmpty = false;
		}
	}

	DispatchZeroes();

	OptimizeString();
}

unsigned int CPatternCompiler::FindInstrument(int Instrument, unsigned int *pInstList)
{
	for (int i = 0; i < MAX_INSTRUMENTS; i++) {
		if (pInstList[i] == Instrument)
			return i;
	}

	return Instrument;
}

stSpacingInfo CPatternCompiler::ScanNoteLengths(CFamiTrackerDoc *pDoc, int Track, unsigned int StartRow, int Pattern, int Channel)
{
	stChanNote NoteData;
	bool NoteUsed;
	int StartSpace = -1, Space = 0, SpaceCount = 0;
	stSpacingInfo Info;

	Info.SpaceCount = 0;
	Info.SpaceSize = 0;

	for (unsigned i = StartRow; i < pDoc->GetPatternLength(Track); ++i) {
		pDoc->GetDataAtPattern(Track, Pattern, Channel, i, &NoteData);
		NoteUsed = false;

		if (NoteData.Note > 0)
			NoteUsed = true;
		if (NoteData.Instrument < MAX_INSTRUMENTS)
			NoteUsed = true;
		if (NoteData.Vol < 0x10)
			NoteUsed = true;
		for (unsigned j = 0; j < (pDoc->GetEffColumns(Track, Channel) + 1); ++j) {
			if (NoteData.EffNumber[j] != EF_NONE)
				NoteUsed = true;
		}

		if (i == StartRow && NoteUsed == false) {
			Info.SpaceCount = 0xFF;
			Info.SpaceSize = StartSpace;
			return Info;
		}

		if (i > StartRow) {
			if (NoteUsed) {
				if (StartSpace == -1)
					StartSpace = Space;
				else {
					if (StartSpace == Space)
						SpaceCount++;
					else {
						Info.SpaceCount = SpaceCount;
						Info.SpaceSize = StartSpace;
						return Info;
					}
				}
				Space = 0;
			}
			else
				Space++;
		}
	}

	if (StartSpace == Space) {
		SpaceCount++;
	}

	Info.SpaceCount = SpaceCount;
	Info.SpaceSize = StartSpace;

	return Info;
}

void CPatternCompiler::WriteData(unsigned char Value)
{
	ASSERT(m_iDataPointer < 0x1000);
	m_pData[m_iDataPointer++] = Value;
	m_iHash = ((m_iHash << 5) ^ ((m_iHash & 0xF8000000) >> 27)) ^ Value;	// Simple CRC-hash
}

void CPatternCompiler::AccumulateZero()
{
	m_iZeroes++;
}

void CPatternCompiler::DispatchZeroes()
{
	if (m_iCurrentDefaultDuration == 0xFF) {
		if (!m_iDataPointer && m_iZeroes > 0)
			WriteData(0x00);
		if (m_iZeroes > 0)
			WriteData(m_iZeroes - 1);
	}

	m_iZeroes = 0;
}

// Returns the size of the block at 'position' in the data array. A block is terminated by a note
int CPatternCompiler::GetBlockSize(int Position)
{
	unsigned char data;
	unsigned int Pos = Position, i;

	int iDuration = 1;

	// Find if note duration optimization is on
	for (i = 0; i < unsigned(Position); ++i) {
		if (m_pData[i] == CMD_SET_DURATION)
			iDuration = 0;
		else if (m_pData[i] == CMD_RESET_DURATION)
			iDuration = 1;
	}

	for (; Pos < m_iDataPointer; Pos++) {
		data = m_pData[Pos];
		if (data < 0x80) {		// Note
			//int size = (Pos + 1 + iDuration) - Position;
			int size = (Pos - Position);
//			if (size > 1)
//				return size - 1;

			return size + 1 + iDuration;// (Pos + 1 + iDuration) - Position;
		}
		else if (data == CMD_SET_DURATION)
			iDuration = 0;
		else if (data == CMD_RESET_DURATION)
			iDuration = 1;
		else {
			if (data < 0xE0 || data > 0xEF)
				Pos++;				// Command, skip parameter
		}
	//	Pos++;
	}

	// Error
	return 1;
}

void CPatternCompiler::OptimizeString()
{
	// Try to optimize by finding repeating patterns and compress them into a loop (simple RLE)
	//

	//
	// Ok, just figured this won't work without using loads of NES RAM so I'll
	// probably put this on hold for a while
	//

	unsigned int i, j, k, l;
	int matches, best_matches, best_length, last_inst;
	bool matched;

	/*

	80 00 2E 00 2E 00 2E 00 2E 00 2E 00 2E 00 ->
	80 00 2E 00 FF 06 02

	*/

	// Always copy first 2 bytes
//	memcpy(m_pCompressedData, m_pData, 2);
//	m_iCompressedDataPointer += 2;

	if (m_pData[0] == 0x80)
		last_inst = m_pData[1];
	else
		last_inst = 0;

	// Loop from start
	for (i = 0; i < m_iDataPointer; /*i += 2*/) {

		best_matches = 0;

		// Instrument
		if (m_pData[i] == 0x80)
			last_inst = m_pData[i + 1];
		else if (m_pData[i] >= 0xE0 && m_pData[i] <= 0xEF)
			last_inst = m_pData[i & 0xF];

		// Start checking from the first tuple
		for (l = GetBlockSize(i); l < (m_iDataPointer - i); /*l += 2*/) {
			matches = 0;
			// See how many following matches there are from this combination in a row
			for (j = i + l; j <= m_iDataPointer; j += l) {
				matched = true;
				// Compare one word
				for (k = 0; k < l; k++) {
					if (m_pData[i + k] != m_pData[j + k])
						matched = false;
				}
				if (!matched)
					break;
				matches++;
				/*
				if ((j + l) <= m_iDataPointer) {
					if (memcmp(m_pData + i, m_pData + j, l) == 0)
						matches++;
					else
						break;
				}
				*/
			}
			// Save
			if (matches > best_matches) {
				best_matches = matches;
				best_length = l;
			}

			l += GetBlockSize(i + l);
		}
		// Compress
		if ((best_matches > 1 && best_length > 4) || best_matches > 2 /*&& (best_length > 2 && best_matches > 1)*/) {
			// Include the first one
			best_matches++;
			int size = best_length * best_matches;
			//
			// Last known instrument must also be added
			//
			memcpy(m_pCompressedData + m_iCompressedDataPointer, m_pData + i, best_length);
			m_iCompressedDataPointer += best_length;
			// Define a loop point: 0xFF (number of loops) (number of bytes)
			m_pCompressedData[m_iCompressedDataPointer++] = CMD_LOOP_POINT;
			m_pCompressedData[m_iCompressedDataPointer++] = best_matches - 1;	// the nsf code sees one less
			m_pCompressedData[m_iCompressedDataPointer++] = best_length;
			i += size;
		}
		else {
			// No loop
			int size = GetBlockSize(i);
			memcpy(m_pCompressedData + m_iCompressedDataPointer, m_pData + i, size);
			m_iCompressedDataPointer += size;
			i += size;
		}
	}
}	

bool CPatternCompiler::EmptyPattern() const
{
	return m_bEmpty;
}

unsigned int CPatternCompiler::GetHash() const
{
	return m_iHash;
}