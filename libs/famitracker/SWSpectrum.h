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

#include "SampleWindow.h"
#include "FFT/Fft.h"

const int FFT_POINTS = 256;

class CSWSpectrum : public CSampleWinState
{
public:
	CSWSpectrum();
	~CSWSpectrum();

	void Activate();
	void Deactivate();
	void SetSampleRate(int SampleRate);
	void SetSampleData(int *iSamples, unsigned int iCount);
	void Draw(CDC *pDC, bool bMessage);

private:
	unsigned int m_iCount;

	int	*m_pSamples;
	int	*m_pBlitBuffer;
	int	m_iWindowBufPtr, *m_pWindowBuf;

	int	m_iLogTable[WIN_HEIGHT];

	int m_iFillPos;

	BITMAPINFO bmi;

	Fft	*m_pFftObject;
	int	m_iFftPoint[FFT_POINTS];

	int m_pSampleBuffer[FFT_POINTS];
};
