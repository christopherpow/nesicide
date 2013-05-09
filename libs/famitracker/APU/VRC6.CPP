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

#include <memory>
#include "APU.h"
#include "VRC6.h"

// Konami VRC6 external sound chip emulation

void CVRC6_Pulse::Reset()
{
	DutyCycle = Volume = Gate = Enabled = 0;
	Frequency = FreqLow = FreqHigh = 0;
	Counter = 0;
	DutyCycleCounter = 0;
}

void CVRC6_Pulse::Write(uint16 Address, uint8 Value)
{
	switch (Address) {
		case 0x00:
            Gate = Value & 0x80;
            DutyCycle = ((Value & 0x70) >> 4) + 1;
            Volume = Value & 0x0F;
			if (Gate)
				Mix(Volume);
  			break;
		case 0x01:
            FreqLow = Value;
            Frequency = FreqLow + (FreqHigh << 8);
			break;
		case 0x02:
            Enabled = (Value & 0x80);
            FreqHigh = (Value & 0x0F);
            Frequency = FreqLow + (FreqHigh << 8);
			break;
	}
}

void CVRC6_Pulse::Process(int Time)
{
	if (!Enabled || Frequency == 0) {
		m_iTime += Time;
		return;
	}

	while (Time >= Counter) {
		Time    -= Counter;
		m_iTime	+= Counter;
		Counter	 = Frequency + 1;
	
		DutyCycleCounter = (DutyCycleCounter + 1) & 0x0F;
		Mix((Gate || DutyCycleCounter >= DutyCycle) ? Volume : 0);
	}

	Counter -= Time;
	m_iTime += Time;
}

void CVRC6_Sawtooth::Reset()
{
	PhaseAccumulator = PhaseInput = Enabled = ResetReg = 0;
	Frequency = 0;
	FreqLow = FreqHigh = 0;
	Counter = 0;
}

void CVRC6_Sawtooth::Write(uint16 Address, uint8 Value)
{
	switch (Address) {
		case 0x00:
            PhaseInput = (Value & 0x3F);
			break;
		case 0x01:
            FreqLow = Value;
            Frequency = FreqLow + (FreqHigh << 8);
			break;
		case 0x02:
            Enabled = (Value & 0x80);
            FreqHigh = (Value & 0x0F);
            Frequency = FreqLow + (FreqHigh << 8);
			break;
	}
}

void CVRC6_Sawtooth::Process(int Time)
{
	if (!Enabled || !Frequency) {
		m_iTime += Time;
		return;
	}

	while (Time >= Counter) {
		Time 	-= Counter;
		m_iTime	+= Counter;
		Counter	 = Frequency + 1;

		if (ResetReg & 1)
			PhaseAccumulator = (PhaseAccumulator + PhaseInput) & 0xFF;

		ResetReg++;

		if (ResetReg == 14) {
			PhaseAccumulator = 0;
			ResetReg = 0;
		}

		// The 5 highest bits of accumulator are sent to the mixer
		Mix(PhaseAccumulator >> 3);
	}

	Counter -= Time;
	m_iTime += Time;
}

CVRC6::CVRC6(CMixer *pMixer)
{
	Pulse1 = new CVRC6_Pulse(pMixer, CHANID_VRC6_PULSE1);
	Pulse2 = new CVRC6_Pulse(pMixer, CHANID_VRC6_PULSE2);
	Sawtooth = new CVRC6_Sawtooth(pMixer, CHANID_VRC6_SAWTOOTH);
}

CVRC6::~CVRC6()
{
	if (Pulse1)
		delete Pulse1;

	if (Pulse2)
		delete Pulse2;

	if (Sawtooth)
		delete Sawtooth;
}

void CVRC6::Reset()
{
	Pulse1->Reset();
	Pulse2->Reset();
	Sawtooth->Reset();
}

void CVRC6::Write(uint16 Address, uint8 Value)
{
	switch (Address) {
		case 0x9000:
		case 0x9001:
		case 0x9002:
			Pulse1->Write(Address & 3, Value);
			break;			
		case 0xA000:
		case 0xA001:
		case 0xA002:
			Pulse2->Write(Address & 3, Value);
			break;
		case 0xB000:
		case 0xB001:
		case 0xB002:
			Sawtooth->Write(Address & 3, Value);
			break;
	}
}

uint8 CVRC6::Read(uint16 Address, bool &Mapped)
{
	Mapped = false;
	return 0;
}

void CVRC6::EndFrame()
{
	Pulse1->EndFrame();
	Pulse2->EndFrame();
	Sawtooth->EndFrame();
}

void CVRC6::Process(uint32 Time)
{
	Pulse1->Process(Time);
	Pulse2->Process(Time);
	Sawtooth->Process(Time);
}
