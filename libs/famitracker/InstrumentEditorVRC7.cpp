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
//   IDD_INSTRUMENT_VRC7 DIALOGEX 0, 0, 372, 172
   CRect rect(CPoint(0,0),CSize(372,172));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());
   
//       GROUPBOX        "Patch",IDC_STATIC,7,7,310,30
   CGroupBox* mfc1 = new CGroupBox(this);
   mfc1->setTitle("Patch");
   CRect r1(CPoint(7,7),CSize(310,30));
   MapDialogRect(&r1);
   mfc1->setGeometry(r1);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       COMBOBOX        IDC_PATCH,15,18,295,128,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
   CComboBox* mfc2 = new CComboBox(this);
   CRect r2(CPoint(15,18),CSize(295,12)); // COMBOBOX resource vertical extent includes drop-down height
   MapDialogRect(&r2);
   mfc2->setGeometry(r2);
   mfcToQtWidget.insert(IDC_PATCH,mfc2);
   QObject::connect(mfc2,SIGNAL(currentIndexChanged(int)),this,SLOT(patch_currentIndexChanged(int)));
//       PUSHBUTTON      "Copy",IDC_COPY,322,7,43,14
   CButton* mfc3 = new CButton(this);
   mfc3->setText("Copy");
   CRect r3(CPoint(322,7),CSize(43,14));
   MapDialogRect(&r3);
   mfc3->setGeometry(r3);
   mfcToQtWidget.insert(IDC_COPY,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),this,SLOT(copy_clicked()));
//       PUSHBUTTON      "Paste",IDC_PASTE,322,23,43,14
   CButton* mfc4 = new CButton(this);
   mfc4->setText("Paste");
   CRect r4(CPoint(322,23),CSize(43,14));
   MapDialogRect(&r4);
   mfc4->setGeometry(r4);
   mfcToQtWidget.insert(IDC_PASTE,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(paste_clicked()));
//       GROUPBOX        "Modulator settings",IDC_STATIC,7,42,153,123
   CGroupBox* mfc5 = new CGroupBox(this);
   mfc5->setTitle("Modulator settings");
   CRect r5(CPoint(7,42),CSize(153,123));
   MapDialogRect(&r5);
   mfc5->setGeometry(r5);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "Amplitude modulation",IDC_M_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,54,84,10,WS_EX_TRANSPARENT
   CCheckBox* mfc6 = new CCheckBox(this);
   mfc6->setText("Amplitude modulation");
   CRect r6(CPoint(14,54),CSize(84,10));
   MapDialogRect(&r6);
   mfc6->setGeometry(r6);
   mfcToQtWidget.insert(IDC_M_AM,mfc6);
   QObject::connect(mfc6,SIGNAL(clicked()),this,SLOT(m_am_clicked()));
//       CONTROL         "Vibrato",IDC_M_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,104,54,39,10,WS_EX_TRANSPARENT
   CCheckBox* mfc7 = new CCheckBox(this);
   mfc7->setText("Vibrato");
   CRect r7(CPoint(104,54),CSize(39,10));
   MapDialogRect(&r7);
   mfc7->setGeometry(r7);
   mfcToQtWidget.insert(IDC_M_VIB,mfc7);
   QObject::connect(mfc7,SIGNAL(clicked()),this,SLOT(m_vib_clicked()));
//       CONTROL         "Sustained",IDC_M_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,70,47,10,WS_EX_TRANSPARENT
   CCheckBox* mfc8 = new CCheckBox(this);
   mfc8->setText("Sustained");
   CRect r8(CPoint(14,70),CSize(47,10));
   MapDialogRect(&r8);
   mfc8->setGeometry(r8);
   mfcToQtWidget.insert(IDC_M_EG,mfc8);
   QObject::connect(mfc8,SIGNAL(clicked()),this,SLOT(m_eg_clicked()));
//       CONTROL         "RATE key scale",IDC_M_KSR2,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,89,70,65,10,WS_EX_TRANSPARENT
   CCheckBox* mfc9 = new CCheckBox(this);
   mfc9->setText("RATE key scale");
   CRect r9(CPoint(89,70),CSize(65,10));
   MapDialogRect(&r9);
   mfc9->setGeometry(r9);
   mfcToQtWidget.insert(IDC_M_KSR2,mfc9);
   QObject::connect(mfc9,SIGNAL(clicked()),this,SLOT(m_ksr2_clicked()));
//       CONTROL         "Wave rectification",IDC_M_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,14,86,74,10,WS_EX_TRANSPARENT
   CCheckBox* mfc10 = new CCheckBox(this);
   mfc10->setText("Wave rectification");
   CRect r10(CPoint(14,86),CSize(74,10));
   MapDialogRect(&r10);
   mfc10->setGeometry(r10);
   mfcToQtWidget.insert(IDC_M_DM,mfc10);
   QObject::connect(mfc10,SIGNAL(clicked()),this,SLOT(m_dm_clicked()));
//       LTEXT           "Level",IDC_STATIC,91,87,18,8
   CStatic* mfc11 = new CStatic(this);
   mfc11->setText("Level");
   CRect r11(CPoint(91,87),CSize(18,8));
   MapDialogRect(&r11);
   mfc11->setGeometry(r11);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,112,86,44,11,WS_EX_TRANSPARENT
   CSliderCtrl* mfc12 = new CSliderCtrl(this);
   CRect r12(CPoint(112,86),CSize(44,11));
   MapDialogRect(&r12);
   mfc12->setGeometry(r12);
   mfcToQtWidget.insert(IDC_M_KSL,mfc12);
   QObject::connect(mfc12,SIGNAL(valueChanged(int)),this,SLOT(m_ksl_valueChanged(int)));
//       LTEXT           "Mult. factor",IDC_STATIC,14,108,38,8
   CStatic* mfc13 = new CStatic(this);
   mfc13->setText("Mult. factor");
   CRect r13(CPoint(14,108),CSize(38,8));
   MapDialogRect(&r13);
   mfc13->setGeometry(r13);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_MUL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,51,107,105,11,WS_EX_TRANSPARENT
   CSliderCtrl* mfc14 = new CSliderCtrl(this);
   CRect r14(CPoint(51,107),CSize(105,11));
   MapDialogRect(&r14);
   mfc14->setGeometry(r14);
   mfcToQtWidget.insert(IDC_M_MUL,mfc14);
   QObject::connect(mfc14,SIGNAL(valueChanged(int)),this,SLOT(m_mul_valueChanged(int)));
//       LTEXT           "Attack",IDC_STATIC,14,127,22,8
   CStatic* mfc15 = new CStatic(this);
   mfc15->setText("Attack");
   CRect r15(CPoint(14,127),CSize(22,8));
   MapDialogRect(&r15);
   mfc15->setGeometry(r15);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_AR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,40,126,42,11
   CSliderCtrl* mfc16 = new CSliderCtrl(this);
   CRect r16(CPoint(40,126),CSize(42,11));
   MapDialogRect(&r16);
   mfc16->setGeometry(r16);
   mfcToQtWidget.insert(IDC_M_AR,mfc16);
   QObject::connect(mfc16,SIGNAL(valueChanged(int)),this,SLOT(m_ar_valueChanged(int)));
//       LTEXT           "Decay",IDC_STATIC,87,127,21,8
   CStatic* mfc17 = new CStatic(this);
   mfc17->setText("Decay");
   CRect r17(CPoint(87,127),CSize(21,8));
   MapDialogRect(&r17);
   mfc17->setGeometry(r17);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_DR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,114,126,42,11
   CSliderCtrl* mfc18 = new CSliderCtrl(this);
   CRect r18(CPoint(114,126),CSize(42,11));
   MapDialogRect(&r18);
   mfc18->setGeometry(r18);
   mfcToQtWidget.insert(IDC_M_DR,mfc18);
   QObject::connect(mfc18,SIGNAL(valueChanged(int)),this,SLOT(m_dr_valueChanged(int)));
//       LTEXT           "Sustain",IDC_STATIC,14,144,24,8
   CStatic* mfc19 = new CStatic(this);
   mfc19->setText("Sustain");
   CRect r19(CPoint(14,144),CSize(24,8));
   MapDialogRect(&r19);
   mfc19->setGeometry(r19);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_SL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,39,143,42,11
   CSliderCtrl* mfc20 = new CSliderCtrl(this);
   CRect r20(CPoint(39,143),CSize(42,11));
   MapDialogRect(&r20);
   mfc20->setGeometry(r20);
   mfcToQtWidget.insert(IDC_M_SL,mfc20);
   QObject::connect(mfc20,SIGNAL(valueChanged(int)),this,SLOT(m_sl_valueChanged(int)));
//       LTEXT           "Release",IDC_STATIC,85,144,26,8
   CStatic* mfc21 = new CStatic(this);
   mfc21->setText("Release");
   CRect r21(CPoint(85,144),CSize(26,8));
   MapDialogRect(&r21);
   mfc21->setGeometry(r21);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_M_RR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,114,143,42,11
   CSliderCtrl* mfc22 = new CSliderCtrl(this);
   CRect r22(CPoint(114,143),CSize(42,11));
   MapDialogRect(&r22);
   mfc22->setGeometry(r22);
   mfcToQtWidget.insert(IDC_M_RR,mfc22);
   QObject::connect(mfc22,SIGNAL(valueChanged(int)),this,SLOT(m_rr_valueChanged(int)));
//       GROUPBOX        "Carrier settings",IDC_STATIC,166,42,152,123
   CGroupBox* mfc23 = new CGroupBox(this);
   mfc23->setTitle("Carrier settings");
   CRect r23(CPoint(166,42),CSize(152,123));
   MapDialogRect(&r23);
   mfc23->setGeometry(r23);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "Amplitude modulation",IDC_C_AM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,54,84,10
   CCheckBox* mfc24 = new CCheckBox(this);
   mfc24->setText("Amplitude modulation");
   CRect r24(CPoint(171,54),CSize(84,10));
   MapDialogRect(&r24);
   mfc24->setGeometry(r24);
   mfcToQtWidget.insert(IDC_C_AM,mfc24);
   QObject::connect(mfc24,SIGNAL(clicked()),this,SLOT(c_am_clicked()));
//       CONTROL         "Vibrato",IDC_C_VIB,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,263,54,39,10
   CCheckBox* mfc25 = new CCheckBox(this);
   mfc25->setText("Vibrato");
   CRect r25(CPoint(263,54),CSize(39,10));
   MapDialogRect(&r25);
   mfc25->setGeometry(r25);
   mfcToQtWidget.insert(IDC_C_VIB,mfc25);
   QObject::connect(mfc25,SIGNAL(clicked()),this,SLOT(c_vib_clicked()));
//       CONTROL         "Sustained",IDC_C_EG,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,70,47,10
   CCheckBox* mfc26 = new CCheckBox(this);
   mfc26->setText("Sustained");
   CRect r26(CPoint(171,70),CSize(47,10));
   MapDialogRect(&r26);
   mfc26->setGeometry(r26);
   mfcToQtWidget.insert(IDC_C_EG,mfc26);
   QObject::connect(mfc26,SIGNAL(clicked()),this,SLOT(c_eg_clicked()));
//       CONTROL         "RATE key scale",IDC_C_KSR,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,247,70,65,10
   CCheckBox* mfc27 = new CCheckBox(this);
   mfc27->setText("RATE key scale");
   CRect r27(CPoint(247,70),CSize(65,10));
   MapDialogRect(&r27);
   mfc27->setGeometry(r27);
   mfcToQtWidget.insert(IDC_C_KSR,mfc27);
   QObject::connect(mfc27,SIGNAL(clicked()),this,SLOT(c_ksr_clicked()));
//       CONTROL         "Wave rectification",IDC_C_DM,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,171,86,74,10
   CCheckBox* mfc28 = new CCheckBox(this);
   mfc28->setText("Wave rectification");
   CRect r28(CPoint(171,86),CSize(74,10));
   MapDialogRect(&r28);
   mfc28->setGeometry(r28);
   mfcToQtWidget.insert(IDC_C_DM,mfc28);
   QObject::connect(mfc28,SIGNAL(clicked()),this,SLOT(c_dm_clicked()));
//       LTEXT           "Level",IDC_STATIC,249,87,18,8
   CStatic* mfc29 = new CStatic(this);
   mfc29->setText("Level");
   CRect r29(CPoint(249,87),CSize(18,8));
   MapDialogRect(&r29);
   mfc29->setGeometry(r29);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_KSL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,270,86,40,11
   CSliderCtrl* mfc30 = new CSliderCtrl(this);
   CRect r30(CPoint(270,86),CSize(40,11));
   MapDialogRect(&r30);
   mfc30->setGeometry(r30);
   mfcToQtWidget.insert(IDC_C_KSL,mfc30);
   QObject::connect(mfc30,SIGNAL(valueChanged(int)),this,SLOT(c_ksl_valueChanged(int)));
//       LTEXT           "Mult. factor",IDC_STATIC,171,108,38,8
   CStatic* mfc31 = new CStatic(this);
   mfc31->setText("Mult. factor");
   CRect r31(CPoint(171,108),CSize(38,8));
   MapDialogRect(&r31);
   mfc31->setGeometry(r31);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_MUL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,208,107,106,11
   CSliderCtrl* mfc32 = new CSliderCtrl(this);
   CRect r32(CPoint(208,107),CSize(106,11));
   MapDialogRect(&r32);
   mfc32->setGeometry(r32);
   mfcToQtWidget.insert(IDC_C_MUL,mfc32);
   QObject::connect(mfc32,SIGNAL(valueChanged(int)),this,SLOT(c_mul_valueChanged(int)));
//       LTEXT           "Attack",IDC_STATIC,171,127,22,8
   CStatic* mfc33 = new CStatic(this);
   mfc33->setText("Attack");
   CRect r33(CPoint(171,127),CSize(22,8));
   MapDialogRect(&r33);
   mfc33->setGeometry(r33);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_AR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,199,126,42,11
   CSliderCtrl* mfc34 = new CSliderCtrl(this);
   CRect r34(CPoint(199,126),CSize(42,11));
   MapDialogRect(&r34);
   mfc34->setGeometry(r34);
   mfcToQtWidget.insert(IDC_C_AR,mfc34);
   QObject::connect(mfc34,SIGNAL(valueChanged(int)),this,SLOT(c_ar_valueChanged(int)));
//       LTEXT           "Decay",IDC_STATIC,247,127,21,8
   CStatic* mfc35 = new CStatic(this);
   mfc35->setText("Decay");
   CRect r35(CPoint(247,127),CSize(21,8));
   MapDialogRect(&r35);
   mfc35->setGeometry(r35);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_DR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,271,126,43,11
   CSliderCtrl* mfc36 = new CSliderCtrl(this);
   CRect r36(CPoint(271,126),CSize(43,11));
   MapDialogRect(&r36);
   mfc36->setGeometry(r36);
   mfcToQtWidget.insert(IDC_C_DR,mfc36);
   QObject::connect(mfc36,SIGNAL(valueChanged(int)),this,SLOT(c_dr_valueChanged(int)));
//       LTEXT           "Sustain",IDC_STATIC,171,144,24,8
   CStatic* mfc37 = new CStatic(this);
   mfc37->setText("Sustain");
   CRect r37(CPoint(171,144),CSize(24,8));
   MapDialogRect(&r37);
   mfc37->setGeometry(r37);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_SL,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,199,143,42,11
   CSliderCtrl* mfc38 = new CSliderCtrl(this);
   CRect r38(CPoint(199,143),CSize(42,11));
   MapDialogRect(&r38);
   mfc38->setGeometry(r38);
   mfcToQtWidget.insert(IDC_C_SL,mfc38);
   QObject::connect(mfc38,SIGNAL(valueChanged(int)),this,SLOT(c_sl_valueChanged(int)));
//       LTEXT           "Release",IDC_STATIC,245,144,26,8
   CStatic* mfc39 = new CStatic(this);
   mfc39->setText("Release");
   CRect r39(CPoint(245,144),CSize(26,8));
   MapDialogRect(&r39);
   mfc39->setGeometry(r39);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_C_RR,"msctls_trackbar32",TBS_AUTOTICKS | WS_TABSTOP,271,143,43,11
   CSliderCtrl* mfc40 = new CSliderCtrl(this);
   CRect r40(CPoint(271,143),CSize(43,11));
   MapDialogRect(&r40);
   mfc40->setGeometry(r40);
   mfcToQtWidget.insert(IDC_C_RR,mfc40);
   QObject::connect(mfc40,SIGNAL(valueChanged(int)),this,SLOT(c_rr_valueChanged(int)));
//       CTEXT           "Modulator\nlevel",IDC_STATIC,322,42,36,17
   CStatic* mfc41 = new CStatic(this);
   mfc41->setText("Modulator\nlevel");
   CRect r41(CPoint(322,42),CSize(36,17));
   MapDialogRect(&r41);
   mfc41->setGeometry(r41);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_TL,"msctls_trackbar32",TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_TABSTOP,329,59,16,53
   CSliderCtrl* mfc42 = new CSliderCtrl(this);
   mfc42->setOrientation(Qt::Vertical);
   mfc42->setInvertedAppearance(true);
   CRect r42(CPoint(329,59),CSize(16,53));
   MapDialogRect(&r42);
   mfc42->setGeometry(r42);
   mfcToQtWidget.insert(IDC_TL,mfc42);
   QObject::connect(mfc42,SIGNAL(valueChanged(int)),this,SLOT(tl_valueChanged(int)));
//       CTEXT           "Feedback",IDC_STATIC,322,112,36,9
   CStatic* mfc43 = new CStatic(this);
   mfc43->setText("Feedback");
   CRect r43(CPoint(322,112),CSize(36,9));
   MapDialogRect(&r43);
   mfc43->setGeometry(r43);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       CONTROL         "",IDC_FB,"msctls_trackbar32",TBS_AUTOTICKS | TBS_VERT | TBS_BOTH | WS_TABSTOP,325,124,25,41
   CSliderCtrl* mfc44 = new CSliderCtrl(this);
   mfc44->setOrientation(Qt::Vertical);
   mfc44->setInvertedAppearance(true);
   CRect r44(CPoint(325,124),CSize(25,41));
   MapDialogRect(&r44);
   mfc44->setGeometry(r44);
   mfcToQtWidget.insert(IDC_FB,mfc44);
   QObject::connect(mfc44,SIGNAL(valueChanged(int)),this,SLOT(fb_valueChanged(int)));
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
