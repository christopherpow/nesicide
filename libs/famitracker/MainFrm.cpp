#include "stdafx.h"
#include "MainFrm.h"
#include "FamiTracker.h"
#include "FamiTrackerView.h"
#include "SampleWindow.h"
#include "ChannelsDlg.h"
#include "CommentsDlg.h"
#include "ModulePropertiesDlg.h"
#include "ControlPanelDlg.h"
#include "ExportDialog.h"
#include "Accelerator.h"
#include "Settings.h"
#include "InstrumentFileTree.h"

typedef void (CMainFrame::*actionHandler)();

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

CMainFrame::CMainFrame(CWnd *parent) :
   CFrameWnd(parent),
   m_iInstrument(0),
   m_iTrack(0),
   m_pFrameEditor(0),
   m_pInstrumentFileTree(NULL),
   initialized(false)
{
   CREATESTRUCT cs;
   OnCreate(&cs);
   
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
      
      // Connect buried signals.
      qDebug("START CONNECTING BURIED SIGNALS NOW...");
      QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));      
      QObject::connect(&m_wndToolBar,SIGNAL(toolBarAction_triggered(int)),this,SLOT(toolBarAction_triggered(int)));
      QObject::connect(&m_wndInstToolBar,SIGNAL(toolBarAction_triggered(int)),this,SLOT(instToolBarAction_triggered(int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_OCTAVE)->toQWidget(),SIGNAL(currentIndexChanged(int)),this,SLOT(octave_currentIndexChanged(int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_FOLLOW)->toQWidget(),SIGNAL(clicked()),this,SLOT(follow_clicked()));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHT1)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(highlight1_textChanged(QString)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHTSPIN1)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(highlightspin1_valueChanged(int)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHT2)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(highlight2_textChanged(QString)));
      QObject::connect(m_wndOctaveBar.GetDlgItem(IDC_HIGHLIGHTSPIN2)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(highlightspin2_valueChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->toQWidget(),SIGNAL(doubleClicked(QModelIndex)),this,SLOT(instruments_doubleClicked(QModelIndex)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->toQWidget(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(instruments_currentChanged(QModelIndex,QModelIndex)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SPEED)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(speed_textChanged(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SPEED_SPIN)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(speedSpin_valueChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_TEMPO)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(tempo_textChanged(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_TEMPO_SPIN)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(tempoSpin_valueChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_ROWS)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(rows_textChanged(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_ROWS_SPIN)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(rowsSpin_valueChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_FRAMES)->toQWidget(),SIGNAL(textChanged(QString)),this,SLOT(frames_textChanged(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_FRAME_SPIN)->toQWidget(),SIGNAL(valueChanged(int)),this,SLOT(framesSpin_valueChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SUBTUNE)->toQWidget(),SIGNAL(currentIndexChanged(int)),this,SLOT(subtune_currentIndexChanged(int)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_NAME)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songName_textEdited(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_ARTIST)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songArtist_textEdited(QString)));
      QObject::connect(m_wndDialogBar.GetDlgItem(IDC_SONG_COPYRIGHT)->toQWidget(),SIGNAL(textEdited(QString)),this,SLOT(songCopyright_textEdited(QString)));
//      void frameChangeAll_clicked(bool checked);
//      void frameInc_clicked();   
//      void frameDec_clicked();
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

void CMainFrame::idleProcessing()
{   
   CCmdUI cmdUI;
   
   cmdUI.m_nIndex = -1;
   
   cmdUI.m_nID = ID_INDICATOR_CHIP;
   cmdUI.m_pOther = m_wndStatusBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBChip(&cmdUI);
   }
   cmdUI.m_nID = ID_INDICATOR_OCTAVE;
   cmdUI.m_pOther = m_wndStatusBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBOctave(&cmdUI);
   }
   cmdUI.m_nID = ID_INDICATOR_INSTRUMENT;
   cmdUI.m_pOther = m_wndStatusBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBInstrument(&cmdUI);
   }
   cmdUI.m_nID = ID_INDICATOR_RATE;
   cmdUI.m_pOther = m_wndStatusBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBFrequency(&cmdUI);
   }
   cmdUI.m_nID = ID_INDICATOR_TEMPO;
   cmdUI.m_pOther = m_wndStatusBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSBTempo(&cmdUI);
   }
   cmdUI.m_nID = IDC_HIGHLIGHT1;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   cmdUI.m_nID = IDC_HIGHLIGHTSPIN1;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   cmdUI.m_nID = IDC_HIGHLIGHT2;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   cmdUI.m_nID = IDC_HIGHLIGHTSPIN2;
   cmdUI.m_pOther = m_wndOctaveBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateHighlight(&cmdUI);
   }
   
   cmdUI.m_nID = IDC_SPEED;
   cmdUI.m_pOther = m_wndDialogBar.GetDlgItem(cmdUI.m_nID);
   if ( cmdUI.m_pOther )
   {
      OnUpdateSpeedEdit(&cmdUI);
   }
}

void CMainFrame::updateViews(long hint)
{
}

void CMainFrame::toolBarAction_triggered(int id)
{
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
   qDebug("previousTrack");
   OnPrevSong();
}

void CMainFrame::toolBarAction_nextTrack()
{
   qDebug("nextTrack");
   OnNextSong();
}

void CMainFrame::toolBarAction_settings()
{
   qDebug("settings");
}

void CMainFrame::toolBarAction_createNSF()
{
   qDebug("createNSF");
   OnCreateNSF();
}

void CMainFrame::instToolBarAction_triggered(int id)
{
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
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->IncreaseCurrentPattern();
}

void CMainFrame::frameDec_clicked()
{
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->DecreaseCurrentPattern();
}

void CMainFrame::frameChangeAll_clicked(bool checked)
{
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->SetChangeAllPattern(checked);
}

void CMainFrame::instruments_currentChanged(const QModelIndex &index,const QModelIndex &)
{
   NM_LISTVIEW nmlv;
   LRESULT result;
   
   nmlv.uNewState = LVIS_SELECTED;
   nmlv.iItem = index.row();
   OnChangedInstruments((NMHDR*)&nmlv,&result);
}

void CMainFrame::instruments_doubleClicked(const QModelIndex &index)
{
   NMHDR nmhdr;
   LRESULT result;
   OnDblClkInstruments(&nmhdr,&result);
}

void CMainFrame::speedSpin_valueChanged(int arg1)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg1;
   nmud.iDelta = arg1-CFamiTrackerDoc::GetDoc()->GetSongSpeed();
   OnDeltaposSpeedSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::tempoSpin_valueChanged(int arg1)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg1;
   nmud.iDelta = arg1-CFamiTrackerDoc::GetDoc()->GetSongTempo();
   OnDeltaposTempoSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::rowsSpin_valueChanged(int arg1)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg1;
   nmud.iDelta = arg1-CFamiTrackerDoc::GetDoc()->GetPatternLength();
   OnDeltaposRowsSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::framesSpin_valueChanged(int arg1)
{
   NMUPDOWN nmud;
   LRESULT result;

   nmud.iPos = arg1;
   nmud.iDelta = arg1-CFamiTrackerDoc::GetDoc()->GetFrameCount();
   OnDeltaposFrameSpin((NMHDR*)&nmud,&result);
}

void CMainFrame::subtune_currentIndexChanged(int index)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   if ( index >= 0 )
   {
      pDoc->SelectTrack(index);
   }
}

void CMainFrame::songName_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongName(arg1.toAscii().data());
}

void CMainFrame::songArtist_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongArtist(arg1.toAscii().data());
}

void CMainFrame::songCopyright_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongCopyright(arg1.toAscii().data());
}

void CMainFrame::follow_clicked()
{
   OnClickedFollow();
}

void CMainFrame::octave_currentIndexChanged(int)
{
   OnCbnSelchangeOctave();
}

void CMainFrame::highlight1_textChanged(QString)
{
   
}

void CMainFrame::highlightspin1_valueChanged(int)
{
}

void CMainFrame::highlight2_textChanged(QString)
{
}

void CMainFrame::highlightspin2_valueChanged(int)
{
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

//	// Initial message, 100ms
//	SetTimer(TMR_WELCOME, 100, NULL);

//	// Periodic synth check, 50ms
//	SetTimer(TMR_SOUND_CHECK, 50, NULL);

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

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

   qDebug("OnSize not quite done...");
//	if (m_wndToolBarReBar.m_hWnd == NULL)
//		return;

//	m_wndToolBarReBar.GetReBarCtrl().MinimizeBand(0);

	ResizeFrameWindow();
}

void CMainFrame::UpdateMenus()
{
	// Write new shortcuts to menus
	UpdateMenu(GetMenu());
}

void CMainFrame::UpdateMenu(CMenu *pMenu)
{
	CAccelerator *pAccel = theApp.GetAccelerator();

   qDebug("CMainFrame::UpdateMenu");
//	for (UINT i = 0; i < pMenu->GetMenuItemCount(); ++i) {
//		UINT state = pMenu->GetMenuState(i, MF_BYPOSITION);
//		if (state & MF_POPUP) {
//			// Update sub menu
//			UpdateMenu(pMenu->GetSubMenu(i));
//		}
//		else if ((state & MF_SEPARATOR) == 0) {
//			// Change menu name
//			CString shortcut;
//			UINT id = pMenu->GetMenuItemID(i);

//			if (pAccel->GetShortcutString(id, shortcut)) {
//				CString string;
//				pMenu->GetMenuString(i, string, MF_BYPOSITION);

//				int tab = string.Find('\t');

//				if (tab != -1) {
//					string = string.Left(tab);
//				}

//				string += shortcut;
//				pMenu->ModifyMenu(i, MF_BYPOSITION, id, string);
//			}
//		}
//	}
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

CFrameEditor *CMainFrame::GetFrameEditor() const
{
   return m_pFrameEditor;
}

void CMainFrame::UpdateControls()
{
	m_wndDialogBar.UpdateDialogControls(&m_wndDialogBar, TRUE);
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

void CMainFrame::CloseInstrumentEditor()
{
	if (m_wndInstEdit.IsOpened())
		m_wndInstEdit.DestroyWindow();
}

void CMainFrame::ClearInstrumentList()
{
	m_pInstrumentList->DeleteAllItems();
	m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
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

void CMainFrame::SetSongInfo(char *Name, char *Artist, char *Copyright)
{
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_NAME, Name);
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_ARTIST, Artist);
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_COPYRIGHT, Copyright);
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

void CMainFrame::SetStatusText(LPCTSTR Text,...)
{
}

void CMainFrame::OnNextSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
	
	if (m_iTrack < (signed)pDoc->GetTrackCount() - 1)
		pDoc->SelectTrack(m_iTrack + 1);
}

void CMainFrame::OnPrevSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);

	if (m_iTrack > 0)
		pDoc->SelectTrack(m_iTrack - 1);
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

void CMainFrame::OnModuleModuleproperties()
{
	// Display module properties dialog
	CModulePropertiesDlg propertiesDlg;
	propertiesDlg.DoModal();
   
   // CP: TEST
   NMHDR nmhdr;
   LRESULT result;
   OnDblClkInstruments(&nmhdr,&result);
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
		else 
      {
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

bool CMainFrame::CreateToolbars()
{
	REBARBANDINFO rbi1;

	// Add the toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))  {
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}

//	m_wndToolBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

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
      
//	m_wndToolBarReBar.GetReBarCtrl().MinimizeBand(0);

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

	if (!m_pFrameEditor->CreateEx(WS_EX_STATICEDGE, NULL, _T(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, (CWnd*)&m_wndControlBar, 0)) {
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

   qDebug("Subclassing...");
//	m_pLockedEditSpeed->SubclassDlgItem(IDC_SPEED, &m_wndDialogBar);
//	m_pLockedEditTempo->SubclassDlgItem(IDC_TEMPO, &m_wndDialogBar);
//	m_pLockedEditLength->SubclassDlgItem(IDC_ROWS, &m_wndDialogBar);
//	m_pLockedEditFrames->SubclassDlgItem(IDC_FRAMES, &m_wndDialogBar);
//	m_pLockedEditStep->SubclassDlgItem(IDC_KEYSTEP, &m_wndDialogBar);

	// Subclass and setup the instrument list
//	m_pInstrumentList = new CInstrumentList(this);
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
//	m_pBannerEditName = new CBannerEdit(CString(_T("(title)")));
//	m_pBannerEditArtist = new CBannerEdit(CString(_T("(author)")));
//	m_pBannerEditCopyright = new CBannerEdit(CString(_T("(copyright)")));

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
   
//	m_wndInstToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	// Set 24-bit icons
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
   qDebug("CToolBar::SetButtonStyle");
//	m_wndInstToolBar.SetButtonStyle(0, TBBS_DROPDOWN);
	// Turn load instrument button into a drop-down list
//	m_wndInstToolBar.SetButtonStyle(4, TBBS_DROPDOWN);

	return true;
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
	int Instrument = 0;
	TCHAR Text[256];
	m_pInstrumentList->GetItemText(ListIndex, 0, Text, 256);
	_stscanf(Text, _T("%X"), &Instrument);
	return Instrument;
}

void CMainFrame::ChangeNoteState(int Note)
{
	m_wndInstEdit.ChangeNoteState(Note);
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

void CMainFrame::OnCreateNSF()
{
	CExportDialog ExportDialog(this);
	ExportDialog.DoModal();
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

void CMainFrame::OnCbnSelchangeOctave()
{
	CComboBox *TrackBox		= (CComboBox*)m_wndOctaveBar.GetDlgItem(IDC_OCTAVE);
	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
	unsigned int Octave		= TrackBox->GetCurSel();

	if (pView->GetOctave() != Octave)
		pView->SetOctave(Octave);
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
