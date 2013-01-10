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

#pragma once

#include <QString>

#include "patterncompiler.h"

struct stNSFHeader {
	unsigned char	Ident[5];
	unsigned char	Version;
	unsigned char	TotalSongs;
	unsigned char	StartSong;
	unsigned short	LoadAddr;
	unsigned short	InitAddr;
	unsigned short	PlayAddr;
	unsigned char	SongName[32];
	unsigned char	ArtistName[32];
	unsigned char	Copyright[32];
	unsigned short	Speed_NTSC;
	unsigned char	BankValues[8];
	unsigned short	Speed_PAL;
	unsigned char	Flags;
	unsigned char	SoundChip;
	unsigned char	Reserved[4];
};

const unsigned int MAX_BANKS = 0x80;

// A databank is a 4kB NSF bank
// It holds the bank data, location and a write pointer
class CDataBank {
public:
	CDataBank();
	~CDataBank();
	void			AllocateBank(unsigned char Origin);
	void			WriteByte(unsigned int Pointer, unsigned char Value);
	void			WriteShort(unsigned int Pointer, unsigned short Value);
	bool			Overflow(unsigned int iSize) const;
	unsigned char	*GetData() const;
	void			CopyData(unsigned char *pToArray) const;

private:
	unsigned char	*m_pData;
	unsigned int	m_iOrigin;
	unsigned int	m_iPointer;
};

// This is stored first in NSF data, 8 bytes
struct stTuneHeader {
	unsigned short	SongListOffset;
	unsigned short	InstrumentListOffset;
	unsigned char	Flags;
	unsigned short	DPCMInstListOffset;
	unsigned short	DPCMSamplesOffset;
	unsigned short	DividerNTSC;
	unsigned short	DividerPAL;
	unsigned short	WaveTable;
};

// One song header per track, 6 bytes
struct stSongHeader {
	unsigned short	FrameListOffset;
	unsigned char	FrameCount;
	unsigned char	PatternLength;
	unsigned char	Speed;
	unsigned char	Tempo;
	unsigned char	FrameBank;
};

class CCompiler
{
public:
	CCompiler(CFamiTrackerDoc *pDoc);
	virtual ~CCompiler();

	void ExportNSF(QString FileName, bool EnablePAL);
	void ExportNES(QString FileName, bool EnablePAL);
	void ExportBIN(QString BIN_File, QString DPCM_File);
	void ExportPRG(QString FileName, bool EnablePAL);
	void ExportASM(QString FileName);

	void StoreShort(unsigned short Value);
	void StoreByte(unsigned char Value);
	void WriteLog(char *text, ...);

	unsigned int GetSequenceAddress2A03(int Sequence, int Type) const;
	unsigned int GetSequenceAddressVRC6(int Sequence, int Type) const;
	unsigned int GetSequenceAddressFDS(int Instrument, int Type) const;

	CFamiTrackerDoc *GetDocument() const { return m_pDocument; };

	// FDS
	int AddWavetable(unsigned char Wave[64]);

private:
	void CreateHeader(stNSFHeader *pHeader, bool EnablePAL);

	const char *LoadDriver(const char *driver) const;

	void AllocateSpace();
	void RemoveSpace();
	void ScanSong();

	void CompileData();
	void AssembleData();

	// Data bank functions
	void AllocateNewBank(unsigned int iAddress);
	void SetMemoryOffset(unsigned int iOffset);
	void SetInitialPosition(unsigned int iAddress);
	void SkipBytes(unsigned int iBytes);

	unsigned int GetCurrentOffset() const;
	unsigned int GetAbsoluteAddress() const;

	// Song conversion functions
	void WriteHeader(stTuneHeader *pHeader);
	void WriteTrackHeader(stSongHeader *m_stTrackHeader, unsigned int iAddress);

	void StoreSongs();
	void CreateFrameList(int Track);
	void StorePatterns(unsigned int Track);

	void CreateInstrumentList();
	void CreateSequenceList();

	void CreateDPCMList();
	void StoreDPCM();
	void StoreDPCM2();

	unsigned int StoreSequence(CSequence *pSeq);

	bool IsPatternAddressed(int Track, int Pattern, int Channel);

	int GetSampleIndex(int SampleNumber);
	bool IsSampleAccessed(int SampleNumber);

	// Debugging
	void Print(char *text, ...) const;

private:
	const int		*m_pChanOrder;

private:
	// Data
	unsigned char	*m_pData;					// Linear music data
	unsigned char	*m_pDPCM;					// DPCM samples (for both bank switched and linear)
	unsigned int	m_iDataPointer;
	unsigned int	m_iInitialPosition;
	bool			m_bBankSwitched;			// True for bank switched song
	unsigned int	m_iMasterHeaderAddr;
	unsigned int	m_iDriverSize;				// Size of driver binary

//	unsigned char	*m_pDPCMBanks[MAX_BANKS];

	// For bank switching
	CDataBank		m_DataBanks[MAX_BANKS];
	unsigned int	m_iSelectedBank;
	unsigned int	m_iAllocatedBanks;
	unsigned char	m_cSelectedBanks[8];		// There are 8 banks visible, $8000-$FFFF
	CDataBank		m_pSelectedBanks[8];

	unsigned int	m_iLoadAddress;
	unsigned int	m_iDriverLocation;
	bool			m_bErrorFlag;

	// Sequences and instruments
	unsigned int	m_iDPCMSize;
	unsigned int	m_iInstruments;
	unsigned int	m_iSequences;
	unsigned int	m_iAssignedInstruments[MAX_INSTRUMENTS];
	unsigned int	m_iSequenceAddresses2A03[MAX_SEQUENCES][SEQ_COUNT];
	unsigned int	m_iSequenceAddressesVRC6[MAX_SEQUENCES][SEQ_COUNT];
	unsigned int	m_iSequenceAddressesFDS[MAX_INSTRUMENTS][SEQ_COUNT];

	// Patterns and frames
	unsigned short	m_iPatternAddresses[MAX_PATTERN][MAX_CHANNELS];
	unsigned char	m_iPatternBanks[MAX_PATTERN][MAX_CHANNELS];
	unsigned int	m_iFrameListAddress;

	CPatternCompiler m_oPatternCompiler;

	// Sample variables
	unsigned char	m_iDPCM_LookUp[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE];
	bool			m_bSamplesAccessed[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE];
	unsigned char	m_iSampleBank[MAX_DSAMPLES];
	unsigned int	m_iSamplesUsed;
	unsigned int	m_iSampleStart;

	// Other
	stTuneHeader	m_stTuneHeader;
	stSongHeader	m_stTrackHeader;

	CFamiTrackerDoc	*m_pDocument;

	int				m_iFrameBank;

	// Expansion chips
	// FDS
	unsigned char	*m_iWaveTable[64];
	int				m_iWaveTables;
};
