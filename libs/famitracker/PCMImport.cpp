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

#include <numeric>
#include <algorithm>
#include <cmath>
#include "resampler/resample.hpp"
#include "stdafx.h"
#include <mmsystem.h>
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "PCMImport.h"
#include "Settings.h"
#include "SoundGen.h"
#include "apu/apu.h"
#include "apu/dpcm.h"
#include "resampler/resample.inl"


const int CPCMImport::MAX_QUALITY = 15;
const int CPCMImport::MIN_QUALITY = 0;

const int CPCMImport::SAMPLES_MAX = 0x0FF1;		// Max amount of 8-bit DPCM samples

const int CPCMImport::VOLUME_RANGE = 12;		// +/- dB

LPCTSTR CPCMImport::QUALITY_FORMAT = _T("Quality: %i");
LPCTSTR CPCMImport::GAIN_FORMAT	   = _T("Gain: %+.0f dB");

// Implement a resampler using CRTP idiom
class resampler : public jarh::resample<resampler>
{
    typedef jarh::resample<resampler> base;
public:
    resampler(const jarh::sinc &sinc, float ratio, int channels, int smpsize,
              size_t nbsamples, CFile &cfile)
    // TODO: cutoff is currently fixed to a value (.9f), make it modifiable.
     : base(sinc), channels_(channels), smpsize_(smpsize),
       nbsamples_(nbsamples), remain_(nbsamples), cfile_(cfile)
    {
        init(ratio, .9f);
    }

    bool initstream()
    {
        // Don't seek to the begin of wave chunk, as it is already done.
        // This stream will not be reinitialized, then.
        remain_ = nbsamples_;
        return true;
    }

    float *fill(float *first, float *end)
    {
        int val;
        for(;first != end && remain_ && ReadSample(val); ++first, --remain_)
        {
            *first = (float)val;
        }
        return first;
    }

private:
    bool ReadSample(int &v)
    {
        int ret, nbytes;
        if (smpsize_ == 2) {
            // 16 bit samples
            short sample_word[2];
            if (channels_ == 2) {
                ret = cfile_.Read(sample_word, nbytes = 2*sizeof(short));
                v = (sample_word[0] + sample_word[1]) / 2;
            }
            else {
                ret = cfile_.Read(sample_word, nbytes = sizeof(short));
                v = *sample_word;
            }
        }
        else if (smpsize_ == 1) {
            // 8 bit samples
            unsigned char sample_byte[2];
            if (channels_ == 2) {
                ret = cfile_.Read(sample_byte, nbytes = 2);
                // convert to a proper signed representation
                // shift left only by 7; because we want a mean
                v = ((int)sample_byte[0] + (int)sample_byte[1] - 256) << 7;
            }
            else {
                ret = cfile_.Read(sample_byte, nbytes = 1);
				v = ((int)(*sample_byte) - 128) << 8;
            }
        }
		else if (smpsize_ == 3) {
			// 24 bit samples
	        unsigned char sample_byte[6];
            if (channels_ == 2) {
				ret = cfile_.Read(sample_byte, nbytes = 6);
				v = (*((signed short*)(sample_byte + 1)) + *((signed short*)(sample_byte + 4))) / 2;
			}
			else {
				ret = cfile_.Read(sample_byte, nbytes = 3);
				v = *((signed short*)(sample_byte + 1));
			}
		}
		else if (smpsize_ == 4) {
			// 32 bit samples
	        int sample_word[2];
            if (channels_ == 2) {
				ret = cfile_.Read(sample_word, nbytes = 8);
				v = ((sample_word[0] >> 16) + (sample_word[1] >> 16)) / 2;
			}
			else {
				ret = cfile_.Read(sample_word, nbytes = 4);
				v = sample_word[0] >> 16;
			}
		}

        return ret == nbytes;
    }

    CFile &cfile_;
    int    channels_;
    int    smpsize_;
    size_t nbsamples_;
    size_t remain_;
};

// Derive a new class from CFileDialog with implemented preview of audio files

class CFileSoundDialog : public CFileDialog
{
public:
	CFileSoundDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL, DWORD dwSize = 0);
	~CFileSoundDialog();

protected:
	virtual void OnFileNameChange();
};

//	CFileSoundDialog

CFileSoundDialog::CFileSoundDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize)
	: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
}

CFileSoundDialog::~CFileSoundDialog()
{
	// Stop any possible playing sound
	PlaySound(NULL, NULL, SND_NODEFAULT | SND_SYNC);
}

void CFileSoundDialog::OnFileNameChange()
{
	// Preview wave file

	if (!GetFileExt().CompareNoCase(_T("wav")) && theApp.GetSettings()->General.bWavePreview)
		PlaySound(GetPathName(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC | SND_NOWAIT);

	CFileDialog::OnFileNameChange();
}

// CPCMImport dialog

IMPLEMENT_DYNAMIC(CPCMImport, CDialog)
CPCMImport::CPCMImport(CWnd* pParent /*=NULL*/)
	: CDialog(CPCMImport::IDD, pParent),
	m_psinc(new jarh::sinc(512, 32)) // sinc object. TODO: parametrise
{
}

CPCMImport::~CPCMImport()
{
	SAFE_RELEASE(m_psinc);
}

void CPCMImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPCMImport, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_PREVIEW, &CPCMImport::OnBnClickedPreview)
END_MESSAGE_MAP()

CDSample *CPCMImport::ShowDialog()
{
	// Return import parameters, 0 if cancel

	CFileSoundDialog OpenFileDialog(TRUE, 0, 0, OFN_HIDEREADONLY, _T("Wave files (*.wav)|*.wav|All files (*.*)|*.*||"));

	OpenFileDialog.m_pOFN->lpstrInitialDir = theApp.GetSettings()->GetPath(PATH_WAV);

	if (OpenFileDialog.DoModal() == IDCANCEL)
		return NULL;

	theApp.GetSettings()->SetPath(OpenFileDialog.GetPathName(), PATH_WAV);

	m_strPath	  = OpenFileDialog.GetPathName();
	m_strFileName = OpenFileDialog.GetFileName();
	m_pImported	  = NULL;

	// Open file and read header
	if (!OpenWaveFile())
		return NULL;

	CDialog::DoModal();

	// Close file
	m_fSampleFile.Close();

	return m_pImported;
}

// CPCMImport message handlers

BOOL CPCMImport::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl *pQualitySlider = (CSliderCtrl*)GetDlgItem(IDC_QUALITY);
	CSliderCtrl *pVolumeSlider = (CSliderCtrl*)GetDlgItem(IDC_VOLUME);
	CString Text;

	// Initial volume & quality
	m_iQuality = MAX_QUALITY;
	m_iVolume = 0;			// 0dB

	pQualitySlider->SetRange(MIN_QUALITY, MAX_QUALITY);
	pQualitySlider->SetPos(m_iQuality);

	pVolumeSlider->SetRange(0, VOLUME_RANGE * 2);
	pVolumeSlider->SetPos(m_iVolume + VOLUME_RANGE);
	pVolumeSlider->SetTicFreq(3);	// 3dB/tick

	Text.Format(QUALITY_FORMAT, m_iQuality);
	SetDlgItemText(IDC_QUALITY_FRM, Text);

	Text.Format(GAIN_FORMAT, float(m_iVolume));
	SetDlgItemText(IDC_VOLUME_FRM, Text);

	UpdateFileInfo();

	CString WinTitle;
	WinTitle.Format(_T("PCM Import - [%s]"), m_strFileName);
	SetWindowText(WinTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPCMImport::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString Text;
	CSliderCtrl *pQualitySlider = (CSliderCtrl*)GetDlgItem(IDC_QUALITY);
	CSliderCtrl *pVolumeSlider = (CSliderCtrl*)GetDlgItem(IDC_VOLUME);

	m_iQuality = pQualitySlider->GetPos();
	m_iVolume = pVolumeSlider->GetPos() - VOLUME_RANGE;

	Text.Format(QUALITY_FORMAT, m_iQuality);
	SetDlgItemText(IDC_QUALITY_FRM, Text);

	Text.Format(GAIN_FORMAT, float(m_iVolume));
	SetDlgItemText(IDC_VOLUME_FRM, Text);

	UpdateFileInfo();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPCMImport::OnBnClickedCancel()
{
	m_iQuality = 0;
	m_iVolume = 0;
	m_pImported = NULL;
	OnCancel();
}

void CPCMImport::OnBnClickedOk()
{
	CDSample *pSample = ConvertFile();

	if (pSample == NULL)
		return;

	// remove .wav
	m_strFileName.Truncate(m_strFileName.GetLength() - 4);

	// Set the name
	strcpy_s(pSample->Name, 256, (char*)(LPCSTR)m_strFileName);

	m_pImported = pSample;

	OnOK();
}

void CPCMImport::OnBnClickedPreview()
{
	CDSample *pSample = ConvertFile();

	if (!pSample)
		return;

	// Play sample, it'll be removed automatically when finished
	theApp.GetSoundGenerator()->PreviewSample(pSample, 0, m_iQuality);
}

void CPCMImport::UpdateFileInfo()
{
	CString SampleRate;
	SampleRate.Format(_T("%i Hz, %i bits, %s"), m_iSamplesPerSec, m_iSampleSize * 8, (m_iChannels == 2) ? _T("Stereo") : _T("Mono"));
	SetDlgItemText(IDC_SAMPLE_RATE, SampleRate);

	float base_freq = (float)CAPU::BASE_FREQ_NTSC / (float)CDPCM::DMC_PERIODS_NTSC[m_iQuality];

	CString Resampling;
	Resampling.Format(_T("Target sample rate: %g Hz"), base_freq);
	SetDlgItemText(IDC_RESAMPLING, Resampling);
}

CDSample *CPCMImport::ConvertFile()
{
	// Converts a WAV file to a DPCM sample
	static const int DMC_BIAS = 32;

	unsigned char DeltaAcc = 0;	// DPCM sample accumulator
	int Sample = 0;				// PCM sample
	int Delta = DMC_BIAS;		// Delta counter
	int AccReady = 8;

	float resample_factor;

	float volume = powf(10, float(m_iVolume) / 20.0f);		// Convert dB to linear

	// Display wait cursor
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	// Seek to start of samples
	m_fSampleFile.Seek(m_ullSampleStart, CFile::begin);

	// Allocate space
	char *pSamples = new char[SAMPLES_MAX];
	int iSamples = 0;

	// Determine resampling factor
	float base_freq = (float)CAPU::BASE_FREQ_NTSC / (float)CDPCM::DMC_PERIODS_NTSC[m_iQuality];
	resample_factor = base_freq / (float)m_iSamplesPerSec;

    resampler resmpler(*m_psinc, resample_factor, m_iChannels, m_iSampleSize, m_iWaveSize, m_fSampleFile);
    float val;
	// Conversion
	while (resmpler.get(val) && (iSamples < SAMPLES_MAX)) {

		// when resampling we must clip because of possible ringing.
		static const int MAX_AMP =  (1 << 16) - 1;
		static const int MIN_AMP = -(1 << 16) + 1; // just being symetric
		val = (std::max<float>(std::min<float>(val, (float)MAX_AMP), (float)MIN_AMP));

        // Volume done this way so it acts as before
        Sample = (int)((val * volume) / 1024.f)
                  + DMC_BIAS;

		DeltaAcc >>= 1;

		// PCM -> DPCM
		if (Sample >= Delta) {
			++Delta;
			if (Delta > 63)
				Delta = 63;
			DeltaAcc |= 0x80;
		}
		else if (Sample < Delta) {
			--Delta;
			if (Delta < 0)
				Delta = 0;
		}

		if (--AccReady == 0) {
			// Store sample
			pSamples[iSamples++] = DeltaAcc;
			AccReady = 8;
		}
	}

    // TODO: error handling with th efile
    // if (!resmpler.eof())
    //      throw ?? or something else.

	// Adjust sample until size is x * $10 + 1 bytes
	while (iSamples < SAMPLES_MAX && ((iSamples & 0x0F) - 1) != 0)
		pSamples[iSamples++] = 0x55;

	// Return a sample object
	return new CDSample(iSamples, pSamples);
}

bool CPCMImport::OpenWaveFile()
{
	// Open and read wave file header
	PCMWAVEFORMAT WaveFormat;
	char Header[4];
	bool Scanning = true;
	int BlockSize;
	CFileException ex;

	if (!m_fSampleFile.Open(m_strPath, CFile::modeRead, &ex)) {
		TCHAR   szCause[255];
		CString strFormatted;
		ex.GetErrorMessage(szCause, 255);
		strFormatted = _T("Could not open file: ");
		strFormatted += szCause;
		AfxMessageBox(strFormatted);
		return false;
	}

	m_fSampleFile.Read(Header, 4);

	if (memcmp(Header, "RIFF", 4) != 0) {
		AfxMessageBox(_T("File is not a valid RIFF format!"));
		m_fSampleFile.Close();
		return false;
	}

	// Read sample size
	m_fSampleFile.Read(&m_iWaveSize, 4);

	// This is not perfect, but seems to work for the files I tried
	while (Scanning) {
		m_fSampleFile.Read(Header, 4);

		if (memcmp(Header, "WAVE", 4)) {
			m_fSampleFile.Read(&BlockSize, 4);

			if (!memcmp(Header, "fmt ", 4)) {
				// Read the wave-format
				int ReadSize = BlockSize;
				if (ReadSize > sizeof(PCMWAVEFORMAT))
					ReadSize = sizeof(PCMWAVEFORMAT);

				m_fSampleFile.Read(&WaveFormat, ReadSize);
				m_fSampleFile.Seek(BlockSize - ReadSize, CFile::current);

				if (WaveFormat.wf.wFormatTag != WAVE_FORMAT_PCM) {
					AfxMessageBox(IDS_INVALID_WAVEFILE, MB_ICONERROR);
					m_fSampleFile.Close();
					return false;
				}
			}
			else if (!memcmp(Header, "data", 4)) {
				m_iWaveSize = BlockSize;
				Scanning = false;
			}
			else if (!memcmp(Header, "fact", 4)) {
				m_fSampleFile.Seek(BlockSize, CFile::current);
			}
			else {
				Scanning = false;
			}
		}
	}

	m_ullSampleStart = m_fSampleFile.GetPosition();

	// Save file info
	m_iChannels		  = WaveFormat.wf.nChannels;
	m_iSampleSize	  = WaveFormat.wf.nBlockAlign / WaveFormat.wf.nChannels;
	m_iBlockAlign	  = WaveFormat.wf.nBlockAlign;
	m_iAvgBytesPerSec = WaveFormat.wf.nAvgBytesPerSec;
	m_iSamplesPerSec  = WaveFormat.wf.nSamplesPerSec;

	return true;
}
