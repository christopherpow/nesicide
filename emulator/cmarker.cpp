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

static unsigned char markerColors [][3] =
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
   int marker;
   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      m_marker [ marker ].state = eMarkerSet_Invalid;
      m_marker [ marker ].red = markerColors [ marker ][ 0 ];
      m_marker [ marker ].green = markerColors [ marker ][ 1 ];
      m_marker [ marker ].blue = markerColors [ marker ][ 2 ];
   }
}

void CMarker::ClearAllMarkers(void)
{
   int marker;
   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      m_marker [ marker ].state = eMarkerSet_Invalid;
   }
}

void CMarker::ZeroAllMarkers(void)
{
   int marker;
   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      m_marker [ marker ].startFrame = MARKER_NOT_MARKED;
      m_marker [ marker ].startCycle = MARKER_NOT_MARKED;
      m_marker [ marker ].endFrame = MARKER_NOT_MARKED;
      m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
   }
}

int CMarker::AddSpecificMarker(int marker, unsigned int absAddr)
{
   m_marker [ marker ].state = eMarkerSet_Started;
   m_marker [ marker ].startAbsAddr = absAddr;
   m_marker [ marker ].startFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].startCycle = MARKER_NOT_MARKED;
   m_marker [ marker ].endAbsAddr = absAddr;
   m_marker [ marker ].endFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].endCycle = MARKER_NOT_MARKED;

   return marker;
}

int CMarker::AddMarker(unsigned int absAddr)
{
   int marker = -1;
   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( m_marker[marker].state == eMarkerSet_Invalid )
      {
         AddSpecificMarker(marker,absAddr);
         break;
      }
   }
   return marker;
}

void CMarker::CompleteMarker(int marker, unsigned int absAddr)
{
   m_marker [ marker ].state = eMarkerSet_Complete;
   m_marker [ marker ].endAbsAddr = absAddr;
   m_marker [ marker ].endFrame = MARKER_NOT_MARKED;
   m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
}

int CMarker::FindInProgressMarker(void)
{
   int marker = -1;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( m_marker[marker].state == eMarkerSet_Started )
      {
         break;
      }
   }

   return marker;
}

void CMarker::UpdateMarkers(unsigned int absAddr, unsigned int frame, unsigned int cycle)
{
   int marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( (m_marker[marker].state == eMarkerSet_Started) ||
           (m_marker[marker].state == eMarkerSet_Complete) )
      {
         if ( m_marker[marker].startAbsAddr == absAddr )
         {
            m_marker [ marker ].startFrame = frame;
            m_marker [ marker ].startCycle = cycle;
            m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
         }
         if ( m_marker[marker].endAbsAddr == absAddr )
         {
            m_marker [ marker ].endFrame = frame;
            m_marker [ marker ].endCycle = cycle;
         }
      }
   }
}
