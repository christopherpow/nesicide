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
** Any permitted reproduction of these routin, in whole or in part,
** must bear this legend.
*/

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "MainFrm.h"
#include "ExportDialog.h"
#include "CreateWaveDlg.h"
#include "InstrumentEditDlg.h"
#include "ModulePropertiesDlg.h"
#include "ChannelsDlg.h"
#include "SampleWindow.h"
#include "InstrumentEditor2A03.h"
#include "InstrumentEditorDPCM.h"
//#include "MidiImport.h"
#include "ConfigGeneral.h"
#include "ConfigAppearance.h"
#include "ConfigMIDI.h"
#include "ConfigSound.h"
#include "ConfigShortcuts.h"
#include "ConfigWindow.h"
//#include "ConfigLevels.h"
#include "Settings.h"
#include "Accelerator.h"
#include "SoundGen.h"
//#include "MIDI.h"
#include "TrackerChannel.h"
#include "CommentsDlg.h"
#include "InstrumentFileTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CHIP,
	ID_INDICATOR_INSTRUMENT, 
	ID_INDICATOR_OCTAVE,
	ID_INDICATOR_RATE,
	ID_INDICATOR_TEMPO,
	ID_INDICATOR_TIME,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// Timers
enum {TMR_WELCOME, TMR_SOUND_CHECK, TMR_AUTOSAVE};

// File filters (move to string table)
LPCTSTR FTI_FILTER_NAME = _T("FamiTracker Instrument (*.fti)");
LPCTSTR FTI_FILTER_EXT = _T(".fti");

// DPI variables
static const int DEFAULT_DPI = 96;
static int _dpiX, _dpiY;

// DPI scaling functions
int SX(int pt)
{
	return MulDiv(pt, _dpiX, DEFAULT_DPI);
}

int SY(int pt)
{
	return MulDiv(pt, _dpiY, DEFAULT_DPI);
}

void ScaleMouse(CPoint &pt)
{
	pt.x = MulDiv(pt.x, DEFAULT_DPI, _dpiX);
	pt.y = MulDiv(pt.y, DEFAULT_DPI, _dpiY);
}

// TODO: fix
//CDialogBar m_wndInstrumentBar;

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

// CMainFrame construction/destruction

CMainFrame::CMainFrame(CWnd *parent) :
   CFrameWnd(parent),
	m_pSampleWindow(NULL), 
	m_pFrameEditor(NULL),
	m_pImageList(NULL),
	m_pLockedEditSpeed(NULL),
	m_pLockedEditTempo(NULL),
	m_pLockedEditLength(NULL),
	m_pLockedEditFrames(NULL),
	m_pLockedEditStep(NULL),
	m_pBannerEditName(NULL),
	m_pBannerEditArtist(NULL),
	m_pBannerEditCopyright(NULL),
	m_pInstrumentList(NULL),
	m_iInstrument(0),
	m_iTrack(0),
	m_pActionHandler(NULL),
	m_pInstrumentFileTree(NULL),
   initialized(false)
{
   CREATESTRUCT cs;
   OnCreate(&cs);

	_dpiX = DEFAULT_DPI;
	_dpiY = DEFAULT_DPI;

	m_iFrameEditorPos = FRAME_EDIT_POS_TOP;
   
   idleTimer = new QTimer;
   QObject::connect(idleTimer,SIGNAL(timeout()),this,SLOT(idleProcessing()));
}

CMainFrame::~CMainFrame()
{
	SAFE_RELEASE(m_pImageList);
	SAFE_RELEASE(m_pLockedEditSpeed);
	SAFE_RELEASE(m_pLockedEditTempo);
	SAFE_RELEASE(m_pLockedEditLength);
	SAFE_RELEASE(m_pLockedEditFrames);
	SAFE_RELEASE(m_pLockedEditStep);
	SAFE_RELEASE(m_pBannerEditName);
	SAFE_RELEASE(m_pBannerEditArtist);
	SAFE_RELEASE(m_pBannerEditCopyright);
	SAFE_RELEASE(m_pFrameEditor);
	SAFE_RELEASE(m_pInstrumentList);
	SAFE_RELEASE(m_pSampleWindow);
	SAFE_RELEASE(m_pActionHandler);
	SAFE_RELEASE(m_pInstrumentFileTree);
   delete m_pView;
   delete m_pDocument;
}

//BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//	ON_WM_CREATE()
//	ON_WM_SIZE()
//	ON_WM_TIMER()
//	ON_WM_ERASEBKGND()
//	ON_WM_SHOWWINDOW()
//	ON_WM_DESTROY()
//	ON_WM_COPYDATA()
//	// Global help commands
//	ON_COMMAND(ID_FILE_GENERALSETTINGS, OnFileGeneralsettings)
//	ON_COMMAND(ID_FILE_IMPORTMIDI, OnFileImportmidi)
//	ON_COMMAND(ID_FILE_CREATE_NSF, OnCreateNSF)
//	ON_COMMAND(ID_FILE_CREATEWAV, OnCreateWAV)
//	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
//	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
//	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
//	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
//	ON_COMMAND(ID_EDIT_ENABLEMIDI, OnEditEnableMIDI)
//	ON_COMMAND(ID_EDIT_EXPANDPATTERNS, OnEditExpandpatterns)
//	ON_COMMAND(ID_EDIT_SHRINKPATTERNS, OnEditShrinkpatterns)
//	ON_COMMAND(ID_INSTRUMENT_NEW, OnAddInstrument)
//	ON_COMMAND(ID_INSTRUMENT_REMOVE, OnRemoveInstrument)
//	ON_COMMAND(ID_INSTRUMENT_CLONE, OnCloneInstrument)
//	ON_COMMAND(ID_INSTRUMENT_DEEPCLONE, OnDeepCloneInstrument)
//	ON_COMMAND(ID_INSTRUMENT_SAVE, OnSaveInstrument)
//	ON_COMMAND(ID_INSTRUMENT_LOAD, OnLoadInstrument)
//	ON_COMMAND(ID_INSTRUMENT_EDIT, OnEditInstrument)
//	ON_COMMAND(ID_INSTRUMENT_ADD_2A03, OnAddInstrument2A03)
//	ON_COMMAND(ID_INSTRUMENT_ADD_VRC6, OnAddInstrumentVRC6)
//	ON_COMMAND(ID_INSTRUMENT_ADD_VRC7, OnAddInstrumentVRC7)
//	ON_COMMAND(ID_INSTRUMENT_ADD_FDS, OnAddInstrumentFDS)
//	ON_COMMAND(ID_INSTRUMENT_ADD_MMC5, OnAddInstrumentMMC5)
//	ON_COMMAND(ID_INSTRUMENT_ADD_N163, OnAddInstrumentN163)
//	ON_COMMAND(ID_INSTRUMENT_ADD_S5B, OnAddInstrumentS5B)
//	ON_COMMAND(ID_MODULE_MODULEPROPERTIES, OnModuleModuleproperties)
//	ON_COMMAND(ID_MODULE_CHANNELS, OnModuleChannels)
//	ON_COMMAND(ID_MODULE_COMMENTS, OnModuleComments)
//	ON_COMMAND(ID_MODULE_INSERTFRAME, OnModuleInsertFrame)
//	ON_COMMAND(ID_MODULE_REMOVEFRAME, OnModuleRemoveFrame)
//	ON_COMMAND(ID_MODULE_DUPLICATEFRAME, OnModuleDuplicateFrame)
//	ON_COMMAND(ID_MODULE_DUPLICATEFRAMEPATTERNS, OnModuleDuplicateFramePatterns)
//	ON_COMMAND(ID_MODULE_MOVEFRAMEDOWN, OnModuleMoveframedown)
//	ON_COMMAND(ID_MODULE_MOVEFRAMEUP, OnModuleMoveframeup)
//	ON_COMMAND(ID_TRACKER_KILLSOUND, OnTrackerKillsound)
//	ON_COMMAND(ID_TRACKER_SWITCHTOTRACKINSTRUMENT, OnTrackerSwitchToInstrument)
//	ON_COMMAND(ID_TRACKER_DPCM, OnTrackerDPCM)
//	ON_COMMAND(ID_VIEW_CONTROLPANEL, OnViewControlpanel)
//	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
//	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
//	ON_COMMAND(ID_HELP_PERFORMANCE, OnHelpPerformance)
//	ON_COMMAND(ID_HELP_EFFECTTABLE, &CMainFrame::OnHelpEffecttable)
//	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
//	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)

//	ON_COMMAND(ID_FRAMEEDITOR_TOP, OnFrameeditorTop)
//	ON_COMMAND(ID_FRAMEEDITOR_LEFT, OnFrameeditorLeft)

//	ON_COMMAND(ID_NEXT_FRAME, OnNextFrame)
//	ON_COMMAND(ID_PREV_FRAME, OnPrevFrame)
//	ON_COMMAND(IDC_KEYREPEAT, OnKeyRepeat)
//	ON_COMMAND(IDC_CHANGE_ALL, OnChangeAll)
//	ON_COMMAND(ID_NEXT_SONG, OnNextSong)
//	ON_COMMAND(ID_PREV_SONG, OnPrevSong)
//	ON_COMMAND(IDC_FOLLOW_TOGGLE, OnToggleFollow)
//	ON_COMMAND(ID_FOCUS_PATTERN_EDITOR, OnSelectPatternEditor)
//	ON_COMMAND(ID_FOCUS_FRAME_EDITOR, OnSelectFrameEditor)
//	ON_COMMAND(ID_CMD_NEXT_INSTRUMENT, OnNextInstrument)
//	ON_COMMAND(ID_CMD_PREV_INSTRUMENT, OnPrevInstrument)
//	ON_COMMAND(ID_TOGGLE_SPEED, OnToggleSpeed)

//	ON_COMMAND(ID_DECAY_FAST, OnDecayFast)
//	ON_COMMAND(ID_DECAY_SLOW, OnDecaySlow)

//	ON_BN_CLICKED(IDC_FRAME_INC, OnBnClickedIncFrame)
//	ON_BN_CLICKED(IDC_FRAME_DEC, OnBnClickedDecFrame)
//	ON_BN_CLICKED(IDC_FOLLOW, OnClickedFollow)

//	ON_NOTIFY(NM_CLICK, IDC_INSTRUMENTS, OnClickInstruments)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INSTRUMENTS, OnChangedInstruments)
//	ON_NOTIFY(NM_DBLCLK, IDC_INSTRUMENTS, OnDblClkInstruments)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_SPEED_SPIN, OnDeltaposSpeedSpin)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_TEMPO_SPIN, OnDeltaposTempoSpin)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_ROWS_SPIN, OnDeltaposRowsSpin)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_FRAME_SPIN, OnDeltaposFrameSpin)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_KEYSTEP_SPIN, OnDeltaposKeyStepSpin)

//	ON_EN_CHANGE(IDC_INSTNAME, OnInstNameChange)
//	ON_EN_CHANGE(IDC_KEYSTEP, OnEnKeyStepChange)
//	ON_EN_CHANGE(IDC_SONG_NAME, OnEnSongNameChange)
//	ON_EN_CHANGE(IDC_SONG_ARTIST, OnEnSongArtistChange)
//	ON_EN_CHANGE(IDC_SONG_COPYRIGHT, OnEnSongCopyrightChange)
	
//	ON_EN_SETFOCUS(IDC_KEYREPEAT, OnRemoveFocus)

//	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_ENABLEMIDI, OnUpdateEditEnablemidi)
//	ON_UPDATE_COMMAND_UI(ID_MODULE_INSERTFRAME, OnUpdateInsertFrame)
//	ON_UPDATE_COMMAND_UI(ID_MODULE_REMOVEFRAME, OnUpdateRemoveFrame)
//	ON_UPDATE_COMMAND_UI(ID_MODULE_DUPLICATEFRAME, OnUpdateDuplicateFrame)
//	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEDOWN, OnUpdateModuleMoveframedown)
//	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEUP, OnUpdateModuleMoveframeup)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_NEW, OnUpdateInstrumentNew)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_REMOVE, OnUpdateInstrumentRemove)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_CLONE, OnUpdateInstrumentClone)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_DEEPCLONE, OnUpdateInstrumentDeepClone)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_EDIT, OnUpdateInstrumentEdit)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_LOAD, OnUpdateInstrumentLoad)
//	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_SAVE, OnUpdateInstrumentSave)
//	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INSTRUMENT, OnUpdateSBInstrument)
//	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OCTAVE, OnUpdateSBOctave)
//	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RATE, OnUpdateSBFrequency)
//	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TEMPO, OnUpdateSBTempo)
//	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CHIP, OnUpdateSBChip)
//	ON_UPDATE_COMMAND_UI(IDC_KEYSTEP, OnUpdateKeyStepEdit)
//	ON_UPDATE_COMMAND_UI(IDC_KEYREPEAT, OnUpdateKeyRepeat)
//	ON_UPDATE_COMMAND_UI(IDC_SPEED, OnUpdateSpeedEdit)
//	ON_UPDATE_COMMAND_UI(IDC_TEMPO, OnUpdateTempoEdit)
//	ON_UPDATE_COMMAND_UI(IDC_ROWS, OnUpdateRowsEdit)
//	ON_UPDATE_COMMAND_UI(IDC_FRAMES, OnUpdateFramesEdit)
//	ON_UPDATE_COMMAND_UI(ID_NEXT_SONG, OnUpdateNextSong)
//	ON_UPDATE_COMMAND_UI(ID_PREV_SONG, OnUpdatePrevSong)
//	ON_UPDATE_COMMAND_UI(ID_TRACKER_SWITCHTOTRACKINSTRUMENT, OnUpdateTrackerSwitchToInstrument)
//	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLPANEL, OnUpdateViewControlpanel)
//	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT1, OnUpdateHighlight)
//	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT2, OnUpdateHighlight)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPANDPATTERNS, OnUpdateSelectionEnabled)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_SHRINKPATTERNS, OnUpdateSelectionEnabled)

//	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_TOP, OnUpdateFrameeditorTop)
//	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_LEFT, OnUpdateFrameeditorLeft)

//	ON_CBN_SELCHANGE(IDC_SUBTUNE, OnCbnSelchangeSong)
//	ON_CBN_SELCHANGE(IDC_OCTAVE, OnCbnSelchangeOctave)
//END_MESSAGE_MAP()

void CMainFrame::focusInEvent(QFocusEvent *)
{
   m_pView->GetPatternView()->SetFocus(true);
   m_pFrameEditor->SetFocus();
   m_pView->GetPatternView()->setFocus();
}

void CMainFrame::showEvent(QShowEvent *)
{   
   if ( !initialized )
   {
      // Perform initialization that couldn't yet be done in the constructor due to MFC crap.
      m_pDocument = (CFamiTrackerDoc*)GetActiveDocument();
      
      m_pView = (CFamiTrackerView*)GetActiveView();
      
      m_pView->GetPatternView()->setParent(m_pView->toQWidget());
      m_pView->setVisible(false); // Hide the view otherwise it 'obscures' the real view.
      
      m_pView->GetPatternView()->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
      m_pView->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
      
      m_pView->setFocusPolicy(Qt::StrongFocus);
      m_pView->setFocusProxy(m_pView->GetPatternView());
      m_pView->GetPatternView()->setFocusPolicy(Qt::StrongFocus);
      m_pFrameEditor->setFocusPolicy(Qt::NoFocus);
      
      realCentralWidget->setLayout(m_pView->toQWidget()->layout());
      
      QObject::connect(m_pDocument,SIGNAL(updateViews(long)),m_pFrameEditor,SLOT(updateViews(long)));
      QObject::connect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
      QObject::connect(m_pMenu->GetSubMenu(0),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(1),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(2),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(3),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(4),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(5),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(6),SIGNAL(menuAction_triggered(int)),this,SLOT(menuAction_triggered(int)));
      QObject::connect(m_pMenu->GetSubMenu(0),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(1),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(2),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(3),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(4),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(5),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      QObject::connect(m_pMenu->GetSubMenu(6),SIGNAL(menuAboutToShow(CMenu*)),this,SLOT(menuAboutToShow(CMenu*)));
      
      // Connect buried signals.
      qDebug("START CONNECTING BURIED SIGNALS NOW...");
      QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));      
      QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SLOT(setModified(bool)));
      QObject::connect(&m_wndToolBar,SIGNAL(toolBarAction_triggered(int)),this,SLOT(toolBarAction_triggered(int)));
      QObject::connect(&m_wndInstToolBar,SIGNAL(toolBarAction_triggered(int)),this,SLOT(instToolBarAction_triggered(int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_OCTAVE)->toQWidget(),SIGNAL(currentIndexChanged(int)),this,SLOT(octave_currentIndexChanged(int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_FOLLOW)->toQWidget(),SIGNAL(clicked()),this,SLOT(follow_clicked()));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHTSPIN1),SIGNAL(valueChanged(int,int)),this,SLOT(highlightspin1_valueChanged(int,int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHTSPIN2),SIGNAL(valueChanged(int,int)),this,SLOT(highlightspin2_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->toQWidget(),SIGNAL(doubleClicked(QModelIndex)),this,SLOT(instruments_doubleClicked(QModelIndex)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->toQWidget(),SIGNAL(currentRowChanged(int)),this,SLOT(instruments_currentRowChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SPEED_SPIN),SIGNAL(valueChanged(int,int)),this,SLOT(speedSpin_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_TEMPO_SPIN),SIGNAL(valueChanged(int,int)),this,SLOT(tempoSpin_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_ROWS_SPIN),SIGNAL(valueChanged(int,int)),this,SLOT(rowsSpin_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_FRAME_SPIN),SIGNAL(valueChanged(int,int)),this,SLOT(framesSpin_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_KEYSTEP_SPIN),SIGNAL(valueChanged(int,int)),this,SLOT(keyStepSpin_valueChanged(int,int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SUBTUNE)->toQWidget(),SIGNAL(currentIndexChanged(int)),this,SLOT(subtune_currentIndexChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_NAME)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songName_textEdited(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_ARTIST)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songArtist_textEdited(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_COPYRIGHT)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songCopyright_textEdited(QString)));
      QObject::connect(m_wndFrameControls.GetDlgItem(IDC_FRAME_INC)->toQWidget(),SIGNAL(clicked()),this,SLOT(frameInc_clicked()));
      QObject::connect(m_wndFrameControls.GetDlgItem(IDC_FRAME_DEC)->toQWidget(),SIGNAL(clicked()),this,SLOT(frameDec_clicked()));
      QObject::connect(m_wndFrameControls.GetDlgItem(IDC_CHANGE_ALL)->toQWidget(),SIGNAL(clicked()),this,SLOT(frameChangeAll_clicked()));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(instName_textEdited(QString)));
      qDebug("DONE CONNECTING BURIED SIGNALS NOW...");
      
      
      // CP: If I don't do this here the pattern editor takes up the whole window at start until first resize.
      //     Not sure why...yet.
      RecalcLayout();
      
      initialized = true;
   }
   
   idleTimer->start();

   setFocus();
}

void CMainFrame::hideEvent(QHideEvent *)
{
   idleTimer->stop();
}

void CMainFrame::resizeEvent(QResizeEvent *event)
{
   OnSize(0,event->size().width(),event->size().height());
}

void CMainFrame::timerEvent(QTimerEvent *event)
{
   int mfcId = mfcTimerId(event->timerId());
   OnTimer(mfcId);
}

void CMainFrame::idleProcessing()
{   
   CCmdUI cmdUI;
   
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   cmdUI.m_nID = ID_EDIT_COPY;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
   }
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   cmdUI.m_nID = ID_EDIT_PASTE;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
   }
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_ENABLEMIDI, OnUpdateEditEnablemidi)
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_INSERTFRAME, OnUpdateInsertFrame)
   cmdUI.m_nID = ID_MODULE_INSERTFRAME;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInsertFrame(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_REMOVEFRAME, OnUpdateRemoveFrame)
   cmdUI.m_nID = ID_MODULE_REMOVEFRAME;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateRemoveFrame(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_DUPLICATEFRAME, OnUpdateDuplicateFrame)
   cmdUI.m_nID = ID_MODULE_DUPLICATEFRAME;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateDuplicateFrame(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEDOWN, OnUpdateModuleMoveframedown)
   cmdUI.m_nID = ID_MODULE_MOVEFRAMEDOWN;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateModuleMoveframedown(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEUP, OnUpdateModuleMoveframeup)
   cmdUI.m_nID = ID_MODULE_MOVEFRAMEUP;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateModuleMoveframeup(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_NEW, OnUpdateInstrumentNew)
   cmdUI.m_nID = ID_INSTRUMENT_NEW;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentNew(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_REMOVE, OnUpdateInstrumentRemove)
   cmdUI.m_nID = ID_INSTRUMENT_REMOVE;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentRemove(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_CLONE, OnUpdateInstrumentClone)
   cmdUI.m_nID = ID_INSTRUMENT_CLONE;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentClone(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_DEEPCLONE, OnUpdateInstrumentDeepClone)
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_EDIT, OnUpdateInstrumentEdit)
   cmdUI.m_nID = ID_INSTRUMENT_EDIT;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_LOAD, OnUpdateInstrumentLoad)
   cmdUI.m_nID = ID_INSTRUMENT_LOAD;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentLoad(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_SAVE, OnUpdateInstrumentSave)
   cmdUI.m_nID = ID_INSTRUMENT_SAVE;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateInstrumentSave(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INSTRUMENT, OnUpdateSBInstrument)
   cmdUI.m_nID = ID_INDICATOR_INSTRUMENT;
   cmdUI.m_pOther = &m_wndStatusBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBInstrument(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OCTAVE, OnUpdateSBOctave)
   cmdUI.m_nID = ID_INDICATOR_OCTAVE;
   cmdUI.m_pOther = &m_wndStatusBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBOctave(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RATE, OnUpdateSBFrequency)
   cmdUI.m_nID = ID_INDICATOR_RATE;
   cmdUI.m_pOther = &m_wndStatusBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBFrequency(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TEMPO, OnUpdateSBTempo)
   cmdUI.m_nID = ID_INDICATOR_TEMPO;
   cmdUI.m_pOther = &m_wndStatusBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBTempo(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CHIP, OnUpdateSBChip)
   cmdUI.m_nID = ID_INDICATOR_CHIP;
   cmdUI.m_pOther = &m_wndStatusBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBChip(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_KEYSTEP, OnUpdateKeyStepEdit)
   cmdUI.m_nID = IDC_KEYSTEP;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateKeyStepEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_KEYREPEAT, OnUpdateKeyRepeat)
   cmdUI.m_nID = IDC_KEYREPEAT;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateKeyRepeat(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_SPEED, OnUpdateSpeedEdit)
   cmdUI.m_nID = IDC_SPEED;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSpeedEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_TEMPO, OnUpdateTempoEdit)
   cmdUI.m_nID = IDC_TEMPO;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateTempoEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_ROWS, OnUpdateRowsEdit)
   cmdUI.m_nID = IDC_ROWS;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateRowsEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_FRAMES, OnUpdateFramesEdit)
   cmdUI.m_nID = IDC_FRAMES;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateFramesEdit(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_NEXT_SONG, OnUpdateNextSong)
   cmdUI.m_nID = ID_NEXT_SONG;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdateNextSong(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_PREV_SONG, OnUpdatePrevSong)
   cmdUI.m_nID = ID_PREV_SONG;
   cmdUI.m_pOther = &m_wndToolBar;
   if ( cmdUI.m_pOther )
   {
      OnUpdatePrevSong(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_TRACKER_SWITCHTOTRACKINSTRUMENT, OnUpdateTrackerSwitchToInstrument)
   //	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLPANEL, OnUpdateViewControlpanel)
   //	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT1, OnUpdateHighlight)
   cmdUI.m_nID = IDC_HIGHLIGHT1;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT2, OnUpdateHighlight)
   cmdUI.m_nID = IDC_HIGHLIGHT2;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPANDPATTERNS, OnUpdateSelectionEnabled)
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_SHRINKPATTERNS, OnUpdateSelectionEnabled)
   
   //	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_TOP, OnUpdateFrameeditorTop)
   //	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_LEFT, OnUpdateFrameeditorLeft)
}

void CMainFrame::updateViews(long hint)
{
}

void CMainFrame::setModified(bool modified)
{
}

void CMainFrame::toolBarAction_triggered(int id)
{
   typedef void (CMainFrame::*actionHandler)();
   actionHandler actionHandlers[] =
   {
      &CMainFrame::toolBarAction_newDocument,
      &CMainFrame::toolBarAction_openDocument,
      &CMainFrame::toolBarAction_saveDocument,
      &CMainFrame::toolBarAction_editCut,
      &CMainFrame::toolBarAction_editCopy,
      &CMainFrame::toolBarAction_editPaste,
      &CMainFrame::toolBarAction_about,
      &CMainFrame::toolBarAction_help,
      &CMainFrame::toolBarAction_addFrame,
      &CMainFrame::toolBarAction_removeFrame,
      &CMainFrame::toolBarAction_moveFrameDown,
      &CMainFrame::toolBarAction_moveFrameUp,
      &CMainFrame::toolBarAction_duplicateFrame,
      &CMainFrame::toolBarAction_moduleProperties,
      &CMainFrame::toolBarAction_play,
      &CMainFrame::toolBarAction_playLoop,
      &CMainFrame::toolBarAction_stop,
      &CMainFrame::toolBarAction_editMode,
      &CMainFrame::toolBarAction_previousTrack,
      &CMainFrame::toolBarAction_nextTrack,
      &CMainFrame::toolBarAction_settings,
      &CMainFrame::toolBarAction_createNSF
   };
   if ( id >= 0 )
   {
      (this->*((actionHandlers[id])))();
   }
}

void CMainFrame::toolBarAction_newDocument()
{
   ((CFamiTrackerApp*)AfxGetApp())->OnFileNew();
}

void CMainFrame::toolBarAction_openDocument()
{
   ((CFamiTrackerApp*)AfxGetApp())->OnFileOpen();
}

void CMainFrame::toolBarAction_saveDocument()
{
   qDebug("saveDocument");
}

void CMainFrame::toolBarAction_editCut()
{
   qDebug("editCut");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditCut();
}

void CMainFrame::toolBarAction_editCopy()
{
   qDebug("editCopy");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditCopy();
}

void CMainFrame::toolBarAction_editPaste()
{
   qDebug("editPaste");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditPaste();   
}

void CMainFrame::toolBarAction_about()
{
   qDebug("about");
}

void CMainFrame::toolBarAction_help()
{
   qDebug("help");
}

void CMainFrame::toolBarAction_addFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_ADD));
}

void CMainFrame::toolBarAction_removeFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_REMOVE));
}

void CMainFrame::toolBarAction_moveFrameDown()
{
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_DOWN));
}

void CMainFrame::toolBarAction_moveFrameUp()
{
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_UP));
}

void CMainFrame::toolBarAction_duplicateFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_DUPLICATE));
}

void CMainFrame::toolBarAction_moduleProperties()
{
   qDebug("moduleProperties");
   OnModuleModuleproperties();
}

void CMainFrame::toolBarAction_play()
{
   qDebug("play");
   theApp.OnTrackerPlay();
}

void CMainFrame::toolBarAction_playLoop()
{
   qDebug("playLoop");
   theApp.OnTrackerPlaypattern();
}

void CMainFrame::toolBarAction_stop()
{
   qDebug("stop");
   theApp.OnTrackerStop();
}

void CMainFrame::toolBarAction_editMode()
{
   qDebug("editMode");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnTrackerEdit();
}

void CMainFrame::toolBarAction_previousTrack()
{
   OnPrevSong();
}

void CMainFrame::toolBarAction_nextTrack()
{
   OnNextSong();
}

void CMainFrame::toolBarAction_settings()
{
   OnFileGeneralsettings();
}

void CMainFrame::toolBarAction_createNSF()
{
   OnCreateNSF();
}

void CMainFrame::instToolBarAction_triggered(int id)
{
   typedef void (CMainFrame::*actionHandler)();
   actionHandler actionHandlers[] =
   {
      &CMainFrame::instToolBarAction_new,
      &CMainFrame::instToolBarAction_remove,
      &CMainFrame::instToolBarAction_clone,
      &CMainFrame::instToolBarAction_load,
      &CMainFrame::instToolBarAction_save,
      &CMainFrame::instToolBarAction_edit,
   };
   if ( id >= 0 )
   {
      (this->*((actionHandlers[id])))();
   }
}

void CMainFrame::instToolBarAction_new()
{
   OnAddInstrument();
}

void CMainFrame::instToolBarAction_remove()
{
   OnRemoveInstrument();
}

void CMainFrame::instToolBarAction_clone()
{
   OnCloneInstrument();
}

void CMainFrame::instToolBarAction_load()
{
   OnLoadInstrument();
}

void CMainFrame::instToolBarAction_save()
{
   OnSaveInstrument();
}

void CMainFrame::instToolBarAction_edit()
{
   OnEditInstrument();
}

void CMainFrame::frameInc_clicked()
{
   OnBnClickedIncFrame();
}

void CMainFrame::frameDec_clicked()
{
   OnBnClickedDecFrame();
}

void CMainFrame::frameChangeAll_clicked()
{
   OnChangeAll();
}

void CMainFrame::instruments_currentRowChanged(int row)
{
   NM_LISTVIEW nmlv;
   LRESULT result;
   
   nmlv.uNewState = LVIS_SELECTED;
   nmlv.iItem = row;
   OnChangedInstruments((NMHDR*)&nmlv,&result);
}

void CMainFrame::instruments_doubleClicked(const QModelIndex &index)
{
   NMHDR nmhdr;
   LRESULT result;
   OnDblClkInstruments(&nmhdr,&result);
}

void CMainFrame::subtune_currentIndexChanged(int index)
{
   OnCbnSelchangeSong();
}

void CMainFrame::songName_textEdited(const QString &arg1)
{
   OnEnSongNameChange();
}

void CMainFrame::songArtist_textEdited(const QString &arg1)
{
   OnEnSongArtistChange();
}

void CMainFrame::songCopyright_textEdited(const QString &arg1)
{
   OnEnSongCopyrightChange();
}

void CMainFrame::instName_textEdited(const QString &arg1)
{
   OnInstNameChange();
}

void CMainFrame::follow_clicked()
{
   OnClickedFollow();
}

void CMainFrame::octave_currentIndexChanged(int)
{
   OnCbnSelchangeOctave();
}

void CMainFrame::highlightspin1_valueChanged(int arg1, int arg2)
{
}

void CMainFrame::highlightspin2_valueChanged(int arg1, int arg2)
{
}

void CMainFrame::speedSpin_valueChanged(int arg1, int arg2)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg2;
   nmud.iDelta = arg1-arg2;
   OnDeltaposSpeedSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::tempoSpin_valueChanged(int arg1, int arg2)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg2;
   nmud.iDelta = arg1-arg2;
   OnDeltaposTempoSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::rowsSpin_valueChanged(int arg1, int arg2)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg2;
   nmud.iDelta = arg1-arg2;
   OnDeltaposRowsSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::framesSpin_valueChanged(int arg1, int arg2)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg2;
   nmud.iDelta = arg1-arg2;
   OnDeltaposFrameSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::keyStepSpin_valueChanged(int arg1, int arg2)
{
   OnEnKeyStepChange();
}

void CMainFrame::menuAboutToShow(CMenu* menu)
{
   CCmdUI cmdUI;
   cmdUI.m_pMenu = menu;

   // Pass to view class first.
   m_pView->menuAboutToShow(menu);
   
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
   cmdUI.m_nID = ID_EDIT_UNDO;
   OnUpdateEditUndo(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
   cmdUI.m_nID = ID_EDIT_REDO;
   OnUpdateEditRedo(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   cmdUI.m_nID = ID_EDIT_COPY;
   OnUpdateEditCopy(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   cmdUI.m_nID = ID_EDIT_PASTE;
   OnUpdateEditPaste(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_ENABLEMIDI, OnUpdateEditEnablemidi)
   cmdUI.m_nID = ID_EDIT_ENABLEMIDI;
   OnUpdateEditEnablemidi(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_INSERTFRAME, OnUpdateInsertFrame)
   cmdUI.m_nID = ID_MODULE_INSERTFRAME;
   OnUpdateInsertFrame(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_REMOVEFRAME, OnUpdateRemoveFrame)
   cmdUI.m_nID = ID_MODULE_REMOVEFRAME;
   OnUpdateRemoveFrame(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_DUPLICATEFRAME, OnUpdateDuplicateFrame)
   cmdUI.m_nID = ID_MODULE_DUPLICATEFRAME;
   OnUpdateDuplicateFrame(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEDOWN, OnUpdateModuleMoveframedown)
   cmdUI.m_nID = ID_MODULE_MOVEFRAMEDOWN;
   OnUpdateModuleMoveframedown(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_MODULE_MOVEFRAMEUP, OnUpdateModuleMoveframeup)
   cmdUI.m_nID = ID_MODULE_MOVEFRAMEUP;
   OnUpdateModuleMoveframeup(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_NEW, OnUpdateInstrumentNew)
   cmdUI.m_nID = ID_INSTRUMENT_NEW;
   OnUpdateInstrumentNew(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_REMOVE, OnUpdateInstrumentRemove)
   cmdUI.m_nID = ID_INSTRUMENT_REMOVE;
   OnUpdateInstrumentRemove(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_CLONE, OnUpdateInstrumentClone)
   cmdUI.m_nID = ID_INSTRUMENT_CLONE;
   OnUpdateInstrumentClone(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_DEEPCLONE, OnUpdateInstrumentDeepClone)
   cmdUI.m_nID = ID_INSTRUMENT_DEEPCLONE;
   OnUpdateInstrumentDeepClone(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_EDIT, OnUpdateInstrumentEdit)
   cmdUI.m_nID = ID_INSTRUMENT_EDIT;
   OnUpdateInstrumentEdit(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_LOAD, OnUpdateInstrumentLoad)
   cmdUI.m_nID = ID_INSTRUMENT_LOAD;
   OnUpdateInstrumentLoad(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INSTRUMENT_SAVE, OnUpdateInstrumentSave)
   cmdUI.m_nID = ID_INSTRUMENT_SAVE;
   OnUpdateInstrumentSave(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_INSTRUMENT, OnUpdateSBInstrument)
   cmdUI.m_nID = ID_INDICATOR_INSTRUMENT;
   OnUpdateSBInstrument(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OCTAVE, OnUpdateSBOctave)
   cmdUI.m_nID = ID_INDICATOR_OCTAVE;
   OnUpdateSBOctave(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RATE, OnUpdateSBFrequency)
   cmdUI.m_nID = ID_INDICATOR_RATE;
   OnUpdateSBFrequency(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TEMPO, OnUpdateSBTempo)
   cmdUI.m_nID = ID_INDICATOR_TEMPO;
   OnUpdateSBTempo(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CHIP, OnUpdateSBChip)
   cmdUI.m_nID = ID_INDICATOR_CHIP;
   OnUpdateSBChip(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_KEYREPEAT, OnUpdateKeyRepeat)
   cmdUI.m_nID = IDC_KEYREPEAT;
   OnUpdateKeyRepeat(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_SPEED, OnUpdateSpeedEdit)
   cmdUI.m_nID = IDC_SPEED;
   OnUpdateSpeedEdit(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_TEMPO, OnUpdateTempoEdit)
   cmdUI.m_nID = IDC_TEMPO;
   OnUpdateTempoEdit(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_ROWS, OnUpdateRowsEdit)
   cmdUI.m_nID = IDC_ROWS;
   OnUpdateRowsEdit(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_FRAMES, OnUpdateFramesEdit)
   cmdUI.m_nID = IDC_FRAMES;
   OnUpdateFramesEdit(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_NEXT_SONG, OnUpdateNextSong)
   cmdUI.m_nID = ID_NEXT_SONG;
   OnUpdateNextSong(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_PREV_SONG, OnUpdatePrevSong)
   cmdUI.m_nID = ID_PREV_SONG;
   OnUpdatePrevSong(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_TRACKER_SWITCHTOTRACKINSTRUMENT, OnUpdateTrackerSwitchToInstrument)
   cmdUI.m_nID = ID_TRACKER_SWITCHTOTRACKINSTRUMENT;
   OnUpdateTrackerSwitchToInstrument(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLPANEL, OnUpdateViewControlpanel)
   cmdUI.m_nID = ID_VIEW_CONTROLPANEL;
   OnUpdateViewControlpanel(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT1, OnUpdateHighlight)
   cmdUI.m_nID = IDC_HIGHLIGHT1;
   OnUpdateHighlight(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(IDC_HIGHLIGHT2, OnUpdateHighlight)
   cmdUI.m_nID = IDC_HIGHLIGHT2;
   OnUpdateHighlight(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPANDPATTERNS, OnUpdateSelectionEnabled)
   cmdUI.m_nID = ID_EDIT_EXPANDPATTERNS;
   OnUpdateSelectionEnabled(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_EDIT_SHRINKPATTERNS, OnUpdateSelectionEnabled)
   cmdUI.m_nID = ID_EDIT_SHRINKPATTERNS;
   OnUpdateSelectionEnabled(&cmdUI);
   
   //	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_TOP, OnUpdateFrameeditorTop)
   cmdUI.m_nID = ID_FRAMEEDITOR_TOP;
   OnUpdateFrameeditorTop(&cmdUI);
   //	ON_UPDATE_COMMAND_UI(ID_FRAMEEDITOR_LEFT, OnUpdateFrameeditorLeft)
   cmdUI.m_nID = ID_FRAMEEDITOR_LEFT;
   OnUpdateFrameeditorLeft(&cmdUI);
   
   // Pass to app next.
   ((CFamiTrackerApp*)AfxGetApp())->menuAboutToShow(menu);
}

void CMainFrame::menuAction_triggered(int id)
{
   typedef void (CMainFrame::*actionHandler)();
   QHash<UINT_PTR,actionHandler> actionHandlers;
   //	ON_COMMAND(ID_FILE_GENERALSETTINGS, OnFileGeneralsettings)
   actionHandlers.insert(ID_FILE_GENERALSETTINGS,&CMainFrame::OnFileGeneralsettings);
   //	ON_COMMAND(ID_FILE_IMPORTMIDI, OnFileImportmidi)
   actionHandlers.insert(ID_FILE_IMPORTMIDI,&CMainFrame::OnFileImportmidi);
   //	ON_COMMAND(ID_FILE_CREATE_NSF, OnCreateNSF)
   actionHandlers.insert(ID_FILE_CREATE_NSF,&CMainFrame::OnCreateNSF);
   //	ON_COMMAND(ID_FILE_CREATEWAV, OnCreateWAV)
   actionHandlers.insert(ID_FILE_CREATEWAV,&CMainFrame::OnCreateWAV);
   //	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
   actionHandlers.insert(ID_EDIT_UNDO,&CMainFrame::OnEditUndo);
   //	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
   actionHandlers.insert(ID_EDIT_REDO,&CMainFrame::OnEditRedo);
   //	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   actionHandlers.insert(ID_EDIT_COPY,&CMainFrame::OnEditCopy);
   //	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
   actionHandlers.insert(ID_EDIT_PASTE,&CMainFrame::OnEditPaste);
   //	ON_COMMAND(ID_EDIT_ENABLEMIDI, OnEditEnableMIDI)
   actionHandlers.insert(ID_EDIT_ENABLEMIDI,&CMainFrame::OnEditEnableMIDI);
   //	ON_COMMAND(ID_EDIT_EXPANDPATTERNS, OnEditExpandpatterns)
   actionHandlers.insert(ID_EDIT_EXPANDPATTERNS,&CMainFrame::OnEditExpandpatterns);
   //	ON_COMMAND(ID_EDIT_SHRINKPATTERNS, OnEditShrinkpatterns)
   actionHandlers.insert(ID_EDIT_SHRINKPATTERNS,&CMainFrame::OnEditShrinkpatterns);
   //	ON_COMMAND(ID_INSTRUMENT_NEW, OnAddInstrument)
   actionHandlers.insert(ID_INSTRUMENT_NEW,&CMainFrame::OnAddInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_REMOVE, OnRemoveInstrument)
   actionHandlers.insert(ID_INSTRUMENT_REMOVE,&CMainFrame::OnRemoveInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_CLONE, OnCloneInstrument)
   actionHandlers.insert(ID_INSTRUMENT_CLONE,&CMainFrame::OnCloneInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_DEEPCLONE, OnDeepCloneInstrument)
   actionHandlers.insert(ID_INSTRUMENT_DEEPCLONE,&CMainFrame::OnDeepCloneInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_SAVE, OnSaveInstrument)
   actionHandlers.insert(ID_INSTRUMENT_SAVE,&CMainFrame::OnSaveInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_LOAD, OnLoadInstrument)
   actionHandlers.insert(ID_INSTRUMENT_LOAD,&CMainFrame::OnLoadInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_EDIT, OnEditInstrument)
   actionHandlers.insert(ID_INSTRUMENT_EDIT,&CMainFrame::OnEditInstrument);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_2A03, OnAddInstrument2A03)
   actionHandlers.insert(ID_INSTRUMENT_ADD_2A03,&CMainFrame::OnAddInstrument2A03);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_VRC6, OnAddInstrumentVRC6)
   actionHandlers.insert(ID_INSTRUMENT_ADD_VRC6,&CMainFrame::OnAddInstrumentVRC6);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_VRC7, OnAddInstrumentVRC7)
   actionHandlers.insert(ID_INSTRUMENT_ADD_VRC7,&CMainFrame::OnAddInstrumentVRC7);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_FDS, OnAddInstrumentFDS)
   actionHandlers.insert(ID_INSTRUMENT_ADD_FDS,&CMainFrame::OnAddInstrumentFDS);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_MMC5, OnAddInstrumentMMC5)
   actionHandlers.insert(ID_INSTRUMENT_ADD_MMC5,&CMainFrame::OnAddInstrumentMMC5);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_N163, OnAddInstrumentN163)
   actionHandlers.insert(ID_INSTRUMENT_ADD_N163,&CMainFrame::OnAddInstrumentN163);
   //	ON_COMMAND(ID_INSTRUMENT_ADD_S5B, OnAddInstrumentS5B)
   actionHandlers.insert(ID_INSTRUMENT_ADD_S5B,&CMainFrame::OnAddInstrumentS5B);
   //	ON_COMMAND(ID_MODULE_MODULEPROPERTIES, OnModuleModuleproperties)
   actionHandlers.insert(ID_MODULE_MODULEPROPERTIES,&CMainFrame::OnModuleModuleproperties);
   //	ON_COMMAND(ID_MODULE_CHANNELS, OnModuleChannels)
   actionHandlers.insert(ID_MODULE_CHANNELS,&CMainFrame::OnModuleChannels);
   //	ON_COMMAND(ID_MODULE_COMMENTS, OnModuleComments)
   actionHandlers.insert(ID_MODULE_COMMENTS,&CMainFrame::OnModuleComments);
   //	ON_COMMAND(ID_MODULE_INSERTFRAME, OnModuleInsertFrame)
   actionHandlers.insert(ID_MODULE_INSERTFRAME,&CMainFrame::OnModuleInsertFrame);
   //	ON_COMMAND(ID_MODULE_REMOVEFRAME, OnModuleRemoveFrame)
   actionHandlers.insert(ID_MODULE_REMOVEFRAME,&CMainFrame::OnModuleRemoveFrame);
   //	ON_COMMAND(ID_MODULE_DUPLICATEFRAME, OnModuleDuplicateFrame)
   actionHandlers.insert(ID_MODULE_DUPLICATEFRAME,&CMainFrame::OnModuleDuplicateFrame);
   //	ON_COMMAND(ID_MODULE_DUPLICATEFRAMEPATTERNS, OnModuleDuplicateFramePatterns)
   actionHandlers.insert(ID_MODULE_DUPLICATEFRAMEPATTERNS,&CMainFrame::OnModuleDuplicateFramePatterns);
   //	ON_COMMAND(ID_MODULE_MOVEFRAMEDOWN, OnModuleMoveframedown)
   actionHandlers.insert(ID_MODULE_MOVEFRAMEDOWN,&CMainFrame::OnModuleMoveframedown);
   //	ON_COMMAND(ID_MODULE_MOVEFRAMEUP, OnModuleMoveframeup)
   actionHandlers.insert(ID_MODULE_MOVEFRAMEUP,&CMainFrame::OnModuleMoveframeup);
   //	ON_COMMAND(ID_TRACKER_KILLSOUND, OnTrackerKillsound)
   actionHandlers.insert(ID_TRACKER_KILLSOUND,&CMainFrame::OnTrackerKillsound);
   //	ON_COMMAND(ID_TRACKER_SWITCHTOTRACKINSTRUMENT, OnTrackerSwitchToInstrument)
   actionHandlers.insert(ID_TRACKER_SWITCHTOTRACKINSTRUMENT,&CMainFrame::OnTrackerSwitchToInstrument);
   //	ON_COMMAND(ID_TRACKER_DPCM, OnTrackerDPCM)
   actionHandlers.insert(ID_TRACKER_DPCM,&CMainFrame::OnTrackerDPCM);
   //	ON_COMMAND(ID_VIEW_CONTROLPANEL, OnViewControlpanel)
   actionHandlers.insert(ID_VIEW_CONTROLPANEL,&CMainFrame::OnViewControlpanel);
//   //	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
//   actionHandlers.insert(ID_HELP,CFrameWnd::OnHelp);
//   //	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
//   actionHandlers.insert(ID_HELP_FINDER,CFrameWnd::OnHelpFinder);
   //	ON_COMMAND(ID_HELP_PERFORMANCE, OnHelpPerformance)
   actionHandlers.insert(ID_HELP_PERFORMANCE,&CMainFrame::OnHelpPerformance);
   //	ON_COMMAND(ID_HELP_EFFECTTABLE, &CMainFrame::OnHelpEffecttable)
   actionHandlers.insert(ID_HELP_EFFECTTABLE,&CMainFrame::OnHelpEffecttable);
//   //	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
//   actionHandlers.insert(ID_DEFAULT_HELP,CFrameWnd::OnHelpFinder);
//   //	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
//   actionHandlers.insert(ID_CONTEXT_HELP,CFrameWnd::OnContextHelp);
   
   //	ON_COMMAND(ID_FRAMEEDITOR_TOP, OnFrameeditorTop)
   actionHandlers.insert(ID_FRAMEEDITOR_TOP,&CMainFrame::OnFrameeditorTop);
   //	ON_COMMAND(ID_FRAMEEDITOR_LEFT, OnFrameeditorLeft)
   actionHandlers.insert(ID_FRAMEEDITOR_LEFT,&CMainFrame::OnFrameeditorLeft);
   
   //	ON_COMMAND(ID_NEXT_FRAME, OnNextFrame)
   actionHandlers.insert(ID_NEXT_FRAME,&CMainFrame::OnNextFrame);
   //	ON_COMMAND(ID_PREV_FRAME, OnPrevFrame)
   actionHandlers.insert(ID_PREV_FRAME,&CMainFrame::OnPrevFrame);
   //	ON_COMMAND(IDC_KEYREPEAT, OnKeyRepeat)
   actionHandlers.insert(IDC_KEYREPEAT,&CMainFrame::OnKeyRepeat);
   //	ON_COMMAND(IDC_CHANGE_ALL, OnChangeAll)
   actionHandlers.insert(IDC_CHANGE_ALL,&CMainFrame::OnChangeAll);
   //	ON_COMMAND(ID_NEXT_SONG, OnNextSong)
   actionHandlers.insert(ID_NEXT_SONG,&CMainFrame::OnNextSong);
   //	ON_COMMAND(ID_PREV_SONG, OnPrevSong)
   actionHandlers.insert(ID_PREV_SONG,&CMainFrame::OnPrevSong);
   //	ON_COMMAND(IDC_FOLLOW_TOGGLE, OnToggleFollow)
   actionHandlers.insert(IDC_FOLLOW_TOGGLE,&CMainFrame::OnToggleFollow);
   //	ON_COMMAND(ID_FOCUS_PATTERN_EDITOR, OnSelectPatternEditor)
   actionHandlers.insert(ID_FOCUS_PATTERN_EDITOR,&CMainFrame::OnSelectPatternEditor);
   //	ON_COMMAND(ID_FOCUS_FRAME_EDITOR, OnSelectFrameEditor)
   actionHandlers.insert(ID_FOCUS_FRAME_EDITOR,&CMainFrame::OnSelectFrameEditor);
   //	ON_COMMAND(ID_CMD_NEXT_INSTRUMENT, OnNextInstrument)
   actionHandlers.insert(ID_CMD_NEXT_INSTRUMENT,&CMainFrame::OnNextInstrument);
   //	ON_COMMAND(ID_CMD_PREV_INSTRUMENT, OnPrevInstrument)
   actionHandlers.insert(ID_CMD_PREV_INSTRUMENT,&CMainFrame::OnPrevInstrument);
   //	ON_COMMAND(ID_TOGGLE_SPEED, OnToggleSpeed)
   actionHandlers.insert(ID_TOGGLE_SPEED,&CMainFrame::OnToggleSpeed);
   
   //	ON_COMMAND(ID_DECAY_FAST, OnDecayFast)
   actionHandlers.insert(ID_DECAY_FAST,&CMainFrame::OnDecayFast);
   //	ON_COMMAND(ID_DECAY_SLOW, OnDecaySlow)
   actionHandlers.insert(ID_DECAY_SLOW,&CMainFrame::OnDecaySlow);

   // Pass to view class first.
   m_pView->menuAction_triggered(id);
   
   if ( actionHandlers.contains(id) )
   {
      (this->*((actionHandlers[id])))();
   }

   // Pass to app next.
   ((CFamiTrackerApp*)AfxGetApp())->menuAction_triggered(id);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDC *pDC = GetDC();

	// Get the DPI setting
	if (pDC) {
		_dpiX = pDC->GetDeviceCaps(LOGPIXELSX);
		_dpiY = pDC->GetDeviceCaps(LOGPIXELSY);
		ReleaseDC(pDC);
	}

	m_pActionHandler = new CActionHandler();

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!CreateToolbars())
		return -1;

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!CreateDialogPanels())
		return -1;

	if (!CreateInstrumentToolbar()) {
		TRACE0("Failed to create instrument toolbar\n");
		return -1;      // fail to create
	}

	/*
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/
	
	if (!CreateSampleWindow()) {
		TRACE0("Failed to create sample window\n");
		return -1;      // fail to create
	}

	// Initial message, 100ms
	SetTimer(TMR_WELCOME, 100, NULL);

	// Periodic synth check, 50ms
	SetTimer(TMR_SOUND_CHECK, 50, NULL);

	// Auto save
//	SetTimer(TMR_AUTOSAVE, 1000, NULL);

	m_wndOctaveBar.CheckDlgButton(IDC_FOLLOW, TRUE);
	m_wndOctaveBar.SetDlgItemInt(IDC_HIGHLIGHT1, CFamiTrackerDoc::DEFAULT_FIRST_HIGHLIGHT, 0);
	m_wndOctaveBar.SetDlgItemInt(IDC_HIGHLIGHT2, CFamiTrackerDoc::DEFAULT_SECOND_HIGHLIGHT, 0);

	// Icons for the instrument list
	m_iInstrumentIcons[INST_2A03] = 0;
	m_iInstrumentIcons[INST_VRC6] = 1;
	m_iInstrumentIcons[INST_VRC7] = 2;
	m_iInstrumentIcons[INST_FDS] = 3;
	//m_iInstrumentIcons[INST_MMC5] = 0;
	m_iInstrumentIcons[INST_N163] = 4;
	m_iInstrumentIcons[INST_S5B] = 5;

	UpdateMenus();

	// Setup saved frame editor position
	SetFrameEditorPosition(theApp.GetSettings()->FrameEditPos);

#ifdef _DEBUG
	m_strTitle.Append(" [DEBUG]");
#endif
#ifdef WIP
	m_strTitle.Append(" [BETA]");
#endif

	return 0;
}

bool CMainFrame::CreateToolbars()
{
	REBARBANDINFO rbi1;

	// Add the toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))  {
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}

	m_wndToolBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	if (!m_wndOctaveBar.Create(this, (UINT)IDD_OCTAVE, CBRS_TOOLTIPS | CBRS_FLYBY, IDD_OCTAVE)) {
		TRACE0("Failed to create octave bar\n");
		return false;      // fail to create
	}

	if (!m_wndToolBarReBar.Create(this)) {
		TRACE0("Failed to create rebar\n");
		return false;      // fail to create
	}

	rbi1.cbSize		= sizeof(REBARBANDINFO);
	rbi1.fMask		= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_SIZE;
	rbi1.fStyle		= RBBS_NOGRIPPER;
	rbi1.hwndChild	= m_wndToolBar;
	rbi1.cxMinChild	= SX(554);
	rbi1.cyMinChild	= SY(22);
	rbi1.cx			= SX(496);

	if (!m_wndToolBarReBar.GetReBarCtrl().InsertBand(-1, &rbi1)) {
		TRACE0("Failed to create rebar\n");
		return false;      // fail to create
	}

	rbi1.cbSize		= sizeof(REBARBANDINFO);
	rbi1.fMask		= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_SIZE;
	rbi1.fStyle		= RBBS_NOGRIPPER;
	rbi1.hwndChild	= m_wndOctaveBar;
	rbi1.cxMinChild	= SX(120);
	rbi1.cyMinChild	= SY(22);
	rbi1.cx			= SX(100);

	if (!m_wndToolBarReBar.GetReBarCtrl().InsertBand(-1, &rbi1)) {
		TRACE0("Failed to create rebar\n");
		return false;      // fail to create
	}

	m_wndToolBarReBar.GetReBarCtrl().MinimizeBand(0);

   qDebug("ToolBar setup...");
//	HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TOOLBAR_256), IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION /*| LR_LOADMAP3DCOLORS*/);
//	m_bmToolbar.Attach(hbm); 
	
//	m_ilToolBar.Create(16, 15, ILC_COLOR8 | ILC_MASK, 4, 4);
//	m_ilToolBar.Add(&m_bmToolbar, RGB(192, 192, 192));
//	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

	return true;
}

bool CMainFrame::CreateDialogPanels()
{
	//CDialogBar

	// Top area
	if (!m_wndControlBar.Create(this, IDD_MAINBAR, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_MAINBAR)) {
		TRACE0("Failed to create frame main bar\n");
		return false;
	}

	/////////
	if (!m_wndVerticalControlBar.Create(this, IDD_MAINBAR, CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_MAINBAR)) {
		TRACE0("Failed to create frame main bar\n");
		return false;
	}

	m_wndVerticalControlBar.ShowWindow(SW_HIDE);
	/////////

	// Create frame controls
	m_wndFrameControls.SetFrameParent(this);
	m_wndFrameControls.Create(IDD_FRAMECONTROLS, &m_wndControlBar);
	m_wndFrameControls.ShowWindow(SW_SHOW);

	// Create frame editor
	m_pFrameEditor = new CFrameEditor(this);

	CRect rect(SX(12), SY(10), SX(162), SY(173));

	if (!m_pFrameEditor->CreateEx(WS_EX_STATICEDGE, NULL, "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, (CWnd*)&m_wndControlBar, 0)) {
		TRACE0("Failed to create pattern window\n");
		return false;
	}

	m_wndDialogBar.SetFrameParent(this);

	if (!m_wndDialogBar.Create(IDD_MAINFRAME, &m_wndControlBar)) {
		TRACE0("Failed to create dialog bar\n");
		return false;
	}
	
	m_wndDialogBar.ShowWindow(SW_SHOW);

	// Subclass edit boxes
	m_pLockedEditSpeed	= new CLockedEdit();
	m_pLockedEditTempo	= new CLockedEdit();
	m_pLockedEditLength = new CLockedEdit();
	m_pLockedEditFrames = new CLockedEdit();
	m_pLockedEditStep	= new CLockedEdit();

//	m_pLockedEditSpeed->SubclassDlgItem(IDC_SPEED, &m_wndDialogBar);
//	m_pLockedEditTempo->SubclassDlgItem(IDC_TEMPO, &m_wndDialogBar);
//	m_pLockedEditLength->SubclassDlgItem(IDC_ROWS, &m_wndDialogBar);
//	m_pLockedEditFrames->SubclassDlgItem(IDC_FRAMES, &m_wndDialogBar);
//	m_pLockedEditStep->SubclassDlgItem(IDC_KEYSTEP, &m_wndDialogBar);

	// Subclass and setup the instrument list
	m_pInstrumentList = new CInstrumentList(this);
//	m_pInstrumentList->SubclassDlgItem(IDC_INSTRUMENTS, &m_wndDialogBar);
   m_pInstrumentList = (CListCtrl*)m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS);

	SetupColors();

	m_pImageList = new CImageList();
	m_pImageList->Create(16, 16, ILC_COLOR32, 1, 1);
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_2A03INV));
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_VRC6INV));
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_VRC7INV));
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_FDS));
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_N163));
	m_pImageList->Add(theApp.LoadIcon(IDI_INST_5B));

	m_pInstrumentList->SetImageList(m_pImageList, LVSIL_NORMAL);
	m_pInstrumentList->SetImageList(m_pImageList, LVSIL_SMALL);

	m_pInstrumentList->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	// Title, author & copyright
	m_pBannerEditName = new CBannerEdit(CString(_T("(title)")));
	m_pBannerEditArtist = new CBannerEdit(CString(_T("(author)")));
	m_pBannerEditCopyright = new CBannerEdit(CString(_T("(copyright)")));

//	m_pBannerEditName->SubclassDlgItem(IDC_SONG_NAME, &m_wndDialogBar);
//	m_pBannerEditArtist->SubclassDlgItem(IDC_SONG_ARTIST, &m_wndDialogBar);
//	m_pBannerEditCopyright->SubclassDlgItem(IDC_SONG_COPYRIGHT, &m_wndDialogBar);

	// New instrument editor

#ifdef NEW_INSTRUMENTPANEL
/*
	if (!m_wndInstrumentBar.Create(this, IDD_INSTRUMENTPANEL, CBRS_RIGHT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_INSTRUMENTPANEL)) {
		TRACE0("Failed to create frame instrument bar\n");
	}

	m_wndInstrumentBar.ShowWindow(SW_SHOW);
*/
#endif

	// Frame bar
/*
	if (!m_wndFrameBar.Create(this, IDD_FRAMEBAR, CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_FRAMEBAR)) {
		TRACE0("Failed to create frame bar\n");
	}
	
	m_wndFrameBar.ShowWindow(SW_SHOW);
*/

	return true;
}

bool CMainFrame::CreateSampleWindow()
{
	const int POS_X = 137;
	const int POS_Y = 113;

	CRect rect(SX(POS_X), SY(POS_Y), SX(POS_X) + CSampleWindow::WIN_WIDTH, SY(POS_Y) + CSampleWindow::WIN_HEIGHT);

	// Create the sample graph window
	m_pSampleWindow = new CSampleWindow();

	if (!m_pSampleWindow->CreateEx(WS_EX_CLIENTEDGE, NULL, _T("Samples"), WS_CHILD | WS_VISIBLE, rect, (CWnd*)&m_wndDialogBar, 0))
		return false;

	// Assign this to the sound generator
	CSoundGen *pSoundGen = theApp.GetSoundGenerator();
	
	if (pSoundGen)
		pSoundGen->SetSampleWindow(m_pSampleWindow);

	return true;
}

bool CMainFrame::CreateInstrumentToolbar()
{
	// Setup the instrument toolbar
	REBARBANDINFO rbi;

	if (!m_wndInstToolBarWnd.CreateEx(0, NULL, _T(""), WS_CHILD | WS_VISIBLE, CRect(SX(288), SY(173), SX(472), SY(199)), (CWnd*)&m_wndDialogBar, 0))
		return false;

	if (!m_wndInstToolReBar.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndInstToolBarWnd, AFX_IDW_REBAR))
		return false;

	if (!m_wndInstToolBar.CreateEx(&m_wndInstToolReBar, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndInstToolBar.LoadToolBar(IDR_INSTRUMENT_TOOLBAR))
		return false;

   qDebug("ToolBar setup");
//	m_wndInstToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

//	// Set 24-bit icons
//	HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TOOLBAR_INST_256), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
//	m_bmInstToolbar.Attach(hbm);
//	m_ilInstToolBar.Create(16, 16, ILC_COLOR24 | ILC_MASK, 4, 4);
//	m_ilInstToolBar.Add(&m_bmInstToolbar, RGB(255, 0, 255));
//	m_wndInstToolBar.GetToolBarCtrl().SetImageList(&m_ilInstToolBar);

	rbi.cbSize		= sizeof(REBARBANDINFO);
	rbi.fMask		= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_TEXT;
	rbi.fStyle		= RBBS_NOGRIPPER;
	rbi.cxMinChild	= SX(300);
	rbi.cyMinChild	= SY(30);
	rbi.lpText		= "";
	rbi.cch			= 7;
	rbi.cx			= SX(300);
	rbi.hwndChild	= m_wndInstToolBar;

	m_wndInstToolReBar.InsertBand(-1, &rbi);

	// Route messages to this window
	m_wndInstToolBar.SetOwner(this);

	// Turn add new instrument button into a drop-down list
	m_wndInstToolBar.SetButtonStyle(0, TBBS_DROPDOWN);
	// Turn load instrument button into a drop-down list
	m_wndInstToolBar.SetButtonStyle(4, TBBS_DROPDOWN);

	return true;
}

void CMainFrame::ResizeFrameWindow()
{
	// Called when the number of channels has changed
	ASSERT(m_pFrameEditor != NULL);

	CFamiTrackerDoc *pDocument = (CFamiTrackerDoc*)GetActiveDocument();

	if (pDocument != NULL) {

		int Channels = pDocument->GetAvailableChannels();
		int Chip = pDocument->GetExpansionChip();
		int Height(0), Width(0);

		// Located to the right
		if (m_iFrameEditorPos == FRAME_EDIT_POS_TOP) {
			// Frame editor window
			Height = 161;
			Width = CFrameEditor::FIXED_WIDTH + CFrameEditor::FRAME_ITEM_WIDTH * Channels;

			m_pFrameEditor->MoveWindow(SX(12), SY(12), SX(Width), SY(Height));

			// Move frame controls
			m_wndFrameControls.MoveWindow(SX(10), SY(Height) + SY(21), SX(150), SY(26));
		}
		// Located to the left
		else {
			// Frame editor window
			CRect rect;
			m_wndVerticalControlBar.GetClientRect(&rect);

			Height = rect.Height() - HEADER_HEIGHT - 2;
			Width = CFrameEditor::FIXED_WIDTH + CFrameEditor::FRAME_ITEM_WIDTH * Channels;

			m_pFrameEditor->MoveWindow(SX(2), SY(HEADER_HEIGHT + 1), SX(Width), SY(Height));

			// Move frame controls
			m_wndFrameControls.MoveWindow(SX(4), SY(10), SX(150), SY(26));
		}

		// Vertical control bar
		m_wndVerticalControlBar.m_sizeDefault.cx = SX(54) + SX(CFrameEditor::FRAME_ITEM_WIDTH * Channels);
		m_wndVerticalControlBar.CalcFixedLayout(TRUE, FALSE);
		RecalcLayout();
	}

	CRect ChildRect, ParentRect, FrameEditorRect, FrameBarRect;

	m_wndControlBar.GetClientRect(&ParentRect);
	m_pFrameEditor->GetClientRect(&FrameEditorRect);

	int DialogStartPos;

	if (m_iFrameEditorPos == FRAME_EDIT_POS_TOP)
		DialogStartPos = FrameEditorRect.right + 32;
	else
		DialogStartPos = 0;

	m_wndDialogBar.MoveWindow(DialogStartPos, 2, ParentRect.Width() - DialogStartPos, ParentRect.Height() - 4);
	m_wndDialogBar.GetWindowRect(&ChildRect);
	m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->MoveWindow(SX(288), SY(10), ChildRect.Width() - SX(296), SY(158));
	m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->MoveWindow(SX(478), SY(175), ChildRect.Width() - SX(486), SY(22));

	m_pFrameEditor->RedrawWindow();
}

void CMainFrame::SetupColors(void)
{
	// Instrument list colors
	m_pInstrumentList->SetBkColor(theApp.GetSettings()->Appearance.iColBackground);
	m_pInstrumentList->SetTextBkColor(theApp.GetSettings()->Appearance.iColBackground);
	m_pInstrumentList->SetTextColor(theApp.GetSettings()->Appearance.iColPatternText);
	m_pInstrumentList->RedrawWindow();
}

void CMainFrame::SetTempo(int Tempo)
{
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	int MinTempo = pDoc->GetSpeedSplitPoint();
	LIMIT(Tempo, MAX_TEMPO, MinTempo);
	pDoc->SetSongTempo(Tempo);
	theApp.GetSoundGenerator()->ResetTempo();

	if (m_wndDialogBar.GetDlgItemInt(IDC_TEMPO) != Tempo)
		m_wndDialogBar.SetDlgItemInt(IDC_TEMPO, Tempo, FALSE);
}

void CMainFrame::SetSpeed(int Speed)
{
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	int MaxSpeed = pDoc->GetSpeedSplitPoint() - 1;
	LIMIT(Speed, MaxSpeed, MIN_SPEED);
	pDoc->SetSongSpeed(Speed);
	theApp.GetSoundGenerator()->ResetTempo();

	if (m_wndDialogBar.GetDlgItemInt(IDC_SPEED) != Speed)
		m_wndDialogBar.SetDlgItemInt(IDC_SPEED, Speed, FALSE);
}

void CMainFrame::SetRowCount(int Count)
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	if (!pDoc)
		return;
	
	LIMIT(Count, MAX_PATTERN_LENGTH, 1);

	if (Count != pDoc->GetPatternLength()) {

		CPatternAction *pAction = dynamic_cast<CPatternAction*>(GetLastAction());

		if (pAction == NULL || pAction->GetAction() != CPatternAction::ACT_PATTERN_LENGTH) {
			// New action
			pAction = new CPatternAction(CPatternAction::ACT_PATTERN_LENGTH);
			pAction->SetPatternLength(Count);
			AddAction(pAction);
		}
		else {
			// Update existing action
			pAction->SetPatternLength(Count);
			pAction->Update(this);
		}
	}

	if (m_wndDialogBar.GetDlgItemInt(IDC_ROWS) != Count)
		m_wndDialogBar.SetDlgItemInt(IDC_ROWS, Count, FALSE);
}

void CMainFrame::SetFrameCount(int Count)
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	if (!pDoc || !pDoc->IsFileLoaded())
		return;

	LIMIT(Count, MAX_FRAMES, 1);

	if (Count != pDoc->GetFrameCount()) {

		CFrameAction *pAction = dynamic_cast<CFrameAction*>(GetLastAction());

		if (pAction == NULL || pAction->GetAction() != CFrameAction::ACT_CHANGE_COUNT) {
			// New action
			pAction = new CFrameAction(CFrameAction::ACT_CHANGE_COUNT);
			pAction->SetFrameCount(Count);
			AddAction(pAction);
		}
		else {
			// Update existing action
			pAction->SetFrameCount(Count);
			pAction->Update(this);
		}
	}

	if (m_wndDialogBar.GetDlgItemInt(IDC_FRAMES) != Count)
		m_wndDialogBar.SetDlgItemInt(IDC_FRAMES, Count, FALSE);
}

void CMainFrame::UpdateControls()
{
	m_wndDialogBar.UpdateDialogControls(&m_wndDialogBar, TRUE);
}

void CMainFrame::SetHighlightRow(int Rows)
{
	m_wndOctaveBar.SetDlgItemInt(IDC_HIGHLIGHT1, Rows);
}

void CMainFrame::SetSecondHighlightRow(int Rows)
{
	m_wndOctaveBar.SetDlgItemInt(IDC_HIGHLIGHT2, Rows);
}

void CMainFrame::DisplayOctave()
{
	CComboBox *pOctaveList = (CComboBox*)m_wndOctaveBar.GetDlgItem(IDC_OCTAVE);
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pOctaveList->SetCurSel(pView->GetOctave());
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::SetStatusText(LPCTSTR Text,...)
{
	char	Buf[512];
    va_list argp;
    
	va_start(argp, Text);
    
	if (!Text)
		return;

    vsprintf(Buf, Text, argp);

	m_wndStatusBar.SetWindowText(Buf);
}

void CMainFrame::ClearInstrumentList()
{
	m_pInstrumentList->DeleteAllItems();
	m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
}

void CMainFrame::NewInstrument(int ChipType)
{
	// Add new instrument to module
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();

	int Index = pDoc->AddInstrument(CFamiTrackerDoc::NEW_INST_NAME, ChipType);

	if (Index == -1) {
		// Out of slots
		AfxMessageBox(IDS_INST_LIMIT, MB_ICONERROR);
		return;
	}

	// Add to list
	AddInstrument(Index);

	// also select it
	SelectInstrument(Index);
}

void CMainFrame::AddInstrument(int Index)
{
	// Adds an instrument to the instrument list
	// instrument must exist in document.
	//
	// Index = instrument slot

	ASSERT(Index != -1);

	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	char Name[256];
	pDoc->GetInstrumentName(Index, Name);
	int Type = pDoc->GetInstrument(Index)->GetType();

	// Name is of type index - name
	CString Text;
	Text.Format(_T("%02X - %s"), Index, A2T(Name));
	m_pInstrumentList->InsertItem(Index, Text, m_iInstrumentIcons[Type]);
}

void CMainFrame::RemoveInstrument(int Index) 
{
	// Remove instrument from instrument list
	ASSERT(Index != -1);

	if (m_wndInstEdit.IsOpened()) {
		m_wndInstEdit.DestroyWindow();
	}

	m_pInstrumentList->DeleteItem(FindInstrument(Index));
}

int CMainFrame::FindInstrument(int Index) const
{
	// Find an instrument from the instrument list
	CString Txt;
	Txt.Format(_T("%02X"), Index);

	LVFINDINFO info;
	info.flags = LVFI_PARTIAL | LVFI_STRING;
	info.psz = Txt;

	return m_pInstrumentList->FindItem(&info);
}

int CMainFrame::GetInstrumentIndex(int ListIndex) const
{
	// Convert instrument list index to instrument slot index
	int Instrument;
	TCHAR Text[256];
	m_pInstrumentList->GetItemText(ListIndex, 0, Text, 256);
	_stscanf(Text, _T("%X"), &Instrument);
	return Instrument;
}

void CMainFrame::UpdateInstrumentList()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	ClearInstrumentList();

	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
		if (pDoc->IsInstrumentUsed(i)) {
			AddInstrument(i);
		}
	}
}

void CMainFrame::SelectInstrument(int Index)
{
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	
	int ListCount = m_pInstrumentList->GetItemCount();

	// No instruments added
	if (ListCount == 0) {
		m_iInstrument = 0;
		m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
		return;
	}

	int LastInst = m_iInstrument;
	m_iInstrument = Index;

	if (pDoc->IsInstrumentUsed(Index)) {
		// Select instrument in list
		int ListIndex = FindInstrument(Index);
		int Selected = m_pInstrumentList->GetSelectionMark();

		if (ListIndex != Selected || LastInst != Index) {
			m_pInstrumentList->SetSelectionMark(ListIndex);
			m_pInstrumentList->SetItemState(ListIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_pInstrumentList->EnsureVisible(ListIndex, FALSE);

			// Set instrument name (this will trigger the name change routine)
			char Text[256];
			pDoc->GetInstrumentName(Index, Text);
			m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(Text);

			// Update instrument editor
			if (m_wndInstEdit.IsOpened())
				m_wndInstEdit.SetCurrentInstrument(Index);
		}
	}
	else {
		// Remove selection
		m_pInstrumentList->SetSelectionMark(-1);
		m_pInstrumentList->SetItemState(LastInst, 0, LVIS_SELECTED | LVIS_FOCUSED);
		m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
		if (m_wndInstEdit.IsOpened())
			m_wndInstEdit.DestroyWindow();
	}
}

int CMainFrame::GetSelectedInstrument() const
{
	// Returns selected instrument
	return m_iInstrument;
}

void CMainFrame::OnNextInstrument()
{
	// Select next instrument in the list
	int SelIndex = m_pInstrumentList->GetSelectionMark();
	int Count = m_pInstrumentList->GetItemCount();
	if (SelIndex < (Count - 1)) {
		int Slot = GetInstrumentIndex(SelIndex + 1);
		SelectInstrument(Slot);
	}
}

void CMainFrame::OnPrevInstrument()
{
	// Select previous instrument in the list
	int SelIndex = m_pInstrumentList->GetSelectionMark();
	if (SelIndex > 0) {
		int Slot = GetInstrumentIndex(SelIndex - 1);
		SelectInstrument(Slot);
	}
}

void CMainFrame::SetIndicatorTime(int Min, int Sec, int MSec)
{
	static int LMin, LSec, LMSec;

	if (Min != LMin || Sec != LSec || MSec != LMSec) {
		LMin = Min;
		LSec = Sec;
		LMSec = MSec;
		CString String;
		String.Format(_T("%02i:%02i:%01i0"), Min, Sec, MSec);
		m_wndStatusBar.SetPaneText(6, String);
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (m_wndToolBarReBar.m_hWnd == NULL)
		return;

	m_wndToolBarReBar.GetReBarCtrl().MinimizeBand(0);

	ResizeFrameWindow();
}

void CMainFrame::OnClickInstruments(NMHDR *pNMHDR, LRESULT *result)
{
	GetActiveView()->SetFocus();
}

void CMainFrame::OnChangedInstruments(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Change selected instrument
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (!(pNMListView->uNewState & LVIS_SELECTED))
		return;

	int SelIndex = pNMListView->iItem; 

	if (SelIndex == -1)
		return;

	// Find selected instrument
	int Instrument = GetInstrumentIndex(SelIndex);
	SelectInstrument(Instrument);
}

void CMainFrame::OnDblClkInstruments(NMHDR *pNotifyStruct, LRESULT *result)
{
	OpenInstrumentSettings();
}

void CMainFrame::OnInstNameChange()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	int SelIndex = m_pInstrumentList->GetSelectionMark();

	if (SelIndex == -1 || m_pInstrumentList->GetItemCount() == 0)
		return;

	if (!pDoc->IsInstrumentUsed(m_iInstrument))
		return;

	TCHAR Text[256];
	m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->GetWindowText(Text, 256);

	// Doesn't need to be longer than 60 chars
	Text[60] = 0;

	// Update instrument list & document
	CString Name;
	Name.Format(_T("%02X - %s"), m_iInstrument, Text);
	m_pInstrumentList->SetItemText(SelIndex, 0, Name);
	pDoc->SetInstrumentName(m_iInstrument, T2A(Text));
}

void CMainFrame::OnAddInstrument2A03()
{
	NewInstrument(SNDCHIP_NONE);
}

void CMainFrame::OnAddInstrumentVRC6()
{
	NewInstrument(SNDCHIP_VRC6);
}

void CMainFrame::OnAddInstrumentVRC7()
{
	NewInstrument(SNDCHIP_VRC7);
}

void CMainFrame::OnAddInstrumentFDS()
{
	NewInstrument(SNDCHIP_FDS);
}

void CMainFrame::OnAddInstrumentMMC5()
{
	NewInstrument(SNDCHIP_MMC5);
}

void CMainFrame::OnAddInstrumentN163()
{
	NewInstrument(SNDCHIP_N163);
}

void CMainFrame::OnAddInstrumentS5B()
{
	NewInstrument(SNDCHIP_S5B);
}

void CMainFrame::OnAddInstrument()
{
	// Add new instrument to module

	// Chip type depends on selected channel
	CFamiTrackerView *pView = ((CFamiTrackerView*)GetActiveView());
	int ChipType = pView->GetSelectedChipType();
	NewInstrument(ChipType);
}

void CMainFrame::OnRemoveInstrument()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	// No instruments in list
	if (m_pInstrumentList->GetItemCount() == 0)
		return;

	int Instrument = m_iInstrument;
	int SelIndex = m_pInstrumentList->GetSelectionMark();

	ASSERT(pDoc->IsInstrumentUsed(Instrument));

	// Remove from document
	pDoc->RemoveInstrument(Instrument);

	// Remove from list
	RemoveInstrument(Instrument);

	int Count = m_pInstrumentList->GetItemCount();

	// Select a new instrument
	if (Count == 0) {
		SelectInstrument(0);
	}
	else if (Count == SelIndex) {
		SelectInstrument(GetInstrumentIndex(SelIndex - 1));
	}
	else {
		SelectInstrument(GetInstrumentIndex(SelIndex));
	}
}

void CMainFrame::OnCloneInstrument() 
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	
	// No instruments in list
	if (m_pInstrumentList->GetItemCount() == 0)
		return;

	int Slot = pDoc->CloneInstrument(m_iInstrument);

	if (Slot == -1) {
		AfxMessageBox(IDS_INST_LIMIT, MB_ICONERROR);
		return;
	}

	AddInstrument(Slot);
	SelectInstrument(Slot);
}

void CMainFrame::OnDeepCloneInstrument()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	
	// No instruments in list
	if (m_pInstrumentList->GetItemCount() == 0)
		return;

	int Slot = pDoc->DeepCloneInstrument(m_iInstrument);

	if (Slot == -1) {
		AfxMessageBox(IDS_INST_LIMIT, MB_ICONERROR);
		return;
	}

	AddInstrument(Slot);
	SelectInstrument(Slot);
}

void CMainFrame::OnBnClickedEditInst()
{
	OpenInstrumentSettings();
}

void CMainFrame::OnEditInstrument()
{
	OpenInstrumentSettings();
}

void CMainFrame::OnLoadInstrument()
{
	// Loads an instrument from a file

	CString filter;
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));

	filter = FTI_FILTER_NAME;
	filter += _T("|*");
	filter += FTI_FILTER_EXT;
	filter += _T("|");
	filter += allFilter;
	filter += _T("|*.*||");

	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();
	CFileDialog FileDialog(TRUE, _T("fti"), 0, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, filter);

	FileDialog.m_pOFN->lpstrInitialDir = theApp.GetSettings()->GetPath(PATH_FTI);

	if (FileDialog.DoModal() == IDCANCEL)
		return;

	POSITION pos (FileDialog.GetStartPosition());

	// Load multiple files
	while(pos) {
		CString csFileName(FileDialog.GetNextPathName(pos));
		int Index = pDoc->LoadInstrument(csFileName);
		if (Index == -1)
			return;
		AddInstrument(Index);
	}

	theApp.GetSettings()->SetPath(FileDialog.GetPathName(), PATH_FTI);
}

void CMainFrame::OnSaveInstrument()
{
	// Saves instrument to a file

	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();

	char Name[256];
	CString String;

	int Index = GetSelectedInstrument();

	if (Index == -1)
		return;

	if (!pDoc->IsInstrumentUsed(Index))
		return;

	pDoc->GetInstrumentName(Index, Name);

	// Remove bad characters
	char *ptr = Name;

	while (*ptr != 0) {
		if (*ptr == '/')
			*ptr = ' ';
		ptr++;
	}

	CString filter;
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));

	filter = FTI_FILTER_NAME;
	filter += _T("|*");
	filter += FTI_FILTER_EXT;
	filter += _T("|");
	filter += allFilter;
	filter += _T("|*.*||");

	CFileDialog FileDialog(FALSE, _T("fti"), Name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);

	FileDialog.m_pOFN->lpstrInitialDir = theApp.GetSettings()->GetPath(PATH_FTI);

	if (FileDialog.DoModal() == IDCANCEL)
		return;

	pDoc->SaveInstrument(pView->GetInstrument(), FileDialog.GetPathName());

	theApp.GetSettings()->SetPath(FileDialog.GetPathName(), PATH_FTI);

	if (m_pInstrumentFileTree)
		m_pInstrumentFileTree->Changed();
}

void CMainFrame::OnDeltaposSpeedSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int NewSpeed = CFamiTrackerDoc::GetDoc()->GetSongSpeed() - ((NMUPDOWN*)pNMHDR)->iDelta;
	SetSpeed(NewSpeed);
}

void CMainFrame::OnDeltaposTempoSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int NewTempo = CFamiTrackerDoc::GetDoc()->GetSongTempo() - ((NMUPDOWN*)pNMHDR)->iDelta;
	SetTempo(NewTempo);
}

void CMainFrame::OnDeltaposRowsSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int NewRows = CFamiTrackerDoc::GetDoc()->GetPatternLength() - ((NMUPDOWN*)pNMHDR)->iDelta;
	SetRowCount(NewRows);
}

void CMainFrame::OnDeltaposFrameSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int NewFrames = CFamiTrackerDoc::GetDoc()->GetFrameCount() - ((NMUPDOWN*)pNMHDR)->iDelta;
	SetFrameCount(NewFrames);
}

void CMainFrame::OnTrackerKillsound()
{
	theApp.LoadSoundConfig();
	theApp.SilentEverything();
}

void CMainFrame::OnBnClickedIncFrame()
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(GetActiveView());
	pView->IncreaseCurrentPattern();
	pView->SetFocus();
}

void CMainFrame::OnBnClickedDecFrame()
{
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(GetActiveView());
	pView->DecreaseCurrentPattern();
	pView->SetFocus();
}

void CMainFrame::OnKeyRepeat()
{
	theApp.GetSettings()->General.bKeyRepeat = (m_wndDialogBar.IsDlgButtonChecked(IDC_KEYREPEAT) == 1);
}

void CMainFrame::OnDeltaposKeyStepSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	int Pos = m_wndDialogBar.GetDlgItemInt(IDC_KEYSTEP) - ((NMUPDOWN*)pNMHDR)->iDelta;
	LIMIT(Pos, MAX_PATTERN_LENGTH, 0);
	m_wndDialogBar.SetDlgItemInt(IDC_KEYSTEP, Pos);
}

void CMainFrame::OnEnKeyStepChange()
{
	int Step = m_wndDialogBar.GetDlgItemInt(IDC_KEYSTEP);
	LIMIT(Step, MAX_PATTERN_LENGTH, 0);
	static_cast<CFamiTrackerView*>(GetActiveView())->SetStepping(Step);
}

void CMainFrame::OnCreateNSF()
{
	CExportDialog ExportDialog(this);
	ExportDialog.DoModal();
}

void CMainFrame::OnCreateWAV()
{
   qDebug("CMainFrame::OnCreateWAV");
//	CCreateWaveDlg WaveDialog;
//	WaveDialog.ShowDialog();
}

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle , const RECT& rect , CWnd* pParentWnd , LPCTSTR lpszMenuName , DWORD dwExStyle , CCreateContext* pContext)
{
   qDebug("CMainFrame::Create");
//	CSettings *pSettings = theApp.GetSettings();
//	RECT newrect;

//	// Load stored position
//	newrect.bottom	= pSettings->WindowPos.iBottom;
//	newrect.left	= pSettings->WindowPos.iLeft;
//	newrect.right	= pSettings->WindowPos.iRight;
//	newrect.top		= pSettings->WindowPos.iTop;

//	return CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, newrect, pParentWnd, lpszMenuName, dwExStyle, pContext);
}

void CMainFrame::OnNextFrame()
{
	static_cast<CFamiTrackerView*>(GetActiveView())->SelectNextFrame();
}

void CMainFrame::OnPrevFrame()
{
	static_cast<CFamiTrackerView*>(GetActiveView())->SelectPrevFrame();
}

void CMainFrame::OnChangeAll()
{	
	CFamiTrackerView *pView = static_cast<CFamiTrackerView*>(GetActiveView());
	bool Enabled = m_wndFrameControls.IsDlgButtonChecked(IDC_CHANGE_ALL) != 0;
	pView->SetChangeAllPattern(Enabled);
}

void CMainFrame::OnHelpPerformance()
{
   qDebug("Performance dialog not implemented...");
//	m_wndPerformanceDlg.Create(MAKEINTRESOURCE(IDD_PERFORMANCE), this);
//	m_wndPerformanceDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateSBInstrument(CCmdUI *pCmdUI)
{
	CString String;
	int Instrument = ((CFamiTrackerView*)GetActiveView())->GetInstrument();
	String.Format(_T("Instrument: %02X"), Instrument);
	pCmdUI->Enable(); 
	pCmdUI->SetText(String);
}

void CMainFrame::OnUpdateSBOctave(CCmdUI *pCmdUI)
{
	CString String;
	int Octave = ((CFamiTrackerView*)GetActiveView())->GetOctave();
	String.Format(_T("Octave: %i"), Octave);
	pCmdUI->Enable(); 
	pCmdUI->SetText(String);
}

void CMainFrame::OnUpdateSBFrequency(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc *pDoc = ((CFamiTrackerDoc*)GetActiveDocument());
	int Machine = pDoc->GetMachine();
	int EngineSpeed = pDoc->GetEngineSpeed();
	CString String;

	if (EngineSpeed == 0)
		EngineSpeed = (Machine == NTSC ? CAPU::FRAME_RATE_NTSC : CAPU::FRAME_RATE_PAL);

	String.Format(_T("%i Hz"), EngineSpeed);

	pCmdUI->Enable(); 
	pCmdUI->SetText(String);
}

void CMainFrame::OnUpdateSBTempo(CCmdUI *pCmdUI)
{
	CString String;
	CSoundGen *pSoundGen = theApp.GetSoundGenerator();
	if (pSoundGen) {
		int Highlight = m_wndOctaveBar.GetDlgItemInt(IDC_HIGHLIGHT1);
		if (Highlight == 0)
			Highlight = 4;
		int BPM = (pSoundGen->GetTempo() * 4) / Highlight;
		String.Format(_T("%i BPM"), BPM);
		pCmdUI->Enable(); 
		pCmdUI->SetText(String);
	}
}

void CMainFrame::OnUpdateSBChip(CCmdUI *pCmdUI)
{
	CString String;
	
	CFamiTrackerDoc *pDoc = ((CFamiTrackerDoc*)GetActiveDocument());
	int Chip = pDoc->GetExpansionChip();

	switch (Chip) {
		case SNDCHIP_NONE:
			String = _T("No expansion chip");
			break;
		case SNDCHIP_VRC6:
			String = _T("Konami VRC6");
			break;
		case SNDCHIP_MMC5:
			String = _T("Nintendo MMC5");
			break;
		case SNDCHIP_FDS:
			String = _T("Nintendo FDS");
			break;
		case SNDCHIP_VRC7:
			String = _T("Konami VRC7");
			break;
		case SNDCHIP_N163:
			String = _T("Namco 163");
			break;
		case SNDCHIP_S5B:
			String = _T("Sunsoft 5B");
			break;
	}

	pCmdUI->Enable(); 
	pCmdUI->SetText(String);
}

void CMainFrame::OnUpdateInsertFrame(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	if (!pDoc->IsFileLoaded())
		return;

	pCmdUI->Enable(pDoc->GetFrameCount() < MAX_FRAMES);
}

void CMainFrame::OnUpdateRemoveFrame(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	if (!pDoc->IsFileLoaded())
		return;

	pCmdUI->Enable(pDoc->GetFrameCount() > 1);
}

void CMainFrame::OnUpdateDuplicateFrame(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();

	if (!pDoc->IsFileLoaded())
		return;

	pCmdUI->Enable(pDoc->GetFrameCount() < MAX_FRAMES);
}

void CMainFrame::OnUpdateModuleMoveframedown(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();

	if (!pDoc->IsFileLoaded())
		return;

	pCmdUI->Enable(!(pView->GetSelectedFrame() == (pDoc->GetFrameCount() - 1)));
}

void CMainFrame::OnUpdateModuleMoveframeup(CCmdUI *pCmdUI)
{
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();
	pCmdUI->Enable(pView->GetSelectedFrame() > 0);
}

void CMainFrame::OnUpdateInstrumentNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() < MAX_INSTRUMENTS);
}

void CMainFrame::OnUpdateInstrumentRemove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() > 0);
}

void CMainFrame::OnUpdateInstrumentClone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() > 0 && m_pInstrumentList->GetItemCount() < MAX_INSTRUMENTS);
}

void CMainFrame::OnUpdateInstrumentDeepClone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() > 0 && m_pInstrumentList->GetItemCount() < MAX_INSTRUMENTS);
}

void CMainFrame::OnUpdateInstrumentLoad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() < MAX_INSTRUMENTS);
}

void CMainFrame::OnUpdateInstrumentSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() > 0);
}

void CMainFrame::OnUpdateInstrumentEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pInstrumentList->GetItemCount() > 0);
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	CString text;
	switch (nIDEvent) {
		// Welcome message
		case TMR_WELCOME:
			text.Format(IDS_WELCOME_VER, VERSION_MAJ, VERSION_MIN, VERSION_REV);
			SetMessageText(text);
			KillTimer(0);
			break;
		// Check sound player
		case TMR_SOUND_CHECK:
			theApp.CheckSynth();
			break;
		// Auto save
		case TMR_AUTOSAVE: {
			/*
				CFamiTrackerDoc *pDoc = dynamic_cast<CFamiTrackerDoc*>(GetActiveDocument());
				if (pDoc != NULL)
					pDoc->AutoSave();
					*/
			}
			break;
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateKeyStepEdit(CCmdUI *pCmdUI)
{
	CString Text;
	Text.Format(_T("%i"), ((CFamiTrackerView*)GetActiveView())->GetStepping());
	pCmdUI->SetText(Text);
}

void CMainFrame::OnUpdateSpeedEdit(CCmdUI *pCmdUI)
{
//	if (!m_pLockedEditSpeed->IsEditable()) {
		if (m_pLockedEditSpeed->Update())
			SetSpeed(m_pLockedEditSpeed->GetValue());
		else {
			CString Text;
			Text.Format(_T("%i"), ((CFamiTrackerDoc*)GetActiveDocument())->GetSongSpeed());
			pCmdUI->SetText(Text);
		}
//	}	
}

void CMainFrame::OnUpdateTempoEdit(CCmdUI *pCmdUI)
{
//	if (!m_pLockedEditTempo->IsEditable()) {
		if (m_pLockedEditTempo->Update())
			SetTempo(m_pLockedEditTempo->GetValue());
		else {
			CString Text;
			Text.Format(_T("%i"), ((CFamiTrackerDoc*)GetActiveDocument())->GetSongTempo());
			pCmdUI->SetText(Text);
		}
//	}
}

void CMainFrame::OnUpdateRowsEdit(CCmdUI *pCmdUI)
{
	CString Text;

//	if (!m_pLockedEditLength->IsEditable()) {
		if (m_pLockedEditLength->Update())
			SetRowCount(m_pLockedEditLength->GetValue());
		else {
			Text.Format(_T("%i"), ((CFamiTrackerDoc*)GetActiveDocument())->GetPatternLength());
			pCmdUI->SetText(Text);
		}
//	}
}

void CMainFrame::OnUpdateFramesEdit(CCmdUI *pCmdUI)
{
//	if (!m_pLockedEditFrames->IsEditable()) {
		if (m_pLockedEditFrames->Update())
			SetFrameCount(m_pLockedEditFrames->GetValue());
		else {
			CString Text;
			Text.Format(_T("%i"), ((CFamiTrackerDoc*)GetActiveDocument())->GetFrameCount());
			pCmdUI->SetText(Text);
		}
//	}	
}

void CMainFrame::OnFileGeneralsettings()
{
	CString Title;
	GetMessageString(IDS_CONFIG_WINDOW, Title);
	CConfigWindow ConfigWindow(Title, this, 0);

	CConfigGeneral		TabGeneral;
	CConfigAppearance	TabAppearance;
   qDebug("NO MIDI SUPPORT YET...");
//	CConfigMIDI			TabMIDI;
//	CConfigSound		TabSound;
	CConfigShortcuts	TabShortcuts;
	//CConfigLevels		TabLevels;

//	ConfigWindow.m_psh.dwFlags	&= ~PSH_HASHELP;
//	TabGeneral.m_psp.dwFlags	&= ~PSP_HASHELP;
//	TabAppearance.m_psp.dwFlags &= ~PSP_HASHELP;
//	TabMIDI.m_psp.dwFlags		&= ~PSP_HASHELP;
//	TabSound.m_psp.dwFlags		&= ~PSP_HASHELP;
//	TabShortcuts.m_psp.dwFlags	&= ~PSP_HASHELP;
//	//TabLevels.m_psp.dwFlags		&= ~PSP_HASHELP;
	
	ConfigWindow.AddPage(&TabGeneral);
	ConfigWindow.AddPage(&TabAppearance);
//	ConfigWindow.AddPage(&TabMIDI);
//	ConfigWindow.AddPage(&TabSound);
	ConfigWindow.AddPage(&TabShortcuts);
	//ConfigWindow.AddPage(&TabLevels);

	ConfigWindow.DoModal();
}

void CMainFrame::SetSongInfo(char *Name, char *Artist, char *Copyright)
{
	m_wndDialogBar.SetDlgItemText(IDC_SONG_NAME, Name);
	m_wndDialogBar.SetDlgItemText(IDC_SONG_ARTIST, Artist);
	m_wndDialogBar.SetDlgItemText(IDC_SONG_COPYRIGHT, Copyright);
}

void CMainFrame::OnEnSongNameChange()
{
	CFamiTrackerDoc* pDoc = ((CFamiTrackerDoc*)GetActiveDocument());
	char Text[64];
	m_wndDialogBar.GetDlgItemText(IDC_SONG_NAME, Text, 32);
	pDoc->SetSongName(Text);
}

void CMainFrame::OnEnSongArtistChange()
{
	CFamiTrackerDoc* pDoc = ((CFamiTrackerDoc*)GetActiveDocument());
	char Text[64];
	m_wndDialogBar.GetDlgItemText(IDC_SONG_ARTIST, Text, 32);
	pDoc->SetSongArtist(Text);
}

void CMainFrame::OnEnSongCopyrightChange()
{
	CFamiTrackerDoc* pDoc = ((CFamiTrackerDoc*)GetActiveDocument());
	char Text[64];
	m_wndDialogBar.GetDlgItemText(IDC_SONG_COPYRIGHT, Text, 32);
	pDoc->SetSongCopyright(Text);
}

void CMainFrame::ChangeNoteState(int Note)
{
	m_wndInstEdit.ChangeNoteState(Note);
}

void CMainFrame::OpenInstrumentSettings()
{
	CFamiTrackerDoc	*pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();

	if (pDoc->IsInstrumentUsed(pView->GetInstrument())) {
		if (m_wndInstEdit.IsOpened() == false) {
			m_wndInstEdit.Create(IDD_INSTRUMENT, this);
			m_wndInstEdit.SetCurrentInstrument(pView->GetInstrument());
			m_wndInstEdit.ShowWindow(SW_SHOW);
		}
		else
			m_wndInstEdit.SetCurrentInstrument(pView->GetInstrument());
		m_wndInstEdit.UpdateWindow();
	}
}

void CMainFrame::CloseInstrumentEditor()
{
	if (m_wndInstEdit.IsOpened())
		m_wndInstEdit.DestroyWindow();
}

void CMainFrame::OnUpdateKeyRepeat(CCmdUI *pCmdUI)
{
	if (theApp.GetSettings()->General.bKeyRepeat)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnFileImportmidi()
{
   qDebug("Missing support for MIDI import...");
//	CMIDIImport	Importer;
//	CFileDialog FileDialog(TRUE, 0, 0, OFN_HIDEREADONLY, _T("MIDI files (*.mid)|*.mid|All files|*.*||"));

//	if (GetActiveDocument()->SaveModified() == 0)
//		return;

//	if (FileDialog.DoModal() == IDCANCEL)
//		return;

//	Importer.ImportFile(FileDialog.GetPathName());
}

BOOL CMainFrame::DestroyWindow()
{
   qDebug("Destroy window isn't really necessary...");
//	// Store window position

//	CRect WinRect;
//	int State = STATE_NORMAL;

//	GetWindowRect(WinRect);

//	if (IsZoomed()) {
//		State = STATE_MAXIMIZED;
//		// Ignore window position if maximized
//		WinRect.top = theApp.GetSettings()->WindowPos.iTop;
//		WinRect.bottom = theApp.GetSettings()->WindowPos.iBottom;
//		WinRect.left = theApp.GetSettings()->WindowPos.iLeft;
//		WinRect.right = theApp.GetSettings()->WindowPos.iRight;
//	}

//	if (IsIconic()) {
//		WinRect.top = WinRect.left = 100;
//		WinRect.bottom = 920;
//		WinRect.right = 950;
//	}

//	// Save window position
//	theApp.GetSettings()->SetWindowPos(WinRect.left, WinRect.top, WinRect.right, WinRect.bottom, State);

	return CFrameWnd::DestroyWindow();
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CMainFrame::OnTrackerSwitchToInstrument()
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pView->SwitchToInstrument(!pView->SwitchToInstrument());
}

void CMainFrame::OnTrackerDPCM()
{
	CMenu *pMenu = GetMenu();

	if (pMenu->GetMenuState(ID_TRACKER_DPCM, MF_BYCOMMAND) == MF_CHECKED)
		pMenu->CheckMenuItem(ID_TRACKER_DPCM, MF_UNCHECKED);
	else
		pMenu->CheckMenuItem(ID_TRACKER_DPCM, MF_CHECKED);
}

void CMainFrame::OnUpdateTrackerSwitchToInstrument(CCmdUI *pCmdUI)
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pCmdUI->SetCheck(pView->SwitchToInstrument() ? 1 : 0);
}

void CMainFrame::OnModuleModuleproperties()
{
	// Display module properties dialog
	CModulePropertiesDlg propertiesDlg;
	propertiesDlg.DoModal();
}

void CMainFrame::OnModuleChannels()
{
	CChannelsDlg channelsDlg;
	channelsDlg.DoModal();
}

void CMainFrame::OnModuleComments()
{
	CCommentsDlg commentsDlg;
	commentsDlg.DoModal();
}

void CMainFrame::UpdateTrackBox()
{
	// Fill the track box with all songs
	CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
	CFamiTrackerDoc	*pDoc		= (CFamiTrackerDoc*)GetActiveDocument();
	CString			Text;

	ASSERT(pTrackBox != NULL);
	ASSERT(pDoc != NULL);

	pTrackBox->ResetContent();

	int Count = pDoc->GetTrackCount();

	for (int i = 0; i < Count; ++i) {
		Text.Format(_T("#%i %s"), i + 1, pDoc->GetTrackTitle(i));
		pTrackBox->AddString(Text);
	}

	if (m_iTrack >= Count)
		m_iTrack = Count - 1;

	pTrackBox->SetCurSel(m_iTrack);
}

void CMainFrame::OnCbnSelchangeSong()
{
	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
	CFamiTrackerDoc	*pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	m_iTrack = pTrackBox->GetCurSel();

	pDoc->SelectTrack(m_iTrack);

	GetActiveView()->SetFocus();
}

void CMainFrame::OnCbnSelchangeOctave()
{
	CComboBox *TrackBox		= (CComboBox*)m_wndOctaveBar.GetDlgItem(IDC_OCTAVE);
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	unsigned int Octave		= TrackBox->GetCurSel();

	if (pView->GetOctave() != Octave)
		pView->SetOctave(Octave);
}

void CMainFrame::OnRemoveFocus()
{
	GetActiveView()->SetFocus();
}

void CMainFrame::OnNextSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
	
	if (m_iTrack < (signed)pDoc->GetTrackCount() - 1)
		pDoc->SelectTrack(m_iTrack + 1);
}

void CMainFrame::OnPrevSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);

	if (m_iTrack > 0)
		pDoc->SelectTrack(m_iTrack - 1);
}

void CMainFrame::OnUpdateNextSong(CCmdUI *pCmdUI)
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	if (GetSelectedTrack() < (signed)(pDoc->GetTrackCount() - 1))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdatePrevSong(CCmdUI *pCmdUI)
{
	if (GetSelectedTrack() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnClickedFollow()
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pView->SetFollowMode(m_wndOctaveBar.IsDlgButtonChecked(IDC_FOLLOW) != 0);
	pView->SetFocus();
}

void CMainFrame::OnToggleFollow()
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	m_wndOctaveBar.CheckDlgButton(IDC_FOLLOW, !m_wndOctaveBar.IsDlgButtonChecked(IDC_FOLLOW));
	pView->SetFollowMode(m_wndOctaveBar.IsDlgButtonChecked(IDC_FOLLOW) != 0);
	pView->SetFocus();
}

void CMainFrame::OnViewControlpanel()
{
	if (m_wndControlBar.IsVisible()) {
		m_wndControlBar.ShowWindow(SW_HIDE);
	}
	else {
		m_wndControlBar.ShowWindow(SW_SHOW);
		m_wndControlBar.UpdateWindow();
	}

	RecalcLayout();
	ResizeFrameWindow();
}

void CMainFrame::OnUpdateViewControlpanel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndControlBar.IsVisible());
}

void CMainFrame::OnUpdateHighlight(CCmdUI *pCmdUI)
{
	static int LastHighlight1, LastHighlight2;
	int Highlight1, Highlight2;
	Highlight1 = m_wndOctaveBar.GetDlgItemInt(IDC_HIGHLIGHT1);
	Highlight2 = m_wndOctaveBar.GetDlgItemInt(IDC_HIGHLIGHT2);
	if (Highlight1 != LastHighlight1 || Highlight2 != LastHighlight2) {

		CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

		pDoc->SetHighlight(Highlight1, Highlight2);
		pDoc->UpdateAllViews(NULL, UPDATE_HIGHLIGHT);

		LastHighlight1 = Highlight1;
		LastHighlight2 = Highlight2;
	}
}

void CMainFrame::OnSelectPatternEditor()
{
	GetActiveView()->SetFocus();
}

void CMainFrame::OnSelectFrameEditor()
{
	m_pFrameEditor->EnableInput();
}

void CMainFrame::OnModuleInsertFrame()
{
	AddAction(new CFrameAction(CFrameAction::ACT_ADD));
}

void CMainFrame::OnModuleRemoveFrame()
{
	AddAction(new CFrameAction(CFrameAction::ACT_REMOVE));
}

void CMainFrame::OnModuleDuplicateFrame()
{
	AddAction(new CFrameAction(CFrameAction::ACT_DUPLICATE));
}

void CMainFrame::OnModuleDuplicateFramePatterns()
{
	AddAction(new CFrameAction(CFrameAction::ACT_DUPLICATE_PATTERNS));
}

void CMainFrame::OnModuleMoveframedown()
{
	AddAction(new CFrameAction(CFrameAction::ACT_MOVE_DOWN));
}

void CMainFrame::OnModuleMoveframeup()
{
	AddAction(new CFrameAction(CFrameAction::ACT_MOVE_UP));
}

// UI updates

void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pCmdUI->Enable((pView->IsSelecting() || m_pFrameEditor->InputEnabled()) ? 1 : 0);
}

void CMainFrame::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pCmdUI->Enable((pView->IsClipboardAvailable() || m_pFrameEditor->InputEnabled()) ? 1 : 0);
}

void CMainFrame::OnHelpEffecttable()
{
   qDebug("HtmlHelp not done...");
//	// Display effect table in help
//	HtmlHelp((DWORD)_T("effect_list.htm"), HH_DISPLAY_TOPIC);
}

CFrameEditor *CMainFrame::GetFrameEditor() const
{
	return m_pFrameEditor;
}

void CMainFrame::OnEditEnableMIDI()
{
   qDebug("No MIDI support...");
//	theApp.GetMIDI()->ToggleInput();
}

void CMainFrame::OnUpdateEditEnablemidi(CCmdUI *pCmdUI)
{
   qDebug("No MIDI support...");
//	pCmdUI->Enable(theApp.GetMIDI()->IsAvailable());
//	pCmdUI->SetCheck(theApp.GetMIDI()->IsOpened());
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
   qDebug("CMainFrame::OnShowWindow");
//	CFrameWnd::OnShowWindow(bShow, nStatus);

//	if (bShow == TRUE) {
//		// Set the window state as saved in settings
//		if (theApp.GetSettings()->WindowPos.iState == STATE_MAXIMIZED)
//			CFrameWnd::ShowWindow(SW_MAXIMIZE);
//	}
}

void CMainFrame::OnDestroy()
{
	TRACE("FrameWnd: Destroying frame window\n");

	CSoundGen *pSoundGen = theApp.GetSoundGenerator();
	
	// Clean up sound stuff
	if (pSoundGen && pSoundGen->m_hThread != NULL) {
		// Remove sample window from sound generator
		pSoundGen->SetSampleWindow(NULL);
		// Kill the sound interface since the main window is being destroyed
		CEvent *pSoundEvent = new CEvent(FALSE, FALSE);
		pSoundGen->PostThreadMessage(WM_USER_CLOSE_SOUND, (WPARAM)pSoundEvent, NULL);
		// Wait for sound to close
      qDebug("WaitForSingleObject?");
//		::WaitForSingleObject(pSoundEvent->m_hObject, 5000);
		delete pSoundEvent;
	}

	CFrameWnd::OnDestroy();
}

void CMainFrame::ChangedTrack()
{
	// Called from the view
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	m_iTrack = pDoc->GetSelectedTrack();
	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
	pTrackBox->SetCurSel(m_iTrack);
}

int CMainFrame::GetSelectedTrack() const
{
	return m_iTrack;
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   qDebug("CMainFrame::OnNotify");
//    LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR) lParam;
	
//	// Handle new instrument menu
//	switch (((LPNMHDR)lParam)->code) {
//		case TBN_DROPDOWN:
//			switch (lpnmtb->iItem) {
//				case ID_INSTRUMENT_NEW:
//					OnNewInstrumentMenu((LPNMHDR)lParam, pResult);
//					return FALSE;
//				case ID_INSTRUMENT_LOAD:
//					OnLoadInstrumentMenu((LPNMHDR)lParam, pResult);
//					return FALSE;
//			}
//	}

//	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

void CMainFrame::OnNewInstrumentMenu( NMHDR * pNotifyStruct, LRESULT * result )
{
	CRect rect;
	::GetWindowRect(pNotifyStruct->hwndFrom, &rect);

	CMenu menu;
	menu.CreatePopupMenu();

	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	CFamiTrackerView *pView = (CFamiTrackerView*)GetActiveView();
	
	int Chip = pDoc->GetExpansionChip();
	int SelectedChip = pDoc->GetChannel(pView->GetSelectedChannel())->GetChip();	// where the cursor is located

	menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_2A03, _T("New 2A03 instrument"));

	if (Chip & SNDCHIP_VRC6)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_VRC6, _T("New VRC6 instrument"));
	if (Chip & SNDCHIP_VRC7)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_VRC7, _T("New VRC7 instrument"));
	if (Chip & SNDCHIP_FDS)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_FDS, _T("New FDS instrument"));
	if (Chip & SNDCHIP_MMC5)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_MMC5, _T("New MMC5 instrument"));
	if (Chip & SNDCHIP_N163)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_N163, _T("New Namco instrument"));
	if (Chip & SNDCHIP_S5B)
		menu.AppendMenu(MF_STRING, ID_INSTRUMENT_ADD_S5B, _T("New Sunsoft instrument"));

	switch (SelectedChip) {
		case SNDCHIP_NONE:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_2A03);
			break;
		case SNDCHIP_VRC6:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_VRC6);
			break;
		case SNDCHIP_VRC7:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_VRC7);
			break;
		case SNDCHIP_FDS:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_FDS);
			break;
		case SNDCHIP_MMC5:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_MMC5);
			break;
		case SNDCHIP_N163:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_N163);
			break;
		case SNDCHIP_S5B:
			menu.SetDefaultItem(ID_INSTRUMENT_ADD_S5B);
			break;
	}
	
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, this);
}

void CMainFrame::OnLoadInstrumentMenu(NMHDR * pNotifyStruct, LRESULT * result)
{
	CRect rect;
	::GetWindowRect(pNotifyStruct->hwndFrom, &rect);

	// Build menu tree
	if (!m_pInstrumentFileTree) {
		m_pInstrumentFileTree = new CInstrumentFileTree();
		m_pInstrumentFileTree->BuildMenuTree(theApp.GetSettings()->InstrumentMenuPath);
	}
	else if (m_pInstrumentFileTree->ShouldRebuild()) {
		m_pInstrumentFileTree->BuildMenuTree(theApp.GetSettings()->InstrumentMenuPath);
	}

	UINT retValue = m_pInstrumentFileTree->GetMenu()->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, rect.left, rect.bottom, this);

	if (retValue == CInstrumentFileTree::MENU_BASE) {
		// Open file
		OnLoadInstrument();
	}
	else if (retValue == CInstrumentFileTree::MENU_BASE + 1) {
		// Select dir
		SelectInstrumentFolder();
	}
	else if (retValue >= CInstrumentFileTree::MENU_BASE + 2) {
		// A file
		CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

		int Index = pDoc->LoadInstrument(m_pInstrumentFileTree->GetFile(retValue));

		if (Index == -1)
			return;

		AddInstrument(Index);
	}
}

void CMainFrame::SelectInstrumentFolder()
{
   qDebug("CMainFrame::SelectInstrumentFolder");
//	BROWSEINFOA Browse;	
//	LPITEMIDLIST lpID;
//	char Path[MAX_PATH];

//	Browse.lpszTitle	= _T("Choose a folder where you have instrument files");
//	Browse.hwndOwner	= m_hWnd;
//	Browse.pidlRoot		= NULL;
//	Browse.lpfn			= NULL;
//	Browse.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
//	Browse.pszDisplayName = Path;

//	lpID = SHBrowseForFolder(&Browse);

//	if (lpID != NULL) {
//		SHGetPathFromIDList(lpID, Path);
//		theApp.GetSettings()->InstrumentMenuPath = Path;
//		m_pInstrumentFileTree->Changed();
//	}
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
   qDebug("CMainFrame::OnCopyData");
//	switch (pCopyDataStruct->dwData) {
//		case IPC_LOAD:
//			// Load file
//			if (_tcslen((LPCTSTR)pCopyDataStruct->lpData) > 0)
//				theApp.OpenDocumentFile((LPCTSTR)pCopyDataStruct->lpData);
//			return TRUE;
//		case IPC_LOAD_PLAY:
//			// Load file
//			if (_tcslen((LPCTSTR)pCopyDataStruct->lpData) > 0)
//				theApp.OpenDocumentFile((LPCTSTR)pCopyDataStruct->lpData);
//			// and play
//			if (CFamiTrackerDoc::GetDoc()->IsFileLoaded() &&
//				!CFamiTrackerDoc::GetDoc()->HasLastLoadFailed())
//				theApp.GetSoundGenerator()->StartPlayer(MODE_PLAY_START);
//			return TRUE;
//	}

//	return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

bool CMainFrame::AddAction(CAction *pAction)
{
	if (!m_pActionHandler)
		return false;

	if (!pAction->SaveState(this)) {
		// Operation cancelled
		SAFE_RELEASE(pAction);
		return false;
	}

	m_pActionHandler->Push(pAction);

	return true;
}

CAction *CMainFrame::GetLastAction() const
{
	if (!m_pActionHandler)
		return NULL;

	return m_pActionHandler->GetLastAction();
}

void CMainFrame::ResetUndo()
{
	if (!m_pActionHandler)
		return;

	m_pActionHandler->Clear();
}

void CMainFrame::OnEditUndo()
{
	if (!m_pActionHandler)
		return;

	CAction *pAction = m_pActionHandler->PopUndo();

	if (pAction != NULL)
		pAction->Undo(this);
}

void CMainFrame::OnEditRedo()
{
	if (!m_pActionHandler)
		return;

	CAction *pAction = m_pActionHandler->PopRedo();

	if (pAction != NULL)
		pAction->Redo(this);
}

void CMainFrame::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	if (!m_pActionHandler)
		return;

	pCmdUI->Enable(m_pActionHandler->CanUndo() ? 1 : 0);
}

void CMainFrame::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	if (!m_pActionHandler)
		return;

	pCmdUI->Enable(m_pActionHandler->CanRedo() ? 1 : 0);
}

void CMainFrame::UpdateMenus()
{
	// Write new shortcuts to menus
	UpdateMenu(GetMenu());
}

void CMainFrame::UpdateMenu(CMenu *pMenu)
{
	CAccelerator *pAccel = theApp.GetAccelerator();

	for (UINT i = 0; i < pMenu->GetMenuItemCount(); ++i) {
		UINT state = pMenu->GetMenuState(i, MF_BYPOSITION);
		if (state & MF_POPUP) {
			// Update sub menu
			UpdateMenu(pMenu->GetSubMenu(i));
		}
		else if ((state & MF_SEPARATOR) == 0) {
			// Change menu name
			CString shortcut;
			UINT id = pMenu->GetMenuItemID(i);

			if (pAccel->GetShortcutString(id, shortcut)) {
				CString string;
				pMenu->GetMenuString(i, string, MF_BYPOSITION);

				int tab = string.Find('\t');

				if (tab != -1) {
					string = string.Left(tab);
				}

				string += shortcut;
				pMenu->ModifyMenu(i, MF_BYPOSITION, id, string);
			}
		}
	}
}

void CMainFrame::OnEditCopy()
{
	if (GetFocus() == GetActiveView())
		((CFamiTrackerView*)GetActiveView())->OnEditCopy();
	else if (GetFocus() == GetFrameEditor())
		GetFrameEditor()->OnFrameCopy();
}

void CMainFrame::OnEditPaste()
{
	if (GetFocus() == GetActiveView())
		((CFamiTrackerView*)GetActiveView())->OnEditPaste();
	else if (GetFocus() == GetFrameEditor())
		GetFrameEditor()->OnFramePaste();
}

void CMainFrame::OnDecayFast()
{
	// TODO add this
}

void CMainFrame::OnDecaySlow()
{
	// TODO add this
}

void CMainFrame::OnEditExpandpatterns()
{
	AddAction(new CPatternAction(CPatternAction::ACT_EXPAND_PATTERN));
}

void CMainFrame::OnEditShrinkpatterns()
{
	AddAction(new CPatternAction(CPatternAction::ACT_SHRINK_PATTERN));
}

void CMainFrame::OnUpdateSelectionEnabled(CCmdUI *pCmdUI)
{
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	pCmdUI->Enable((pView->IsSelecting()) ? 1 : 0);
}

void CMainFrame::SetFrameEditorPosition(int Position)
{
	// Change frame editor position
	m_iFrameEditorPos = Position;

	m_pFrameEditor->ShowWindow(SW_HIDE);

	switch (Position) {
		case FRAME_EDIT_POS_TOP:
			m_wndVerticalControlBar.ShowWindow(SW_HIDE);
			m_pFrameEditor->SetParent(&m_wndControlBar);
			m_wndFrameControls.SetParent(&m_wndControlBar);
			break;
		case FRAME_EDIT_POS_LEFT:
			m_wndVerticalControlBar.ShowWindow(SW_SHOW);
			m_pFrameEditor->SetParent(&m_wndVerticalControlBar);
			m_wndFrameControls.SetParent(&m_wndVerticalControlBar);
			break;
	}

	ResizeFrameWindow();

	m_pFrameEditor->ShowWindow(SW_SHOW);
	m_pFrameEditor->Invalidate();
	m_pFrameEditor->RedrawWindow();

	ResizeFrameWindow();	// This must be called twice or the editor disappears, I don't know why

	// Save to settings
	theApp.GetSettings()->FrameEditPos = Position;
}

void CMainFrame::OnFrameeditorTop()
{
	SetFrameEditorPosition(FRAME_EDIT_POS_TOP);
}

void CMainFrame::OnFrameeditorLeft()
{
	SetFrameEditorPosition(FRAME_EDIT_POS_LEFT);
}

void CMainFrame::OnUpdateFrameeditorTop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_iFrameEditorPos == FRAME_EDIT_POS_TOP);
}

void CMainFrame::OnUpdateFrameeditorLeft(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_iFrameEditorPos == FRAME_EDIT_POS_LEFT);
}

void CMainFrame::OnToggleSpeed()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
	int Speed = pDoc->GetSpeedSplitPoint();
	
	if (Speed == DEFAULT_SPEED_SPLIT_POINT) 
		Speed = OLD_SPEED_SPLIT_POINT;
	else 
		Speed = DEFAULT_SPEED_SPLIT_POINT;

	pDoc->SetSpeedSplitPoint(Speed);
	SetStatusText("Speed/tempo split-point set to %i", Speed);
}
