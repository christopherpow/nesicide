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
#include "chunk.h"

/**
 * CChunk - Stores NSF data
 *
 */

CChunk::~CChunk()
{
	for (unsigned int i = 0; i < m_vChunkData.size(); ++i) {
		delete m_vChunkData[i];
	}
}

void CChunk::Clear()
{
	for (unsigned int i = 0; i < m_vChunkData.size(); ++i) {
		delete m_vChunkData[i];
	}

	m_vChunkData.clear();
}

void CChunk::StoreByte(unsigned char data)
{
	m_vChunkData.push_back(new CChunkDataByte(CHUNK_DATA_BYTE, data));
}

void CChunk::StoreWord(unsigned short data)
{
	m_vChunkData.push_back(new CChunkDataWord(CHUNK_DATA_WORD, data));
}

void CChunk::StoreReference(CString refName)
{
	m_vChunkData.push_back(new CChunkDataReference(CHUNK_DATA_REFERENCE, refName));
}

void CChunk::StoreString(char *pString, int len)
{
	m_vChunkData.push_back(new CChunkDataString(CHUNK_DATA_STRING, pString, len));
}

void CChunk::ChangeByte(int index, unsigned char data)
{
	ASSERT(index < (int)m_vChunkData.size());
	ASSERT(m_vChunkData[index]->GetType() == CHUNK_DATA_BYTE);
	dynamic_cast<CChunkDataByte*>(m_vChunkData[index])->m_data = data;
}

int CChunk::GetType() const
{
	return m_iType;
}

int CChunk::GetLength() const
{
	return m_vChunkData.size();
}

unsigned int CChunk::CountData() const
{
	int Size = 0;

	for (unsigned int i = 0; i < m_vChunkData.size(); ++i) {
		switch (m_vChunkData[i]->GetType()) {
			case CHUNK_DATA_BYTE:
				Size += 1;
				break;
			case CHUNK_DATA_WORD:
			case CHUNK_DATA_REFERENCE:
				Size += 2;
				break;
			case CHUNK_DATA_STRING:
				Size += ((CChunkDataString*)m_vChunkData[i])->m_iLen;
				break;
		}
	}

	return Size;
}

void CChunk::AssignLabels(CMap<CString, LPCSTR, int, int> &labelMap)
{
	for (unsigned int i = 0; i < m_vChunkData.size(); ++i) {
		if (m_vChunkData[i]->GetType() == CHUNK_DATA_REFERENCE) {
			CChunkDataReference *pChunkData = dynamic_cast<CChunkDataReference*>(m_vChunkData[i]);
			pChunkData->ref = labelMap[pChunkData->m_refName];
		}
	}
}

unsigned short CChunk::GetData(int index) const
{
	CChunkData *pData = m_vChunkData[index];
	switch (pData->GetType()) {
		case CHUNK_DATA_BYTE:
			return ((CChunkDataByte*)pData)->m_data;
		case CHUNK_DATA_WORD:
			return ((CChunkDataWord*)pData)->m_data;
		case CHUNK_DATA_REFERENCE:
			return ((CChunkDataReference*)pData)->ref;
	}
	return 0;
}

int CChunk::GetDataType(int index) const
{
	return m_vChunkData[index]->GetType();
}

unsigned short CChunk::GetDataSize(int index) const
{
	CChunkData *pData = m_vChunkData[index];
	switch (pData->GetType()) {
		case CHUNK_DATA_BYTE:
			return 1;
		case CHUNK_DATA_WORD:
		case CHUNK_DATA_REFERENCE:
			return 2;
		case CHUNK_DATA_STRING:
			return ((CChunkDataString*)pData)->m_iLen;
	}
	return 0;
}

unsigned char CChunk::GetStringData(int index, int pos) const
{
	return ((CChunkDataString*)m_vChunkData[index])->m_str[pos];
}

int CChunk::GetStringLength(int index) const 
{
	return ((CChunkDataString*)m_vChunkData[index])->m_iLen;
}

unsigned char *CChunk::GetStringData(int index) const
{
	return (unsigned char*)((CChunkDataString*)m_vChunkData[index])->m_str;
}

CString CChunk::GetDataRefName(int index) const
{
	CChunkData *pData = m_vChunkData[index];
	
	switch (pData->GetType()) {
		case CHUNK_DATA_REFERENCE:
			return ((CChunkDataReference*)pData)->m_refName;
	}

	return "";
}

void CChunk::UpdateDataRefName(int index, CString name)
{
	CChunkData *pData = m_vChunkData[index];
	
	switch (pData->GetType()) {
		case CHUNK_DATA_REFERENCE:
			((CChunkDataReference*)pData)->m_refName = name;
	}
}

void CChunk::SetLabel(CString name)
{
	m_strLabel = name;
}

CString CChunk::GetLabel() const
{
	return m_strLabel;
}

void CChunk::SetBank(int Bank)
{
	m_iBank = Bank;
}

unsigned int CChunk::GetBank() const
{
	return m_iBank;
}
