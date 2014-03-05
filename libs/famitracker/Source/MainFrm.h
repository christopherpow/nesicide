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

#pragma once

#include "cqtmfc.h"

#include "FamiTrackerDoc.h"
#include "PatternEditor.h"
#include "Action.h"
#include "FrameEditor.h"
#include "InstrumentEditDlg.h"
//#include "PerformanceDlg.h"
#include "DialogReBar.h"
#include "ControlPanelDlg.h"
#include "CustomControls.h"

enum FRAME_EDIT_POS { 
	FRAME_EDIT_POS_TOP, 
	FRAME_EDIT_POS_LEFT
};

class CSampleWindow;
class CInstrumentFileTree;
class CAction;
class CActionHandler;
class CFrameEditor;

class CMainFrame : public CFrameWnd
{
   Q_OBJECT
   // Qt interfaces
protected:
   void showEvent(QShowEvent *);
   void hideEvent(QHideEvent *);
private:
   QTimer* pTimer;
   CFamiTrackerDoc* m_pDocument;
   CFamiTrackerView* m_pView;
   QString m_fileName;
   bool initialized;
public: // For some reason MOC doesn't like the protection specification inside DECLARE_DYNAMIC

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CFrameEditor *GetFrameEditor() const;

// Operations
public:

	void	SetStatusText(LPCTSTR Text,...);
	void	ChangeNoteState(int Note);
	
	void	UpdateTrackBox();
	void	ChangedTrack();
	void	ResizeFrameWindow();

	void	UpdateControls();

	// Instrument
	void	UpdateInstrumentList();
	void	CloseInstrumentEditor();
	void	SelectInstrument(int Index);
	void	ClearInstrumentList();
	void	AddInstrument(int Index);
	void	RemoveInstrument(int Index);
	void	NewInstrument(int ChipType);

	void	SetIndicatorTime(int Min, int Sec, int MSec);
	void	SetSongInfo(char *Name, char *Artist, char *Copyright);
	void	SetupColors(void);
	void	DisplayOctave();

	void	SetFirstHighlightRow(int Rows);
	void	SetSecondHighlightRow(int Rows);

	int		GetSelectedInstrument() const;
	int		GetSelectedTrack() const;
	void	SelectTrack(int Track);

	bool	AddAction(CAction *pAction);
	CAction *GetLastAction(int Filter) const;
	void	ResetUndo();

	void	UpdateMenus();

	bool	ChangeAllPatterns() const;

// Overrides
public:

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool	CreateDialogPanels();
	bool	CreateToolbars();
	bool	CreateInstrumentToolbar();
	bool	CreateSampleWindow();
	void	OpenInstrumentSettings();
	void	SetSpeed(int Speed);
	void	SetTempo(int Tempo);
	void	SetRowCount(int Count);
	void	SetFrameCount(int Count);
	int		FindInstrument(int Index) const;
	int		GetInstrumentIndex(int ListIndex) const;

	void	UpdateMenu(CMenu *pMenu);
	void	SetFrameEditorPosition(int Position);
	void	SelectInstrumentFolder();

protected:  // control bar embedded members
	CStatusBar			m_wndStatusBar;
	CToolBar			m_wndToolBar;
	CReBar				m_wndToolBarReBar;
	CDialogReBar		m_wndOctaveBar;
	//CDialogBar			m_wndOctaveBar;
	CDialogBar			m_wndControlBar;	// Parent to frame editor and settings/instrument editor
	CDialogBar			m_wndVerticalControlBar;	// Parent to large frame editor
//	CControlPanelDlg	m_wndFrameBar;
	CControlPanelDlg	m_wndDialogBar;

	CControlPanelDlg	m_wndFrameControls;		// Contains +, - and change all

	CWnd				m_wndInstToolBarWnd;
	CToolBar			m_wndInstToolBar;
	CReBarCtrl			m_wndInstToolReBar;
	CInstrumentEditDlg	m_wndInstEdit;
//	CPerformanceDlg		m_wndPerformanceDlg;

	CFrameEditor		*m_pFrameEditor;
	CListCtrl			*m_pInstrumentList;
	CImageList			*m_pImageList;
	CSampleWindow		*m_pSampleWindow;

	CLockedEdit			*m_pLockedEditSpeed;
	CLockedEdit			*m_pLockedEditTempo;
	CLockedEdit			*m_pLockedEditLength;
	CLockedEdit			*m_pLockedEditFrames;
	CLockedEdit			*m_pLockedEditStep;

	CBannerEdit			*m_pBannerEditName;
	CBannerEdit			*m_pBannerEditArtist;
	CBannerEdit			*m_pBannerEditCopyright;

	CBitmap				m_bmToolbar;			// main toolbar
	CImageList			m_ilToolBar;

	CBitmap				m_bmInstToolbar;		// instrument toolbar
	CImageList			m_ilInstToolBar;

	int					m_iInstrumentIcons[8];

	CActionHandler		*m_pActionHandler;

	int					m_iFrameEditorPos;

	CInstrumentFileTree	*m_pInstrumentFileTree;

private:
	// State variables (to be used) TODO
	int					m_iInstrument;				// Selected instrument
	int					m_iTrack;					// Selected track

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnClickInstruments(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnChangedInstruments(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRClickInstruments(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnDblClkInstruments(NMHDR *pNMHDR, LRESULT *result);
	afx_msg void OnDeltaposTempoSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpeedSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRowsSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposFrameSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposKeyStepSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInstNameChange();
	afx_msg void OnTrackerKillsound();
	afx_msg void OnCreateNSF();
	afx_msg void OnCreateWAV();
	afx_msg void OnNextFrame();
	afx_msg void OnPrevFrame();
	afx_msg void OnChangeAll();
	afx_msg void OnBnClickedIncFrame();
	afx_msg void OnBnClickedDecFrame();
	afx_msg void OnKeyRepeat();
	afx_msg void OnEnKeyStepChange();
	afx_msg void OnHelpPerformance();
	afx_msg void OnUpdateSBTempo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBPosition(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBInstrument(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBOctave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBFrequency(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBChip(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKeyStepEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSpeedEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTempoEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRowsEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFramesEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKeyRepeat(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRemoveFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDuplicateFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModuleMoveframedown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModuleMoveframeup(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentRemove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentClone(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentDeepClone(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentLoad(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInstrumentSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNextSong(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePrevSong(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewControlpanel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHighlight(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditEnablemidi(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelectionEnabled(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileGeneralsettings();
	afx_msg void OnEnSongNameChange();
	afx_msg void OnEnSongArtistChange();
	afx_msg void OnEnSongCopyrightChange();
	afx_msg void OnFileImportmidi();
	afx_msg void OnFileImportText();
	afx_msg void OnFileExportText();
	afx_msg void OnEnKillfocusTempo();
	afx_msg void OnModuleInsertFrame();
	afx_msg void OnModuleRemoveFrame();
	afx_msg void OnModuleDuplicateFrame();
	afx_msg void OnModuleDuplicateFramePatterns();
	afx_msg void OnModuleChannels();
	afx_msg void OnModuleComments();
	afx_msg void OnModuleModuleproperties();
	afx_msg void OnModuleMoveframedown();
	afx_msg void OnModuleMoveframeup();
	afx_msg void OnLoadInstrument();
	afx_msg void OnSaveInstrument();
	afx_msg void OnEditInstrument();
	afx_msg void OnAddInstrument();
	afx_msg void OnRemoveInstrument();
	afx_msg void OnCloneInstrument();
	afx_msg void OnDeepCloneInstrument();
	afx_msg void OnBnClickedEditInst();
	afx_msg void OnCbnSelchangeSong();
	afx_msg void OnCbnSelchangeOctave();
	afx_msg void OnRemoveFocus();
	afx_msg void OnNextSong();
	afx_msg void OnPrevSong();
	afx_msg void OnTrackerSwitchToInstrument();
	afx_msg void OnUpdateTrackerSwitchToInstrument(CCmdUI *pCmdUI);
	afx_msg void OnClickedFollow();
	afx_msg void OnToggleFollow();
	afx_msg void OnViewControlpanel();
	afx_msg void OnTrackerDPCM();
	afx_msg void OnSelectPatternEditor();
	afx_msg void OnSelectFrameEditor();
	afx_msg void OnHelpEffecttable();
	afx_msg void OnDestroy();
	afx_msg void OnNextInstrument();
	afx_msg void OnPrevInstrument();
	afx_msg void OnNewInstrumentMenu( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnLoadInstrumentMenu(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnAddInstrument2A03();
	afx_msg void OnAddInstrumentVRC6();
	afx_msg void OnAddInstrumentVRC7();
	afx_msg void OnAddInstrumentFDS();
	afx_msg void OnAddInstrumentMMC5();
	afx_msg void OnAddInstrumentN163();
	afx_msg void OnAddInstrumentS5B();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectall();
	afx_msg void OnEditExpandpatterns();
	afx_msg void OnEditShrinkpatterns();
	afx_msg void OnEditEnableMIDI();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnDecayFast();
	afx_msg void OnDecaySlow();
	afx_msg void OnFrameeditorTop();
	afx_msg void OnFrameeditorLeft();
	afx_msg void OnUpdateFrameeditorTop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFrameeditorLeft(CCmdUI *pCmdUI);
	afx_msg void OnToggleSpeed();

public slots:
   void onIdleSlot();
   void toolBarAction_triggered(int id);
   void toolBarAction_newDocument();
   void toolBarAction_openDocument();
   void toolBarAction_saveDocument();
   void toolBarAction_editCut();
   void toolBarAction_editCopy();
   void toolBarAction_editPaste();
   void toolBarAction_about();
   void toolBarAction_help();
   void toolBarAction_addFrame();
   void toolBarAction_removeFrame();
   void toolBarAction_moveFrameDown();
   void toolBarAction_moveFrameUp();
   void toolBarAction_duplicateFrame();
   void toolBarAction_moduleProperties();
   void toolBarAction_play();
   void toolBarAction_playLoop();
   void toolBarAction_stop();
   void toolBarAction_editMode();
   void toolBarAction_previousTrack();
   void toolBarAction_nextTrack();
   void toolBarAction_settings();
   void toolBarAction_createNSF();
   void instToolBarAction_triggered(int id);
   void instToolBarAction_menu_aboutToShow(int id);
   void instToolBarAction_new();
   void instToolBarAction_remove();
   void instToolBarAction_clone();
   void instToolBarAction_load();
   void instToolBarAction_save();
   void instToolBarAction_edit();
   void setModified(bool modified);
   
signals:
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void editor_modificationChanged(bool m);
   void documentSaved();
   void documentClosed();
};

// Global DPI functions
int  SX(int pt);
int  SY(int pt);
void ScaleMouse(CPoint &pt);

// Global helpers
CString LoadDefaultFilter(LPCTSTR Name, LPCTSTR Ext);
