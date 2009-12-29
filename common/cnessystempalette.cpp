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

#include "cnessystempalette.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QColor CBasePalette::m_paletteBase [ 64 ] =
{
   QColor ( 102, 102, 102 ),
   QColor ( 0, 42, 136 ),
   QColor ( 20, 18, 167 ),
   QColor ( 59, 0, 164 ),
   QColor ( 92, 0, 126 ),
   QColor ( 110, 0, 64 ),
   QColor ( 108, 6, 0 ),
   QColor ( 86, 29, 0 ),
   QColor ( 51, 53, 0 ),
   QColor ( 11, 72, 0 ),
   QColor ( 0, 82, 0 ),
   QColor ( 0, 79, 8 ),
   QColor ( 0, 64, 77 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 ),
   QColor ( 173, 173, 173 ),
   QColor ( 21, 95, 217 ),
   QColor ( 66, 64, 255 ),
   QColor ( 117, 39, 254 ),
   QColor ( 160, 26, 204 ),
   QColor ( 183, 30, 123 ),
   QColor ( 181, 49, 32 ),
   QColor ( 153, 78, 0 ),
   QColor ( 107, 109, 0 ),
   QColor ( 56, 135, 0 ),
   QColor ( 12, 147, 0 ),
   QColor ( 0, 143, 50 ),
   QColor ( 0 ,124, 141 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 ),
   QColor ( 255, 254, 255 ),
   QColor ( 100, 176, 255 ),
   QColor ( 146, 144, 255 ),
   QColor ( 198, 118, 255 ),
   QColor ( 243, 106, 255 ),
   QColor ( 254, 110, 204 ),
   QColor ( 254, 129, 112 ),
   QColor ( 234, 158, 34 ),
   QColor ( 188, 190, 0 ),
   QColor ( 136, 216, 0 ),
   QColor ( 92, 228, 48 ),
   QColor ( 69, 224, 130 ),
   QColor ( 72, 205, 222 ),
   QColor ( 79, 79, 79 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 ),
   QColor ( 255, 254, 255 ),
   QColor ( 192, 223, 255 ),
   QColor ( 211, 210, 255 ),
   QColor ( 232, 200, 255 ),
   QColor ( 251, 194, 255 ),
   QColor ( 254, 196, 234 ),
   QColor ( 254, 204, 197 ),
   QColor ( 247, 216, 165 ),
   QColor ( 228, 229, 148 ),
   QColor ( 207, 239, 150 ),
   QColor ( 189, 244, 171 ),
   QColor ( 179, 243, 204 ),
   QColor ( 181, 235, 242 ),
   QColor ( 184, 184, 184 ),
   QColor ( 0, 0, 0 ),
   QColor ( 0, 0, 0 )
};

QColor CBasePalette::m_paletteVariants [ 8 ] [ 64 ];

char   CBasePalette::m_paletteRGBs [ 8 ] [ 64 ] [ 3 ];

static CBasePalette __init __attribute((unused));

void CBasePalette::CalculateVariants ( void )
{
   int idx1, idx2;
   int temp;
   double emphfactor [ 8 ] [ 3 ] = {
   {1.00, 1.00, 1.00},
   {1.00, 0.80, 0.81},
   {0.78, 0.94, 0.66},
   {0.79, 0.77, 0.63},
   {0.82, 0.83, 1.12},
   {0.81, 0.71, 0.87},
   {0.68, 0.79, 0.79},
   {0.70, 0.70, 0.70}};

   for ( idx2 = 0; idx2 < NUM_PALETTES; idx2++ )
   {
      m_paletteRGBs [ 0 ] [ idx2 ] [ 0 ] = m_paletteVariants[0][idx2].red();
      m_paletteRGBs [ 0 ] [ idx2 ] [ 1 ] = m_paletteVariants[0][idx2].green();
      m_paletteRGBs [ 0 ] [ idx2 ] [ 2 ] = m_paletteVariants[0][idx2].blue();
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
         m_paletteVariants [ idx1 ] [ idx2 ] = m_paletteBase [ idx2 ];

         temp = m_paletteVariants[idx1][idx2].red();
         temp = ((double)temp)*emphfactor [ idx1 ] [ 0 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ].setRed ( temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 0 ] = temp;

         temp = m_paletteVariants[idx1][idx2].green();
         temp = ((double)temp)*emphfactor [ idx1 ] [ 1 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ].setGreen ( temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 1 ] = temp;

         temp = m_paletteVariants[idx1][idx2].blue();
         temp = ((double)temp)*emphfactor [ idx1 ] [ 2 ];
         if ( temp > 0xFF ) temp = 0xFF;
         m_paletteVariants [ idx1 ] [ idx2 ].setBlue ( temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 2 ] = temp;
      }
   }
}
