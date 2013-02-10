#include "MainFrame.h"
#include "ui_MainFrame.h"
#include "FamiTracker.h"
#include "FamiTrackerView.h"

#include "famitrackermodulepropertiesdialog.h"

#include <QFrame>
#include <QLayout>
#include <QAction>

#include "cqtmfc.h"

CMainFrame::CMainFrame(QWidget *parent) :
   CFrameWnd(parent),
   ui(new Ui::CMainFrame),
   m_iInstrument(0),
   m_iTrack(0)
{
   int idx;
   int col;
   
   ui->setupUi(this);
   
   m_pDocument = new CFamiTrackerDoc();
   m_pDocument->SetTitle("Untitled");
   
   m_pView = new CFamiTrackerView(this);
   
   // Create frame editor
	m_pFrameEditor = new CFrameEditor(this);

   ui->songFrames->layout()->addWidget(m_pFrameEditor);
   
   ui->songPatterns->layout()->addWidget(m_pView->GetPatternView());

   m_pActionHandler = new CActionHandler();

   m_pView->OnInitialUpdate();

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

   instrumentsModel = new CMusicFamiTrackerInstrumentsModel(m_pDocument);

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
   
   // Connect buried signals.
   QObject::connect(m_pDocument,SIGNAL(setModified(bool)),this,SIGNAL(editor_modificationChanged(bool)));
}

CMainFrame::~CMainFrame()
{
   delete ui;
   delete instrumentsModel;
   delete m_pView;
   delete m_pDocument;
   delete octaveLabel;
   delete octaveComboBox;
   delete toolBar;
   SAFE_RELEASE(m_pFrameEditor);
}

CView* CMainFrame::GetActiveView()
{
   return m_pView;
}

void CMainFrame::showEvent(QShowEvent *)
{
   theApp.GetSoundGenerator()->start();

   emit addToolBarWidget(toolBar);
}

void CMainFrame::hideEvent(QHideEvent *)
{
   emit removeToolBarWidget(toolBar);
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
   m_pView->OnEditCut();
}

void CMainFrame::trackerAction_editCopy()
{
   qDebug("editCopy");
   m_pView->OnEditCopy();
}

void CMainFrame::trackerAction_editPaste()
{
   qDebug("editPaste");
   m_pView->OnEditPaste();   
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
   
//   instrumentsModel->update();
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
   m_pView->OnTrackerEdit();
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
   qDebug("CheckRepeat?");
	int Add = 1;//(CheckRepeat() ? 4 : 1);
   CFrameAction *pAction = new CFrameAction(ui->frameChangeAll->isChecked() ? CFrameAction::ACT_CHANGE_PATTERN_ALL : CFrameAction::ACT_CHANGE_PATTERN);
	pAction->SetPatternDelta(Add, ui->frameChangeAll->isChecked());
	AddAction(pAction);
}

void CMainFrame::on_frameDec_clicked()
{
   qDebug("CheckRepeat?");
	int Remove = 1;//(CheckRepeat() ? 4 : 1);
   CFrameAction *pAction = new CFrameAction(ui->frameChangeAll->isChecked() ? CFrameAction::ACT_CHANGE_PATTERN_ALL : CFrameAction::ACT_CHANGE_PATTERN);
	pAction->SetPatternDelta(Remove, ui->frameChangeAll->isChecked());
	AddAction(pAction);
}

void CMainFrame::on_speed_valueChanged(int arg1)
{
   CFamiTrackerDoc* pDoc = GetDocument();
   pDoc->SetSongSpeed(arg1);
}

void CMainFrame::on_tempo_valueChanged(int arg1)
{
   CFamiTrackerDoc* pDoc = GetDocument();
   pDoc->SetSongTempo(arg1);
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
   CFamiTrackerDoc* pDoc = GetDocument();
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
   CFamiTrackerDoc* pDoc = GetDocument();
   pDoc->SetSongName(arg1.toAscii().data());
}

void CMainFrame::on_author_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = GetDocument();
   pDoc->SetSongArtist(arg1.toAscii().data());
}

void CMainFrame::on_copyright_textEdited(const QString &arg1)
{
   CFamiTrackerDoc* pDoc = GetDocument();
   pDoc->SetSongCopyright(arg1.toAscii().data());
}

void CMainFrame::setFileName(QString fileName)
{
   m_fileName = fileName;

   GetDocument()->OnOpenDocument((TCHAR*)fileName.toAscii().constData());
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
//	m_iInstrument = Index;

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
   qDebug("GetSelectedInstrument");
	// Returns selected instrument
//	return m_iInstrument;
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
   qDebug("UpdateTrackBox");
//	// Fill the track box with all songs
//	CComboBox		*pTrackBox	= (CComboBox*)ui->songs;
////	CComboBox		*pTrackBox	= (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
//	CFamiTrackerDoc	*pDoc		= (CFamiTrackerDoc*)GetActiveDocument();
//	CString			Text;

//	ASSERT(pTrackBox != NULL);
//	ASSERT(pDoc != NULL);

//	pTrackBox->ResetContent();

//	int Count = pDoc->GetTrackCount();

//	for (int i = 0; i < Count; ++i) {
//		Text.Format(_T("#%i %s"), i + 1, pDoc->GetTrackTitle(i));
//		pTrackBox->AddString(Text);
//	}

//	if (m_iTrack >= Count)
//		m_iTrack = Count - 1;

//	pTrackBox->SetCurSel(m_iTrack);
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

void CMainFrame::OnNextSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
//	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
   CComboBox *pTrackBox = (CComboBox*)ui->songs;
	
	if (m_iTrack < (signed)pDoc->GetTrackCount() - 1)
		pDoc->SelectTrack(m_iTrack + 1);
}

void CMainFrame::OnPrevSong()
{
	CFamiTrackerDoc *pDoc = (CFamiTrackerDoc*)GetActiveDocument();
   //	CComboBox *pTrackBox = (CComboBox*)m_wndDialogBar.GetDlgItem(IDC_SUBTUNE);
      CComboBox *pTrackBox = (CComboBox*)ui->songs;

	if (m_iTrack > 0)
		pDoc->SelectTrack(m_iTrack - 1);
}
