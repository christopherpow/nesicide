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

#include <memory>
#include "../common.h"
#include "APU.h"
#include "N106.h"

//
// N106
//

CN106::CN106(CMixer *pMixer) : m_iChansInUse(0)
{
	m_pWaveData = new uint8[0x40];

	m_pChannels[0] = new CN106Chan(pMixer, CHANID_N106_CHAN1, m_pWaveData);
	m_pChannels[1] = new CN106Chan(pMixer, CHANID_N106_CHAN2, m_pWaveData);
	m_pChannels[2] = new CN106Chan(pMixer, CHANID_N106_CHAN3, m_pWaveData);
	m_pChannels[3] = new CN106Chan(pMixer, CHANID_N106_CHAN4, m_pWaveData);
	m_pChannels[4] = new CN106Chan(pMixer, CHANID_N106_CHAN5, m_pWaveData);
	m_pChannels[5] = new CN106Chan(pMixer, CHANID_N106_CHAN6, m_pWaveData);
	m_pChannels[6] = new CN106Chan(pMixer, CHANID_N106_CHAN7, m_pWaveData);
	m_pChannels[7] = new CN106Chan(pMixer, CHANID_N106_CHAN8, m_pWaveData);
}

CN106::~CN106()
{
	if (m_pWaveData)
		delete [] m_pWaveData;

	for (int i = 0; i < 8; ++i)
		delete m_pChannels[i];
}

void CN106::Reset()
{
	for (int i = 0; i < 8; ++i)
		m_pChannels[i]->Reset();
}

void CN106::Process(uint32 Time)
{
	for (int i = 7 - m_iChansInUse; i < 8; ++i)
		m_pChannels[i]->Process(Time, m_iChansInUse);
}

void CN106::EndFrame()
{
	for (int i = 0; i < 8; ++i)
		m_pChannels[i]->EndFrame();
}

void CN106::Write(uint16 Address, uint8 Value)
{
	int Area = ExpandAddr & 0x7F;

	switch (Address) {
		case 0x4800:
			if (Area < 0x40)
				m_pWaveData[Area] = Value;

			else if (Area >= 0x40) {
				int Channel = (Area & 0x3F) >> 3;
				m_pChannels[Channel]->Write(Area & 0x07, Value);

				if (Area == 0x7F)
					m_iChansInUse = (Value >> 4) & 0x07;
			}

			if (ExpandAddr & 0x80)
				ExpandAddr = (ExpandAddr + 1) | 0x80;
			break;

		case 0xF800:
			ExpandAddr = Value;
			break;
	}

}

uint8 CN106::Read(uint16 Address, bool &Mapped)
{
	uint16 ReadAddr;

	switch (Address) {
		case 0x4800:
			Mapped = true;
			ReadAddr = ExpandAddr & 0x3F;
			if (ExpandAddr & 0x80)
				ExpandAddr = ((ExpandAddr + 1) & 0x7F) | 0x80;
			return m_pWaveData[ReadAddr];
	}

	return 0;
}

//
// N106 channels
//

CN106Chan::CN106Chan(CMixer *pMixer, int ID, uint8 *pWaveData) : 
	CExChannel(pMixer, SNDCHIP_N106, ID),
	m_pWaveData(pWaveData)
{
}

CN106Chan::~CN106Chan()
{
}

void CN106Chan::Reset()
{
	m_iWavePtr	  = 0;
	m_iCounter	  = 0;
	m_iWaveOffset = 0;
	m_iWaveLength = 0x20;

	m_iFreqs[0] = 0;
	m_iFreqs[1] = 0;
	m_iFreqs[2] = 0;

	m_iVolume = 0;

	EndFrame();
}

void CN106Chan::Write(uint16 Address, uint8 Value)
{
	switch (Address) {
		case 0x00:
			m_iFreqs[0] = Value;
			break;
		case 0x02:
			m_iFreqs[1] = Value;
			break;
		case 0x04:
			m_iFreqs[2] = Value & 3;
//			m_iWaveLength = (8 - ((Value >> 2) & 0x07)) << 2;
			m_iWaveLength = 32 - (Value & 0x1C);
			break;
		case 0x06:
			m_iWaveOffset = Value;
			break;
		case 0x07:
			m_iVolume = Value & 0x0F;
			break;
	}
}

void CN106Chan::Process(uint32 Time, uint8 ChannelsActive)
{
	uint8	Sample;
	uint32	Period;

	m_iFrequency = m_iFreqs[0] | (m_iFreqs[1] << 8) | (m_iFreqs[2] << 16);

	if (!m_iFrequency)
		return;

	Period = (uint32)(((((ChannelsActive + 1) * 45 * 0x40000) / (float)21477270) * (float)CAPU::BASE_FREQ_NTSC) / m_iFrequency);

	if (!m_iWaveLength || !m_iVolume || !Period)
		return;

	while (Time >= m_iCounter) {
		m_iTime += m_iCounter;
		Time -= m_iCounter;
		m_iCounter = Period;
		
		Sample = m_pWaveData[((m_iWavePtr + m_iWaveOffset) & 0x7F) >> 1];

		if (m_iWavePtr & 1)
			Sample >>= 4;

		if (++m_iWavePtr >= m_iWaveLength)
			m_iWavePtr = 0;

		Mix((Sample & 0x0F) * m_iVolume);
	}
	
	m_iCounter -= Time;
	m_iTime += Time;
}
