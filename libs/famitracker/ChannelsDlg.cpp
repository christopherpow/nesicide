// Source\ChannelsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "TrackerChannel.h"
#include "ChannelsDlg.h"
#include "apu/apu.h"

// Used to handle channels in a future version. Not finished.

const TCHAR *ROOT_ITEMS[] = {
	_T("2A03/2A07"), 
	_T("Konami VRC6"), 
	_T("Konami VRC7"), 
	_T("Nintendo FDS"), 
	_T("Nintendo MMC5"), 
	_T("Namco 106"), 
	_T("Sunsoft 5B")
};

const int CHILD_ITEMS_ID[ROOT_ITEM_COUNT][9] = {
	// 2A03
	{CHANID_SQUARE1, CHANID_SQUARE2, CHANID_TRIANGLE, CHANID_NOISE, CHANID_DPCM},
	// VRC 6
	{CHANID_VRC6_PULSE1, CHANID_VRC6_PULSE2, CHANID_VRC6_SAWTOOTH},
	// VRC 7
	{CHANID_VRC7_CH1, CHANID_VRC7_CH2, CHANID_VRC7_CH3, CHANID_VRC7_CH4, CHANID_VRC7_CH5, CHANID_VRC7_CH6},
	// FDS
	{CHANID_FDS},
	// MMC5
	{CHANID_MMC5_SQUARE1, CHANID_MMC5_SQUARE2},
	// N163
	{CHANID_N163_CHAN1, CHANID_N163_CHAN2, CHANID_N163_CHAN3, CHANID_N163_CHAN4, CHANID_N163_CHAN5, CHANID_N163_CHAN6, CHANID_N163_CHAN7, CHANID_N163_CHAN8}, 
	 // S5B
	{CHANID_S5B_CH1, CHANID_S5B_CH2, CHANID_S5B_CH3}
};

const TCHAR *CHILD_ITEMS[ROOT_ITEM_COUNT][9] = {
	// 2A03
	{_T("Square 1"), _T("Square 2"), _T("Triangle"), _T("Noise"), _T("DPCM")},
	// VRC 6
	{_T("Pulse 1"), _T("Pulse 2"), _T("Sawtooth")},
	// VRC 7
	{_T("Channel 1"), _T("Channel 2"), _T("Channel 3"), _T("Channel 4"), _T("Channel 5"), _T("Channel 6")},
	// FDS
	{_T("FDS")},
	// MMC5
	{_T("Square 1"), _T("Square 2")},
	// N163
	{_T("Channel 1"), _T("Channel 2"), _T("Channel 3"), _T("Channel 4"), _T("Channel 5"), _T("Channel 6"), _T("Channel 7"), _T("Channel 8")},
	 // S5B
	{_T("Square 1"), _T("Square 2"), _T("Square 3")}
};

// CChannelsDlg dialog

IMPLEMENT_DYNAMIC(CChannelsDlg, CDialog)

CChannelsDlg::CChannelsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelsDlg::IDD, pParent)
{
//   IDD_CHANNELS DIALOGEX 0, 0, 303, 236
   CRect rect(CPoint(0,0),CSize(303,236));
   MapDialogRect(&rect);
   setFixedSize(rect.Width(),rect.Height());

//       GROUPBOX        "Available channels",IDC_STATIC,7,7,138,197
   CGroupBox* mfc6 = new CGroupBox(this);
   CRect r6(CPoint(7,7),CSize(138,197));
   MapDialogRect(&r6);
   mfc6->Create(_T("Available channels"),WS_VISIBLE,r6,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       GROUPBOX        "Added channels",IDC_STATIC,151,7,145,197
   CGroupBox* mfc7 = new CGroupBox(this);
   CRect r7(CPoint(151,7),CSize(145,197));
   MapDialogRect(&r7);
   mfc7->Create(_T("Added channels"),WS_VISIBLE,r7,this,IDC_STATIC);
   // IDC_STATIC do not get added to MFC-to-Qt map.
//       DEFPUSHBUTTON   "OK",IDOK,194,215,50,14
   CButton* mfc1 = new CButton(this);
   CRect r1(CPoint(194,215),CSize(50,14));
   MapDialogRect(&r1);
   mfc1->Create(_T("OK"),BS_DEFPUSHBUTTON | WS_VISIBLE,r1,this,IDOK);
   mfcToQtWidget.insert(IDOK,mfc1);
   QObject::connect(mfc1,SIGNAL(clicked()),this,SLOT(ok_clicked()));
//       PUSHBUTTON      "Cancel",IDCANCEL,246,215,50,14
   CButton* mfc2 = new CButton(this);
   CRect r2(CPoint(246,215),CSize(50,14));
   MapDialogRect(&r2);
   mfc2->Create(_T("Cancel"),WS_VISIBLE,r2,this,IDCANCEL);
   mfcToQtWidget.insert(IDCANCEL,mfc2);
   QObject::connect(mfc2,SIGNAL(clicked()),this,SLOT(cancel_clicked()));
//       PUSHBUTTON      "Move up",IDC_MOVE_UP,7,215,50,14
   CButton* mfc3 = new CButton(this);
   CRect r3(CPoint(7,215),CSize(50,14));
   MapDialogRect(&r3);
   mfc3->Create(_T("Move up"),WS_VISIBLE,r3,this,IDC_MOVE_UP);
   mfcToQtWidget.insert(IDC_MOVE_UP,mfc3);
   QObject::connect(mfc3,SIGNAL(clicked()),this,SLOT(moveUp_clicked()));
//       PUSHBUTTON      "Move down",IDC_MOVE_DOWN,62,215,50,14
   CButton* mfc4 = new CButton(this);
   CRect r4(CPoint(62,215),CSize(50,14));
   MapDialogRect(&r4);
   mfc4->Create(_T("Move down"),WS_VISIBLE,r4,this,IDC_MOVE_DOWN);
   mfcToQtWidget.insert(IDC_MOVE_DOWN,mfc4);
   QObject::connect(mfc4,SIGNAL(clicked()),this,SLOT(moveDown_clicked()));
//       CONTROL         "",IDC_ADDED_LIST,"SysListView32",LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP,159,17,129,181
   CListCtrl* mfc5 = new CListCtrl(this);
   CRect r5(CPoint(159,17),CSize(129,181));
   MapDialogRect(&r5);
   mfc5->Create(LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER | WS_BORDER | WS_TABSTOP | WS_VISIBLE,r5,this,IDC_ADDED_LIST);
   mfcToQtWidget.insert(IDC_ADDED_LIST,mfc5);
   QObject::connect(mfc5,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(addedList_cellDoubleClicked(int,int)));
//       CONTROL         "",IDC_STATIC,"Static",SS_ETCHEDHORZ,7,209,289,1
   qDebug("Static horz line...");
//       CONTROL         "",IDC_AVAILABLE_TREE,"SysTreeView32",TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP,14,17,122,181
   CTreeCtrl* mfc8 = new CTreeCtrl(this);
   CRect r8(CPoint(14,17),CSize(122,181));
   MapDialogRect(&r8);
   mfc8->Create(TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | WS_BORDER | WS_HSCROLL | WS_TABSTOP | WS_VISIBLE,r8,this,IDC_AVAILABLE_TREE);
   mfcToQtWidget.insert(IDC_AVAILABLE_TREE,mfc8);
   QObject::connect(mfc8,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(availableTree_itemClicked(QTreeWidgetItem*,int)));
   QObject::connect(mfc8,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(availableTree_itemDoubleClicked(QTreeWidgetItem*,int)));
}

CChannelsDlg::~CChannelsDlg()
{
}

void CChannelsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


//BEGIN_MESSAGE_MAP(CChannelsDlg, CDialog)
//	ON_NOTIFY(NM_CLICK, IDC_AVAILABLE_TREE, OnClickAvailable)
//	ON_NOTIFY(NM_DBLCLK, IDC_AVAILABLE_TREE, OnDblClickAvailable)
//	ON_NOTIFY(NM_DBLCLK, IDC_ADDED_LIST, OnDblClickAdded)
//	ON_BN_CLICKED(IDC_MOVE_DOWN, &CChannelsDlg::OnBnClickedMoveDown)
//	ON_NOTIFY(NM_RCLICK, IDC_AVAILABLE_TREE, &CChannelsDlg::OnNMRClickAvailableTree)
//	ON_BN_CLICKED(IDC_MOVE_UP, &CChannelsDlg::OnBnClickedMoveUp)
//END_MESSAGE_MAP()

void CChannelsDlg::ok_clicked()
{
   OnOK();
}

void CChannelsDlg::cancel_clicked()
{
   OnCancel();
}

void CChannelsDlg::moveUp_clicked()
{
   OnBnClickedMoveUp();
}

void CChannelsDlg::moveDown_clicked()
{
   OnBnClickedMoveDown();
}

void CChannelsDlg::addedList_cellDoubleClicked(int row,int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnDblClickAdded(&nmhdr,&result);
}

void CChannelsDlg::availableTree_itemClicked(QTreeWidgetItem* item,int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnClickAvailable(&nmhdr,&result);
}

void CChannelsDlg::availableTree_itemDoubleClicked(QTreeWidgetItem* item,int column)
{
   NMHDR nmhdr;
   LRESULT result;
   OnDblClickAvailable(&nmhdr,&result);
}

// CChannelsDlg message handlers

BOOL CChannelsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pAvailableTree = (CTreeCtrl*)GetDlgItem(IDC_AVAILABLE_TREE);
	m_pAddedChannels = (CListCtrl*)GetDlgItem(IDC_ADDED_LIST);

	int RootItems = sizeof(ROOT_ITEMS) / sizeof(TCHAR);

//	m_pAddedChannels->GetWIndowLon

	m_pAddedChannels->InsertColumn(0, _T("Name"), 0, 150);

	for (int i = 0; i < ROOT_ITEM_COUNT; ++i) {
		HTREEITEM hItem = m_pAvailableTree->InsertItem(ROOT_ITEMS[i]);
		m_hRootItems[i] = hItem;
		for (int j = 0; CHILD_ITEMS[i][j] != NULL; ++j) {
			CString str;
			str.Format(_T("%i: %s"), j + 1, CHILD_ITEMS[i][j]);
			HTREEITEM hChild = m_pAvailableTree->InsertItem(str, hItem);
			m_pAvailableTree->SetItemData(hChild, CHILD_ITEMS_ID[i][j]);
		}
		m_pAvailableTree->SortChildren(hItem);
	}

	CChannelMap *map = theApp.GetChannelMap();

	CFamiTrackerDoc *pDoc = CFamiTrackerDoc::GetDoc();

	for (unsigned i = 0; i < pDoc->GetAvailableChannels(); ++i) {
		CTrackerChannel *pChannel = pDoc->GetChannel(i);
		AddChannel(pChannel->GetID());
	}
/*
	AddChannel(CHANID_SQUARE1);
	AddChannel(CHANID_SQUARE2);
	AddChannel(CHANID_TRIANGLE);
	AddChannel(CHANID_NOISE);
	AddChannel(CHANID_DPCM);
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelsDlg::OnClickAvailable(NMHDR *pNMHDR, LRESULT *result)
{
	
}

void CChannelsDlg::OnDblClickAvailable(NMHDR *pNMHDR, LRESULT *result)
{
	// Add channel

	HTREEITEM hItem = m_pAvailableTree->GetSelectedItem();

	if ((hItem != NULL) && !m_pAvailableTree->ItemHasChildren(hItem)) {
		InsertChannel(hItem);
	}
}

void CChannelsDlg::OnDblClickAdded(NMHDR *pNMHDR, LRESULT *result)
{
	int Index = m_pAddedChannels->GetSelectionMark();
	int Count = m_pAddedChannels->GetItemCount();

	if (Index != -1 && Count > 1) {

		int ChanID = m_pAddedChannels->GetItemData(Index);

		m_pAvailableTree->GetRootItem();

		// Put back in available list
		for (int i = 0; i < ROOT_ITEM_COUNT; ++i) {
			HTREEITEM hParent = m_hRootItems[i];
			HTREEITEM hItem = m_pAvailableTree->GetNextItem(hParent, TVGN_CHILD);
			for (int j = 0; CHILD_ITEMS[i][j] != NULL; ++j) {
				if (CHILD_ITEMS_ID[i][j] == ChanID) {
					CString str;
					str.Format(_T("%i: %s"), j, CHILD_ITEMS[i][j]);
					HTREEITEM hChild = m_pAvailableTree->InsertItem(str, hParent, hParent);
					m_pAvailableTree->SetItemData(hChild, CHILD_ITEMS_ID[i][j]);
					m_pAvailableTree->Expand(hParent, TVE_EXPAND);
				}
				hItem = m_pAvailableTree->GetNextItem(hItem, TVGN_NEXT);
			}
			m_pAvailableTree->SortChildren(hParent);
		}

		m_pAddedChannels->DeleteItem(Index);
	}
}

void CChannelsDlg::AddChannel(int ChanID)
{
	for (int i = 0; i < ROOT_ITEM_COUNT; ++i) {
		HTREEITEM hItem = m_pAvailableTree->GetNextItem(m_hRootItems[i], TVGN_CHILD);
		for (int j = 0; hItem != NULL; ++j) {

			int ID = m_pAvailableTree->GetItemData(hItem);

			if (ID == ChanID) {
				InsertChannel(hItem);
				return;
			}

			hItem = m_pAvailableTree->GetNextItem(hItem, TVGN_NEXT);
		}
	}
}

void CChannelsDlg::InsertChannel(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_pAvailableTree->GetParentItem(hItem);

	if (hParentItem != NULL) {

		CString ChanName = m_pAvailableTree->GetItemText(hItem);
		CString ChipName = m_pAvailableTree->GetItemText(hParentItem);

		CString AddStr = ChipName + _T(" :: ") + ChanName.Right(ChanName.GetLength() - 3);

		// Channel ID
		int ChanId = m_pAvailableTree->GetItemData(hItem);

		int ChansAdded = m_pAddedChannels->GetItemCount();
		int Index = m_pAddedChannels->InsertItem(ChansAdded, AddStr);

		m_pAddedChannels->SetItemData(Index, ChanId);

		// Remove channel from available list
		m_pAvailableTree->DeleteItem(hItem);
	}
}

void CChannelsDlg::OnBnClickedMoveDown()
{
	int Index = m_pAddedChannels->GetSelectionMark();

	if (Index >= m_pAddedChannels->GetItemCount() - 1 || Index == -1)
		return;

	CString text = m_pAddedChannels->GetItemText(Index, 0);
	int data = m_pAddedChannels->GetItemData(Index);

	m_pAddedChannels->SetItemText(Index, 0, m_pAddedChannels->GetItemText(Index + 1, 0));
	m_pAddedChannels->SetItemData(Index, m_pAddedChannels->GetItemData(Index + 1));

	m_pAddedChannels->SetItemText(Index + 1, 0, text);
	m_pAddedChannels->SetItemData(Index + 1, data);

	m_pAddedChannels->SetSelectionMark(Index + 1);
	m_pAddedChannels->SetItemState(Index + 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_pAddedChannels->EnsureVisible(Index + 1, FALSE);
}

void CChannelsDlg::OnBnClickedMoveUp()
{
	int Index = m_pAddedChannels->GetSelectionMark();

	if (Index == 0 || Index == -1)
		return;

	CString text = m_pAddedChannels->GetItemText(Index, 0);
	int data = m_pAddedChannels->GetItemData(Index);

	m_pAddedChannels->SetItemText(Index, 0, m_pAddedChannels->GetItemText(Index - 1, 0));
	m_pAddedChannels->SetItemData(Index, m_pAddedChannels->GetItemData(Index - 1));

	m_pAddedChannels->SetItemText(Index - 1, 0, text);
	m_pAddedChannels->SetItemData(Index - 1, data);

	m_pAddedChannels->SetSelectionMark(Index - 1);
	m_pAddedChannels->SetItemState(Index - 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_pAddedChannels->EnsureVisible(Index - 1, FALSE);
}

void CChannelsDlg::OnNMRClickAvailableTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
