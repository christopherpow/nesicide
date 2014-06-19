/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2013  Jonathan Liss
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

#include "stdafx.h"
#include "FamiTrackerDoc.h"

/*
 * CDSample
 *
 * This class is used to store delta encoded samples (DPCM).
 *
 */

CDSample::CDSample() 
	: SampleSize(0), SampleData(NULL) 
{
	memset(Name, 0, 256);
}

CDSample::CDSample(int Size, char *pData) 
	: SampleSize(Size), SampleData(pData)
{
	if (SampleData == NULL)
		SampleData = new char[Size];

	memset(Name, 0, 256);
}

CDSample::CDSample(CDSample &sample) 
	: SampleSize(sample.SampleSize), SampleData(new char[sample.SampleSize])
{
	// Should never be empty
	ASSERT(sample.SampleSize != 0);
	memcpy(SampleData, sample.SampleData, SampleSize);
	strncpy(Name, sample.Name, 256);
}

CDSample::~CDSample()
{
	SAFE_RELEASE_ARRAY(SampleData);
}

void CDSample::Copy(const CDSample *pDSample) 
{
	SAFE_RELEASE_ARRAY(SampleData);

	SampleSize = pDSample->SampleSize;
	SampleData = new char[SampleSize];

	memcpy(SampleData, pDSample->SampleData, SampleSize);
	strcpy(Name, pDSample->Name);
}

void CDSample::Allocate(int iSize, char *pData)
{
	SAFE_RELEASE_ARRAY(SampleData);

	SampleData = new char[iSize];
	SampleSize = iSize;

	if (pData != NULL)
		memcpy(SampleData, pData, iSize);
}

void CDSample::Clear()
{
	SAFE_RELEASE_ARRAY(SampleData);
	SampleSize = 0;
}

void CDSample::SetData(int Size, char *pData)
{
	SampleData = pData;
	SampleSize = Size;
}

int CDSample::GetSize() const
{
	return SampleSize;
}

char *CDSample::GetData() const
{
	return SampleData;
}
