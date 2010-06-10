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
    
// CodeDataLogger.cpp: implementation of the CCodeDataLogger class.
//
//////////////////////////////////////////////////////////////////////

#include "ccodedatalogger.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT CCodeDataLogger::m_curCycle = 0;
UINT CCodeDataLogger::m_maxCount = 1;
LoggerInfo* CCodeDataLogger::m_pLastLoad = NULL;

CCodeDataLogger::CCodeDataLogger(UINT size, UINT mask)
{
   m_pLogger = new LoggerInfo [ size ];
   m_size = size;
   m_mask = mask;
   ClearData ();
}

CCodeDataLogger::~CCodeDataLogger()
{
   delete [] m_pLogger;
}

unsigned char CCodeDataLogger::GetLastValue ( UINT addr, unsigned char count )
{
   LoggerInfo* pLogger = m_pLogger+(addr&m_mask);
   int pos = pLogger->lastValuePos;

   pos -= count;
   pos -= 1;
   if ( pos < 0 )
   {
      pos = LAST_VALUE_LIST_LEN+pos;
   }
   return pLogger->lastValue [ pos ];
}

void CCodeDataLogger::ClearData ( void )
{
   unsigned int idx = 0;
   for ( idx = 0; idx < m_size; idx++ )
   {
      m_pLogger [ idx ].count = 0;
      m_pLogger [ idx ].lastValueCount = 0;
      m_pLogger [ idx ].lastValuePos = 0;
   }
   m_maxCount = 1;
}

unsigned int CCodeDataLogger::GetLastLoadAddr ( UINT addr )
{
   unsigned int laddr = 0xFFFFFFFF;

   LoggerInfo* pLogger = (*(m_pLogger+addr)).pLastLoad;

   if ( pLogger )
   {
      laddr = pLogger->cpuAddr;
   }

   return laddr;
}

void CCodeDataLogger::LogAccess ( unsigned int cycle, UINT addr, unsigned char data, char type, char source )
{
   LoggerInfo* pLogger = m_pLogger+(addr&m_mask);

   pLogger->cpuAddr = addr;
   pLogger->cycle = cycle;
   pLogger->type = type;
   pLogger->source = source;
   if ( pLogger->count < 0xFFFFFFFF )
   {
      pLogger->count++;
   }
   if ( pLogger->count > m_maxCount )
   {
      m_maxCount = pLogger->count;
   }
   *(pLogger->lastValue+pLogger->lastValuePos) = data;
   pLogger->lastValuePos++;
   pLogger->lastValueCount++;
   if ( pLogger->lastValueCount >= LAST_VALUE_LIST_LEN )
   {
      pLogger->lastValueCount = LAST_VALUE_LIST_LEN;
   }
   if ( pLogger->lastValuePos >= LAST_VALUE_LIST_LEN )
   {
      pLogger->lastValuePos = 0;
   }
   pLogger->pLastLoad = NULL;
   if ( (m_pLastLoad) &&
        (type == eLogger_DataWrite) )
   {
      pLogger->pLastLoad = m_pLastLoad;
   }
   if ( type == eLogger_DataRead )
   {
      m_pLastLoad = pLogger;
   }

   m_curCycle = cycle;
}

void CCodeDataLogger::GetPrintable ( UINT addr, int subItem, char* str )
{
   LoggerInfo* pLogger = m_pLogger+(addr&m_mask);

   switch ( subItem )
   {
      case eLoggerCol_Cycle:
         sprintf ( str, "%u", pLogger->cycle );
      break;
      case eLoggerCol_LastLoadAddr:
         pLogger = pLogger->pLastLoad;
         if ( pLogger )
         {
            sprintf ( str, "$%04X", pLogger->cpuAddr );
         }
         else
         {
            strcpy ( str, "N/A" );
         }
      break;
      case eLoggerCol_Source:
         switch ( pLogger->source )
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
         }
      break;
      case eLoggerCol_Type:
         switch ( pLogger->type )
         {
            case eLogger_InstructionFetch:
               strcpy ( str, "Instruction Fetch" );
            break;
            case eLogger_DataRead:
               strcpy ( str, "Memory Read" );
            break;
            case eLogger_DMA:
               strcpy ( str, "DMA" );
            break;
            case eLogger_DataWrite:
               strcpy ( str, "Memory Write" );
            break;
         }
      break;
      case eLoggerCol_CPUAddr:
         sprintf ( str, "$%04X", pLogger->cpuAddr );
      break;
      case eLoggerCol_Count:
         sprintf ( str, "%u", pLogger->count );
      break;
   }
}
