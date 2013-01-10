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

//
// This file takes care of the NES sound playback
//

#include <cmath>
#include "FamiTrackerDoc.h"
#include "apu/apu.h"

#include "ChannelHandler.h"
#include "Channels2A03.h"
#include "ChannelsVRC6.h"
#include "ChannelsMMC5.h"
#include "ChannelsVRC7.h"
#include "ChannelsFDS.h"
#include "ChannelsN106.h"
#include "ChannelsS5B.h"

#include "SoundGen.h"
#include "TrackerChannel.h"

// 1kHz test tone
//#define AUDIO_TEST

// Output period tables to files
//#define WRITE_PERIOD_FILES

// Write a file with the volume table
//#define WRITE_VOLUME_FILE

// The depth of each vibrato level
const double CSoundGen::NEW_VIBRATO_DEPTH[] = {
	1.0, 1.5, 2.5, 4.0, 5.0, 7.0, 10.0, 12.0, 14.0, 17.0, 22.0, 30.0, 44.0, 64.0, 96.0, 128.0
};

const double CSoundGen::OLD_VIBRATO_DEPTH[] = {
	1.0, 1.0, 2.0, 3.0, 4.0, 7.0, 8.0, 15.0, 16.0, 31.0, 32.0, 63.0, 64.0, 127.0, 128.0, 255.0
};

CSoundGen::CSoundGen() :
	m_pAPU(NULL),
	m_pSampleMem(NULL),
//	m_pDSound(NULL),
//	m_pDSoundChannel(NULL),
	m_pAccumBuffer(NULL),
	m_iGraphBuffer(NULL),
	m_pDocument(NULL),
//	m_pTrackerView(NULL),
	m_bRendering(false),
	m_bPlaying(false),
	m_pPreviewSample(NULL),
//	m_pSampleWnd(NULL),
	m_iSpeed(0),
	m_iTempo(0),
	m_bPlayerHalted(false)
{
	// DPCM sample interface
	m_pSampleMem = new CSampleMem();

	// Create APU
	m_pAPU = new CAPU(this, m_pSampleMem);

	// Create all kinds of channels
	CreateChannels();

   // CP: From InitInstance
   // Generate default vibrato table
	GenerateVibratoTable(VIBRATO_NEW);

   qDebug("ResetSound");
//	ResetSound();

	LoadMachineSettings(DEFAULT_MACHINE_TYPE, DEFAULT_MACHINE_TYPE == NTSC ? CAPU::FRAME_RATE_NTSC : CAPU::FRAME_RATE_PAL);

	ResetAPU();

	// Default tempo & speed
	m_iSpeed = DEFAULT_SPEED;
	m_iTempo = (DEFAULT_MACHINE_TYPE == NTSC) ? DEFAULT_TEMPO_NTSC : DEFAULT_TEMPO_PAL;

	TRACE0("SoundGen: Created thread\n");
}

CSoundGen::~CSoundGen()
{
	// Delete APU
	SAFE_RELEASE(m_pAPU);
//	SAFE_RELEASE(m_pSampleMem);

	// Remove channels
	for (int i = 0; i < CHANNELS; ++i) {
		SAFE_RELEASE(m_pChannels[i]);
		SAFE_RELEASE(m_pTrackerChannels[i]);
	}
}


void CSoundGen::CreateChannels()
{
	// Clear all channels
	for (int i = 0; i < CHANNELS; ++i) {
		m_pChannels[i] = NULL;
		m_pTrackerChannels[i] = NULL;
	}

	// 2A03/2A07
	AssignChannel(new CTrackerChannel(_T("Square 1"), SNDCHIP_NONE, CHANID_SQUARE1), new CSquare1Chan());
	AssignChannel(new CTrackerChannel(_T("Square 2"), SNDCHIP_NONE, CHANID_SQUARE2), new CSquare2Chan());
	AssignChannel(new CTrackerChannel(_T("Triangle"), SNDCHIP_NONE, CHANID_TRIANGLE), new CTriangleChan());
	AssignChannel(new CTrackerChannel(_T("Noise"), SNDCHIP_NONE, CHANID_NOISE), new CNoiseChan());
	AssignChannel(new CTrackerChannel(_T("DPCM"), SNDCHIP_NONE, CHANID_DPCM), new CDPCMChan(m_pSampleMem));

	// Konami VRC6
	AssignChannel(new CTrackerChannel(_T("Square 1"), SNDCHIP_VRC6, CHANID_VRC6_PULSE1), new CVRC6Square1());
	AssignChannel(new CTrackerChannel(_T("Square 2"), SNDCHIP_VRC6, CHANID_VRC6_PULSE2), new CVRC6Square2());
	AssignChannel(new CTrackerChannel(_T("Sawtooth"), SNDCHIP_VRC6, CHANID_VRC6_SAWTOOTH), new CVRC6Sawtooth());

	// Konami VRC7
	AssignChannel(new CTrackerChannel(_T("FM Channel 1"), SNDCHIP_VRC7, CHANID_VRC7_CH1), new CVRC7Channel());
	AssignChannel(new CTrackerChannel(_T("FM Channel 2"), SNDCHIP_VRC7, CHANID_VRC7_CH2), new CVRC7Channel());
	AssignChannel(new CTrackerChannel(_T("FM Channel 3"), SNDCHIP_VRC7, CHANID_VRC7_CH3), new CVRC7Channel());
	AssignChannel(new CTrackerChannel(_T("FM Channel 4"), SNDCHIP_VRC7, CHANID_VRC7_CH4), new CVRC7Channel());
	AssignChannel(new CTrackerChannel(_T("FM Channel 5"), SNDCHIP_VRC7, CHANID_VRC7_CH5), new CVRC7Channel());
	AssignChannel(new CTrackerChannel(_T("FM Channel 6"), SNDCHIP_VRC7, CHANID_VRC7_CH6), new CVRC7Channel());

	// Nintendo FDS
	AssignChannel(new CTrackerChannel(_T("FDS"), SNDCHIP_FDS, CHANID_FDS), new CChannelHandlerFDS());

	// Nintendo MMC5
	AssignChannel(new CTrackerChannel(_T("Square 1"), SNDCHIP_MMC5, CHANID_MMC5_SQUARE1), new CMMC5Square1Chan());
	AssignChannel(new CTrackerChannel(_T("Square 2"), SNDCHIP_MMC5, CHANID_MMC5_SQUARE2), new CMMC5Square2Chan());

	// Namco N106
	AssignChannel(new CTrackerChannel(_T("Namco 1"), SNDCHIP_N106, CHANID_N106_CHAN1), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 2"), SNDCHIP_N106, CHANID_N106_CHAN2), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 3"), SNDCHIP_N106, CHANID_N106_CHAN3), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 4"), SNDCHIP_N106, CHANID_N106_CHAN4), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 5"), SNDCHIP_N106, CHANID_N106_CHAN5), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 6"), SNDCHIP_N106, CHANID_N106_CHAN6), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 7"), SNDCHIP_N106, CHANID_N106_CHAN7), new CChannelHandlerN106());
	AssignChannel(new CTrackerChannel(_T("Namco 8"), SNDCHIP_N106, CHANID_N106_CHAN8), new CChannelHandlerN106());

	// Sunsoft 5B
	AssignChannel(new CTrackerChannel(_T("Square 1"), SNDCHIP_S5B, CHANID_S5B_CH1), new CS5BChannel1());
	AssignChannel(new CTrackerChannel(_T("Square 2"), SNDCHIP_S5B, CHANID_S5B_CH2), new CS5BChannel2());
	AssignChannel(new CTrackerChannel(_T("Square 3"), SNDCHIP_S5B, CHANID_S5B_CH3), new CS5BChannel3());
}

// Object initialization

void CSoundGen::AssignDocument(CFamiTrackerDoc *pDoc)
{
	// Will only work for the first document (as new documents are used to import files)
	if (m_pDocument != NULL)
		return;

	// Assigns a document to this object
	m_pDocument = pDoc;

	// Setup all channels
	for (int i = 0; i < CHANNELS; ++i) {
		if (m_pChannels[i])
			m_pChannels[i]->InitChannel(m_pAPU, m_iVibratoTable, m_pDocument);
	}
}

//void CSoundGen::AssignView(CFamiTrackerView *pView)
//{
//	if (m_pTrackerView != NULL)
//		return;

//	// Assigns the tracker view to this object
//	m_pTrackerView = pView;
//}

void CSoundGen::RemoveDocument()
{
	// Removes both the document and view from this object
   qDebug("::RemoveDocument");

//	if (!m_pDocument || !m_hThread)
//		return;

//	// Player cannot play when removing the document
//	StopPlayer();

//	DWORD StartTime = GetTickCount();

//	while (m_bPlaying) {
//		if ((GetTickCount() - StartTime) > 2000)
//			break;
//	}

//	m_csDocumentLock.Lock();

//	m_pDocument = NULL;
//	m_pTrackerView = NULL;

//	m_csDocumentLock.Unlock();
}

//void CSoundGen::SetSampleWindow(CSampleWindow *pWnd)
//{
//	m_csSampleWndLock.Lock();
//	m_pSampleWnd = pWnd;
//	m_csSampleWndLock.Unlock();
//}

void CSoundGen::RegisterChannels(int Chip)
{
	// Called from the document object (from the main thread)
	//

	if (m_bPlaying)
		StopPlayer();

	if (!WaitForStop()) {
		TRACE0("CSoundGen: Could not stop player!");
		return;
	}

	// This affects the sound channel interface so it must be synchronized
//	m_csDocumentLock.Lock();

	m_pAPU->SetExternalSound(Chip);

	ResetAPU();

	// Clear all registered channels
	m_pDocument->ResetChannels();

	// Register the channels in the document
	//

	// Expansion & internal channels
	for (int i = 0; i < CHANNELS; ++i) {
		if (m_pTrackerChannels[i] && ((m_pTrackerChannels[i]->GetChip() & Chip) || (i < 5))) {
			m_pDocument->RegisterChannel(m_pTrackerChannels[i], i, m_pTrackerChannels[i]->GetChip());
		}
	}

//	m_csDocumentLock.Unlock();
}

void CSoundGen::SetupChannels()
{
//	m_csDocumentLock.Lock();

	// Initialize channels
	for (int i = 0; i < CHANNELS; ++i) {
		if (m_pChannels[i]) {
			m_pChannels[i]->InitChannel(m_pAPU, m_iVibratoTable, m_pDocument);
			m_pChannels[i]->MakeSilent();
		}
	}

//	m_csDocumentLock.Unlock();
}

//// Interface functions ///////////////////////////////////////////////////////////////////////////////////

void CSoundGen::StartPlayer(int Mode)
{
//	if (!m_pDocument || !m_pDocument->IsFileLoaded() || !m_hThread)
//		return;

	m_iPlayMode = Mode;

//	PostThreadMessage(M_PLAY, 0, 0);
}

void CSoundGen::StopPlayer()
{
//	if (!m_pDocument || !m_hThread)
//		return;

//	PostThreadMessage(M_STOP, 0, 0);
}

void CSoundGen::ResetPlayer()
{
//	if (!m_hThread)
//		return;

//	PostThreadMessage(M_RESET, 0, 0);
}

//// Sound handling ////////////////////////////////////////////////////////////////////////////////////////

void CSoundGen::AssignChannel(CTrackerChannel *pTrackerChannel, CChannelHandler *pRenderer)
{
	int ID = pTrackerChannel->GetID();

	pRenderer->SetChannelID(ID);

	m_pTrackerChannels[ID] = pTrackerChannel;
	m_pChannels[ID] = pRenderer;
}

//bool CSoundGen::InitializeSound(HWND hWnd, HANDLE hAliveCheck, HANDLE hNotification)
//{
//	// Initialize sound, this is only called once!
//	// Start with NTSC by default

//	ASSERT(m_pDSound == NULL);

//	m_iMachineType = NTSC;//SPEED_NTSC;
//	m_hWnd = hWnd;

//	m_hNotificationEvent = hNotification;
//	m_hAliveCheck = hAliveCheck;

//	// Create DirectSound object
//	m_pDSound = new CDSound();

//	// Out of memory
//	if (!m_pDSound)
//		return false;

//	m_pDSound->EnumerateDevices();

//	return true;
//}

//bool CSoundGen::ResetSound()
//{
//	// Setup sound, return false if failed
//	//

//	CSettings *pSettings = theApp.GetSettings();

//	unsigned int SampleSize = pSettings->Sound.iSampleSize;
//	unsigned int SampleRate = pSettings->Sound.iSampleRate;
//	unsigned int BufferLen	= pSettings->Sound.iBufferLength;

//	m_iSampleSize = SampleSize;
//	m_iAudioUnderruns = 0;
//	m_iBufferPtr = 0;

//	// Close the old sound channel
//	if (m_pDSoundChannel) {
//		m_pDSound->CloseChannel(m_pDSoundChannel);
//		m_pDSoundChannel = NULL;
//	}

//	// Reinitialize direct sound
//	if (!m_pDSound->Init(m_hWnd, m_hNotificationEvent, theApp.GetSettings()->Sound.iDevice)) {
//		AfxMessageBox(_T("Direct sound error!"));
//		return false;
//	}

//	int iBlocks = 2;	// default = 2

//	// Create more blocks if a bigger buffer than 100ms is used to enhance program response
//	if (BufferLen > 100)
//		iBlocks = (BufferLen / 66);

//	// Create channel
//	m_pDSoundChannel = m_pDSound->OpenChannel(SampleRate, SampleSize, 1, BufferLen, iBlocks);

//	// Channel failed
//	if (m_pDSoundChannel == NULL) {
//		AfxMessageBox(_T("Direct sound error: Could not create buffer!"));
//		return false;
//	}

//	// Create a buffer
//	m_iBufSizeBytes	  = m_pDSoundChannel->GetBlockSize();
//	m_iBufSizeSamples = m_iBufSizeBytes / (SampleSize / 8);

//	// Temp. audio buffer
//	SAFE_RELEASE(m_pAccumBuffer);
//	m_pAccumBuffer = new char[m_iBufSizeBytes];

//	// Out of memory
//	if (!m_pAccumBuffer)
//		return false;

//	// Sample graph buffer
//	SAFE_RELEASE(m_iGraphBuffer);
//	m_iGraphBuffer = new int32[m_iBufSizeSamples];

//	// Out of memory
//	if (!m_iGraphBuffer)
//		return false;

//	// Sample graph rate
//	if (m_pSampleWnd) {
//		m_pSampleWnd->SetSampleRate(SampleRate);
//	}

//	if (!m_pAPU->SetupSound(SampleRate, 1, (m_iMachineType == NTSC) ? MACHINE_NTSC : MACHINE_PAL))
//		return false;

//	// Update blip-buffer filtering
//	m_pAPU->SetupMixer(pSettings->Sound.iBassFilter,
//		pSettings->Sound.iTrebleFilter,
//		pSettings->Sound.iTrebleDamping,
//		pSettings->Sound.iMixVolume);

//	return true;
//}

//void CSoundGen::CloseSound()
//{
//	// Kill DirectSound
//	if (m_pDSoundChannel) {
//		m_pDSoundChannel->Stop();
//		m_pDSound->CloseChannel(m_pDSoundChannel);
//		m_pDSoundChannel = NULL;
//	}

//	if (m_pDSound) {
//		m_pDSound->Close();
//		delete m_pDSound;
//		m_pDSound = NULL;
//	}
//}

//void CSoundGen::ResetBuffer()
//{
//	m_iBufferPtr = 0;

//	if (m_pDSoundChannel)
//		m_pDSoundChannel->Clear();

//	m_pAPU->Reset();
//}

void CSoundGen::FlushBuffer(int16 *pBuffer, uint32 Size)
{
//	// Called when the APU audio buffer is full and
//	// ready for playing

//	// May only be called from sound player thread
//	ASSERT(GetCurrentThreadId() == m_nThreadID);

//	const int SAMPLE_MAX = 32767;
//	const int SAMPLE_MIN = -32768;

//	if (!m_pDSoundChannel)
//		return;

//	BOOL bLocked = m_csDocumentLock.Unlock();

//	for (uint32 i = 0; i < Size; ++i) {

//		int32 Sample = pBuffer[i];

//		// 1000 Hz test tone
//#ifdef AUDIO_TEST
//		static double sine_phase = 0;
//		Sample = int32(sin(sine_phase) * 10000.0);

//		static double freq = 1000;
//		// Sweep
//		//freq+=0.1;
//		if (freq > 20000)
//			freq = 20;

//		sine_phase += freq / (double(m_pDSoundChannel->GetSampleRate()) / 6.283184);
//		if (sine_phase > 6.283184)
//			sine_phase -= 6.283184;
//#endif

//		// Limit
//		if (Sample > SAMPLE_MAX)
//			Sample = SAMPLE_MAX;
//		if (Sample < SAMPLE_MIN)
//			Sample = SAMPLE_MIN;

//		ASSERT(m_iBufferPtr < m_iBufSizeSamples);

//		// Sample scope
//		m_iGraphBuffer[m_iBufferPtr] = Sample;

//		// Convert sample and store in temp buffer
//		//if (m_iBufferPtr < m_iBufSizeBytes)
//		if (m_iSampleSize == 8)
//			((int8*)m_pAccumBuffer)[m_iBufferPtr] = (uint8)(Sample >> 8) ^ 0x80;
//		else
//			((int16*)m_pAccumBuffer)[m_iBufferPtr] = (int16)Sample;

//		++m_iBufferPtr;

//		// If buffer is filled, throw it to direct sound
//		if (m_iBufferPtr >= m_iBufSizeSamples) {

//			if (m_bRendering) {
//				// Output to file
//				m_wfWaveFile.WriteWave((char*)m_pAccumBuffer, m_iBufSizeBytes);
//				m_iBufferPtr = 0;
//			}
//			else {
//				// Output to direct sound
//				DWORD dwEvent;
//				bool bUnderrun = false;

//				// Wait for a buffer event
//				while ((dwEvent = m_pDSoundChannel->WaitForDirectSoundEvent()) != BUFFER_IN_SYNC) {
//					switch (dwEvent) {
//						case CUSTOM_EVENT:
//							// Custom event, quit this function
//							m_iBufferPtr = 0;
//							if (bLocked)
//								m_csDocumentLock.Lock();
//							return;
//						case BUFFER_OUT_OF_SYNC:
//							// Buffer underrun detected
//							static DWORD LastUnderrun;
//							m_csUnderrunLock.Lock();
//							m_iAudioUnderruns++;
//							m_csUnderrunLock.Unlock();
//							if ((GetTickCount() - LastUnderrun) < 2000) {
//								// Display underrun message in main window
//								CFrameWnd *pFrameWnd = (CFrameWnd*) theApp.GetMainWnd();
//								if (pFrameWnd)
//									pFrameWnd->SetMessageText(IDS_UNDERRUN_MESSAGE);
//							}
//							LastUnderrun = GetTickCount();
//							bUnderrun = true;
//							break;
//					}
//				}

//				// Write audio to buffer
//				m_pDSoundChannel->WriteSoundBuffer(m_pAccumBuffer, m_iBufSizeBytes);

//				// Draw graph
//				m_csSampleWndLock.Lock();

//				if (m_pSampleWnd)
//					m_pSampleWnd->DrawSamples((int*)m_iGraphBuffer, m_iBufSizeSamples);

//				m_csSampleWndLock.Unlock();

//				// Reset buffer position
//				m_iBufferPtr = 0;
//			}
//		}
//	}

//	if (bLocked)
//		m_csDocumentLock.Lock();
}

//unsigned int CSoundGen::GetUnderruns()
//{
//	m_csUnderrunLock.Lock();
//	int Value = m_iAudioUnderruns;
//	m_csUnderrunLock.Unlock();
//	return Value;
//}

//unsigned int CSoundGen::GetFrameRate()
//{
//	m_csFrameCounterLock.Lock();
//	int FrameRate = m_iFrameCounter;
//	m_iFrameCounter = 0;
//	m_csFrameCounterLock.Unlock();
//	return FrameRate;
//}

//// Tracker playing routines //////////////////////////////////////////////////////////////////////////////

void CSoundGen::GenerateVibratoTable(int Type)
{
	for (int i = 0; i < 16; ++i) {	// depth
		for (int j = 0; j < 16; ++j) {	// phase
			int value = 0;
			double angle = (double(j) / 16.0) * (3.1415 / 2.0);

			if (Type == VIBRATO_NEW)
				value = int(sin(angle) * NEW_VIBRATO_DEPTH[i] /*+ 0.5f*/);
			else {
				value = (int)((double(j * OLD_VIBRATO_DEPTH[i]) / 16.0) + 1);
			}

			m_iVibratoTable[i * 16 + j] = value;
		}
	}

#ifdef _DEBUG
/*
	CFile a("vibrato.txt", CFile::modeWrite | CFile::modeCreate);
	CString b;
	for (int i = 0; i < 16; i++) {	// depth
		b = "\t.byte ";
		a.Write(b.GetBuffer(), b.GetLength());
		for (int j = 0; j < 16; j++) {	// phase
			int value = m_iVibratoTable[i * 16 + j];
			b.Format("$%02X, ", value);
			a.Write(b.GetBuffer(), b.GetLength());
		}
		b = "\n";
		a.Write(b.GetBuffer(), b.GetLength());
	}
	a.Close();
*/
#endif

	// Notify channels
	for (int i = 0; i < CHANNELS; ++i)
		if (m_pChannels[i])
			m_pChannels[i]->SetVibratoStyle(Type);
}

int CSoundGen::ReadVibratoTable(int index) const
{
	return m_iVibratoTable[index];
}

void CSoundGen::BeginPlayer()
{
//	if (!m_pDSoundChannel || !m_pDocument)
//		return;

//	switch (m_iPlayMode) {
//		// Play from top of pattern
//		case MODE_PLAY:
//			m_bPlayLooping = false;
//			m_pTrackerView->PlayerCommand(CMD_MOVE_TO_TOP, 0);
//			break;
//		// Repeat pattern
//		case MODE_PLAY_REPEAT:
//			m_bPlayLooping = true;
//			m_pTrackerView->PlayerCommand(CMD_MOVE_TO_TOP, 0);
//			break;
//		// Start of song
//		case MODE_PLAY_START:
//			m_bPlayLooping = false;
//			m_pTrackerView->PlayerCommand(CMD_MOVE_TO_START, 0);
//			break;
//		// From cursor
//		case MODE_PLAY_CURSOR:
//			m_bPlayLooping = false;
//			m_pTrackerView->PlayerCommand(CMD_MOVE_TO_CURSOR, 0);
//			break;
//	}

//	m_bPlaying			= true;
//	m_iPlayTime			= 0;
//	m_iJumpToPattern	= -1;
//	m_iSkipToRow		= -1;
//	m_bUpdateRow		= true;
//	m_bPlayerHalted		= false;

//	ResetTempo();

//	LoadMachineSettings(m_pDocument->GetMachine(), m_pDocument->GetEngineSpeed());

//	theApp.SilentEverything();
}

void CSoundGen::HaltPlayer()
{
//	m_bPlaying = false;
//	m_bPlayerHalted = false;
//	MakeSilent();
}

//void CSoundGen::HandleMessages()
//{
//	MSG msg;

//	// Read thread messages
//	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//		switch (msg.message) {
//			// Close the thread
//			case WM_QUIT:
//				TRACE("SoundGen: Received the quit message\n");
//				m_bRunning = false;
//				return;
//			// Make everything silent
//			case M_SILENT_ALL:
//				MakeSilent();
//				break;
//			// Load new settings
//			case M_LOAD_SETTINGS:
//				if (!ResetSound()) {
//					TRACE("SoundGen: Error when resetting sound settings\n");
//				}
//				break;
//			// Play
//			case M_PLAY:
//				BeginPlayer();
//				break;
//			// Reset
//			case M_RESET:
//				if (m_bPlaying)
//					BeginPlayer();
//				break;
//			// Stop
//			case M_STOP:
//				HaltPlayer();
//				break;
//			// Render to file
//			case M_START_RENDER:
//				ResetBuffer();
//				m_bRequestRenderStop = false;
//				m_bRendering = true;
//				m_iDelayedStart = 5;	// Wait 5 frames until player starts
//				m_iDelayedEnd = 5;
//				break;
//			// Preview sample
//			case M_PREVIEW_SAMPLE:
//				PlaySample((CDSample*)msg.wParam, LOWORD(msg.lParam), HIWORD(msg.lParam));
//				break;
//			// Direct APU write
//			case M_WRITE_APU:
//				m_pAPU->Write((uint16)msg.wParam, (uint8)msg.lParam);
//				break;
//			case M_CLOSE_SOUND:
//				CloseSound();
//				// Notification
//				if (msg.wParam != NULL)
//					((CEvent*) msg.wParam)->SetEvent();
//				break;
//		}
//	}
//}

void CSoundGen::ResetAPU()
{
	// Reset the APU
	m_pAPU->Reset();

	// Enable all channels
	m_pAPU->Write(0x4015, 0x0F);
	m_pAPU->Write(0x4017, 0x00);

	// MMC5
	m_pAPU->ExternalWrite(0x5015, 0x03);

	m_pSampleMem->SetMem(0, 0);
}

// Overloaded functions

//BOOL CSoundGen::InitInstance()
//{
//	//
//	// Setup the synth, called when thread is started
//	//
//	// Sound must be working before entering here!
//	//

//	// Generate default vibrato table
//	GenerateVibratoTable(VIBRATO_NEW);

//	if (!ResetSound()) {
//		return FALSE;
//	}

//	LoadMachineSettings(DEFAULT_MACHINE_TYPE, DEFAULT_MACHINE_TYPE == NTSC ? CAPU::FRAME_RATE_NTSC : CAPU::FRAME_RATE_PAL);

//	ResetAPU();

//	// Default tempo & speed
//	m_iSpeed = DEFAULT_SPEED;
//	m_iTempo = (DEFAULT_MACHINE_TYPE == NTSC) ? DEFAULT_TEMPO_NTSC : DEFAULT_TEMPO_PAL;

//	TRACE0("SoundGen: Created thread\n");

//	return TRUE;
//}

//int CSoundGen::Run()
//{
//	// This is the main entry of the sound synthesizer, the document and view must be
//	// assigned at this point
//	//

//	// TODO: split this into several functions

//	// Make sure sound isn't skipping due to running out of CPU time
//	SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

//	m_bRunning = true;
//	m_iDelayedStart = 0;
//	m_csFrameCounterLock.Lock();
//	m_iFrameCounter = 0;
//	m_csFrameCounterLock.Unlock();

//	SetupChannels();

//	// The sound generator and NES APU emulation routine
//	// This is always running in the background when the tracker is running
//	while (m_bRunning) {

//		// Check messages
//		HandleMessages();

//		if (!m_bRunning)
//			break;

//		m_csFrameCounterLock.Lock();
//		++m_iFrameCounter;
//		m_csFrameCounterLock.Unlock();

//		SetEvent(m_hAliveCheck);

//		// Access the document object
//		m_csDocumentLock.Lock();

//		if (!m_pDocument || !m_pDSoundChannel) {
//			// Document is unloaded or no sound
//			m_csDocumentLock.Unlock();
//			// Wait for kill signal
//			continue;
//		}

//		if (m_pDocument->IsFileLoaded()) {

//			RunFrame();

//			int Channels = m_pDocument->GetChannelCount();

//			// Read notes
//			for (int i = 0; i < Channels; ++i) {
//				int Channel = m_pDocument->GetChannelType(i);

//				// TODO: clean up!
//				if (m_pTrackerView->Arpeggiate[i] > 0) {
//					m_pChannels[Channel]->Arpeggiate(m_pTrackerView->Arpeggiate[i]);
//					m_pTrackerView->Arpeggiate[i] = 0;
//				}
//				// !!!

//				if (m_pTrackerChannels[Channel]->NewNoteData()) {
//					stChanNote Note = m_pTrackerChannels[Channel]->GetNote();
//					//PlayNote(Channel, &Note, m_pTrackerChannels[Channel]->GetColumnCount() + 1);
//					PlayNote(Channel, &Note, m_pDocument->GetEffColumns(i) + 1);
//				}

//				// Pitch wheel
//				int Pitch = m_pTrackerChannels[Channel]->GetPitch();
//				m_pChannels[Channel]->SetPitch(Pitch);

//				// Update volume meters
//				m_pTrackerChannels[Channel]->SetVolumeMeter(m_pAPU->GetVol(Channel));
//			}

//			int SelectedChan = m_pTrackerView->GetSelectedChannel();

//			// Instrument sequence visualization
//			if (m_pChannels[SelectedChan])
//				m_pChannels[SelectedChan]->UpdateSequencePlayPos();
//		}


//		//
//		// APU refresh cycle
//		//

//		if (m_bPlayerHalted) {
//			for (int i = 0; i < CHANNELS; ++i) {
//				if (m_pChannels[i] != NULL) {
//					m_pChannels[i]->MakeSilent();
//				}
//			}
//		}

//		const int CHANNEL_DELAY = 250;

//		m_iConsumedCycles = 0;

//		int FrameRate = m_pDocument->GetFrameRate();

//		// Update channels and channel registers
//		for (int i = 0; i < CHANNELS; ++i) {
//			if (m_pChannels[i] != NULL) {
//				m_pChannels[i]->ProcessChannel();
//				m_pChannels[i]->RefreshChannel();
//				m_pAPU->Process();
//				// Add some delay between each channel update
//				if (FrameRate == CAPU::FRAME_RATE_NTSC || FrameRate == CAPU::FRAME_RATE_PAL)
//					AddCycles(CHANNEL_DELAY);
//			}
//		}

//		// Finish the audio frame
//		m_pAPU->AddTime(m_iUpdateCycles - m_iConsumedCycles);
//		m_pAPU->Process();

//#ifdef LOGGING
//		if (m_bPlaying)
//			m_pAPU->Log();
//#endif

//		// Update player

//		//if (m_bUpdateRow && !m_bPlayerHalted)
//		if (m_bUpdateRow && !m_bPlayerHalted)
//			CheckControl();

//		if (m_bPlaying) {
//			m_iTempoAccum -= m_iTempoDecrement;
//		}

//		// Leave document object
//		// TODO: move this object to above APU update?
//		m_csDocumentLock.Unlock();

//		if (m_bPlayerHalted && m_bUpdateRow)
//			HaltPlayer();
////			m_bPlaying = false;

//		// Rendering
//		if (m_bRendering && m_bRequestRenderStop) {
//			HaltPlayer();
//			if (!m_iDelayedEnd)
//				StopRendering();
//			else
//				--m_iDelayedEnd;
//		}

//		if (m_iDelayedStart > 0) {
//			--m_iDelayedStart;
//			if (!m_iDelayedStart) {
//				m_iPlayMode = MODE_PLAY_START;
//				PostThreadMessage(M_PLAY, 0, 0);
//			}
//		}

//		// Check if a previewed sample should be removed
//		if (m_pPreviewSample && PreviewDone()) {
//			delete m_pPreviewSample;
//			m_pPreviewSample = NULL;
//		}
//	}

//	return ExitInstance();
//}

//int CSoundGen::ExitInstance()
//{
//	// Shutdown the thread

//	// Free allocated memory
//	SAFE_RELEASE_ARRAY(m_iGraphBuffer);
//	SAFE_RELEASE_ARRAY(m_pAccumBuffer);

//	// Make sure sound interface is shut down
//	CloseSound();

//	TRACE0("SoundGen: Closing thread\n");

//	return CWinThread::ExitInstance();
//}

// End of overloaded functions

void CSoundGen::AddCycles(int Count)
{
	// Add APU cycles
	m_iConsumedCycles += Count;
	m_pAPU->AddTime(Count);
}

void CSoundGen::MakeSilent()
{
	m_pAPU->Reset();
	m_pSampleMem->SetMem(0, 0);

	for (int i = 0; i < CHANNELS; ++i) {
		if (m_pChannels[i]) {
			m_pChannels[i]->ResetChannel();
			m_pChannels[i]->MakeSilent();
		}

		if (m_pTrackerChannels[i]) {
			m_pTrackerChannels[i]->Reset();
		}
	}
}

// Get tempo values from the document
void CSoundGen::ResetTempo()
{
	if (!m_pDocument)
		return;

	m_iSpeed = m_pDocument->GetSongSpeed();
	m_iTempo = m_pDocument->GetSongTempo();

	m_iTempoAccum = 0;
	m_iTempoDecrement = (m_iTempo * 24) / m_iSpeed;

	m_bUpdateRow = false;
}

// Return current tempo setting in BPM
unsigned int CSoundGen::GetTempo() const
{
	return !m_iSpeed ? 0 : (m_iTempo * 6) / m_iSpeed;
}

//void CSoundGen::RunFrame()
//{
//	int TicksPerSec = m_pDocument->GetFrameRate();

//	m_pTrackerView->PlayerCommand(CMD_TICK, 0);

//	if (m_bPlaying) {

//		m_iPlayTime++;
//		if (m_bRendering) {
//			if (m_iRenderEndWhen == SONG_TIME_LIMIT) {
//				if (m_iPlayTime >= (unsigned int)m_iRenderEndParam)
//					m_bRequestRenderStop = true;
//			}
//		}

//		// Calculate playtime
//		m_pTrackerView->PlayerCommand(CMD_TIME, (m_iPlayTime * 10) / TicksPerSec);

//		m_iStepRows = 0;

//		// Fetch next row
//		if (m_iTempoAccum <= 0) {
//			// Enable this to skip rows on high tempos
////			while (m_iTempoAccum <= 0)  {
//				m_iTempoAccum += (60 * TicksPerSec);
//				m_iStepRows++;
////			}
//			m_bUpdateRow = true;
//			m_pTrackerView->PlayerCommand(CMD_READ_ROW, 0);
//		}
//		else {
//			m_bUpdateRow = false;
//		}
//	}
//}

//void CSoundGen::CheckControl()
//{
//	// This function takes care of jumping and skipping

//	if (m_bPlaying) {
//		// If looping, halt when a jump or skip command are encountered
//		if (m_bPlayLooping) {
//			if (m_iJumpToPattern != -1 || m_iSkipToRow != -1)
//				m_pTrackerView->PlayerCommand(CMD_MOVE_TO_TOP, 0);
//			else
//				while (m_iStepRows--)
//					m_pTrackerView->PlayerCommand(CMD_STEP_DOWN, 1);
//		}
//		else {
//			// Jump
//			if (m_iJumpToPattern != -1)
//				m_pTrackerView->PlayerCommand(CMD_JUMP_TO, m_iJumpToPattern - 1);
//			// Skip
//			else if (m_iSkipToRow != -1)
//				m_pTrackerView->PlayerCommand(CMD_SKIP_TO, m_iSkipToRow);
//			// or just move on
//			else
//				while (m_iStepRows--)
//					m_pTrackerView->PlayerCommand(CMD_STEP_DOWN, 0);
//		}

//		m_iJumpToPattern = -1;
//		m_iSkipToRow = -1;
//	}
//}

void CSoundGen::LoadMachineSettings(int Machine, int Rate)
{
	// Setup machine-type and speed
	//
	// Machine = NTSC or PAL
	//
	// Rate = frame rate (0 means machine default)
	//

	ASSERT(m_pAPU != NULL);

	const double BASE_FREQ = 32.7032;
	double Freq;
	double Pitch;

	int BaseFreq	= (Machine == NTSC) ? CAPU::BASE_FREQ_NTSC  : CAPU::BASE_FREQ_PAL;
	int DefaultRate = (Machine == NTSC) ? CAPU::FRAME_RATE_NTSC : CAPU::FRAME_RATE_PAL;

	m_iMachineType = Machine;

	m_pAPU->ChangeMachine(Machine == NTSC ? MACHINE_NTSC : MACHINE_PAL);

	// Choose a default rate if not predefined
	if (Rate == 0)
		Rate = DefaultRate;

	double clock_ntsc = CAPU::BASE_FREQ_NTSC / 16.0;
	double clock_pal = CAPU::BASE_FREQ_PAL / 16.0;

	for (int i = 0; i < NOTE_COUNT; ++i) {
		// Frequency (in Hz)
		Freq = BASE_FREQ * pow(2.0, double(i) / 12.0);

		// 2A07
		Pitch = (clock_pal / Freq) - 0.5;
		m_iNoteLookupTablePAL[i] = (unsigned int)Pitch;

		// 2A03 / MMC5 / VRC6
		Pitch = (clock_ntsc / Freq) - 0.5;
		m_iNoteLookupTableNTSC[i] = (unsigned int)Pitch;

		// VRC6 Saw
		Pitch = ((clock_ntsc * 16.0) / (Freq * 14.0)) - 0.5;
		m_iNoteLookupTableSaw[i] = (unsigned int)Pitch;

		// FDS
#ifdef TRANSPOSE_FDS
		Pitch = (Freq * 65536.0) / (clock_ntsc / 2.0) + 0.5;
#else
		Pitch = (Freq * 65536.0) / (clock_ntsc / 4.0) + 0.5;
#endif
		m_iNoteLookupTableFDS[i] = (unsigned int)Pitch;
		// N106
		int C = 7; // channels
		int L = 0; // waveform length

//		Pitch = (3019898880.0 * Freq) / 21477272.7272;
		Pitch = (1509949440.0 * Freq) / 21477272.7272;

		//Pitch = (double(0x40000 * 45 * (C + 1) * (8 - L) * 4) * Freq) / (21477272.7272);
		//Pitch = (double(0x40000 * 45 * 8 * 8 * 4) * Freq) / (21477272.7272);

//		Period = (uint32)(((((ChannelsActive + 1) * 45 * 0x40000) / (float)21477270) * (float)CAPU::BASE_FREQ_NTSC) / m_iFrequency);
		//Pitch =
		m_iNoteLookupTableN106[i] = (unsigned int)Pitch;
		// Sunsoft 5B
		Pitch = (clock_ntsc / Freq) - 0.5;
		m_iNoteLookupTableS5B[i] = (unsigned int)Pitch;
	}


#ifdef WRITE_PERIOD_FILES
	// Write period tables to files
	CFile file1("..\\nsf driver\\freq_pal.bin", CFile::modeWrite | CFile::modeCreate);
	for (int i = 0 ; i < 96; i++) {
		unsigned short s = m_iNoteLookupTablePAL[i];
		file1.Write(&s, 2);
	}
	file1.Close();

	CFile file2("..\\nsf driver\\freq_ntsc.bin", CFile::modeWrite | CFile::modeCreate);
	for (int i = 0 ; i < 96; i++) {
		unsigned short s = m_iNoteLookupTableNTSC[i];
		file2.Write(&s, 2);
	}
	file2.Close();

	CFile file3("..\\nsf driver\\freq_sawtooth.bin", CFile::modeWrite | CFile::modeCreate);
	for (int i = 0 ; i < 96; i++) {
		unsigned short s = m_iNoteLookupTableSaw[i];
		file3.Write(&s, 2);
	}
	file3.Close();

	CFile file4("..\\nsf driver\\freq_fds.bin", CFile::modeWrite | CFile::modeCreate);
	for (int i = 0 ; i < 96; i++) {
		unsigned short s = m_iNoteLookupTableFDS[i];
		file4.Write(&s, 2);
	}
	file4.Close();

	CFile file5("..\\nsf driver\\freq_n106.bin", CFile::modeWrite | CFile::modeCreate);
	for (int i = 0 ; i < 96; i++) {
		unsigned int s = m_iNoteLookupTableN106[i];
		file5.Write(&s, 3);
	}
	file5.Close();
#endif

#if WRITE_VOLUME_FILE
	CFile file("vol.txt", CFile::modeWrite | CFile::modeCreate);

	for (int i = 0; i < 16; i++) {
		for (int j = /*(i / 2)*/0; j < 8; j++) {
			int a = (i*(j*2)) / 15;
			int b = (i*(j*2+1)) / 15;
			if (i > 0 && j > 0 && a == 0) a = 1;
			if (j > 0 && i > 0 && b == 0) b = 1;
			unsigned char c = (a<<4) | b;
			CString st;
			st.Format("$%02X, ", c);
			file.Write(st, st.GetLength());
		}
		file.Write("\n", 1);
	}

	file.Close();
#endif

	if (Machine == NTSC)
		m_pNoteLookupTable = m_iNoteLookupTableNTSC;
	else
		m_pNoteLookupTable = m_iNoteLookupTablePAL;

	// Number of cycles between each APU update
	m_iUpdateCycles = BaseFreq / Rate;

	// Setup note tables
	m_pChannels[CHANID_SQUARE1]->SetNoteTable(m_pNoteLookupTable);
	m_pChannels[CHANID_SQUARE2]->SetNoteTable(m_pNoteLookupTable);
	m_pChannels[CHANID_TRIANGLE]->SetNoteTable(m_pNoteLookupTable);
	m_pChannels[CHANID_MMC5_SQUARE1]->SetNoteTable(m_iNoteLookupTableNTSC);
	m_pChannels[CHANID_MMC5_SQUARE2]->SetNoteTable(m_iNoteLookupTableNTSC);
	m_pChannels[CHANID_VRC6_PULSE1]->SetNoteTable(m_iNoteLookupTableNTSC);
	m_pChannels[CHANID_VRC6_PULSE2]->SetNoteTable(m_iNoteLookupTableNTSC);
	m_pChannels[CHANID_VRC6_SAWTOOTH]->SetNoteTable(m_iNoteLookupTableSaw);
	m_pChannels[CHANID_FDS]->SetNoteTable(m_iNoteLookupTableFDS);

	m_pChannels[CHANID_N106_CHAN1]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN2]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN3]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN4]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN5]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN6]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN7]->SetNoteTable(m_iNoteLookupTableN106);
	m_pChannels[CHANID_N106_CHAN8]->SetNoteTable(m_iNoteLookupTableN106);

	m_pChannels[CHANID_S5B_CH1]->SetNoteTable(m_iNoteLookupTableS5B);
	m_pChannels[CHANID_S5B_CH2]->SetNoteTable(m_iNoteLookupTableS5B);
	m_pChannels[CHANID_S5B_CH3]->SetNoteTable(m_iNoteLookupTableS5B);
}

int CSoundGen::FindNote(unsigned int Period) const
{
	for (int i = 0; i < NOTE_COUNT; ++i) {
		if (Period >= m_pNoteLookupTable[i])
			return i;
	}
	return 0;
}

unsigned int CSoundGen::GetPeriod(int Note) const
{
	return m_pNoteLookupTable[Note];
}

stDPCMState CSoundGen::GetDPCMState() const
{
	stDPCMState State;

	if (!m_bRunning) {
		State.DeltaCntr = 0;
		State.SamplePos = 0;
	}
	else {
		State.DeltaCntr = m_pAPU->GetDeltaCounter();
		State.SamplePos = m_pAPU->GetSamplePos();
	}

	return State;
}

void CSoundGen::PlayNote(int Channel, stChanNote *NoteData, int EffColumns)
{
	if (!NoteData)
		return;

	// Update the individual channel
	m_pChannels[Channel]->PlayNote(NoteData, EffColumns);
}

void CSoundGen::EvaluateGlobalEffects(stChanNote *NoteData, int EffColumns)
{
	// Handle global effects (effects that affects all channels)
	for (int i = 0; i < EffColumns; ++i) {
		unsigned char EffNum   = NoteData->EffNumber[i];
		unsigned char EffParam = NoteData->EffParam[i];

		switch (EffNum) {
			// Fxx: Sets speed to xx
			case EF_SPEED:
				if (!EffParam)
					++EffParam;
				if (EffParam > MIN_TEMPO)
					m_iTempo = EffParam;
				else
					m_iSpeed = EffParam;
				m_iTempoDecrement = (m_iTempo * 24) / m_iSpeed;  // 24 = 6 * 4
				break;

			// Bxx: Jump to pattern xx
			case EF_JUMP:
				m_iJumpToPattern = EffParam;
				FrameIsDone(1);
				if (m_bRendering)
					CheckRenderStop();
				FrameIsDone(m_iJumpToPattern);
				if (m_bRendering)
					CheckRenderStop();
				break;

			// Dxx: Skip to next track and start at row xx
			case EF_SKIP:
				m_iSkipToRow = EffParam;
				FrameIsDone(1);
				if (m_bRendering)
					CheckRenderStop();
				break;

			// Cxx: Halt playback
			case EF_HALT:
				m_bPlayerHalted = true;
				//HaltPlayer();
				if (m_bRendering) {
					// Unconditional stop
					m_iRenderedFrames++;
					m_bRequestRenderStop = true;
				}
				break;
		}
	}
}

// File rendering functions

//bool CSoundGen::RenderToFile(char *File, int SongEndType, int SongEndParam)
//{
//	if (m_bPlaying)
//		HaltPlayer();

//	m_iRenderEndWhen = (RENDER_END)SongEndType;
//	m_iRenderEndParam = SongEndParam;
//	m_iRenderedFrames = 0;

//	if (m_iRenderEndWhen == SONG_TIME_LIMIT) {
//		// This variable is stored in seconds, convert to frames
//		m_iRenderEndParam *= m_pTrackerView->GetDocument()->GetFrameRate();
//	}
//	else if (m_iRenderEndWhen == SONG_LOOP_LIMIT) {
//		m_iRenderEndParam *= m_pTrackerView->GetDocument()->GetFrameCount();
//	}

//	if (!m_wfWaveFile.OpenFile(File, theApp.GetSettings()->Sound.iSampleRate, theApp.GetSettings()->Sound.iSampleSize, 1)) {
//		AfxMessageBox(_T("Could not open output file!"));
//		return false;
//	}
//	else
//		PostThreadMessage(M_START_RENDER, 0, 0);

//	return true;
//}

//void CSoundGen::StopRendering()
//{
//	if (!IsRendering())
//		return;

//	// Send this as message instead

//	m_bRendering = false;
//	m_pTrackerView->PlayerCommand(CMD_MOVE_TO_START, 0);
//	m_wfWaveFile.CloseFile();

//	ResetBuffer();
//}

//void CSoundGen::GetRenderStat(int &Frame, int &Time, bool &Done)
//{
//	Frame = m_iRenderedFrames;
//	Time = m_iPlayTime / m_pTrackerView->GetDocument()->GetFrameRate();
//	Done = m_bRendering;
//}

bool CSoundGen::IsRendering()
{
	return m_bRendering;
}

//void CSoundGen::SongIsDone()
//{
//	if (IsRendering())
//		CheckRenderStop();
//}

void CSoundGen::FrameIsDone(int SkipFrames)
{
	if (m_bRequestRenderStop)
		return;

	m_iRenderedFrames += SkipFrames;
}

void CSoundGen::CheckRenderStop()
{
	if (m_bRequestRenderStop)
		return;

	if (m_iRenderEndWhen == SONG_LOOP_LIMIT) {
		if (m_iRenderedFrames >= m_iRenderEndParam)
			m_bRequestRenderStop = true;
	}
}

// DPCM handling

void CSoundGen::PlaySample(CDSample *pSample, int Offset, int Pitch)
{
	SAFE_RELEASE(m_pPreviewSample);

	m_pSampleMem->SetMem(pSample->SampleData, pSample->SampleSize);

	int Loop = 0;
	int Length = ((pSample->SampleSize - 1) >> 4) - (Offset << 2);

	m_pAPU->Write(0x4010, Pitch | Loop);
	m_pAPU->Write(0x4012, Offset);			// load address, start at $C000
	m_pAPU->Write(0x4013, Length);			// length
	m_pAPU->Write(0x4015, 0x0F);
	m_pAPU->Write(0x4015, 0x1F);			// fire sample

	// Auto-delete samples with no name
	if (pSample->Name[0] == 0)
		m_pPreviewSample = pSample;
}

//void CSoundGen::PreviewSample(CDSample *pSample, int Offset, int Pitch)
//{
//	// Preview a DPCM sample. If the name of sample is null,
//	// the sample will be removed after played
//	PostThreadMessage(M_PREVIEW_SAMPLE, (WPARAM)pSample, MAKELPARAM(Offset, Pitch));
//}

//bool CSoundGen::PreviewDone() const
//{
//	return (m_pAPU->DPCMPlaying() == false);
//}

//void CSoundGen::WriteAPU(int Address, char Value)
//{
//	// Direct APU interface
//	PostThreadMessage(M_WRITE_APU, (WPARAM)Address, (LPARAM)Value);
//}

// Synchronization

//void CSoundGen::LockDocument()
//{
//	m_csDocumentLock.Lock();
//}

//void CSoundGen::UnlockDocument()
//{
//	m_csDocumentLock.Unlock();
//}

bool CSoundGen::WaitForStop() const
{
	// Wait for player to stop, timeout = 2s

	for (int i = 0; i < 20 && m_bPlaying; ++i)
		QThread::sleep(100);

	return !m_bPlaying;	// return false if still playing
}
