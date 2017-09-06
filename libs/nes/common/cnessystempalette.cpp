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

inline uint8_t red(uint32_t rgb)
{
   return (rgb>>24)&0xFF;
}
inline uint8_t green(uint32_t rgb)
{
   return (rgb>>16)&0xFF;
}
inline uint8_t blue(uint32_t rgb)
{
   return (rgb>>8)&0xFF;
}

int32_t CBasePalette::m_paletteBase [ 64 ] =
{
   RGB_VALUE ( 102, 102, 102 ),
   RGB_VALUE ( 0, 42, 136 ),
   RGB_VALUE ( 20, 18, 167 ),
   RGB_VALUE ( 59, 0, 164 ),
   RGB_VALUE ( 92, 0, 126 ),
   RGB_VALUE ( 110, 0, 64 ),
   RGB_VALUE ( 108, 6, 0 ),
   RGB_VALUE ( 86, 29, 0 ),
   RGB_VALUE ( 51, 53, 0 ),
   RGB_VALUE ( 11, 72, 0 ),
   RGB_VALUE ( 0, 82, 0 ),
   RGB_VALUE ( 0, 79, 8 ),
   RGB_VALUE ( 0, 64, 77 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 173, 173, 173 ),
   RGB_VALUE ( 21, 95, 217 ),
   RGB_VALUE ( 66, 64, 255 ),
   RGB_VALUE ( 117, 39, 254 ),
   RGB_VALUE ( 160, 26, 204 ),
   RGB_VALUE ( 183, 30, 123 ),
   RGB_VALUE ( 181, 49, 32 ),
   RGB_VALUE ( 153, 78, 0 ),
   RGB_VALUE ( 107, 109, 0 ),
   RGB_VALUE ( 56, 135, 0 ),
   RGB_VALUE ( 12, 147, 0 ),
   RGB_VALUE ( 0, 143, 50 ),
   RGB_VALUE ( 0 ,124, 141 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 255, 254, 255 ),
   RGB_VALUE ( 100, 176, 255 ),
   RGB_VALUE ( 146, 144, 255 ),
   RGB_VALUE ( 198, 118, 255 ),
   RGB_VALUE ( 243, 106, 255 ),
   RGB_VALUE ( 254, 110, 204 ),
   RGB_VALUE ( 254, 129, 112 ),
   RGB_VALUE ( 234, 158, 34 ),
   RGB_VALUE ( 188, 190, 0 ),
   RGB_VALUE ( 136, 216, 0 ),
   RGB_VALUE ( 92, 228, 48 ),
   RGB_VALUE ( 69, 224, 130 ),
   RGB_VALUE ( 72, 205, 222 ),
   RGB_VALUE ( 79, 79, 79 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 255, 254, 255 ),
   RGB_VALUE ( 192, 223, 255 ),
   RGB_VALUE ( 211, 210, 255 ),
   RGB_VALUE ( 232, 200, 255 ),
   RGB_VALUE ( 251, 194, 255 ),
   RGB_VALUE ( 254, 196, 234 ),
   RGB_VALUE ( 254, 204, 197 ),
   RGB_VALUE ( 247, 216, 165 ),
   RGB_VALUE ( 228, 229, 148 ),
   RGB_VALUE ( 207, 239, 150 ),
   RGB_VALUE ( 189, 244, 171 ),
   RGB_VALUE ( 179, 243, 204 ),
   RGB_VALUE ( 181, 235, 242 ),
   RGB_VALUE ( 184, 184, 184 ),
   RGB_VALUE ( 0, 0, 0 ),
   RGB_VALUE ( 0, 0, 0 )
};

int32_t CBasePalette::m_paletteVariants [ 8 ] [ 64 ];

int8_t   CBasePalette::m_paletteRGBs [ 8 ] [ 64 ] [ 3 ];

static CBasePalette __init __attribute__((unused));

void CBasePalette::CalculateVariants ( void )
{
   int idx1, idx2;
   int temp;
   double emphfactor [ 8 ] [ 3 ] =
   {
      {1.00, 1.00, 1.00},
      {1.00, 0.80, 0.81},
      {0.78, 0.94, 0.66},
      {0.79, 0.77, 0.63},
      {0.82, 0.83, 1.12},
      {0.81, 0.71, 0.87},
      {0.68, 0.79, 0.79},
      {0.70, 0.70, 0.70}
   };

   for ( idx2 = 0; idx2 < 64; idx2++ )
   {
      m_paletteRGBs [ 0 ] [ idx2 ] [ 0 ] = red(m_paletteVariants[0][idx2]);
      m_paletteRGBs [ 0 ] [ idx2 ] [ 1 ] = green(m_paletteVariants[0][idx2]);
      m_paletteRGBs [ 0 ] [ idx2 ] [ 2 ] = blue(m_paletteVariants[0][idx2]);
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
      for ( idx2 = 0; idx2 < 64; idx2++ )
      {
         m_paletteVariants [ idx1 ] [ idx2 ] = m_paletteBase [ idx2 ];

         temp = red(m_paletteVariants[idx1][idx2]);
         temp = ((double)temp)*emphfactor [ idx1 ] [ 0 ];

         if ( temp > 0xFF )
         {
            temp = 0xFF;
         }

         setRed ( m_paletteVariants [ idx1 ] [ idx2 ], temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 0 ] = temp;

         temp = green(m_paletteVariants[idx1][idx2]);
         temp = ((double)temp)*emphfactor [ idx1 ] [ 1 ];

         if ( temp > 0xFF )
         {
            temp = 0xFF;
         }

         setGreen ( m_paletteVariants [ idx1 ] [ idx2 ], temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 1 ] = temp;

         temp = blue(m_paletteVariants[idx1][idx2]);
         temp = ((double)temp)*emphfactor [ idx1 ] [ 2 ];

         if ( temp > 0xFF )
         {
            temp = 0xFF;
         }

         setBlue ( m_paletteVariants [ idx1 ] [ idx2 ], temp );
         m_paletteRGBs [ idx1 ] [ idx2 ] [ 2 ] = temp;
      }
   }
}
