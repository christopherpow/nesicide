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

// This code comes from nintendulator

#include <cmath>
#include <string.h>
#include "apu.h"
#include "fds.h"

#define LOG_BITS 12
#define LIN_BITS 7
#define LOG_LIN_BITS 30

uint32 LinearToLog(int32 l);
int32 LogToLinear(uint32 l, uint32 sft);
void LogTableInitialize(void);

static uint32 lineartbl[(1 << LIN_BITS) + 1];
static uint32 logtbl[1 << LOG_BITS];

uint32 LinearToLog(int32 l)
{
	return (l < 0) ? (lineartbl[-l] + 1) : lineartbl[l];
}

int32 LogToLinear(uint32 l, uint32 sft)
{
	int32 ret;
	uint32 ofs;
	l += sft << (LOG_BITS + 1);
	sft = l >> (LOG_BITS + 1);
	if (sft >= LOG_LIN_BITS) return 0;
	ofs = (l >> 1) & ((1 << LOG_BITS) - 1);
	ret = logtbl[ofs] >> sft;
	return (l & 1) ? -ret : ret;
}

void LogTableInitialize(void)
{
	static volatile uint32 initialized = 0;
	uint32 i;
	double a;
	if (initialized) return;
	initialized = 1;
	for (i = 0; i < (1 << LOG_BITS); i++)
	{
		a = (1 << LOG_LIN_BITS) / pow(2, i / (double)(1 << LOG_BITS));
		logtbl[i] = (uint32)a;
	}
	lineartbl[0] = LOG_LIN_BITS << LOG_BITS;
	for (i = 1; i < (1 << LIN_BITS) + 1; i++)
	{
		uint32 ua;
		a = i << (LOG_LIN_BITS - LIN_BITS);
		ua = (uint32)((LOG_LIN_BITS - (double(log(a)) / double(log(2.0)))) * (1 << LOG_BITS));
		lineartbl[i] = ua << 1;
	}
}


void FDSSoundInstall(void);
void FDSSelect(unsigned type);

#define FDS_DYNAMIC_BIAS 1


#define FM_DEPTH 0 /* 0,1,2 */
#define NES_BASECYCLES (21477270)
#define PGCPS_BITS (32-16-6)
#define EGCPS_BITS (12)
#define VOL_BITS 12

typedef struct {
	uint8 spd;
	uint8 cnt;
	uint8 mode;
	uint8 volume;
} FDS_EG;
typedef struct {
	uint32 spdbase;
	uint32 spd;
	uint32 freq;
} FDS_PG;
typedef struct {
	uint32 phase;
	int8 wave[0x40];
	uint8 wavptr;
	int8 output;
	uint8 disable;
	uint8 disable2;
} FDS_WG;
typedef struct {
	FDS_EG eg;
	FDS_PG pg;
	FDS_WG wg;
	uint8 bias;
	uint8 wavebase;
	uint8 d[2];
} FDS_OP;

typedef struct FDSSOUND_tag {
	FDS_OP op[2];
	uint32 phasecps;
	uint32 envcnt;
	uint32 envspd;
	uint32 envcps;
	uint8 envdisable;
	uint8 d[3];
	uint32 lvl;
	int32 mastervolumel[4];
	uint32 mastervolume;
	uint32 srate;
	uint8 reg[0x10];
} FDSSOUND;

static FDSSOUND fdssound;

static void FDSSoundWGStep(FDS_WG *pwg)
{
#if 0
	if (pwg->disable | pwg->disable2)
		pwg->output = 0;
	else
		pwg->output = pwg->wave[(pwg->phase >> (PGCPS_BITS+16)) & 0x3f];
#else
	if (pwg->disable || pwg->disable2) return;
	pwg->output = pwg->wave[(pwg->phase >> (PGCPS_BITS+16)) & 0x3f];
#endif
}

static void FDSSoundEGStep(FDS_EG *peg)
{
	if (peg->mode & 0x80) return;
	if (++peg->cnt <= peg->spd) return;
	peg->cnt = 0;
	if (peg->mode & 0x40)
		peg->volume += (peg->volume < 0x1f);
	else
		peg->volume -= (peg->volume > 0);
}


static int32 __fastcall FDSSoundRender(void)
{
	int32 output;
	/* Wave Generator */
	FDSSoundWGStep(&fdssound.op[1].wg);
	FDSSoundWGStep(&fdssound.op[0].wg);

	/* Frequency Modulator */
	fdssound.op[1].pg.spd = fdssound.op[1].pg.spdbase;
	if (fdssound.op[1].wg.disable)
		fdssound.op[0].pg.spd = fdssound.op[0].pg.spdbase;
	else
	{
		uint32 v1;
#if FDS_DYNAMIC_BIAS
		v1 = 0x10000 + ((int32)fdssound.op[1].eg.volume) * (((int32)((((uint8)fdssound.op[1].wg.output) + fdssound.op[1].bias) & 255)) - 64);
#else
		v1 = 0x10000 + ((int32)fdssound.op[1].eg.volume) * (((int32)((((uint8)fdssound.op[1].wg.output)                      ) & 255)) - 64);
#endif
		v1 = ((1 << 10) + v1) & 0xfff;
		v1 = (fdssound.op[0].pg.freq * v1) >> 10;
		fdssound.op[0].pg.spd = v1 * fdssound.phasecps;
	}

	/* Accumulator */
	output = fdssound.op[0].eg.volume;
	if (output > 0x20) output = 0x20;
	output = (fdssound.op[0].wg.output * output * fdssound.mastervolumel[fdssound.lvl]) >> (VOL_BITS - 4);

	/* Envelope Generator */
	if (!fdssound.envdisable && fdssound.envspd)
	{
		fdssound.envcnt += fdssound.envcps;
		while (fdssound.envcnt >= fdssound.envspd)
		{
			fdssound.envcnt -= fdssound.envspd;
			FDSSoundEGStep(&fdssound.op[1].eg);
			FDSSoundEGStep(&fdssound.op[0].eg);
		}
	}

	/* Phase Generator */
	fdssound.op[1].wg.phase += fdssound.op[1].pg.spd;
	fdssound.op[0].wg.phase += fdssound.op[0].pg.spd;
	return (fdssound.op[0].pg.freq != 0) ? output : 0;
}

static void __fastcall FDSSoundVolume(unsigned int volume)
{
	volume += 196;
	fdssound.mastervolume = (volume << (LOG_BITS - 8)) << 1;
	fdssound.mastervolumel[0] = LogToLinear(fdssound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2;
	fdssound.mastervolumel[1] = LogToLinear(fdssound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 4 / 3;
	fdssound.mastervolumel[2] = LogToLinear(fdssound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2 / 2;
	fdssound.mastervolumel[3] = LogToLinear(fdssound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 8 / 10;
}

static const uint8 wave_delta_table[8] = {
	0,(1 << FM_DEPTH),(2 << FM_DEPTH),(4 << FM_DEPTH),
	0,256 - (4 << FM_DEPTH),256 - (2 << FM_DEPTH),256 - (1 << FM_DEPTH),
};

static void __fastcall FDSSoundWrite(uint16 address, uint8 value)
{
	if (0x4040 <= address && address <= 0x407F)
	{
		fdssound.op[0].wg.wave[address - 0x4040] = ((int)(value & 0x3f)) - 0x20;
	}
	else if (0x4080 <= address && address <= 0x408F)
	{
		FDS_OP *pop = &fdssound.op[(address & 4) >> 2];
		fdssound.reg[address - 0x4080] = value;
		switch (address & 0xf)
		{
			case 0:
			case 4:
				pop->eg.mode = value & 0xc0;
				if (pop->eg.mode & 0x80)
				{
					pop->eg.volume = (value & 0x3f);
				}
				else
				{
					pop->eg.spd = value & 0x3f;
				}
				break;
			case 5:
#if 1
				fdssound.op[1].bias = value & 255;
#else
				fdssound.op[1].bias = (((value & 0x7f) ^ 0x40) - 0x40) & 255;
#endif
#if 0
				fdssound.op[1].wg.phase = 0;
#endif
				break;
			case 2:	case 6:
				pop->pg.freq &= 0x00000F00;
				pop->pg.freq |= (value & 0xFF) << 0;
				pop->pg.spdbase = pop->pg.freq * fdssound.phasecps;
				break;
			case 3:
				fdssound.envdisable = value & 0x40;
			case 7:
#if 0
				pop->wg.phase = 0;
#endif
				pop->pg.freq &= 0x000000FF;
				pop->pg.freq |= (value & 0x0F) << 8;
				pop->pg.spdbase = pop->pg.freq * fdssound.phasecps;
				pop->wg.disable = value & 0x80;
				if (pop->wg.disable)
				{
					pop->wg.phase = 0;
					pop->wg.wavptr = 0;
					pop->wavebase = 0;
				}
				break;
			case 8:
				if (fdssound.op[1].wg.disable)
				{
					int32 idx = value & 7;
					if (idx == 4)
					{
						fdssound.op[1].wavebase = 0;
					}
#if FDS_DYNAMIC_BIAS
					fdssound.op[1].wavebase += wave_delta_table[idx];
					fdssound.op[1].wg.wave[fdssound.op[1].wg.wavptr + 0] = (fdssound.op[1].wavebase + 64) & 255;
					fdssound.op[1].wavebase += wave_delta_table[idx];
					fdssound.op[1].wg.wave[fdssound.op[1].wg.wavptr + 1] = (fdssound.op[1].wavebase + 64) & 255;
					fdssound.op[1].wg.wavptr = (fdssound.op[1].wg.wavptr + 2) & 0x3f;
#else
					fdssound.op[1].wavebase += wave_delta_table[idx];
					fdssound.op[1].wg.wave[fdssound.op[1].wg.wavptr + 0] = (fdssound.op[1].wavebase + fdssound.op[1].bias + 64) & 255;
					fdssound.op[1].wavebase += wave_delta_table[idx];
					fdssound.op[1].wg.wave[fdssound.op[1].wg.wavptr + 1] = (fdssound.op[1].wavebase + fdssound.op[1].bias + 64) & 255;
					fdssound.op[1].wg.wavptr = (fdssound.op[1].wg.wavptr + 2) & 0x3f;
#endif
				}
				break;
			case 9:
				fdssound.lvl = (value & 3);
				fdssound.op[0].wg.disable2 = value & 0x80;
				break;
			case 10:
				fdssound.envspd = value << EGCPS_BITS;
				break;
		}
	}
}

static uint8 __fastcall FDSSoundRead(uint16 address)
{
	if (0x4040 <= address && address <= 0x407f)
	{
		return fdssound.op[0].wg.wave[address & 0x3f] + 0x20;
	}
	if (0x4090 == address)
		return fdssound.op[0].eg.volume | 0x40;
	if (0x4092 == address) /* 4094? */
		return fdssound.op[1].eg.volume | 0x40;
	return 0;
}

static uint32 DivFix(uint32 p1, uint32 p2, uint32 fix)
{
	uint32 ret;
	ret = p1 / p2;
	p1  = p1 % p2;/* p1 = p1 - p2 * ret; */
	while (fix--)
	{
		p1 += p1;
		ret += ret;
		if (p1 >= p2)
		{
			p1 -= p2;
			ret++;
		}
	}
	return ret;
}

static void __fastcall FDSSoundReset(void)
{
	uint32 i;
	memset(&fdssound, 0, sizeof(FDSSOUND));
	// Todo: Fix srate
	fdssound.srate = CAPU::BASE_FREQ_NTSC; ///NESAudioFrequencyGet();
	fdssound.envcps = DivFix(NES_BASECYCLES, 12 * fdssound.srate, EGCPS_BITS + 5 - 9 + 1);
	fdssound.envspd = 0xe8 << EGCPS_BITS;
	fdssound.envdisable = 1;
	fdssound.phasecps = DivFix(NES_BASECYCLES, 12 * fdssound.srate, PGCPS_BITS);
	for (i = 0; i < 0x40; i++)
	{
		fdssound.op[0].wg.wave[i] = (i < 0x20) ? 0x1f : -0x20;
		fdssound.op[1].wg.wave[i] = 64;
	}
}

void FDSSoundInstall3(void)
{
	LogTableInitialize();
}

// FDS interface (jsr)

CFDS::CFDS(CMixer *pMixer) : CExChannel(pMixer, SNDCHIP_FDS, CHANID_FDS)
{
	FDSSoundInstall3();
}

CFDS::~CFDS()
{
}

void CFDS::Reset()
{
	FDSSoundReset();
	FDSSoundVolume(0);
}

void CFDS::Write(uint16 Address, uint8 Value)
{
	FDSSoundWrite(Address, Value);
}

uint8 CFDS::Read(uint16 Address, bool &Mapped)
{
	return FDSSoundRead(Address);
}

void CFDS::EndFrame()
{
	m_iFrameCycles = 0;
}

void CFDS::Process(uint32 Time)
{
	if (!Time)
		return;

	while (Time--) {
		Mix(FDSSoundRender() >> 12);
		m_iFrameCycles++;
	}
}
