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

#include <iterator> 
#include <string>
#include <sstream>
#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "InstrumentEditPanel.h"
#include "InstrumentEditorVRC7.h"

using namespace std;

static unsigned char default_inst[(16+3)*16] = 
{
#include "apu/vrc7tone.h" 
};

// CInstrumentSettingsVRC7 dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorVRC7, CInstrumentEditPanel)

CInstrumentEditorVRC7::CInstrumentEditorVRC7(CWnd* pParent /*=NULL*/)
	: CInstrumentEditPanel(CInstrumentEditorVRC7::IDD, pParent)
{
}

CInstrumentEditorVRC7::~CInstrumentEditorVRC7()
{
}

void CInstrumentEditorVRC7::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CInstrumentEditorVRC7, CInstrumentEditPanel)
//	ON_CBN_SELCHANGE(IDC_PATCH, OnCbnSelchangePatch)
//	ON_BN_CLICKED(IDC_M_AM, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_M_VIB, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_M_EG, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_M_KSR2, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_M_DM, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_C_AM, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_C_VIB, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_C_EG, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_C_KSR, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_BN_CLICKED(IDC_C_DM, &CInstrumentEditorVRC7::OnBnClickedCheckbox)
//	ON_WM_VSCROLL()
//	ON_WM_HSCROLL()
//	ON_WM_CONTEXTMENU()
//	ON_COMMAND(IDC_COPY, &CInstrumentEditorVRC7::OnCopy)
//	ON_COMMAND(IDC_PASTE, &CInstrumentEditorVRC7::OnPaste)
//END_MESSAGE_MAP()

void CInstrumentEditorVRC7::patch_currentIndexChanged(int index)
{
   OnCbnSelchangePatch();
}

void CInstrumentEditorVRC7::copy_clicked()
{
   OnCopy();
}

void CInstrumentEditorVRC7::paste_clicked()
{
   OnPaste();
}

void CInstrumentEditorVRC7::m_am_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_vib_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_eq_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_ksr2_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_dm_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_eg_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::m_ksl_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_KSL)));
}

void CInstrumentEditorVRC7::m_mul_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_MUL)));
}

void CInstrumentEditorVRC7::m_ar_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_AR)));
}

void CInstrumentEditorVRC7::m_dr_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_DR)));
}

void CInstrumentEditorVRC7::m_sl_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_SL)));
}

void CInstrumentEditorVRC7::m_rr_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_M_RR)));
}

void CInstrumentEditorVRC7::c_am_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_vib_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_eq_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_ksr_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_dm_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_eg_clicked()
{
   OnBnClickedCheckbox();
}

void CInstrumentEditorVRC7::c_ksl_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_KSL)));
}

void CInstrumentEditorVRC7::c_mul_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_MUL)));
}

void CInstrumentEditorVRC7::c_ar_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_AR)));
}

void CInstrumentEditorVRC7::c_dr_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_DR)));
}

void CInstrumentEditorVRC7::c_sl_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_SL)));
}

void CInstrumentEditorVRC7::c_rr_valueChanged(int val)
{
   OnHScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_C_RR)));
}

void CInstrumentEditorVRC7::tl_valueChanged(int val)
{
   OnVScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_TL)));
}

void CInstrumentEditorVRC7::fb_valueChanged(int val)
{
   OnVScroll(SB_HORZ,val,dynamic_cast<CScrollBar*>(GetDlgItem(IDC_FB)));
}

// CInstrumentSettingsVRC7 message handlers

BOOL CInstrumentEditorVRC7::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox *pPatchBox = (CComboBox*)GetDlgItem(IDC_PATCH);
	CString Text;

	Text.Format(_T("Patch #0 (custom patch)"));
	pPatchBox->AddString(Text);

	for (int i = 1; i < 16; ++i) {
		Text.Format(_T("Patch #%i"), i);
		pPatchBox->AddString(Text);
	}

	pPatchBox->SetCurSel(0);

	SetupSlider(IDC_M_MUL, 15);
	SetupSlider(IDC_C_MUL, 15);
	SetupSlider(IDC_M_KSL, 3);
	SetupSlider(IDC_C_KSL, 3);
	SetupSlider(IDC_TL, 63);
	SetupSlider(IDC_FB, 7);
	SetupSlider(IDC_M_AR, 15);
	SetupSlider(IDC_M_DR, 15);
	SetupSlider(IDC_M_SL, 15);
	SetupSlider(IDC_M_RR, 15);
	SetupSlider(IDC_C_AR, 15);
	SetupSlider(IDC_C_DR, 15);
	SetupSlider(IDC_C_SL, 15);
	SetupSlider(IDC_C_RR, 15);

	EnableControls(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditorVRC7::OnCbnSelchangePatch()
{
	int Patch;
	CComboBox *pPatchBox = (CComboBox*)GetDlgItem(IDC_PATCH);
	Patch = pPatchBox->GetCurSel();
	//CInstrumentVRC7 *InstConf = (CInstrumentVRC7*)GetDocument()->GetInstrument(m_iInstrument);
	m_pInstrument->SetPatch(Patch);
	//InstConf->SetPatch(Patch);
	EnableControls(Patch == 0);

	if (Patch == 0)
		LoadCustomPatch();
	else
		LoadInternalPatch(Patch);

//	CheckDlgButton(IDC_HOLD, InstConf->GetHold());
}

void CInstrumentEditorVRC7::EnableControls(bool bEnable)
{
	const int SLIDER_IDS[] = {
		IDC_M_AM, IDC_M_AR, 
		IDC_M_DM, IDC_M_DR, 
		IDC_M_EG, IDC_M_KSL, 
		IDC_M_KSR2, IDC_M_MUL, 
		IDC_M_RR, IDC_M_SL, 
		IDC_M_SL, IDC_M_VIB,
		IDC_C_AM, IDC_C_AR, 
		IDC_C_DM, IDC_C_DR, 
		IDC_C_EG, IDC_C_KSL, 
		IDC_C_KSR, IDC_C_MUL, 
		IDC_C_RR, IDC_C_SL, 
		IDC_C_SL, IDC_C_VIB,
		IDC_TL, IDC_FB
	};

	const int SLIDERS = sizeof(SLIDER_IDS) / sizeof(SLIDER_IDS[0]);

	for (int i = 0; i < SLIDERS; ++i)
		GetDlgItem(SLIDER_IDS[i])->EnableWindow(bEnable ? TRUE : FALSE);
}

void CInstrumentEditorVRC7::SelectInstrument(int Instrument)
{
	CComboBox *pPatchBox = (CComboBox*)GetDlgItem(IDC_PATCH);
	m_pInstrument = (CInstrumentVRC7*)GetDocument()->GetInstrument(Instrument);
	int Patch = m_pInstrument->GetPatch();

	pPatchBox->SetCurSel(Patch);
	
	if (Patch == 0)
		LoadCustomPatch();
	else
		LoadInternalPatch(Patch);
	
	EnableControls(Patch == 0);
}

BOOL CInstrumentEditorVRC7::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CInstrumentEditorVRC7::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CInstrumentEditorVRC7::SetupSlider(int Slider, int Max)
{
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(Slider);
	pSlider->SetRangeMax(Max);
}

int CInstrumentEditorVRC7::GetSliderVal(int Slider)
{
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(Slider);
	return pSlider->GetPos();
}

void CInstrumentEditorVRC7::SetSliderVal(int Slider, int Value)
{
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(Slider);
	pSlider->SetPos(Value);
}

void CInstrumentEditorVRC7::LoadInternalPatch(int Num)
{
	unsigned int Reg;

	GetDlgItem(IDC_PASTE)->EnableWindow(FALSE);

	// Register 0
	Reg = default_inst[(Num * 16) + 0];
	CheckDlgButton(IDC_M_AM, Reg & 0x80 ? 1 : 0);
	CheckDlgButton(IDC_M_VIB, Reg & 0x40 ? 1 : 0);
	CheckDlgButton(IDC_M_EG, Reg & 0x20 ? 1 : 0);
	CheckDlgButton(IDC_M_KSR2, Reg & 0x10 ? 1 : 0);
	SetSliderVal(IDC_M_MUL, Reg & 0x0F);

	// Register 1
	Reg = default_inst[(Num * 16) + 1];
	CheckDlgButton(IDC_C_AM, Reg & 0x80 ? 1 : 0);
	CheckDlgButton(IDC_C_VIB, Reg & 0x40 ? 1 : 0);
	CheckDlgButton(IDC_C_EG, Reg & 0x20 ? 1 : 0);
	CheckDlgButton(IDC_C_KSR, Reg & 0x10 ? 1 : 0);
	SetSliderVal(IDC_C_MUL, Reg & 0x0F);

	// Register 2
	Reg = default_inst[(Num * 16) + 2];
	SetSliderVal(IDC_M_KSL, Reg >> 6);
	SetSliderVal(IDC_TL, Reg & 0x3F);

	// Register 3
	Reg = default_inst[(Num * 16) + 3];
	SetSliderVal(IDC_C_KSL, Reg >> 6);
	SetSliderVal(IDC_FB, 7 - (Reg & 7));
	CheckDlgButton(IDC_C_DM, Reg & 0x10 ? 1 : 0);
	CheckDlgButton(IDC_M_DM, Reg & 0x08 ? 1 : 0);

	// Register 4
	Reg = default_inst[(Num * 16) + 4];
	SetSliderVal(IDC_M_AR, Reg >> 4);
	SetSliderVal(IDC_M_DR, Reg & 0x0F);

	// Register 5
	Reg = default_inst[(Num * 16) + 5];
	SetSliderVal(IDC_C_AR, Reg >> 4);
	SetSliderVal(IDC_C_DR, Reg & 0x0F);

	// Register 6
	Reg = default_inst[(Num * 16) + 6];
	SetSliderVal(IDC_M_SL, Reg >> 4);
	SetSliderVal(IDC_M_RR, Reg & 0x0F);

	// Register 7
	Reg = default_inst[(Num * 16) + 7];
	SetSliderVal(IDC_C_SL, Reg >> 4);
	SetSliderVal(IDC_C_RR, Reg & 0x0F);
}

void CInstrumentEditorVRC7::LoadCustomPatch()
{
	unsigned int Reg;

	GetDlgItem(IDC_PASTE)->EnableWindow(TRUE);

	// Register 0
	Reg = m_pInstrument->GetCustomReg(0);
	CheckDlgButton(IDC_M_AM, Reg & 0x80 ? 1 : 0);
	CheckDlgButton(IDC_M_VIB, Reg & 0x40 ? 1 : 0);
	CheckDlgButton(IDC_M_EG, Reg & 0x20 ? 1 : 0);
	CheckDlgButton(IDC_M_KSR2, Reg & 0x10 ? 1 : 0);
	SetSliderVal(IDC_M_MUL, Reg & 0x0F);

	// Register 1
	Reg = m_pInstrument->GetCustomReg(1);
	CheckDlgButton(IDC_C_AM, Reg & 0x80 ? 1 : 0);
	CheckDlgButton(IDC_C_VIB, Reg & 0x40 ? 1 : 0);
	CheckDlgButton(IDC_C_EG, Reg & 0x20 ? 1 : 0);
	CheckDlgButton(IDC_C_KSR, Reg & 0x10 ? 1 : 0);
	SetSliderVal(IDC_C_MUL, Reg & 0x0F);

	// Register 2
	Reg = m_pInstrument->GetCustomReg(2);
	SetSliderVal(IDC_M_KSL, Reg >> 6);
	SetSliderVal(IDC_TL, Reg & 0x3F);

	// Register 3
	Reg = m_pInstrument->GetCustomReg(3);
	SetSliderVal(IDC_C_KSL, Reg >> 6);
	SetSliderVal(IDC_FB, 7 - (Reg & 7));
	CheckDlgButton(IDC_C_DM, Reg & 0x10 ? 1 : 0);
	CheckDlgButton(IDC_M_DM, Reg & 0x08 ? 1 : 0);

	// Register 4
	Reg = m_pInstrument->GetCustomReg(4);
	SetSliderVal(IDC_M_AR, Reg >> 4);
	SetSliderVal(IDC_M_DR, Reg & 0x0F);

	// Register 5
	Reg = m_pInstrument->GetCustomReg(5);
	SetSliderVal(IDC_C_AR, Reg >> 4);
	SetSliderVal(IDC_C_DR, Reg & 0x0F);

	// Register 6
	Reg = m_pInstrument->GetCustomReg(6);
	SetSliderVal(IDC_M_SL, Reg >> 4);
	SetSliderVal(IDC_M_RR, Reg & 0x0F);

	// Register 7
	Reg = m_pInstrument->GetCustomReg(7);
	SetSliderVal(IDC_C_SL, Reg >> 4);
	SetSliderVal(IDC_C_RR, Reg & 0x0F);
}

void CInstrumentEditorVRC7::SaveCustomPatch()
{
	int Reg;

	// Register 0
	Reg  = (IsDlgButtonChecked(IDC_M_AM) ? 0x80 : 0);
	Reg |= (IsDlgButtonChecked(IDC_M_VIB) ? 0x40 : 0);
	Reg |= (IsDlgButtonChecked(IDC_M_EG) ? 0x20 : 0);
	Reg |= (IsDlgButtonChecked(IDC_M_KSR2) ? 0x10 : 0);
	Reg |= GetSliderVal(IDC_M_MUL);
	m_pInstrument->SetCustomReg(0, Reg);

	// Register 1
	Reg  = (IsDlgButtonChecked(IDC_C_AM) ? 0x80 : 0);
	Reg |= (IsDlgButtonChecked(IDC_C_VIB) ? 0x40 : 0);
	Reg |= (IsDlgButtonChecked(IDC_C_EG) ? 0x20 : 0);
	Reg |= (IsDlgButtonChecked(IDC_C_KSR) ? 0x10 : 0);
	Reg |= GetSliderVal(IDC_C_MUL);
	m_pInstrument->SetCustomReg(1, Reg);

	// Register 2
	Reg  = GetSliderVal(IDC_M_KSL) << 6;
	Reg |= GetSliderVal(IDC_TL);
	m_pInstrument->SetCustomReg(2, Reg);

	// Register 3
	Reg  = GetSliderVal(IDC_C_KSL) << 6;
	Reg |= IsDlgButtonChecked(IDC_C_DM) ? 0x10 : 0;
	Reg |= IsDlgButtonChecked(IDC_M_DM) ? 0x08 : 0;
	Reg |= 7 - GetSliderVal(IDC_FB);
	m_pInstrument->SetCustomReg(3, Reg);

	// Register 4
	Reg = GetSliderVal(IDC_M_AR) << 4;
	Reg |= GetSliderVal(IDC_M_DR);
	m_pInstrument->SetCustomReg(4, Reg);

	// Register 5
	Reg = GetSliderVal(IDC_C_AR) << 4;
	Reg |= GetSliderVal(IDC_C_DR);
	m_pInstrument->SetCustomReg(5, Reg);

	// Register 6
	Reg = GetSliderVal(IDC_M_SL) << 4;
	Reg |= GetSliderVal(IDC_M_RR);
	m_pInstrument->SetCustomReg(6, Reg);

	// Register 7
	Reg = GetSliderVal(IDC_C_SL) << 4;
	Reg |= GetSliderVal(IDC_C_RR);
	m_pInstrument->SetCustomReg(7, Reg);	
}

void CInstrumentEditorVRC7::OnBnClickedCheckbox()
{
	SaveCustomPatch();
	SetFocus();
}

void CInstrumentEditorVRC7::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SaveCustomPatch();
	SetFocus();
	CInstrumentEditPanel::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorVRC7::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SaveCustomPatch();
	SetFocus();
	CInstrumentEditPanel::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorVRC7::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, 1, _T("&Copy"));
	menu.AppendMenu(MF_STRING, 2, _T("&Paste"));

	switch (menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this)) {
		case 1: // Copy
			OnCopy();
			break;
		case 2: // Paste
			OnPaste();
			break;
	}
}

void CInstrumentEditorVRC7::OnCopy()
{
	CString MML;

	int patch = m_pInstrument->GetPatch();
	// Assemble a MML string
	for (int i = 0; i < 8; ++i)
		MML.AppendFormat(_T("$%02X "), (patch == 0) ? (unsigned char)(m_pInstrument->GetCustomReg(i)) : default_inst[patch * 16 + i]);
	
	if (!OpenClipboard())
		return;

	EmptyClipboard();

	int size = MML.GetLength() + 1;
	HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, size);
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);  
	strcpy_s(lptstrCopy, size, MML.GetBuffer());
	GlobalUnlock(hMem);
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

void CInstrumentEditorVRC7::OnPaste()
{
   qDebug("CInstrumentEditorVRC7::OnPaste not implemented...");
#if 0
   // Copy from clipboard
	if (!OpenClipboard())
		return;

	HANDLE hMem = GetClipboardData(CF_TEXT);
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hMem);
	string str = lptstrCopy;
	GlobalUnlock(hMem);
	CloseClipboard();

	// Convert to register values
	istringstream values(str);
	istream_iterator<string> begin(values);
	istream_iterator<string> end;

	for (int i = 0; (i < 8) && (begin != end); ++i) {
		string number = *begin++;
		if (number[0] == _T('$')) {
			int value;
			_stscanf_s(number.c_str(), _T("$%X"), &value);
			if (value >= 0 && value <= 0xFF) {
				m_pInstrument->SetCustomReg(i, value);
			}
		}
	}

	LoadCustomPatch();
#endif
}
