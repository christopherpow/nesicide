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

#include <QDebug>

// get rid of MFC crap
#define _T(x) x
#define TRACE0(x) qDebug(x);

// stubs for theApp for now
class CSoundGen;
class MFCCRAP
{
public:
   MFCCRAP(CSoundGen* pSG)
   {
      _pSG = pSG;
   }

   void RegisterKeyState(int Channel, int Note)
   {
      qDebug("::RegisterKeyState");
   }
   CSoundGen* GetSoundGenerator()
   {
      return _pSG;
   }
private:
   CSoundGen* _pSG;
};

extern MFCCRAP theApp;

#define LIMIT(v, max, min) v = ((v > max) ? max : ((v < min) ? min : v));//  if (v > max) v = max; else if (v < min) v = min;

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (b) : (a))

#ifdef QT_NO_DEBUG
#define ASSERT(y)
#else
#define ASSERT(y) { if (!(y)) qDebug("ASSERT!"); }
#endif

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;
typedef unsigned long long	uint64;
typedef signed char			int8;
typedef signed short		int16;
typedef signed long			int32;
typedef signed long long		int64;

#define _MAIN_H_

#define SAMPLES_IN_BYTES(x) (x << SampleSizeShift)

const int SPEED_AUTO	= 0;
const int SPEED_NTSC	= 1;
const int SPEED_PAL		= 2;

// Releasing pointers
#define SAFE_RELEASE(p) \
        if (p != NULL) { \
                delete p;       \
                p = NULL;       \
        }       \

#define SAFE_RELEASE_ARRAY(p) \
        if (p != NULL) { \
                delete [] p;    \
                p = NULL;       \
        }       \

// Used to play the audio when the buffer is full
class ICallback {
public:
	virtual void FlushBuffer(int16 *Buffer, uint32 Size) = 0;
};


// class for simulating CPU memory, used by the DPCM channel
class CSampleMem
{
	public:
		uint8 Read(uint16 Address) {
			if (!m_pMemory)
				return 0;
			uint16 Addr = (Address - 0xC000);// % m_iMemSize;
			if (Addr >= m_iMemSize)
				return 0;
			return m_pMemory[Addr];
		};

		void SetMem(char *Ptr, int Size) {
			m_pMemory = (uint8*)Ptr;
			m_iMemSize = Size;
		};

	private:
		uint8  *m_pMemory;
		uint16	m_iMemSize;
};
