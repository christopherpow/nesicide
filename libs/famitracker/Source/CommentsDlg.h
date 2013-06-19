#pragma once

#include "cqtmfc.h"
#include "resource.h"

// CCommentsDlg dialog

class CCommentsDlg : public CDialog
{
   Q_OBJECT
   // Qt interfaces
public slots:
   void ok_clicked();
   void cancel_clicked();
   
public:
	DECLARE_DYNAMIC(CCommentsDlg)

public:
	CCommentsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommentsDlg();

// Dialog Data
	enum { IDD = IDD_COMMENTS };

	static LPCTSTR FONT_FACE;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
