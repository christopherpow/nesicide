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
    
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "ctracer.h"

// CPTODO: CONFIG static object contains registry config items...removed for now.
//#include "Configurator.h"

#include "cnes6502.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracer::CTracer()
{
   m_cursor = 0;
   m_samples = 0;

   m_cpuCursor = 0;
   m_cpuSamples = 0;
   m_ppuCursor = 0;
   m_ppuSamples = 0;

   m_pSamples = new TracerInfo [ TRACER_DEFAULT_DEPTH ];
   m_ppCPUSamples = new TracerInfo* [ TRACER_DEFAULT_DEPTH ];
   m_ppPPUSamples = new TracerInfo* [ TRACER_DEFAULT_DEPTH ];

   m_sampleBufferDepth = TRACER_DEFAULT_DEPTH;
}


CTracer::~CTracer()
{
   delete m_pSamples;
}

bool CTracer::ReallocateTracerMemory(int newDepth)
{
   bool ok = true;

   delete m_pSamples;

   m_pSamples = new TracerInfo [ newDepth ];

   m_sampleBufferDepth = newDepth;

   if ( !m_pSamples )
   {
      ok = false;
   }

   return ok;
}

TracerInfo* CTracer::AddSample(unsigned int cycle, char type, char source, char target, unsigned short addr, unsigned char data)
{
   TracerInfo* pSample = NULL;
   TracerInfo** ppCPUSample = NULL;
   TracerInfo** ppPPUSample = NULL;
   char        overwrittenSource;

   pSample = m_pSamples + m_cursor;

   // Save overwritten sample's type to adjust
   // sample counts later on...
   overwrittenSource = pSample->source;

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

   switch ( source )
   {
      case eSource_CPU:
      case eSource_APU:
      case eSource_Mapper:
         ppCPUSample = m_ppCPUSamples + m_cpuCursor;
         (*ppCPUSample) = pSample;

         m_cpuCursor++;
         m_cpuCursor %= m_sampleBufferDepth;
      break;
      case eSource_PPU:
         ppPPUSample = m_ppPPUSamples + m_ppuCursor;
         (*ppPPUSample) = pSample;

         m_ppuCursor++;
         m_ppuCursor %= m_sampleBufferDepth;
      break;
   }

   if ( m_samples < m_sampleBufferDepth )
   {
      m_samples++;

      switch ( source )
      {
         case eSource_CPU:
         case eSource_APU:
         case eSource_Mapper:
            m_cpuSamples++;
         break;
         case eSource_PPU:
            m_ppuSamples++;
         break;
      }
   }
   else
   {
      switch ( source )
      {
         case eSource_CPU:
         case eSource_APU:
         case eSource_Mapper:
            if ( m_cpuSamples < m_sampleBufferDepth )
            {
               m_cpuSamples++;
            }
         break;
         case eSource_PPU:
            if ( m_ppuSamples < m_sampleBufferDepth )
            {
               m_ppuSamples++;
            }
         break;
      }

      if ( overwrittenSource == eSource_PPU )
      {
         m_ppuSamples--;
      }
      else if ( overwrittenSource != eSource_PPU )
      {
         m_cpuSamples--;
      }
   }

   return pSample;
}

TracerInfo* CTracer::AddRESET(void)
{
   return AddSample ( 0, eTracer_RESET, eSource_CPU, 0, 0, 0 );
}

TracerInfo* CTracer::AddNMI(char source)
{
   return AddSample ( 0, eTracer_NMI, source, 0, 0, 0 );
}

TracerInfo* CTracer::AddIRQ(char source)
{
   return AddSample ( 0, eTracer_IRQ, source, 0, 0, 0 );
}

TracerInfo* CTracer::AddGarbageFetch( unsigned int cycle, char target, unsigned short addr )
{
   // Special trace tag from PPU
   return AddSample ( cycle, eTracer_GarbageRead, eSource_PPU, target, addr, 0 );
}

void CTracer::ClearSampleBuffer(void)
{
   m_samples = 0;
   m_cursor = 0;
}

TracerInfo* CTracer::GetLastSample ( void )
{
   int getsample = m_cursor;

   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth-1;
   }

   return m_pSamples+getsample;
}

TracerInfo* CTracer::GetSample ( unsigned int sample )
{
   int getsample = m_cursor;

   getsample -= sample;
   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return m_pSamples+getsample;
}

TracerInfo* CTracer::GetCPUSample ( unsigned int sample )
{
   int getsample = m_cpuCursor;

   getsample -= sample;
   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return *(m_ppCPUSamples+getsample);
}

TracerInfo* CTracer::GetPPUSample ( unsigned int sample )
{
   int getsample = m_ppuCursor;

   getsample -= sample;
   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample;
   }

   return *(m_ppPPUSamples+getsample);
}

TracerInfo* CTracer::SetDisassembly ( unsigned char* szD )
{ 
   TracerInfo* pSample = GetLastSample ();
   if ( pSample )
   {
      (*((pSample->disassemble)+0)) = (*szD); szD++;
      (*((pSample->disassemble)+1)) = (*szD); szD++;
      (*((pSample->disassemble)+2)) = (*szD);
      (*((pSample->disassemble)+3)) = 0x00;
   }
   return pSample;
}

TracerInfo* CTracer::SetRegisters ( TracerInfo* pS, unsigned char a, unsigned char x, unsigned char y, unsigned char sp, unsigned char f )
{
   if ( pS )
   {
      pS->a = a;
      pS->x = x;
      pS->y = y;
      pS->sp = sp;
      pS->f = f;
      pS->ea = 0xFFFFFFFF;
      pS->regsset = 1;
   }
   return pS;
}

