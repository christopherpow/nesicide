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

#include "cqtmfc.h"

// Get access to some APU constants
#include "APU/APU.h"
// Some pure virtual interfaces that expose non MFC functionality to exporter plugins
#include "CustomExporterInterfaces.h"
// Constants, types and enums
#include "FamiTrackerTypes.h"

#define TRANSPOSE_FDS

// Default song settings
const unsigned int DEFAULT_TEMPO_NTSC   = 150;
const unsigned int DEFAULT_TEMPO_PAL    = 125;
const unsigned int DEFAULT_SPEED	    = 6;
const unsigned int DEFAULT_MACHINE_TYPE = NTSC;

const unsigned int DEFAULT_SPEED_SPLIT_POINT = 32;
const unsigned int OLD_SPEED_SPLIT_POINT	 = 21;

// Columns
enum {C_NOTE, 
	  C_INSTRUMENT1, C_INSTRUMENT2, 
	  C_VOLUME, 
	  C_EFF_NUM, C_EFF_PARAM1, C_EFF_PARAM2,
	  C_EFF2_NUM, C_EFF2_PARAM1, C_EFF2_PARAM2, 
	  C_EFF3_NUM, C_EFF3_PARAM1, C_EFF3_PARAM2, 
	  C_EFF4_NUM, C_EFF4_PARAM1, C_EFF4_PARAM2
};

const unsigned int COLUMNS = 7;

const unsigned int VOLUME_EMPTY = 0x10;		// Value of cleared volume column field

// Special assert used when loading files
#ifdef _DEBUG
	#define ASSERT_FILE_DATA(Statement) ASSERT(Statement)
#else
	#define ASSERT_FILE_DATA(Statement) if (!(Statement)) return true
#endif

// View update modes
enum {
	CHANGED_TRACK = 1,		// Changed track
	CHANGED_TRACKCOUNT,		// Update track count
	CHANGED_FRAMES,			// Redraw frame window
	CHANGED_PATTERN,
	CHANGED_HEADER,			// Redraw header (???)
	CHANGED_ERASE,			// Also make sure background is erased
	CHANGED_CLEAR_SEL,		// Clear selection
	
	UPDATE_ENTIRE,			// Redraw entire document
	UPDATE_FAST,			// Fast document drawing
	UPDATE_EDIT,			// When document changed
	UPDATE_CURSOR,			// When cursor moved
	UPDATE_FRAME,			// Frame cursor has changed
	UPDATE_PLAYING,			// Tracker is playing
	UPDATE_HIGHLIGHT,		// Highlight option has changed

	UPDATE_INSTRUMENTS,		// Instrument list has changed
	CHANGED_CHANNEL_COUNT,	// Number of channels changed

	CLOSE_DOCUMENT			// Document is closing
};

// Old sequence list, kept for compability
struct stSequence {
	unsigned int Count;
	signed char Length[MAX_SEQUENCE_ITEMS];
	signed char Value[MAX_SEQUENCE_ITEMS];
};


// Access data types used by the document class
#include "Sequence.h"
#include "PatternData.h"
#include "Instrument.h"


// External classes
class CTrackerChannel;
class CMainFrame;
class CDocumentFile;

// Use this when it's done
/*
class CPatternCell {
public:
	unsigned char	GetNote() { return m_cNote; };
	unsigned char	GetOctave() { return m_cOctave; };
	unsigned char	GetVolume() { return m_cVolume; };
	unsigned char	GetInstrument() { return m_cInstrument; };
	unsigned char	GetEffect(unsigned char Column) { return m_cEffNumber[MAX_EFFECT_COLUMNS]; };
	unsigned char	GetEffectParam(unsigned char Column) { return m_cEffParam[MAX_EFFECT_COLUMNS]; };
public:
	unsigned char	m_cNote;
	unsigned char	m_cOctave;
	unsigned char	m_cVolume;
	unsigned char	m_cInstrument;
	unsigned char	m_cEffNumber[MAX_EFFECT_COLUMNS];
	unsigned char	m_cEffParam[MAX_EFFECT_COLUMNS];

};
*/

//
// I'll try to organize this class, things are quite messy right now!
//

class CFamiTrackerDoc : public QObject, public CDocument, public CFamiTrackerDocInterface
{
   Q_OBJECT
public:
	CFamiTrackerDoc(QObject* parent = NULL);
	DECLARE_DYNCREATE(CFamiTrackerDoc)

	int m_iVersion;
public:
	static const int CLASS_VERSION = 3;
	const int GetVersion() const { return m_iVersion; };

	// Static functions
public:
	static CFamiTrackerDoc *GetDoc();

// Attributes
public:
	CString GetFileTitle() const;
/*
	// Other
	void AutoSave();
*/
	//
	// Public functions
	//
public:

	//
	// Document file I/O
	//
	bool IsFileLoaded();
	bool HasLastLoadFailed() const;

	// Import
	CFamiTrackerDoc *LoadImportFile(LPCTSTR lpszPathName);
	bool ImportInstruments(CFamiTrackerDoc *pImported, int *pInstTable);
	bool ImportTrack(int Track, CFamiTrackerDoc *pImported, int *pInstTable);

	//
	// Interface functions (not related to document data)
	//
	void			ResetChannels();
	void			RegisterChannel(CTrackerChannel *pChannel, int ChannelType, int ChipType);
	CTrackerChannel	*GetChannel(int Index) const;

	int				GetChannelType(int Channel) const;
	int				GetChipType(int Channel) const;
	int				GetChannelCount() const;


	//
	// Document data access functions
	//

	// Local (song) data
	void			SetFrameCount(unsigned int Count);
	void			SetPatternLength(unsigned int Length);
	void			SetSongSpeed(unsigned int Speed);
	void			SetSongTempo(unsigned int Tempo);

	unsigned int	GetPatternLength()		const { return m_pSelectedTune->GetPatternLength(); };
	unsigned int	GetFrameCount()			const { return m_pSelectedTune->GetFrameCount(); };
	unsigned int	GetSongSpeed()			const { return m_pSelectedTune->GetSongSpeed(); };
	unsigned int	GetSongTempo()			const { return m_pSelectedTune->GetSongTempo(); };
	unsigned int	GetAvailableChannels()	const { return m_iChannelsAvailable; };

	unsigned int	GetPatternLength(int Track) const { return m_pTunes[Track]->GetPatternLength(); };
	unsigned int	GetFrameCount(int Track)	const { return m_pTunes[Track]->GetFrameCount(); };
	unsigned int	GetSongSpeed(int Track)		const { return m_pTunes[Track]->GetSongSpeed(); };
	unsigned int	GetSongTempo(int Track)		const { return m_pTunes[Track]->GetSongTempo(); };

	unsigned int	GetEffColumns(int Track, unsigned int Channel) const;
	unsigned int	GetEffColumns(unsigned int Channel) const; 
	void			SetEffColumns(unsigned int Channel, unsigned int Columns);

	unsigned int 	GetPatternAtFrame(int Track, unsigned int Frame, unsigned int Channel) const;
	unsigned int	GetPatternAtFrame(unsigned int Frame, unsigned int Channel) const;
	void			SetPatternAtFrame(unsigned int Frame, unsigned int Channel, unsigned int Pattern);

	int				GetFirstFreePattern(int Channel);

	void			ClearPatterns();

	// Pattern editing
	void			IncreasePattern(unsigned int Frame, unsigned int Channel, int Count);
	void			DecreasePattern(unsigned int Frame, unsigned int Channel, int Count);
	void			IncreaseInstrument(unsigned int Frame, unsigned int Channel, unsigned int Row);
	void			DecreaseInstrument(unsigned int Frame, unsigned int Channel, unsigned int Row);
	void			IncreaseVolume(unsigned int Frame, unsigned int Channel, unsigned int Row);
	void			DecreaseVolume(unsigned int Frame, unsigned int Channel, unsigned int Row);
	void			IncreaseEffect(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Index);
	void			DecreaseEffect(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Index);

	void			SetNoteData(unsigned int Frame, unsigned int Channel, unsigned int Row, stChanNote *Data);
	void			GetNoteData(unsigned int Frame, unsigned int Channel, unsigned int Row, stChanNote *Data) const;

	void			SetDataAtPattern(unsigned int Track, unsigned int Pattern, unsigned int Channel, unsigned int Row, stChanNote *Data);
	void			GetDataAtPattern(unsigned int Track, unsigned int Pattern, unsigned int Channel, unsigned int Row, stChanNote *Data) const;

	unsigned int	GetNoteEffectType(unsigned int Frame, unsigned int Channel, unsigned int Row, int Index) const;
	unsigned int	GetNoteEffectParam(unsigned int Frame, unsigned int Channel, unsigned int Row, int Index) const;

	void			ClearPattern(unsigned int Frame, unsigned int Channel);

	bool			InsertRow(unsigned int Frame, unsigned int Channel, unsigned int Row);
	bool			DeleteNote(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Column);
	bool			ClearRow(unsigned int Frame, unsigned int Channel, unsigned int Row);
	bool			ClearRowField(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Column);
	bool			RemoveNote(unsigned int Frame, unsigned int Channel, unsigned int Row);
	bool			PullUp(unsigned int Frame, unsigned int Channel, unsigned int Row);

	// Frame editing
	bool			InsertFrame(int Pos);
	bool			RemoveFrame(int Pos);
	bool			DuplicateFrame(int Pos);
	bool			DuplicatePatterns(int Pos);
	bool			MoveFrameDown(int Pos);
	bool			MoveFrameUp(int Pos);

	// Global (module) data
	void			SetEngineSpeed(unsigned int Speed);
	void			SetMachine(unsigned int Machine);
	unsigned int	GetMachine() const		{ return m_iMachine; };
	unsigned int	GetEngineSpeed() const	{ return m_iEngineSpeed; };
	unsigned int	GetFrameRate(void) const;

	void			SelectExpansionChip(unsigned char Chip);
	unsigned char	GetExpansionChip() const { return m_iExpansionChip; };
	bool			ExpansionEnabled(int Chip) const;

	int				GetNamcoChannels() const;
	void			SetNamcoChannels(int Channels);

	void			SetSongInfo(const char *Name, const char *Artist, const char *Copyright);
	char			*GetSongName()		 { return m_strName; };
	char			*GetSongArtist()	 { return m_strArtist; };
	char			*GetSongCopyright()	 { return m_strCopyright; };
	void			SetSongName(char *pName);
	void			SetSongArtist(char *pArtist);
	void			SetSongCopyright(char *pCopyright);

	int				GetVibratoStyle() const;
	void			SetVibratoStyle(int Style);

	bool			GetLinearPitch() const;
	void			SetLinearPitch(bool Enable);

	void			SetComment(CString &comment);
	CString			GetComment() const;

	void			SetSpeedSplitPoint(int SplitPoint);
	int				GetSpeedSplitPoint() const;

	void			SetHighlight(int First, int Second);
	int				GetFirstHighlight() const;
	int				GetSecondHighlight() const;

	// Track management functions
	void			SelectTrack(unsigned int Track);
//	void			SelectTrackFast(unsigned int Track);	//	TODO: should be removed
	unsigned int	GetTrackCount() const;
	unsigned int	GetSelectedTrack() const;
	char			*GetTrackTitle(unsigned int Track) const;
	bool			AddTrack();
	void			RemoveTrack(unsigned int Track);
	void			SetTrackTitle(unsigned int Track, CString Title);
	void			MoveTrackUp(unsigned int Track);
	void			MoveTrackDown(unsigned int Track);


	// Instruments functions
	CInstrument		*GetInstrument(int Index);
	int				GetInstrumentCount() const;
	bool			IsInstrumentUsed(int Index) const;

	int				AddInstrument(const char *Name, int ChipType);				// Add a new instrument
	int				AddInstrument(CInstrument *pInst);
	void			RemoveInstrument(unsigned int Index);						// Remove an instrument
	void			SetInstrumentName(unsigned int Index, const char *Name);	// Set the name of an instrument
	void			GetInstrumentName(unsigned int Index, char *Name) const;	// Get the name of an instrument
	int				CloneInstrument(unsigned int Index);						// Create a copy of an instrument
	CInstrument		*CreateInstrument(int InstType);							// Creates a new instrument of InstType
	int				FindFreeInstrumentSlot();
	void			SaveInstrument(unsigned int Instrument, CString FileName);
	int 			LoadInstrument(CString FileName);
	int				GetInstrumentType(unsigned int Index) const;

	int				DeepCloneInstrument(unsigned int Index);

	// Read only getter for exporter plugins
	CInstrument2A03Interface const *Get2A03Instrument(int Instrument) const;


	// Sequences functions
	CSequence		*GetSequence(int Chip, int Index, int Type);
	CSequence		*GetSequence(int Index, int Type);
	int				GetSequenceItemCount(int Index, int Type) const;
	int				GetFreeSequence(int Type) const;
	int				GetSequenceCount(int Type) const;

	CSequence		*GetSequenceVRC6(int Index, int Type);
	CSequence		*GetSequenceVRC6(int Index, int Type) const;
	int				GetSequenceItemCountVRC6(int Index, int Type) const;
	int				GetFreeSequenceVRC6(int Type) const;

	CSequence		*GetSequenceN163(int Index, int Type);
	CSequence		*GetSequenceN163(int Index, int Type) const;
	int				GetSequenceItemCountN163(int Index, int Type) const;
	int				GetFreeSequenceN163(int Type) const;

	CSequence		*GetSequenceS5B(int Index, int Type);
	CSequence		*GetSequenceS5B(int Index, int Type) const;
	int				GetSequenceItemCountS5B(int Index, int Type) const;
	int				GetFreeSequenceS5B(int Type) const;

	// Read only getter for exporter plugins
	CSequenceInterface const *GetSequence(int Index, int Type) const;


	// DPCM samples
	CDSample		*GetDSample(unsigned int Index);
	int				GetSampleCount() const;
	int				GetFreeDSample() const;
	void			RemoveDSample(unsigned int Index);
	void			GetSampleName(unsigned int Index, char *Name) const;
	int				GetSampleSize(unsigned int Sample);
	char			GetSampleData(unsigned int Sample, unsigned int Offset);
	int				GetTotalSampleSize() const;

	// For file version compability
	void			ConvertSequence(stSequence *OldSequence, CSequence *NewSequence, int Type);

	int				ScanActualLength(int Track, int Count) const;

	// Constants
public:
	static const char*	DEFAULT_TRACK_NAME;
	static const int	DEFAULT_ROW_COUNT;
	static const char*	NEW_INST_NAME;

	static const int	DEFAULT_NAMCO_CHANS;

	static const int	DEFAULT_FIRST_HIGHLIGHT;
	static const int	DEFAULT_SECOND_HIGHLIGHT;

	//
	// Protected functions
	//
protected:

	//
	// File management functions (load/save)
	//

	void			CreateEmpty();

	BOOL			SaveDocument(LPCTSTR lpszPathName) const;
	BOOL			OpenDocument(LPCTSTR lpszPathName);

	BOOL			OpenDocumentOld(CFile *pOpenFile);
	BOOL			OpenDocumentNew(CDocumentFile &DocumentFile);

	bool			WriteBlocks(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Parameters(CDocumentFile *pDocFile) const;
	bool			WriteBlock_SongInfo(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Header(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Instruments(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Sequences(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Frames(CDocumentFile *pDocFile) const;
	bool			WriteBlock_Patterns(CDocumentFile *pDocFile) const;
	bool			WriteBlock_DSamples(CDocumentFile *pDocFile) const;
	bool			WriteBlock_SequencesVRC6(CDocumentFile *pDocFile) const;
	bool			WriteBlock_SequencesN163(CDocumentFile *pDocFile) const;
	bool			WriteBlock_SequencesS5B(CDocumentFile *pDocFile) const;

	bool			ReadBlock_Parameters(CDocumentFile *pDocFile);
	bool			ReadBlock_Header(CDocumentFile *pDocFile);
	bool			ReadBlock_Instruments(CDocumentFile *pDocFile);
	bool			ReadBlock_Sequences(CDocumentFile *pDocFile);
	bool			ReadBlock_Frames(CDocumentFile *pDocFile);
	bool			ReadBlock_Patterns(CDocumentFile *pDocFile);
	bool			ReadBlock_DSamples(CDocumentFile *pDocFile);
	bool			ReadBlock_SequencesVRC6(CDocumentFile *pDocFile);
	bool			ReadBlock_SequencesN163(CDocumentFile *pDocFile);
	bool			ReadBlock_SequencesS5B(CDocumentFile *pDocFile);

	void			SwitchToTrack(unsigned int Track);

	// For file version compability
	void			ReorderSequences();
	void			ConvertSequences();
/*
	void			SetupAutoSave();
	void			ClearAutoSave();
*/
	//
	// Module data
	//

	void			AllocateSong(unsigned int Song);

	void			SetupChannels(unsigned char Chip);
	void			ApplyExpansionChip();

	//
	// Private variables
	//
private:

	//
	// Interface variables
	//

	// Channels (TODO: remove or move these?)
	CTrackerChannel	*m_pChannels[CHANNELS];
	int				m_iRegisteredChannels;
	int				m_iChannelTypes[CHANNELS];
	int				m_iChannelChip[CHANNELS];


	//
	// State variables
	//

	bool			m_bFileLoaded;			// Is a file loaded?
	bool			m_bFileLoadFailed;		// Last file load operation failed
	unsigned int	m_iFileVersion;			// Loaded file version
	unsigned int	m_iTrack;				// Selected track

	bool			m_bForceBackup;
	bool			m_bBackupDone;

/*
	// Auto save
	int				m_iAutoSaveCounter;
	CString			m_sAutoSaveFile;
*/

	//
	// Document data
	//

	// Patterns and song data
	CPatternData	*m_pSelectedTune;							// Points to selecte tune
	CPatternData	*m_pTunes[MAX_TRACKS];						// List of all tunes
	CString			m_sTrackNames[MAX_TRACKS];

	unsigned int	m_iTracks;									// Track count
	unsigned int	m_iChannelsAvailable;						// Number of channels added

	// Instruments, samples and sequences
	CInstrument		*m_pInstruments[MAX_INSTRUMENTS];
	CDSample		m_DSamples[MAX_DSAMPLES];					// The DPCM sample list
	CSequence		*m_pSequences2A03[MAX_SEQUENCES][SEQ_COUNT];
	CSequence		*m_pSequencesVRC6[MAX_SEQUENCES][SEQ_COUNT];
	CSequence		*m_pSequencesN163[MAX_SEQUENCES][SEQ_COUNT];
	CSequence		*m_pSequencesS5B[MAX_SEQUENCES][SEQ_COUNT];

	// Module properties
	unsigned char	m_iExpansionChip;							// Expansion chip
	unsigned int	m_iNamcoChannels;
	int				m_iVibratoStyle;							// 0 = old style, 1 = new style
	bool			m_bLinearPitch;

	// NSF info
	char			m_strName[32];								// Song name
	char			m_strArtist[32];							// Song artist
	char			m_strCopyright[32];							// Song copyright

	unsigned int	m_iMachine;									// NTSC / PAL
	unsigned int	m_iEngineSpeed;								// Refresh rate
	unsigned int	m_iSpeedSplitPoint;							// Speed/tempo split-point

	CString			m_strComment;

	unsigned int	m_iFirstHighlight;
	unsigned int	m_iSecondHighlight;

	// Things below are for compability with older files
	stSequence		m_Sequences[MAX_SEQUENCES][SEQ_COUNT];		// Allocate one sequence-list for each effect
	stSequence		m_TmpSequences[MAX_SEQUENCES];

	//
	// End of document data
	//

	// Synchronization
	CCriticalSection m_csLoadedLock;

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void DeleteContents();
	virtual void SetModifiedFlag(BOOL bModified = 1);
//	virtual void Serialize(CArchive& ar);

   // MFC stuff from CDocument
   virtual void UpdateAllViews(void* ptr,long hint = 0) { emit updateViews(hint); }
   virtual CString GetTitle() { return m_docTitle; }

   // HACKS
   static CFamiTrackerDoc* _this;
   
// Implementation
public:
	virtual ~CFamiTrackerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnEditRemoveUnusedInstruments();
	afx_msg void OnEditRemoveUnusedPatterns();
	afx_msg void OnEditClearPatterns();
   
   // CP: Moved here because it makes sense...
   int  GetChannelColumns(int Channel) const;  

signals:
   void setModified(bool f);
   void updateViews(long hint);
};
