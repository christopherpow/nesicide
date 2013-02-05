#include "MainFrame.h"
#include "ui_MainFrame.h"

#include "famitrackermodulepropertiesdialog.h"
#include "famitracker/SoundGen.h"
#include "famitracker/Settings.h"
#include "famitracker/TrackerChannel.h"

#include "cqtmfc.h"

#include <QPainter>

CFamiTrackerFrameTableView::CFamiTrackerFrameTableView(QWidget *parent)
 : QTableView(parent)
{
}

void CFamiTrackerFrameTableView::resizeEvent(QResizeEvent *event)
{
   QTableView::resizeEvent(event);
   
   scrollTo(selectionModel()->currentIndex(),QAbstractItemView::PositionAtCenter);
}

void CFamiTrackerFrameTableView::setModel(QAbstractItemModel *model)
{
   QTableView::setModel(model);
   
   setCurrentIndex(model->index(0,0));
}

CMainFrame::CMainFrame(QString fileName,QWidget *parent) :
   QWidget(parent),
   ui(new Ui::CMainFrame),
 m_iMoveKeyStepping(1),
 m_iInsertKeyStepping(1),
 m_bEditEnable(false),
 m_bShiftPressed(false),
 m_bControlPressed(false),
 m_bChangeAllPattern(false),
 m_iPasteMode(PASTE_MODE_NORMAL),
 m_bMaskInstrument(false),
 m_bSwitchToInstrument(false),

 m_iOctave(3),
 m_iLastVolume(0x10),
 m_iLastInstrument(0),
 m_iSwitchToInstrument(-1),

 m_iPlayTime(0),

 m_bForceRedraw(false),
 m_bUpdateBackground(true),
 m_bFollowMode(true),

 m_iAutoArpPtr(0),
 m_iLastAutoArpPtr(0),
 m_iAutoArpKeyCount(0),

 m_iMenuChannel(-1),
 m_iFrameQueue(-1),
 m_iKeyboardNote(-1)
{
   int width;
   int col;
   int song;
   int idx;

   ui->setupUi(this);
   
   // Unmute all channels
	for (int i = 0; i < MAX_CHANNELS; ++i) {
		m_bMuteChannels[i] = false;
	}

   m_pDocument = new CFamiTrackerDoc();

   m_pActionHandler = new CActionHandler();

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
         
   // Connect buried signals.
   QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));

   m_fileName = fileName;

   m_pDocument->OnOpenDocument((TCHAR*)fileName.toAscii().constData());

   instrumentsModel = new CMusicFamiTrackerInstrumentsModel(m_pDocument);

   on_songs_currentIndexChanged(0);

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

   for ( song = 0; song < m_pDocument->GetTrackCount(); song++ )
   {
      ui->songs->addItem(m_pDocument->GetTrackTitle(song));
   }

   ui->title->setText(m_pDocument->GetSongName());
   ui->author->setText(m_pDocument->GetSongArtist());
   ui->copyright->setText(m_pDocument->GetSongCopyright());
   ui->tempo->setValue(m_pDocument->GetSongTempo());
   ui->speed->setValue(m_pDocument->GetSongSpeed());
   ui->numRows->setValue(m_pDocument->GetPatternLength());
   ui->numFrames->setValue(m_pDocument->GetFrameCount());
   
   // Pass document to other "views"...
   ui->songFrames->AssignDocument(m_pDocument);
   ui->songPatterns->AssignDocument(m_pDocument,this);
   
   // Set up other "views"...
   ui->songPatterns->ApplyColorScheme();
   
   // Connect views together the Qt way.
   QObject::connect(ui->songFrames,SIGNAL(selectFrame(uint)),this,SLOT(frameEditor_selectFrame(uint)));
}

CMainFrame::~CMainFrame()
{
   delete ui;
   delete instrumentsModel;
   delete octaveLabel;
   delete octaveComboBox;
   delete toolBar;
}

CFrameEditor* CMainFrame::GetFrameEditor()
{
   return ui->songFrames;
}

CPatternEditor* CMainFrame::GetPatternEditor()
{
   return ui->songPatterns;
}

void CMainFrame::showEvent(QShowEvent *)
{
   CFamiTrackerApp::GetSoundGenerator()->start();

   emit addToolBarWidget(toolBar);

   QObject::connect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
}

void CMainFrame::hideEvent(QHideEvent *)
{
   emit removeToolBarWidget(toolBar);
   
   QObject::disconnect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
}

void CMainFrame::keyPressEvent(QKeyEvent *event)
{
   UINT nChar = event->nativeVirtualKey();
   UINT nRepCnt = event->count();
   
   // Called when a key is pressed
	 
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	if (GetFocus() != this)
//		return;

//	if (nChar >= VK_NUMPAD0 && nChar <= VK_NUMPAD9) {
   qDebug("SetInstrument");
//		// Switch instrument
//		if (m_pPatternView->GetColumn() == C_NOTE) {
//			SetInstrument(nChar - VK_NUMPAD0);
//			return;
//		}
//	}
	
	switch (nChar) {
		// Shift
		case VK_SHIFT:
			m_pPatternView->ShiftPressed(true);
			m_bShiftPressed = true;
			break;
		// Control
		case VK_CONTROL:
			m_pPatternView->ControlPressed(true);
			m_bControlPressed = true;
			break;
		// Movement
		case VK_UP:
         m_pPatternView->MoveUp(m_iMoveKeyStepping);
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_DOWN:
         m_pPatternView->MoveDown(m_iMoveKeyStepping);
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_LEFT:
			m_pPatternView->MoveLeft();
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_RIGHT:
			m_pPatternView->MoveRight();
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_HOME:
      qDebug("OnKeyHome");
//			OnKeyHome();
			break;
		case VK_END:
      qDebug("OnKeyEnd");
//			OnKeyEnd();
			break;
		case VK_PRIOR:
			m_pPatternView->MovePageUp();
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_NEXT:
			m_pPatternView->MovePageDown();
			UpdateEditor(UPDATE_CURSOR);
			break;
		case VK_TAB:	// Move between channels
      qDebug("m_bShiftPressed");
//         if (m_bShiftPressed)
//				m_pPatternView->PreviousChannel();
//			else
//				m_pPatternView->NextChannel();
			UpdateEditor(UPDATE_CURSOR);
			break;

		// Pattern editing
		case VK_ADD:
			KeyIncreaseAction();
			break;
		case VK_SUBTRACT:
			KeyDecreaseAction();
			break;
		case VK_DELETE:
			OnKeyDelete();
			break;
		case VK_INSERT:
			OnKeyInsert();
			break;
		case VK_BACK:
			OnKeyBack();
			break;

		// ESC -> remove selection
		case VK_ESCAPE:
			m_pPatternView->ClearSelection();
			UpdateEditor(UPDATE_CURSOR);
			break;

		// Octaves
		case VK_F2: SetOctave(0); break;
		case VK_F3: SetOctave(1); break;
		case VK_F4: SetOctave(2); break;
		case VK_F5: SetOctave(3); break;
		case VK_F6: SetOctave(4); break;
		case VK_F7: SetOctave(5); break;
		case VK_F8: SetOctave(6); break;
		case VK_F9: SetOctave(7); break;

		default:
      qDebug("HandleKeyboardInput");
//			HandleKeyboardInput(nChar);
	}

//	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::keyReleaseEvent(QKeyEvent *event)
{
   UINT nChar = event->nativeVirtualKey();
   
   // Called when a key is released

	switch (nChar) {
		case VK_SHIFT:
			m_bShiftPressed = false;
			m_pPatternView->ShiftPressed(false);
			break;
		case VK_CONTROL:
			m_bControlPressed = false;
			m_pPatternView->ControlPressed(false);
			break;
	}

   qDebug("HandleKeyboardNote");
//	HandleKeyboardNote(nChar, false);

	m_cKeyList[nChar] = 0;

//	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::updateViews(long hint)
{
   QString str;
   str.sprintf("updateViews(%d)",hint);
   qDebug(str.toAscii().constData());
   instrumentsModel->update();
}

void CMainFrame::on_frameInc_clicked()
{
   qDebug("CheckRepeat() NOT IMPL");
	int Add = 1;//(CheckRepeat() ? 4 : 1);
   CFrameAction *pAction = new CFrameAction(ui->frameChangeAll->isChecked() ? CFrameAction::ACT_CHANGE_PATTERN_ALL : CFrameAction::ACT_CHANGE_PATTERN);
	pAction->SetPatternDelta(Add, ui->frameChangeAll->isChecked());
	AddAction(pAction);
}

void CMainFrame::on_frameDec_clicked()
{
   qDebug("CheckRepeat() NOT IMPL");
	int Remove = -1;//(CheckRepeat() ? 4 : 1);
   CFrameAction *pAction = new CFrameAction(ui->frameChangeAll->isChecked() ? CFrameAction::ACT_CHANGE_PATTERN_ALL : CFrameAction::ACT_CHANGE_PATTERN);
	pAction->SetPatternDelta(Remove, ui->frameChangeAll->isChecked());
	AddAction(pAction);
}

void CMainFrame::on_speed_valueChanged(int arg1)
{
   m_pDocument->SetSongSpeed(arg1);
}

void CMainFrame::on_tempo_valueChanged(int arg1)
{
   m_pDocument->SetSongTempo(arg1);
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
   if ( index >= 0 )
   {
      m_pDocument->SelectTrack(index);
   }
   
   instrumentsModel->update();

   ui->tempo->setValue(m_pDocument->GetSongTempo());
   ui->speed->setValue(m_pDocument->GetSongSpeed());
   ui->numRows->setValue(m_pDocument->GetPatternLength());
   ui->numFrames->setValue(m_pDocument->GetFrameCount());
}

void CMainFrame::on_title_textEdited(const QString &arg1)
{
   m_pDocument->SetSongName(arg1.toAscii().data());
}

void CMainFrame::on_author_textEdited(const QString &arg1)
{
   m_pDocument->SetSongArtist(arg1.toAscii().data());
}

void CMainFrame::on_copyright_textEdited(const QString &arg1)
{
   m_pDocument->SetSongCopyright(arg1.toAscii().data());
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
   //onSave();
}

void CMainFrame::trackerAction_editCut()
{
   qDebug("editCut");
}

void CMainFrame::trackerAction_editCopy()
{
   qDebug("editCopy");
}

void CMainFrame::trackerAction_editPaste()
{
   qDebug("editPaste");
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
   qDebug("addFrame");
   AddAction(new CFrameAction(CFrameAction::ACT_ADD));
}

void CMainFrame::trackerAction_removeFrame()
{
   qDebug("removeFrame");
   AddAction(new CFrameAction(CFrameAction::ACT_REMOVE));
}

void CMainFrame::trackerAction_moveFrameDown()
{
   qDebug("moveFrameDown");
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_DOWN));
}

void CMainFrame::trackerAction_moveFrameUp()
{
   qDebug("moveFrameUp");
   AddAction(new CFrameAction(CFrameAction::ACT_MOVE_UP));
}

void CMainFrame::trackerAction_duplicateFrame()
{
   qDebug("duplicateFrame");
   AddAction(new CFrameAction(CFrameAction::ACT_DUPLICATE));
}

void CMainFrame::trackerAction_moduleProperties()
{
   qDebug("moduleProperties");
   FamiTrackerModulePropertiesDialog dlg(m_pDocument);

   dlg.exec();
   
   // The song list may have changed, rebuild it from the model.
   ui->songs->clear();
   ui->songs->addItems(dlg.tracks());
   
   instrumentsModel->update();
}

void CMainFrame::trackerAction_play()
{
   qDebug("play");
}

void CMainFrame::trackerAction_playLoop()
{
   qDebug("playLoop");
}

void CMainFrame::trackerAction_stop()
{
   qDebug("stop");
}

void CMainFrame::trackerAction_editMode()
{
   qDebug("editMode");
   m_bEditEnable = !m_bEditEnable;

//	if (m_bEditEnable)
//		GetParentFrame()->SetMessageText(_T("Changed to edit mode"));
//	else
//		GetParentFrame()->SetMessageText(_T("Changed to normal mode"));
	
	UpdateEditor(UPDATE_ENTIRE);
}

void CMainFrame::trackerAction_previousTrack()
{
   qDebug("previousTrack");
}

void CMainFrame::trackerAction_nextTrack()
{
   qDebug("nextTrack");
}

void CMainFrame::trackerAction_settings()
{
   qDebug("settings");
}

void CMainFrame::trackerAction_createNSF()
{
   qDebug("createNSF");
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

void CMainFrame::frameEditor_selectFrame(unsigned int Frame)
{
   ASSERT(Frame < MAX_FRAMES);

	unsigned int FrameCount = GetDocument()->GetFrameCount();
	if (Frame >= FrameCount)
		Frame = FrameCount - 1;		

	m_pPatternView->MoveToFrame(Frame);

	UpdateEditor(UPDATE_FRAME);
}

void CMainFrame::UpdateEditor(UINT lHint)
{
   m_pDocument->UpdateAllViews(0,lHint);
}

//
// Custom key handling routines
//

void CMainFrame::OnKeyHome()
{
	m_pPatternView->OnHomeKey();
	UpdateEditor(UPDATE_CURSOR);
}

void CMainFrame::OnKeyEnd()
{
	m_pPatternView->OnEndKey();
	UpdateEditor(UPDATE_CURSOR);
}

void CMainFrame::OnKeyInsert()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (PreventRepeat(VK_INSERT, true))
		return;

	AddAction(new CPatternAction(CPatternAction::ACT_INSERT_ROW));
}

void CMainFrame::RemoveWithoutDelete()
{
	AddAction(new CPatternAction(CPatternAction::ACT_EDIT_DELETE_ROWS));
}

void CMainFrame::OnKeyBack()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_pPatternView->IsSelecting()) 
		RemoveWithoutDelete();
	else {
		if (PreventRepeat(VK_BACK, true))
			return;

		CPatternAction *pAction = new CPatternAction(CPatternAction::ACT_DELETE_ROW);
		pAction->SetDelete(true, true);
		if (AddAction(pAction))
			m_pPatternView->MoveUp(1);
	}

	UpdateEditor(CHANGED_PATTERN);
}

void CMainFrame::OnEditDelete()
{
	AddAction(new CPatternAction(CPatternAction::ACT_EDIT_DELETE));
}

void CMainFrame::OnKeyDelete()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (PreventRepeat(VK_DELETE, true))
		return;

	if (m_pPatternView->IsSelecting()) {
		OnEditDelete();
	}
	else {
		CPatternAction *pAction = new CPatternAction(CPatternAction::ACT_DELETE_ROW);
		bool bPullUp = theApp.GetSettings()->General.bPullUpDelete || m_bShiftPressed;
		pAction->SetDelete(bPullUp, false);
		AddAction(pAction);
		if (!bPullUp)
			StepDown();
	}
}

void CMainFrame::StepDown()
{
	if (m_iInsertKeyStepping)
		m_pPatternView->MoveDown(m_iInsertKeyStepping);

	UpdateEditor(UPDATE_CURSOR);
}

void CMainFrame::KeyIncreaseAction()
{
	AddAction(new CPatternAction(CPatternAction::ACT_INCREASE));
}

void CMainFrame::KeyDecreaseAction()
{
	AddAction(new CPatternAction(CPatternAction::ACT_DECREASE));
}

bool CMainFrame::PreventRepeat(unsigned char Key, bool Insert)
{
	if (m_cKeyList[Key] == 0)
		m_cKeyList[Key] = 1;
	else {
		if ((!theApp.GetSettings()->General.bKeyRepeat || !Insert))
			return true;
	}

	return false;
}

void CMainFrame::RepeatRelease(unsigned char Key)
{
	memset(m_cKeyList, 0, 256);
}

void CMainFrame::SetOctave(unsigned int iOctave)
{
	m_iOctave = iOctave;
   octaveComboBox->setCurrentIndex(iOctave);
}

void CMainFrame::on_editStep_valueChanged(int arg1)
{
   SetStepping(arg1);   
}

void CMainFrame::SetStepping(int Step) 
{ 
	m_iInsertKeyStepping = Step;

	if (Step > 0 && !theApp.GetSettings()->General.bNoStepMove)
		m_iMoveKeyStepping = Step;
	else 
		m_iMoveKeyStepping = 1;

//	((CMainFrame*)GetParentFrame())->UpdateControls();
}

void CMainFrame::ToggleChannel(unsigned int Channel)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (Channel >= pDoc->GetAvailableChannels())
		return;

	m_bMuteChannels[Channel] = !m_bMuteChannels[Channel];
	HaltNote(Channel);
	UpdateEditor(CHANGED_HEADER);
}

void CMainFrame::SoloChannel(unsigned int Channel)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int channels = pDoc->GetAvailableChannels();

	if (Channel >= unsigned(channels))
		return;

	if (IsChannelSolo(Channel)) {
		// Revert channels
		for (int i = 0; i < channels; ++i) {
			m_bMuteChannels[i] = false;
		}
	}
	else {
		// Solo selected channel
		for (int i = 0; i < channels; ++i) {
			if (i != Channel) {
				m_bMuteChannels[i] = true;
				HaltNote(i);
			}
		}
		m_bMuteChannels[Channel] = false;
	}

	UpdateEditor(CHANGED_HEADER);
}

void CMainFrame::UnmuteAllChannels()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int channels = pDoc->GetAvailableChannels();

	for (int i = 0; i < channels; ++i) {
		m_bMuteChannels[i] = false;
	}

	UpdateEditor(CHANGED_HEADER);
}

bool CMainFrame::IsChannelSolo(unsigned int Channel)
{
	// Returns true if Channel is the only active channel 
   CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int channels = pDoc->GetAvailableChannels();

	for (int i = 0; i < channels; ++i) {
		if (m_bMuteChannels[i] == false && i != Channel)
			return false;
	}
	return true;
}

bool CMainFrame::IsChannelMuted(unsigned int Channel)
{
	ASSERT(Channel < MAX_CHANNELS);
	return m_bMuteChannels[Channel];
}

unsigned int CMainFrame::GetInstrument() const 
{
//	CMainFrame *pMainFrm = (CMainFrame*)GetParentFrame();
//	ASSERT_VALID(pMainFrm);
//	return pMainFrm->GetSelectedInstrument();
   return ui->songInstruments->currentIndex().row();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Note playing routines
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMainFrame::PlayNote(unsigned int Channel, unsigned int Note, unsigned int Octave, unsigned int Velocity)
{
	// Play a note in a channel
	stChanNote NoteData;

	memset(&NoteData, 0, sizeof(stChanNote));

	NoteData.Note		= Note;
	NoteData.Octave		= Octave;
	NoteData.Vol		= Velocity / 8;
	NoteData.Instrument	= GetInstrument();
/*	
	if (theApp.GetSettings()->General.iEditStyle == EDIT_STYLE3)
		NoteData.Instrument	= m_iLastInstrument;
	else
		NoteData.Instrument	= GetInstrument();
*/
	FeedNote(Channel, &NoteData);
}

void CMainFrame::ReleaseNote(unsigned int Channel)
{
	// Releases a channel
	stChanNote NoteData;
	memset(&NoteData, 0, sizeof(stChanNote));

	NoteData.Note = RELEASE;
	NoteData.Vol = 0x10;
	NoteData.Instrument = GetInstrument();

	FeedNote(Channel, &NoteData);
}

void CMainFrame::HaltNote(unsigned int Channel)
{
	// Halts a channel
	stChanNote NoteData;
	memset(&NoteData, 0, sizeof(stChanNote));

	NoteData.Note = HALT;
	NoteData.Vol = 0x10;
	NoteData.Instrument = GetInstrument();

	FeedNote(Channel, &NoteData);
}

void CMainFrame::FeedNote(int Channel, stChanNote *NoteData)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	CTrackerChannel *pChannel = pDoc->GetChannel(Channel);

	pChannel->SetNote(*NoteData);
   qDebug("theApp.GetMIDI");
//	theApp.GetMIDI()->WriteNote(Channel, NoteData->Note, NoteData->Octave, NoteData->Vol);
}

