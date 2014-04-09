#pragma once

const int ROOT_ITEM_COUNT = 7;

// CChannelsDlg dialog

class CChannelsDlg : public CDialog
{
	DECLARE_DYNAMIC(CChannelsDlg)

public:
	CChannelsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChannelsDlg();

// Dialog Data
	enum { IDD = IDD_CHANNELS };

protected:
	CTreeCtrl *m_pAvailableTree;
	CListCtrl *m_pAddedChannels;

	HTREEITEM m_hRootItems[ROOT_ITEM_COUNT];

	void AddChannel(int ChanID);
	void InsertChannel(HTREEITEM hItem);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAvailable(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnDblClickAvailable(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnDblClickAdded(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnBnClickedMoveDown();
	afx_msg void OnNMRClickAvailableTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedMoveUp();
};
