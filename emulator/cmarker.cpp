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
      m_marker [ marker ].startCycle = MARKER_NOT_MARKED;
      m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
   }
}

int CMarker::AddMarker(unsigned int absAddr)
{
   int marker = -1;
   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( m_marker[marker].state == eMarkerSet_Invalid )
      {
         m_marker [ marker ].state = eMarkerSet_Started;
         m_marker [ marker ].startAbsAddr = absAddr;
         m_marker [ marker ].startCycle = MARKER_NOT_MARKED;
         m_marker [ marker ].endAbsAddr = absAddr;
         m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
         break;
      }
   }
   return marker;
}

void CMarker::CompleteMarker(int marker, unsigned int absAddr)
{
   m_marker [ marker ].state = eMarkerSet_Complete;
   m_marker [ marker ].endAbsAddr = absAddr;
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

void CMarker::UpdateMarkers(unsigned int absAddr, unsigned int cycle)
{
   int marker;

   for ( marker = 0; marker < MAX_MARKER_SETS; marker++ )
   {
      if ( (m_marker[marker].state == eMarkerSet_Started) ||
           (m_marker[marker].state == eMarkerSet_Complete) )
      {
         if ( m_marker[marker].startAbsAddr == absAddr )
         {
            m_marker [ marker ].startCycle = cycle;
            m_marker [ marker ].endCycle = MARKER_NOT_MARKED;
         }
         if ( m_marker[marker].endAbsAddr == absAddr )
         {
            m_marker [ marker ].endCycle = cycle;
         }
      }
   }
}
