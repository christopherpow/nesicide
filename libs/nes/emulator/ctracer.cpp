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

#include "ctracer.h"

CTracer::CTracer()
{
   m_frame = 0;

   m_cursor = 0;
   m_samples = 0;

   m_cpuCursor = 0;
   m_cpuSamples = 0;
   m_ppuCursor = 0;
   m_ppuSamples = 0;

   m_pSamples = new TracerInfo [ TRACER_DEFAULT_DEPTH ];

   m_sampleBufferDepth = TRACER_DEFAULT_DEPTH;
}


CTracer::~CTracer()
{
   delete [] m_pSamples;
}

bool CTracer::ReallocateTracerMemory(int32_t newDepth)
{
   bool ok = true;

   delete [] m_pSamples;

   m_pSamples = new TracerInfo [ newDepth ];

   m_sampleBufferDepth = newDepth;

   if ( !m_pSamples )
   {
      ok = false;
   }

   return ok;
}

TracerInfo* CTracer::AddSample(uint32_t cycle, int8_t type, int8_t source, int8_t target, uint16_t addr, uint8_t data)
{
   TracerInfo* pSample = m_pSamples + m_cursor;
   TracerInfo* pTargetSample = NULL;
   int8_t      overwrittenSource;

   // Save overwritten sample's type to adjust
   // sample counts later on...
   overwrittenSource = pSample->source;

   // Set frame from emulator.
   pSample->frame = m_frame;

   pSample->cycle = cycle;
   pSample->type = type;
   pSample->source = source;
   pSample->target = target;
   pSample->addr = addr;
   pSample->data = data;
   *(pSample->disassemble+3) = 0xFF;
   pSample->ea = 0xFFFFFFFF;
   pSample->regsset = 0;

   m_cursor++;
   m_cursor %= m_sampleBufferDepth;

   if ( source == eNESSource_PPU )
   {
      pTargetSample = m_pSamples + m_ppuCursor;
      pTargetSample->pPPUSample = pSample;

      m_ppuCursor++;
      m_ppuCursor %= m_sampleBufferDepth;

      if ( m_samples < m_sampleBufferDepth )
      {
         m_samples++;
         m_ppuSamples++;
      }
      else
      {
         if ( m_ppuSamples < m_sampleBufferDepth )
         {
            m_ppuSamples++;
         }

         if ( overwrittenSource == eNESSource_PPU )
         {
            m_ppuSamples--;
         }
         else
         {
            m_cpuSamples--;
         }
      }
   }
   else
   {
      pTargetSample = m_pSamples + m_cpuCursor;
      pTargetSample->pCPUSample = pSample;

      m_cpuCursor++;
      m_cpuCursor %= m_sampleBufferDepth;

      if ( m_samples < m_sampleBufferDepth )
      {
         m_samples++;
         m_cpuSamples++;
      }
      else
      {
         if ( m_cpuSamples < m_sampleBufferDepth )
         {
            m_cpuSamples++;
         }

         if ( overwrittenSource == eNESSource_PPU )
         {
            m_ppuSamples--;
         }
         else
         {
            m_cpuSamples--;
         }
      }
   }

   return pSample;
}

void CTracer::ClearSampleBuffer(void)
{
   m_frame = 0;

   m_cursor = 0;
   m_samples = 0;

   m_cpuCursor = 0;
   m_cpuSamples = 0;
   m_ppuCursor = 0;
   m_ppuSamples = 0;
}

TracerInfo* CTracer::GetLastSample ( void )
{
   int getsample = m_cursor-1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth-1;
   }

   return m_pSamples+getsample;
}

TracerInfo* CTracer::GetLastCPUSample ( void )
{
   int getsample = m_cpuCursor-1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth-1;
   }

   return (m_pSamples+getsample)->pCPUSample;
}

TracerInfo* CTracer::GetSample ( uint32_t sample )
{
   int32_t getsample = m_cursor-(sample+1);

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return m_pSamples+getsample;
}

TracerInfo* CTracer::GetCPUSample ( uint32_t sample )
{
   int32_t getsample = m_cpuCursor-(sample+1);

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return (m_pSamples+getsample)->pCPUSample;
}

TracerInfo* CTracer::GetPPUSample ( uint32_t sample )
{
   int32_t getsample = m_ppuCursor-(sample+1);

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return (m_pSamples+getsample)->pPPUSample;
}

void CTracer::SetDisassembly ( TracerInfo* pS, uint8_t* szD )
{
   if ( pS )
   {
      (*((pS->disassemble)+0)) = (*szD);
      szD++;
      (*((pS->disassemble)+1)) = (*szD);
      szD++;
      (*((pS->disassemble)+2)) = (*szD);
      (*((pS->disassemble)+3)) = 0x00;
   }
}

void CTracer::SetRegisters ( TracerInfo* pS, uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint8_t f )
{
   if ( pS )
   {
      pS->a = a;
      pS->x = x;
      pS->y = y;
      pS->sp = sp;
      pS->f = f;
      pS->regsset = 1;
   }
}
