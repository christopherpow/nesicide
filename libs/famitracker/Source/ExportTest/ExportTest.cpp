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

#include <vector>
#include "../stdafx.h"
#include "../FamiTracker.h"
#include "../FamiTrackerDoc.h"
#include "../Compiler.h"
#include "ExportTest.h"

/*
 * This class is used for export verification.
 *
 */

#ifdef EXPORT_TEST

CExportTest::CExportTest()
{
	m_pHeader = new stNSFHeader;
}

CExportTest::~CExportTest()
{
	SAFE_RELEASE(m_pHeader);
}

bool CExportTest::Setup()
{
	HMODULE hModule = LoadLibrary(_T("ExportTest.dll"));

	if (hModule == NULL) {
		AfxMessageBox(_T("Could not load ExportTest.dll"));
		return false;
	}

	ImportFuncs.LoadFileFunc = (LoadFile_t)GetProcAddress(hModule, "LoadFile");
	ImportFuncs.RunFrameFunc = (RunFrame_t)GetProcAddress(hModule, "RunFrame");
	ImportFuncs.ReadResultFunc = (ReadResult_t)GetProcAddress(hModule, "ReadResult");

	if (ImportFuncs.LoadFileFunc == NULL)
		return false;

	if (ImportFuncs.RunFrameFunc == NULL)
		return false;

	if (ImportFuncs.ReadResultFunc == NULL)
		return false;

	CFamiTrackerDoc *pDoc = CFamiTrackerDoc::GetDoc();
	CCompiler Compiler(pDoc, NULL);

	char *pMemory;
	pMemory = new char[0x10000];
	//stNSFHeader header;

	int song = 0;

	Compiler.ExportTest(pMemory, m_pHeader, MACHINE_NTSC);

	// Setup memory
	ImportFuncs.LoadFileFunc(pMemory, m_pHeader->LoadAddr, 0x10000);

	SAFE_RELEASE_ARRAY(pMemory);

	return true;
}

void CExportTest::RunInit(int Song)
{
	int cycles = ImportFuncs.RunFrameFunc(m_pHeader->InitAddr, Song);
}

void CExportTest::RunPlay()
{
	int cycles = ImportFuncs.RunFrameFunc(m_pHeader->PlayAddr, 0);
}

unsigned char CExportTest::ReadReg(int Reg)
{
	return ImportFuncs.ReadResultFunc(Reg);
}

#endif /* EXPORT_TEST */

