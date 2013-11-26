/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2012  Jonathan Liss
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

// Some graphics helpers

#include "stdafx.h"
#include "Graphics.h"

void GradientRectTriple(CDC *pDC, int x, int y, int w, int h, COLORREF c1, COLORREF c2, COLORREF c3)
{
	// c1 -> c2 -> c3
	TRIVERTEX Vertices[4];
	GRADIENT_RECT Rects[2];

	Vertices[0].x = x;
	Vertices[0].y = y;
	Vertices[0].Red = RED(c1) << 8;
	Vertices[0].Green = GREEN(c1) << 8;
	Vertices[0].Blue = BLUE(c1) << 8;
	Vertices[0].Alpha = 0xFF00;

	Vertices[1].x = x + w;
	Vertices[1].y = y + h / 3;
	Vertices[1].Red = RED(c2) << 8;
	Vertices[1].Green = GREEN(c2) << 8;
	Vertices[1].Blue = BLUE(c2) << 8;
	Vertices[1].Alpha = 0xFF00;

	Vertices[2].x = x;
	Vertices[2].y = y + h / 3;
	Vertices[2].Red = RED(c2) << 8;
	Vertices[2].Green = GREEN(c2) << 8;
	Vertices[2].Blue = BLUE(c2) << 8;
	Vertices[2].Alpha = 0xFF00;

	Vertices[3].x = x + w;
	Vertices[3].y = y + h;
	Vertices[3].Red = RED(c3) << 8;
	Vertices[3].Green = GREEN(c3) << 8;
	Vertices[3].Blue = BLUE(c3) << 8;
	Vertices[3].Alpha = 0xFF00;

	Rects[0].UpperLeft = 0;
	Rects[0].LowerRight = 1;
	Rects[1].UpperLeft = 2;
	Rects[1].LowerRight = 3;

	pDC->GradientFill(Vertices, 4, Rects, 2, GRADIENT_FILL_RECT_V);
}

void GradientBar(CDC *pDC, int x, int y, int w, int h, COLORREF col_fg, COLORREF col_bg)
{
	TRIVERTEX Vertices[2];
	GRADIENT_RECT Rect;

	unsigned int col2 = BLEND(col_fg, col_bg, 60);
	unsigned int top_col = BLEND(col_fg, 0xFFFFFF, 95);

	Vertices[0].x = x;
	Vertices[0].y = y + 1;
	Vertices[0].Red = RED(col_fg) << 8;
	Vertices[0].Green = GREEN(col_fg) << 8;
	Vertices[0].Blue = BLUE(col_fg) << 8;
	Vertices[0].Alpha = 0xFF00;

	Vertices[1].x = x + w;
	Vertices[1].y = y + h;
	Vertices[1].Red = RED(col2) << 8;
	Vertices[1].Green = GREEN(col2) << 8;
	Vertices[1].Blue = BLUE(col2) << 8;
	Vertices[1].Alpha = 0xFF00;

	Rect.UpperLeft = 0;
	Rect.LowerRight = 1;

	pDC->FillSolidRect(x, y, w, 1, top_col);
	pDC->GradientFill(Vertices, 2, &Rect, 1, GRADIENT_FILL_RECT_V);
}
