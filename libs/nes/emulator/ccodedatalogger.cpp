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

#include "ccodedatalogger.h"

#include "nes_emulator_core.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

uint32_t CCodeDataLogger::m_curCycle = 0;
LoggerInfo* CCodeDataLogger::m_pLastLoad = NULL;

CCodeDataLogger::CCodeDataLogger(uint32_t size, uint32_t mask)
{
   m_size = size;
   m_mask = mask;
   m_pLogger = new LoggerInfo [ size ];
   ClearData ();
}

CCodeDataLogger::~CCodeDataLogger()
{
   delete [] m_pLogger;
}

void CCodeDataLogger::ClearData ( void )
{
   uint32_t idx = 0;

   for ( idx = 0; idx < m_size; idx++ )
   {
      m_pLogger [ idx ].count = 0;
   }

   m_maxCount = 1;
}

uint32_t CCodeDataLogger::GetLastLoadAddr ( uint32_t addr )
{
   uint32_t laddr = 0xFFFFFFFF;

   LoggerInfo* pLogger = (*(m_pLogger+(addr&m_mask))).pLastLoad;

   if ( pLogger )
   {
      laddr = pLogger->cpuAddr;
   }

   return laddr;
}

void CCodeDataLogger::LogAccess ( uint32_t cycle, uint32_t addr, uint8_t data, int8_t type, int8_t source )
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

void CCodeDataLogger::GetPrintable ( uint32_t addr, int32_t subItem, char* str )
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
            sprintf ( str, "%04X", pLogger->cpuAddr );
         }
         else
         {
            strcpy ( str, "N/A" );
         }

         break;
      case eLoggerCol_Source:

         switch ( pLogger->source )
         {
            case eNESSource_CPU:
               strcpy ( str, "CPU" );
               break;
            case eNESSource_PPU:
               strcpy ( str, "PPU" );
               break;
            case eNESSource_APU:
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
         case eLogger_OperandFetch:
            strcpy ( str, "Operand Fetch" );
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
         sprintf ( str, "%04X", pLogger->cpuAddr );
         break;
      case eLoggerCol_Count:
         sprintf ( str, "%u", pLogger->count );
         break;
   }
}
