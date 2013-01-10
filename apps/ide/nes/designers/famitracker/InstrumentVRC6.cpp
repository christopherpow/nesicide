/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2010  Jonathan Liss
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

#include "instrument.h"
#include "compiler.h"

/*
 * class CInstrumentVRC6
 *
 */

const int CInstrumentVRC6::SEQUENCE_TYPES[] = {SEQ_VOLUME, SEQ_ARPEGGIO, SEQ_PITCH, SEQ_HIPITCH, SEQ_DUTYCYCLE};

CInstrumentVRC6::CInstrumentVRC6()
{
	for (int i = 0; i < SEQUENCE_COUNT; i++) {
		m_iSeqEnable[i] = 0;
		m_iSeqIndex[i] = 0;
	}
}

CInstrument *CInstrumentVRC6::Clone()
{
	CInstrumentVRC6 *pNew = new CInstrumentVRC6();

	for (int i = 0; i < SEQUENCE_COUNT; i++) {
		pNew->SetSeqEnable(i, GetSeqEnable(i));
		pNew->SetSeqIndex(i, GetSeqIndex(i));
	}

	pNew->SetName(GetName());

	return pNew;
}

//void CInstrumentVRC6::Store(CDocumentFile *pDocFile)
//{
//	pDocFile->WriteBlockInt(SEQUENCE_COUNT);

//	for (int i = 0; i < SEQUENCE_COUNT; i++) {
//		pDocFile->WriteBlockChar(GetSeqEnable(i));
//		pDocFile->WriteBlockChar(GetSeqIndex(i));
//	}
//}

//bool CInstrumentVRC6::Load(CDocumentFile *pDocFile)
//{
//	int i, Index;
//	int SeqCnt = pDocFile->GetBlockInt();

//	ASSERT_FILE_DATA(SeqCnt < (SEQUENCE_COUNT + 1));

//	SeqCnt = SEQUENCE_COUNT;//SEQ_COUNT;

//	for (i = 0; i < SeqCnt; i++) {
//		SetSeqEnable(i, pDocFile->GetBlockChar());
//		Index = pDocFile->GetBlockChar();
//		ASSERT_FILE_DATA(Index < MAX_SEQUENCES);
//		SetSeqIndex(i, Index);
//	}

//	return true;
//}

//void CInstrumentVRC6::SaveFile(CFile *pFile, CFamiTrackerDoc *pDoc)
//{
////	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)theApp.GetFirstDocument();

//	// Sequences
//	unsigned char SeqCount = SEQUENCE_COUNT;
//	pFile->Write(&SeqCount, sizeof(char));

//	for (int i = 0; i < SEQUENCE_COUNT; ++i) {
//		int Sequence = GetSeqIndex(i);
//		if (GetSeqEnable(i)) {
//			CSequence *pSeq = pDoc->GetSequence(SNDCHIP_VRC6, Sequence, i);

//			char Enabled = 1;
//			int ItemCount = pSeq->GetItemCount();
//			int LoopPoint = pSeq->GetLoopPoint();
//			int ReleasePoint = pSeq->GetReleasePoint();
//			int Setting	= pSeq->GetSetting();

//			pFile->Write(&Enabled, sizeof(char));
//			pFile->Write(&ItemCount, sizeof(int));
//			pFile->Write(&LoopPoint, sizeof(int));
//			pFile->Write(&ReleasePoint, sizeof(int));
//			pFile->Write(&Setting, sizeof(int));

//			for (int j = 0; j < ItemCount; ++j) {
//				signed char Value = pSeq->GetItem(j);
//				pFile->Write(&Value, sizeof(char));
//			}
//		}
//		else {
//			char Enabled = 0;
//			pFile->Write(&Enabled, sizeof(char));
//		}
//	}
//}

//bool CInstrumentVRC6::LoadFile(CFile *pFile, int iVersion, CFamiTrackerDoc *pDoc)
//{
//	// Sequences
//	stSequence OldSequence;
//	unsigned char SeqCount;
//	unsigned char Enabled;
//	int Count, Index;
//	int LoopPoint, ReleasePoint, Setting;

//	pFile->Read(&SeqCount, sizeof(char));

//	// Loop through all instrument effects
//	for (int i = 0; i < SeqCount; ++i) {
//		pFile->Read(&Enabled, sizeof(char));
//		if (Enabled == 1) {
//			// Read the sequence

//			pFile->Read(&Count, sizeof(int));
//			Index = pDoc->GetFreeSequenceVRC6(i);

//			CSequence *pSeq = pDoc->GetSequence(SNDCHIP_VRC6, Index, i);

//			if (iVersion < 20) {
//				OldSequence.Count = Count;
//				for (int j = 0; j < Count; ++j) {
//					pFile->Read(&OldSequence.Length[j], sizeof(char));
//					pFile->Read(&OldSequence.Value[j], sizeof(char));
//				}
//				pDoc->ConvertSequence(&OldSequence, pSeq, i);	// convert
//			}
//			else {
//				pSeq->SetItemCount(Count);
//				pFile->Read(&LoopPoint, sizeof(int));
//				pSeq->SetLoopPoint(LoopPoint);
//				if (iVersion > 20) {
//					pFile->Read(&ReleasePoint, sizeof(int));
//					pSeq->SetReleasePoint(ReleasePoint);
//				}
//				if (iVersion >= 22) {
//					pFile->Read(&Setting, sizeof(int));
//					pSeq->SetSetting(Setting);
//				}
//				for (int j = 0; j < Count; ++j) {
//					char Val;
//					pFile->Read(&Val, sizeof(char));
//					pSeq->SetItem(j, Val);
//				}
//			}
//			SetSeqEnable(i, true);
//			SetSeqIndex(i, Index);
//		}
//		else {
//			SetSeqEnable(i, false);
//			SetSeqIndex(i, 0);
//		}
//	}

//	return true;
//}

int CInstrumentVRC6::CompileSize(CCompiler *pCompiler)
{
	int Size = 1;

   qDebug("::CompileSize");
//	CFamiTrackerDoc *pDoc = pCompiler->GetDocument();

//	for (int i = 0; i < SEQUENCE_COUNT; ++i)
//		if (GetSeqEnable(i) && pDoc->GetSequence(SNDCHIP_VRC6, GetSeqIndex(i), i)->GetItemCount() > 0)
//			Size += 2;

	return Size;
}

int CInstrumentVRC6::Compile(CCompiler *pCompiler, int Index)
{
	int ModSwitch;
	int StoredBytes = 0;
	int iAddress;
	int i;

   qDebug("::Compile");
//	CFamiTrackerDoc *pDoc = pCompiler->GetDocument();

//	pCompiler->WriteLog("VRC6 {");
//	ModSwitch = 0;

//	for (i = 0; i < SEQUENCE_COUNT; i++) {
//		ModSwitch = (ModSwitch >> 1) | (GetSeqEnable(i) && (pDoc->GetSequence(SNDCHIP_VRC6, GetSeqIndex(i), i)->GetItemCount() > 0) ? 0x10 : 0);
//	}

//	pCompiler->StoreByte(ModSwitch);
//	pCompiler->WriteLog("%02X ", ModSwitch);
//	StoredBytes++;

//	for (i = 0; i < SEQUENCE_COUNT; i++) {
//		iAddress = (GetSeqEnable(i) == 0 || (pDoc->GetSequence(SNDCHIP_VRC6, GetSeqIndex(i), i)->GetItemCount() == 0)) ? 0 : pCompiler->GetSequenceAddressVRC6(GetSeqIndex(i), i);
//		if (iAddress > 0) {
//			pCompiler->StoreShort(iAddress);
//			pCompiler->WriteLog("%04X ", iAddress);
//			StoredBytes += 2;
//		}
//	}

	return StoredBytes;
}

bool CInstrumentVRC6::CanRelease() const
{
   qDebug("::CanRelease");
//	if (GetSeqEnable(0) != 0) {
//		int index = GetSeqIndex(SEQ_VOLUME);
//		return ((CFamiTrackerDoc*) theApp.GetActiveDocument())->GetSequence(SNDCHIP_VRC6, index, SEQ_VOLUME)->GetReleasePoint() != -1;
//	}

	return false;
}

int	CInstrumentVRC6::GetSeqEnable(int Index) const
{
	return m_iSeqEnable[Index];
}

int	CInstrumentVRC6::GetSeqIndex(int Index) const
{
	return m_iSeqIndex[Index];
}

void CInstrumentVRC6::SetSeqEnable(int Index, int Value)
{
	m_iSeqEnable[Index] = Value;
}

void CInstrumentVRC6::SetSeqIndex(int Index, int Value)
{
	m_iSeqIndex[Index] = Value;
}
