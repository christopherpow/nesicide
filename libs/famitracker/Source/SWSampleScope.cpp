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
#include <cmath>
#include "FamiTracker.h"
#include "SampleWindow.h"
#include "SWSampleScope.h"
#include "Graphics.h"

CSWSampleScope::CSWSampleScope(bool bBlur) :
	m_pBlitBuffer(NULL),
	m_pWindowBuf(NULL),
	m_iCount(0),
	m_bBlur(bBlur)
{
}

CSWSampleScope::~CSWSampleScope()
{
	SAFE_RELEASE_ARRAY(m_pBlitBuffer);
	SAFE_RELEASE_ARRAY(m_pWindowBuf);
}

void CSWSampleScope::Activate()
{
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize	 = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biHeight	 = WIN_HEIGHT;
	bmi.bmiHeader.biWidth	 = WIN_WIDTH;
	bmi.bmiHeader.biPlanes	 = 1;

	SAFE_RELEASE_ARRAY(m_pBlitBuffer);

	m_pBlitBuffer = new int[WIN_WIDTH * WIN_HEIGHT * 2];
	memset(m_pBlitBuffer, 0, WIN_WIDTH * WIN_HEIGHT * sizeof(int));

	SAFE_RELEASE_ARRAY(m_pWindowBuf);

	m_pWindowBuf = new int[WIN_WIDTH];
	m_iWindowBufPtr = 0;
}

void CSWSampleScope::Deactivate()
{
	SAFE_RELEASE_ARRAY(m_pBlitBuffer);
	SAFE_RELEASE_ARRAY(m_pWindowBuf);
}

void CSWSampleScope::SetSampleRate(int SampleRate)
{
}

void CSWSampleScope::SetSampleData(int *pSamples, unsigned int iCount)
{
	m_iCount = iCount;
	m_pSamples = pSamples;
}

void CSWSampleScope::Draw(CDC *pDC, bool bMessage)
{
	/*
	int GraphColor		= theApp.m_pSettings->Appearance.iColPatternText;
	int GraphColor2		= DIM(theApp.m_pSettings->Appearance.iColPatternText, 50);
	int GraphBgColor	= theApp.m_pSettings->Appearance.iColBackground;
	*/

#ifdef _DEBUG
	static int _peak = 0;
	static int _min = 0;
	static int _max = 0;
#endif

	COLORREF GraphColor	  = 0xFFFFFF;
	COLORREF GraphColor2  = DIM(GraphColor, 50);
	COLORREF GraphBgColor = 0x000000;

	static COLORREF BackColors[WIN_HEIGHT];

	if (BackColors[0] == 0) {
		for (int y = 0; y < WIN_HEIGHT; y++) {
			BackColors[y] = DIM(GraphColor, (int)(sinf( (((y * 100) / WIN_HEIGHT) * 2 * 3.14f) / 100 + 1.8f) * 10 + 10));
		}
	}

	unsigned int i = 0;

	if (bMessage)
		return;

	GraphBgColor = ((GraphBgColor & 0xFF) << 16) | (GraphBgColor & 0xFF00) | ((GraphBgColor & 0xFF0000) >> 16);

	while (i < m_iCount) {

#ifdef _DEBUG
		if (_min > m_pSamples[i])
			_min = m_pSamples[i];
		if (_max < m_pSamples[i])
			_max = m_pSamples[i];
		if (abs(m_pSamples[i]) > _peak)
			_peak = abs(m_pSamples[i]);
#endif

		m_pWindowBuf[m_iWindowBufPtr++ / 7] = m_pSamples[i++];

		if ((m_iWindowBufPtr / 7) >= WIN_WIDTH) {
			m_iWindowBufPtr = 0;
			int x = 0;
			int y = 0;
			int s;
			int l;

			l = (m_pWindowBuf[0] / 1000) + (WIN_HEIGHT / 2) - 1;

			for (x = 0; x < WIN_WIDTH; x++) {
				s = (m_pWindowBuf[x] / 1000) + (WIN_HEIGHT / 2) - 1;

				for (y = 0; y < WIN_HEIGHT; y++) {
					if ((y == s) || ((y > s && y <= l) || (y >= l && y < s))) {
						if (m_bBlur) {
							m_pBlitBuffer[(y + 0) * WIN_WIDTH + x] = 0xFFFFFF;
						}
						else {
							if (y > 2)
								m_pBlitBuffer[(y - 2) * WIN_WIDTH + x] = GraphColor2;
							if (y > 1)
								m_pBlitBuffer[(y - 1) * WIN_WIDTH + x] = GraphColor;
							m_pBlitBuffer[(y + 0) * WIN_WIDTH + x] = GraphColor2;
						}
					}
					else {
						if (m_bBlur) {
							if (y > 1 && y < (WIN_HEIGHT - 1) && x > 0 && x < (WIN_WIDTH - 5)) {
								const int BLUR_DECAY = 13;
								int Col1 = m_pBlitBuffer[(y + 1) * WIN_WIDTH + x];
								int Col2 = m_pBlitBuffer[(y - 1) * WIN_WIDTH + x];
								int Col3 = m_pBlitBuffer[y * WIN_WIDTH + (x + 1)];
								int Col4 = m_pBlitBuffer[y * WIN_WIDTH + (x - 1)];
								int Col5 = m_pBlitBuffer[(y - 1) * WIN_WIDTH + (x + 1)];
								int Col6 = m_pBlitBuffer[(y - 1) * WIN_WIDTH + (x - 1)];
								int Col7 = m_pBlitBuffer[(y + 1) * WIN_WIDTH + (x + 1)];
								int Col8 = m_pBlitBuffer[(y + 1) * WIN_WIDTH + (x - 1)];

								int r = ((Col1 >> 16) & 0xFF) + ((Col2 >> 16) & 0xFF) + ((Col3 >> 16) & 0xFF) + 
									((Col4 >> 16) & 0xFF) + ((Col5 >> 16) & 0xFF) + ((Col6 >> 16) & 0xFF) + 
									((Col7 >> 16) & 0xFF) + ((Col8 >> 16) & 0xFF);

								int g = ((Col1 >> 8) & 0xFF) + ((Col2 >> 8) & 0xFF) + ((Col3 >> 8) & 0xFF) + 
									((Col4 >> 8) & 0xFF) + ((Col5 >> 8) & 0xFF) + ((Col6 >> 8) & 0xFF) + 
									((Col7 >> 8) & 0xFF) + ((Col8 >> 8) & 0xFF);

								int b = ((Col1) & 0xFF) + ((Col2) & 0xFF) + ((Col3) & 0xFF) + 
									((Col4) & 0xFF) + ((Col5) & 0xFF) + ((Col6) & 0xFF) + 
									((Col7) & 0xFF) + ((Col8) & 0xFF);

								r = r / 8 - BLUR_DECAY / 1;
								g = g / 8 - BLUR_DECAY / 1;
								b = b / 8 - BLUR_DECAY / 2;

								if (r < 0) r = 0;
								if (g < 0) g = 0;
								if (b < 0) b = 0;

								m_pBlitBuffer[y * WIN_WIDTH + x] = (r << 16) + (g << 8) + b;
								//BlitBuffer[y * WIN_WIDTH + x] = DIM(BlitBuffer[y * WIN_WIDTH + x], 70);
							}
							else {
								m_pBlitBuffer[y * WIN_WIDTH + x] = GraphBgColor;
							}
						}
						else {
							//BlitBuffer[y * WIN_WIDTH + x] = GraphBgColor;
							//m_pBlitBuffer[y * WIN_WIDTH + x] = DIM(GraphColor, (int)(sinf( (((y * 100) / WIN_HEIGHT) * 2 * 3.14f) / 100 + 1.8f) * 15 + 15));
							m_pBlitBuffer[y * WIN_WIDTH + x] = BackColors[y];
						}
					}
				}
				l = s;
			}
			StretchDIBits(*pDC, 0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT, m_pBlitBuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);

#ifdef _DEBUG
			CString PeakText;
			_peak = _max - _min;
			PeakText.Format(_T("%i"), _peak);
			pDC->TextOut(0, 0, PeakText);
			PeakText.Format(_T("-%gdB"), 20.0 * log(double(_peak) / 65535.0));
			pDC->TextOut(0, 16, PeakText);
			_peak = 0;
			_min = 0;
			_max = 0;
#endif
			
		}
	}	
}
