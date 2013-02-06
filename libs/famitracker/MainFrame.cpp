#include "MainFrame.h"
#include "ui_MainFrame.h"

#include "famitrackermodulepropertiesdialog.h"
#include "SoundGen.h"
#include "Settings.h"
#include "TrackerChannel.h"

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

const unsigned char KEY_DOT = 0xBD;		// '.'
const unsigned char KEY_DASH = 0xBE;	// '-'
const int KEY_REMOVE = -67;				// '-'

const int NOTE_HALT = -1;
const int NOTE_RELEASE = -2;

const int SINGLE_STEP = 1;

// Convert keys 0-F to numbers, -1 = invalid key
int ConvertKeyToHex(int Key) {

	switch (Key) {
		case 48: case VK_NUMPAD0: return 0x00;
		case 49: case VK_NUMPAD1: return 0x01;
		case 50: case VK_NUMPAD2: return 0x02;
		case 51: case VK_NUMPAD3: return 0x03;
		case 52: case VK_NUMPAD4: return 0x04;
		case 53: case VK_NUMPAD5: return 0x05;
		case 54: case VK_NUMPAD6: return 0x06;
		case 55: case VK_NUMPAD7: return 0x07;
		case 56: case VK_NUMPAD8: return 0x08;
		case 57: case VK_NUMPAD9: return 0x09;
		case 65: return 0x0A;
		case 66: return 0x0B;
		case 67: return 0x0C;
		case 68: return 0x0D;
		case 69: return 0x0E;
		case 70: return 0x0F;

		case KEY_DOT:
		case KEY_DASH:
			return 0x80;
	}

	return -1;
}

// Convert keys 0-9 to numbers
int ConvertKeyToDec(int Key)
{
	if (Key > 47 && Key < 58)
		return Key - 48;

	return -1;
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
   
   memset(m_bMuteChannels, 0, sizeof(bool) * MAX_CHANNELS);
   qDebug("m_iActiveNotes and Arpeggiate not initialized yet");
//	memset(m_iActiveNotes, 0, sizeof(int) * MAX_CHANNELS);
	memset(m_cKeyList, 0, sizeof(char) * 256);
//	memset(Arpeggiate, 0, sizeof(int) * MAX_CHANNELS);

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

//
// Custom key handling routines
//

void CFamiTrackerView::OnKeyHome()
{
	m_pPatternView->OnHomeKey();
	UpdateEditor(UPDATE_CURSOR);
}

void CFamiTrackerView::OnKeyEnd()
{
	m_pPatternView->OnEndKey();
	UpdateEditor(UPDATE_CURSOR);
}

void CFamiTrackerView::OnKeyInsert()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (PreventRepeat(VK_INSERT, true))
		return;

	AddAction(new CPatternAction(CPatternAction::ACT_INSERT_ROW));
}

void CFamiTrackerView::OnKeyBack()
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

void CFamiTrackerView::OnKeyDelete()
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

bool CFamiTrackerView::CheckClearKey(unsigned char Key) const
{
	return (Key == theApp.GetSettings()->Keys.iKeyClear);
}

bool CFamiTrackerView::CheckReleaseKey(unsigned char Key) const
{
	return (Key == theApp.GetSettings()->Keys.iKeyNoteRelease);
}

bool CFamiTrackerView::CheckHaltKey(unsigned char Key) const
{
	return (Key == theApp.GetSettings()->Keys.iKeyNoteCut);
}

bool CFamiTrackerView::CheckRepeatKey(unsigned char Key) const
{
	return (Key == theApp.GetSettings()->Keys.iKeyRepeat);
}

void CFamiTrackerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
   // Called when a key is pressed
	 
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	if (GetFocus() != this)
//		return;

	if (nChar >= VK_NUMPAD0 && nChar <= VK_NUMPAD9) {
		// Switch instrument
		if (m_pPatternView->GetColumn() == C_NOTE) {
			SetInstrument(nChar - VK_NUMPAD0);
			return;
		}
	}
	
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
			OnKeyHome();
			break;
		case VK_END:
			OnKeyEnd();
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
         if (m_bShiftPressed)
				m_pPatternView->PreviousChannel();
			else
				m_pPatternView->NextChannel();
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
			HandleKeyboardInput(nChar);
	}

//	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFamiTrackerView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
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

void CFamiTrackerView::InsertNote(int Note, int Octave, int Channel, int Velocity)
{
	// Inserts a note
	stChanNote Cell;

	int Frame = m_pPatternView->GetFrame();
	int Row = m_pPatternView->GetRow();

	GetDocument()->GetNoteData(Frame, Channel, Row, &Cell);

	Cell.Note = Note;

	if (Note != HALT && Note != RELEASE) {
		Cell.Octave	= Octave;

		if (!m_bMaskInstrument)
			Cell.Instrument = GetInstrument();

		if (Velocity < 128)
			Cell.Vol = (Velocity / 8);
	}	

	// Quantization
   qDebug("MIDI quantization???");
//	if (theApp.GetSettings()->Midi.bMidiMasterSync) {
//		int Delay = theApp.GetMIDI()->GetQuantization();
//		if (Delay > 0) {
//			Cell.EffNumber[0] = EF_DELAY;
//			Cell.EffParam[0] = Delay;
//		}
//	}
	
	if (m_bEditEnable) {
		if (Note == HALT)
			m_iLastNote = NOTE_HALT;
		else if (Note == RELEASE)
			m_iLastNote = NOTE_RELEASE;
		else
			m_iLastNote = (Note - 1) + Octave * 12;
		
		CPatternAction *pAction = new CPatternAction(CPatternAction::ACT_EDIT_NOTE);
		pAction->SetNote(Cell);
		AddAction(pAction);

		CSettings *pSettings = theApp.GetSettings();

		if ((m_pPatternView->GetColumn() == 0) && !theApp.IsPlaying() && (m_iInsertKeyStepping > 0) && !pSettings->Midi.bMidiMasterSync)
			StepDown();

		UpdateEditor(CHANGED_PATTERN);
	}
}

bool CFamiTrackerView::EditInstrumentColumn(stChanNote &Note, int Key, bool &StepDown, bool &MoveRight, bool &MoveLeft)
{
	int EditStyle = theApp.GetSettings()->General.iEditStyle;
	int Column = m_pPatternView->GetColumn();

	if (!m_bEditEnable)
		return false;

	if (CheckClearKey(Key)) {
		Note.Instrument = MAX_INSTRUMENTS;	// Indicate no instrument selected
		if (EditStyle != EDIT_STYLE2)
			StepDown = true;
		m_iLastInstrument = MAX_INSTRUMENTS;
		return true;
	}
	else if (CheckRepeatKey(Key)) {
		Note.Instrument = m_iLastInstrument;
		SetInstrument(m_iLastInstrument);
		if (EditStyle != EDIT_STYLE2)
			StepDown = true;
		return true;
	}

	int Value = ConvertKeyToHex(Key);
	unsigned char Mask, Shift;

	if (Value == -1)
		return false;

	if (Column == C_INSTRUMENT1) {
		Mask = 0x0F;
		Shift = 4;
	}
	else {
		Mask = 0xF0;
		Shift = 0;
	}

	if (Note.Instrument == MAX_INSTRUMENTS)
		Note.Instrument = 0;

	switch (EditStyle) {
		case EDIT_STYLE1: // FT2
			Note.Instrument = (Note.Instrument & Mask) | (Value << Shift);
			StepDown = true;
			break;
		case EDIT_STYLE2: // MPT
			if (Note.Instrument == (MAX_INSTRUMENTS - 1))
				Note.Instrument = 0;
			Note.Instrument = ((Note.Instrument & 0x0F) << 4) | Value & 0x0F;
			break;
		case EDIT_STYLE3: // IT
			Note.Instrument = (Note.Instrument & Mask) | (Value << Shift);
			if (Column == C_INSTRUMENT1)
				MoveRight = true;
			else if (Column == C_INSTRUMENT2) {
				MoveLeft = true;
				StepDown = true;
			}
			break;
	}

	if (Note.Instrument > (MAX_INSTRUMENTS - 1))
		Note.Instrument = (MAX_INSTRUMENTS - 1);

	if (Value == 0x80)
		Note.Instrument = MAX_INSTRUMENTS;

	SetInstrument(Note.Instrument);

	return true;
}

bool CFamiTrackerView::EditVolumeColumn(stChanNote &Note, int Key, bool &bStepDown)
{
	int EditStyle = theApp.GetSettings()->General.iEditStyle;

	if (!m_bEditEnable)
		return false;

	if (CheckClearKey(Key)) {
		Note.Vol = 0x10;
		if (EditStyle != EDIT_STYLE2)
			bStepDown = true;
		m_iLastVolume = 0x10;
		return true;
	}
	else if (CheckRepeatKey(Key)) {
		Note.Vol = m_iLastVolume;
		if (EditStyle != EDIT_STYLE2)
			bStepDown = true;
		return true;
	}

	int Value = ConvertKeyToHex(Key);

	if (Value == -1)
		return false;

	if (Value == 0x80)
		Note.Vol = 0x10;
	else
		Note.Vol = Value;

	m_iLastVolume = Note.Vol;

	if (EditStyle != EDIT_STYLE2)
		bStepDown = true;

	return true;
}

bool CFamiTrackerView::EditEffNumberColumn(stChanNote &Note, unsigned char nChar, int EffectIndex, bool &bStepDown)
{
	int EditStyle = theApp.GetSettings()->General.iEditStyle;

	if (!m_bEditEnable)
		return false;

	if (CheckRepeatKey(nChar)) {
		Note.EffNumber[EffectIndex] = m_iLastEffect;
		Note.EffParam[EffectIndex] = m_iLastEffectParam;
		return true;
	}

	if (CheckClearKey(nChar)) {
		Note.EffNumber[EffectIndex] = EF_NONE;
		if (EditStyle != EDIT_STYLE2)
			bStepDown = true;
		return true;
	}

	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int Chip = pDoc->GetChannel(m_pPatternView->GetChannel())->GetChip();

	bool bValidEffect = false;
	int Effect;

	if (nChar >= VK_NUMPAD0 && nChar <= VK_NUMPAD9)
		nChar = '0' + nChar - VK_NUMPAD0;

	if (Chip == SNDCHIP_FDS) {
		// FDS effects
		const char FDS_EFFECTS[] = {EF_FDS_MOD_DEPTH, EF_FDS_MOD_SPEED_HI, EF_FDS_MOD_SPEED_LO};
		for (int i = 0; i < sizeof(FDS_EFFECTS) && !bValidEffect; ++i) {
			if (nChar == EFF_CHAR[FDS_EFFECTS[i] - 1]) {
				bValidEffect = true;
				Effect = FDS_EFFECTS[i];
			}
		}
	}
	else if (Chip == SNDCHIP_VRC7) {
		// VRC7 effects
		/*
		const char VRC7_EFFECTS[] = {EF_VRC7_MODULATOR, EF_VRC7_CARRIER, EF_VRC7_LEVELS};
		for (int i = 0; i < sizeof(VRC7_EFFECTS) && !bValidEffect; ++i) {
			if (nChar == EFF_CHAR[VRC7_EFFECTS[i] - 1]) {
				bValidEffect = true;
				Effect = VRC7_EFFECTS[i];
			}
		}
		*/
	}
	else if (Chip == SNDCHIP_S5B) {
		// Sunsoft effects
		const char SUNSOFT_EFFECTS[] = {EF_SUNSOFT_ENV_LO, EF_SUNSOFT_ENV_HI, EF_SUNSOFT_ENV_TYPE};
		for (int i = 0; i < sizeof(SUNSOFT_EFFECTS) && !bValidEffect; ++i) {
			if (nChar == EFF_CHAR[SUNSOFT_EFFECTS[i] - 1]) {
				bValidEffect = true;
				Effect = SUNSOFT_EFFECTS[i];
			}
		}
	}

	for (int i = 0; i < EF_COUNT && !bValidEffect; ++i) {
		if (nChar == EFF_CHAR[i]) {
			bValidEffect = true;
			Effect = i + 1;
		}
	}

	if (bValidEffect) {
		Note.EffNumber[EffectIndex] = Effect;
		switch (EditStyle) {
			case EDIT_STYLE2:	// Modplug
				if (Effect == m_iLastEffect)
					Note.EffParam[EffectIndex] = m_iLastEffectParam;
				break;
			default:
				bStepDown = true;
		}
		m_iLastEffect = Effect;
		return true;
	}

	return false;
}

bool CFamiTrackerView::EditEffParamColumn(stChanNote &Note, int Key, int EffectIndex, bool &bStepDown, bool &bMoveRight, bool &bMoveLeft)
{
	int EditStyle = theApp.GetSettings()->General.iEditStyle;
	unsigned char Mask, Shift;
	int Column = m_pPatternView->GetColumn();
	int Value = ConvertKeyToHex(Key);

	if (!m_bEditEnable)
		return false;

	if (CheckRepeatKey(Key)) {
		Note.EffNumber[EffectIndex] = m_iLastEffect;
		Note.EffParam[EffectIndex] = m_iLastEffectParam;
		return true;
	}

	if (CheckClearKey(Key)) {
		Note.EffParam[EffectIndex] = 0;
		if (EditStyle != EDIT_STYLE2)
			bStepDown = true;
		return true;
	}

	if (Value == -1 || Value == 0x80)
		return false;

	if (Column == C_EFF_PARAM1 || Column == C_EFF2_PARAM1 || Column == C_EFF3_PARAM1 || Column == C_EFF4_PARAM1) {
		Mask = 0x0F;
		Shift = 4;
	}
	else {
		Mask = 0xF0;
		Shift = 0;
	}
	
	switch (EditStyle) {
		case EDIT_STYLE1:	// FT2
			Note.EffParam[EffectIndex] = (Note.EffParam[EffectIndex] & Mask) | Value << Shift;
			bStepDown = true;
			break;
		case EDIT_STYLE2:	// Modplug
			Note.EffParam[EffectIndex] = ((Note.EffParam[EffectIndex] & 0x0F) << 4) | Value & 0x0F;
			UpdateEditor(CHANGED_PATTERN);
			break;
		case EDIT_STYLE3:	// IT
			Note.EffParam[EffectIndex] = (Note.EffParam[EffectIndex] & Mask) | Value << Shift;
			if (Mask == 0x0F)
				bMoveRight = true;
			else {
				bMoveLeft = true;
				bStepDown = true;
			}
			break;
	}

	m_iLastEffectParam = Note.EffParam[EffectIndex];

	return true;
}

void CFamiTrackerView::HandleKeyboardInput(char nChar)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	static bool FirstChar = false;

	stChanNote Note;

	int EditStyle = theApp.GetSettings()->General.iEditStyle;
	int Index;

	int Frame = m_pPatternView->GetFrame();
	int Row = m_pPatternView->GetRow();
	int Channel = m_pPatternView->GetChannel();
	int Column = m_pPatternView->GetColumn();

	bool bStepDown = false;
	bool bMoveRight = false;
	bool bMoveLeft = false;

	// Watch for repeating keys
	if (PreventRepeat(nChar, m_bEditEnable))
		return;

	// Get the note data
	pDoc->GetNoteData(Frame, Channel, Row, &Note);

	// Make all effect columns look the same, save an index instead
	switch (Column) {
		case C_EFF_NUM:		Column = C_EFF_NUM;	Index = 0; break;
		case C_EFF2_NUM:	Column = C_EFF_NUM;	Index = 1; break;
		case C_EFF3_NUM:	Column = C_EFF_NUM;	Index = 2; break;
		case C_EFF4_NUM:	Column = C_EFF_NUM;	Index = 3; break;
		case C_EFF_PARAM1:	Column = C_EFF_PARAM1; Index = 0; break;
		case C_EFF2_PARAM1:	Column = C_EFF_PARAM1; Index = 1; break;
		case C_EFF3_PARAM1:	Column = C_EFF_PARAM1; Index = 2; break;
		case C_EFF4_PARAM1:	Column = C_EFF_PARAM1; Index = 3; break;
		case C_EFF_PARAM2:	Column = C_EFF_PARAM2; Index = 0; break;
		case C_EFF2_PARAM2:	Column = C_EFF_PARAM2; Index = 1; break;
		case C_EFF3_PARAM2:	Column = C_EFF_PARAM2; Index = 2; break;
		case C_EFF4_PARAM2:	Column = C_EFF_PARAM2; Index = 3; break;			
	}

	switch (Column) {
		// Note & octave column
		case C_NOTE:
			if (CheckRepeatKey(nChar)) {
				if (m_iLastNote == 0) {
					Note.Note = 0;
				}
				else if (m_iLastNote == NOTE_HALT) {
					Note.Note = HALT;
				}
				else if (m_iLastNote == NOTE_RELEASE) {
					Note.Note = RELEASE;
				}
				else {
					Note.Note = GET_NOTE(m_iLastNote);
					Note.Octave = GET_OCTAVE(m_iLastNote);
				}
			}
			else if (CheckClearKey(nChar)) {
				// Remove note
				Note.Note = 0;
				Note.Octave = 0;
				m_iLastNote = 0;
				if (EditStyle != EDIT_STYLE2)
					bStepDown = true;
					//StepDown();
			}
			else {
				// This is special
            qDebug("HandleKeyboardNote");
//				HandleKeyboardNote(nChar, true);
				return;
			}
			break;
		// Instrument column
		case C_INSTRUMENT1:
		case C_INSTRUMENT2:
			if (!EditInstrumentColumn(Note, nChar, bStepDown, bMoveRight, bMoveLeft))
				return;
			break;
		// Volume column
		case C_VOLUME:
			if (!EditVolumeColumn(Note, nChar, bStepDown))
				return;
			break;
		// Effect number
		case C_EFF_NUM:
			if (!EditEffNumberColumn(Note, nChar, Index, bStepDown))
				return;
			break;
		// Effect parameter
		case C_EFF_PARAM1:
		case C_EFF_PARAM2:
			if (!EditEffParamColumn(Note, nChar, Index, bStepDown, bMoveRight, bMoveLeft))
				return;
			break;
	}

	// Something changed, store pattern data in document and update screen
	if (m_bEditEnable) {
		CPatternAction *pAction = new CPatternAction(CPatternAction::ACT_EDIT_NOTE);
		pAction->SetNote(Note);
		AddAction(pAction);

		if (bMoveLeft)
			m_pPatternView->MoveLeft();

		if (bMoveRight)
			m_pPatternView->MoveRight();

		if (bStepDown)
			StepDown();
	}
}

void CMainFrame::keyPressEvent(QKeyEvent *event)
{
   UINT nChar = event->nativeVirtualKey();
   UINT nRepCnt = event->count();
   
   OnKeyDown(nChar,nRepCnt,0);
   repaint();
}

void CMainFrame::keyReleaseEvent(QKeyEvent *event)
{
   UINT nChar = event->nativeVirtualKey();
   UINT nRepCnt = event->count();
   
   OnKeyUp(nChar,nRepCnt,0);
   repaint();
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

bool CFamiTrackerView::AddAction(CAction *pAction)
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

CAction *CFamiTrackerView::GetLastAction() const
{
	if (!m_pActionHandler)
		return NULL;

	return m_pActionHandler->GetLastAction();
}

void CFamiTrackerView::SetRowCount(int Count)
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

void CFamiTrackerView::SetFrameCount(int Count)
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

void CFamiTrackerView::RemoveWithoutDelete()
{
	AddAction(new CPatternAction(CPatternAction::ACT_EDIT_DELETE_ROWS));
}

void CFamiTrackerView::OnEditDelete()
{
	AddAction(new CPatternAction(CPatternAction::ACT_EDIT_DELETE));
}

void CFamiTrackerView::StepDown()
{
	if (m_iInsertKeyStepping)
		m_pPatternView->MoveDown(m_iInsertKeyStepping);

	UpdateEditor(UPDATE_CURSOR);
}

void CFamiTrackerView::KeyIncreaseAction()
{
	AddAction(new CPatternAction(CPatternAction::ACT_INCREASE));
}

void CFamiTrackerView::KeyDecreaseAction()
{
	AddAction(new CPatternAction(CPatternAction::ACT_DECREASE));
}

bool CFamiTrackerView::PreventRepeat(unsigned char Key, bool Insert)
{
	if (m_cKeyList[Key] == 0)
		m_cKeyList[Key] = 1;
	else {
		if ((!theApp.GetSettings()->General.bKeyRepeat || !Insert))
			return true;
	}

	return false;
}

void CFamiTrackerView::RepeatRelease(unsigned char Key)
{
	memset(m_cKeyList, 0, 256);
}

void CFamiTrackerView::SetOctave(unsigned int iOctave)
{
	m_iOctave = iOctave;
   octaveComboBox->setCurrentIndex(iOctave);
}

void CMainFrame::on_editStep_valueChanged(int arg1)
{
   SetStepping(arg1);   
}

void CFamiTrackerView::SetStepping(int Step) 
{ 
	m_iInsertKeyStepping = Step;

	if (Step > 0 && !theApp.GetSettings()->General.bNoStepMove)
		m_iMoveKeyStepping = Step;
	else 
		m_iMoveKeyStepping = 1;

//	((CMainFrame*)GetParentFrame())->UpdateControls();
}

void CFamiTrackerView::ToggleChannel(unsigned int Channel)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (Channel >= pDoc->GetAvailableChannels())
		return;

	m_bMuteChannels[Channel] = !m_bMuteChannels[Channel];
	HaltNote(Channel);
	UpdateEditor(CHANGED_HEADER);
}

void CFamiTrackerView::SoloChannel(unsigned int Channel)
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

void CFamiTrackerView::UnmuteAllChannels()
{
	CFamiTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int channels = pDoc->GetAvailableChannels();

	for (int i = 0; i < channels; ++i) {
		m_bMuteChannels[i] = false;
	}

	UpdateEditor(CHANGED_HEADER);
}

bool CFamiTrackerView::IsChannelSolo(unsigned int Channel)
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

bool CFamiTrackerView::IsChannelMuted(unsigned int Channel)
{
	ASSERT(Channel < MAX_CHANNELS);
	return m_bMuteChannels[Channel];
}

void CFamiTrackerView::SetInstrument(int Instrument)
{
	CFamiTrackerDoc* pDoc = GetDocument();
//	CMainFrame *pMainFrm = (CMainFrame*)GetParentFrame();
	ASSERT_VALID(pDoc);
//	ASSERT_VALID(pMainFrm);
	ASSERT(Instrument >= 0 && Instrument < MAX_INSTRUMENTS);

	if (Instrument == MAX_INSTRUMENTS)
		return;

   qDebug("select instrument in main frame...");
//	pMainFrm->SelectInstrument(Instrument);
	m_iLastInstrument = GetInstrument(); // Gets actual selected instrument //  Instrument;
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

void CFamiTrackerView::PlayNote(unsigned int Channel, unsigned int Note, unsigned int Octave, unsigned int Velocity)
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

void CFamiTrackerView::ReleaseNote(unsigned int Channel)
{
	// Releases a channel
	stChanNote NoteData;
	memset(&NoteData, 0, sizeof(stChanNote));

	NoteData.Note = RELEASE;
	NoteData.Vol = 0x10;
	NoteData.Instrument = GetInstrument();

	FeedNote(Channel, &NoteData);
}

void CFamiTrackerView::HaltNote(unsigned int Channel)
{
	// Halts a channel
	stChanNote NoteData;
	memset(&NoteData, 0, sizeof(stChanNote));

	NoteData.Note = HALT;
	NoteData.Vol = 0x10;
	NoteData.Instrument = GetInstrument();

	FeedNote(Channel, &NoteData);
}

void CFamiTrackerView::FeedNote(int Channel, stChanNote *NoteData)
{
	CFamiTrackerDoc* pDoc = GetDocument();
	CTrackerChannel *pChannel = pDoc->GetChannel(Channel);

	pChannel->SetNote(*NoteData);
   qDebug("theApp.GetMIDI");
//	theApp.GetMIDI()->WriteNote(Channel, NoteData->Note, NoteData->Octave, NoteData->Vol);
}

