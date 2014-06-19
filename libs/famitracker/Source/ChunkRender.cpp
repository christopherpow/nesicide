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

#include <map>
#include <vector>
#include "stdafx.h"
#include "Chunk.h"
#include "ChunkRender.h"

/**
 * Text chunk render
 *
 */

// String render functions
const stChunkRenderFunc CChunkRenderText::RENDER_FUNCTIONS[] = {
	{CHUNK_HEADER,			&CChunkRenderText::StoreHeaderChunk},
	{CHUNK_SEQUENCE,		&CChunkRenderText::StoreSequenceChunk},
	{CHUNK_INSTRUMENT_LIST,	&CChunkRenderText::StoreInstrumentListChunk},
	{CHUNK_INSTRUMENT,		&CChunkRenderText::StoreInstrumentChunk},
	{CHUNK_SAMPLE_LIST,		&CChunkRenderText::StoreSampleListChunk},
	{CHUNK_SAMPLE_POINTERS,	&CChunkRenderText::StoreSamplePointersChunk},
	{CHUNK_SONG_LIST,		&CChunkRenderText::StoreSongListChunk},
	{CHUNK_SONG,			&CChunkRenderText::StoreSongChunk},
	{CHUNK_FRAME_LIST,		&CChunkRenderText::StoreFrameListChunk},
	{CHUNK_FRAME,			&CChunkRenderText::StoreFrameChunk},
	{CHUNK_PATTERN,			&CChunkRenderText::StorePatternChunk},
	{CHUNK_WAVETABLE,		&CChunkRenderText::StoreWavetableChunk},
	{CHUNK_WAVES,			&CChunkRenderText::StoreWavesChunk}
};

void CChunkRenderText::StoreChunks(std::vector<CChunk*> &m_vChunks, CFile *pFile)
{
	// Generate strings
	for (std::vector<CChunk*>::iterator it = m_vChunks.begin(); it != m_vChunks.end(); ++it) {
		for (int j = 0; j < sizeof(RENDER_FUNCTIONS) / sizeof(stChunkRenderFunc); ++j) {
			if ((*it)->GetType() == RENDER_FUNCTIONS[j].type)
				(this->*RENDER_FUNCTIONS[j].function)(*it, pFile);
		}
	}
	
	// Write strings to file
	WriteFileString(pFile, CStringA("; FamiTracker exported music data\n;\n\n"));

	// Module header
	DumpStrings(CStringA("; Module header\n"), CStringA("\n"), m_headerStrings, pFile);

	// Instrument list
	DumpStrings(CStringA("; Instrument pointer list\n"), CStringA("\n"), m_instrumentListStrings, pFile);
	DumpStrings(CStringA("; Instruments\n"), CStringA(""), m_instrumentStrings, pFile);

	// Sequences
	DumpStrings(CStringA("; Sequences\n"), CStringA("\n"), m_sequenceStrings, pFile);

	// Waves (FDS & N163)
	if (m_wavetableStrings.IsEmpty() == FALSE) {
		DumpStrings(CStringA("; FDS waves\n"), CStringA("\n"), m_wavetableStrings, pFile);
	}

	if (m_wavesStrings.IsEmpty() == FALSE) {
		DumpStrings(CStringA("; N163 waves\n"), CStringA("\n"), m_wavesStrings, pFile);
	}

	// Samples
	DumpStrings(CStringA("; DPCM instrument list (pitch, sample index)\n"), CStringA("\n"), m_sampleListStrings, pFile);
	DumpStrings(CStringA("; DPCM samples list (location, size, bank)\n"), CStringA("\n"), m_samplePointersStrings, pFile);

	// Songs
	DumpStrings(CStringA("; Song pointer list\n"), CStringA("\n"), m_songListStrings, pFile);
	DumpStrings(CStringA("; Song info\n"), CStringA("\n"), m_songStrings, pFile);

	// Song data
	DumpStrings(CStringA(";\n; Pattern and frame data for all songs below\n;\n\n"), CStringA(""), m_songDataStrings, pFile);

	// Actual DPCM samples are stored later
}

void CChunkRenderText::DumpStrings(const CStringA &preStr, const CStringA &postStr, CStringArray &stringArray, CFile *pFile) const
{
	WriteFileString(pFile, preStr);

	for (int i = 0; i < stringArray.GetCount(); ++i) {
		WriteFileString(pFile, stringArray[i]);
	}

	WriteFileString(pFile, postStr);
}

void CChunkRenderText::StoreHeaderChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();
	int i = 0;

	str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));
	str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));
	str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));
	str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));
	str.AppendFormat("\t.byte %i ; flags\n", pChunk->GetData(i++));
	if (pChunk->IsDataReference(i))
		str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));	// FDS waves
	str.AppendFormat("\t.word %i ; NTSC speed\n", pChunk->GetData(i++));
	str.AppendFormat("\t.word %i ; PAL speed\n", pChunk->GetData(i++));
	if (i < pChunk->GetLength())
		str.AppendFormat("\t.word %i ; N163 channels\n", pChunk->GetData(i++));	// N163 channels

	m_headerStrings.Add(str);
}

void CChunkRenderText::StoreInstrumentListChunk(CChunk *pChunk, CFile *pFile)
{
	CString str;

	// Store instrument pointers
	str.Format(_T("%s:\n"), pChunk->GetLabel());

	for (int i = 0; i < pChunk->GetLength(); ++i) {
		str.AppendFormat(_T("\t.word %s\n"), pChunk->GetDataRefName(i));
	}

	m_instrumentListStrings.Add(str);
}

void CChunkRenderText::StoreInstrumentChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

	str.Format("%s:\n\t.byte %i\n", pChunk->GetLabel(), pChunk->GetData(0));

	for (int i = 1; i < len; ++i) {
		if (pChunk->IsDataReference(i)) {
			str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i));
		}
		else {
			if (pChunk->GetDataSize(i) == 1) {
				str.AppendFormat("\t.byte $%02X\n", pChunk->GetData(i));
			}
			else {
				str.AppendFormat("\t.word $%04X\n", pChunk->GetData(i));
			}
		}
	}

	str.Append("\n");

	m_instrumentStrings.Add(str);
}

void CChunkRenderText::StoreSequenceChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;

	str.Format("%s:\n", pChunk->GetLabel());
	StoreByteString(pChunk, str);

	m_sequenceStrings.Add(str);
}

void CChunkRenderText::StoreSampleListChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;

	// Store sample list
	str.Format("%s:\n", pChunk->GetLabel());

	for (int i = 0; i < pChunk->GetLength(); i += 3) {
		str.AppendFormat("\t.byte %i, %i, %i\n", pChunk->GetData(i + 0), pChunk->GetData(i + 1), pChunk->GetData(i + 2));
	}

	m_sampleListStrings.Add(str);
}

void CChunkRenderText::StoreSamplePointersChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

	// Store sample pointer
	str.Format("%s:\n", pChunk->GetLabel());

	if (len > 0) {
		str.Append("\t.byte ");

		int len = pChunk->GetLength();
		for (int i = 0; i < len; ++i) {
			str.AppendFormat("%i%s", pChunk->GetData(i), (i < len - 1) && (i % 3 != 2) ? ", " : "");
			if (i % 3 == 2 && i < (len - 1))
				str.Append("\n\t.byte ");
		}
	}

	str.Append("\n");

	m_samplePointersStrings.Add(str);
}

void CChunkRenderText::StoreSongListChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;

	str.Format("%s: \n", pChunk->GetLabel());

	for (int i = 0; i < pChunk->GetLength(); ++i) {
		str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i));
	}

	m_songListStrings.Add(str);
}

void CChunkRenderText::StoreSongChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;

	str.Format("%s: \n", pChunk->GetLabel());

	for (int i = 0; i < pChunk->GetLength();) {
		str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i++));
		str.AppendFormat("\t.byte %i\t; frame count\n", pChunk->GetData(i++));
		str.AppendFormat("\t.byte %i\t; pattern length\n", pChunk->GetData(i++));
		str.AppendFormat("\t.byte %i\t; speed\n", pChunk->GetData(i++));
		str.AppendFormat("\t.byte %i\t; tempo\n", pChunk->GetData(i++));
		str.AppendFormat("\t.byte %i\t; initial bank\n", pChunk->GetData(i++)); //pChunk->GetBankRefName(i++));
	}

	str.Append("\n");

	m_songStrings.Add(str);
}

void CChunkRenderText::StoreFrameListChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;

	// Pointers to frames
	str.Format("%s: \n", pChunk->GetLabel());

	for (int i = 0; i < pChunk->GetLength(); ++i) {
		str.AppendFormat("\t.word %s\n", pChunk->GetDataRefName(i));
	}

	m_songDataStrings.Add(str);
}

void CChunkRenderText::StoreFrameChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

	// Frame list
	str.Format("%s:\n\t.word ", pChunk->GetLabel());

	for (int i = 0, j = 0; i < len; ++i) {
		if (pChunk->IsDataReference(i))
			str.AppendFormat("%s%s", (j++ > 0) ? _T(", ") : _T(""), pChunk->GetDataRefName(i));
	}

	// Bank values
	for (int i = 0, j = 0; i < len; ++i) {
		if (pChunk->IsDataBank(i)) {
			if (j == 0) {
				str.AppendFormat("\n\t.byte ", pChunk->GetLabel());
			}
			str.AppendFormat("%s$%02X", (j++ > 0) ? _T(", ") : _T(""), pChunk->GetData(i));
		}
	}

	str.Append("\n");

	m_songDataStrings.Add(str);
}

void CChunkRenderText::StorePatternChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

	// Patterns
	str.Format("%s: \n", pChunk->GetLabel());
	str.Append("\t.byte ");

	len = pChunk->GetStringLength(0);

	for (int i = 0; i < len; ++i) {
		str.AppendFormat("$%02X", pChunk->GetStringData(0, i));
		if ((i % 20 == 19) && (i < len - 1))
			str.Append("\n\t.byte ");
		else {
			if (i < len - 1)
				str.Append(", ");
		}
	}

	str.Append("\n");

	m_songDataStrings.Add(str);
}

void CChunkRenderText::StoreWavetableChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

	// FDS waves
	str.Format("%s: \n", pChunk->GetLabel());
	str.Append("\t.byte ");

	for (int i = 0; i < len; ++i) {
		str.AppendFormat("$%02X", pChunk->GetData(i));
		if ((i % 64 == 63) && (i < len - 1))
			str.Append("\n\t.byte ");
		else {
			if (i < len - 1)
				str.Append(", ");
		}
	}

	str.Append("\n");

	m_wavetableStrings.Add(str);
}

void CChunkRenderText::StoreWavesChunk(CChunk *pChunk, CFile *pFile)
{
	CStringA str;
	int len = pChunk->GetLength();

//				int waves = pChunk->GetData(0);
	int wave_len = 16;//(len - 1) / waves;

	// Namco waves
	str.Format("%s: \n", pChunk->GetLabel());
//				str.AppendFormat("\t.byte %i\n", waves);
	
	str.Append("\t.byte ");

	for (int i = 0; i < len; ++i) {
		str.AppendFormat("$%02X", pChunk->GetData(i));
		if ((i % wave_len == (wave_len - 1)) && (i < len - 1))
			str.Append("\n\t.byte ");
		else {
			if (i < len - 1)
				str.Append(", ");
		}
	}

	str.Append("\n");

	m_wavesStrings.Add(str);
}

void CChunkRenderText::WriteFileString(CFile *pFile, const CStringA &str) const
{
	pFile->Write(const_cast<CStringA&>(str).GetBuffer(), str.GetLength());
}

void CChunkRenderText::PrintList(CChunk *pChunk, CStringA &str) const
{
	int len = pChunk->GetLength();
	for (int i = 0; i < len; ++i) {
		str.AppendFormat("%i%s", pChunk->GetData(i), (i < len - 1) ? ", " : "");
	}
}

void CChunkRenderText::StoreByteString(CChunk *pChunk, CStringA &str) const
{
	int len = pChunk->GetLength();
	
	str.Append("\t.byte ");

	for (int i = 0; i < len; ++i) {
		str.AppendFormat("$%02X", pChunk->GetData(i));

		if ((i % 20 == 19) && (i < len - 1))
			str.Append("\n\t.byte ");
		else if (i < len - 1)
			str.Append(", ");
	}

	str.Append("\n");
}

/**
 * Binray chunk render
 *
 */

void CChunkRenderBinary::StoreChunk(CChunk *pChunk, CFile *pFile)
{
	for (int i = 0; i < pChunk->GetLength(); ++i) {
		if (pChunk->GetType() == CHUNK_PATTERN) {
			for (unsigned int j = 0; j < pChunk->GetStringLength(0); ++j) {
				unsigned char data = pChunk->GetStringData(0, j);
				pFile->Write(&data, 1);
			}
		}
		else {
			unsigned short data = pChunk->GetData(i);
			unsigned short size = pChunk->GetDataSize(i);
			pFile->Write(&data, size);
		}
	}
}
