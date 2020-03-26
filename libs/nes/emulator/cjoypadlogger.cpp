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

#include "cjoypadlogger.h"

CJoypadLogger::CJoypadLogger()
{
   m_cursor = 0;
   m_samples = 0;

   m_pSampleBuffer = new JoypadLoggerInfo [ JOYPAD_LOGGER_DEFAULT_DEPTH ];

   m_sampleBufferDepth = JOYPAD_LOGGER_DEFAULT_DEPTH;
}

CJoypadLogger::~CJoypadLogger()
{
   delete [] m_pSampleBuffer;
}

bool CJoypadLogger::ReallocateLoggerMemory(int newDepth)
{
   bool ok = true;

   delete [] m_pSampleBuffer;

   m_pSampleBuffer = new JoypadLoggerInfo [ newDepth ];

   m_sampleBufferDepth = newDepth;

   if ( !m_pSampleBuffer )
   {
      ok = false;
   }

   return ok;
}

JoypadLoggerInfo* CJoypadLogger::AddSample(unsigned int cycle, unsigned char data)
{
   JoypadLoggerInfo* pSample = NULL;

   pSample = m_pSampleBuffer + m_cursor;

   pSample->cycle = cycle;
   pSample->data = data;

   m_cursor++;
   m_cursor %= m_sampleBufferDepth;

   if ( m_samples < m_sampleBufferDepth )
   {
      m_samples++;
   }

   return pSample;
}

void CJoypadLogger::ClearSampleBuffer(void)
{
   m_samples = 0;
   m_cursor = 0;
}

JoypadLoggerInfo* CJoypadLogger::GetLastSample ( void )
{
   JoypadLoggerInfo* pSample = NULL;

   int getsample = m_cursor;

   getsample -= 1;

   if ( getsample < 0 )
   {
      getsample = m_sampleBufferDepth-1;
   }

   pSample = m_pSampleBuffer+getsample;

   return pSample;
}

JoypadLoggerInfo* CJoypadLogger::GetSample ( unsigned int sample )
{
   int getsample = sample;

   return m_pSampleBuffer+getsample;
}

void CJoypadLogger::SetSample ( unsigned int sample, unsigned char data )
{
   JoypadLoggerInfo* pSample = GetSample ( sample );

   pSample->data = data;
}
