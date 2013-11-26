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

#pragma once

#ifdef EXPORT_TEST

// DLL imports
typedef void (*LoadFile_t)(char*, int, int);
typedef int (*RunFrame_t)(unsigned int, int);
typedef unsigned char (*ReadResult_t)(unsigned int);

struct stNSFHeader;

// Test class
class CExportTest
{
public:
	CExportTest();
	~CExportTest();

	bool Setup();
	void RunInit(int Song);
	void RunPlay();

	unsigned char ReadReg(int Reg);

private:
	struct {
		LoadFile_t		LoadFileFunc;
		RunFrame_t		RunFrameFunc;
		ReadResult_t	ReadResultFunc;
	} ImportFuncs;

	stNSFHeader *m_pHeader;
};

#endif /* EXPORT_TEST */
