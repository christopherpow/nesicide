#include "MainFrame.h"
#include "ui_MainFrame.h"

#include "famitrackermodulepropertiesdialog.h"
#include "famitracker/SoundGen.h"

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

CFamiTrackerPatternTableView::CFamiTrackerPatternTableView(QWidget *parent)
 : QTableView(parent)
{
}

void CFamiTrackerPatternTableView::resizeEvent(QResizeEvent *event)
{
   QTableView::resizeEvent(event);
   
   scrollTo(selectionModel()->currentIndex(),QAbstractItemView::PositionAtCenter);
}

void CFamiTrackerPatternTableView::setModel(QAbstractItemModel *model)
{
   QTableView::setModel(model);
   
   setCurrentIndex(model->index(0,0));
}

void CFamiTrackerPatternTableView::paintEvent(QPaintEvent *event)
{
//   QTableView::paintEvent(event);
   
   CDC dc(viewport());
   CPen pen1(PS_SOLID,12,RGB(255,0,0));
   CPen pen2(PS_DASHDOT,4,RGB(0,255,0));
   dc.MoveTo(10,10);
   dc.SelectObject(&pen1);
   dc.LineTo(100,100);
   dc.SelectObject(&pen2);
   dc.LineTo(200,10);
   
   CFont font;
   LOGFONT lf;
   strcpy((char*)lf.lfFaceName,"Courier");
   lf.lfHeight = 12;
   font.CreateFontIndirect(&lf);

//   int channel;
//   int column = 0;
//   QPainter p(viewport());
//   p.setPen(QColor(120,120,120));
   
//   // Draw channel separation lines.
//   for ( channel = 1; channel < m_pDocument->GetChannelCount(); channel++ )
//   {      
//      column += m_pDocument->GetChannelColumns(channel-1);
//      p.drawLine(columnViewportPosition(column),event->rect().top(),columnViewportPosition(column),event->rect().bottom());
//   }
}

CMainFrame::CMainFrame(QString fileName,QWidget *parent) :
   QWidget(parent),
   ui(new Ui::CMainFrame)
{
   int width;
   int col;
   int song;
   int idx;

   ui->setupUi(this);
   
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
   ui->songPatterns->AssignDocument(m_pDocument);
   
   // Connect views together the Qt way.
   QObject::connect(ui->songFrames,SIGNAL(selectFrame(uint)),this,SLOT(frameEditor_selectFrame(uint)));
}

CMainFrame::~CMainFrame()
{
   delete ui;
   delete instrumentsModel;
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
