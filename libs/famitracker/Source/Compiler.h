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

#include <QString>

#include "Chunk.h"

// NSF file header
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

const int BANK_SIZE = 4096;	// The NSF specification

/*
 * NSF file bank
 */
class CFileBank
{
public:
	CFileBank() : m_iSize(0), m_iLocation(0), m_iOffset(0) {};

	char m_data[BANK_SIZE];
	int	 m_iSize;
	int	 m_iLocation;
	int	 m_iOffset;
};

struct driver_t;

/*
 * The compiler
 */
class CCompiler
{
public:
	CCompiler(CFamiTrackerDoc *pDoc, CEdit *pLogText);
	~CCompiler();

	void	ExportNSF(CString FileName, int MachineType);
	void	ExportNES(CString FileName, bool EnablePAL);
	void	ExportBIN(CString BIN_File, CString DPCM_File);
	void	ExportPRG(CString FileName, bool EnablePAL);
	void	ExportASM(CString FileName);

private:
	void	CreateHeader(stNSFHeader *pHeader, int MachineType);
	void	SetDriverSongAddress(unsigned char *pDriver, unsigned short Address);

	void	PatchVibratoTable(unsigned char *pDriver);
	void	PatchSpeedSplitPoint(unsigned char *pDriver);

	unsigned char *LoadDriver(const driver_t *pDriver, unsigned short Origin) const;

	// NSF banks
	void	CopyData(char *pData, int iSize);
	void	AllocateBank(int Location);

	// Compiler
	bool	CompileData();
	void	AllocateData();
	void	AllocateDataBankswitched();
	void	AddBankswitching();
	void	Cleanup();

	void	ScanSong();
	int		GetSampleIndex(int SampleNumber);
	bool	IsPatternAddressed(int Track, int Pattern, int Channel);
	bool	IsInstrumentInPattern(int index) const;

	void	CreateMainHeader();
	void	CreateSequenceList();
	void	CreateInstrumentList();
	void	CreateSampleList();
	void	CreateFrameList(int Track, int *pFrameSize, int *pFrameCount);

	CChunk *StoreSequence(CSequence *pSeq, CString &label);
	void	StoreSamples();
	void	StoreSongs();
	void	StorePatterns(unsigned int Track, int *pPatternSize, int *pPatternCount);

	void	WriteSamplesAssembly(CFile *pFile);
	void	WriteSamplesBinary(CFile *pFile);
	void	WriteSamplesToBanks(unsigned short Address);

	// Bankswitching functions
	void	UpdateSamplePointers(unsigned short Origin);
	void	UpdateFrameBanks();
	void	UpdateSongBanks();
	void	EnableBankswitching();

	int		AdjustSampleAddress(unsigned short Address);

	// FDS
	void	AddWavetable(CInstrumentFDS *pInstrument, CChunk *pChunk);

	// File writing
	void	WriteAssembly(CFile *pFile);
	void	WriteBinary(CFile *pFile);
	void	WriteBinaryBanks();
	void	WriteBinaryBankswitched();

	// Object list functions
	CChunk	*CreateChunk(int Type, CString label);
	CChunk	*GetObjectByRef(CString label) const;
	int		CountData() const;

	// File functions
	void	WriteFileString(CFile *pFile, CString &str);

	// Debugging
	void	Print(char *text, ...) const;
	void	ClearLog() const;

public:
	static const int PAGE_SIZE;
	static const int PAGE_START;
	static const int PAGE_BANKED;
	static const int PAGE_SAMPLES;

	// Channel order lists
	static const int CHAN_ORDER_DEFAULT[];
	static const int CHAN_ORDER_VRC6[];
	static const int CHAN_ORDER_MMC5[];
	static const int CHAN_ORDER_VRC7[];
	static const int CHAN_ORDER_FDS[];
	static const int CHAN_ORDER_N163[];
	static const int CHAN_ORDER_S5B[];

	// Labels
	static const char LABEL_SONG_LIST[];
	static const char LABEL_INSTRUMENT_LIST[];
	static const char LABEL_SAMPLES_LIST[];
	static const char LABEL_SAMPLES[];
	static const char LABEL_WAVETABLE[];
	static const char LABEL_WAVES[];
	static const char LABEL_SEQ_2A03[];
	static const char LABEL_SEQ_VRC6[];
	static const char LABEL_SEQ_FDS[];
	static const char LABEL_SEQ_N163[];
	static const char LABEL_INSTRUMENT[];
	static const char LABEL_SONG[];
	static const char LABEL_SONG_FRAMES[];
	static const char LABEL_SONG_FRAME[];
	static const char LABEL_PATTERN[];

private:
	int				m_iChanOrder[MAX_CHANNELS];

private:
	CFamiTrackerDoc *m_pDocument;

	// Object list
	std::vector<CChunk*> m_vChunks;
	std::vector<CChunk*> m_vSequenceChunks;
	std::vector<CChunk*> m_vInstrumentChunks;
	std::vector<CChunk*> m_vSongChunks;
	std::vector<CChunk*> m_vFrameChunks;
	std::vector<CChunk*> m_vPatternChunks;


	// Special objects
	CChunk			*m_pSamplePointersChunk;
	CChunk			*m_pHeaderChunk;

	// Samples
	std::vector<CDSample*> m_vSamples;

	// Flags
	bool			m_bBankSwitched;

	// Driver
	const driver_t	*m_pDriverData;
	unsigned int	m_iVibratoTableLocation;
	unsigned int	m_iSpeedPatchLocation;

	// Sequences and instruments
	unsigned int	m_iInstruments;
	unsigned int	m_iAssignedInstruments[MAX_INSTRUMENTS];
	bool			m_bSequencesUsed2A03[MAX_SEQUENCES][SEQ_COUNT];
	bool			m_bSequencesUsedVRC6[MAX_SEQUENCES][SEQ_COUNT];
	bool			m_bSequencesUsedN163[MAX_SEQUENCES][SEQ_COUNT];

	// Sample variables
	unsigned char	m_iSamplesLookUp[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE];
	bool			m_bSamplesAccessed[MAX_INSTRUMENTS][OCTAVE_RANGE][NOTE_RANGE];
	unsigned char	m_iSampleBank[MAX_DSAMPLES];
	unsigned int	m_iSampleStart;
	unsigned int	m_iSamplesUsed;

	unsigned int	m_iMusicDataSize;		// All music data
	unsigned int	m_iDriverSize;			// Size of selected music driver
	unsigned int	m_iSamplesSize;

	unsigned int	m_iLoadAddress;			// NSF load address
	unsigned int	m_iInitAddress;			// NSF init address
	unsigned int	m_iDriverAddress;		// Music driver location

	unsigned int	m_iTrackFrameSize[MAX_TRACKS];

	unsigned int	m_iDuplicatePatterns;

	// NSF banks
	CFileBank		*m_pFileBanks[256];
	CFileBank		*m_pCurrentBank;
	unsigned int	m_iBanksUsed;
	unsigned int	m_iFirstSampleBank;

	unsigned int	m_iSamplePointerBank;
	unsigned int	m_iSamplePointerOffset;

	// FDS
	unsigned int	m_iWaveTables;

	// Optimization
	CMap<UINT, UINT, CChunk*, CChunk*>		 m_PatternMap;
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_DuplicateMap;

	// Debugging
	CEdit			*m_pLogText;
};
