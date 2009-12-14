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
    
// BasePalette.cpp: implementation of the CBasePalette class.
//
//////////////////////////////////////////////////////////////////////

// CPTODO: include for ?
//#include "NESICIDE.h"
#include "BasePalette.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLORREF CBasePalette::m_paletteBase [ 64 ] = 
{ 
   RGB ( 102, 102, 102 ),
   RGB ( 0, 42, 136 ),
   RGB ( 20, 18, 167 ),
   RGB ( 59, 0, 164 ),
   RGB ( 92, 0, 126 ),
   RGB ( 110, 0, 64 ),
   RGB ( 108, 6, 0 ),
   RGB ( 86, 29, 0 ),
   RGB ( 51, 53, 0 ),
   RGB ( 11, 72, 0 ),
   RGB ( 0, 82, 0 ),
   RGB ( 0, 79, 8 ),
   RGB ( 0, 64, 77 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 ),
   RGB ( 173, 173, 173 ),
   RGB ( 21, 95, 217 ),
   RGB ( 66, 64, 255 ),
   RGB ( 117, 39, 254 ),
   RGB ( 160, 26, 204 ),
   RGB ( 183, 30, 123 ),
   RGB ( 181, 49, 32 ),
   RGB ( 153, 78, 0 ),
   RGB ( 107, 109, 0 ),
   RGB ( 56, 135, 0 ),
   RGB ( 12, 147, 0 ),
   RGB ( 0, 143, 50 ),
   RGB ( 0 ,124, 141 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 ),
   RGB ( 255, 254, 255 ),
   RGB ( 100, 176, 255 ),
   RGB ( 146, 144, 255 ),
   RGB ( 198, 118, 255 ),
   RGB ( 243, 106, 255 ),
   RGB ( 254, 110, 204 ),
   RGB ( 254, 129, 112 ),
   RGB ( 234, 158, 34 ),
   RGB ( 188, 190, 0 ),
   RGB ( 136, 216, 0 ),
   RGB ( 92, 228, 48 ),
   RGB ( 69, 224, 130 ),
   RGB ( 72, 205, 222 ),
   RGB ( 79, 79, 79 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 ),
   RGB ( 255, 254, 255 ),
   RGB ( 192, 223, 255 ),
   RGB ( 211, 210, 255 ),
   RGB ( 232, 200, 255 ),
   RGB ( 251, 194, 255 ),
   RGB ( 254, 196, 234 ),
   RGB ( 254, 204, 197 ),
   RGB ( 247, 216, 165 ),
   RGB ( 228, 229, 148 ),
   RGB ( 207, 239, 150 ),
   RGB ( 189, 244, 171 ),
   RGB ( 179, 243, 204 ),
   RGB ( 181, 235, 242 ),
   RGB ( 184, 184, 184 ),
   RGB ( 0, 0, 0 ),
   RGB ( 0, 0, 0 )
};

COLORREF CBasePalette::m_paletteVariants [ 8 ] [ 64 ]; 

COLORREF CBasePalette::m_paletteDisplayVariants [ 8 ] [ 64 ]; 

static CBasePalette __init;

void CBasePalette::CalculateVariants ( void )
{
   int idx1, idx2;
   COLORREF temp;
   double emphfactor [ 8 ] [ 3 ] = {
   {1.00, 1.00, 1.00},
   {1.00, 0.80, 0.81},
   {0.78, 0.94, 0.66},
   {0.79, 0.77, 0.63},
   {0.82, 0.83, 1.12},
   {0.81, 0.71, 0.87},
   {0.68, 0.79, 0.79},
   {0.70, 0.70, 0.70}};

   memcpy ( m_paletteDisplayVariants[0], m_paletteVariants[0], sizeof(m_paletteBase) );
   for ( idx1 = 0; idx1 < NUM_PALETTES; idx1++ )
   {
      m_paletteDisplayVariants[0][idx1] = SWAPRB(m_paletteDisplayVariants[0][idx1]);
   }

   for ( idx1 = 1; idx1 < 8; idx1++ )
   {
      // Calculate emphasis palettes:
      // 0: none
      // 1: red only
      // 2: green only
      // 3: red and green
      // 4: blue only
      // 5: red and blue
      // 6: green and blue
      // 7: all
      for ( idx2 = 0; idx2 < NUM_PALETTES; idx2++ )
      {
         m_paletteVariants [ idx1 ] [ idx2 ] = m_paletteVariants [ 0 ] [ idx2 ];
         m_paletteDisplayVariants [ idx1 ] [ idx2 ] = m_paletteVariants [ 0 ] [ idx2 ];

         temp = (m_paletteVariants[idx1][idx2])&0xFF;
         temp = ((double)temp)*emphfactor [ idx1 ] [ 0 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ] &= 0xFFFF00;
         m_paletteVariants [ idx1 ] [ idx2 ] |= temp;

         temp = ((m_paletteVariants[idx1][idx2])>>8)&0xFF;
         temp = ((double)temp)*emphfactor [ idx1 ] [ 1 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ] &= 0xFF00FF;
         m_paletteVariants [ idx1 ] [ idx2 ] |= (temp<<8);

         temp = ((m_paletteVariants[idx1][idx2])>>16)&0xFF;
         temp = ((double)temp)*emphfactor [ idx1 ] [ 2 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ] &= 0x00FFFF;
         m_paletteVariants [ idx1 ] [ idx2 ] |= (temp<<16);

         m_paletteDisplayVariants [ idx1 ] [ idx2 ] = SWAPRB(m_paletteVariants[idx1][idx2]);
      }
   }
}

// CPTODO: use Qt methods for project palette serialization...
#if 0
void CBasePalette::Serialize ( CArchive& ar )
{
   if ( ar.IsStoring() )
   {
      ar.Write ( m_paletteVariants[0], sizeof(m_paletteBase) );
   }
   else
   {
      ar.Read ( m_paletteVariants[0], sizeof(m_paletteBase) );
      CalculateVariants ();
   }
}
#endif
