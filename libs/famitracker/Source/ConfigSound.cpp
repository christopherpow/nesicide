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

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "ConfigSound.h"
#include "SoundGen.h"
#include "Settings.h"
#include "DirectSound.h"

// CConfigSound dialog

IMPLEMENT_DYNAMIC(CConfigSound, CPropertyPage)
CConfigSound::CConfigSound()
	: CPropertyPage(CConfigSound::IDD)
{
}

CConfigSound::~CConfigSound()
{
}

void CConfigSound::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CConfigSound, CPropertyPage)
//	ON_WM_HSCROLL()
//	ON_CBN_SELCHANGE(IDC_SAMPLE_RATE, OnCbnSelchangeSampleRate)
//	ON_CBN_SELCHANGE(IDC_SAMPLE_SIZE, OnCbnSelchangeSampleSize)
//	ON_CBN_SELCHANGE(IDC_DEVICES, OnCbnSelchangeDevices)
//END_MESSAGE_MAP()

//int iDeviceIndex;

const int MAX_BUFFER_LEN = 500;	// 500 ms

// CConfigSound message handlers

BOOL CConfigSound::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString Text;

	CComboBox	*pSampleRate, *pSampleSize, *pDevices;
	CSliderCtrl	*pBufSlider, *pBassSlider, *pTrebleSliderFreq, *pTrebleSliderDamping, *pVolumeSlider;

	pSampleRate	= (CComboBox*)GetDlgItem(IDC_SAMPLE_RATE);
	pSampleSize	= (CComboBox*)GetDlgItem(IDC_SAMPLE_SIZE);
	pDevices	= (CComboBox*)GetDlgItem(IDC_DEVICES);
	pBufSlider	= (CSliderCtrl*)GetDlgItem(IDC_BUF_LENGTH);

	pBassSlider			 = (CSliderCtrl*)GetDlgItem(IDC_BASS_FREQ);
	pTrebleSliderFreq	 = (CSliderCtrl*)GetDlgItem(IDC_TREBLE_FREQ);
	pTrebleSliderDamping = (CSliderCtrl*)GetDlgItem(IDC_TREBLE_DAMP);
	pVolumeSlider		 = (CSliderCtrl*)GetDlgItem(IDC_VOLUME);


	pBufSlider->SetRange(1, MAX_BUFFER_LEN);

	switch (theApp.GetSettings()->Sound.iSampleRate) {
		case 11025: pSampleRate->SelectString(0, _T("11 025 Hz")); break;
		case 22050: pSampleRate->SelectString(0, _T("22 050 Hz")); break;
		case 44100: pSampleRate->SelectString(0, _T("44 100 Hz")); break;
		case 48000: pSampleRate->SelectString(0, _T("48 000 Hz")); break;
		case 96000: pSampleRate->SelectString(0, _T("96 000 Hz")); break;
	}

	switch (theApp.GetSettings()->Sound.iSampleSize) {
		case 16: pSampleSize->SelectString(0, _T("16 bit")); break;
		case 8:	 pSampleSize->SelectString(0, _T("8 bit")); break;
	}

	pBufSlider->SetPos(theApp.GetSettings()->Sound.iBufferLength);

	Text.Format(_T("%i ms"), theApp.GetSettings()->Sound.iBufferLength);
	SetDlgItemText(IDC_BUF_LEN, Text);

	pBassSlider->SetRange(16, 4000);
	pTrebleSliderFreq->SetRange(20, 20000);
	pTrebleSliderDamping->SetRange(0, 90);
	pVolumeSlider->SetRange(0, 100);

	pBassSlider->SetPos(theApp.GetSettings()->Sound.iBassFilter);
	pTrebleSliderFreq->SetPos(theApp.GetSettings()->Sound.iTrebleFilter);
	pTrebleSliderDamping->SetPos(theApp.GetSettings()->Sound.iTrebleDamping);
	pVolumeSlider->SetPos(theApp.GetSettings()->Sound.iMixVolume);

	Text.Format(_T("%i Hz"), pBassSlider->GetPos());
	SetDlgItemText(IDC_BASS_FREQ_T, Text);

	Text.Format(_T("%i Hz"), pTrebleSliderFreq->GetPos());
	SetDlgItemText(IDC_TREBLE_FREQ_T, Text);

	Text.Format(_T("-%i dB"), pTrebleSliderDamping->GetPos());
	SetDlgItemText(IDC_TREBLE_DAMP_T, Text);

	Text.Format(_T("%i %%"), pVolumeSlider->GetPos());
	SetDlgItemText(IDC_VOLUME_T, Text);

	CDSound *pDSound = theApp.GetSoundGenerator()->GetSoundInterface();
	int iCount = pDSound->GetDeviceCount();

	for (int i = 0; i < iCount; ++i)
		pDevices->AddString(pDSound->GetDeviceName(i));

	pDevices->SetCurSel(theApp.GetSettings()->Sound.iDevice);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigSound::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString Text;

	Text.Format(_T("%i ms"), ((CSliderCtrl*)GetDlgItem(IDC_BUF_LENGTH))->GetPos());
	SetDlgItemText(IDC_BUF_LEN, Text);

	Text.Format(_T("%i Hz"), ((CSliderCtrl*)GetDlgItem(IDC_BASS_FREQ))->GetPos());
	SetDlgItemText(IDC_BASS_FREQ_T, Text);

	Text.Format(_T("%i Hz"), ((CSliderCtrl*)GetDlgItem(IDC_TREBLE_FREQ))->GetPos());
	SetDlgItemText(IDC_TREBLE_FREQ_T, Text);

	Text.Format(_T("-%i dB"), ((CSliderCtrl*)GetDlgItem(IDC_TREBLE_DAMP))->GetPos());
	SetDlgItemText(IDC_TREBLE_DAMP_T, Text);

	Text.Format(_T("%i %%"), ((CSliderCtrl*)GetDlgItem(IDC_VOLUME))->GetPos());
	SetDlgItemText(IDC_VOLUME_T, Text);

	SetModified();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CConfigSound::OnApply()
{
	CComboBox	*pSampleRate, *pSampleSize, *pDevices;
	CSliderCtrl	*pBufSlider;

	pDevices = (CComboBox*)GetDlgItem(IDC_DEVICES);
	pSampleRate = (CComboBox*)GetDlgItem(IDC_SAMPLE_RATE);
	pSampleSize = (CComboBox*)GetDlgItem(IDC_SAMPLE_SIZE);
	pBufSlider = (CSliderCtrl*)GetDlgItem(IDC_BUF_LENGTH);

	switch (pSampleRate->GetCurSel()) {
		case 0: theApp.GetSettings()->Sound.iSampleRate = 11025; break;
		case 1: theApp.GetSettings()->Sound.iSampleRate = 22050; break;
		case 2: theApp.GetSettings()->Sound.iSampleRate = 44100; break;
		case 3: theApp.GetSettings()->Sound.iSampleRate = 48000; break;
		case 4: theApp.GetSettings()->Sound.iSampleRate = 96000; break;
	}

	switch (pSampleSize->GetCurSel()) {
		case 0: theApp.GetSettings()->Sound.iSampleSize = 16; break;
		case 1: theApp.GetSettings()->Sound.iSampleSize = 8; break;
	}

	theApp.GetSettings()->Sound.iBufferLength = pBufSlider->GetPos();

	theApp.GetSettings()->Sound.iBassFilter		= ((CSliderCtrl*)GetDlgItem(IDC_BASS_FREQ))->GetPos();
	theApp.GetSettings()->Sound.iTrebleFilter	= ((CSliderCtrl*)GetDlgItem(IDC_TREBLE_FREQ))->GetPos();
	theApp.GetSettings()->Sound.iTrebleDamping	= ((CSliderCtrl*)GetDlgItem(IDC_TREBLE_DAMP))->GetPos();
	theApp.GetSettings()->Sound.iMixVolume		= ((CSliderCtrl*)GetDlgItem(IDC_VOLUME))->GetPos();

	theApp.GetSettings()->Sound.iDevice	= pDevices->GetCurSel();

	theApp.LoadSoundConfig();

	return CPropertyPage::OnApply();
}

void CConfigSound::OnCbnSelchangeSampleRate()
{
	SetModified();
}

void CConfigSound::OnCbnSelchangeSampleSize()
{
	SetModified();
}

void CConfigSound::OnCbnSelchangeDevices()
{
	SetModified();
}
