#pragma once


// CCommentsDlg dialog

class CCommentsDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommentsDlg)

public:
	CCommentsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommentsDlg();

// Dialog Data
	enum { IDD = IDD_COMMENTS };

	static LPCTSTR FONT_FACE;
	static int FONT_SIZE;

protected:
	static RECT WinRect;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void SaveComment();

protected:
	bool m_bChanged;
	CFont *m_pFont;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnEnChangeComments();
};
