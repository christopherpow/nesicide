//    NESICIDE - an IDE for the 8-bit NES.  
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
// APU.cpp: implementation of the CAPU class.
//
//////////////////////////////////////////////////////////////////////

#include "cnesapu.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include <QSemaphore>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char CAPU::m_APUreg [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char CAPU::m_APUregDirty [] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
unsigned char CAPU::m_APUreg4015mask = 0x1F;
int           CAPU::m_sequencerMode = 0;
int           CAPU::m_sequence = 0;
bool          CAPU::m_irqEnabled = false;
bool          CAPU::m_irqAsserted = false;
CAPUSquare    CAPU::m_square[2];
CAPUTriangle  CAPU::m_triangle;
CAPUNoise     CAPU::m_noise;
CAPUDMC       CAPU::m_dmc;

SDL_AudioSpec  CAPU::m_sdlAudioSpec;
unsigned short CAPU::m_waveBuf [ NUM_APU_BUFS ][ 2000 ] = { { 0, }, };
int            CAPU::m_waveBufDepth [ NUM_APU_BUFS ] = { 0, };
int            CAPU::m_waveBufProduce = 0;
int            CAPU::m_waveBufConsume = 0;
int            CAPU::m_iFreq = 0;
int            CAPU::m_iFactorIdx = 2;

// Events that can occur during the APU sequence stepping
enum
{
   APU_SEQ_INT_FLAG = 0x01,
   APU_SEQ_CLK_LENGTH_CTR = 0x02,
   APU_SEQ_CLK_ENVELOPE_CTR = 0x04
};

static int m_seq4 [ 4 ] = 
{
   APU_SEQ_CLK_ENVELOPE_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR|APU_SEQ_CLK_LENGTH_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR|APU_SEQ_CLK_LENGTH_CTR|APU_SEQ_INT_FLAG,
};

static int m_seq5 [ 5 ] = 
{
   APU_SEQ_CLK_ENVELOPE_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR|APU_SEQ_CLK_LENGTH_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR|APU_SEQ_CLK_LENGTH_CTR,
   APU_SEQ_CLK_ENVELOPE_CTR
};

static int m_samplesPerSeqTick [ 2 ] [ 4 ] =
{
   { 184, 184, 184, 183 },
   { 221, 220, 221, 220 }
};

static unsigned short m_squareTable [ 31 ] =
{
   0x0000,
   0x023A,
   0x0467,
   0x0687,
   0x089A,
   0x0AA0,
   0x0C9B,
   0x0E8A,
   0x106E,
   0x1248,
   0x1417,
   0x15DC,
   0x1797,
   0x1949,
   0x1AF2,
   0x1C92,
   0x1E29,
   0x1FB9,
   0x2140,
   0x22BF,
   0x2437,
   0x25A7,
   0x2710,
   0x2873,
   0x29CE,
   0x2B23,
   0x2C72,
   0x2DBA,
   0x2EFD,
   0x303A,
   0x3171
};

static unsigned short m_tndTable [ 203 ] = 
{
   0x0000,
   0x0149,
   0x028F,
   0x03D3,
   0x0515,
   0x0653,
   0x0790,
   0x08C9,
   0x0A01,
   0x0B35,
   0x0C68,
   0x0D97,
   0x0EC5,
   0x0FF0,
   0x1119,
   0x123F,
   0x1364,
   0x1486,
   0x15A5,
   0x16C3,
   0x17DE,
   0x18F8,
   0x1A0F,
   0x1B24,
   0x1C37,
   0x1D48,
   0x1E57,
   0x1F63,
   0x206E,
   0x2177,
   0x227E,
   0x2383,
   0x2487,
   0x2588,
   0x2687,
   0x2785,
   0x2881,
   0x297B,
   0x2A73,
   0x2B6A,
   0x2C5E,
   0x2D51,
   0x2E43,
   0x2F32,
   0x3020,
   0x310D,
   0x31F7,
   0x32E0,
   0x33C8,
   0x34AE,
   0x3592,
   0x3675,
   0x3756,
   0x3836,
   0x3914,
   0x39F0,
   0x3ACC,
   0x3BA5,
   0x3C7E,
   0x3D55,
   0x3E2A,
   0x3EFE,
   0x3FD1,
   0x40A2,
   0x4172,
   0x4241,
   0x430E,
   0x43DA,
   0x44A5,
   0x456E,
   0x4636,
   0x46FD,
   0x47C2,
   0x4886,
   0x4949,
   0x4A0B,
   0x4ACC,
   0x4B8B,
   0x4C49,
   0x4D06,
   0x4DC2,
   0x4E7D,
   0x4F37,
   0x4FEF,
   0x50A6,
   0x515C,
   0x5211,
   0x52C5,
   0x5378,
   0x542A,
   0x54DB,
   0x558A,
   0x5639,
   0x56E7,
   0x5793,
   0x583F,
   0x58E9,
   0x5993,
   0x5A3B,
   0x5AE3,
   0x5B89,
   0x5C2F,
   0x5CD4,
   0x5D77,
   0x5E1A,
   0x5EBC,
   0x5F5D,
   0x5FFD,
   0x609C,
   0x613A,
   0x61D7,
   0x6273,
   0x630F,
   0x63A9,
   0x6443,
   0x64DC,
   0x6574,
   0x660B,
   0x66A2,
   0x6737,
   0x67CC,
   0x6860,
   0x68F3,
   0x6985,
   0x6A17,
   0x6AA7,
   0x6B37,
   0x6BC6,
   0x6C55,
   0x6CE2,
   0x6D6F,
   0x6DFB,
   0x6E87,
   0x6F11,
   0x6F9B,
   0x7024,
   0x70AD,
   0x7134,
   0x71BB,
   0x7241,
   0x72C7,
   0x734C,
   0x73D0,
   0x7454,
   0x74D6,
   0x7559,
   0x75DA,
   0x765B,
   0x76DB,
   0x775B,
   0x77D9,
   0x7858,
   0x78D5,
   0x7952,
   0x79CE,
   0x7A4A,
   0x7AC5,
   0x7B40,
   0x7BB9,
   0x7C33,
   0x7CAB,
   0x7D23,
   0x7D9B,
   0x7E12,
   0x7E88,
   0x7EFE,
   0x7F73,
   0x7FE7,
   0x805B,
   0x80CF,
   0x8142,
   0x81B4,
   0x8226,
   0x8297,
   0x8308,
   0x8378,
   0x83E7,
   0x8456,
   0x84C5,
   0x8533,
   0x85A0,
   0x860D,
   0x867A,
   0x86E6,
   0x8751,
   0x87BC,
   0x8827,
   0x8891,
   0x88FA,
   0x8963,
   0x89CB,
   0x8A33,
   0x8A9B,
   0x8B02,
   0x8B69,
   0x8BCF,
   0x8C34,
   0x8C9A,
   0x8CFE,
   0x8D63,
   0x8DC6,
   0x8E2A,
   0x8E8D
};

static int m_samples [ 6 ] = { 11025, 22050, 44100, 88200, 176400, 0 };

extern QSemaphore emulatorSemaphore;

extern "C" void SDL_GetMoreData(void *userdata, Uint8 *stream, int len)
{
   CAPU::PLAY ( stream, len );

   emulatorSemaphore.release();
}

static CAPU __init __attribute((unused));

CAPU::CAPU()
{
   m_square[0].SetChannel ( 0 );
   m_square[1].SetChannel ( 1 );
   m_triangle.SetChannel ( 2 );
   m_noise.SetChannel ( 3 );
   m_dmc.SetChannel ( 4 );

   SDL_Init ( SDL_INIT_AUDIO );

   m_sdlAudioSpec.channels = 1;
   m_sdlAudioSpec.format = AUDIO_U16SYS;
   m_sdlAudioSpec.freq = 44100;
   m_sdlAudioSpec.samples = 735;
   m_sdlAudioSpec.callback = SDL_GetMoreData;
   m_sdlAudioSpec.userdata = (void*)this;
}

CAPU::~CAPU()
{
   SDL_Quit ();
}

void CAPU::OPEN ( void )
{
   SDL_AudioSpec obtained;
   SDL_OpenAudio ( &m_sdlAudioSpec, &obtained );

   memcpy ( &m_sdlAudioSpec, &obtained, sizeof(SDL_AudioSpec) );

   SDL_PauseAudio ( 0 );
}

void CAPU::PLAY ( Uint8 *stream, int len )
{
   int  waveBufDepth;
   unsigned short* waveBuf;

   waveBufDepth = *(m_waveBufDepth+m_waveBufConsume);
   waveBuf = *(m_waveBuf + m_waveBufConsume);

   if ( waveBufDepth )
   {
      SDL_MixAudio ( stream, (const Uint8*)waveBuf, len, 64/*SDL_MIX_MAXVOLUME*/ );
      m_waveBufConsume++;
      m_waveBufConsume %= NUM_APU_BUFS;
   }
}

void CAPU::RUN ( void )
{
   int timerTicks = 41;
   int sampleTick;
   int samplesPerSeqTick1 = *(*(m_samplesPerSeqTick+m_iFreq));
   int samplesPerSeqTick2 = *(*(m_samplesPerSeqTick+m_iFreq)+1);
   int samplesPerSeqTick3 = *(*(m_samplesPerSeqTick+m_iFreq)+2);
   int samplesPerSeqTick4 = *(*(m_samplesPerSeqTick+m_iFreq)+3);

   SDL_LockAudio ();

   unsigned short* pWaveBuf = *(m_waveBuf+m_waveBufProduce);
   int* pWaveBufDepth = m_waveBufDepth+m_waveBufProduce;

   SEQTICK ();

   for ( sampleTick = 0; sampleTick < samplesPerSeqTick1; sampleTick++ )
   {
      m_square[0].TIMERTICK ( timerTicks );
      m_square[1].TIMERTICK ( timerTicks );
      m_triangle.TIMERTICK ( timerTicks );
      m_noise.TIMERTICK ( timerTicks );
      m_dmc.TIMERTICK ( timerTicks );

      if ( timerTicks == 41 ) { timerTicks = 40; } else { timerTicks = 41; }

      (*(pWaveBuf+(*pWaveBufDepth))) = AMPLITUDE ();
      (*pWaveBufDepth)++;
   }

   SEQTICK ();

   for ( sampleTick = 0; sampleTick < samplesPerSeqTick2; sampleTick++ )
   {
      m_square[0].TIMERTICK ( timerTicks );
      m_square[1].TIMERTICK ( timerTicks );
      m_triangle.TIMERTICK ( timerTicks );
      m_noise.TIMERTICK ( timerTicks );
      m_dmc.TIMERTICK ( timerTicks );

      if ( timerTicks == 41 ) { timerTicks = 40; } else { timerTicks = 41; }

      (*(pWaveBuf+(*pWaveBufDepth))) = AMPLITUDE ();
      (*pWaveBufDepth)++;
   }

   SEQTICK ();

   for ( sampleTick = 0; sampleTick < samplesPerSeqTick3; sampleTick++ )
   {
      m_square[0].TIMERTICK ( timerTicks );
      m_square[1].TIMERTICK ( timerTicks );
      m_triangle.TIMERTICK ( timerTicks );
      m_noise.TIMERTICK ( timerTicks );
      m_dmc.TIMERTICK ( timerTicks );

      if ( timerTicks == 41 ) { timerTicks = 40; } else { timerTicks = 41; }

      (*(pWaveBuf+(*pWaveBufDepth))) = AMPLITUDE ();
      (*pWaveBufDepth)++;
   }

   SEQTICK ();

   for ( sampleTick = 0; sampleTick < samplesPerSeqTick4; sampleTick++ )
   {
      m_square[0].TIMERTICK ( timerTicks );
      m_square[1].TIMERTICK ( timerTicks );
      m_triangle.TIMERTICK ( timerTicks );
      m_noise.TIMERTICK ( timerTicks );
      m_dmc.TIMERTICK ( timerTicks );

      if ( timerTicks == 41 ) { timerTicks = 40; } else { timerTicks = 41; }

      (*(pWaveBuf+(*pWaveBufDepth))) = AMPLITUDE ();
      (*pWaveBufDepth)++;
   }

   m_waveBufProduce++;
   m_waveBufProduce %= NUM_APU_BUFS;
   m_waveBufDepth [ m_waveBufProduce ] = 0;

   SDL_UnlockAudio ();
}

void CAPU::CLOSE ( void )
{
   SDL_CloseAudio ();
}

void CAPU::GETDACS ( unsigned char* square1,
                     unsigned char* square2,
                     unsigned char* triangle,
                     unsigned char* noise,
                     unsigned char* dmc )
{
   (*square1) = m_square[0].GETDAC ();
   (*square2) = m_square[1].GETDAC ();
   (*triangle) = m_triangle.GETDAC ();
   (*noise) = m_noise.GETDAC ();
   (*dmc) = m_dmc.GETDAC ();
}


unsigned short CAPU::AMPLITUDE ( void )
{
   static short prevamp = 0;
   static short realamp;
   short amp;
   short delta;

   amp = *(m_squareTable+(m_square[0].GETDAC()+m_square[1].GETDAC()))
         + *(m_tndTable+(((m_triangle.GETDAC()<<1)+m_triangle.GETDAC())+(m_noise.GETDAC()<<1)+m_dmc.GETDAC()));

   delta = amp - prevamp;
   prevamp = amp;

   realamp = (short)((float)realamp * .9975) + delta;
   return realamp;
}

void CAPU::SEQTICK ( void )
{
   if ( m_sequencerMode )
   {
      if ( m_seq5[m_sequence]&APU_SEQ_CLK_ENVELOPE_CTR )
      {
         m_square[0].CLKENVELOPE ();
         m_square[1].CLKENVELOPE ();
         m_triangle.CLKLINEARCOUNTER ();
         m_noise.CLKENVELOPE ();
      }
      if ( m_seq5[m_sequence]&APU_SEQ_CLK_LENGTH_CTR )
      {
         m_square[0].CLKLENGTHCOUNTER ();
         m_square[0].CLKSWEEPUNIT ();
         m_square[1].CLKLENGTHCOUNTER ();
         m_square[1].CLKSWEEPUNIT ();
         m_triangle.CLKLENGTHCOUNTER ();
         m_noise.CLKLENGTHCOUNTER ();
      }
   }
   else
   {
      if ( m_seq4[m_sequence]&APU_SEQ_CLK_ENVELOPE_CTR )
      {
         m_square[0].CLKENVELOPE ();
         m_square[1].CLKENVELOPE ();
         m_triangle.CLKLINEARCOUNTER ();
         m_noise.CLKENVELOPE ();
      }
      if ( m_seq4[m_sequence]&APU_SEQ_CLK_LENGTH_CTR )
      {
         m_square[0].CLKLENGTHCOUNTER ();
         m_square[0].CLKSWEEPUNIT ();
         m_square[1].CLKLENGTHCOUNTER ();
         m_square[1].CLKSWEEPUNIT ();
         m_triangle.CLKLENGTHCOUNTER ();
         m_noise.CLKLENGTHCOUNTER ();
      }
      if ( m_seq4[m_sequence]&APU_SEQ_INT_FLAG )
      {
         if ( m_irqEnabled )
         {
            m_irqAsserted = true;
            C6502::IRQ ( eSource_APU );
            CPPU::IRQ ( eSource_APU ); // Just for Tracer tag
         }
      }
   }
   m_sequence++;
   if ( ((m_sequencerMode == 0) && (m_sequence == 4)) ||
        ((m_sequencerMode) && (m_sequence == 5)) )
   {
      m_sequence = 0;
   }
}

void CAPU::RESET ( void )
{
   int idx;

   CAPU::CLOSE ();

   for ( idx = 0; idx < 32; idx++ )
   {
      m_APUreg [ idx ] = 0x00;
      m_APUregDirty [ idx ] = 1;
   }
   m_APUreg4015mask = 0x1F;
   m_square[0].RESET ();
   m_square[1].RESET ();
   m_triangle.RESET ();
   m_noise.RESET ();
   m_dmc.RESET ();

   m_irqEnabled = true;
   m_irqAsserted = false;
   m_sequence = 0;
   m_sequencerMode = 0;

   CAPU::OPEN ();
}

CAPUOscillator::CAPUOscillator ()
{
   int idx;

   m_linearCounter = 0;
   m_linearCounterReload = 0;
   m_lengthCounter = 0;
   m_period = 0;
   m_periodCounter = 0;
   m_envelope = 0;
   m_envelopeCounter = 0;
   m_envelopeDivider = 0;
   m_volume = 0;
   m_volumeSet = 0;
   m_sweepDivider = 0;
   m_sweepShift = 0;
   m_sweepNegate = false;
   m_sweep = 0;
   m_enabled = false;
   m_halted = false;
   m_envelopeEnabled = false;
   m_sweepEnabled = false;
   m_linearCounterHalted = false;
   m_dac = 0x00;
   m_reg1Wrote = false;
   m_reg3Wrote = false;
   for ( idx = 0; idx < 4; idx++ )
   {
      m_reg [ idx ] = 0x00;
   }
}

void CAPUOscillator::APU ( UINT addr, unsigned char data )
{
   *(m_reg+(addr&0x03)) = data;
}

void CAPUOscillator::CLKSWEEPUNIT ( void )
{
   bool sweepClkEdge = false;
   short sweepPeriod = 0;

   // sweep unit...
   m_sweep++;
   if ( m_sweep >= m_sweepDivider )
   {
      m_sweep = 0;
      sweepClkEdge = true;
   }
   if ( m_reg1Wrote == true )
   {
      m_reg1Wrote = false;
      m_sweep = 0;
   }

   if ( m_sweepEnabled )
   {
      sweepPeriod = m_period;
      sweepPeriod >>= m_sweepShift;
      if ( m_sweepNegate ) 
      {
         sweepPeriod = ~sweepPeriod;
         if ( m_channel == 1 )
         {
            sweepPeriod++;
         }
      }
      sweepPeriod += m_period;
   }

   if ( (m_period < 8) || 
        (sweepPeriod > 0x7FF) )
   {
      m_volume = 0;
   }
   else
   {
      if ( (m_sweepEnabled) && (m_sweepShift) )
      {
         if ( sweepClkEdge )
         {
            m_period = sweepPeriod&0x7FF;
         }
      }
   }
}

void CAPUOscillator::CLKLENGTHCOUNTER ( void )
{
   if ( !m_halted )
   {
      // length counter...
      if ( m_lengthCounter )
      {
         m_lengthCounter--;
      }
   }
}

void CAPUOscillator::CLKLINEARCOUNTER ( void )
{
   // linear counter...
   if ( m_linearCounterHalted )
   {
      m_linearCounter = m_linearCounterReload;
   }
   else if ( m_linearCounter )
   {
      m_linearCounter--;
   }
   if ( !(m_reg[0]&0x80) )
   {
      m_linearCounterHalted = false;
   }
}

void CAPUOscillator::CLKENVELOPE ( void )
{
   bool envelopeClkEdge = false;

   // envelope...
   if ( m_reg3Wrote )
   {
      m_reg3Wrote = false;
      m_envelopeCounter = 15;
      m_envelope = 0;
   }
   else
   {
      m_envelope++;
      if ( m_envelope >= m_envelopeDivider )
      {
         m_envelope = 0;
         envelopeClkEdge = true;
      }
   }
   if ( envelopeClkEdge )
   {
      if ( (m_halted) && (m_envelopeCounter == 0) )
      {
         m_envelopeCounter = 15;
      }
      else if ( m_envelopeCounter )
      {
         m_envelopeCounter--;
      }
   }
   if ( m_envelopeEnabled )
   {
      m_volume = m_envelopeCounter;
   }
   else
   {
      m_volume = m_volumeSet;
   }
}

UINT CAPUOscillator::CLKDIVIDER ( UINT sampleTicks )
{
   UINT periodClkEdges = 0;

   if ( m_period > 0 )
   {
      m_periodCounter += sampleTicks;
      periodClkEdges = m_periodCounter / m_period;
      m_periodCounter %= m_period;
   }

   return periodClkEdges;
}

static int m_squareSeq [ 4 ] [ 8 ] =
{
   { 0, 1, 0, 0, 0, 0, 0, 0 },
   { 0, 1, 1, 0, 0, 0, 0, 0 },
   { 0, 1, 1, 1, 1, 0, 0, 0 },
   { 1, 0, 0, 1, 1, 1, 1, 1 }
};

void CAPUSquare::APU ( UINT addr, unsigned char data )
{
   CAPUOscillator::APU ( addr, data );

   if ( addr == 0 )
   {
      m_duty = ((data&0xC0)>>6);
      m_halted = data&0x20;
      m_envelopeEnabled = !(data&0x10);
      m_envelopeDivider = (data&0x0F)+1;
      m_volume = (data&0x0F);
      m_volumeSet = m_volume;
   }
   else if ( addr == 1 )
   {
      m_sweepEnabled = data&0x80;
      m_sweepDivider = ((data&0x70)>>4)+1;
      m_sweepNegate = data&0x08;
      m_sweepShift = (data&0x07);
      m_reg1Wrote = true;
   }
   else if ( addr == 2 )
   {
      m_period &= 0xFF00;
      m_period |= data;
   }
   else if ( addr == 3 )
   {
      if ( m_enabled )
      {
         m_lengthCounter = *(m_lengthLUT+(data>>3));
      }
      m_period &= 0x00FF;
      m_period |= ((data&0x07)<<8);
      m_period += 1;
      m_reg3Wrote = true;
      m_seqTick = 0;
   }
}

void CAPUSquare::TIMERTICK ( UINT sampleTicks )
{
   UINT ticks = CLKDIVIDER ( sampleTicks );
   UINT seqTicks;

   if ( (m_enabled) &&
        (m_lengthCounter) )
   {
      // divide timer by 2...
      m_timerClk += ticks;
      seqTicks = m_timerClk>>1;
      m_timerClk &= 0x1;
      m_seqTick += seqTicks;
      m_seqTick &= 0x7;

      if ( *(*(m_squareSeq+m_duty)+m_seqTick) )
      {
         SETDAC ( m_volume );
      }
      else
      {
         SETDAC ( 0 );
      }
   }
   else
   {
      SETDAC ( 0 );
   }
}

static int m_triangleSeq [ 32 ] =
{
   0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8,
   0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
   0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
   0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

void CAPUTriangle::APU ( UINT addr, unsigned char data )
{
   CAPUOscillator::APU ( addr, data );

   if ( addr == 0 )
   {
      m_halted = data&0x80;
      m_linearCounterReload = (data&0x7F);
   }
   else if ( addr == 2 )
   {
      m_period &= 0xFF00;
      m_period |= data;
   }
   else if ( addr == 3 )
   {
      if ( m_enabled )
      {
         m_lengthCounter = *(m_lengthLUT+(data>>3));
      }
      m_period &= 0x00FF;
      m_period |= ((data&0x07)<<8);
      m_period += 1;
      m_linearCounterHalted = true;
      m_reg3Wrote = true;
   }
}

void CAPUTriangle::TIMERTICK ( UINT sampleTicks )
{
   UINT ticks = CLKDIVIDER ( sampleTicks );

   if ( (m_enabled) && 
        (m_linearCounter) && 
        (m_lengthCounter) )
   {
      m_seqTick += ticks;
      m_seqTick &= 0x1F;

      SETDAC ( *(m_triangleSeq+m_seqTick) );
   }
   else
   {
      SETDAC ( 0 );
   }
}

static unsigned short m_noisePeriod [ 16 ] =
{
   0x004,
   0x008,
   0x010,
   0x020,
   0x040,
   0x060,
   0x080,
   0x0A0,
   0x0CA,
   0x0FE,
   0x17C,
   0x1FC,
   0x2FA,
   0x3F8,
   0x7F2,
   0xFE4
};

void CAPUNoise::APU ( UINT addr, unsigned char data )
{
   CAPUOscillator::APU ( addr, data );

   if ( addr == 0 )
   {
      m_halted = data&0x20;
      m_envelopeEnabled = !(data&0x10);
      m_envelopeDivider = (data&0x0F)+1;
      m_volume = (data&0x0F);
      m_volumeSet = m_volume;
   }
   else if ( addr == 2 )
   {
      m_mode = data&0x80;
      m_period = m_noisePeriod [ data&0xF ];
   }
   else if ( addr == 3 )
   {
      if ( m_enabled )
      {
         m_lengthCounter = *(m_lengthLUT+(data>>3));
      }
      m_reg3Wrote = true;
   }
}

CAPUNoise::CAPUNoise ()
 : CAPUOscillator(), m_mode(0)
{
   unsigned char preShiftXor;
   int idx;

   // Calculate short table...
   m_shift = 1;
   for ( idx = 0; idx < 93; idx++ )
   {
      preShiftXor = ((m_shift&0x4000)>>14)^((m_shift&0x0100)>>8);
      m_shift <<= 1;
      m_shift |= preShiftXor;
      m_shortTable [ idx ] = !(m_shift&0x4000);
   }
   // Calculate long table...
   m_shift = 1;
   for ( idx = 0; idx < 32767; idx++ )
   {
      preShiftXor = ((m_shift&0x4000)>>14)^((m_shift&0x2000)>>13);
      m_shift <<= 1;
      m_shift |= preShiftXor;
      m_longTable [ idx ] = !(m_shift&0x4000);
   }
   m_shift = 1;
}

void CAPUNoise::TIMERTICK ( UINT sampleTicks )
{
   UINT ticks = CLKDIVIDER ( sampleTicks );
   unsigned char set;

   if ( m_mode )
   {
      m_shortTableIdx += ticks;
      m_shortTableIdx %= 93;
      set = *(m_shortTable+m_shortTableIdx);
   }
   else
   {
      m_longTableIdx += ticks;
      m_longTableIdx %= 32767;
      set = *(m_longTable+m_longTableIdx);
   }
   if ( (m_enabled) &&
        (m_lengthCounter) )
   {      
      if ( set )
      {
         SETDAC ( m_volume );
      }
      else
      {
         SETDAC ( 0 );
      }
   }
   else
   {
      SETDAC ( 0 );
   }
}

static unsigned short m_dmcPeriod [ 16 ] =
{
   0x1AC,
   0x17C,
   0x154,
   0x140,
   0x11E,
   0x0FE,
   0x0E2,
   0x0D6,
   0x0BE,
   0x0A0,
   0x08E,
   0x080,
   0x06A,
   0x054,
   0x048,
   0x036
};

void CAPUDMC::APU ( UINT addr, unsigned char data )
{
   CAPUOscillator::APU ( addr, data );

   if ( addr == 0 )
   {
      m_irqEnabled = data&0x80;
      if ( !m_irqEnabled )
      {
         m_irqAsserted = false;
      }
      m_loop = data&0x40;
      m_period = *(m_dmcPeriod+(data&0xF));
   }
   else if ( addr == 1 )
   {
      m_volume = data&0x7f;
   }
   else if ( addr == 2 )
   {
      m_sampleAddr = (data<<6)|0xC000;
   }
   else if ( addr == 3 )
   {
      m_sampleLength = (data<<4)+1;
      m_lengthCounter = 0;
   }
}

void CAPUDMC::ENABLE ( bool enabled )
{ 
   CAPUOscillator::ENABLE(enabled);
   
   if ( enabled )
   {
      if ( !m_lengthCounter )
      {
         if ( m_dmaSource != NULL )
         {
            m_dmaSourcePtr = m_dmaSource;
         }
         else
         {
            m_dmaReaderAddrPtr = m_sampleAddr;
         }
         m_lengthCounter = m_sampleLength;
      }
   }
   m_irqAsserted = false;
}

UINT CAPUDMC::DMAREADER ( void )
{
   UINT cycles = 0;

   if ( !m_sampleBufferFull )
   {
      if ( m_lengthCounter )
      {
         cycles = 4;
         if ( m_dmaSource != NULL )
         {
            m_sampleBuffer = (*m_dmaSourcePtr);
         }
         else
         {
            m_sampleBuffer = C6502::DMA ( m_dmaReaderAddrPtr, eLoggerSource_APU );
         }
         m_sampleBufferFull = true;
         if ( m_dmaSource != NULL )
         {
            m_dmaSourcePtr++;
         }
         else
         {
            if ( m_dmaReaderAddrPtr == 0xFFFF )
            {
               m_dmaReaderAddrPtr = 0x8000;
            }
            else
            {
               m_dmaReaderAddrPtr++;
            }
         }
         m_lengthCounter--;
      }
      if ( (!m_lengthCounter) && m_loop )
      {
         if ( m_dmaSource != NULL )
         {
            m_dmaSourcePtr = m_dmaSource;
         }
         else
         {
            m_dmaReaderAddrPtr = m_sampleAddr;
         }
         m_lengthCounter = m_sampleLength;
      }
      if ( (!m_lengthCounter) && m_irqEnabled )
      {
         m_irqAsserted = true;
      }
   }

   return cycles;
}

void CAPUDMC::TIMERTICK ( UINT sampleTicks )
{
   UINT ticks = CLKDIVIDER ( sampleTicks );
   
   for ( ; ticks > 0; ticks-- )
   {
      DMAREADER ();

      if ( m_outputShiftCounter == 0 )
      {
         m_outputShiftCounter = 8;
         if ( m_sampleBufferFull )
         {
            m_outputShift = m_sampleBuffer;
            m_sampleBufferFull = false;
            m_silence = false;
         }
         else
         {
            m_silence = true;
         }
      }

      if ( !m_silence )
      {
         if ( (!(m_outputShift&0x01)) && (m_volume > 1) )
         {
            m_volume -= 2;
         }
         else if ( (m_outputShift&0x01) && (m_volume < 126) )
         {
            m_volume += 2;
         }
      }

      if ( m_outputShiftCounter )
      {
         m_outputShift >>= 1;
         m_outputShiftCounter--;
      }
   }

   if ( (m_enabled) && (!m_silence) )
   {
      SETDAC ( m_volume );
   }
   else
   {
      SETDAC ( 0 );
   }
}

UINT CAPU::APU ( UINT addr )
{
   UINT data = 0x00;

   data |= (m_square[0].LENGTH()?0x01:0x00);
   data |= (m_square[1].LENGTH()?0x02:0x00);
   data |= (m_triangle.LENGTH()?0x04:0x00);
   data |= (m_noise.LENGTH()?0x08:0x00);
   data |= (m_dmc.LENGTH()?0x10:0x00);
   data |= (m_irqAsserted?0x40:0x00);
   data |= (m_dmc.IRQASSERTED()?0x80:0x00);

   m_irqAsserted = false;

   return data;
}

void CAPU::APU ( UINT addr, unsigned char data )
{
   // For APU recording...
   m_APUreg [ addr&0x1F ] = data;
   m_APUregDirty [ addr&0x1F ] = 1;

   if ( addr < 0x4004 )
   {
      // Square 1
      m_square[0].APU ( addr&0x3, data );
   }
   else if ( addr < 0x4008 )
   {
      // Square 2
      m_square[1].APU ( addr&0x3, data );
   }
   else if ( addr < 0x400C )
   {
      // Triangle
      m_triangle.APU ( addr&0x3, data );
   }
   else if ( addr < 0x4010 )
   {
      // Noise
      m_noise.APU ( addr&0x3, data );
   }
   else if ( addr < 0x4014 )
   {
      // DMC
      m_dmc.APU ( addr&0x3, data );
   }
   else if ( addr == APUCTRL )
   {
      data &= m_APUreg4015mask;
      m_square[0].ENABLE ( !!(data&0x01) );
      m_square[1].ENABLE ( !!(data&0x02) );
      m_triangle.ENABLE ( !!(data&0x04) );
      m_noise.ENABLE ( !!(data&0x08) );
      m_dmc.ENABLE ( !!(data&0x10) );
   }
   else if ( addr == 0x4017 )
   {
      m_sequencerMode = data&0x80;
      m_sequence = 0;
      m_irqEnabled = !(data&0x40);

      if ( m_sequencerMode )
      {
         SEQTICK ();
      }
   }
}

