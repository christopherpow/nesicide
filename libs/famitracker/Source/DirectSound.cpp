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

//
// DirectSound Interface
//

#include "stdafx.h"
#include <cstdio>
#include "common.h"
#include "DirectSound.h"
#include "resource.h"

// The single CDSound object
CDSound *CDSound::pThisObject = NULL;

// Class members

//BOOL CALLBACK CDSound::DSEnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext)
//{
//	return pThisObject->EnumerateCallback(lpGuid, lpcstrDescription, lpcstrModule, lpContext);
//}

// Instance members

CDSound::CDSound(HWND hWnd, HANDLE hNotification) :
	m_iDevices(0)//,
//	m_lpDirectSound(NULL),
//	m_hWndTarget(hWnd),
//	m_hNotificationHandle(hNotification)
{
	ASSERT(pThisObject == NULL);
	pThisObject = this;
}

CDSound::~CDSound()
{
//	for (int i = 0; i < (int)m_iDevices; ++i) {
//		delete [] m_pcDevice[i];
//		delete [] m_pGUIDs[i];
//	}
}

static unsigned char* m_pSoundBuffer = NULL;
static unsigned int   m_iSoundProducer = 0;
static unsigned int   m_iSoundConsumer = 0;
static unsigned int   m_iSoundBufferSize = 0;

QSemaphore ftmAudioSemaphore(0);

QList<SDL_Callback> sdlHooks;

extern "C" void SDL_FamiTracker(void* userdata, uint8_t* stream, int32_t len)
{
   if ( !stream )
      return;
#if 0
   LARGE_INTEGER t;
   static LARGE_INTEGER to;
   LARGE_INTEGER f;
   QueryPerformanceFrequency(&f);
   QueryPerformanceCounter(&t);
   QString str;
   str.sprintf("Smp: %d, Freq: %d, Ctr: %d, Diff %d, Per %lf", len>>1, f.LowPart, t.LowPart,t.LowPart-to.LowPart,(float)(t.LowPart-to.LowPart)/(float)f.LowPart);
   to = t;
   qDebug(str.toAscii().constData());
#endif
   
   if ( m_pSoundBuffer )
      SDL_MixAudio(stream,m_pSoundBuffer,len,SDL_MIX_MAXVOLUME);
   m_iSoundConsumer += len;
   m_iSoundConsumer %= m_iSoundBufferSize;
   
   foreach ( SDL_Callback cb, sdlHooks )
   {
      if ( cb._valid )
      {
         cb._func(cb._user,stream,len);
      }
   }
   
   ftmAudioSemaphore.release();
}

bool CDSound::Init(int Device)
{	
   SDL_Init ( SDL_INIT_AUDIO );

//	if (Device > (int)m_iDevices)
//		Device = 0;
	
//	if (m_lpDirectSound) {
//		m_lpDirectSound->Release();
//		m_lpDirectSound = NULL;
//	}

//	if (FAILED(DirectSoundCreate((LPCGUID)m_pGUIDs[Device], &m_lpDirectSound, NULL))) {
//		m_lpDirectSound = NULL;
//		return false;
//	}

//	if (FAILED(m_lpDirectSound->SetCooperativeLevel(m_hWndTarget, DSSCL_PRIORITY)))
//		return false;
	
	return true;
}

void CDSound::Close()
{
//	if (!m_lpDirectSound)
//		return;

//	m_lpDirectSound->Release();
//	m_lpDirectSound = NULL;

//	if (m_iDevices != 0)
//		ClearEnumeration();
}

void CDSound::ClearEnumeration()
{
//	for (unsigned i = 0; i < m_iDevices; ++i) {
//		delete [] m_pcDevice[i];
//		if (m_pGUIDs[i] != NULL)
//			delete m_pGUIDs[i];
//	}

	m_iDevices = 0;
}

BOOL CDSound::EnumerateCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
//	m_pcDevice[m_iDevices] = new char[strlen(lpcstrDescription) + 1];
//	strcpy(m_pcDevice[m_iDevices], lpcstrDescription);

//	if (lpGuid != NULL) {
//		m_pGUIDs[m_iDevices] = new GUID;
//		memcpy(m_pGUIDs[m_iDevices], lpGuid, sizeof(GUID));
//	}
//	else
//		m_pGUIDs[m_iDevices] = NULL;

//	++m_iDevices;

	return TRUE;
}

void CDSound::EnumerateDevices()
{
	if (m_iDevices != 0)
		ClearEnumeration();

    qDebug("Hook SDL2 here?");
//	DirectSoundEnumerate(DSEnumCallback, NULL);
}

unsigned int CDSound::GetDeviceCount() const
{
	return m_iDevices;
}

char *CDSound::GetDeviceName(int iDevice) const
{
	return m_pcDevice[iDevice];
}

int CDSound::MatchDeviceID(char *Name) const
{
	for (unsigned int i = 0; i < m_iDevices; ++i) {
		if (!strcmp(Name, m_pcDevice[i]))
			return i;
	}

	return 0;
}

int CDSound::CalculateBufferLength(int BufferLen, int Samplerate, int Samplesize, int Channels) const
{
	// Calculate size of the buffer, in bytes
	return ((Samplerate * BufferLen) / 1000) * (Samplesize / 8) * Channels;
}

CDSoundChannel *CDSound::OpenChannel(int SampleRate, int SampleSize, int Channels, int BufferLength, int Blocks)
{
   SDL_AudioSpec sdlAudioSpecIn;
   SDL_AudioSpec sdlAudioSpecOut;
   
	// Open a new secondary buffer
	//

//	DSBPOSITIONNOTIFY	dspn[MAX_BLOCKS];
//	WAVEFORMATEX		wfx;
//	DSBUFFERDESC		dsbd;

//	if (!m_lpDirectSound)
//		return NULL;

	// Adjust buffer length in case a buffer would end up in half samples
	while ((SampleRate * BufferLength / (Blocks * 1000) != (double)SampleRate * BufferLength / (Blocks * 1000)))
		++BufferLength;
 
	CDSoundChannel *pChannel = new CDSoundChannel();

	HANDLE hBufferEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	int SoundBufferSize = CalculateBufferLength(BufferLength, SampleRate, SampleSize, Channels);
	int BlockSize = SoundBufferSize / Blocks;

   sdlAudioSpecIn.callback = SDL_FamiTracker;
   sdlAudioSpecIn.userdata = NULL;
   sdlAudioSpecIn.channels = Channels;
   if ( SampleSize == 8 )
   {
      sdlAudioSpecIn.format = AUDIO_U8;
   }
   else 
   {
      sdlAudioSpecIn.format = AUDIO_S16SYS;
   }
   sdlAudioSpecIn.freq = SampleRate;

   // Set up audio sample rate for video mode...
   sdlAudioSpecIn.samples = (BlockSize/(SampleSize>>3));
   qDebug("---------------------------------BufferSize: %d, BlockSize: %d, SampleSize: %d",SoundBufferSize,BlockSize,SampleSize);
   SDL_OpenAudio ( &sdlAudioSpecIn, &sdlAudioSpecOut );

   if ( !memcmp(&sdlAudioSpecIn,&sdlAudioSpecOut,sizeof(SDL_AudioSpec)) )
   {
      qDebug("SDL_OpenAudio didn't give us what we want...adjust!");
      SoundBufferSize = sdlAudioSpecOut.samples*(SampleSize>>3);
   }
   
   if ( m_pSoundBuffer )
      delete m_pSoundBuffer;
   m_pSoundBuffer = new unsigned char[SoundBufferSize];
   memset(m_pSoundBuffer,0,SoundBufferSize);
   m_iSoundProducer = 0;
   m_iSoundConsumer = 0;
   m_iSoundBufferSize = SoundBufferSize;
   
	pChannel->m_iBufferLength		= BufferLength;			// in ms
	pChannel->m_iSoundBufferSize	= SoundBufferSize;		// in bytes
	pChannel->m_iBlockSize			= BlockSize;			// in bytes
	pChannel->m_iBlocks				= Blocks;
	pChannel->m_iSampleSize			= SampleSize;
	pChannel->m_iSampleRate			= SampleRate;
	pChannel->m_iChannels			= Channels;

//	pChannel->m_iCurrentWriteBlock	= 0;
//	pChannel->m_hWndTarget			= m_hWndTarget;
//	pChannel->m_hEventList[0]		= m_hNotificationHandle;
//	pChannel->m_hEventList[1]		= hBufferEvent;

//	memset(&wfx, 0x00, sizeof(WAVEFORMATEX));
//	wfx.cbSize				= sizeof(WAVEFORMATEX);
//	wfx.nChannels			= Channels;
//	wfx.nSamplesPerSec		= SampleRate;
//	wfx.wBitsPerSample		= SampleSize;
//	wfx.nBlockAlign			= wfx.nChannels * (wfx.wBitsPerSample / 8);
//	wfx.nAvgBytesPerSec		= wfx.nSamplesPerSec * wfx.nBlockAlign;
//	wfx.wFormatTag			= WAVE_FORMAT_PCM;

//	memset(&dsbd, 0x00, sizeof(DSBUFFERDESC));
//	dsbd.dwSize			= sizeof(DSBUFFERDESC);
//	dsbd.dwBufferBytes	= SoundBufferSize;
//	dsbd.dwFlags		= DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
//	dsbd.lpwfxFormat	= &wfx;

//	if (FAILED(m_lpDirectSound->CreateSoundBuffer(&dsbd, &pChannel->m_lpDirectSoundBuffer, NULL))) {
//		delete pChannel;
//		return NULL;
//	}

//	// Setup notifications
//	for (int i = 0; i < Blocks; ++i) {
//		dspn[i].dwOffset = i * BlockSize;
//		dspn[i].hEventNotify = hBufferEvent;
//	}

//	if (FAILED(pChannel->m_lpDirectSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&pChannel->m_lpDirectSoundNotify))) {
//		delete pChannel;
//		return NULL;
//	}

//	if (FAILED(pChannel->m_lpDirectSoundNotify->SetNotificationPositions(Blocks, dspn))) {
//		delete pChannel;
//		return NULL;
//	}

	pChannel->Clear();

   // SDL...
   pChannel->Play();
   
   SDL_PauseAudio(0);
   
	return pChannel;
}

void CDSound::CloseChannel(CDSoundChannel *pChannel)
{
   SDL_PauseAudio ( 1 );
   
   SDL_CloseAudio ();
   
	if (pChannel == NULL)
		return;

//	pChannel->m_lpDirectSoundBuffer->Release();
//	pChannel->m_lpDirectSoundNotify->Release();

	delete pChannel;
}

// CDSoundChannel

CDSoundChannel::CDSoundChannel()
{
   m_bPaused = true;
	m_iCurrentWriteBlock = 0;

//	m_hEventList[0] = NULL;
//	m_hEventList[1] = NULL;
//	m_hWndTarget = NULL;
}

CDSoundChannel::~CDSoundChannel()
{
//	// Kill buffer event
//	if (m_hEventList[1])
//		CloseHandle(m_hEventList[1]);
}

void CDSoundChannel::Play()
{
   ftmAudioSemaphore.release();
   m_bPaused = false;
//	// Begin playback of buffer
//	m_lpDirectSoundBuffer->Play(NULL, NULL, DSBPLAY_LOOPING);
}

void CDSoundChannel::Stop()
{
   m_bPaused = true;
//	// Stop playback
//	m_lpDirectSoundBuffer->Stop();
	Reset();
}

void CDSoundChannel::Pause() 
{
   m_bPaused = true;
//	// Pause playback of buffer, doesn't reset cursors
//	m_lpDirectSoundBuffer->Stop();
}

bool CDSoundChannel::IsPlaying() const
{
   return m_bPaused;
//	DWORD Status;
//	m_lpDirectSoundBuffer->GetStatus(&Status);
//	return ((Status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING);
}

void CDSoundChannel::Clear()
{
//	DWORD	*AudioPtr1, *AudioPtr2;
//	DWORD	AudioBytes1, AudioBytes2;
//	HRESULT	hRes;

	if (IsPlaying())
		Stop();

//	m_lpDirectSoundBuffer->SetCurrentPosition(0);

//	hRes = m_lpDirectSoundBuffer->Lock(0, m_iSoundBufferSize, (void**)&AudioPtr1, &AudioBytes1, (void**)&AudioPtr2, &AudioBytes2, 0);
	
//	if FAILED(hRes) {
//		return;
//	}

//	if (m_iSampleSize == 8) {
//		memset(AudioPtr1, 0x80, AudioBytes1);
//		if (AudioPtr2)
//			memset(AudioPtr2, 0x80, AudioBytes2);
//	}
//	else {
//		memset(AudioPtr1, 0x00, AudioBytes1);
//		if (AudioPtr2)
//			memset(AudioPtr2, 0x00, AudioBytes2);
//	}

//	hRes = m_lpDirectSoundBuffer->Unlock((void*)AudioPtr1, AudioBytes1, (void*)AudioPtr2, AudioBytes2);

//	if FAILED(hRes)
//		return;

//	m_lpDirectSoundBuffer->SetCurrentPosition(0);
//	m_iCurrentWriteBlock = 0;
}

void CDSoundChannel::WriteSoundBuffer(void *Buffer, unsigned int Samples)
{   
   memcpy(m_pSoundBuffer+m_iSoundProducer,Buffer,Samples);
   m_iSoundProducer += Samples;
   m_iSoundProducer %= m_iSoundBufferSize;

   // Fill sound buffer
	//
	// Buffer	- Pointer to a buffer with samples
	// Samples	- Number of samples, in bytes
	//

//	DWORD	*AudioPtr1, *AudioPtr2;
//	DWORD	AudioBytes1, AudioBytes2;
//	HRESULT	hRes;
//	int		Block = m_iCurrentWriteBlock;

//	ASSERT(Samples == m_iBlockSize);
	
//	hRes = m_lpDirectSoundBuffer->Lock(Block * m_iBlockSize, m_iBlockSize, (void**)&AudioPtr1, &AudioBytes1, (void**)&AudioPtr2, &AudioBytes2, 0);

//	// Could not lock
//	if (FAILED(hRes))
//		return;

//	memcpy(AudioPtr1, Buffer, AudioBytes1);

//	if (AudioPtr2)
//		memcpy(AudioPtr2, (unsigned char*)Buffer + AudioBytes1, AudioBytes2);

//	hRes = m_lpDirectSoundBuffer->Unlock((void*)AudioPtr1, AudioBytes1, (void*)AudioPtr2, AudioBytes2);

//	if (FAILED(hRes))
//		return;

//	AdvanceWritePointer();
}

void CDSoundChannel::Reset()
{
//	// Reset playback from the beginning of the buffer
//	m_iCurrentWriteBlock = 0;
//	m_lpDirectSoundBuffer->SetCurrentPosition(0);
}

int  CDSoundChannel::WaitForDirectSoundEvent(DWORD dwTimeout) const
{
	// Wait for a DirectSound event
//	if (!IsPlaying())
//		Play();
   bool ok = ftmAudioSemaphore.tryAcquire(1,dwTimeout);
   return ok?BUFFER_IN_SYNC:BUFFER_OUT_OF_SYNC;

//	// Wait for events
//	switch (WaitForMultipleObjects(2, m_hEventList, FALSE, INFINITE)) {
//		case WAIT_OBJECT_0:			// External event
//			return CUSTOM_EVENT;
//		case WAIT_OBJECT_0 + 1:		// Direct sound buffer
//			return (GetWriteBlock() == m_iCurrentWriteBlock) ? BUFFER_OUT_OF_SYNC : BUFFER_IN_SYNC;
//	}

//	// Error
//	return 0;
}

//int CDSoundChannel::GetPlayBlock() const
//{
//	// Return the block where the play pos is
//	DWORD PlayPos, WritePos;
//	m_lpDirectSoundBuffer->GetCurrentPosition(&PlayPos, &WritePos);
//	return (PlayPos / m_iBlockSize);
//}

//int CDSoundChannel::GetWriteBlock() const
//{
//	// Return the block where the write pos is
//	DWORD PlayPos, WritePos;
//	m_lpDirectSoundBuffer->GetCurrentPosition(&PlayPos, &WritePos);
//	return (WritePos / m_iBlockSize);
//}

void CDSoundChannel::ResetWritePointer()
{
	m_iCurrentWriteBlock = 0;
}

void CDSoundChannel::AdvanceWritePointer()
{
	m_iCurrentWriteBlock = (m_iCurrentWriteBlock + 1) % m_iBlocks;
}
