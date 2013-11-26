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

// Vector.h is needed. This file cannot be included after stdafx.h

// Helper classes/objects for NSF compiling

enum CHUNK_DATA_TYPE { CHUNK_DATA_BYTE, CHUNK_DATA_WORD, CHUNK_DATA_REFERENCE, CHUNK_DATA_BANK, CHUNK_DATA_STRING };

//
// Chunk data classes
//

class CChunkData
{
public:
	CChunkData(int Type) : m_iType(Type) {};
	virtual ~CChunkData() {};
	int GetType() const { return m_iType; }
private:
	int m_iType;
};

class CChunkDataByte : public CChunkData
{
public:
	CChunkDataByte(int Type, unsigned char data) : CChunkData(Type), m_data(data) {};
	unsigned char m_data;
};

class CChunkDataWord : public CChunkData
{
public:
	CChunkDataWord(int Type, unsigned short data) : CChunkData(Type), m_data(data) {};
	unsigned short m_data;
};

class CChunkDataReference : public CChunkData
{
public:
	CChunkDataReference(int Type, CString refName) : CChunkData(Type), m_refName(refName), ref(-1) {};
	CString m_refName;
	unsigned short ref;
};

class CChunkDataBank : public CChunkData
{
public:
	CChunkDataBank(int Type, CString bankOf, int bank) : CChunkData(Type), m_bankOf(bankOf), m_bank(bank) {};
	CString m_bankOf;
	unsigned int m_bank;
};

class CChunkDataString : public CChunkData
{
public:
	CChunkDataString(int Type, char *str, int len) : CChunkData(Type) {
		m_str = new char[len];
		memcpy(m_str, str, len);
		m_iLen = len;
	};
	~CChunkDataString() {
		delete [] m_str;
	};
	char *m_str;
	int m_iLen;
};

enum CHUNK_TYPES { 
	CHUNK_HEADER,
	CHUNK_SEQUENCE, 
	CHUNK_INSTRUMENT_LIST, 
	CHUNK_INSTRUMENT, 
	CHUNK_SAMPLE_LIST, 
	CHUNK_SAMPLE_POINTERS,
	CHUNK_SONG_LIST,
	CHUNK_SONG,
	CHUNK_FRAME_LIST,
	CHUNK_FRAME,
	CHUNK_PATTERN,
	CHUNK_WAVETABLE,
	CHUNK_WAVES
};

//
// Chunk class
//

class CChunk
{
public:
	CChunk(int Type, CString label) : m_iType(Type), m_strLabel(label) {};
	~CChunk();

	void			Clear();
	
	void			StoreByte(unsigned char data);
	void			StoreWord(unsigned short data);
	void			StoreReference(CString refName);
	void			StoreBankReference(CString refName, int bank);
	void			StoreString(char *pString, int len);

	void			ChangeByte(int index, unsigned char data);
	void			SetupBankData(int index, unsigned char bank);

	int				GetLength() const;
	unsigned short	GetData(int index) const;
	unsigned short	GetDataSize(int index) const;
	unsigned char	GetStringData(int index, int pos) const;
	int				GetDataType(int index) const;
	CString			GetDataRefName(int index) const;

	unsigned char	*GetStringData(int index) const;

	int				GetStringLength(int index) const;

	int				GetType() const;

	void			SetLabel(CString str);
	CString			GetLabel() const;

	unsigned int	CountData() const;
	void			AssignLabels(CMap<CString, LPCSTR, int, int> &labelMap);

	void			SetBank(int Bank);
	unsigned int	GetBank() const;

	void			UpdateDataRefName(int index, CString name);
	CString			GetBankRefName(int index) const;

public:
	// Extra
	int				m_iInstrumentType;

private:
	std::vector<CChunkData*> m_vChunkData;

	int				m_iType;
	CString			m_strLabel;

	int				m_iBank;

};
