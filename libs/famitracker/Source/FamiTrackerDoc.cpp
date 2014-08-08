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

/*

 Document file version changes

 Ver 4.0
  - Header block, added song names

 Ver 3.0
  - Sequences are stored in the way they are represented in the instrument editor
  - Added separate speed and tempo settings
  - Changed automatic portamento to 3xx and added 1xx & 2xx portamento

 Ver 2.1
  - Made some additions to support multiple effect columns and prepared for more channels
  - Made some speed adjustments, increase speed effects by one if it's below 20

 Ver 2.0
  - Files are small

*/

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "TrackerChannel.h"
#include "MainFrm.h"
#include "DocumentFile.h"
#include "Settings.h"
#include "SoundGen.h"
#include "ChannelMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GET_PATTERN(Frame, Channel) m_pSelectedTrack->GetFramePattern(Frame, Channel)

// Defaults when creating new modules
const char* CFamiTrackerDoc::DEFAULT_TRACK_NAME = "New song";
const int	CFamiTrackerDoc::DEFAULT_ROW_COUNT	= 64;

const char* CFamiTrackerDoc::NEW_INST_NAME = "New instrument";

// Make 1 channel default since 8 sounds bad
const int	CFamiTrackerDoc::DEFAULT_NAMCO_CHANS = 1;

const int	CFamiTrackerDoc::DEFAULT_FIRST_HIGHLIGHT = 4;
const int	CFamiTrackerDoc::DEFAULT_SECOND_HIGHLIGHT = 16;

const bool	CFamiTrackerDoc::DEFAULT_LINEAR_PITCH = false;

// File I/O constants
const char *FILE_HEADER				= "FamiTracker Module";
const char *FILE_BLOCK_PARAMS		= "PARAMS";
const char *FILE_BLOCK_INFO			= "INFO";
const char *FILE_BLOCK_INSTRUMENTS	= "INSTRUMENTS";
const char *FILE_BLOCK_SEQUENCES	= "SEQUENCES";
const char *FILE_BLOCK_FRAMES		= "FRAMES";
const char *FILE_BLOCK_PATTERNS		= "PATTERNS";
const char *FILE_BLOCK_DSAMPLES		= "DPCM SAMPLES";
const char *FILE_BLOCK_HEADER		= "HEADER";
const char *FILE_BLOCK_COMMENTS		= "COMMENTS";

// VRC6
const char *FILE_BLOCK_SEQUENCES_VRC6 = "SEQUENCES_VRC6";

// N163
const char *FILE_BLOCK_SEQUENCES_N163 = "SEQUENCES_N163";

// Sunsoft
const char *FILE_BLOCK_SEQUENCES_S5B = "SEQUENCES_S5B";

// FTI instruments files
const char INST_HEADER[] = "FTI";
const char INST_VERSION[] = "2.4";

/* 
	Instrument version history
	 * 2.1 - Release points for sequences in 2A03 & VRC6
	 * 2.2 - FDS volume sequences goes from 0-31 instead of 0-15
	 * 2.3 - Support for release points & extra setting in sequences, 2A03 & VRC6
	 * 2.4 - DPCM delta counter setting
*/

// Structures for loading older versions of files

struct stInstrumentImport {
	char Name[256];
	bool Free;
	int	 ModEnable[SEQ_COUNT];
	int	 ModIndex[SEQ_COUNT];
	int	 AssignedSample;				// For DPCM
};

struct stSequenceImport {
	signed char Length[64];	// locked to 64
	signed char Value[64];
	int	Count;
};

struct stDSampleImport {
	char *SampleData;
	int	 SampleSize;
	char Name[256];
};

struct stChanNoteImport {
	int	Note;
	int	Octave;
	int	Vol;
	int	Instrument;
	int	ExtraStuff1;
	int	ExtraStuff2;
};

enum {
	FB_INSTRUMENTS,
	FB_SEQUENCES,
	FB_PATTERN_ROWS,
	FB_PATTERNS,
	FB_SPEED,
	FB_CHANNELS,
	FB_DSAMPLES,
	FB_EOF,
	FB_MACHINE,
	FB_ENGINESPEED,
	FB_SONGNAME,
	FB_SONGARTIST,
	FB_SONGCOPYRIGHT
};


#ifdef TRANSPOSE_FDS
bool bAdjustFDSArpeggio = false;
#endif

//
// CFamiTrackerDoc
//

IMPLEMENT_DYNCREATE(CFamiTrackerDoc, CDocument)

BEGIN_MESSAGE_MAP(CFamiTrackerDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
END_MESSAGE_MAP()

//
// Convert an instrument type to sound chip
//
static int GetChipFromInstrument(int Type)
{
	switch (Type) {
		case INST_2A03:
			return SNDCHIP_NONE;
		case INST_VRC6:
			return SNDCHIP_VRC6;
		case INST_VRC7:
			return SNDCHIP_VRC7;
		case INST_S5B:
			return SNDCHIP_S5B;
		case INST_FDS:
			return SNDCHIP_FDS;
		case INST_N163:
			return SNDCHIP_N163;
	}

	return SNDCHIP_NONE;
}

// CFamiTrackerDoc construction/destruction

CFamiTrackerDoc::CFamiTrackerDoc() : 
	m_iVersion(CLASS_VERSION), 
	m_bFileLoaded(false), 
	m_bFileLoadFailed(false), 
	m_iRegisteredChannels(0), 
	m_iNamcoChannels(DEFAULT_NAMCO_CHANS),
	m_bDisplayComment(false)
{
	// Initialize object

	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		m_DSamples[i].SampleSize = 0;
		m_DSamples[i].SampleData = NULL;
	}

	// Clear pointer arrays
	memset(m_pTracks, 0, sizeof(CPatternData*) * MAX_TRACKS);
	memset(m_pInstruments, 0, sizeof(CInstrument*) * MAX_INSTRUMENTS);
	memset(m_pSequences2A03, 0, sizeof(CSequence*) * MAX_SEQUENCES * SEQ_COUNT);
	memset(m_pSequencesVRC6, 0, sizeof(CSequence*) * MAX_SEQUENCES * SEQ_COUNT);
	memset(m_pSequencesN163, 0, sizeof(CSequence*) * MAX_SEQUENCES * SEQ_COUNT);
	memset(m_pSequencesS5B, 0, sizeof(CSequence*) * MAX_SEQUENCES * SEQ_COUNT);

	// Register this object to the sound generator
	CSoundGen *pSoundGen = theApp.GetSoundGenerator();

	if (pSoundGen)
		pSoundGen->AssignDocument(this);
}

CFamiTrackerDoc::~CFamiTrackerDoc()
{
	// Clean up

	// DPCM samples
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		m_DSamples[i].Clear();
	}

	// Patterns
	for (int i = 0; i < MAX_TRACKS; ++i) {
		SAFE_RELEASE(m_pTracks[i]);
	}

	// Instruments
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (m_pInstruments[i] != NULL) {
			m_pInstruments[i]->Release();
			m_pInstruments[i] = NULL;
		}
	}

	// Sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			SAFE_RELEASE(m_pSequences2A03[i][j]);
			SAFE_RELEASE(m_pSequencesVRC6[i][j]);
			SAFE_RELEASE(m_pSequencesN163[i][j]);
			SAFE_RELEASE(m_pSequencesS5B[i][j]);
		}
	}
}

//
// Static functions
//

CFamiTrackerDoc *CFamiTrackerDoc::GetDoc()
{
	CFrameWnd *pFrame = static_cast<CFrameWnd*>(AfxGetApp()->m_pMainWnd);
	ASSERT_VALID(pFrame);

	return static_cast<CFamiTrackerDoc*>(pFrame->GetActiveDocument());
}

// Synchronization
BOOL CFamiTrackerDoc::LockDocument() const
{
	return m_csDocumentLock.Lock();
}

BOOL CFamiTrackerDoc::LockDocument(DWORD dwTimeout) const
{
	return m_csDocumentLock.Lock(dwTimeout);
}

BOOL CFamiTrackerDoc::UnlockDocument() const
{
	return m_csDocumentLock.Unlock();
}

//
// Overrides
//

BOOL CFamiTrackerDoc::OnNewDocument()
{
	// Called by the GUI to create a new file

	// This calls DeleteContents
	if (!CDocument::OnNewDocument())
		return FALSE;

	CreateEmpty();

	return TRUE;
}

BOOL CFamiTrackerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// This function is called by the GUI to load a file

	//DeleteContents();

	m_csDocumentLock.Lock();

	// Load file
	if (!OpenDocument(lpszPathName)) {
		// Loading failed, create empty document
		//CreateEmpty();
		// and tell doctemplate that loading failed
		m_csDocumentLock.Unlock();
		return FALSE;
	}

	m_csDocumentLock.Unlock();

	// Update main frame
	ApplyExpansionChip();

//	SetupAutoSave();

	// Remove modified flag
	SetModifiedFlag(FALSE);

	return TRUE;
}

BOOL CFamiTrackerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// This function is called by the GUI to save the file

	if (!m_bFileLoaded)
		return FALSE;

	// File backup, now performed on save instead of open
	if ((m_bForceBackup || theApp.GetSettings()->General.bBackups) && !m_bBackupDone) {
		CString BakName;
		BakName.Format(_T("%s.bak"), lpszPathName);
		CopyFile(lpszPathName, BakName.GetBuffer(), FALSE);
		m_bBackupDone = true;
	}

	if (!SaveDocument(lpszPathName))
		return FALSE;

	// Reset modified flag
	SetModifiedFlag(FALSE);

	return TRUE;
}

void CFamiTrackerDoc::OnCloseDocument()
{	
	// Document object is about to be deleted

	// Remove itself from sound generator
	CSoundGen *pSoundGen = theApp.GetSoundGenerator();

	if (pSoundGen)
		pSoundGen->RemoveDocument();

	CDocument::OnCloseDocument();
}

void CFamiTrackerDoc::DeleteContents()
{
	// Current document is being unloaded, clear and reset variables and memory
	// Delete everything because the current object is being reused in SDI

	// Make sure player is stopped
	theApp.StopPlayerAndWait();

	m_csDocumentLock.Lock();

	// Mark file as unloaded
	m_bFileLoaded = false;
	m_bForceBackup = false;
	m_bBackupDone = true;	// No backup on new modules

	UpdateAllViews(NULL, CLOSE_DOCUMENT);

	// DPCM samples
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		m_DSamples[i].Clear();
	}

	// Instruments
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (m_pInstruments[i] != NULL) {
			m_pInstruments[i]->Release();
			m_pInstruments[i] = NULL;
		}
	}

	// Clear sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		m_TmpSequences[i].Count = 0;
		for (int j = 0; j < SEQ_COUNT; ++j) {
			SAFE_RELEASE(m_pSequences2A03[i][j]);
			SAFE_RELEASE(m_pSequencesVRC6[i][j]);
			SAFE_RELEASE(m_pSequencesN163[i][j]);
			SAFE_RELEASE(m_pSequencesS5B[i][j]);
		}
	}

	// Clear tracks
	m_iTracks = 1;
	m_iTrack = 0;

	// Delete all patterns
	for (int i = 0; i < MAX_TRACKS; ++i) {
		SAFE_RELEASE(m_pTracks[i]);
		m_sTrackNames[i].Empty();
	}

	m_pSelectedTrack = NULL;

	// Clear song info
	memset(m_strName, 0, 32);
	memset(m_strArtist, 0, 32);
	memset(m_strCopyright, 0, 32);

	// Reset variables to default
	m_iMachine			 = DEFAULT_MACHINE_TYPE;
	m_iEngineSpeed		 = 0;
	m_iExpansionChip	 = SNDCHIP_NONE;
	m_iVibratoStyle		 = VIBRATO_OLD;
	m_bLinearPitch		 = DEFAULT_LINEAR_PITCH;
	m_iChannelsAvailable = CHANNELS_DEFAULT;
	m_iSpeedSplitPoint	 = DEFAULT_SPEED_SPLIT_POINT;

	m_iFirstHighlight  = DEFAULT_FIRST_HIGHLIGHT;
	m_iSecondHighlight = DEFAULT_SECOND_HIGHLIGHT;

	// Used for loading older files
	memset(m_Sequences, 0, sizeof(stSequence) * MAX_SEQUENCES * SEQ_COUNT);

	// Auto save
//	ClearAutoSave();

	m_strComment.Empty();
	m_bDisplayComment = false;

	// Remove modified flag
	SetModifiedFlag(FALSE);

	m_csDocumentLock.Unlock();

	CDocument::DeleteContents();
}

void CFamiTrackerDoc::SetModifiedFlag(BOOL bModified)
{
	// Trigger auto-save in 10 seconds
	/*
	if (bModified)
		m_iAutoSaveCounter = 10;
		*/

	BOOL bWasModified = IsModified();
	CDocument::SetModifiedFlag(bModified);
	
	CFrameWnd *pFrameWnd = dynamic_cast<CFrameWnd*>(theApp.m_pMainWnd);
	if (pFrameWnd != NULL) {
		if (pFrameWnd->GetActiveDocument() == this && bWasModified != bModified) {
			pFrameWnd->OnUpdateFrameTitle(TRUE);
		}
	}
}

void CFamiTrackerDoc::CreateEmpty()
{
	m_csDocumentLock.Lock();

	// Allocate first song
	SwitchToTrack(0);

	// Auto-select new style vibrato for new modules
	m_iVibratoStyle = VIBRATO_NEW;
	m_bLinearPitch = DEFAULT_LINEAR_PITCH;

	m_iNamcoChannels = DEFAULT_NAMCO_CHANS;

	// and select 2A03 only
	SelectExpansionChip(SNDCHIP_NONE);

//	SetupAutoSave();

	SetModifiedFlag(FALSE);

	// Document is avaliable
	m_bFileLoaded = true;

	m_csDocumentLock.Unlock();

	theApp.GetSoundGenerator()->DocumentPropertiesChange(this);
}

//
// Messages
//

void CFamiTrackerDoc::OnFileSave()
{
	if (GetPathName().GetLength() == 0)
		OnFileSaveAs();
	else
		CDocument::OnFileSave();
}

void CFamiTrackerDoc::OnFileSaveAs()
{
	// Overloaded in order to save the ftm-path
	CString newName = GetPathName();

	if (!AfxGetApp()->DoPromptFileName(newName, AFX_IDS_SAVEFILE, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, GetDocTemplate()))
		return;

	theApp.GetSettings()->SetPath(newName, PATH_FTM);
	
	DoSave(newName);
}

// CFamiTrackerDoc serialization (never used)

void CFamiTrackerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CFamiTrackerDoc diagnostics

#ifdef _DEBUG
void CFamiTrackerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFamiTrackerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFamiTrackerDoc commands

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File load / save routines
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions for compability with older file versions

void CFamiTrackerDoc::ReorderSequences()
{
	int Keepers[SEQ_COUNT] = {0, 0, 0, 0, 0};
	int Indices[MAX_SEQUENCES][SEQ_COUNT];
	int Index;

	memset(Indices, 0xFF, MAX_SEQUENCES * SEQ_COUNT * sizeof(int));

	// Organize sequences
	for (int i = 0; i < MAX_INSTRUMENTS; i++) {
		if (m_pInstruments[i] != NULL) {
			CInstrument2A03 *pInst = dynamic_cast<CInstrument2A03*>(m_pInstruments[i]);
			if (pInst != NULL) {
				for (int x = 0; x < SEQ_COUNT; x++) {
					if (pInst->GetSeqEnable(x)) {
						Index = pInst->GetSeqIndex(x);
						if (Indices[Index][x] >= 0 && Indices[Index][x] != -1) {
							pInst->SetSeqIndex(x, Indices[Index][x]);
						}
						else {
							memcpy(&m_Sequences[Keepers[x]][x], &m_TmpSequences[Index], sizeof(stSequence));
							for (unsigned int j = 0; j < m_Sequences[Keepers[x]][x].Count; j++) {
								switch (x) {
									case SEQ_VOLUME: LIMIT(m_Sequences[Keepers[x]][x].Value[j], 15, 0); break;
									case SEQ_DUTYCYCLE: LIMIT(m_Sequences[Keepers[x]][x].Value[j], 3, 0); break;
								}
							}
							Indices[Index][x] = Keepers[x];
							pInst->SetSeqIndex(x, Keepers[x]);
							Keepers[x]++;
						}
					}
					else
						pInst->SetSeqIndex(x, 0);
				}
			}
		}
	}
}

void CFamiTrackerDoc::ConvertSequences()
{
	int i, j, k;
	int iLength, ValPtr, Count, Value, Length;
	stSequence	*pSeq;
	CSequence	*pNewSeq;

	// This function is used to convert the old type sequences to new type

	for (i = 0; i < MAX_SEQUENCES; ++i) {
		for (j = 0; j < /*MAX_SEQUENCE_ITEMS*/ SEQ_COUNT; ++j) {
			pSeq = &m_Sequences[i][j];
			if (pSeq->Count > 0 && pSeq->Count < MAX_SEQUENCE_ITEMS) {
				pNewSeq = GetSequence(i, j);
				
				// Save a pointer to this
				int iLoopPoint = -1;
				iLength = 0;
				ValPtr = 0;

				// Store the sequence
				Count = pSeq->Count;
				for (k = 0; k < Count; k++) {
					Value	= pSeq->Value[k];
					Length	= pSeq->Length[k];

					if (Length < 0) {
						iLoopPoint = 0;
						for (int l = signed(pSeq->Count) + Length - 1; l < signed(pSeq->Count) - 1; l++)
							iLoopPoint += (pSeq->Length[l] + 1);
					}
					else {
						for (int l = 0; l < Length + 1 && ValPtr < MAX_SEQUENCE_ITEMS; l++) {
							if ((j == SEQ_PITCH || j == SEQ_HIPITCH) && l > 0)
								pNewSeq->SetItem(ValPtr++, 0);
							else
								pNewSeq->SetItem(ValPtr++, (unsigned char)Value);
							iLength++;
						}
					}
				}

				if (iLoopPoint != -1) {
					if (iLoopPoint > iLength)
						iLoopPoint = iLength;
					iLoopPoint = iLength - iLoopPoint;
				}

				pNewSeq->SetItemCount(ValPtr);
				pNewSeq->SetLoopPoint(iLoopPoint);
			}
		}
	}
}

/*** File format description ***

0000: "FamiTracker Module"					id string
000x: Version								int, version number
000x: Start of blocks

 {FILE_BLOCK_PARAMS, 2}
  Expansion chip							char
  Channels									int
  Machine type								int
  Engine speed								int

 {FILE_BLOCK_INFO, 1}
  Song name									string, 32 bytes
  Artist name								string, 32 bytes
  Copyright									string, 32 bytes

000x: End of blocks
000x: "END"						End of file

********************************/


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Document store functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CFamiTrackerDoc::SaveDocument(LPCTSTR lpszPathName) const
{
	CDocumentFile DocumentFile;
	CFileException ex;
	TCHAR TempPath[MAX_PATH], TempFile[MAX_PATH];
	ULONGLONG FileSize;

	if (GetExpansionChip() & SNDCHIP_S5B) {
		AfxMessageBox(_T("Saving Sunsoft modules is not yet supported"));
		return FALSE;
	}

	// First write to a temp file (if saving fails, the original is not destroyed)
	GetTempPath(MAX_PATH, TempPath);
	GetTempFileName(TempPath, _T("FTM"), 0, TempFile);

	if (!DocumentFile.Open(TempFile, CFile::modeWrite | CFile::modeCreate, &ex)) {
		// Could not open file
		TCHAR szCause[255];
		CString strFormatted;
		ex.GetErrorMessage(szCause, 255);
		strFormatted.LoadString(IDS_SAVE_ERROR_REASON);
		strFormatted += szCause;
		AfxMessageBox(strFormatted, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	DocumentFile.BeginDocument();

	if (!WriteBlocks(&DocumentFile)) {
		// The save process failed, delete temp file
		DocumentFile.Close();
		DeleteFile(TempFile);
		// Display error
		CString	ErrorMsg;
		ErrorMsg.LoadString(IDS_SAVE_ERROR);
		AfxMessageBox(ErrorMsg, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	DocumentFile.EndDocument();

	FileSize = DocumentFile.GetLength();

	DocumentFile.Close();

	// Save old creation date
	HANDLE hOldFile;
	FILETIME creationTime;

//	hOldFile = CreateFile(lpszPathName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	GetFileTime(hOldFile, &creationTime, NULL, NULL);
//	CloseHandle(hOldFile);

	// Everything is done and the program cannot crash at this point
	// Replace the original
	if (!MoveFileEx(TempFile, lpszPathName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
		// Display message if saving failed
		CString	ErrorMsg;
//		TCHAR	*lpMsgBuf;
		ErrorMsg.LoadString(IDS_SAVE_ERROR_REASON);
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
//		ErrorMsg.Append(lpMsgBuf);
		AfxMessageBox(ErrorMsg, MB_OK | MB_ICONERROR);
//		LocalFree(lpMsgBuf);
		// Remove temp file
		DeleteFile(TempFile);
		return FALSE;
	}

	// Restore creation date
//	hOldFile = CreateFile(lpszPathName, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	SetFileTime(hOldFile, &creationTime, NULL, NULL);
//	CloseHandle(hOldFile);

	// Todo: avoid calling the main window from document class
	CMainFrame *pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (pMainFrame != NULL) {
		CString text;
		text.Format(_T("File saved (%i bytes)"), FileSize);
		pMainFrame->SetMessageText(text);
	}

	return TRUE;
}

bool CFamiTrackerDoc::WriteBlocks(CDocumentFile *pDocFile) const
{
	if (!WriteBlock_Parameters(pDocFile))
		return false;
	if (!WriteBlock_SongInfo(pDocFile))
		return false;
	if (!WriteBlock_Header(pDocFile))
		return false;
	if (!WriteBlock_Instruments(pDocFile))
		return false;
	if (!WriteBlock_Sequences(pDocFile))
		return false;
	if (!WriteBlock_Frames(pDocFile))
		return false;
	if (!WriteBlock_Patterns(pDocFile))
		return false;
	if (!WriteBlock_DSamples(pDocFile))
		return false;
	if (!WriteBlock_Comments(pDocFile))
		return false;

	if (m_iExpansionChip & SNDCHIP_VRC6) {
		if (!WriteBlock_SequencesVRC6(pDocFile))
			return false;
	}

	if (m_iExpansionChip & SNDCHIP_N163) {
		if (!WriteBlock_SequencesN163(pDocFile))
			return false;
	}
#if 0
	if (m_iExpansionChip & SNDCHIP_S5B) {
		if (!WriteBlock_SequencesS5B(pDocFile))
			return false;
	}
#endif
	return true;
}

bool CFamiTrackerDoc::WriteBlock_Parameters(CDocumentFile *pDocFile) const
{
	// Module parameters
	pDocFile->CreateBlock(FILE_BLOCK_PARAMS, 6);
	
	pDocFile->WriteBlockChar(m_iExpansionChip);		// ver 2 change
	pDocFile->WriteBlockInt(m_iChannelsAvailable);
	pDocFile->WriteBlockInt(m_iMachine);
	pDocFile->WriteBlockInt(m_iEngineSpeed);
	pDocFile->WriteBlockInt(m_iVibratoStyle);		// ver 3 change
	// TODO write m_bLinearPitch
	pDocFile->WriteBlockInt(m_iFirstHighlight);		// ver 4 change
	pDocFile->WriteBlockInt(m_iSecondHighlight);

	if (GetExpansionChip() & SNDCHIP_N163)
		pDocFile->WriteBlockInt(m_iNamcoChannels);	// ver 5 change

	pDocFile->WriteBlockInt(m_iSpeedSplitPoint);	// ver 6 change

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_SongInfo(CDocumentFile *pDocFile) const
{
	// Song info
	pDocFile->CreateBlock(FILE_BLOCK_INFO, 1);
	
	pDocFile->WriteBlock(m_strName, 32);
	pDocFile->WriteBlock(m_strArtist, 32);
	pDocFile->WriteBlock(m_strCopyright, 32);

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Header(CDocumentFile *pDocFile) const
{
	/* 
	 *  Header data
 	 *
	 *  Store song count and then for each channel: 
	 *  channel type and number of effect columns
	 *
	 */

	// Version 3 adds song names

	// Header data
	pDocFile->CreateBlock(FILE_BLOCK_HEADER, 3);

	// Write number of tracks
	pDocFile->WriteBlockChar(m_iTracks - 1);

	// Ver 3, store track names
	for (unsigned int i = 0; i < m_iTracks; ++i) {
		pDocFile->WriteString(m_sTrackNames[i]);
	}

	for (unsigned int i = 0; i < m_iChannelsAvailable; ++i) {
		// Channel type
		pDocFile->WriteBlockChar(m_iChannelTypes[i]);	
		for (unsigned int j = 0; j < m_iTracks; ++j) {
			ASSERT(m_pTracks[j] != NULL);
//			AllocateSong(j);
			// Effect columns
			pDocFile->WriteBlockChar(m_pTracks[j]->GetEffectColumnCount(i));
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Instruments(CDocumentFile *pDocFile) const
{
	// A bug in v0.3.0 causes a crash if this is not 2, so change only when that ver is obsolete!
	//
	// Log:
	// - v6: adds DPCM delta settings
	//
	const int BLOCK_VERSION = 6;

	// If FDS is used then version must be at least 4 or recent files won't load
	int Version = BLOCK_VERSION;

	// Fix for FDS instruments
/*	if (m_iExpansionChip & SNDCHIP_FDS)
		Version = 4;

	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (m_pInstruments[i] != NULL) {
			if (m_pInstruments[i]->GetType() == INST_FDS)
				Version = 4;
		}
	}
*/
	int Count = 0;
	char Name[CInstrument::INST_NAME_MAX];
	char Type;

	// Instruments block
	pDocFile->CreateBlock(FILE_BLOCK_INSTRUMENTS, Version);

	// Count number of instruments
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (m_pInstruments[i] != NULL)
			Count++;
	}

	pDocFile->WriteBlockInt(Count);

	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		// Only write instrument if it's used
		if (m_pInstruments[i] != NULL) {

			Type = m_pInstruments[i]->GetType();

			// Write index and type
			pDocFile->WriteBlockInt(i);
			pDocFile->WriteBlockChar(Type);

			// Store the instrument
			m_pInstruments[i]->Store(pDocFile);

			// Store the name
			m_pInstruments[i]->GetName(Name);
			pDocFile->WriteBlockInt((int)strlen(Name));
			pDocFile->WriteBlock(Name, (int)strlen(Name));			
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Sequences(CDocumentFile *pDocFile) const
{
	/* 
	 * Store 2A03 sequences
	 */ 

	// Sequences, version 6
	pDocFile->CreateBlock(FILE_BLOCK_SEQUENCES, 6);

	int Count = 0;

	// Count number of used sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			if (GetSequenceItemCount(i, j) > 0)
				Count++;
		}
	}

	pDocFile->WriteBlockInt(Count);

	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCount(i, j);
			if (Count > 0) {
				const CSequence *pSeq = reinterpret_cast<const CSequence*>(GetSequence(i, j));
				// Store index
				pDocFile->WriteBlockInt(i);
				// Store type of sequence
				pDocFile->WriteBlockInt(j);
				// Store number of items in this sequence
				pDocFile->WriteBlockChar(Count);
				// Store loop point
				pDocFile->WriteBlockInt(pSeq->GetLoopPoint());
				// Store items
				for (int k = 0; k < Count; ++k) {
					pDocFile->WriteBlockChar(pSeq->GetItem(k));
				}
			}
		}
	}

	// v6
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCount(i, j);
			if (Count > 0) {
				const CSequence *pSeq = reinterpret_cast<const CSequence*>(GetSequence(i, j));
				// Store release point
				pDocFile->WriteBlockInt(pSeq->GetReleasePoint());
				// Store setting
				pDocFile->WriteBlockInt(pSeq->GetSetting());
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_SequencesVRC6(CDocumentFile *pDocFile) const
{
	/* 
	 * Store VRC6 sequences
	 */ 

	// Sequences, version 6
	pDocFile->CreateBlock(FILE_BLOCK_SEQUENCES_VRC6, 6);

	int Count = 0;

	// Count number of used sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			if (GetSequenceItemCountVRC6(i, j) > 0)
				Count++;
		}
	}

	// Write it
	pDocFile->WriteBlockInt(Count);

	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCountVRC6(i, j);
			if (Count > 0) {
				const CSequence *pSeq = GetSequenceVRC6(i, j);
				// Store index
				pDocFile->WriteBlockInt(i);
				// Store type of sequence
				pDocFile->WriteBlockInt(j);
				// Store number of items in this sequence
				pDocFile->WriteBlockChar(Count);
				// Store loop point
				pDocFile->WriteBlockInt(pSeq->GetLoopPoint());
				// Store items
				for (int k = 0; k < Count; ++k) {
					pDocFile->WriteBlockChar(pSeq->GetItem(k));
				}
			}
		}
	}

	// v6
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCountVRC6(i, j);
			if (Count > 0) {
				const CSequence *pSeq = GetSequenceVRC6(i, j);
				// Store release point
				pDocFile->WriteBlockInt(pSeq->GetReleasePoint());
				// Store setting
				pDocFile->WriteBlockInt(pSeq->GetSetting());
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_SequencesN163(CDocumentFile *pDocFile) const
{
	/* 
	 * Store N163 sequences
	 */ 

	// Sequences, version 0
	pDocFile->CreateBlock(FILE_BLOCK_SEQUENCES_N163, 1);

	int Count = 0;

	// Count number of used sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			if (GetSequenceItemCountN163(i, j) > 0)
				Count++;
		}
	}

	// Write it
	pDocFile->WriteBlockInt(Count);

	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCountN163(i, j);
			if (Count > 0) {
				const CSequence *pSeq = GetSequenceN163(i, j);
				// Store index
				pDocFile->WriteBlockInt(i);
				// Store type of sequence
				pDocFile->WriteBlockInt(j);
				// Store number of items in this sequence
				pDocFile->WriteBlockChar(Count);
				// Store loop point
				pDocFile->WriteBlockInt(pSeq->GetLoopPoint());
				// Store release point
				pDocFile->WriteBlockInt(pSeq->GetReleasePoint());
				// Store setting
				pDocFile->WriteBlockInt(pSeq->GetSetting());
				// Store items
				for (int k = 0; k < Count; ++k) {
					pDocFile->WriteBlockChar(pSeq->GetItem(k));
				}
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_SequencesS5B(CDocumentFile *pDocFile) const
{
	/* 
	 * Store N163 sequences
	 */ 

	// Sequences, version 0
	pDocFile->CreateBlock(FILE_BLOCK_SEQUENCES_S5B, 1);

	int Count = 0;

	// Count number of used sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			if (GetSequenceItemCountS5B(i, j) > 0)
				Count++;
		}
	}

	// Write it
	pDocFile->WriteBlockInt(Count);

	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			Count = GetSequenceItemCountS5B(i, j);
			if (Count > 0) {
				const CSequence *pSeq = GetSequenceS5B(i, j);
				// Store index
				pDocFile->WriteBlockInt(i);
				// Store type of sequence
				pDocFile->WriteBlockInt(j);
				// Store number of items in this sequence
				pDocFile->WriteBlockChar(Count);
				// Store loop point
				pDocFile->WriteBlockInt(pSeq->GetLoopPoint());
				// Store release point
				pDocFile->WriteBlockInt(pSeq->GetReleasePoint());
				// Store setting
				pDocFile->WriteBlockInt(pSeq->GetSetting());
				// Store items
				for (int k = 0; k < Count; ++k) {
					pDocFile->WriteBlockChar(pSeq->GetItem(k));
				}
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Frames(CDocumentFile *pDocFile) const
{
	/* Store frame count
	 *
	 * 1. Number of channels (5 for 2A03 only)
	 * 2. 
	 * 
	 */ 

	pDocFile->CreateBlock(FILE_BLOCK_FRAMES, 3);

	for (unsigned i = 0; i < m_iTracks; ++i) {
		CPatternData *pTune = m_pTracks[i];

		pDocFile->WriteBlockInt(pTune->GetFrameCount());
		pDocFile->WriteBlockInt(pTune->GetSongSpeed());
		pDocFile->WriteBlockInt(pTune->GetSongTempo());
		pDocFile->WriteBlockInt(pTune->GetPatternLength());

		for (unsigned int j = 0; j < pTune->GetFrameCount(); ++j) {
			for (unsigned k = 0; k < m_iChannelsAvailable; ++k) {
				pDocFile->WriteBlockChar((unsigned char)pTune->GetFramePattern(j, k));
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Patterns(CDocumentFile *pDocFile) const
{
	/*
	 * Version changes: 
	 *
	 *  2: Support multiple tracks
	 *  3: Changed portamento effect
	 *  4: Switched portamento effects for VRC7 (1xx & 2xx), adjusted Pxx for FDS
	 *  5: Adjusted FDS octave
	 *  (6: Noise pitch slide effects fix)
	 *
	 */ 

#ifdef TRANSPOSE_FDS
	pDocFile->CreateBlock(FILE_BLOCK_PATTERNS, 5);
#else
	pDocFile->CreateBlock(FILE_BLOCK_PATTERNS, 4);
#endif

	for (unsigned t = 0; t < m_iTracks; ++t) {
		for (unsigned i = 0; i < m_iChannelsAvailable; ++i) {
			for (unsigned x = 0; x < MAX_PATTERN; ++x) {
				unsigned Items = 0;

				// Save all rows
				unsigned int PatternLen = MAX_PATTERN_LENGTH;
				//unsigned int PatternLen = m_pTracks[t]->GetPatternLength();
				
				// Get the number of items in this pattern
				for (unsigned y = 0; y < PatternLen; ++y) {
					if (!m_pTracks[t]->IsCellFree(i, x, y))
						Items++;
				}

				if (Items > 0) {
					pDocFile->WriteBlockInt(t);		// Write track
					pDocFile->WriteBlockInt(i);		// Write channel
					pDocFile->WriteBlockInt(x);		// Write pattern
					pDocFile->WriteBlockInt(Items);	// Number of items

					for (unsigned y = 0; y < PatternLen; y++) {
						if (!m_pTracks[t]->IsCellFree(i, x, y)) {
							pDocFile->WriteBlockInt(y);

							pDocFile->WriteBlockChar(m_pTracks[t]->GetNote(i, x, y));
							pDocFile->WriteBlockChar(m_pTracks[t]->GetOctave(i, x, y));
							pDocFile->WriteBlockChar(m_pTracks[t]->GetInstrument(i, x, y));
							pDocFile->WriteBlockChar(m_pTracks[t]->GetVolume(i, x, y));

							int EffColumns = (m_pTracks[t]->GetEffectColumnCount(i) + 1);

							for (int n = 0; n < EffColumns; n++) {
								pDocFile->WriteBlockChar(m_pTracks[t]->GetEffect(i, x, y, n));
								pDocFile->WriteBlockChar(m_pTracks[t]->GetEffectParam(i, x, y, n));
							}
						}
					}
				}
			}
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_DSamples(CDocumentFile *pDocFile) const
{
	int Count = 0;

	pDocFile->CreateBlock(FILE_BLOCK_DSAMPLES, 1);

	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		if (m_DSamples[i].SampleSize > 0)
			Count++;
	}

	// Write sample count
	pDocFile->WriteBlockChar(Count);

	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		if (m_DSamples[i].SampleSize > 0) {
			// Write sample
			pDocFile->WriteBlockChar(i);
			pDocFile->WriteBlockInt((int)strlen(m_DSamples[i].Name));
			pDocFile->WriteBlock(m_DSamples[i].Name, (int)strlen(m_DSamples[i].Name));
			pDocFile->WriteBlockInt(m_DSamples[i].SampleSize);
			pDocFile->WriteBlock(m_DSamples[i].SampleData, m_DSamples[i].SampleSize);
		}
	}

	return pDocFile->FlushBlock();
}

bool CFamiTrackerDoc::WriteBlock_Comments(CDocumentFile *pDocFile) const
{
	if (m_strComment.GetLength() == 0)
		return true;

	pDocFile->CreateBlock(FILE_BLOCK_COMMENTS, 1);
	pDocFile->WriteBlockInt(m_bDisplayComment ? 1 : 0);
	pDocFile->WriteString(m_strComment);
	return pDocFile->FlushBlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Document load functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CFamiTrackerDoc::OpenDocument(LPCTSTR lpszPathName)
{
	CFileException ex;
	CDocumentFile  OpenFile;
	unsigned int   iVersion;
//	bool		   bForceBackup = false;

	m_bBackupDone = false;
	m_bFileLoadFailed = true;

	// Open file
	if (!OpenFile.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &ex)) {
		TCHAR   szCause[255];
		CString strFormatted;
		ex.GetErrorMessage(szCause, 255);
		strFormatted = _T("Could not open file.\n\n");
		strFormatted += szCause;
		AfxMessageBox(strFormatted);
		//OnNewDocument();
		return FALSE;
	}

	// Check if empty file
	if (OpenFile.GetLength() == 0) {
		// Setup default settings
		CreateEmpty();
		return TRUE;
	}

	// Read header ID and version
	if (!OpenFile.CheckValidity()) {
		AfxMessageBox(IDS_FILE_VALID_ERROR, MB_ICONERROR);
		return FALSE;
	}

	iVersion = OpenFile.GetFileVersion();

	if (iVersion < 0x0200) {
		// Older file version
		if (iVersion < CDocumentFile::COMPATIBLE_VER) {
			AfxMessageBox(IDS_FILE_VERSION_ERROR, MB_ICONERROR);
			return FALSE;
		}

		if (!OpenDocumentOld(&OpenFile))
			return FALSE;

		// Create a backup of this file, since it's an old version 
		// and something might go wrong when converting
		//bForceBackup = true;
		m_bForceBackup = true;

		// Auto-select old style vibrato for old files
		m_iVibratoStyle = VIBRATO_OLD;
		m_bLinearPitch = false;
	}
	else if (iVersion >= 0x0200) {
		// New file version

		// Try to open file, create new if it fails
		if (!OpenDocumentNew(OpenFile))
			return FALSE;

		// Backup if files was of an older version
		//bForceBackup = m_iFileVersion < CDocumentFile::FILE_VER;
		m_bForceBackup = m_iFileVersion < CDocumentFile::FILE_VER;
	}

#ifdef WIP
	// Force backups if compiled as beta
//	bForceBackup = true;
//	m_bForceBackup = true;
#endif

	// File is loaded
	m_bFileLoaded = true;
	m_bFileLoadFailed = false;

	theApp.GetSoundGenerator()->DocumentPropertiesChange(this);

	return TRUE;
}

/**
 * This function reads the old obsolete file version. 
 */
BOOL CFamiTrackerDoc::OpenDocumentOld(CFile *pOpenFile)
{
	unsigned int i, c, ReadCount, FileBlock;

	// Delete loaded document
	DeleteContents();

	FileBlock = 0;

	// Only single track files
	SwitchToTrack(0);

	m_iVibratoStyle = VIBRATO_OLD;
	m_bLinearPitch = false;

	stInstrumentImport	ImportedInstruments;
	stSequenceImport	ImportedSequence;
	stDSampleImport		ImportedDSample;
	stChanNoteImport	ImportedNote;

	while (FileBlock != FB_EOF) {
		if (pOpenFile->Read(&FileBlock, sizeof(int)) == 0)
			FileBlock = FB_EOF;

		unsigned int Speed, FrameCount, Pattern, PatternLength;

		switch (FileBlock) {
			case FB_CHANNELS:
				pOpenFile->Read(&m_iChannelsAvailable, sizeof(int));
				break;

			case FB_SPEED:
				pOpenFile->Read(&Speed, sizeof(int));
				m_pSelectedTrack->SetSongSpeed(Speed + 1);
				break;

			case FB_MACHINE:
				pOpenFile->Read(&m_iMachine, sizeof(int));				
				break;

			case FB_ENGINESPEED:
				pOpenFile->Read(&m_iEngineSpeed, sizeof(int));
				break;

			case FB_INSTRUMENTS:
				pOpenFile->Read(&ReadCount, sizeof(int));
				if (ReadCount > MAX_INSTRUMENTS)
					ReadCount = MAX_INSTRUMENTS - 1;
				for (i = 0; i < ReadCount; i++) {
					pOpenFile->Read(&ImportedInstruments, sizeof(stInstrumentImport));
					if (ImportedInstruments.Free == false) {
						CInstrument2A03 *pInst = new CInstrument2A03();
						for (int j = 0; j < SEQ_COUNT; j++) {
							pInst->SetSeqEnable(j, ImportedInstruments.ModEnable[j]);
							pInst->SetSeqIndex(j, ImportedInstruments.ModIndex[j]);
						}
						pInst->SetName(ImportedInstruments.Name);

						if (ImportedInstruments.AssignedSample > 0) {
							int Pitch = 0;
							for (int y = 0; y < 6; y++) {
								for (int x = 0; x < 12; x++) {
									pInst->SetSample(y, x, ImportedInstruments.AssignedSample);
									pInst->SetSamplePitch(y, x, Pitch);
									Pitch = (Pitch + 1) % 16;
								}
							}
						}

						m_pInstruments[i] = pInst;
					}
				}
				break;

			case FB_SEQUENCES:
				pOpenFile->Read(&ReadCount, sizeof(int));
				for (i = 0; i < ReadCount; i++) {
					pOpenFile->Read(&ImportedSequence, sizeof(stSequenceImport));
					if (ImportedSequence.Count > 0 && ImportedSequence.Count < MAX_SEQUENCE_ITEMS) {
						m_TmpSequences[i].Count = ImportedSequence.Count;
						memcpy(m_TmpSequences[i].Length, ImportedSequence.Length, ImportedSequence.Count);
						memcpy(m_TmpSequences[i].Value, ImportedSequence.Value, ImportedSequence.Count);
					}
				}
				break;

			case FB_PATTERN_ROWS:
				pOpenFile->Read(&FrameCount, sizeof(int));
				m_pSelectedTrack->SetFrameCount(FrameCount);
				for (c = 0; c < FrameCount; c++) {
					for (i = 0; i < m_iChannelsAvailable; i++) {
						pOpenFile->Read(&Pattern, sizeof(int));
						m_pSelectedTrack->SetFramePattern(c, i, Pattern);
					}
				}
				break;

			case FB_PATTERNS:
				pOpenFile->Read(&ReadCount, sizeof(int));
				pOpenFile->Read(&PatternLength, sizeof(int));
				m_pSelectedTrack->SetPatternLength(PatternLength);
				for (unsigned int x = 0; x < m_iChannelsAvailable; x++) {
					for (c = 0; c < ReadCount; c++) {
						for (i = 0; i < PatternLength; i++) {
							pOpenFile->Read(&ImportedNote, sizeof(stChanNoteImport));
							if (ImportedNote.ExtraStuff1 == EF_PORTAOFF) {
								ImportedNote.ExtraStuff1 = EF_PORTAMENTO;
								ImportedNote.ExtraStuff2 = 0;
							}
							else if (ImportedNote.ExtraStuff1 == EF_PORTAMENTO) {
								if (ImportedNote.ExtraStuff2 < 0xFF)
									ImportedNote.ExtraStuff2++;
							}
							stChanNote *Note;
							Note = m_pSelectedTrack->GetPatternData(x, c, i);
							Note->EffNumber[0]	= ImportedNote.ExtraStuff1;
							Note->EffParam[0]	= ImportedNote.ExtraStuff2;
							Note->Instrument	= ImportedNote.Instrument;
							Note->Note			= ImportedNote.Note;
							Note->Octave		= ImportedNote.Octave;
							Note->Vol			= 0;
							if (Note->Note == 0)
								Note->Instrument = MAX_INSTRUMENTS;
							if (Note->Vol == 0)
								Note->Vol = 0x10;
						}
					}
				}
				break;

			case FB_DSAMPLES:
				memset(m_DSamples, 0, sizeof(CDSample) * MAX_DSAMPLES);
				pOpenFile->Read(&ReadCount, sizeof(int));
				for (i = 0; i < ReadCount; i++) {
					pOpenFile->Read(&ImportedDSample, sizeof(stDSampleImport));
					if (ImportedDSample.SampleSize != 0 && ImportedDSample.SampleSize < 0x4000) {
						ImportedDSample.SampleData = new char[ImportedDSample.SampleSize];
						pOpenFile->Read(ImportedDSample.SampleData, ImportedDSample.SampleSize);
					}
					else
						ImportedDSample.SampleData = NULL;
					memcpy(m_DSamples[i].Name, ImportedDSample.Name, 256);
					m_DSamples[i].SampleSize = ImportedDSample.SampleSize;
					m_DSamples[i].SampleData = ImportedDSample.SampleData;
				}
				break;

			case FB_SONGNAME:
				pOpenFile->Read(m_strName, sizeof(char) * 32);
				break;

			case FB_SONGARTIST:
				pOpenFile->Read(m_strArtist, sizeof(char) * 32);
				break;
		
			case FB_SONGCOPYRIGHT:
				pOpenFile->Read(m_strCopyright, sizeof(char) * 32);
				break;
			
			default:
				FileBlock = FB_EOF;
		}
	}

	SetupChannels(m_iExpansionChip);

	ReorderSequences();
	ConvertSequences();

	pOpenFile->Close();

	return TRUE;
}

/**
 *  This function opens the most recent file version
 *
 */
BOOL CFamiTrackerDoc::OpenDocumentNew(CDocumentFile &DocumentFile)
{
	const char *BlockID;
	bool FileFinished = false;
	bool ErrorFlag = false;

#ifdef _DEBUG
	int _msgs_ = 0;
#endif

	// File version checking
	m_iFileVersion = DocumentFile.GetFileVersion();

	// From version 2.0, all files should be compatible (though individual blocks may not)
	if (m_iFileVersion < 0x0200) {
		AfxMessageBox(IDS_FILE_VERSION_ERROR, MB_ICONERROR);
		DocumentFile.Close();
		return FALSE;
	}

	// File version is too new
	if (m_iFileVersion > CDocumentFile::FILE_VER) {
		AfxMessageBox(IDS_FILE_VERSION_TOO_NEW, MB_ICONERROR);
		DocumentFile.Close();
		return FALSE;
	}

	// Delete loaded document
	DeleteContents();

	if (m_iFileVersion < 0x0210) {
		// This has to be done for older files
		SwitchToTrack(0);
	}

	// Read all blocks
	while (!DocumentFile.Finished() && !FileFinished && !ErrorFlag) {
		ErrorFlag = DocumentFile.ReadBlock();
		BlockID = DocumentFile.GetBlockHeaderID();

		if (!strcmp(BlockID, FILE_BLOCK_PARAMS)) {
			ErrorFlag = ReadBlock_Parameters(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_INFO)) {
			DocumentFile.GetBlock(m_strName, 32);
			DocumentFile.GetBlock(m_strArtist, 32);
			DocumentFile.GetBlock(m_strCopyright, 32);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_INSTRUMENTS)) {
			ErrorFlag = ReadBlock_Instruments(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_SEQUENCES)) {
			ErrorFlag = ReadBlock_Sequences(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_FRAMES)) {
			ErrorFlag = ReadBlock_Frames(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_PATTERNS)) {
			ErrorFlag = ReadBlock_Patterns(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_DSAMPLES)) {
			ErrorFlag = ReadBlock_DSamples(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_HEADER)) {
			ErrorFlag = ReadBlock_Header(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_COMMENTS)) {
			ErrorFlag = ReadBlock_Comments(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_SEQUENCES_VRC6)) {
			ErrorFlag = ReadBlock_SequencesVRC6(&DocumentFile);
		}
		else if (!strcmp(BlockID, FILE_BLOCK_SEQUENCES_N163) || !strcmp(BlockID, "SEQUENCES_N106")) {	// TODO remove 106 eventually
			ErrorFlag = ReadBlock_SequencesN163(&DocumentFile);
		}
#if 0
		else if (!strcmp(BlockID, FILE_BLOCK_SEQUENCES_S5B)) {
			ErrorFlag = ReadBlock_SequencesS5B(&DocumentFile);
		}
#endif
		else if (!strcmp(BlockID, "END")) {
			FileFinished = true;
		}
		else {
			// This shouldn't show up in release (debug only)
#ifdef _DEBUG
			_msgs_++;
			if (_msgs_ < 5)
				AfxMessageBox(_T("Unknown file block!"));
#endif
			if (DocumentFile.IsFileIncomplete())
				ErrorFlag = true;
		}
	}

	DocumentFile.Close();

	if (ErrorFlag) {
		AfxMessageBox(IDS_FILE_LOAD_ERROR, MB_ICONERROR);
		DeleteContents();
		return FALSE;
	}

	if (m_iFileVersion <= 0x0201)
		ReorderSequences();

	if (m_iFileVersion < 0x0300)
		ConvertSequences();

	// Always start with track 1
	SwitchToTrack(0);

#ifdef TRANSPOSE_FDS
	if (bAdjustFDSArpeggio) {
		for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
			if (IsInstrumentUsed(i) && GetInstrumentType(i) == INST_FDS) {
				CInstrumentFDS *pInstrument = static_cast<CInstrumentFDS*>(GetInstrument(i));
				CSequence *pSeq = pInstrument->GetArpSeq();
				if (pSeq->GetItemCount() > 0 && pSeq->GetSetting() == ARP_SETTING_FIXED) {
					for (unsigned int j = 0; j < pSeq->GetItemCount(); ++j) {
						pSeq->SetItem(j, pSeq->GetItem(j) + 24);
					}
				}
				pInstrument->Release();
			}
		}
	}
#endif /* TRANSPOSE_FDS */

	return TRUE;
}

bool CFamiTrackerDoc::ReadBlock_Parameters(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();

	if (Version == 1) {
		m_pSelectedTrack->SetSongSpeed(pDocFile->GetBlockInt());
	}
	else
		m_iExpansionChip = pDocFile->GetBlockChar();

	m_iChannelsAvailable	= pDocFile->GetBlockInt();
	m_iMachine				= pDocFile->GetBlockInt();
	m_iEngineSpeed			= pDocFile->GetBlockInt();

	ASSERT_FILE_DATA(m_iMachine == NTSC || m_iMachine == PAL);
	ASSERT_FILE_DATA(m_iChannelsAvailable < MAX_CHANNELS);

	if (m_iMachine != NTSC && m_iMachine != PAL)
		m_iMachine = NTSC;

	if (Version > 2)
		m_iVibratoStyle = (vibrato_t)pDocFile->GetBlockInt();
	else
		m_iVibratoStyle = VIBRATO_OLD;

	// TODO read m_bLinearPitch

	m_iFirstHighlight  = DEFAULT_FIRST_HIGHLIGHT;
	m_iSecondHighlight = DEFAULT_SECOND_HIGHLIGHT;

	if (Version > 3) {
		m_iFirstHighlight = pDocFile->GetBlockInt();
		m_iSecondHighlight = pDocFile->GetBlockInt();
	}

	// This is strange. Sometimes expansion chip is set to 0xFF in files
	if (m_iChannelsAvailable == 5)
		m_iExpansionChip = 0;

	if (m_iFileVersion == 0x0200) {
		int Speed = m_pSelectedTrack->GetSongSpeed();
		if (Speed < 20)
			m_pSelectedTrack->SetSongSpeed(Speed + 1);
	}

	if (Version == 1) {
		if (m_pSelectedTrack->GetSongSpeed() > 19) {
			m_pSelectedTrack->SetSongTempo(m_pSelectedTrack->GetSongSpeed());
			m_pSelectedTrack->SetSongSpeed(6);
		}
		else {
			m_pSelectedTrack->SetSongTempo(m_iMachine == NTSC ? DEFAULT_TEMPO_NTSC : DEFAULT_TEMPO_PAL);
		}
	}

	// Read namco channel count
	if (Version >= 5 && m_iExpansionChip & SNDCHIP_N163) {
		m_iNamcoChannels = pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(m_iNamcoChannels < 9);
	}

	if (Version >= 6) {
		m_iSpeedSplitPoint = pDocFile->GetBlockInt();
	}
	else {
		// Determine if new or old split point is preferred
		m_iSpeedSplitPoint = OLD_SPEED_SPLIT_POINT;
	}

	SetupChannels(m_iExpansionChip);

	return false;
}

bool CFamiTrackerDoc::ReadBlock_Header(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();

	if (Version == 1) {
		// Single track
		m_iTracks = 1;
		SwitchToTrack(0);
		for (unsigned int i = 0; i < m_iChannelsAvailable; ++i) {
			// Channel type (unused)
			pDocFile->GetBlockChar();
			// Effect columns
			m_pSelectedTrack->SetEffectColumnCount(i, pDocFile->GetBlockChar());
		}
	}
	else if (Version >= 2) {
		// Multiple tracks
		m_iTracks = pDocFile->GetBlockChar() + 1;	// 0 means one track

		ASSERT_FILE_DATA(m_iTracks <= MAX_TRACKS);

		// Add tracks to document
		for (unsigned i = 0; i < m_iTracks; ++i) {
			AllocateTrack(i);
		}

		// Track names
		if (Version >= 3) {
			for (unsigned i = 0; i < m_iTracks; ++i) {
				m_sTrackNames[i] = pDocFile->ReadString();
			}
		}

		for (unsigned i = 0; i < m_iChannelsAvailable; ++i) {
			unsigned char ChannelType = pDocFile->GetBlockChar();					// Channel type (unused)
			for (unsigned j = 0; j < m_iTracks; ++j) {
				CPatternData *pTrack = GetTrack(j);
				pTrack->SetEffectColumnCount(i, pDocFile->GetBlockChar());		// Effect columns
			}
		}
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_Comments(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();
	m_bDisplayComment = (pDocFile->GetBlockInt() == 1) ? true : false;
	m_strComment = pDocFile->ReadString();
	return false;
}

bool CFamiTrackerDoc::ReadBlock_Instruments(CDocumentFile *pDocFile)
{
	/*
	 * Version changes
	 *
	 *  2 - Extended DPCM octave range
	 *  3 - Added settings to the arpeggio sequence
	 *
	 */
	
	int Version = pDocFile->GetBlockVersion();

	// Number of instruments
	int Count = pDocFile->GetBlockInt();
	ASSERT_FILE_DATA(Count <= MAX_INSTRUMENTS);

	for (int i = 0; i < Count; ++i) {
		// Instrument index
		int index = pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(index <= MAX_INSTRUMENTS);

		// Read instrument type and create an instrument
		int Type = pDocFile->GetBlockChar();
		CInstrument *pInstrument = CreateInstrument(Type);
		ASSERT_FILE_DATA(pInstrument != NULL);

		// Load the instrument
		ASSERT_FILE_DATA(pInstrument->Load(pDocFile));

		// Read name
		unsigned int size = pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(size <= CInstrument::INST_NAME_MAX);
		char Name[CInstrument::INST_NAME_MAX];
		pDocFile->GetBlock(Name, size);
		Name[size] = 0;
		pInstrument->SetName(Name);

		// Store instrument
		m_pInstruments[index] = pInstrument;
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_Sequences(CDocumentFile *pDocFile)
{
	unsigned int ReleasePoint = -1, Settings = 0;
	int Version = pDocFile->GetBlockVersion();

	unsigned int Count = pDocFile->GetBlockInt();
	ASSERT_FILE_DATA(Count < (MAX_SEQUENCES * SEQ_COUNT));

	if (Version == 1) {
		for (unsigned int i = 0; i < Count; ++i) {
			unsigned int Index = pDocFile->GetBlockInt();
			unsigned char SeqCount = pDocFile->GetBlockChar();
			ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
			ASSERT_FILE_DATA(SeqCount < MAX_SEQUENCE_ITEMS);
			m_TmpSequences[Index].Count = SeqCount;
			for (int j = 0; j < SeqCount; ++j) {
				m_TmpSequences[Index].Value[j] = pDocFile->GetBlockChar();
				m_TmpSequences[Index].Length[j] = pDocFile->GetBlockChar();
			}
		}
	}
	else if (Version == 2) {
		for (unsigned int i = 0; i < Count; ++i) {
			unsigned int Index = pDocFile->GetBlockInt();
			unsigned int Type = pDocFile->GetBlockInt();
			unsigned char SeqCount = pDocFile->GetBlockChar();
			ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
			ASSERT_FILE_DATA(Type < SEQ_COUNT);
			ASSERT_FILE_DATA(SeqCount < MAX_SEQUENCE_ITEMS);
			m_Sequences[Index][Type].Count = SeqCount;
			for (int j = 0; j < SeqCount; ++j) {
				char Value = pDocFile->GetBlockChar();
				char Length = pDocFile->GetBlockChar();
				m_Sequences[Index][Type].Value[j] = Value;
				m_Sequences[Index][Type].Length[j] = Length;
			}
		}
	}
	else if (Version >= 3) {
		int Indices[MAX_SEQUENCES * SEQ_COUNT];
		int Types[MAX_SEQUENCES * SEQ_COUNT];

		for (unsigned int i = 0; i < Count; ++i) {
			unsigned int Index = pDocFile->GetBlockInt();
			unsigned int Type = pDocFile->GetBlockInt();
			unsigned char SeqCount = pDocFile->GetBlockChar();
			unsigned int LoopPoint = pDocFile->GetBlockInt();

			// Work-around for some older files
			if (LoopPoint == SeqCount)
				LoopPoint = -1;

			Indices[i] = Index;
			Types[i] = Type;

			ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
			ASSERT_FILE_DATA(Type < SEQ_COUNT);
//			ASSERT_FILE_DATA(SeqCount <= MAX_SEQUENCE_ITEMS);

			CSequence *pSeq = GetSequence(Index, Type);

			pSeq->Clear();
			pSeq->SetItemCount(SeqCount < MAX_SEQUENCE_ITEMS ? SeqCount : MAX_SEQUENCE_ITEMS);
			pSeq->SetLoopPoint(LoopPoint);

			if (Version == 4) {
				ReleasePoint = pDocFile->GetBlockInt();
				Settings = pDocFile->GetBlockInt();
				pSeq->SetReleasePoint(ReleasePoint);
				pSeq->SetSetting(Settings);
			}

			for (int j = 0; j < SeqCount; ++j) {
				char Value = pDocFile->GetBlockChar();
				if (j <= MAX_SEQUENCE_ITEMS)
					pSeq->SetItem(j, Value);
			}
		}

		if (Version == 5) {
			// Version 5 saved the release points incorrectly, this is fixed in ver 6
			for (unsigned int i = 0; i < MAX_SEQUENCES; ++i) {
				for (int j = 0; j < SEQ_COUNT; ++j) {
					ReleasePoint = pDocFile->GetBlockInt();
					Settings = pDocFile->GetBlockInt();
					if (GetSequenceItemCount(i, j) > 0) {
						CSequence *pSeq = GetSequence(i, j);
						pSeq->SetReleasePoint(ReleasePoint);
						pSeq->SetSetting(Settings);
					}
				}
			}
		}
		else if (Version >= 6) {
			// Read release points correctly stored
			for (unsigned int i = 0; i < Count; ++i) {
				ReleasePoint = pDocFile->GetBlockInt();
				Settings = pDocFile->GetBlockInt();
				unsigned int Index = Indices[i];
				unsigned int Type = Types[i];
				CSequence *pSeq = GetSequence(Index, Type);
				pSeq->SetReleasePoint(ReleasePoint);
				pSeq->SetSetting(Settings);
			}
		}

	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_SequencesVRC6(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();

	unsigned int Count = pDocFile->GetBlockInt();
	ASSERT_FILE_DATA(Count < (MAX_SEQUENCES * SEQ_COUNT));

	if (Version < 4) {
		for (unsigned int i = 0; i < Count; ++i) {
			unsigned int Index	  = pDocFile->GetBlockInt();
			unsigned int Type	  = pDocFile->GetBlockInt();
			unsigned char SeqCount = pDocFile->GetBlockChar();
			unsigned int LoopPoint = pDocFile->GetBlockInt();
//			if (SeqCount > MAX_SEQUENCE_ITEMS)
//				SeqCount = MAX_SEQUENCE_ITEMS;
			ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
			ASSERT_FILE_DATA(Type < SEQ_COUNT);
//			ASSERT_FILE_DATA(SeqCount <= MAX_SEQUENCE_ITEMS);
			CSequence *pSeq = GetSequenceVRC6(Index, Type);
			pSeq->Clear();
			pSeq->SetItemCount(SeqCount < MAX_SEQUENCE_ITEMS ? SeqCount : MAX_SEQUENCE_ITEMS);
			pSeq->SetLoopPoint(LoopPoint);
			for (unsigned int j = 0; j < SeqCount; ++j) {
				char Value = pDocFile->GetBlockChar();
				if (j <= MAX_SEQUENCE_ITEMS)
					pSeq->SetItem(j, Value);
			}
		}
	}
	else {
		int Indices[MAX_SEQUENCES];
		int Types[MAX_SEQUENCES];
		unsigned int ReleasePoint = -1, Settings = 0;

		for (unsigned int i = 0; i < Count; ++i) {
			unsigned int Index	  = pDocFile->GetBlockInt();
			unsigned int Type	  = pDocFile->GetBlockInt();
			unsigned char SeqCount  = pDocFile->GetBlockChar();
			unsigned int LoopPoint = pDocFile->GetBlockInt();

			Indices[i] = Index;
			Types[i] = Type;
/*
			if (SeqCount >= MAX_SEQUENCE_ITEMS)
				SeqCount = MAX_SEQUENCE_ITEMS - 1;
*/
			ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
			ASSERT_FILE_DATA(Type < SEQ_COUNT);
//			ASSERT_FILE_DATA(SeqCount <= MAX_SEQUENCE_ITEMS);

			CSequence *pSeq = GetSequenceVRC6(Index, Type);

			pSeq->Clear();
			pSeq->SetItemCount(SeqCount);
			pSeq->SetLoopPoint(LoopPoint);

			if (Version == 4) {
				ReleasePoint = pDocFile->GetBlockInt();
				Settings = pDocFile->GetBlockInt();
				pSeq->SetReleasePoint(ReleasePoint);
				pSeq->SetSetting(Settings);
			}

			for (unsigned int j = 0; j < SeqCount; ++j) {
				char Value = pDocFile->GetBlockChar();
				if (j <= MAX_SEQUENCE_ITEMS)
					pSeq->SetItem(j, Value);
			}
		}

		if (Version == 5) {
			// Version 5 saved the release points incorrectly, this is fixed in ver 6
			for (int i = 0; i < MAX_SEQUENCES; ++i) {
				for (int j = 0; j < SEQ_COUNT; ++j) {
					ReleasePoint = pDocFile->GetBlockInt();
					Settings = pDocFile->GetBlockInt();
					if (GetSequenceItemCountVRC6(i, j) > 0) {
						CSequence *pSeq = GetSequenceVRC6(i, j);
						pSeq->SetReleasePoint(ReleasePoint);
						pSeq->SetSetting(Settings);
					}
				}
			}
		}
		else if (Version >= 6) {
			for (unsigned int i = 0; i < Count; ++i) {
				ReleasePoint = pDocFile->GetBlockInt();
				Settings = pDocFile->GetBlockInt();
				unsigned int Index = Indices[i];
				unsigned int Type = Types[i];
				CSequence *pSeq = GetSequenceVRC6(Index, Type);
				pSeq->SetReleasePoint(ReleasePoint);
				pSeq->SetSetting(Settings);
			}
		}
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_SequencesN163(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();

	unsigned int Count = pDocFile->GetBlockInt();
	ASSERT_FILE_DATA(Count < (MAX_SEQUENCES * SEQ_COUNT));

	for (unsigned int i = 0; i < Count; i++) {
		unsigned int  Index		   = pDocFile->GetBlockInt();
		unsigned int  Type		   = pDocFile->GetBlockInt();
		unsigned char SeqCount	   = pDocFile->GetBlockChar();
		unsigned int  LoopPoint	   = pDocFile->GetBlockInt();
		unsigned int  ReleasePoint = pDocFile->GetBlockInt();
		unsigned int  Setting	   = pDocFile->GetBlockInt();

		ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
		ASSERT_FILE_DATA(Type < SEQ_COUNT);

		CSequence *pSeq = GetSequenceN163(Index, Type);

		pSeq->Clear();
		pSeq->SetItemCount(SeqCount);
		pSeq->SetLoopPoint(LoopPoint);
		pSeq->SetReleasePoint(ReleasePoint);
		pSeq->SetSetting(Setting);

		for (int j = 0; j < SeqCount; ++j) {
			char Value = pDocFile->GetBlockChar();
			if (j <= MAX_SEQUENCE_ITEMS)
				pSeq->SetItem(j, Value);
		}
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_SequencesS5B(CDocumentFile *pDocFile)
{
	int Version = pDocFile->GetBlockVersion();

	unsigned int Count = pDocFile->GetBlockInt();
	ASSERT_FILE_DATA(Count < (MAX_SEQUENCES * SEQ_COUNT));

	for (unsigned int i = 0; i < Count; i++) {
		unsigned int  Index		   = pDocFile->GetBlockInt();
		unsigned int  Type		   = pDocFile->GetBlockInt();
		unsigned char SeqCount	   = pDocFile->GetBlockChar();
		unsigned int  LoopPoint	   = pDocFile->GetBlockInt();
		unsigned int  ReleasePoint = pDocFile->GetBlockInt();
		unsigned int  Setting	   = pDocFile->GetBlockInt();

		ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
		ASSERT_FILE_DATA(Type < SEQ_COUNT);

		CSequence *pSeq = GetSequenceS5B(Index, Type);

		pSeq->Clear();
		pSeq->SetItemCount(SeqCount);
		pSeq->SetLoopPoint(LoopPoint);
		pSeq->SetReleasePoint(ReleasePoint);
		pSeq->SetSetting(Setting);

		for (int j = 0; j < SeqCount; ++j) {
			char Value = pDocFile->GetBlockChar();
			if (j <= MAX_SEQUENCE_ITEMS)
				pSeq->SetItem(j, Value);
		}
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_Frames(CDocumentFile *pDocFile)
{
	unsigned int Version = pDocFile->GetBlockVersion();

	if (Version == 1) {
		unsigned int FrameCount = pDocFile->GetBlockInt();
		m_pSelectedTrack->SetFrameCount(FrameCount);
		m_iChannelsAvailable = pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(FrameCount <= MAX_FRAMES);
		ASSERT_FILE_DATA(m_iChannelsAvailable <= MAX_CHANNELS);
		for (unsigned i = 0; i < FrameCount; ++i) {
			for (unsigned j = 0; j < m_iChannelsAvailable; ++j) {
				unsigned Pattern = (unsigned)pDocFile->GetBlockChar();
				ASSERT_FILE_DATA(Pattern < MAX_FRAMES);
				m_pSelectedTrack->SetFramePattern(i, j, Pattern);
			}
		}
	}
	else if (Version > 1) {

		for (unsigned y = 0; y < m_iTracks; ++y) {
			unsigned int FrameCount = pDocFile->GetBlockInt();
			unsigned int Speed = pDocFile->GetBlockInt();
			ASSERT_FILE_DATA(FrameCount > 0 && FrameCount <= MAX_FRAMES);
			ASSERT_FILE_DATA(Speed > 0);

			CPatternData *pTrack = GetTrack(y);
			pTrack->SetFrameCount(FrameCount);

			if (Version == 3) {
				unsigned int Tempo = pDocFile->GetBlockInt();
//				ASSERT_FILE_DATA(Tempo >= 0 && Tempo <= MAX_TEMPO);
//				ASSERT_FILE_DATA(Speed >= 0 && Speed <= MAX_SPEED);
				ASSERT_FILE_DATA(Speed >= 0);
				ASSERT_FILE_DATA(Tempo >= 0);
				pTrack->SetSongTempo(Tempo);
				pTrack->SetSongSpeed(Speed);
			}
			else {
				if (Speed < 20) {
					unsigned int Tempo = (m_iMachine == NTSC) ? DEFAULT_TEMPO_NTSC : DEFAULT_TEMPO_PAL;
					ASSERT_FILE_DATA(Tempo >= 0 && Tempo <= MAX_TEMPO);
					//ASSERT_FILE_DATA(Speed >= 0 && Speed <= MAX_SPEED);
					ASSERT_FILE_DATA(Speed >= 0);
					pTrack->SetSongTempo(Tempo);
					pTrack->SetSongSpeed(Speed);
				}
				else {
					ASSERT_FILE_DATA(Speed >= 0 && Speed <= MAX_TEMPO);
					pTrack->SetSongTempo(Speed);
					pTrack->SetSongSpeed(DEFAULT_SPEED);
				}
			}

			unsigned PatternLength = (unsigned)pDocFile->GetBlockInt();
			ASSERT_FILE_DATA(PatternLength > 0 && PatternLength <= MAX_PATTERN_LENGTH);

			pTrack->SetPatternLength(PatternLength);
			
			for (unsigned i = 0; i < FrameCount; ++i) {
				for (unsigned j = 0; j < m_iChannelsAvailable; ++j) {
					// Read pattern index
					unsigned Pattern = (unsigned char)pDocFile->GetBlockChar();
					ASSERT_FILE_DATA(Pattern < MAX_PATTERN);
					pTrack->SetFramePattern(i, j, Pattern);
				}
			}
		}
	}

	return false;
}

bool CFamiTrackerDoc::ReadBlock_Patterns(CDocumentFile *pDocFile)
{
	unsigned int Version = pDocFile->GetBlockVersion();

	if (Version == 1) {
		int PatternLen = pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(PatternLen <= MAX_PATTERN_LENGTH);
		m_pSelectedTrack->SetPatternLength(PatternLen);
	}

	while (!pDocFile->BlockDone()) {
		unsigned Track;
		if (Version > 1)
			Track = pDocFile->GetBlockInt();
		else if (Version == 1)
			Track = 0;

		unsigned Channel = pDocFile->GetBlockInt();
		unsigned Pattern = pDocFile->GetBlockInt();
		unsigned Items	= pDocFile->GetBlockInt();

		if (Channel > MAX_CHANNELS)
			return false;

		ASSERT_FILE_DATA(Track < MAX_TRACKS);
		ASSERT_FILE_DATA(Channel < MAX_CHANNELS);
		ASSERT_FILE_DATA(Pattern < MAX_PATTERN);
		ASSERT_FILE_DATA((Items - 1) < MAX_PATTERN_LENGTH);

		SwitchToTrack(Track);

		for (unsigned i = 0; i < Items; ++i) {
			unsigned Row;
			if (m_iFileVersion == 0x0200)
				Row = pDocFile->GetBlockChar();
			else
				Row = pDocFile->GetBlockInt();

			ASSERT_FILE_DATA(Row < MAX_PATTERN_LENGTH);

			stChanNote *Note = m_pSelectedTrack->GetPatternData(Channel, Pattern, Row);
			memset(Note, 0, sizeof(stChanNote));

			Note->Note		 = pDocFile->GetBlockChar();
			Note->Octave	 = pDocFile->GetBlockChar();
			Note->Instrument = pDocFile->GetBlockChar();
			Note->Vol		 = pDocFile->GetBlockChar();

			if (m_iFileVersion == 0x0200) {
				unsigned char EffectNumber, EffectParam;
				EffectNumber = pDocFile->GetBlockChar();
				EffectParam = pDocFile->GetBlockChar();
				if (Version < 3) {
					if (EffectNumber == EF_PORTAOFF) {
						EffectNumber = EF_PORTAMENTO;
						EffectParam = 0;
					}
					else if (EffectNumber == EF_PORTAMENTO) {
						if (EffectParam < 0xFF)
							EffectParam++;
					}
				}

				stChanNote *Note = m_pSelectedTrack->GetPatternData(Channel, Pattern, Row);

				Note->EffNumber[0]	= EffectNumber;
				Note->EffParam[0]	= EffectParam;
			}
			else {
				for (int n = 0; n < (m_pSelectedTrack->GetEffectColumnCount(Channel) + 1); ++n) {
					unsigned char EffectNumber, EffectParam;
					EffectNumber = pDocFile->GetBlockChar();
					EffectParam = pDocFile->GetBlockChar();

					if (Version < 3) {
						if (EffectNumber == EF_PORTAOFF) {
							EffectNumber = EF_PORTAMENTO;
							EffectParam = 0;
						}
						else if (EffectNumber == EF_PORTAMENTO) {
							if (EffectParam < 0xFF)
								EffectParam++;
						}
					}

					Note->EffNumber[n]	= EffectNumber;
					Note->EffParam[n] 	= EffectParam;
				}
			}

			if (Note->Vol > 0x10)
				Note->Vol &= 0x0F;

			// Specific for version 2.0
			if (m_iFileVersion == 0x0200) {

				if (Note->EffNumber[0] == EF_SPEED && Note->EffParam[0] < 20)
					Note->EffParam[0]++;
				
				if (Note->Vol == 0)
					Note->Vol = 0x10;
				else {
					Note->Vol--;
					Note->Vol &= 0x0F;
				}

				if (Note->Note == 0)
					Note->Instrument = MAX_INSTRUMENTS;
			}

			if (Version == 3) {
				// Fix for VRC7 portamento
				if (GetExpansionChip() == SNDCHIP_VRC7 && Channel > 4) {
					for (int n = 0; n < MAX_EFFECT_COLUMNS; ++n) {
						switch (Note->EffNumber[n]) {
							case EF_PORTA_DOWN:
								Note->EffNumber[n] = EF_PORTA_UP;
								break;
							case EF_PORTA_UP:
								Note->EffNumber[n] = EF_PORTA_DOWN;
								break;
						}
					}
				}
				// FDS pitch effect fix
				else if (ExpansionEnabled(SNDCHIP_FDS) && GetChannelType(Channel) == CHANID_FDS) {
					for (int n = 0; n < MAX_EFFECT_COLUMNS; ++n) {
						switch (Note->EffNumber[n]) {
							case EF_PITCH:
								if (Note->EffParam[n] != 0x80)
									Note->EffParam[n] = (0x100 - Note->EffParam[n]) & 0xFF;
								break;
						}
					}
				}
			}
#ifdef TRANSPOSE_FDS
			if (Version < 5) {
				// FDS octave
				if (ExpansionEnabled(SNDCHIP_FDS) && GetChannelType(Channel) == CHANID_FDS && Note->Octave < 6) {
					Note->Octave += 2;
					bAdjustFDSArpeggio = true;
				}
			}
#endif /* TRANSPOSE_FDS */
			/*
			if (Version < 6) {
				// Noise pitch slide fix
				if (GetChannelType(Channel) == CHANID_NOISE) {
					for (int n = 0; n < MAX_EFFECT_COLUMNS; ++n) {
						switch (Note->EffNumber[n]) {
							case EF_PORTA_DOWN:
								Note->EffNumber[n] = EF_PORTA_UP;
								Note->EffParam[n] = Note->EffParam[n] << 4;
								break;
							case EF_PORTA_UP:
								Note->EffNumber[n] = EF_PORTA_DOWN;
								Note->EffParam[n] = Note->EffParam[n] << 4;
								break;
							case EF_PORTAMENTO:
								Note->EffParam[n] = Note->EffParam[n] << 4;
								break;
							case EF_SLIDE_UP:
								Note->EffParam[n] = Note->EffParam[n] + 0x70;
								break;
							case EF_SLIDE_DOWN:
								Note->EffParam[n] = Note->EffParam[n] + 0x70;
								break;
						}
					}
				}
			}
			*/
		}
	}
	
	return false;
}

bool CFamiTrackerDoc::ReadBlock_DSamples(CDocumentFile *pDocFile)
{
	int Count = 0, i, Item, Len, Size;
	int Version = pDocFile->GetBlockVersion();

	Count = pDocFile->GetBlockChar();
	ASSERT_FILE_DATA(Count <= MAX_DSAMPLES);
	
	memset(m_DSamples, 0, sizeof(CDSample) * MAX_DSAMPLES);

	for (i = 0; i < Count; i++) {
		Item	= pDocFile->GetBlockChar();
		Len		= pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(Item < MAX_DSAMPLES);
		ASSERT_FILE_DATA(Len < 256);
		pDocFile->GetBlock(m_DSamples[Item].Name, Len);
		m_DSamples[Item].Name[Len] = 0;
		Size	= pDocFile->GetBlockInt();
		ASSERT_FILE_DATA(Size < 0x8000);
		m_DSamples[Item].SampleData = new char[Size];
		m_DSamples[Item].SampleSize = Size;
		pDocFile->GetBlock(m_DSamples[Item].SampleData, Size);
	}

	return false;
}

// FTM import ////

CFamiTrackerDoc *CFamiTrackerDoc::LoadImportFile(LPCTSTR lpszPathName)
{
	// Import a module as new subtunes
	CFamiTrackerDoc *pImported = new CFamiTrackerDoc();

	pImported->DeleteContents();

	// Load into a new document
	if (!pImported->OpenDocument(lpszPathName))
		SAFE_RELEASE(pImported);

	return pImported;
}

bool CFamiTrackerDoc::ImportInstruments(CFamiTrackerDoc *pImported, int *pInstTable)
{
	// Copy instruments to current module
	//
	// pInstTable must point to an int array of size MAX_INSTRUMENTS
	//

	int SamplesTable[MAX_DSAMPLES];
	int SequenceTable[MAX_SEQUENCES][SEQ_COUNT];
	int SequenceTableVRC6[MAX_SEQUENCES][SEQ_COUNT];
	int SequenceTableN163[MAX_SEQUENCES][SEQ_COUNT];

	memset(SamplesTable, 0, sizeof(int) * MAX_DSAMPLES);
	memset(SequenceTable, 0, sizeof(int) * MAX_SEQUENCES * SEQ_COUNT);
	memset(SequenceTableVRC6, 0, sizeof(int) * MAX_SEQUENCES * SEQ_COUNT);
	memset(SequenceTableN163, 0, sizeof(int) * MAX_SEQUENCES * SEQ_COUNT);

	// Check instrument count
	if (GetInstrumentCount() + pImported->GetInstrumentCount() > MAX_INSTRUMENTS) {
		// Out of instrument slots
		AfxMessageBox(IDS_IMPORT_INSTRUMENT_COUNT, MB_ICONERROR);
		return false;
	}

	// Copy sequences
	for (int s = 0; s < MAX_SEQUENCES; ++s) {
		for (int t = 0; t < SEQ_COUNT; ++t) {
			// 2A03
			if (pImported->GetSequenceItemCount(s, t) > 0) {
				CSequence *pImportSeq = pImported->GetSequence(s, t);
				int index = GetFreeSequence(t);
				CSequence *pSeq = GetSequence(index, t);
				pSeq->Copy(pImportSeq);
				// Save a reference to this sequence
				SequenceTable[s][t] = index;
			}
			// VRC6
			if (pImported->GetSequenceItemCountVRC6(s, t) > 0) {
				CSequence *pImportSeq = pImported->GetSequenceVRC6(s, t);
				int index = GetFreeSequenceVRC6(t);
				CSequence *pSeq = GetSequenceVRC6(index, t);
				pSeq->Copy(pImportSeq);
				// Save a reference to this sequence
				SequenceTableVRC6[s][t] = index;
			}
			// N163
			if (pImported->GetSequenceItemCountN163(s, t) > 0) {
				CSequence *pImportSeq = pImported->GetSequenceN163(s, t);
				int index = GetFreeSequenceN163(t);
				CSequence *pSeq = GetSequenceN163(index, t);
				pSeq->Copy(pImportSeq);
				// Save a reference to this sequence
				SequenceTableN163[s][t] = index;
			}
		}
	}

	bool bOutOfSampleSpace = false;

	// Copy DPCM samples
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		CDSample *pImportDSample = pImported->GetSample(i);
		if (pImportDSample->SampleSize > 0) {
			int Index = GetFreeSampleSlot();
			if (Index != -1) {
				CDSample *pDSample = GetSample(Index);
				pDSample->Copy(pImportDSample);
				// Save a reference to this DPCM sample
				SamplesTable[i] = Index;
			}
			else
				bOutOfSampleSpace = true;
		}
	}

	if (bOutOfSampleSpace) {
		// Out of sample space
		AfxMessageBox(IDS_IMPORT_SAMPLE_SLOTS, MB_ICONEXCLAMATION);
		return false;
	}

	// Copy instruments
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (pImported->IsInstrumentUsed(i)) {
			CInstrument *pImportInst = pImported->GetInstrument(i);
			CInstrument *pInst = pImportInst->Clone();
			pImportInst->Release();
			// Update references
			switch (pInst->GetType()) {
				case INST_2A03: 
					{
						CInstrument2A03 *pInstrument = static_cast<CInstrument2A03*>(pInst);
						// Update sequence references
						for (int t = 0; t < SEQ_COUNT; ++t) {
							if (pInstrument->GetSeqEnable(t)) {
								pInstrument->SetSeqIndex(t, SequenceTable[pInstrument->GetSeqIndex(t)][t]);
							}
						}
						// Update DPCM samples
						for (int o = 0; o < OCTAVE_RANGE; ++o) {
							for (int n = 0; n < NOTE_RANGE; ++n) {
								int Sample = pInstrument->GetSample(o, n);
								if (Sample != 0) {
									pInstrument->SetSample(o, n, SamplesTable[Sample - 1] + 1);
								}
							}
						}
					}
					break;
				case INST_VRC6: 
					{
						CInstrumentVRC6 *pInstrument = static_cast<CInstrumentVRC6*>(pInst);
						// Update sequence references
						for (int t = 0; t < SEQ_COUNT; ++t) {
							if (pInstrument->GetSeqEnable(t)) {
								pInstrument->SetSeqIndex(t, SequenceTableVRC6[pInstrument->GetSeqIndex(t)][t]);
							}
						}
					}
					break;
				case INST_N163:
					{
						CInstrumentN163 *pInstrument = static_cast<CInstrumentN163*>(pInst);
						// Update sequence references
						for (int t = 0; t < SEQ_COUNT; ++t) {
							if (pInstrument->GetSeqEnable(t)) {
								pInstrument->SetSeqIndex(t, SequenceTableN163[pInstrument->GetSeqIndex(t)][t]);
							}
						}
					}
					break;
				default:
					AfxDebugBreak();	// Add code for this instrument
			}
			// Update samples
			int Index = AddInstrument(pInst);
			// Save a reference to this instrument
			pInstTable[i] = Index;
		}
	}

	return true;
}

bool CFamiTrackerDoc::ImportTrack(int Track, CFamiTrackerDoc *pImported, int *pInstTable)
{
	// Import a selected track from specified source document

	int NewTrack = AddTrack();

	if (NewTrack == -1)
		return false;

	m_pSelectedTrack = GetTrack(NewTrack);

	// Copy parameters
	SetPatternLength(pImported->GetPatternLength(Track));
	SetFrameCount(pImported->GetFrameCount(Track));
	SetSongTempo(pImported->GetSongTempo(Track));
	SetSongSpeed(pImported->GetSongSpeed(Track));

	// Copy track name
	SetTrackTitle(NewTrack, pImported->GetTrackTitle(Track));

	// Copy frames
	for (unsigned int f = 0; f < pImported->GetFrameCount(Track); ++f) {
		for (unsigned int c = 0; c < GetAvailableChannels(); ++c) {
			SetPatternAtFrame(f, c, pImported->GetPatternAtFrame(Track, f, c));
		}
	}

	stChanNote data;

	// Copy patterns
	for (unsigned int p = 0; p < MAX_PATTERN; ++p) {
		for (unsigned int c = 0; c < GetAvailableChannels(); ++c) {
			for (unsigned int r = 0; r < pImported->GetPatternLength(Track); ++r) {
				// Get note
				pImported->GetDataAtPattern(Track, p, c, r, &data);
				// Translate instrument number
				if (data.Instrument < MAX_INSTRUMENTS)
					data.Instrument = pInstTable[data.Instrument];
				// Store
				SetDataAtPattern(NewTrack, p, c, r, &data);
			}
		}
	}

	// Effect columns
	for (unsigned int c = 0; c < GetAvailableChannels(); ++c) {
		SetEffColumns(c, pImported->GetEffColumns(Track, c));
	}

	m_pSelectedTrack = GetTrack(m_iTrack);

	return true;
}

// End of file load/save

// DMC Stuff

CDSample *CFamiTrackerDoc::GetSample(unsigned int Index)
{
	ASSERT(Index < MAX_DSAMPLES);
	return &m_DSamples[Index];
}

void CFamiTrackerDoc::GetSampleName(unsigned int Index, char *Name) const
{
	ASSERT(Index < MAX_DSAMPLES);
	ASSERT(m_DSamples[Index].SampleSize > 0);
	strcpy(Name, m_DSamples[Index].Name);
}

int CFamiTrackerDoc::GetFreeSampleSlot() const
{
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		if (m_DSamples[i].SampleSize == 0) {
			return i;
		}
	}
	// Out of free samples
	return -1;
}

void CFamiTrackerDoc::RemoveSample(unsigned int Index)
{
	ASSERT(Index < MAX_DSAMPLES);

	if (m_DSamples[Index].SampleSize != 0) {
		SAFE_RELEASE_ARRAY(m_DSamples[Index].SampleData);
		m_DSamples[Index].SampleSize = 0;
		SetModifiedFlag();
	}
}

int CFamiTrackerDoc::GetTotalSampleSize() const
{
	// Return total size of all loaded samples
	int Size = 0;
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		Size += m_DSamples[i].SampleSize;
	}
	return Size;

}

// ---------------------------------------------------------------------------------------------------------
// Document access functions
// ---------------------------------------------------------------------------------------------------------

void CFamiTrackerDoc::SetSongInfo(const char *Name, const char *Artist, const char *Copyright)
{
	ASSERT(Name != NULL && Artist != NULL && Copyright != NULL);

	// Check if strings actually changed
	if (strcmp(Name, m_strName) || strcmp(Artist, m_strArtist) || strcmp(Copyright, m_strCopyright))
		SetModifiedFlag();

	strncpy(m_strName, Name, 32);
	strncpy(m_strArtist, Artist, 32);
	strncpy(m_strCopyright, Copyright, 32);

	// Limit strings to 31 characters to make room for null-termination
	m_strName[31] = 0;
	m_strArtist[31] = 0;
	m_strCopyright[31] = 0;
}

// Sequences

CSequence *CFamiTrackerDoc::GetSequence(int Chip, int Index, int Type)
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	switch (Chip) {
		case SNDCHIP_NONE: 
			return GetSequence(Index, Type);
		case SNDCHIP_VRC6: 
			return GetSequenceVRC6(Index, Type);
		case SNDCHIP_N163: 
			return GetSequenceN163(Index, Type);
		case SNDCHIP_S5B:
			return 0;
			//return GetSequenceS5B(Index, Type);
	}

	return NULL;
}

CSequence *CFamiTrackerDoc::GetSequence(int Index, int Type)
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequences2A03[Index][Type] == NULL)
		m_pSequences2A03[Index][Type] = new CSequence();

	return m_pSequences2A03[Index][Type];
}

CSequenceInterface const *CFamiTrackerDoc::GetSequence(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);
	return dynamic_cast<CSequenceInterface const *>(m_pSequences2A03[Index][Type]);
}

int CFamiTrackerDoc::GetSequenceItemCount(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);
	
	if (m_pSequences2A03[Index][Type] == NULL)
		return 0;

	return m_pSequences2A03[Index][Type]->GetItemCount();
}

int CFamiTrackerDoc::GetFreeSequence(int Type) const
{
	// Return a free sequence slot
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		if (GetSequenceItemCount(i, Type) == 0)
			return i;
	}
	return 0;
}

int CFamiTrackerDoc::GetSequenceCount(int Type) const
{
	// Return number of allocated sequences of Type
	int Count = 0;
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		if (GetSequenceItemCount(i, Type) > 0)
			++Count;
	}
	return Count;
}

// VRC6

CSequence *CFamiTrackerDoc::GetSequenceVRC6(int Index, int Type)
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesVRC6[Index][Type] == NULL)
		m_pSequencesVRC6[Index][Type] = new CSequence();

	return m_pSequencesVRC6[Index][Type];
}

CSequence *CFamiTrackerDoc::GetSequenceVRC6(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);
	return m_pSequencesVRC6[Index][Type];
}

int CFamiTrackerDoc::GetSequenceItemCountVRC6(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesVRC6[Index][Type] == NULL)
		return 0;

	return m_pSequencesVRC6[Index][Type]->GetItemCount();
}

int CFamiTrackerDoc::GetFreeSequenceVRC6(int Type) const
{
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		if (GetSequenceItemCountVRC6(i, Type) == 0)
			return i;
	}
	return 0;
}

// N163

CSequence *CFamiTrackerDoc::GetSequenceN163(int Index, int Type)
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesN163[Index][Type] == NULL)
		m_pSequencesN163[Index][Type] = new CSequence();

	return m_pSequencesN163[Index][Type];
}

CSequence *CFamiTrackerDoc::GetSequenceN163(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);
	return m_pSequencesN163[Index][Type];
}

int CFamiTrackerDoc::GetSequenceItemCountN163(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesN163[Index][Type] == NULL)
		return 0;

	return m_pSequencesN163[Index][Type]->GetItemCount();
}

int CFamiTrackerDoc::GetFreeSequenceN163(int Type) const
{
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		if (GetSequenceItemCountN163(i, Type) == 0)
			return i;
	}
	return 0;
}

// Sunsoft

CSequence *CFamiTrackerDoc::GetSequenceS5B(int Index, int Type)
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesS5B[Index][Type] == NULL)
		m_pSequencesS5B[Index][Type] = new CSequence();

	return m_pSequencesS5B[Index][Type];
}

CSequence *CFamiTrackerDoc::GetSequenceS5B(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);
	return m_pSequencesS5B[Index][Type];
}

int CFamiTrackerDoc::GetSequenceItemCountS5B(int Index, int Type) const
{
	ASSERT(Index >= 0 && Index < MAX_SEQUENCES && Type >= 0 && Type < SEQ_COUNT);

	if (m_pSequencesS5B[Index][Type] == NULL)
		return 0;

	return m_pSequencesS5B[Index][Type]->GetItemCount();
}

int CFamiTrackerDoc::GetFreeSequenceS5B(int Type) const
{
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		if (GetSequenceItemCountS5B(i, Type) == 0)
			return i;
	}
	return 0;
}

// Song info

void CFamiTrackerDoc::SetSongName(const char *pName)
{
	if (strcmp(m_strName, pName) != 0) {
		strcpy(m_strName, pName);
		SetModifiedFlag();
	}
}

void CFamiTrackerDoc::SetSongArtist(const char *pArtist)
{
	if (strcmp(m_strArtist, pArtist) != 0) {
		strcpy(m_strArtist, pArtist);
		SetModifiedFlag();
	}
}

void CFamiTrackerDoc::SetSongCopyright(const char *pCopyright)
{
	if (strcmp(m_strCopyright, pCopyright) != 0) {
		strcpy(m_strCopyright, pCopyright);
		SetModifiedFlag();
	}
}


//
// Instruments
//

CInstrument *CFamiTrackerDoc::GetInstrument(int Index) const
{
	//
	// Note!
	//
	// Always call Release() on instruments when done, otherwise they will leak.
	// CInstrumentContainer can be used to automatically call Release()
	//

	// This may return a NULL pointer
	ASSERT(Index >= 0 && Index < MAX_INSTRUMENTS);

	m_csInstrument.Lock();

	CInstrument *pInstrument = m_pInstruments[Index];

	if (pInstrument != NULL)
		pInstrument->Retain();

	m_csInstrument.Unlock();

	return pInstrument;
}

CInstrument2A03Interface const *CFamiTrackerDoc::Get2A03Instrument(int Instrument) const
{
	// This may return a NULL pointer
	ASSERT(Instrument >= 0 && Instrument < MAX_INSTRUMENTS);
	return dynamic_cast<CInstrument2A03Interface const *>(m_pInstruments[Instrument]);
}

int CFamiTrackerDoc::GetInstrumentCount() const
{
	int count = 0;
	for(int i = 0; i < MAX_INSTRUMENTS; ++i)
	{
		if( IsInstrumentUsed( i ) ) ++count;
	}
	return count;
}

bool CFamiTrackerDoc::IsInstrumentUsed(int Index) const
{
	ASSERT(Index >= 0 && Index < MAX_INSTRUMENTS);
	return !(m_pInstruments[Index] == NULL);
}

CInstrument *CFamiTrackerDoc::CreateInstrument(int InstType)
{
	// Creates a new instrument of selected type
	switch (InstType) {
		case INST_2A03: return new CInstrument2A03();
		case INST_VRC6: return new CInstrumentVRC6(); 
		case INST_VRC7: return new CInstrumentVRC7();
		case INST_N163:	return new CInstrumentN163();
		case INST_FDS: return new CInstrumentFDS();
		case INST_S5B: return NULL;//return new CInstrumentS5B();
	}

	return NULL;
}

int CFamiTrackerDoc::FindFreeInstrumentSlot()
{
	// Returns a free instrument slot, or -1 if no free slots exists
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (m_pInstruments[i] == NULL)
			return i;
	}
	return INVALID_INSTRUMENT;
}

int CFamiTrackerDoc::AddInstrument(CInstrument *pInst)
{
	// Add an existing instrument to instrument list

	int Slot = FindFreeInstrumentSlot();

	if (Slot == INVALID_INSTRUMENT)
		return INVALID_INSTRUMENT;

	m_pInstruments[Slot] = pInst;

	SetModifiedFlag();
	UpdateAllViews(NULL, UPDATE_ENTIRE);	// TODO: remove

	return Slot;
}

void CFamiTrackerDoc::AddInstrument(CInstrument *pInst, unsigned int Slot)
{
	ASSERT(Slot < MAX_INSTRUMENTS);

	RemoveInstrument(Slot);

	m_pInstruments[Slot] = pInst;

	SetModifiedFlag();
	UpdateAllViews(NULL, UPDATE_ENTIRE);	// TODO: remove
}

int CFamiTrackerDoc::AddInstrument(const char *Name, int ChipType)
{
	// Adds a new instrument to the module
	int Slot = FindFreeInstrumentSlot();

	if (Slot == INVALID_INSTRUMENT)
		return INVALID_INSTRUMENT;

	m_pInstruments[Slot] = theApp.GetChannelMap()->GetChipInstrument(ChipType);

	if (m_pInstruments[Slot] == NULL) {
#ifdef _DEBUG
		MessageBox(NULL, _T("(TODO) add instrument definitions for this chip"), _T("Stop"), MB_OK);
#endif
		return INVALID_INSTRUMENT;
	}

	m_pInstruments[Slot]->Setup();
	m_pInstruments[Slot]->SetName(Name);

	SetModifiedFlag();
	UpdateAllViews(NULL, UPDATE_ENTIRE);	// TODO: remove

	return Slot;
}

void CFamiTrackerDoc::RemoveInstrument(unsigned int Index)
{
	// Removes an instrument from the module

	ASSERT(Index < MAX_INSTRUMENTS);
	
	if (m_pInstruments[Index] == NULL)
		return;

	m_csInstrument.Lock();

	m_pInstruments[Index]->Release();
	m_pInstruments[Index] = NULL;

	m_csInstrument.Unlock();

	SetModifiedFlag();
   UpdateAllViews(NULL, UPDATE_ENTIRE);	// TODO: remove
}

int CFamiTrackerDoc::CloneInstrument(unsigned int Index)
{
	ASSERT(Index < MAX_INSTRUMENTS);

	if (!IsInstrumentUsed(Index))
		return INVALID_INSTRUMENT;

	int Slot = FindFreeInstrumentSlot();

	if (Slot != INVALID_INSTRUMENT) {
		CInstrument *pInstrument = GetInstrument(Index);
		m_pInstruments[Slot] = pInstrument->Clone();
		pInstrument->Release();
		SetModifiedFlag();
		//UpdateAllViews(NULL, UPDATE_INSTRUMENTS);
	}

	return Slot;
}

void CFamiTrackerDoc::GetInstrumentName(unsigned int Index, char *Name) const
{
	ASSERT(Index < MAX_INSTRUMENTS);
	ASSERT(m_pInstruments[Index] != NULL);

	m_pInstruments[Index]->GetName(Name);
}

void CFamiTrackerDoc::SetInstrumentName(unsigned int Index, const char *Name)
{
	ASSERT(Index < MAX_INSTRUMENTS);
	ASSERT(m_pInstruments[Index] != NULL);

	if (m_pInstruments[Index] != NULL) {
		if (strcmp(m_pInstruments[Index]->GetName(), Name) != 0) {
			m_pInstruments[Index]->SetName(Name);
			SetModifiedFlag();
		}
	}
}

int CFamiTrackerDoc::GetInstrumentType(unsigned int Index) const
{
	ASSERT(Index < MAX_INSTRUMENTS);

	if (!IsInstrumentUsed(Index))
		return INST_NONE;

	return m_pInstruments[Index]->GetType();
}

//
// DPCM samples
//

int CFamiTrackerDoc::GetSampleSize(unsigned int Sample)
{
	ASSERT(Sample < MAX_DSAMPLES);
	return m_DSamples[Sample].SampleSize;
}

char CFamiTrackerDoc::GetSampleData(unsigned int Sample, unsigned int Offset)
{
	ASSERT(Sample < MAX_DSAMPLES);

	if (Offset >= m_DSamples[Sample].SampleSize)
		return 0;

	return m_DSamples[Sample].SampleData[Offset];
}

//
// General document
//

void CFamiTrackerDoc::SetFrameCount(unsigned int Count)
{
	ASSERT(Count <= MAX_FRAMES);

	if (m_pSelectedTrack->GetFrameCount() != Count) {
		m_pSelectedTrack->SetFrameCount(Count);
		SetModifiedFlag();
	}
}

void CFamiTrackerDoc::SetPatternLength(unsigned int Length)
{ 
	ASSERT(Length <= MAX_PATTERN_LENGTH);

	if (m_pSelectedTrack->GetPatternLength() != Length) {
		m_pSelectedTrack->SetPatternLength(Length);
		SetModifiedFlag();
	}
}

void CFamiTrackerDoc::SetSongSpeed(unsigned int Speed)
{
	ASSERT(Speed <= MAX_TEMPO);
	
	if (m_pSelectedTrack->GetSongSpeed() != Speed) {
		m_pSelectedTrack->SetSongSpeed(Speed);
		SetModifiedFlag();
	}
}

void CFamiTrackerDoc::SetSongTempo(unsigned int Tempo)
{
	ASSERT(Tempo <= MAX_TEMPO);

	if (m_pSelectedTrack->GetSongTempo() != Tempo) {
		m_pSelectedTrack->SetSongTempo(Tempo);
		SetModifiedFlag();
	}
}

unsigned int CFamiTrackerDoc::GetEffColumns(int Track, unsigned int Channel) const
{
	ASSERT(Channel < MAX_CHANNELS);
	return m_pTracks[Track]->GetEffectColumnCount(Channel);
}

unsigned int CFamiTrackerDoc::GetEffColumns(unsigned int Channel) const
{
	ASSERT(Channel < MAX_CHANNELS);
	return m_pSelectedTrack->GetEffectColumnCount(Channel);
}

void CFamiTrackerDoc::SetEffColumns(unsigned int Channel, unsigned int Columns)
{
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Columns < MAX_EFFECT_COLUMNS);

	GetChannel(Channel)->SetColumnCount(Columns);
	m_pSelectedTrack->SetEffectColumnCount(Channel, Columns);

	SetModifiedFlag();
}

void CFamiTrackerDoc::SetEngineSpeed(unsigned int Speed)
{
	ASSERT(Speed <= 800); // hardcoded at the moment, TODO: fix this
	ASSERT(Speed >= 10 || Speed == 0);

	m_iEngineSpeed = Speed;
	SetModifiedFlag();
}

void CFamiTrackerDoc::SetMachine(unsigned int Machine)
{
	ASSERT(Machine == PAL || Machine == NTSC);
	m_iMachine = Machine;
	SetModifiedFlag();
}

unsigned int CFamiTrackerDoc::GetPatternAtFrame(int Track, unsigned int Frame, unsigned int Channel) const
{
	ASSERT(Frame < MAX_FRAMES && Channel < MAX_CHANNELS);
	return m_pTracks[Track]->GetFramePattern(Frame, Channel);
}

unsigned int CFamiTrackerDoc::GetPatternAtFrame(unsigned int Frame, unsigned int Channel) const
{
	ASSERT(Frame < MAX_FRAMES && Channel < MAX_CHANNELS);
	return m_pSelectedTrack->GetFramePattern(Frame, Channel);
}

void CFamiTrackerDoc::SetPatternAtFrame(unsigned int Frame, unsigned int Channel, unsigned int Pattern)
{
	ASSERT(Frame < MAX_FRAMES && Channel < MAX_CHANNELS && Pattern < MAX_PATTERN);
	m_pSelectedTrack->SetFramePattern(Frame, Channel, Pattern);
//	SetModifiedFlag();	// TODO removed for a reason?
}

unsigned int CFamiTrackerDoc::GetFrameRate(void) const
{
	if (m_iEngineSpeed == 0)
		return (m_iMachine == NTSC) ? CAPU::FRAME_RATE_NTSC : CAPU::FRAME_RATE_PAL;
	
	return m_iEngineSpeed;
}

//// Pattern functions ////////////////////////////////////////////////////////////////////////////////

void CFamiTrackerDoc::IncreasePattern(unsigned int Frame, unsigned int Channel, int Count)
{
	ASSERT(Frame < MAX_FRAMES && Channel < MAX_CHANNELS);

	int Current = m_pSelectedTrack->GetFramePattern(Frame, Channel);

	// Selects the next channel pattern
	if ((Current + Count) < (MAX_PATTERN - 1)) {
		m_pSelectedTrack->SetFramePattern(Frame, Channel, Current + Count);
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
	else {
		m_pSelectedTrack->SetFramePattern(Frame, Channel, MAX_PATTERN - 1);
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::DecreasePattern(unsigned int Frame, unsigned int Channel, int Count)
{
	ASSERT(Frame < MAX_FRAMES && Channel < MAX_CHANNELS);

	int Current = m_pSelectedTrack->GetFramePattern(Frame, Channel);

	// Selects the previous channel pattern
	if (Current > Count) {
		m_pSelectedTrack->SetFramePattern(Frame, Channel, Current - Count);
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
	else {
		m_pSelectedTrack->SetFramePattern(Frame, Channel, 0);
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::IncreaseInstrument(unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	unsigned int Inst = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Instrument;

	if (Inst < MAX_INSTRUMENTS) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Instrument = Inst + 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::DecreaseInstrument(unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	unsigned int Inst = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Instrument;

	if (Inst > 0) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Instrument = Inst - 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::IncreaseVolume(unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	unsigned int Vol = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Vol;

	if (Vol < 16) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Vol = Vol + 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::DecreaseVolume(unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	unsigned int Vol = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Vol;

	if (Vol > 1) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->Vol = Vol - 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::IncreaseEffect(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Index)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);
	ASSERT(Index < MAX_EFFECT_COLUMNS);

	unsigned int Effect = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffParam[Index];
	
	if (Effect < 256) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffParam[Index] = Effect + 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::DecreaseEffect(unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Index)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);
	ASSERT(Index < MAX_EFFECT_COLUMNS);

	unsigned int Effect = m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffParam[Index];

	if (Effect > 0) {
		m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffParam[Index] = Effect - 1;
		SetModifiedFlag();
		UpdateAllViews(NULL);	// TODO: remove
	}
}

void CFamiTrackerDoc::SetNoteData(unsigned int Frame, unsigned int Channel, unsigned int Row, stChanNote *Data)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	// Get notes from the pattern
	memcpy(m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row), Data, sizeof(stChanNote));
	SetModifiedFlag();
}

void CFamiTrackerDoc::GetNoteData(unsigned int Frame, unsigned int Channel, unsigned int Row, stChanNote *Data) const
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);
	unsigned int Pattern = GET_PATTERN(Frame, Channel);
	// Sets the notes of the pattern
	memcpy(Data, m_pSelectedTrack->GetPatternData(Channel, Pattern, Row), sizeof(stChanNote));
}

void CFamiTrackerDoc::SetDataAtPattern(unsigned int Track, unsigned int Pattern, unsigned int Channel, unsigned int Row, stChanNote *Data)
{
	ASSERT(Track < MAX_TRACKS);
	ASSERT(Pattern < MAX_PATTERN);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	// Set a note to a direct pattern
	memcpy(m_pTracks[Track]->GetPatternData(Channel, Pattern, Row), Data, sizeof(stChanNote));
	SetModifiedFlag();
}

void CFamiTrackerDoc::GetDataAtPattern(unsigned int Track, unsigned int Pattern, unsigned int Channel, unsigned int Row, stChanNote *Data) const
{
	ASSERT(Track < MAX_TRACKS);
	ASSERT(Pattern < MAX_PATTERN);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	// Get note from a direct pattern
	memcpy(Data, m_pTracks[Track]->GetPatternData(Channel, Pattern, Row), sizeof(stChanNote));
}

unsigned int CFamiTrackerDoc::GetNoteEffectType(unsigned int Frame, unsigned int Channel, unsigned int Row, int Index) const
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);
	ASSERT(Index < MAX_EFFECT_COLUMNS);

	return m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffNumber[Index];
}

unsigned int CFamiTrackerDoc::GetNoteEffectParam(unsigned int Frame, unsigned int Channel, unsigned int Row, int Index) const
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);
	ASSERT(Index < MAX_EFFECT_COLUMNS);

	return m_pSelectedTrack->GetPatternData(Channel, GET_PATTERN(Frame, Channel), Row)->EffParam[Index];
}

bool CFamiTrackerDoc::InsertRow(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	stChanNote Note;

	for (int n = 0; n < MAX_EFFECT_COLUMNS; n++) {
		Note.EffNumber[n] = 0;
		Note.EffParam[n] = 0;
	}

	Note.Note		= 0;
	Note.Octave		= 0;
	Note.Instrument	= MAX_INSTRUMENTS;
	Note.Vol		= 0x10;

	for (unsigned int i = m_pSelectedTrack->GetPatternLength() - 1; i > Row; i--) {
		memcpy(m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), i), 
			m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), i - 1), 
			sizeof(stChanNote));
	}

	*m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), Row) = Note;

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::DeleteNote(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Column)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	stChanNote *Note = m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), Row);

	if (Column == C_NOTE) {
		Note->Note = 0;
		Note->Octave = 0;
		Note->Instrument = MAX_INSTRUMENTS;
		Note->Vol = 0x10;
	}
	else if (Column == C_INSTRUMENT1 || Column == C_INSTRUMENT2) {
		Note->Instrument = MAX_INSTRUMENTS;
	}
	else if (Column == C_VOLUME) {
		Note->Vol = 0x10;
	}
	else if (Column == C_EFF_NUM || Column == C_EFF_PARAM1 || Column == C_EFF_PARAM2) {
		Note->EffNumber[0]	= 0;
		Note->EffParam[0]	= 0;
	}
	else if (Column == C_EFF2_NUM || Column == C_EFF2_PARAM1 || Column == C_EFF2_PARAM2) {
		Note->EffNumber[1]	= 0;
		Note->EffParam[1]	= 0;
	}
	else if (Column == C_EFF3_NUM || Column == C_EFF3_PARAM1 || Column == C_EFF3_PARAM2) {
		Note->EffNumber[2]	= 0;
		Note->EffParam[2]	= 0;
	}
	else if (Column == C_EFF4_NUM || Column == C_EFF4_PARAM1 || Column == C_EFF4_PARAM2) {
		Note->EffNumber[3]	= 0;
		Note->EffParam[3]	= 0;
	}
	
	SetModifiedFlag();

	return true;
}

void CFamiTrackerDoc::ClearPattern(unsigned int Frame, unsigned int Channel)
{
	// Clear entire pattern
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);

	for (int i = 0; i < MAX_PATTERN_LENGTH; ++i) {
		stChanNote *pNote = m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), i);
		pNote->Note = 0;
		pNote->Octave = 0;
		pNote->Instrument = MAX_INSTRUMENTS;
		pNote->Vol = 0x10;
		for (int j = 0; j < MAX_EFFECT_COLUMNS; ++j) {
			pNote->EffNumber[j] = 0;
			pNote->EffParam[j] = 0;
		}
	}

	SetModifiedFlag();
}

bool CFamiTrackerDoc::ClearRow(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	stChanNote *pNote = m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), Row);

	pNote->Note = 0;
	pNote->Octave = 0;
	pNote->Instrument = MAX_INSTRUMENTS;
	pNote->Vol = 0x10;

	for (int i = 0; i < MAX_EFFECT_COLUMNS; ++i) {
		pNote->EffNumber[i] = EF_NONE;
		pNote->EffParam[i] = 0;
	}
	
	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::ClearRowField(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row, unsigned int Column)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	stChanNote *pNote = m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), Row);

	switch (Column) {
		case C_NOTE:			// Note
			pNote->Note = 0;
			pNote->Octave = 0;
			pNote->Instrument = MAX_INSTRUMENTS;	// Fix the old behaviour
			pNote->Vol = VOLUME_EMPTY;
			break;
		case C_INSTRUMENT1:		// Instrument
		case C_INSTRUMENT2:
			pNote->Instrument = MAX_INSTRUMENTS;
			break;
		case C_VOLUME:			// Volume
			pNote->Vol = VOLUME_EMPTY;
			break;
		case C_EFF_NUM:			// Effect 1
		case C_EFF_PARAM1:
		case C_EFF_PARAM2:
			pNote->EffNumber[0] = EF_NONE;
			pNote->EffParam[0] = 0;
			break;
		case C_EFF2_NUM:		// Effect 2
		case C_EFF2_PARAM1:
		case C_EFF2_PARAM2:
			pNote->EffNumber[1] = EF_NONE;
			pNote->EffParam[1] = 0;
			break;
		case C_EFF3_NUM:		// Effect 3
		case C_EFF3_PARAM1:
		case C_EFF3_PARAM2:
			pNote->EffNumber[2] = EF_NONE;
			pNote->EffParam[2] = 0;
			break;
		case C_EFF4_NUM:		// Effect 4
		case C_EFF4_PARAM1:
		case C_EFF4_PARAM2:
			pNote->EffNumber[3] = EF_NONE;
			pNote->EffParam[3] = 0;
			break;
	}
	
	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::RemoveNote(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	ASSERT(Frame < MAX_FRAMES);
	ASSERT(Channel < MAX_CHANNELS);
	ASSERT(Row < MAX_PATTERN_LENGTH);

	stChanNote Note;

	for (int n = 0; n < MAX_EFFECT_COLUMNS; n++) {
		Note.EffNumber[n] = 0;
		Note.EffParam[n] = 0;
	}

	Note.Note = 0;
	Note.Octave = 0;
	Note.Instrument = MAX_INSTRUMENTS;
	Note.Vol = 0x10;

	for (unsigned int i = Row - 1; i < (m_pSelectedTrack->GetPatternLength() - 1); i++) {
		memcpy(m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), i), 
			m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), i + 1),
			sizeof(stChanNote));
	}

	*m_pSelectedTrack->GetPatternData(Channel, m_pSelectedTrack->GetFramePattern(Frame, Channel), m_pSelectedTrack->GetPatternLength() - 1) = Note;

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::PullUp(int Track, unsigned int Frame, unsigned int Channel, unsigned int Row)
{
	int PatternLen = GetPatternLength(Track);
	stChanNote Data;

	for (int i = Row; i < PatternLen - 1; ++i) {
		GetNoteData(Frame, Channel, i + 1, &Data);
		SetNoteData(Frame, Channel, i, &Data);
	}

	// Last note on pattern
	ClearRow(Track, Frame, Channel, PatternLen - 1);

	SetModifiedFlag();

	return true;
}

void CFamiTrackerDoc::CopyPattern(int Track, int Target, int Source, int Channel)
{
	// Copy one pattern to another

	int PatternLen = GetPatternLength(Track);
	stChanNote Data;

	for (int i = 0; i < PatternLen; ++i) {
		GetDataAtPattern(Track, Source, Channel, i, &Data);
		SetDataAtPattern(Track, Target, Channel, i, &Data);
	}

	SetModifiedFlag();
}

//// Frame functions //////////////////////////////////////////////////////////////////////////////////

bool CFamiTrackerDoc::InsertFrame(int Pos)
{
	ASSERT(Pos < MAX_FRAMES);

	int FrameCount = GetFrameCount();
	int Channels = GetAvailableChannels();

	if (FrameCount == MAX_FRAMES)
		return false;

	SetFrameCount(FrameCount + 1);

	for (int i = FrameCount; i > Pos; --i) {
		for (int j = 0; j < Channels; ++j) {
			SetPatternAtFrame(i, j, GetPatternAtFrame(i - 1, j));
		}
	}

	// Select free patterns 
	for (int i = 0; i < Channels; ++i) {
		SetPatternAtFrame(Pos, i, GetFirstFreePattern(i));
	}

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::RemoveFrame(int Pos)
{
	ASSERT(Pos < MAX_FRAMES);

	int FrameCount = GetFrameCount();
	int Channels = GetAvailableChannels();

	for (int i = 0; i < Channels; ++i) {
		SetPatternAtFrame(Pos, i, 0);
	}

	if (FrameCount == 1)
		return false;

	for (int i = Pos; i < FrameCount - 1; ++i) {
		for (int j = 0; j < Channels; ++j) {
			SetPatternAtFrame(i, j, GetPatternAtFrame(i + 1, j));
		}
	}

	SetFrameCount(FrameCount - 1);

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::MoveFrameDown(int Pos)
{
	int Channels = GetAvailableChannels();

	if (Pos == (GetFrameCount() - 1))
		return false;

	for (int i = 0; i < Channels; ++i) {
		int Pattern = GetPatternAtFrame(Pos, i);
		SetPatternAtFrame(Pos, i, GetPatternAtFrame(Pos + 1, i));
		SetPatternAtFrame(Pos + 1, i, Pattern);
	}

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::MoveFrameUp(int Pos)
{
	int Channels = GetAvailableChannels();

	if (Pos == 0)
		return false;

	for (int i = 0; i < Channels; ++i) {
		int Pattern = GetPatternAtFrame(Pos, i);
		SetPatternAtFrame(Pos, i, GetPatternAtFrame(Pos - 1, i));
		SetPatternAtFrame(Pos - 1, i, Pattern);
	}

	SetModifiedFlag();

	return true;
}

void CFamiTrackerDoc::DeleteFrames(int Pos, int Count)
{
	ASSERT(Pos < MAX_FRAMES);

	for (int i = 0; i < Count; ++i) {
		RemoveFrame(Pos);
	}

	SetModifiedFlag();
}

bool CFamiTrackerDoc::DuplicateFrame(int Pos)
{
	// Create a copy of selected frame
	ASSERT(Pos < MAX_FRAMES);

	int Frames = GetFrameCount();
	int Channels = GetAvailableChannels();

	if (Frames == MAX_FRAMES)
		return false;

	SetFrameCount(Frames + 1);

	for (int i = Frames; i > (Pos + 1); --i) {
		for (int j = 0; j < Channels; ++j) {
			SetPatternAtFrame(i, j, GetPatternAtFrame(i - 1, j));
		}
	}

	for (int i = 0; i < Channels; ++i) {
		SetPatternAtFrame(Pos + 1, i, GetPatternAtFrame(Pos, i));
	}

	SetModifiedFlag();

	return true;
}

bool CFamiTrackerDoc::DuplicatePatterns(int Pos)
{
	// Create a copy of selected frame including patterns
	int Frames = GetFrameCount();
	int Channels = GetAvailableChannels();

	// insert new frame with next free pattern numbers
	if (!InsertFrame(Pos))
		return false;

	// copy old patterns into new
	for (int i = 0; i < Channels; ++i) {
		for(unsigned int j = 0; j < MAX_PATTERN_LENGTH; j++) {
			stChanNote note;
			GetNoteData(Pos - 1, i, j, &note);
			SetNoteData(Pos, i, j, &note);
		}
	}

	SetModifiedFlag();

	return true;
}

//// Track functions //////////////////////////////////////////////////////////////////////////////////

void CFamiTrackerDoc::SelectTrack(unsigned int Track)
{
	// TODO: remove 
	// (selected track should be a part of the view and not the document)
	ASSERT(Track < MAX_TRACKS);
	SwitchToTrack(Track);
	UpdateAllViews(0, CHANGED_TRACK);	// TODO: remove
}

void CFamiTrackerDoc::SwitchToTrack(unsigned int Track)
{
	// Select a new track, initialize if it wasn't
	// TODO: should this fail if track didn't exist?
	m_iTrack = Track;
	//AllocateTrack(Track);
	m_pSelectedTrack = GetTrack(Track);// m_pTracks[Track];
	// TODO: The document object shouldn't be aware of run-time states
}

char *CFamiTrackerDoc::GetTrackTitle(unsigned int Track) const
{
	return (char*)m_sTrackNames[Track].GetString();
}

int CFamiTrackerDoc::AddTrack()
{
	// Add new track. Return -1 on failure, or added track number otherwise

	int NewTrack = m_iTracks;

	if (NewTrack >= MAX_TRACKS)
		return -1;

	AllocateTrack(NewTrack);

	++m_iTracks;

	SetModifiedFlag();
	UpdateAllViews(NULL, CHANGED_TRACKCOUNT);	// TODO: remove

	return NewTrack;
}

void CFamiTrackerDoc::RemoveTrack(unsigned int Track)
{
	ASSERT(m_iTracks > 1);
	ASSERT(m_pTracks[Track] != NULL);

	delete m_pTracks[Track];

	// Move down all other tracks
	for (unsigned int i = Track; i < m_iTracks - 1; ++i) {
		m_sTrackNames[i] = m_sTrackNames[i + 1];
		m_pTracks[i] = m_pTracks[i + 1];
	}

	m_pTracks[m_iTracks - 1] = NULL;

	--m_iTracks;

	if (m_iTrack >= m_iTracks)
		m_iTrack = m_iTracks - 1;	// Last track was removed

	SwitchToTrack(m_iTrack);

	SetModifiedFlag();
	UpdateAllViews(NULL, CHANGED_TRACKCOUNT);	// TODO: remove
}

void CFamiTrackerDoc::SetTrackTitle(unsigned int Track, CString Title)
{
	if (m_sTrackNames[Track] == Title)
		return;

	m_sTrackNames[Track] = Title;

	SetModifiedFlag();
	UpdateAllViews(NULL, CHANGED_TRACKCOUNT);	// TODO: remove
}

void CFamiTrackerDoc::MoveTrackUp(unsigned int Track)
{
	ASSERT(Track > 0);

	SwapTracks(Track, Track - 1);
/*
	CString Temp = m_sTrackNames[Track];
	m_sTrackNames[Track] = m_sTrackNames[Track - 1];
	m_sTrackNames[Track - 1] = Temp;

	CPatternData *pTemp = m_pTracks[Track];
	m_pTracks[Track] = m_pTracks[Track - 1];
	m_pTracks[Track - 1] = pTemp;
*/
	SetModifiedFlag();
	UpdateAllViews(NULL, CHANGED_TRACKCOUNT);	// TODO: remove
}

void CFamiTrackerDoc::MoveTrackDown(unsigned int Track)
{
	ASSERT(Track < MAX_TRACKS);

	SwapTracks(Track, Track + 1);
/*
	CString Temp = m_sTrackNames[Track];
	m_sTrackNames[Track] = m_sTrackNames[Track + 1];
	m_sTrackNames[Track + 1] = Temp;

	CPatternData *pTemp = m_pTracks[Track];
	m_pTracks[Track] = m_pTracks[Track + 1];
	m_pTracks[Track + 1] = pTemp;
*/
	SetModifiedFlag();
	UpdateAllViews(NULL, CHANGED_TRACKCOUNT);	// TODO: remove
}

void CFamiTrackerDoc::SwapTracks(unsigned int Track1, unsigned int Track2)
{
	CString Temp = m_sTrackNames[Track1];
	m_sTrackNames[Track1] = m_sTrackNames[Track2];
	m_sTrackNames[Track2] = Temp;

	CPatternData *pTemp = m_pTracks[Track1];
	m_pTracks[Track1] = m_pTracks[Track2];
	m_pTracks[Track2] = pTemp;
}

void CFamiTrackerDoc::AllocateTrack(unsigned int Track)
{
	// Allocate a new song if not already done
	if (m_pTracks[Track] == NULL) {
		int Tempo = (m_iMachine == NTSC) ? DEFAULT_TEMPO_NTSC : DEFAULT_TEMPO_PAL;
		m_pTracks[Track] = new CPatternData(DEFAULT_ROW_COUNT, DEFAULT_SPEED, Tempo);
		m_sTrackNames[Track] = DEFAULT_TRACK_NAME;
	}
}

CPatternData* CFamiTrackerDoc::GetTrack(unsigned int Track)
{
	ASSERT(Track < MAX_TRACKS);
	// Ensure track is allocated
	AllocateTrack(Track);
	return m_pTracks[Track];
}

unsigned int CFamiTrackerDoc::GetTrackCount() const
{
	return m_iTracks;
}

unsigned int CFamiTrackerDoc::GetSelectedTrack() const
{
	// TODO: remove
	return m_iTrack;
}

void CFamiTrackerDoc::SelectExpansionChip(unsigned char Chip)
{
	// Disallow unimplemented chips
#ifndef _DEBUG
	if (Chip & SNDCHIP_S5B)
		Chip = 0;
#endif

	// Complete sound chip setup
	SetupChannels(Chip);
	ApplyExpansionChip();
}

void CFamiTrackerDoc::SetupChannels(unsigned char Chip)
{
	// This will select a chip in the sound emulator

	if (Chip != SNDCHIP_NONE) {
		// Do not allow expansion chips in PAL mode
		SetMachine(NTSC);
	}

	// Store the chip
	m_iExpansionChip = Chip;

	// Register the channels
	theApp.GetSoundGenerator()->RegisterChannels(Chip, this); 

	m_iChannelsAvailable = GetChannelCount();

	if (Chip & SNDCHIP_N163) {
		m_iChannelsAvailable -= (8 - m_iNamcoChannels);
	}

	// Must call ApplyExpansionChip after this
}

void CFamiTrackerDoc::ApplyExpansionChip()
{
	// Tell the sound emulator to switch expansion chip
	theApp.GetSoundGenerator()->SelectChip(m_iExpansionChip);

	// Change period tables
	theApp.GetSoundGenerator()->LoadMachineSettings(m_iMachine, m_iEngineSpeed, m_iNamcoChannels);

	SetModifiedFlag();
}

bool CFamiTrackerDoc::ExpansionEnabled(int Chip) const
{
	// Returns true if a specified chip is enabled
	return (GetExpansionChip() & Chip) == Chip; 
}

void CFamiTrackerDoc::SetNamcoChannels(int Channels)
{
	ASSERT(Channels >= 1 && Channels <= 8);
	m_iNamcoChannels = Channels;
}

int CFamiTrackerDoc::GetNamcoChannels() const
{
	return m_iNamcoChannels;
}

// Instrument functions

void CFamiTrackerDoc::SaveInstrument(unsigned int Instrument, CString FileName)
{
	// Saves an instrument to a file
	CInstrumentFile file(FileName, CFile::modeCreate | CFile::modeWrite);
	CInstrument *pInstrument = GetInstrument(Instrument);

	ASSERT(pInstrument != NULL);
	
	if (file.m_hFile == CFile::hFileNull) {
		AfxMessageBox(IDS_INVALID_INST_FILE, MB_ICONERROR);
		return;
	}

	// Write header
	file.Write(INST_HEADER, (UINT)strlen(INST_HEADER));
	file.Write(INST_VERSION, (UINT)strlen(INST_VERSION));

	// Write type
	file.WriteChar(pInstrument->GetType());

	// Write name
	char Name[256];
	pInstrument->GetName(Name);
	int NameLen = (int)strlen(Name);
	file.WriteInt(NameLen);
	file.Write(Name, NameLen);

	// Write instrument data
	pInstrument->SaveFile(&file, this);
	pInstrument->Release();

	file.Close();
}

int CFamiTrackerDoc::LoadInstrument(CString FileName)
{
	// Loads an instrument from file
	int Slot = FindFreeInstrumentSlot();

	if (Slot == INVALID_INSTRUMENT) {
		AfxMessageBox(IDS_INST_LIMIT, MB_ICONERROR);
		return INVALID_INSTRUMENT;
	}

	// Open file
	CInstrumentFile file(FileName, CFile::modeRead);

	if (file.m_hFile == CFile::hFileNull) {
		AfxMessageBox(IDS_INVALID_INST_FILE, MB_ICONERROR);
		file.Close();
		return INVALID_INSTRUMENT;
	}

	// Signature
	char Text[256];
	file.Read(Text, (UINT)strlen(INST_HEADER));
	Text[strlen(INST_HEADER)] = 0;

	if (strcmp(Text, INST_HEADER) != 0) {
		AfxMessageBox(IDS_INSTRUMENT_FILE_FAIL, MB_ICONERROR);
		file.Close();
		return INVALID_INSTRUMENT;
	}

	// Version
	file.Read(Text, (UINT)strlen(INST_VERSION));

	int iInstMaj, iInstMin;
	sscanf(Text, "%i.%i", &iInstMaj, &iInstMin);
	int iInstVer = iInstMaj * 10 + iInstMin;
	
	sscanf(INST_VERSION, "%i.%i", &iInstMaj, &iInstMin);
	int iCurrentVer = iInstMaj * 10 + iInstMin;

	if (iInstVer > iCurrentVer) {
		AfxMessageBox(IDS_INST_VERSION_UNSUPPORTED, MB_OK);
		file.Close();
		return INVALID_INSTRUMENT;
	}

	m_csDocumentLock.Lock();

	// Type
	char InstType = file.ReadChar();

	if (!InstType)
		InstType = INST_2A03;

	CInstrument *pInstrument = CreateInstrument(InstType);
	m_pInstruments[Slot] = pInstrument;

	// Name
	unsigned int NameLen = file.ReadInt();

	if (NameLen >= 256) {
		m_csDocumentLock.Unlock();
		AfxMessageBox(IDS_INST_FILE_ERROR, MB_OK);
		m_pInstruments[Slot] = NULL;
		SAFE_RELEASE(pInstrument);
		file.Close();
		return INVALID_INSTRUMENT;
	}

	file.Read(Text, NameLen);
	Text[NameLen] = 0;

	pInstrument->SetName(Text);

	if (!pInstrument->LoadFile(&file, iInstVer, this)) {
		m_csDocumentLock.Unlock();
		AfxMessageBox(IDS_INST_FILE_ERROR, MB_OK);
		m_pInstruments[Slot] = NULL;
		SAFE_RELEASE(pInstrument);
		file.Close();
		return INVALID_INSTRUMENT;
	}

	m_csDocumentLock.Unlock();

	file.Close();

	return Slot;
}


void CFamiTrackerDoc::ConvertSequence(stSequence *OldSequence, CSequence *NewSequence, int Type)
{
	// This function is used to convert old version sequences (used by older file versions)
	// to the current version

	if (OldSequence->Count > 0 && OldSequence->Count < MAX_SEQUENCE_ITEMS) {

		// Save a pointer to this
		int iLoopPoint = -1;
		int iLength = 0;
		int ValPtr = 0;

		// Store the sequence
		int Count = OldSequence->Count;

		for (int k = 0; k < Count; ++k) {
			int Value	= OldSequence->Value[k];
			int Length	= OldSequence->Length[k];

			if (Length < 0) {
				iLoopPoint = 0;
				for (int l = signed(OldSequence->Count) + Length - 1; l < signed(OldSequence->Count) - 1; l++)
					iLoopPoint += (OldSequence->Length[l] + 1);
			}
			else {
				for (int l = 0; l < Length + 1; l++) {
					if ((Type == SEQ_PITCH || Type == SEQ_HIPITCH) && l > 0)
						NewSequence->SetItem(ValPtr++, 0);
					else
						NewSequence->SetItem(ValPtr++, (unsigned char)Value);
					iLength++;
				}
			}
		}

		if (iLoopPoint != -1) {
			if (iLoopPoint > iLength)
				iLoopPoint = iLength;
			iLoopPoint = iLength - iLoopPoint;
		}

		NewSequence->SetItemCount(ValPtr);
		NewSequence->SetLoopPoint(iLoopPoint);
	}
}

int CFamiTrackerDoc::GetFirstFreePattern(int Channel) const
{
	for (int i = 0; i < MAX_PATTERN; ++i) {
		if (!m_pSelectedTrack->IsPatternInUse(Channel, i) && m_pSelectedTrack->IsPatternEmpty(Channel, i))
			return i;

		// Changed v0.3.7, patterns can't be used earlier
		//if (m_pSelectedTrack->IsPatternFree(Channel, i))
		//	return i;
	}

	return 0;
}

bool CFamiTrackerDoc::IsPatternEmpty(unsigned int Channel, unsigned int Pattern) const
{
	return m_pSelectedTrack->IsPatternEmpty(Channel, Pattern);
}

void CFamiTrackerDoc::ClearPatterns(int Track)
{
	m_pTracks[Track]->ClearEverything();
}

// Channel interface, these functions must be synchronized!!!

int CFamiTrackerDoc::GetChannelType(int Channel) const
{
	ASSERT(m_iRegisteredChannels != 0);
	ASSERT(Channel < m_iRegisteredChannels);
	return m_iChannelTypes[Channel];
}

int CFamiTrackerDoc::GetChipType(int Channel) const
{
	ASSERT(m_iRegisteredChannels != 0);
	ASSERT(Channel < m_iRegisteredChannels);
	return m_pChannels[Channel]->GetChip();
}

int CFamiTrackerDoc::GetChannelCount() const
{
	return m_iRegisteredChannels;
}

void CFamiTrackerDoc::ResetChannels()
{
	// Clears all channels from the document
	m_iRegisteredChannels = 0;
}

void CFamiTrackerDoc::RegisterChannel(CTrackerChannel *pChannel, int ChannelType, int ChipType)
{
	// Adds a channel to the document
	m_pChannels[m_iRegisteredChannels] = pChannel;
	m_iChannelTypes[m_iRegisteredChannels] = ChannelType;
	m_iChannelChip[m_iRegisteredChannels] = ChipType;
	m_iRegisteredChannels++;
}

CTrackerChannel *CFamiTrackerDoc::GetChannel(int Index) const
{
	ASSERT(m_iRegisteredChannels != 0 && Index < m_iRegisteredChannels);
	ASSERT(m_pChannels[Index] != NULL);
	return m_pChannels[Index];
}

// DPCM samples

int CFamiTrackerDoc::GetSampleCount() const
{
	int Count = 0;
	for (int i = 0; i < MAX_DSAMPLES; ++i) {
		if (m_DSamples[i].SampleSize > 0)
			++Count;
	}
	return Count;
}

// Vibrato functions

vibrato_t CFamiTrackerDoc::GetVibratoStyle() const
{
	return m_iVibratoStyle;
}

void CFamiTrackerDoc::SetVibratoStyle(vibrato_t Style)
{
	m_iVibratoStyle = Style;
	theApp.GetSoundGenerator()->SetupVibratoTable(Style);
}

// Linear pitch slides

bool CFamiTrackerDoc::GetLinearPitch() const
{
	return m_bLinearPitch;
}

void CFamiTrackerDoc::SetLinearPitch(bool Enable)
{
	m_bLinearPitch = Enable;
}

// Attributes

CString CFamiTrackerDoc::GetFileTitle() const 
{
	// Return file name without extension
	CString FileName = GetTitle();

	// Remove extension
	if (FileName.Right(4).CompareNoCase(_T(".ftm")) == 0)
		return FileName.Left(FileName.GetLength() - 4);

	return FileName;
}

bool CFamiTrackerDoc::IsFileLoaded() const
{
	return m_bFileLoaded;
}

bool CFamiTrackerDoc::HasLastLoadFailed() const
{
	return m_bFileLoadFailed;
}

#if 0

// Auto-save (experimental)

void CFamiTrackerDoc::SetupAutoSave()
{
	TCHAR TempPath[MAX_PATH], TempFile[MAX_PATH];

	GetTempPath(MAX_PATH, TempPath);
	GetTempFileName(TempPath, _T("Aut"), 21587, TempFile);

	// Check if file exists
	CFile file;
	if (file.Open(TempFile, CFile::modeRead)) {
		file.Close();
		if (AfxMessageBox(_T("It might be possible to recover last document, do you want to try?"), MB_YESNO) == IDYES) {
			OpenDocument(TempFile);
			SelectExpansionChip(m_iExpansionChip);
		}
		else {
			DeleteFile(TempFile);
		}
	}

	TRACE("Doc: Allocated file for auto save: ");
	TRACE(TempFile);
	TRACE("\n");

	m_sAutoSaveFile = TempFile;
}

void CFamiTrackerDoc::ClearAutoSave()
{
	if (m_sAutoSaveFile.GetLength() == 0)
		return;

	DeleteFile(m_sAutoSaveFile);

	m_sAutoSaveFile = _T("");
	m_iAutoSaveCounter = 0;

	TRACE("Doc: Removed auto save file\n");
}

void CFamiTrackerDoc::AutoSave()
{
	// Autosave
	if (!m_iAutoSaveCounter || !m_bFileLoaded || m_sAutoSaveFile.GetLength() == 0)
		return;

	m_iAutoSaveCounter--;

	if (m_iAutoSaveCounter == 0) {
		TRACE("Doc: Performing auto save\n");
		SaveDocument(m_sAutoSaveFile);
	}
}

#endif

int CFamiTrackerDoc::DeepCloneInstrument(unsigned int Index) 
{
	int Slot = CloneInstrument(Index);

	if (Slot != INVALID_INSTRUMENT) {
		CInstrument *newInst = m_pInstruments[Slot];
		switch (newInst->GetType()) {
			case INST_2A03:
				{
					CInstrument2A03 *pInstrument = static_cast<CInstrument2A03*>(newInst);
					for(unsigned int i = 0; i < CInstrument2A03::SEQUENCE_COUNT; i++) {
						int freeSeq = this->GetFreeSequence(i);
						int oldSeq = pInstrument->GetSeqIndex(i);
						if( pInstrument->GetSeqEnable(i) ) {
							this->GetSequence(freeSeq, i)->Copy( this->GetSequence(oldSeq, i) );
						}
						pInstrument->SetSeqIndex(i, freeSeq);
					}
				}
				break;
			case INST_VRC6:
				{
					CInstrumentVRC6 *pInstrument = static_cast<CInstrumentVRC6*>(newInst);
					for(unsigned int i = 0; i < CInstrumentVRC6::SEQUENCE_COUNT; i++) {
						int freeSeq = this->GetFreeSequenceVRC6(i);
						int oldSeq = pInstrument->GetSeqIndex(i);
						if( pInstrument->GetSeqEnable(i) ) {
							this->GetSequenceVRC6(freeSeq, i)->Copy( this->GetSequenceVRC6(oldSeq, i) );
						}
						pInstrument->SetSeqIndex(i, freeSeq);
					}
				}
				break;
			case INST_N163:
				{
					CInstrumentN163 *pInstrument = static_cast<CInstrumentN163*>(newInst);
					for(unsigned int i = 0; i < CInstrumentN163::SEQUENCE_COUNT; i++) {
						int freeSeq = this->GetFreeSequenceN163(i);
						int oldSeq = pInstrument->GetSeqIndex(i);
						if( pInstrument->GetSeqEnable(i) ) {
							this->GetSequenceN163(freeSeq, i)->Copy( this->GetSequenceN163(oldSeq, i) );
						}
						pInstrument->SetSeqIndex(i, freeSeq);
					}
				}
				break;
		}

	}

	return Slot;
}

void CFamiTrackerDoc::SetComment(CString &comment, bool ShowOnLoad)
{
	m_strComment = comment;
	m_bDisplayComment = ShowOnLoad;
	SetModifiedFlag();
}

CString CFamiTrackerDoc::GetComment() const
{
	return m_strComment;
}

bool CFamiTrackerDoc::ShowCommentOnOpen() const
{
	return m_bDisplayComment;
}

void CFamiTrackerDoc::SetSpeedSplitPoint(int SplitPoint)
{
	m_iSpeedSplitPoint = SplitPoint;
}

int CFamiTrackerDoc::GetSpeedSplitPoint() const
{
	return m_iSpeedSplitPoint;
}

void CFamiTrackerDoc::SetHighlight(int First, int Second)
{
	m_iFirstHighlight = First;
	m_iSecondHighlight = Second;
}

int CFamiTrackerDoc::GetFirstHighlight() const
{
	return m_iFirstHighlight;
}

int CFamiTrackerDoc::GetSecondHighlight() const
{
	return m_iSecondHighlight;
}

int CFamiTrackerDoc::ScanActualLength(int Track, int Count) const 
{
	// Return number for frames played for a certain number of loops

	int FrameVisited[MAX_FRAMES];
	int JumpTo = -1;
	int SkipTo = -1;
	int FirstLoop = 0;
	int SecondLoop = 0;
	int Frame = 0;
	bool bScanning = true;
	int FrameCount = GetFrameCount(Track);

	memset(FrameVisited, 0, sizeof(int) * MAX_FRAMES);

	while (bScanning) {

		JumpTo = -1;
		SkipTo = -1;

		for (unsigned k = 0; k < GetPatternLength(Track) && JumpTo == -1 && SkipTo == -1; ++k) {
			for (int j = 0; j < GetChannelCount(); ++j) {
				stChanNote Note;
				GetNoteData(Frame, j, k, &Note);
				for (unsigned l = 0; l < GetEffColumns(Track, j) + 1; ++l) {
					switch (Note.EffNumber[l]) {
						case EF_JUMP:
							JumpTo = Note.EffParam[l];
							break;
						case EF_SKIP:
							SkipTo = Frame + 1;
							break;
						case EF_HALT:
							Count = 1;
							bScanning = false;
							break;
					}
				}
			}
		}

		if (FrameVisited[Frame] == 0)
			++FirstLoop;
		else if (FrameVisited[Frame] == 1)
			++SecondLoop;
		else if (FrameVisited[Frame] == 2)
			bScanning = false;

		++FrameVisited[Frame];

		if (JumpTo != -1)
			Frame = JumpTo;
		else if (SkipTo != -1)
			Frame = SkipTo;
		else
			++Frame;
		if (Frame >= FrameCount)
			Frame = 0;
	}

	if (Count > 1)
		return FirstLoop + SecondLoop * (Count - 1);
	else if (Count == 1)
		return FirstLoop;

	return 0;
}

int CFamiTrackerDoc::GetActualPatternLength(int Frame) const
{
	// Todo: unused (remove?)

	stChanNote Note;
	int Channels = GetChannelCount();
	int PatternLength = GetPatternLength();	// default length

	if (Frame < 0 || Frame >= (signed)GetFrameCount())
		return PatternLength;

	for (int j = 0; j < PatternLength; ++j) {
		for (int i = 0; i < Channels; ++i) {
			GetNoteData(Frame, i, j, &Note);
			for (int k = 0; k < (signed)GetEffColumns(i) + 1; ++k) {
				switch (Note.EffNumber[k]) {
					case EF_SKIP:
					case EF_JUMP:
					case EF_HALT:
						return j + 1;
				}
			}
		}
	}

	return PatternLength;
}

// Operations

void CFamiTrackerDoc::RemoveUnusedInstruments()
{
	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (IsInstrumentUsed(i)) {
			bool Used = false;
			for (unsigned int j = 0; j < m_iTracks; ++j) {
				for (unsigned int Channel = 0; Channel < m_iChannelsAvailable; ++Channel) {
					for (unsigned int Frame = 0; Frame < m_pTracks[j]->GetFrameCount(); ++Frame) {
						unsigned int Pattern = m_pTracks[j]->GetFramePattern(Frame, Channel);
						for (unsigned int Row = 0; Row < m_pTracks[j]->GetPatternLength(); ++Row) {
							stChanNote *pNote = m_pTracks[j]->GetPatternData(Channel, Pattern, Row);
							if (pNote->Instrument == i)
								Used = true;
						}
					}
				}
			}
			if (!Used)
				RemoveInstrument(i);
		}
	}

	// Also remove unused sequences
	for (int i = 0; i < MAX_SEQUENCES; ++i) {
		for (int j = 0; j < SEQ_COUNT; ++j) {
			// Scan through all 2A03 sequences
			if (GetSequence(i, j)->GetItemCount() > 0) {
				bool Used = false;
				for (int k = 0; k < MAX_INSTRUMENTS; ++k) {
					if (IsInstrumentUsed(k) && GetInstrumentType(k) == INST_2A03) {
						CInstrument2A03 *pInstrument = static_cast<CInstrument2A03*>(GetInstrument(k));
						if (pInstrument->GetSeqIndex(j) == i) {
							Used = true;
							break;
						}
						pInstrument->Release();
					}
				}
				if (!Used)
					GetSequence(i, j)->Clear();
			}
			// Scan through all VRC6 sequences
			if (GetSequenceVRC6(i, j)->GetItemCount() > 0) {
				bool Used = false;
				for (int k = 0; k < MAX_INSTRUMENTS; ++k) {
					if (IsInstrumentUsed(k) && GetInstrumentType(k) == INST_VRC6) {
						CInstrumentVRC6 *pInstrument = static_cast<CInstrumentVRC6*>(GetInstrument(k));
						if (pInstrument->GetSeqIndex(j) == i) {
							Used = true;
							break;
						}
						pInstrument->Release();
					}
				}
				if (!Used)
					GetSequenceVRC6(i, j)->Clear();
			}
			// Scan through all VRC6 sequences
			if (GetSequenceN163(i, j)->GetItemCount() > 0) {
				bool Used = false;
				for (int k = 0; k < MAX_INSTRUMENTS; ++k) {
					if (IsInstrumentUsed(k) && GetInstrumentType(k) == INST_N163) {
						CInstrumentN163 *pInstrument = static_cast<CInstrumentN163*>(GetInstrument(k));
						if (pInstrument->GetSeqIndex(j) == i) {
							Used = true;
							break;
						}
						pInstrument->Release();
					}
				}
				if (!Used)
					GetSequenceN163(i, j)->Clear();
			}	}
	}
}

void CFamiTrackerDoc::RemoveUnusedPatterns()
{
	for (unsigned int i = 0; i < m_iTracks; ++i) {
		for (unsigned int c = 0; c < m_iChannelsAvailable; ++c) {
			for (unsigned int p = 0; p < MAX_PATTERN; ++p) {
				bool bRemove(true);
				// Check if pattern is used in frame list
				unsigned int FrameCount = m_pTracks[i]->GetFrameCount();
				for (unsigned int f = 0; f < FrameCount; ++f) {
					bRemove = (m_pTracks[i]->GetFramePattern(f, c) == p) ? false : bRemove;
				}
				if (bRemove)
					m_pTracks[i]->ClearPattern(c, p);
			}
		}
	}
}

void CFamiTrackerDoc::MergeDuplicatedPatterns()
{
    for (unsigned int i = 0; i < m_iTracks; ++i)
    for (unsigned int c = 0; c < m_iChannelsAvailable; ++c)
    {
        TRACE2("Trim: %d, %d\n", i, c);

        unsigned int uiPatternUsed[MAX_PATTERN];

        // mark all as unused
        for (unsigned int ui=0; ui < MAX_PATTERN; ++ui)
        {
            uiPatternUsed[ui] = MAX_PATTERN;
        }

        // map used patterns to themselves
        for (unsigned int f=0; f < m_pTracks[i]->GetFrameCount(); ++f)
        {
            unsigned int uiPattern = m_pTracks[i]->GetFramePattern(f,c);
            uiPatternUsed[uiPattern] = uiPattern;
        }

        // remap duplicates
        for (unsigned int ui=0; ui < MAX_PATTERN; ++ui)
        {
            if (uiPatternUsed[ui] == MAX_PATTERN) continue;
            for (unsigned int uj=0; uj < ui; ++uj)
            {
                unsigned int uiLen = m_pTracks[i]->GetPatternLength();
                bool bSame = true;
                for (unsigned int uk = 0; uk < uiLen; ++uk)
                {
                    stChanNote* a = m_pTracks[i]->GetPatternData(c, ui, uk);
                    stChanNote* b = m_pTracks[i]->GetPatternData(c, uj, uk);
                    if (0 != ::memcmp(a, b, sizeof(stChanNote)))
                    {
                        bSame = false;
                        break;
                    }
                }
                if (bSame)
                {
                    uiPatternUsed[ui] = uj;
                    TRACE2("Duplicate: %d = %d\n", ui, uj);
                    break;
                }
            }
        }

        // apply mapping
        for (unsigned int f=0; f < m_pTracks[i]->GetFrameCount(); ++f)
        {
            unsigned int uiPattern = m_pTracks[i]->GetFramePattern(f,c);
            m_pTracks[i]->SetFramePattern(f,c,uiPatternUsed[uiPattern]);
        }
    }
}
