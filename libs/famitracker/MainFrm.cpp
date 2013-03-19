#include "stdafx.h"
#include "MainFrm.h"
#include "ui_MainFrm.h"
#include "FamiTracker.h"
#include "FamiTrackerView.h"
#include "SampleWindow.h"

#include "famitrackermodulepropertiesdialog.h"

#include <QFrame>
#include <QLayout>
#include <QAction>

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
   ui(new Ui::CMainFrame),
   m_iInstrument(0),
   m_iTrack(0),
   m_pFrameEditor(0),
   initialized(false)
{
   int idx;
   int col;
   
   ui->setupUi(toQWidget());
   
   _dpiX = DEFAULT_DPI;
	_dpiY = DEFAULT_DPI;

	m_iFrameEditorPos = FRAME_EDIT_POS_TOP;
         
   m_pActionHandler = new CActionHandler();

   if (!CreateSampleWindow()) {
		TRACE0("Failed to create sample window\n");
//		return -1;      // fail to create
	}

   actionHandler actionHandlers[] = 
   {
      &CMainFrame::trackerAction_newDocument,
      &CMainFrame::trackerAction_openDocument,
      &CMainFrame::trackerAction_saveDocument,
      &CMainFrame::trackerAction_editCut,
      &CMainFrame::trackerAction_editCopy,
      &CMainFrame::trackerAction_editPaste,
      &CMainFrame::trackerAction_about,
      &CMainFrame::trackerAction_help,
      &CMainFrame::trackerAction_addFrame,
      &CMainFrame::trackerAction_removeFrame,
      &CMainFrame::trackerAction_moveFrameDown,
      &CMainFrame::trackerAction_moveFrameUp,
      &CMainFrame::trackerAction_duplicateFrame,
      &CMainFrame::trackerAction_moduleProperties,
      &CMainFrame::trackerAction_play,
      &CMainFrame::trackerAction_playLoop,
      &CMainFrame::trackerAction_stop,
      &CMainFrame::trackerAction_editMode,
      &CMainFrame::trackerAction_previousTrack,
      &CMainFrame::trackerAction_nextTrack,
      &CMainFrame::trackerAction_settings,
      &CMainFrame::trackerAction_createNSF
   };
   
   QImage toolBarImage(":/resources/Toolbar-d5.bmp");

   toolBar = new QToolBar("Music Editor");
   toolBar->setObjectName("musicEditorToolBar");

   QAction* trackerAction;
   for ( col = 0, idx = 0; col < toolBarImage.width(); col += 16, idx++ )
   {
      trackerAction = new QAction(this);
      QPixmap trackerActionPixmap = QPixmap::fromImage(toolBarImage.copy(col,0,16,16)).scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
      trackerAction->setIcon(QIcon(trackerActionPixmap));
      QObject::connect(trackerAction,SIGNAL(triggered()),this,SLOT(trackerAction_triggered()));
      trackerActions.insert(trackerAction,actionHandlers[idx]);
      toolBar->addAction(trackerAction);
   }
   
   octaveLabel = new QLabel("Octave");
   toolBar->addSeparator();
   toolBar->addWidget(octaveLabel);
   octaveComboBox = new QComboBox();
   octaveComboBox->addItem("0");
   octaveComboBox->addItem("1");
   octaveComboBox->addItem("2");
   octaveComboBox->addItem("3");
   octaveComboBox->addItem("4");
   octaveComboBox->addItem("5");
   octaveComboBox->addItem("6");
   octaveComboBox->addItem("7");
   toolBar->addWidget(octaveComboBox);

   instrumentsModel = new CMusicFamiTrackerInstrumentsModel();
   
   ui->songInstruments->setModel(instrumentsModel);

#ifdef Q_WS_MAC
   ui->songInstruments->setFont(QFont("Monaco",9));
#endif
#ifdef Q_WS_X11
   ui->songInstruments->setFont(QFont("Monospace",8));
#endif
#ifdef Q_WS_WIN
   ui->songInstruments->setFont(QFont("Consolas",9));
#endif

   ui->songInstruments->setStyleSheet("QListView { background: #000000; color: #ffffff; }");
   
   QObject::connect(ui->songInstruments->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(songInstruments_currentChanged(QModelIndex,QModelIndex)));
   QObject::connect(ui->songInstruments,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(songInstruments_doubleClicked(QModelIndex)));
   
   CComboBox* mfc1 = new CComboBox;
   ui->songsGroupBox->layout()->addWidget(mfc1->toQWidget());
   mfcToQtWidget.insert(IDC_SUBTUNE,mfc1);
}

CMainFrame::~CMainFrame()
{
   delete ui;
   delete instrumentsModel;
   SAFE_RELEASE(m_pFrameEditor);
   delete m_pView;
   delete m_pDocument;
   delete octaveLabel;
   delete octaveComboBox;
   delete toolBar;
}

void CMainFrame::focusInEvent(QFocusEvent *)
{
   m_pView->GetPatternView()->SetFocus(true);
   m_pFrameEditor->SetFocus();
   m_pView->GetPatternView()->setFocus();
}

void CMainFrame::showEvent(QShowEvent *)
{
   emit addToolBarWidget(toolBar);
   toolBar->setVisible(true);

   if ( !initialized )
   {
      // Perform initialization that couldn't yet be done in the constructor due to MFC crap.
      m_pDocument = (CFamiTrackerDoc*)GetActiveDocument();
      m_pDocument->SetTitle("Untitled");
      
      m_pView = (CFamiTrackerView*)GetActiveView();
      
      // Create frame editor
      m_pFrameEditor = new CFrameEditor(this);
   
      CRect rect(SX(12), SY(10), SX(162), SY(173));
   
//      if (!m_pFrameEditor->CreateEx(WS_EX_STATICEDGE, NULL, "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, (CWnd*)&m_wndControlBar, 0)) {
      if (!m_pFrameEditor->CreateEx(WS_EX_STATICEDGE, NULL, _T(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, (CWnd*)NULL, 0)) {
         TRACE0("Failed to create pattern window\n");
      }

      ui->songFrames->layout()->addWidget(m_pFrameEditor->toQWidget());
      
      ui->songPatterns->layout()->addWidget(m_pView->toQWidget());
   
      m_pFrameEditor->setFocusPolicy(Qt::StrongFocus);
      m_pView->setFocusPolicy(Qt::StrongFocus);
      
      QObject::connect(m_pDocument,SIGNAL(updateViews(long)),m_pFrameEditor,SLOT(updateViews(long)));
      QObject::connect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
      
      m_pView->OnInitialUpdate();
      
      // Connect buried signals.
      QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));
      
      initialized = true;
   }
   setFocus();
}

void CMainFrame::hideEvent(QHideEvent *)
{
   emit removeToolBarWidget(toolBar);
}

void CMainFrame::resizeEvent(QResizeEvent *)
{
   if ( m_pFrameEditor )
      ResizeFrameWindow();
}

void CMainFrame::updateViews(long hint)
{
   ui->tempo->setValue(m_pDocument->GetSongTempo());
   ui->speed->setValue(m_pDocument->GetSongSpeed());
   ui->title->setText(m_pDocument->GetSongName());
   ui->author->setText(m_pDocument->GetSongArtist());
   ui->copyright->setText(m_pDocument->GetSongCopyright());
   ui->numFrames->setValue(m_pDocument->GetFrameCount());
   ui->numRows->setValue(m_pDocument->GetPatternLength());
}

void CMainFrame::trackerAction_triggered()
{
   QMap<QAction*,actionHandler>::const_iterator action = trackerActions.find(dynamic_cast<QAction*>(sender()));
   if ( action.value() )
   {
      (this->*(action.value()))();
   }
}

void CMainFrame::trackerAction_newDocument()
{
   qDebug("newDocument");
}

void CMainFrame::trackerAction_openDocument()
{
   qDebug("openDocument");
}

void CMainFrame::trackerAction_saveDocument()
{
   qDebug("saveDocument");
}

void CMainFrame::trackerAction_editCut()
{
   qDebug("editCut");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditCut();
}

void CMainFrame::trackerAction_editCopy()
{
   qDebug("editCopy");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditCopy();
}

void CMainFrame::trackerAction_editPaste()
{
   qDebug("editPaste");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnEditPaste();   
}

void CMainFrame::trackerAction_about()
{
   qDebug("about");
}

void CMainFrame::trackerAction_help()
{
   qDebug("help");
}

void CMainFrame::trackerAction_addFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_ADD));
}

void CMainFrame::trackerAction_removeFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_REMOVE));
}

void CMainFrame::trackerAction_moveFrameDown()
{
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_DOWN));
}

void CMainFrame::trackerAction_moveFrameUp()
{
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_UP));
}

void CMainFrame::trackerAction_duplicateFrame()
{
   AddAction(new CFrameAction(CFrameAction::ACT_DUPLICATE));
}

void CMainFrame::trackerAction_moduleProperties()
{
   qDebug("moduleProperties");
   FamiTrackerModulePropertiesDialog dlg(m_pDocument);

   dlg.exec();
   
   // The song list may have changed, rebuild it from the model.
   qDebug("NOW FIGURE OUT WHAT TO DO WITH THE ACTION...");
//   ui->songs->clear();
//   ui->songs->addItems(dlg.tracks());
   
   instrumentsModel->update();
}

void CMainFrame::trackerAction_play()
{
   qDebug("play");
   theApp.OnTrackerPlay();
}

void CMainFrame::trackerAction_playLoop()
{
   qDebug("playLoop");
   theApp.OnTrackerPlaypattern();
}

void CMainFrame::trackerAction_stop()
{
   qDebug("stop");
   theApp.OnTrackerStop();
}

void CMainFrame::trackerAction_editMode()
{
   qDebug("editMode");
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->OnTrackerEdit();
}

void CMainFrame::trackerAction_previousTrack()
{
   qDebug("previousTrack");
   OnPrevSong();
}

void CMainFrame::trackerAction_nextTrack()
{
   qDebug("nextTrack");
   OnNextSong();
}

void CMainFrame::trackerAction_settings()
{
   qDebug("settings");
}

void CMainFrame::trackerAction_createNSF()
{
   qDebug("createNSF");
}

void CMainFrame::on_frameInc_clicked()
{
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->IncreaseCurrentPattern();
}

void CMainFrame::on_frameDec_clicked()
{
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->DecreaseCurrentPattern();
}

void CMainFrame::on_speed_valueChanged(int arg1)
{
	SetSpeed(arg1);
}

void CMainFrame::on_tempo_valueChanged(int arg1)
{
   SetTempo(arg1);
}

void CMainFrame::on_numRows_valueChanged(int NewRows)
{
   SetRowCount(NewRows);
}

void CMainFrame::on_numFrames_valueChanged(int NewFrames)
{
   SetFrameCount(NewFrames);
}

void CMainFrame::on_songs_currentIndexChanged(int index)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   if ( index >= 0 )
   {
      pDoc->SelectTrack(index);
   }
   
   instrumentsModel->update();

   ui->tempo->setValue(pDoc->GetSongTempo());
   ui->speed->setValue(pDoc->GetSongSpeed());
   ui->numRows->setValue(pDoc->GetPatternLength());
   ui->numFrames->setValue(pDoc->GetFrameCount());
}

void CMainFrame::on_title_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongName(arg1.toAscii().data());
}

void CMainFrame::on_author_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongArtist(arg1.toAscii().data());
}

void CMainFrame::on_copyright_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   pDoc->SetSongCopyright(arg1.toAscii().data());
}

void CMainFrame::setFileName(QString fileName)
{
   m_fileName = fileName;

   GetActiveDocument()->OnOpenDocument((TCHAR*)fileName.unicode());
   
   instrumentsModel->setDocument((CFamiTrackerDoc*)GetActiveDocument());
   instrumentsModel->update();
}

void CMainFrame::SetTempo(int Tempo)
{
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	int MinTempo = pDoc->GetSpeedSplitPoint();
	LIMIT(Tempo, MAX_TEMPO, MinTempo);
	pDoc->SetSongTempo(Tempo);
	theApp.GetSoundGenerator()->ResetTempo();

//	if (m_wndDialogBar.GetDlgItemInt(IDC_TEMPO) != Tempo)
//		m_wndDialogBar.SetDlgItemInt(IDC_TEMPO, Tempo, FALSE);
}

void CMainFrame::SetSpeed(int Speed)
{
	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	int MaxSpeed = pDoc->GetSpeedSplitPoint() - 1;
	LIMIT(Speed, MaxSpeed, MIN_SPEED);
	pDoc->SetSongSpeed(Speed);
	theApp.GetSoundGenerator()->ResetTempo();

//	if (m_wndDialogBar.GetDlgItemInt(IDC_SPEED) != Speed)
//		m_wndDialogBar.SetDlgItemInt(IDC_SPEED, Speed, FALSE);
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

//	if (m_wndDialogBar.GetDlgItemInt(IDC_ROWS) != Count)
//		m_wndDialogBar.SetDlgItemInt(IDC_ROWS, Count, FALSE);
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

//	if (m_wndDialogBar.GetDlgItemInt(IDC_FRAMES) != Count)
//		m_wndDialogBar.SetDlgItemInt(IDC_FRAMES, Count, FALSE);
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
   qDebug("UpdateControls");
   
//   ui->title->setText(pDoc->GetSongName());
//   ui->author->setText(pDoc->GetSongArtist());
//   ui->copyright->setText(pDoc->GetSongCopyright());
//   ui->tempo->setValue(pDoc->GetSongTempo());
//   ui->speed->setValue(pDoc->GetSongSpeed());
//   ui->numRows->setValue(pDoc->GetPatternLength());
//   ui->numFrames->setValue(pDoc->GetFrameCount());
   
//	m_wndDialogBar.UpdateDialogControls(&m_wndDialogBar, TRUE);
}

void CMainFrame::SelectInstrument(int Index)
{
   qDebug("SelectInstrument %d",Index);
//	CFamiTrackerDoc *pDoc = static_cast<CFamiTrackerDoc*>(GetActiveDocument());
	
//	int ListCount = m_pInstrumentList->GetItemCount();

//	// No instruments added
//	if (ListCount == 0) {
//		m_iInstrument = 0;
//		m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
//		return;
//	}

//	int LastInst = m_iInstrument;
	m_iInstrument = Index;

//	if (pDoc->IsInstrumentUsed(Index)) {
//		// Select instrument in list
//		int ListIndex = FindInstrument(Index);
//		int Selected = m_pInstrumentList->GetSelectionMark();

//		if (ListIndex != Selected || LastInst != Index) {
//			m_pInstrumentList->SetSelectionMark(ListIndex);
//			m_pInstrumentList->SetItemState(ListIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
//			m_pInstrumentList->EnsureVisible(ListIndex, FALSE);

//			// Set instrument name (this will trigger the name change routine)
//			char Text[256];
//			pDoc->GetInstrumentName(Index, Text);
//			m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(Text);

//			// Update instrument editor
//			if (m_wndInstEdit.IsOpened())
//				m_wndInstEdit.SetCurrentInstrument(Index);
//		}
//	}
//	else {
//		// Remove selection
//		m_pInstrumentList->SetSelectionMark(-1);
//		m_pInstrumentList->SetItemState(LastInst, 0, LVIS_SELECTED | LVIS_FOCUSED);
//		m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->SetWindowText(_T(""));
//		if (m_wndInstEdit.IsOpened())
//			m_wndInstEdit.DestroyWindow();
//	}
}

int CMainFrame::GetSelectedInstrument() const
{
	// Returns selected instrument
	return m_iInstrument;
}

void CMainFrame::DisplayOctave()
{
   qDebug("DisplayOctave");
//	CComboBox *pOctaveList = (CComboBox*)m_wndOctaveBar.GetDlgItem(IDC_OCTAVE);
//	CFamiTrackerView *pView	= (CFamiTrackerView*)GetActiveView();
//	pOctaveList->SetCurSel(pView->GetOctave());
}

void CMainFrame::UpdateInstrumentList()
{
   qDebug("UpdateInstrumentList");
//	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();

//	ClearInstrumentList();

//	for (int i = 0; i < MAX_INSTRUMENTS; ++i) {
//		if (pDoc->IsInstrumentUsed(i)) {
//			AddInstrument(i);
//		}
//	}
}

void CMainFrame::SetSongInfo(char *Name, char *Artist, char *Copyright)
{
   qDebug("SetSongInfo");
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_NAME, Name);
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_ARTIST, Artist);
//	m_wndDialogBar.SetDlgItemText(IDC_SONG_COPYRIGHT, Copyright);
}

void CMainFrame::UpdateTrackBox()
{
	// Fill the track box with all songs
//   CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
   CComboBox		*pTrackBox	= (CComboBox*)GetDlgItem(IDC_SUBTUNE);
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
   qDebug("ChangedTrack");
//	// Called from the view
//	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
//	m_iTrack = pDoc->GetSelectedTrack();
//	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
//	pTrackBox->SetCurSel(m_iTrack);
}

void CMainFrame::SetStatusText(LPCTSTR Text,...)
{
}

void CMainFrame::OnNextSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
//   CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
   CComboBox		*pTrackBox	= (CComboBox*)GetDlgItem(IDC_SUBTUNE);
	
	if (m_iTrack < (signed)pDoc->GetTrackCount() - 1)
		pDoc->SelectTrack(m_iTrack + 1);
}

void CMainFrame::OnPrevSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
//   CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
   CComboBox		*pTrackBox	= (CComboBox*)GetDlgItem(IDC_SUBTUNE);

	if (m_iTrack > 0)
		pDoc->SelectTrack(m_iTrack - 1);
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

//			// Move frame controls
//			m_wndFrameControls.MoveWindow(SX(10), SY(Height) + SY(21), SX(150), SY(26));
		}
		// Located to the left
		else {
//			// Frame editor window
//			CRect rect;
//			m_wndVerticalControlBar.GetClientRect(&rect);

//			Height = rect.Height() - HEADER_HEIGHT - 2;
//			Width = CFrameEditor::FIXED_WIDTH + CFrameEditor::FRAME_ITEM_WIDTH * Channels;

//			m_pFrameEditor->MoveWindow(SX(2), SY(HEADER_HEIGHT + 1), SX(Width), SY(Height));

//			// Move frame controls
//			m_wndFrameControls.MoveWindow(SX(4), SY(10), SX(150), SY(26));
		}

//		// Vertical control bar
//		m_wndVerticalControlBar.m_sizeDefault.cx = SX(54) + SX(CFrameEditor::FRAME_ITEM_WIDTH * Channels);
//		m_wndVerticalControlBar.CalcFixedLayout(TRUE, FALSE);
//		RecalcLayout();
	}

//	CRect ChildRect, ParentRect, FrameEditorRect, FrameBarRect;

//	m_wndControlBar.GetClientRect(&ParentRect);
//	m_pFrameEditor->GetClientRect(&FrameEditorRect);

//	int DialogStartPos;

//	if (m_iFrameEditorPos == FRAME_EDIT_POS_TOP)
//		DialogStartPos = FrameEditorRect.right + 32;
//	else
//		DialogStartPos = 0;

//	m_wndDialogBar.MoveWindow(DialogStartPos, 2, ParentRect.Width() - DialogStartPos, ParentRect.Height() - 4);
//	m_wndDialogBar.GetWindowRect(&ChildRect);
//	m_wndDialogBar.GetDlgItem(IDC_INSTRUMENTS)->MoveWindow(SX(288), SY(10), ChildRect.Width() - SX(296), SY(158));
//	m_wndDialogBar.GetDlgItem(IDC_INSTNAME)->MoveWindow(SX(478), SY(175), ChildRect.Width() - SX(486), SY(22));

	m_pFrameEditor->RedrawWindow();
}

bool CMainFrame::CreateSampleWindow()
{
	const int POS_X = 137;
	const int POS_Y = 113;

	CRect rect(SX(POS_X), SY(POS_Y), SX(POS_X) + CSampleWindow::WIN_WIDTH, SY(POS_Y) + CSampleWindow::WIN_HEIGHT);

	// Create the sample graph window
	m_pSampleWindow = new CSampleWindow();

//   if (!m_pSampleWindow->CreateEx(WS_EX_CLIENTEDGE, NULL, _T("Samples"), WS_CHILD | WS_VISIBLE, rect, (CWnd*)&m_wndDialogBar, 0))
   if (!m_pSampleWindow->CreateEx(WS_EX_CLIENTEDGE, NULL, _T("Samples"), WS_CHILD | WS_VISIBLE, rect, (CWnd*)NULL, 0))
		return false;

	// Assign this to the sound generator
	CSoundGen *pSoundGen = theApp.GetSoundGenerator();
	
	if (pSoundGen)
		pSoundGen->SetSampleWindow(m_pSampleWindow);
   
   ui->sampleWindow->layout()->addWidget(m_pSampleWindow->toQWidget());

	return true;
}

int CMainFrame::GetInstrumentIndex(int ListIndex) const
{
	// Convert instrument list index to instrument slot index
	int Instrument = 0;
   qDebug("GetInstrumentIndex not implemented yet, instrument=0");
//	TCHAR Text[256];
//	m_pInstrumentList->GetItemText(ListIndex, 0, Text, 256);
//	_stscanf(Text, _T("%X"), &Instrument);
	return Instrument;
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

void CMainFrame::on_frameChangeAll_clicked(bool checked)
{
   CFamiTrackerView* pView = (CFamiTrackerView*)GetActiveView();
   pView->SetChangeAllPattern(checked);
}

void CMainFrame::songInstruments_currentChanged(const QModelIndex &index,const QModelIndex &)
{
   NM_LISTVIEW nmlv;
   LRESULT result;
   
   nmlv.uNewState = LVIS_SELECTED;
   nmlv.iItem = index.row();
   OnChangedInstruments((NMHDR*)&nmlv,&result);
}

void CMainFrame::songInstruments_doubleClicked(const QModelIndex &index)
{
   NMHDR nmhdr;
   LRESULT result;
   OnDblClkInstruments(&nmhdr,&result);
}
