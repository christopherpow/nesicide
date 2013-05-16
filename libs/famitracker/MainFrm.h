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

#include "FamiTrackerView.h"
#include "FamiTrackerDoc.h"
#include "FrameEditor.h"
#include "PatternEditor.h"
#include "Action.h"
#include "InstrumentEditDlg.h"
#include "ControlPanelDlg.h"
#include "CustomControls.h"
#include "DialogReBar.h"

enum FRAME_EDIT_POS { FRAME_EDIT_POS_TOP, FRAME_EDIT_POS_LEFT };

class CSampleWindow;
class CInstrumentFileTree;

class CMainFrame : public CFrameWnd
{
   Q_OBJECT

public:
   CMainFrame(CWnd* parent = 0);
   ~CMainFrame();
   
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
   
   void OnPrevSong();
   void OnNextSong();

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

   void	SetHighlightRow(int Rows);
   void	SetSecondHighlightRow(int Rows);

   int		GetSelectedInstrument() const;
   int		GetSelectedTrack() const;

   bool	AddAction(CAction *pAction);
   CAction *GetLastAction() const;
   void	ResetUndo();
   
   void	UpdateMenus();

   void	SetSpeed(int Speed);
	void	SetTempo(int Tempo);
   void	SetRowCount(int Count);
	void	SetFrameCount(int Count);

protected:
   void focusInEvent(QFocusEvent *);
   void showEvent(QShowEvent *);
   void hideEvent(QHideEvent *);
   void resizeEvent(QResizeEvent *event);
   
   bool	CreateToolbars();
   bool	CreateInstrumentToolbar();
   bool	CreateDialogPanels();
   bool CreateSampleWindow();
   void	OpenInstrumentSettings();
   int		GetInstrumentIndex(int ListIndex) const;
   int		FindInstrument(int Index) const;

   void	UpdateMenu(CMenu *pMenu);
   void	SetFrameEditorPosition(int Position);
   
private:
   // Qt stuff
   QTimer* idleTimer;
   CFamiTrackerDoc* m_pDocument;
   CFamiTrackerView* m_pView;
   CSampleWindow		*m_pSampleWindow;
   QString m_fileName;
   bool initialized;
   
   CInstrumentEditDlg	m_wndInstEdit;
   CStatusBar			m_wndStatusBar;

   CBitmap				m_bmToolbar;			// main toolbar
	CImageList			m_ilToolBar;

	CBitmap				m_bmInstToolbar;		// instrument toolbar
	CImageList			m_ilInstToolBar;

	int					m_iInstrumentIcons[8];

   CActionHandler* m_pActionHandler;
   int					m_iFrameEditorPos;
   
   CInstrumentFileTree	*m_pInstrumentFileTree;
      
private:
	// State variables (to be used)
	int		m_iInstrument;				// Selected instrument
	int		m_iTrack;					// Selected track

   CFrameEditor		*m_pFrameEditor;
   CListCtrl			*m_pInstrumentList;
   CImageList			*m_pImageList;
   CToolBar			m_wndToolBar;
	CReBar				m_wndToolBarReBar;
   CDialogReBar		m_wndOctaveBar;
   CDialogBar			m_wndControlBar;	// Parent to frame editor and settings/instrument editor
	CDialogBar			m_wndVerticalControlBar;	// Parent to large frame editor
//	CControlPanelDlg	m_wndFrameBar;
	CControlPanelDlg	m_wndDialogBar;
   CWnd				m_wndInstToolBarWnd;
	CToolBar			m_wndInstToolBar;
	CReBarCtrl			m_wndInstToolReBar;

	CControlPanelDlg	m_wndFrameControls;		// Contains +, - and change all
   
   CLockedEdit			*m_pLockedEditSpeed;
	CLockedEdit			*m_pLockedEditTempo;
	CLockedEdit			*m_pLockedEditLength;
	CLockedEdit			*m_pLockedEditFrames;
	CLockedEdit			*m_pLockedEditStep;

	CBannerEdit			*m_pBannerEditName;
	CBannerEdit			*m_pBannerEditArtist;
	CBannerEdit			*m_pBannerEditCopyright;

public:
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnModuleChannels();
   afx_msg void OnModuleModuleproperties();
   afx_msg void OnModuleInsertFrame();
	afx_msg void OnModuleRemoveFrame();
	afx_msg void OnModuleDuplicateFrame();
	afx_msg void OnModuleDuplicateFramePatterns();
	afx_msg void OnModuleComments();
   afx_msg void OnCreateNSF();
	afx_msg void OnModuleMoveframedown();
	afx_msg void OnModuleMoveframeup();
   afx_msg void OnDblClkInstruments(NMHDR *pNMHDR, LRESULT *result);
   afx_msg void OnChangedInstruments(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnUpdateSBTempo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBPosition(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBInstrument(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBOctave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBFrequency(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSBChip(CCmdUI *pCmdUI);
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
   afx_msg void OnUpdateSpeedEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTempoEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRowsEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFramesEdit(CCmdUI *pCmdUI);
   afx_msg void OnUpdateHighlight(CCmdUI *pCmdUI);
   afx_msg void OnLoadInstrument();
	afx_msg void OnSaveInstrument();
	afx_msg void OnEditInstrument();
	afx_msg void OnAddInstrument();
	afx_msg void OnRemoveInstrument();
	afx_msg void OnCloneInstrument();
	afx_msg void OnDeepCloneInstrument();
   afx_msg void OnClickedFollow();
	afx_msg void OnToggleFollow();   
   afx_msg void OnCbnSelchangeOctave();
   afx_msg void OnDeltaposTempoSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpeedSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRowsSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposFrameSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposKeyStepSpin(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnChangeAll();
	afx_msg void OnBnClickedIncFrame();
	afx_msg void OnBnClickedDecFrame();
   
public slots:
   void idleProcessing();
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
   void instToolBarAction_new();
   void instToolBarAction_remove();
   void instToolBarAction_clone();
   void instToolBarAction_load();
   void instToolBarAction_save();
   void instToolBarAction_edit();
   void updateViews(long hint);
   void instruments_doubleClicked(const QModelIndex &index);
   void instruments_currentChanged(const QModelIndex& index,const QModelIndex&);
   void frameChangeAll_clicked();
   void frameInc_clicked();   
   void frameDec_clicked();
   void speedSpin_valueChanged(int arg1);
   void tempoSpin_valueChanged(int arg1);
   void rowsSpin_valueChanged(int arg1);
   void framesSpin_valueChanged(int arg1);
   void subtune_currentIndexChanged(int index);
   void songName_textEdited(const QString &arg1);   
   void songArtist_textEdited(const QString &arg1);
   void songCopyright_textEdited(const QString &arg1);
   void octave_currentIndexChanged(int);
   void follow_clicked();
   void highlight1_textChanged(QString);
   void highlightspin1_valueChanged(int);
   void highlight2_textChanged(QString);
   void highlightspin2_valueChanged(int);
   
signals:
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void editor_modificationChanged(bool m);
};

// Global DPI functions
int  SX(int pt);
int  SY(int pt);
void ScaleMouse(CPoint &pt);
