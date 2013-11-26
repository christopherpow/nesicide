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

#include "stdafx.h"
#include "FamiTracker.h"
#include "SampleWindow.h"
#include "SWSpectrum.h"
#include "Graphics.h"

CSWSpectrum::CSWSpectrum() :
	m_pBlitBuffer(NULL),
	m_pFftObject(NULL)
{
	m_iLogTable[0] = 0;
	for (int i = 1; i < WIN_HEIGHT; ++i) {
		m_iLogTable[WIN_HEIGHT - i] = -int(20.0f * log10f(float(i) / float(WIN_HEIGHT)));
	}
	m_iLogTable[WIN_HEIGHT] = WIN_HEIGHT;
}

CSWSpectrum::~CSWSpectrum()
{
	SAFE_RELEASE_ARRAY(m_pBlitBuffer);
	SAFE_RELEASE(m_pFftObject);
}

void CSWSpectrum::Activate()
{
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize	 = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biHeight	 = -WIN_HEIGHT;
	bmi.bmiHeader.biWidth	 = WIN_WIDTH;
	bmi.bmiHeader.biPlanes	 = 1;

	SAFE_RELEASE_ARRAY(m_pBlitBuffer);

	m_pBlitBuffer = new int[WIN_WIDTH * WIN_HEIGHT * 2];
	memset(m_pBlitBuffer, 0, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	SAFE_RELEASE(m_pFftObject);

//	m_pFftObject = new Fft(FFT_POINTS, 44100);

	// Set default sample-rate
	SetSampleRate(44100);
}

void CSWSpectrum::Deactivate()
{
	SAFE_RELEASE_ARRAY(m_pBlitBuffer);
	SAFE_RELEASE(m_pFftObject);
}

void CSWSpectrum::SetSampleRate(int SampleRate)
{
	SAFE_RELEASE(m_pFftObject);

	m_pFftObject = new Fft(FFT_POINTS, SampleRate);

	memset(m_iFftPoint, 0, sizeof(int) * FFT_POINTS);

	m_iCount = 0;
	m_iFillPos = 0;
}

void CSWSpectrum::SetSampleData(int *pSamples, unsigned int iCount)
{
	m_iCount = iCount;
	m_pSamples = pSamples;

	int size, offset = 0;

	if (m_iFillPos > 0) {
		size = FFT_POINTS - m_iFillPos;
		memcpy(m_pSampleBuffer + m_iFillPos, pSamples, size*4);
		m_pFftObject->CopyIn(FFT_POINTS, m_pSampleBuffer);
		m_pFftObject->Transform();
		offset += size;
		iCount -= size;
	}

	while (iCount >= FFT_POINTS) {
		m_pFftObject->CopyIn(FFT_POINTS, pSamples + offset);
		m_pFftObject->Transform();
		offset += FFT_POINTS;
		iCount -= FFT_POINTS;
	}

	// Copy rest
	size = iCount;
	memcpy(m_pSampleBuffer, pSamples + offset, size*4);
	m_iFillPos = size;
}

void CSWSpectrum::Draw(CDC *pDC, bool bMessage)
{
	unsigned int i = 0; int y, bar;

	if (bMessage)
		return;

	float Stepping = (float)(FFT_POINTS) / (float(WIN_WIDTH) * 4.0f);
	float Step = 0;

	for (i = 0; i < WIN_WIDTH; i++) {
		bar = (int)m_pFftObject->GetIntensity(int(Step)) / 400;

		if (bar < 0)
			bar = 0;
		if (bar > WIN_HEIGHT)
			bar = WIN_HEIGHT;

		if (bar > m_iFftPoint[(int)Step])
			m_iFftPoint[(int)Step] = bar;
		else
			m_iFftPoint[(int)Step] -= 2;

		bar = m_iFftPoint[(int)Step];

		for (y = 0; y < WIN_HEIGHT; y++) {
			if (y < bar)
				m_pBlitBuffer[(WIN_HEIGHT - y) * WIN_WIDTH + i] = 0xFFFFFF - (DIM(0xFFFF80, (y * 100) / bar) + 0x80) + 0x000080;
			else
				m_pBlitBuffer[(WIN_HEIGHT - y) * WIN_WIDTH + i] = 0x000000;
		}

		Step += Stepping;
	}

	StretchDIBits(*pDC, 0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT, m_pBlitBuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
}
