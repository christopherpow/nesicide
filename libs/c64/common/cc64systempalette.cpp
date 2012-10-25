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

#include "cc64systempalette.h"

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

uint32_t CBasePalette::m_paletteBase [ 16 ] =
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
   RGB_VALUE ( 0, 0, 0 )
};

int8_t   CBasePalette::m_paletteRGBs [ 16 ] [ 3 ];

static CBasePalette __init __attribute((unused));

void CBasePalette::CalculateVariants ( void )
{
   int idx;

   for ( idx = 0; idx < 16; idx++ )
   {
      m_paletteRGBs [ idx ] [ 0 ] = red(m_paletteBase[idx]);
      m_paletteRGBs [ idx ] [ 1 ] = green(m_paletteBase[idx]);
      m_paletteRGBs [ idx ] [ 2 ] = blue(m_paletteBase[idx]);
   }
}
