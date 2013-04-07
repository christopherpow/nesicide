#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QFrame>
#include <QToolBar>
#include <QLabel>
#include <QComboBox>

#include "FamiTrackerView.h"
#include "FamiTrackerDoc.h"
#include "FrameEditor.h"
#include "PatternEditor.h"
#include "Action.h"
#include "InstrumentEditDlg.h"
#include "ControlPanelDlg.h"
#include "CustomControls.h"

class CMainFrame;
typedef void (CMainFrame::*actionHandler)();
typedef void (CMainFrame::*uiUpdateHandler)(CCmdUI*);

enum FRAME_EDIT_POS { FRAME_EDIT_POS_TOP, FRAME_EDIT_POS_LEFT };

class CSampleWindow;

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
   bool	CreateDialogPanels();
   bool CreateSampleWindow();
   void	OpenInstrumentSettings();
   int		GetInstrumentIndex(int ListIndex) const;
   int		FindInstrument(int Index) const;

   void	SetFrameEditorPosition(int Position);
   
private:
   // Qt stuff
   QTimer* idleTimer;
   QToolBar* toolBar;
   QLabel* octaveLabel;
   QComboBox* octaveComboBox;
   QMap<QAction*,actionHandler> trackerActions;
   QMap<int,uiUpdateHandler> uiUpdateFuncs;
   CFamiTrackerDoc* m_pDocument;
   CFamiTrackerView* m_pView;
   CSampleWindow		*m_pSampleWindow;
   QString m_fileName;
   bool initialized;
   CInstrumentEditDlg	m_wndInstEdit;
   CStatusBar			m_wndStatusBar;

   CActionHandler* m_pActionHandler;
   int					m_iFrameEditorPos;
      
private:
	// State variables (to be used)
	int		m_iInstrument;				// Selected instrument
	int		m_iTrack;					// Selected track

   CFrameEditor		*m_pFrameEditor;
   CListCtrl			*m_pInstrumentList;
//   CToolBar			m_wndToolBar;
//	CReBar				m_wndToolBarReBar;
   CDialogBar			m_wndControlBar;	// Parent to frame editor and settings/instrument editor
	CDialogBar			m_wndVerticalControlBar;	// Parent to large frame editor
//	CControlPanelDlg	m_wndFrameBar;
	CControlPanelDlg	m_wndDialogBar;

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
//	afx_msg void OnModuleChannels();
//	afx_msg void OnModuleComments();
//	afx_msg void OnModuleModuleproperties();
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
   
public slots:
   void idleProcessing();
   void songInstruments_doubleClicked(const QModelIndex &index);
   void songInstruments_currentChanged(const QModelIndex& index,const QModelIndex&);
   void on_frameChangeAll_clicked(bool checked);
   void on_frameInc_clicked();   
   void on_frameDec_clicked();
   void on_speed_valueChanged(int arg1);
   void on_tempo_valueChanged(int arg1);
   void on_numRows_valueChanged(int NewRows);
   void on_numFrames_valueChanged(int NewFrames);
   void on_songs_currentIndexChanged(int index);
   void on_title_textEdited(const QString &arg1);   
   void on_author_textEdited(const QString &arg1);
   void on_copyright_textEdited(const QString &arg1);
   void trackerAction_triggered();
   void trackerAction_newDocument();
   void trackerAction_openDocument();
   void trackerAction_saveDocument();
   void trackerAction_editCut();
   void trackerAction_editCopy();
   void trackerAction_editPaste();
   void trackerAction_about();
   void trackerAction_help();
   void trackerAction_addFrame();
   void trackerAction_removeFrame();
   void trackerAction_moveFrameDown();
   void trackerAction_moveFrameUp();
   void trackerAction_duplicateFrame();
   void trackerAction_moduleProperties();
   void trackerAction_play();
   void trackerAction_playLoop();
   void trackerAction_stop();
   void trackerAction_editMode();
   void trackerAction_previousTrack();
   void trackerAction_nextTrack();
   void trackerAction_settings();
   void trackerAction_createNSF();
   void updateViews(long hint);
   
signals:
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void editor_modificationChanged(bool m);
};

// Global DPI functions
int  SX(int pt);
int  SY(int pt);
void ScaleMouse(CPoint &pt);

#endif // MAINFRAME_H
