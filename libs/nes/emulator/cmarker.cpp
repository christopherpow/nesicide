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

#include "cmarker.h"

static uint8_t markerColors [][3] =
{
   { 255, 0, 0 },
   { 0, 255, 0 },
   { 0, 0, 255 },
   { 255, 255, 0 },
   { 255, 0, 255 },
   { 0, 255, 255 },
   { 255, 255, 255 },
   { 0, 0, 0 }
};

CMarker::CMarker()
{
   int32_t marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      m_marker [ marker ].state = eMarkerSet_Invalid;
      m_marker [ marker ].red = markerColors [ marker ][ 0 ];
      m_marker [ marker ].green = markerColors [ marker ][ 1 ];
      m_marker [ marker ].blue = markerColors [ marker ][ 2 ];
      m_marker [ marker ].minCpuCycles = 0xFFFFFFFF;
      m_marker [ marker ].maxCpuCycles = 0;
      m_marker [ marker ].curCpuCycles = 0;
   }
}

void CMarker::RemoveMarker(int32_t marker)
{
   m_marker [ marker ].state = eMarkerSet_Invalid;
}

void CMarker::RemoveAllMarkers(void)
{
   int32_t marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      RemoveMarker(marker);
   }
}

void CMarker::ZeroMarker(int32_t marker)
{
   m_marker [ marker ].startCpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].startPpuFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].startPpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endCpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].minCpuCycles = 0xFFFFFFFF;
   m_marker [ marker ].maxCpuCycles = 0;
   m_marker [ marker ].curCpuCycles = 0;
}

void CMarker::ZeroAllMarkers(void)
{
   int32_t marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      ZeroMarker(marker);
   }
}

int CMarker::AddSpecificMarker(int32_t marker,uint32_t addr,uint32_t absAddr)
{
   m_marker [ marker ].state = eMarkerSet_Started;
   m_marker [ marker ].startAddr = addr;
   m_marker [ marker ].startAbsAddr = absAddr;
   m_marker [ marker ].startCpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].startPpuFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].startPpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endAddr = addr;
   m_marker [ marker ].endAbsAddr = absAddr;
   m_marker [ marker ].endCpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].minCpuCycles = 0xFFFFFFFF;
   m_marker [ marker ].maxCpuCycles = 0;
   m_marker [ marker ].curCpuCycles = 0;

   return marker;
}

int CMarker::AddMarker(uint32_t addr,uint32_t absAddr)
{
   int32_t marker = -1;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( m_marker[marker].state == eMarkerSet_Invalid )
      {
         AddSpecificMarker(marker,addr,absAddr);
         break;
      }
   }

   return marker;
}

void CMarker::CompleteMarker(int32_t marker,uint32_t addr,uint32_t absAddr)
{
   m_marker [ marker ].state = eMarkerSet_Complete;
   m_marker [ marker ].endAddr = addr;
   m_marker [ marker ].endAbsAddr = absAddr;
   m_marker [ marker ].endCpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].endPpuCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].minCpuCycles = 0xFFFFFFFF;
   m_marker [ marker ].maxCpuCycles = 0;
   m_marker [ marker ].curCpuCycles = 0;
}

int CMarker::FindInProgressMarker(void)
{
   int32_t marker = -1;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( m_marker[marker].state == eMarkerSet_Started )
      {
         break;
      }
   }

   return marker;
}

void CMarker::UpdateMarkers(uint32_t absAddr, uint32_t cpuCycle, uint32_t ppuFrame, uint32_t ppuCycle)
{
   int32_t marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( (m_marker[marker].state == eMarkerSet_Started) ||
            (m_marker[marker].state == eMarkerSet_Complete) )
      {
         if ( m_marker[marker].startAbsAddr == absAddr )
         {
            m_marker [ marker ].startCpuCycle = cpuCycle;
            m_marker [ marker ].startPpuFrame = ppuFrame;
            m_marker [ marker ].startPpuCycle = ppuCycle;
            m_marker [ marker ].endCpuCycle = MARKER_NOT_MARKED;
            m_marker [ marker ].endPpuCycle = MARKER_NOT_MARKED;
         }
         if ( m_marker[marker].endAbsAddr == absAddr )
         {
            m_marker [ marker ].endCpuCycle = cpuCycle;
            m_marker [ marker ].endPpuFrame = ppuFrame;
            m_marker [ marker ].endPpuCycle = ppuCycle;

            m_marker [ marker ].curCpuCycles = m_marker [ marker ].endCpuCycle - m_marker [ marker ].startCpuCycle;
            if ( m_marker[marker].curCpuCycles < m_marker[marker].minCpuCycles )
            {
               m_marker [ marker ].minCpuCycles = m_marker[marker].curCpuCycles;
            }
            if ( m_marker[marker].curCpuCycles > m_marker[marker].maxCpuCycles )
            {
               m_marker [ marker ].maxCpuCycles = m_marker[marker].curCpuCycles;
            }
         }
      }
   }
}
