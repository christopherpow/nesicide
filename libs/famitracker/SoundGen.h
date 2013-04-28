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

#include <QThread>
#include <QTimer>

#include "cqtmfc.h"

#include "common.h"

#include "FamiTrackerDoc.h"

//
// This thread will take care of the NES sound generation
//

const int VIBRATO_LENGTH = 256;
const int TREMOLO_LENGTH = 256;

const int NOTE_COUNT = 96;	// 96 available notes

// Custom messages
enum { WM_USER_SILENT_ALL = WM_USER + 1,
	   WM_USER_LOAD_SETTINGS,
	   WM_USER_PLAY,
	   WM_USER_STOP,
	   WM_USER_RESET,
	   WM_USER_START_RENDER,
	   WM_USER_STOP_RENDER,
	   WM_USER_PREVIEW_SAMPLE,
	   WM_USER_WRITE_APU,
	   WM_USER_CLOSE_SOUND
};

// Player modes
enum {MODE_PLAY,			// Play from top of pattern
	  MODE_PLAY_START,		// Play from start of song
	  MODE_PLAY_REPEAT,		// Play and repeat
	  MODE_PLAY_CURSOR,		// Play from cursor
	  MODE_PLAY_FRAME		// Play frame
};

typedef enum { SONG_TIME_LIMIT, SONG_LOOP_LIMIT } RENDER_END;

// Used to get the DPCM state
struct stDPCMState {	
	int SamplePos;
	int DeltaCntr;
};

class CChannelHandler;
class CFamiTrackerView;
class CAPU;
class CDSound;
class CDSoundChannel;
class CSampleWindow;

// CSoundGen

class CSoundGen : public CWinThread, ICallback
{
   Q_OBJECT
   // Qt stuff
   QThread* pThread;
public slots:
   void onIdleSlot();
   void recvThreadMessage(unsigned int m,unsigned int w,unsigned int l);
signals:
   void DrawSamples(int *Samples, int Count);
public:
	CSoundGen();
	virtual ~CSoundGen();

	//
	// Public functions
	//
public:
	// One time initialization
	void		AssignDocument(CFamiTrackerDoc *pDoc);
	void		AssignView(CFamiTrackerView *pView);
	void		RemoveDocument();
	void		SetSampleWindow(CSampleWindow *pWnd);

	// Multiple times initialization
	void		RegisterChannels(int Chip, CFamiTrackerDoc *pDoc);
	void		SelectChip(int Chip);
	void		LoadMachineSettings(int Machine, int Rate);

	// Sound
	bool		InitializeSound(HWND hWnd, HANDLE hAliveCheck, HANDLE hNotification);
	void		FlushBuffer(int16 *Buffer, uint32 Size);
	CDSound		*GetSoundInterface() const { return m_pDSound; };

	// Synchronization
	void		LockDocument();
	void		UnlockDocument();

	bool		WaitForStop() const;

	int			FindNote(unsigned int Period) const;
	unsigned int GetPeriod(int Note) const;

public:
	// Vibrato
	void		 GenerateVibratoTable(int Type);
	int			 ReadVibratoTable(int index) const;

	int			 ReadNamcoPeriodTable(int index) const;

	// Player interface
	void		 StartPlayer(int Mode);	
	void		 StopPlayer();
	void		 ResetPlayer();
	void		 LoadSettings();
	void		 SilentAll();

	void		 ResetTempo();
	unsigned int GetTempo() const;
	bool		 IsPlaying() const { return m_bPlaying; };

	// Stats
	unsigned int GetUnderruns();
	unsigned int GetFrameRate();

	// Tracker playing
	void		 EvaluateGlobalEffects(stChanNote *NoteData, int EffColumns);
	stDPCMState	 GetDPCMState() const;

	// Rendering
//	bool		 RenderToFile(char *File, int SongEndType, int SongEndParam);
	void		 StopRendering();
//	void		 GetRenderStat(int &Frame, int &Time, bool &Done);
	bool		 IsRendering();
	void		 CheckRenderStop();
	void		 SongIsDone();
	void		 FrameIsDone(int SkipFrames);

	// Sample previewing
	void		 PreviewSample(CDSample *pSample, int Offset, int Pitch);
	bool		 PreviewDone() const;

	void		 WriteAPU(int Address, char Value);

	// Used by channels
	void		AddCycles(int Count);

	// Other
	uint8		GetReg(int Chip, int Reg) const { return m_pAPU->GetReg(Chip, Reg); };

	// FDS & N163 wave preview
	void		WaveChanged();
	bool		HasWaveChanged();


	// 
	// Private functions
	//
private:
	// Internal initialization
	void		CreateChannels();
	void		SetupChannels();
	void		AssignChannel(CTrackerChannel *pTrackerChannel, CChannelHandler *pRenderer);
	void		ResetAPU();
	void		GeneratePeriodTables(int BaseFreq);

	// Audio
	bool		ResetSound();
	void		CloseSound();

	// Player
	void	 	PlayNote(int Channel, stChanNote *NoteData, int EffColumns);
	void		RunFrame();
	void		CheckControl();
	void		ResetBuffer();
	void		BeginPlayer(int Mode);
	void		HaltPlayer();
	void		MakeSilent();

	// Misc
	void		PlaySample(CDSample *pSample, int Offset, int Pitch);

public:
	static const double NEW_VIBRATO_DEPTH[];
	static const double OLD_VIBRATO_DEPTH[];


	//
	// Private variables
	//
private:
	// Objects
	CChannelHandler		*m_pChannels[CHANNELS];
	CTrackerChannel		*m_pTrackerChannels[CHANNELS];
	CDSample			*m_pPreviewSample;
	CFamiTrackerDoc		*m_pDocument;
	CFamiTrackerView	*m_pTrackerView;

	// Sound
	CDSound				*m_pDSound;
	CDSoundChannel		*m_pDSoundChannel;
	CSampleWindow		*m_pSampleWnd;
	CAPU				*m_pAPU;
	CSampleMem			*m_pSampleMem;

	// Sync objects
	CCriticalSection	m_csDocumentLock;
	CCriticalSection	m_csFrameCounterLock;
	CCriticalSection	m_csUnderrunLock;
	CCriticalSection	m_csSampleWndLock;
	CSemaphore			*m_pSoundSemaphore;

private:
	// Handles
//	HANDLE				m_hNotificationEvent;
//	HANDLE				m_hAliveCheck;
//	HWND				m_hWnd;

// Sound variables (TODO: move sound to a new class?)
private:
	unsigned int		m_iSampleSize;						// Size of samples, in bits
	unsigned int		m_iBufSizeSamples;					// Buffer size in samples
	unsigned int		m_iBufSizeBytes;					// Buffer size in bytes
	unsigned int		m_iBufferPtr;						// This will point in samples
	void				*m_pAccumBuffer;
	int32				*m_iGraphBuffer;
	int					m_iAudioUnderruns;					// Keep track of underruns to inform user
	
// Tracker playing variables
private:
	unsigned int		m_iTempo, m_iSpeed;					// Tempo and speed
	int					m_iTempoAccum;						// Used for speed calculation
	unsigned int		m_iPlayTime;
	volatile bool		m_bPlaying;
	bool				m_bPlayLooping;
	int					m_iFrameCounter;

	int					m_iUpdateCycles;					// Number of cycles/APU update

	int					m_iConsumedCycles;					// Cycles consumed by the update registers functions

	unsigned int		m_iSpeedSplitPoint;					// Speed/tempo split point fetched from the module

	// Play control
	int					m_iJumpToPattern;
	int					m_iSkipToRow;
	int					m_iStepRows;
	int					m_iPlayMode;

	unsigned int		*m_pNoteLookupTable;				// NTSC or PAL
	unsigned int		m_iNoteLookupTableNTSC[96];			// For 2A03
	unsigned int		m_iNoteLookupTablePAL[96];			// For 2A07
	unsigned int		m_iNoteLookupTableSaw[96];			// For VRC6 sawtooth
	unsigned int		m_iNoteLookupTableFDS[96];			// For FDS
	unsigned int		m_iNoteLookupTableN163[96];			// For N163
	unsigned int		m_iNoteLookupTableS5B[96];			// For sunsoft
	int					m_iVibratoTable[VIBRATO_LENGTH];

	unsigned int		m_iMachineType;						// NTSC/PAL

	// Rendering
	RENDER_END			m_iRenderEndWhen;
	int					m_iRenderEndParam;
	int					m_iRenderedFrames;
	int					m_iRenderedSong;
	int					m_iDelayedStart;
	int					m_iDelayedEnd;

	int					m_iTempoDecrement;
	bool				m_bUpdateRow;

	bool				m_bRendering;
	bool				m_bRequestRenderStop;
	bool				m_bPlayerHalted;

//	CWaveFile			m_wfWaveFile;

	// FDS & N163 waves
	bool				m_bWaveChanged;

	// Overloaded functions
public:
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
   
   // Implementation
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSilentAll(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLoadSettings(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBeginPlayer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStopPlayer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnResetPlayer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartRender(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStopRender(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPreviewSample(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWriteAPU(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCloseSound(WPARAM wParam, LPARAM lParam);
};
