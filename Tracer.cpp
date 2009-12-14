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

// CPTODO: include for ?
//#include "nesicide.h"
#include "Tracer.h"

// CPTODO: CONFIG static object contains registry config items...removed for now.
//#include "Configurator.h"

#include "6502.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracer::CTracer()
{
   m_cursor = 0;
   m_samples = 0;

   m_pSampleBuffer = new TracerInfo [ TRACER_DEFAULT_DEPTH ];

   m_sampleBufferDepth = TRACER_DEFAULT_DEPTH;
}

CTracer::~CTracer()
{
   delete m_pSampleBuffer;
}

bool CTracer::ReallocateTracerMemory(int newDepth)
{
   bool ok = true;

   delete m_pSampleBuffer;

   m_pSampleBuffer = new TracerInfo [ newDepth ];

   m_sampleBufferDepth = newDepth;

   if ( !m_pSampleBuffer )
   {
      ok = false;
   }

   return ok;
}

TracerInfo* CTracer::AddSample(unsigned int cycle, char type, char source, char target, unsigned short addr, unsigned char data)
{
   TracerInfo* pSample = NULL;

// CPTODO: removed check for enabled
//   if ( CONFIG.IsTracerEnabled() )
   {
      pSample = m_pSampleBuffer + m_cursor;
   
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
      if ( m_samples < m_sampleBufferDepth )
      {
         m_samples++;
      }
   }

   return pSample;
}

TracerInfo* CTracer::AddRESET(void)
{
   return AddSample ( 0, eTracer_RESET, eSource_User, 0, 0, 0 );
}

TracerInfo* CTracer::AddNMI(char source)
{
   return AddSample ( 0, eTracer_NMI, source, 0, 0, 0 );
}

TracerInfo* CTracer::AddIRQ(char source)
{
   return AddSample ( 0, eTracer_IRQ, source, 0, 0, 0 );
}

TracerInfo* CTracer::AddGarbageFetch( unsigned int cycle, char target )
{
   // Special trace tag from PPU
   return AddSample ( cycle, eTracer_GarbageRead, eSource_PPU, target, 0, 0 );
}

void CTracer::ClearSampleBuffer(void)
{
   m_samples = 0;
   m_cursor = 0;
}

TracerInfo* CTracer::GetLastSample ( void )
{
   TracerInfo* pSample = NULL;

// CPTODO: removed check for enabled
//   if ( CONFIG.IsTracerEnabled() )
   {
      int getsample = m_cursor;

      getsample -= 1;

      if ( getsample < 0 )
      {
         getsample = m_sampleBufferDepth-1;
      }

      pSample = m_pSampleBuffer+getsample;
   }

   return pSample;
}

TracerInfo* CTracer::GetSample ( unsigned int sample )
{
   int getsample = m_cursor;

   getsample -= sample;
   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth+getsample-1;
   }

   return m_pSampleBuffer+getsample;
}

// CPTODO: support functions for finding stuff in the Tracer info from the Tracer dialog.
#if 0
int CTracer::Find ( CString str, int start )
{
   unsigned int idx;
   int found = -1;
   int subIdx;
   char buffer [ 256 ];

   for ( idx = 0; idx < m_samples; idx++ )
   {
      for ( subIdx = eTracerCol_Cycle; subIdx < eTracerCol_MAX; subIdx++ )
      {
         GetPrintable ( idx, subIdx, buffer );
         if ( strncmp((LPCTSTR)str,buffer,strlen(str)) == 0 )
         {
            found = idx;
            break;
         }
      }
   }

   return found;
}
#endif

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

void CTracer::GetPrintable ( unsigned int idx, int subItem, char* str )
{
   TracerInfo* pSample = GetSample ( idx );

   if ( pSample )
   {
      switch ( subItem )
      {
         case eTracerCol_Cycle:
            sprintf ( str, "%u", pSample->cycle );
         break;
         case eTracerCol_Source:
            switch ( pSample->source )
            {
               case eSource_CPU:
                  strcpy ( str, "CPU" );
               break;
               case eSource_PPU:
                  strcpy ( str, "PPU" );
               break;
               case eSource_APU:
                  strcpy ( str, "APU" );
               break;
               case eSource_Mapper:
                  strcpy ( str, "Mapper" );
               break;
               case eSource_User:
                  strcpy ( str, "User" );
               break;
            }
         break;
         case eTracerCol_Type:
            switch ( pSample->type )
            {
               case eTracer_Unknown:
                  strcpy ( str, "" );
               break;
               case eTracer_InstructionFetch:
                  strcpy ( str, "Instruction Fetch" );
               break;
               case eTracer_DataRead:
                  strcpy ( str, "Memory Read" );
               break;
               case eTracer_DataWrite:
                  strcpy ( str, "Memory Write" );
               break;
               case eTracer_DMA:
                  strcpy ( str, "DMA" );
               break;
               case eTracer_RESET:
                  strcpy ( str, "RESET" );
               break;
               case eTracer_NMI:
                  strcpy ( str, "NMI" );
               break;
               case eTracer_IRQ:
                  strcpy ( str, "IRQ" );
               break;
               case eTracer_GarbageRead:
                  strcpy ( str, "Garbage Fetch" );
               break;
               case eTracer_RenderBkgnd:
                  strcpy ( str, "Playfield Render" );
               break;
               case eTracer_RenderSprite:
                  strcpy ( str, "Sprite Render" );
               break;
               case eTracer_Sprite0Hit:
                  strcpy ( str, "Sprite 0 Hit" );
               break;
            }
         break;
         case eTracerCol_Target:
            switch ( pSample->target )
            {
               case eTarget_Unknown:
                  strcpy ( str, "" );
               break;
               case eTarget_RAM:
                  strcpy ( str, "RAM" );
               break;
               case eTarget_PPURegister:
                  strcpy ( str, "PPU Register" );
               break;
               case eTarget_APURegister:
                  strcpy ( str, "APU Register" );
               break;
               case eTarget_IORegister:
                  strcpy ( str, "I/O Register" );
               break;
               case eTarget_SRAM:
                  strcpy ( str, "SRAM" );
               break;
               case eTarget_ROM:
                  strcpy ( str, "ROM" );
               break;
               case eTarget_Mapper:
                  strcpy ( str, "Mapper" );
               break;
               case eTarget_PatternMemory:
                  strcpy ( str, "Pattern Memory" );
               break;
               case eTarget_NameTable:
                  strcpy ( str, "NameTable RAM" );
               break;
               case eTarget_AttributeTable:
                  strcpy ( str, "AttributeTable RAM" );
               break;
               case eTarget_Palette:
                  strcpy ( str, "Palette RAM" );
               break;
            }
         break;
         case eTracerCol_Addr:
            if ( pSample->type == eTracer_GarbageRead )
            {
               strcpy ( str, "$XXXX" );
            }
            else if ( (pSample->type == eTracer_RESET) || 
                      (pSample->type == eTracer_NMI) || 
                      (pSample->type == eTracer_IRQ) )
            {
               strcpy ( str, "" );
            }
            else
            {
               sprintf ( str, "$%04X", pSample->addr );
            }
         break;
         case eTracerCol_CPU_EA:
            if ( pSample->ea == 0xFFFFFFFF )
            {
               strcpy ( str, "" );
            }
            else
            {
               sprintf ( str, "$%04X", pSample->ea );
            }
         break;
         case eTracerCol_Data:
            if ( pSample->type == eTracer_GarbageRead )
            {
               strcpy ( str, "$XX" );
            }
            else if ( (pSample->type == eTracer_RESET) || 
                      (pSample->type == eTracer_NMI) || 
                      (pSample->type == eTracer_IRQ) )
            {
               strcpy ( str, "" );
            }
            else
            {
               sprintf ( str, "$%02X", pSample->data );
            }
         break;
         case eTracerCol_CPU_A:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->a );
            }
         break;
         case eTracerCol_CPU_X:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->x );
            }
         break;
         case eTracerCol_CPU_Y:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->y );
            }
         break;
         case eTracerCol_CPU_SP:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->sp );
            }
         break;
         case eTracerCol_CPU_F:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->f );
            }
         break;
         case eTracerCol_Info:
            if ( (*(pSample->disassemble+3)) == 0x00 )
            {
               // Extra byte indicates whether an instruction should be decoded...
// CPTODO: disassembly removed for now from Tracer
//               C6502::Disassemble ( pSample->disassemble, str );
            }
         break;
      }
   }
}
