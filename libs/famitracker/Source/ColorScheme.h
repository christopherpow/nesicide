/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
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

#pragma once


// Default font
const static TCHAR *FONT_FACE = _T("Verdana");
const static int FONT_SIZE	  = 12;

// Static colors
struct _STATIC_COLOR_SCHEME {
	COLORREF CHANNEL_NORMAL;	// Normal channel name
	COLORREF CHANNEL_MUTED;	// Muted channel name
	COLORREF FRAME_LIGHT;	// 3D frame
	COLORREF FRAME_DARK;	// 3D frame
};

const struct _STATIC_COLOR_SCHEME STATIC_COLOR_SCHEME = 
{
   0x00202020,	// Normal channel name
	0x002020E0,	// Muted channel name
	0x00FFFFFF,	// 3D frame
	0x00808080	// 3D frame
};

// Blend levels
struct _SHADE_LEVEL {
	int SEPARATOR;	// Channel separators
	int EMPTY_BG;	// Empty background
	int UNFOCUSED;	// Unfocused cursor
	int FOCUSED;	// Focused cursor
	int SELECT;	// Selection box
	int SELECT_EDGE;	// Edge of selection box
	int UNUSED;	// Empty pattern
	int PREVIEW;	// Pattern preview
	int TEXT_SHADOW;
	int HOVER;
	int EDIT_MODE;
};

const struct _SHADE_LEVEL SHADE_LEVEL = 
{
   75,	// Channel separators
	70,	// Empty background
	50,	// Unfocused cursor
	80,	// Focused cursor
	80,	// Selection box
	70,	// Edge of selection box
	30,	// Empty pattern
	50,	// Pattern preview
	20,
	80,
	80
};

// Custom colors
struct COLOR_SCHEME {
	const TCHAR		*NAME;
	const COLORREF	BACKGROUND;
	const COLORREF	BACKGROUND_HILITE;
	const COLORREF	BACKGROUND_HILITE2;
	const COLORREF	TEXT_NORMAL;
	const COLORREF	TEXT_HILITE;
	const COLORREF	TEXT_HILITE2;
	const COLORREF	TEXT_INSTRUMENT;
	const COLORREF	TEXT_VOLUME;
	const COLORREF	TEXT_EFFECT;
	const COLORREF	SELECTION;
	const COLORREF	CURSOR;
	const TCHAR		*FONT_FACE;
	const int		FONT_SIZE;
};

// Default
const COLOR_SCHEME DEFAULT_COLOR_SCHEME = {
	_T("Default"),		// Name
	0x00000000,			// Background color
	0x00001010,			// Highlighted background color
	0x00002020,			// Highlighted background color 2
	0x0000FF00,			// Normal text color
	0x0000F0F0,			// Highlighted text color
	0x0060FFFF,			// Highlighted text color 2
	0x0080FF80,			// Instrument color
	0x00FF8080,			// Volume color
	0x008080FF,			// Effect color
	0x00F27D86,			// Selection color
	0x00808080,			// Cursor color
	_T("Verdana"),		// Font
	12					// Font size
};

// Monochrome
const COLOR_SCHEME MONOCHROME_COLOR_SCHEME = {
	_T("Monochrome"),	// Name
	0x00181818,			// Background color
	0x00202020,			// Highlighted background color
	0x00303030,			// Highlighted background color 2
	0x00C0C0C0,			// Normal text color
	0x00F0F0F0,			// Highlighted text color
	0x00FFFFFF,			// Highlighted text color 2
	0x0080FF80,			// Instrument color
	0x00FF8080,			// Volume color
	0x008080FF,			// Effect color
	0x00454550,			// Selection color
	0x00908080,			// Cursor color
	_T("Fixedsys"),		// Font
	12					// Font size
};

// Renoise
const COLOR_SCHEME RENOISE_COLOR_SCHEME = {
	_T("Renoise"),		// Name
	0x00131313,			// Background color
	0x00231A18,			// Highlighted background color
	0x00342b29,			// Highlighted background color 2
	0x00FBF4F0,			// Normal text color
	0x00FFD6B9,			// Highlighted text color
	0x00FFF6C9,			// Highlighted text color 2
	0x0080FF80,			// Instrument color
	0x00FF8080,			// Volume color
	0x008080FF,			// Effect color
	0x00FF8080,			// Selection color
	0x00707070,			// Cursor color
	_T("Fixedsys"),		// Font
	12					// Font size
};

// White
const COLOR_SCHEME WHITE_COLOR_SCHEME = {
	_T("White"),		// Name
	0x00FFFFFF,			// Background color
	0x00FFFFFF,			// Highlighted background color
	0x00FFF0FF,			// Highlighted background color 2
	0x00000000,			// Normal text color
	0x00FF0000,			// Highlighted text color
	0x00FF2020,			// Highlighted text color 2
	0x00000000,			// Instrument color
	0x00000000,			// Volume color
	0x00000000,			// Effect color
	0x00FF8080,			// Selection color
	0x00D0A0A0,			// Cursor color
	_T("Courier"),		// Font
	12					// Font size
};
