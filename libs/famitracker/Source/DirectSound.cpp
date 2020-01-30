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
// DirectSound Interface
//

#include "stdafx.h"
#include <cstdio>
#include "Common.h"
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
static unsigned int   m_iTotalSamples = 0;

QSemaphore ftmAudioSemaphore(0);

QList<SDL_Callback> sdlHooks;

bool SDL_Callback::operator==(const SDL_Callback &other) const
{
   if ( other._id == this->_id ) return true;
   return false;
}


extern bool invisibleFamiTracker;

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
   qDebug(str.toLatin1().constData());
#endif
   
   if ( invisibleFamiTracker )
   {
       memset(stream,0,len);
       if ( m_pSoundBuffer &&
            m_iTotalSamples )
          memcpy(stream,m_pSoundBuffer,len);
   }
   else
   {
       if ( m_pSoundBuffer )
          SDL_MixAudio(stream,m_pSoundBuffer,len,SDL_MIX_MAXVOLUME);
   }
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

bool CDSound::SetupDevice(int iDevice)
{	
   SDL_Init ( SDL_INIT_AUDIO );
	
	return true;
}

void CDSound::CloseDevice()
{
   SDL_Quit();
}

void CDSound::ClearEnumeration()
{
	m_iDevices = 0;
}

BOOL CDSound::EnumerateCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext)
{
	return TRUE;
}

void CDSound::EnumerateDevices()
{
	if (m_iDevices != 0)
		ClearEnumeration();

    qDebug("Hook SDL2 here?");
}

unsigned int CDSound::GetDeviceCount() const
{
	return m_iDevices;
}

LPCTSTR CDSound::GetDeviceName(unsigned int iDevice) const
{
	ASSERT(iDevice < m_iDevices);
	return m_pcDevice[iDevice];
}

int CDSound::MatchDeviceID(LPCTSTR Name) const
{
	for (unsigned int i = 0; i < m_iDevices; ++i) {
		if (!_tcscmp(Name, m_pcDevice[i]))
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
   
	// Adjust buffer length in case a buffer would end up in half samples
	while ((SampleRate * BufferLength / (Blocks * 1000) != (double)SampleRate * BufferLength / (Blocks * 1000)))
		++BufferLength;
 
	CDSoundChannel *pChannel = new CDSoundChannel();

	HANDLE hBufferEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	int SoundBufferSize = CalculateBufferLength(BufferLength, SampleRate, SampleSize, Channels);
	int BlockSize = SoundBufferSize / Blocks;

    if ( !invisibleFamiTracker )
    {
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

       SDL_OpenAudio ( &sdlAudioSpecIn, &sdlAudioSpecOut );

       //   qDebug("Adjusting audio: %d",memcmp(&sdlAudioSpecIn,&sdlAudioSpecOut,sizeof(sdlAudioSpecIn)));
       SoundBufferSize = sdlAudioSpecOut.samples*sdlAudioSpecOut.channels*((sdlAudioSpecOut.format==AUDIO_U8?8:16)>>3);
       BlockSize = SoundBufferSize / Blocks;
    }
    else
    {
        SoundBufferSize = 1152;
        BlockSize = 1152;
    }
   
   if ( m_pSoundBuffer )
      delete m_pSoundBuffer;
   m_pSoundBuffer = new unsigned char[SoundBufferSize];
   memset(m_pSoundBuffer,0,SoundBufferSize);
   m_iSoundProducer = 0;
   m_iTotalSamples = 0;
   m_iSoundConsumer = 0;
   m_iSoundBufferSize = SoundBufferSize;
   
	pChannel->m_iBufferLength		= BufferLength;			// in ms
	pChannel->m_iSoundBufferSize	= SoundBufferSize;		// in bytes
	pChannel->m_iBlockSize			= BlockSize;			// in bytes
	pChannel->m_iBlocks				= Blocks;
	pChannel->m_iSampleSize			= SampleSize;
	pChannel->m_iSampleRate			= SampleRate;
	pChannel->m_iChannels			= Channels;

	pChannel->m_iCurrentWriteBlock	= 0;

	pChannel->ClearBuffer();

    // SDL...
    pChannel->Play();

    if ( !invisibleFamiTracker )
    {
        SDL_PauseAudio(0);
    }

	return pChannel;
}

void CDSound::CloseChannel(CDSoundChannel *pChannel)
{
    if ( !invisibleFamiTracker )
    {
       SDL_PauseAudio ( 1 );

       SDL_CloseAudio ();
    }

	if (pChannel == NULL)
		return;

	delete pChannel;
}

// CDSoundChannel

CDSoundChannel::CDSoundChannel()
{
   m_bPaused = true;
	m_iCurrentWriteBlock = 0;
}

CDSoundChannel::~CDSoundChannel()
{
}

bool CDSoundChannel::Play()
{
    m_iTotalSamples = 0;
    m_bPaused = false;
   ftmAudioSemaphore.release();
   return true;
}

bool CDSoundChannel::Stop()
{
   m_bPaused = true;
   return true;
}

bool CDSoundChannel::IsPlaying() const
{
   return m_bPaused;
}

bool CDSoundChannel::ClearBuffer()
{
	if (IsPlaying())
        return Stop();
}

bool CDSoundChannel::WriteBuffer(char *pBuffer, unsigned int Samples)
{   
   memcpy(m_pSoundBuffer+m_iSoundProducer,pBuffer,Samples);
   m_iSoundProducer += Samples;
   m_iTotalSamples = 1;
   m_iSoundProducer %= m_iSoundBufferSize;
   return true;
}

buffer_event_t CDSoundChannel::WaitForSyncEvent(DWORD dwTimeout) const
{
	// Wait for a DirectSound event
   bool ok = ftmAudioSemaphore.tryAcquire(1,dwTimeout);
   return ok?BUFFER_IN_SYNC:BUFFER_OUT_OF_SYNC;
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
