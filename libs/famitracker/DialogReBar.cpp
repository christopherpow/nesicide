// OctaveDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "FamiTracker.h"
#include "DialogReBar.h"
#include "MainFrm.h"

// COctaveDlgBar dialog

IMPLEMENT_DYNAMIC(CDialogReBar, CDialogBar)
CDialogReBar::CDialogReBar(CWnd* pParent /*=NULL*/)
	: CDialogBar(/*COctaveDlgBar::IDD, pParent*/)
{
}

CDialogReBar::~CDialogReBar()
{
}

//BEGIN_MESSAGE_MAP(CDialogReBar, CDialogBar)
//	ON_WM_ERASEBKGND()
//	ON_WM_MOVE()
//	ON_WM_CTLCOLOR()
//END_MESSAGE_MAP()

void CDialogReBar::follow_clicked()
{
}

// COctaveDlgBar message handlers

BOOL CDialogReBar::OnEraseBkgnd(CDC* pDC)
{
	if (!theApp.IsThemeActive()) {
		CDialogBar::OnEraseBkgnd(pDC);
		return TRUE;
	}

	CWnd* pParent = GetParent();
    ASSERT_VALID(pParent);
    CPoint pt(0, 0);
    MapWindowPoints(pParent, &pt, 1);
    pt = pDC->OffsetWindowOrg(pt.x, pt.y);
    LRESULT lResult = pParent->SendMessage(WM_ERASEBKGND, (WPARAM)pDC->m_hDC, 0L);
    pDC->SetWindowOrg(pt.x, pt.y);
    return (BOOL)lResult;
}

void CDialogReBar::OnMove(int x, int y)
{
	Invalidate();
}

HBRUSH CDialogReBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC && theApp.IsThemeActive()) {
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

BOOL CDialogReBar::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{
	CSpinButtonCtrl *Spin;

	if (!CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID))
		return FALSE;

	Spin = (CSpinButtonCtrl*)GetDlgItem(IDC_HIGHLIGHTSPIN1);
	Spin->SetRange(0, 32);

	Spin = (CSpinButtonCtrl*)GetDlgItem(IDC_HIGHLIGHTSPIN2);
	Spin->SetRange(0, 32);

	return TRUE;
}
