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
#include "FamiTrackerView.h"
#include "ConfigAppearance.h"
#include "Settings.h"
#include "ColorScheme.h"
#include "Graphics.h"

const TCHAR *CConfigAppearance::COLOR_ITEMS[] = {
	_T("Background"), 
	_T("Highlighted background"),
	_T("Highlighted background 2"),
	_T("Pattern text"), 
	_T("Highlighted pattern text"),
	_T("Highlighted pattern text 2"),
	_T("Instrument column"),
	_T("Volume column"),
	_T("Effect number column"),
	_T("Selection"),
	_T("Cursor")
};

// Pre-defined color schemes
const COLOR_SCHEME *CConfigAppearance::COLOR_SCHEMES[] = {
	&DEFAULT_COLOR_SCHEME,
	&MONOCHROME_COLOR_SCHEME,
	&RENOISE_COLOR_SCHEME,
	&WHITE_COLOR_SCHEME
};

const int CConfigAppearance::NUM_COLOR_SCHEMES = 4;

const int CConfigAppearance::FONT_SIZES[] = {10, 11, 12, 14, 16, 18, 20, 22};
const int CConfigAppearance::FONT_SIZE_COUNT = sizeof(FONT_SIZES) / sizeof(int);

int CALLBACK CConfigAppearance::EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	if (lpelfe->elfLogFont.lfCharSet == ANSI_CHARSET && lpelfe->elfFullName[0] != '@')
		((CConfigAppearance*)lParam)->AddFontName((char*)&lpelfe->elfFullName);

	return 1;
}

// CConfigAppearance dialog

IMPLEMENT_DYNAMIC(CConfigAppearance, CPropertyPage)
CConfigAppearance::CConfigAppearance()
	: CPropertyPage(CConfigAppearance::IDD)
{
}

CConfigAppearance::~CConfigAppearance()
{
}

void CConfigAppearance::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CConfigAppearance, CPropertyPage)
//	ON_WM_PAINT()
//	ON_CBN_SELCHANGE(IDC_FONT, OnCbnSelchangeFont)
//	ON_BN_CLICKED(IDC_PICK_COL, OnBnClickedPickCol)
//	ON_CBN_SELCHANGE(IDC_COL_ITEM, OnCbnSelchangeColItem)
//	ON_CBN_SELCHANGE(IDC_SCHEME, OnCbnSelchangeScheme)
//	ON_CBN_SELCHANGE(IDC_FONT_SIZE, &CConfigAppearance::OnCbnSelchangeFontSize)
//	ON_BN_CLICKED(IDC_PATTERNCOLORS, &CConfigAppearance::OnBnClickedPatterncolors)
//END_MESSAGE_MAP()

void CConfigAppearance::paintEvent(QPaintEvent *)
{
   OnPaint();
}

void CConfigAppearance::pickCol_clicked()
{
   OnBnClickedPickCol();
}

void CConfigAppearance::patternColors_clicked()
{
   OnBnClickedPatterncolors();
}

void CConfigAppearance::scheme_currentIndexChanged(int)
{
   OnCbnSelchangeScheme();
}

void CConfigAppearance::colItem_currentIndexChanged(int)
{
   OnCbnSelchangeColItem();
}

void CConfigAppearance::font_currentIndexChanged(int)
{
   OnCbnSelchangeFont();
}

void CConfigAppearance::fontSize_currentIndexChanged(int)
{
   OnCbnSelchangeFontSize();
}

// CConfigAppearance message handlers

void CConfigAppearance::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Do not call CPropertyPage::OnPaint() for painting messages

	const TCHAR PREV_LINE[] = _T("--- -- - ---");

	CWnd *pWnd;
	CRect Rect, ParentRect;
	CBrush	BrushColor, *OldBrush;

	int ShadedCol, ShadedHiCol;

	GetWindowRect(ParentRect);

	pWnd = GetDlgItem(IDC_COL_PREVIEW);
	pWnd->GetWindowRect(Rect);

	Rect.top -= ParentRect.top;
	Rect.bottom -= ParentRect.top;
	Rect.left -= ParentRect.left;
	Rect.right -= ParentRect.left;

	BrushColor.CreateSolidBrush(m_iColors[m_iSelectedItem]);

	OldBrush = dc.SelectObject(&BrushColor);

	dc.Rectangle(Rect);

	dc.SelectObject(OldBrush);

	// Preview all colors

	pWnd = GetDlgItem(IDC_PREVIEW);
	pWnd->GetWindowRect(Rect);

	Rect.top -= ParentRect.top;
	Rect.bottom -= ParentRect.top-16;
	Rect.left -= ParentRect.left;
	Rect.right -= ParentRect.left;

	CFont Font, *OldFont;
	LOGFONT LogFont;

	memset(&LogFont, 0, sizeof(LOGFONT));
	strcpy_s(LogFont.lfFaceName, LF_FACESIZE, m_strFont.GetBuffer());

	LogFont.lfHeight = -m_iFontSize;
	LogFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

	Font.CreateFontIndirect(&LogFont);

	OldFont = dc.SelectObject(&Font);

	// Background
	dc.FillSolidRect(Rect, GetColor(COL_BACKGROUND));

	ShadedCol = DIM(GetColor(COL_PATTERN_TEXT), 50);
	ShadedHiCol = DIM(GetColor(COL_PATTERN_TEXT_HILITE), 50);

	int iRowSize = m_iFontSize + 2;

	for (int i = 0; i < 12; ++i) {

		int OffsetTop = Rect.top + (i * (iRowSize + 2));
		int OffsetLeft = Rect.left + 9;

		if (OffsetTop > (Rect.bottom - iRowSize))
			break;

		if ((i & 3) == 0) {

			dc.SetBkColor(GetColor(COL_BACKGROUND_HILITE));
			dc.FillSolidRect(Rect.left, OffsetTop, Rect.right - Rect.left, iRowSize + 2, GetColor(COL_BACKGROUND_HILITE));

			if (i == 0) {
				dc.SetTextColor(GetColor(COL_PATTERN_TEXT_HILITE));
				dc.SetBkColor(GetColor(COL_CURSOR));
				dc.FillSolidRect(Rect.left + 5, OffsetTop, 40, iRowSize + 2, GetColor(COL_CURSOR));
			}
			else
				dc.SetTextColor(ShadedHiCol);
		}
		else {
			if ((i & 3) == 0)
				dc.SetTextColor(GetColor(COL_PATTERN_TEXT));
			else
				dc.SetTextColor(ShadedCol);

			dc.SetBkColor(GetColor(COL_BACKGROUND));
		}

		if (i == 0) {
			dc.TextOut(OffsetLeft, OffsetTop, _T("C"));
			dc.TextOut(OffsetLeft + 12, OffsetTop, _T("-"));
			dc.TextOut(OffsetLeft + 24, OffsetTop, _T("4"));
		}
		else {
			dc.TextOut(OffsetLeft, OffsetTop, _T("-"));
			dc.TextOut(OffsetLeft + 12, OffsetTop, _T("-"));
			dc.TextOut(OffsetLeft + 24, OffsetTop, _T("-"));
		}

		if (i == 0) {
			dc.SetBkColor(GetColor(COL_BACKGROUND_HILITE));
		}

		if ((i & 3) == 0) {
			dc.SetTextColor(ShadedHiCol);
		}
		else {
			dc.SetTextColor(ShadedCol);
		}

		dc.TextOut(OffsetLeft + 40, OffsetTop, _T("-"));
		dc.TextOut(OffsetLeft + 52, OffsetTop, _T("-"));
		dc.TextOut(OffsetLeft + 68, OffsetTop, _T("-"));
		dc.TextOut(OffsetLeft + 84, OffsetTop, _T("-"));
		dc.TextOut(OffsetLeft + 96, OffsetTop, _T("-"));
		dc.TextOut(OffsetLeft + 108, OffsetTop, _T("-"));

	}

	dc.SelectObject(OldFont);
}

BOOL CConfigAppearance::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CComboBox *ItemsBox;

	CDC *pDC = GetDC();
	LOGFONT LogFont;

	m_strFont = theApp.GetSettings()->General.strFont;

	memset(&LogFont, 0, sizeof(LOGFONT));
	LogFont.lfCharSet = DEFAULT_CHARSET;

	m_pFontList = (CComboBox*)GetDlgItem(IDC_FONT);
	m_pFontSizeList = (CComboBox*)GetDlgItem(IDC_FONT_SIZE);

	EnumFontFamiliesEx(pDC->m_hDC, &LogFont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)this, 0);

	ReleaseDC(pDC);

	ItemsBox = (CComboBox*)GetDlgItem(IDC_COL_ITEM);

	for (int i = 0; i < COLOR_ITEM_COUNT; ++i) {
		ItemsBox->AddString(COLOR_ITEMS[i]);
	}

	ItemsBox->SelectString(0, COLOR_ITEMS[0]);

	m_iSelectedItem = 0;

	m_iColors[COL_BACKGROUND]			= theApp.GetSettings()->Appearance.iColBackground;
	m_iColors[COL_BACKGROUND_HILITE]	= theApp.GetSettings()->Appearance.iColBackgroundHilite;
	m_iColors[COL_BACKGROUND_HILITE2]	= theApp.GetSettings()->Appearance.iColBackgroundHilite2;
	m_iColors[COL_PATTERN_TEXT]			= theApp.GetSettings()->Appearance.iColPatternText;
	m_iColors[COL_PATTERN_TEXT_HILITE]	= theApp.GetSettings()->Appearance.iColPatternTextHilite;
	m_iColors[COL_PATTERN_TEXT_HILITE2]	= theApp.GetSettings()->Appearance.iColPatternTextHilite2;
	m_iColors[COL_PATTERN_INSTRUMENT]	= theApp.GetSettings()->Appearance.iColPatternInstrument;
	m_iColors[COL_PATTERN_VOLUME]		= theApp.GetSettings()->Appearance.iColPatternVolume;
	m_iColors[COL_PATTERN_EFF_NUM]		= theApp.GetSettings()->Appearance.iColPatternEffect;
	m_iColors[COL_SELECTION]			= theApp.GetSettings()->Appearance.iColSelection;
	m_iColors[COL_CURSOR]				= theApp.GetSettings()->Appearance.iColCursor;

	m_iFontSize	= theApp.GetSettings()->General.iFontSize;

	m_bPatternColors = theApp.GetSettings()->General.bPatternColor;

	ItemsBox = (CComboBox*)GetDlgItem(IDC_SCHEME);

	for (int i = 0; i < NUM_COLOR_SCHEMES; ++i) {
		ItemsBox->AddString(COLOR_SCHEMES[i]->NAME);
	}

	for (int i = 0; i < FONT_SIZE_COUNT; ++i) {
		CString str;
		str.Format(_T("%i"), FONT_SIZES[i]);
		m_pFontSizeList->AddString(str);
		if (FONT_SIZES[i] == m_iFontSize)
			m_pFontSizeList->SelectString(0, str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigAppearance::AddFontName(char *Name)
{
	m_pFontList->AddString(Name);

	if (m_strFont.Compare(Name) == 0)
		m_pFontList->SelectString(0, Name);
}

BOOL CConfigAppearance::OnApply()
{
	CSettings *pSettings = theApp.GetSettings();

	pSettings->General.strFont		 = m_strFont;
	pSettings->General.iFontSize	 = m_iFontSize;
	pSettings->General.bPatternColor = m_bPatternColors;

	pSettings->Appearance.iColBackground			= m_iColors[COL_BACKGROUND];
	pSettings->Appearance.iColBackgroundHilite		= m_iColors[COL_BACKGROUND_HILITE];
	pSettings->Appearance.iColBackgroundHilite2		= m_iColors[COL_BACKGROUND_HILITE2];
	pSettings->Appearance.iColPatternText			= m_iColors[COL_PATTERN_TEXT];
	pSettings->Appearance.iColPatternTextHilite		= m_iColors[COL_PATTERN_TEXT_HILITE];
	pSettings->Appearance.iColPatternTextHilite2	= m_iColors[COL_PATTERN_TEXT_HILITE2];
	pSettings->Appearance.iColPatternInstrument		= m_iColors[COL_PATTERN_INSTRUMENT];
	pSettings->Appearance.iColPatternVolume			= m_iColors[COL_PATTERN_VOLUME];
	pSettings->Appearance.iColPatternEffect			= m_iColors[COL_PATTERN_EFF_NUM];
	pSettings->Appearance.iColSelection				= m_iColors[COL_SELECTION];
	pSettings->Appearance.iColCursor				= m_iColors[COL_CURSOR];

	theApp.ReloadColorScheme();

	return CPropertyPage::OnApply();
}

void CConfigAppearance::OnCbnSelchangeFont()
{
	m_pFontList->GetLBText(m_pFontList->GetCurSel(), m_strFont);
	RedrawWindow();
	SetModified();
}

BOOL CConfigAppearance::OnSetActive()
{
	CheckDlgButton(IDC_PATTERNCOLORS, m_bPatternColors);
	return CPropertyPage::OnSetActive();
}

void CConfigAppearance::OnBnClickedPickCol()
{
	CColorDialog ColorDialog;

	ColorDialog.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	ColorDialog.m_cc.rgbResult = m_iColors[m_iSelectedItem];
	ColorDialog.DoModal();

	m_iColors[m_iSelectedItem] = ColorDialog.GetColor();

	SetModified();
	RedrawWindow();
}

void CConfigAppearance::OnCbnSelchangeColItem()
{
	CComboBox *List = (CComboBox*)GetDlgItem(IDC_COL_ITEM);
	m_iSelectedItem = List->GetCurSel();
	RedrawWindow();
}

void CConfigAppearance::OnCbnSelchangeScheme()
{
	CComboBox *pList = (CComboBox*)GetDlgItem(IDC_SCHEME);
	
	SelectColorScheme(COLOR_SCHEMES[pList->GetCurSel()]);

	SetModified();
	RedrawWindow();
}

void CConfigAppearance::SelectColorScheme(const COLOR_SCHEME *pColorScheme)
{
	CComboBox *pFontList = (CComboBox*)GetDlgItem(IDC_FONT);
	CComboBox *pFontSizeList = (CComboBox*)GetDlgItem(IDC_FONT_SIZE);

	SetColor(COL_BACKGROUND, pColorScheme->BACKGROUND);
	SetColor(COL_BACKGROUND_HILITE, pColorScheme->BACKGROUND_HILITE);
	SetColor(COL_BACKGROUND_HILITE2, pColorScheme->BACKGROUND_HILITE2);
	SetColor(COL_PATTERN_TEXT, pColorScheme->TEXT_NORMAL);
	SetColor(COL_PATTERN_TEXT_HILITE, pColorScheme->TEXT_HILITE);
	SetColor(COL_PATTERN_TEXT_HILITE2, pColorScheme->TEXT_HILITE2);
	SetColor(COL_PATTERN_INSTRUMENT, pColorScheme->TEXT_INSTRUMENT);
	SetColor(COL_PATTERN_VOLUME, pColorScheme->TEXT_VOLUME);
	SetColor(COL_PATTERN_EFF_NUM, pColorScheme->TEXT_EFFECT);
	SetColor(COL_SELECTION, pColorScheme->SELECTION);
	SetColor(COL_CURSOR, pColorScheme->CURSOR);

	m_strFont = pColorScheme->FONT_FACE;
	m_iFontSize = pColorScheme->FONT_SIZE;
	pFontList->SelectString(0, m_strFont);
	CString SizeStr;
	SizeStr.Format(_T("%i"), m_iFontSize);
	pFontSizeList->SelectString(0, SizeStr);
}

void CConfigAppearance::SetColor(int Index, int Color) 
{
	m_iColors[Index] = Color;
}

int CConfigAppearance::GetColor(int Index) const
{
	return m_iColors[Index];
}

void CConfigAppearance::OnCbnSelchangeFontSize()
{
	CString str;
	m_pFontSizeList->GetLBText(m_pFontSizeList->GetCurSel(), str);
	m_iFontSize = _ttoi(str);
	RedrawWindow();
	SetModified();
}

void CConfigAppearance::OnBnClickedPatterncolors()
{
	m_bPatternColors = IsDlgButtonChecked(IDC_PATTERNCOLORS) != 0;
	SetModified();
}
