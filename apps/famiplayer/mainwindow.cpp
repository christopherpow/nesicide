#include <QDateTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileInfo>
#include <QCompleter>
#include <QUrl>
#include <QMessageBox>
#include <QFileDialog>

#include "cqtmfc_famitracker.h"

#include "Source/FamiTracker.h"
#include "Source/FamiTrackerDoc.h"
#include "Source/FamiTrackerView.h"
#include "Source/PatternEditor.h"
#include "Source/MainFrm.h"
#include "Source/SoundGen.h"
#include "Source/VisualizerWnd.h"
#include "Source/Settings.h"

#include "playlisteditordialog.h"
#include "aboutdialog.h"

IMPLEMENT_DYNAMIC(CWndMFC,CDialog)

BEGIN_MESSAGE_MAP(CWndMFC,CDialog)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, OnDeltaposSpinTime)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP, OnDeltaposSpinLoop)
   ON_EN_CHANGE(IDC_TIMES,OnEnChangeTimes)
   ON_EN_CHANGE(IDC_SECONDS,OnEnChangeSeconds)
END_MESSAGE_MAP()

BOOL CWndMFC::OnInitDialog()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   CString str;
   int Minutes, Seconds;
   int Time = settings.value("PlayTime",180).toInt();
   int Loops = settings.value("PlayLoops",1).toInt();
   if ( Time == 0 )
   {
      Time = 10;
   }
   if ( Loops == 0 )
   {
      Loops = 1;
   }

	if (Time < 1)
		Time = 1;
//	if (Time > MAX_PLAY_TIME)
//		Time = MAX_PLAY_TIME;

	Seconds = Time % 60;
	Minutes = Time / 60;

	str.Format(_T("%02i:%02i"), Minutes, Seconds);
	SetDlgItemText(IDC_SECONDS, str);
   
   SetDlgItemInt(IDC_TIMES, Loops);   
}

int CWndMFC::GetFrameLoopCount()
{
	int Frames = GetDlgItemInt(IDC_TIMES);

	if (Frames < 1)
		Frames = 1;
//	if (Frames > MAX_LOOP_TIMES)
//		Frames = MAX_LOOP_TIMES;

	return Frames;
}

int CWndMFC::ConvertTime(LPCTSTR str)
{
   int Minutes, Seconds, Time;

   _stscanf(str, _T("%u:%u"), &Minutes, &Seconds);
	Time = (Minutes * 60) + (Seconds % 60);

	if (Time < 1)
		Time = 1;
//	if (Time > MAX_PLAY_TIME)
//		Time = MAX_PLAY_TIME;

	return Time;
}

int CWndMFC::GetTimeLimit()
{
	TCHAR str[256];

	GetDlgItemText(IDC_SECONDS, str, 256);
   return ConvertTime(str);
}

void CWndMFC::OnEnChangeTimes()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   int Loops = GetFrameLoopCount();

   settings.setValue("PlayLoops",Loops);
}

void CWndMFC::OnEnChangeSeconds()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   int Time = GetTimeLimit();

   settings.setValue("PlayTime",Time);
}

void CWndMFC::OnDeltaposSpinLoop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int Times = GetFrameLoopCount() - pNMUpDown->iDelta;

	if (Times < 1)
		Times = 1;
//	if (Times > MAX_LOOP_TIMES)
//		Times = MAX_LOOP_TIMES;

	SetDlgItemInt(IDC_TIMES, Times);
	*pResult = 0;
}

void CWndMFC::OnDeltaposSpinTime(NMHDR *pNMHDR, LRESULT *pResult)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int Minutes, Seconds;
	int Time = GetTimeLimit() - pNMUpDown->iDelta;
	CString str;

	if (Time < 1)
		Time = 1;
//	if (Time > MAX_PLAY_TIME)
//		Time = MAX_PLAY_TIME;

	Seconds = Time % 60;
	Minutes = Time / 60;

	str.Format(_T("%02i:%02i"), Minutes, Seconds);
	SetDlgItemText(IDC_SECONDS, str);
   
   settings.setValue("PlayTime",Time);
	*pResult = 0;
}

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   
   // Randomizeme!
   srand(QDateTime::currentDateTime().toTime_t());
   
   ui->setupUi(this);
   
   m_pTimer = new QTimer;
   m_pSettleTimer = new QTimer;
   
   // Initialize the app...
   backgroundifyFamiTracker("FamiPlayer");
   qtMfcInit(this);
   theApp.InitInstance();
   
   ui->paths->setDuplicatesEnabled(false);
      
   ui->indicators->layout()->addWidget(AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR)->GetDlgItem(ID_INDICATOR_TIME)->toQWidget());

   m_bChangeSong = false;
   m_bPlaying = false;
   m_bDraggingPosition = false;
         
   // Enable grabbing the slider while playing.
   ui->position->installEventFilter(this);
   
   // Create play-time widget...from MFC stuff.
   m_pWndMFC = new CWndMFC();   
   m_pWndMFC->Create(0,AfxGetMainWnd());
   QRect qrect = QRect(0,0,132,21);
   m_pWndMFC->setFixedSize(qrect.width(),qrect.height());
//       EDITTEXT        IDC_SECONDS,53,37,44,12,ES_AUTOHSCROLL
   CRect r9(0,0,qrect.width()/2,qrect.height());
   CEdit* mfc9 = new CEdit(m_pWndMFC);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,r9,m_pWndMFC,IDC_SECONDS);
   mfc9->toQWidget()->setToolTip("Seconds to Play");
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_SECONDS,mfc9);
//       CONTROL         "",IDC_SPIN_TIME,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,93,36,11,14
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(m_pWndMFC);
   CRect r10(CPoint(0,0),CSize(16,14));
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,m_pWndMFC,IDC_SPIN_TIME);   
   mfc10->toQWidget()->setToolTip("Seconds to Play");
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_SPIN_TIME,mfc10);
   ui->playTime->layout()->addWidget(m_pWndMFC->toQWidget());
//       EDITTEXT        IDC_TIMES,73,19,36,12,ES_AUTOHSCROLL
   CEdit* mfc6 = new CEdit(m_pWndMFC);
   CRect r6((qrect.width()/2)+1,0,qrect.width(),qrect.height());
   mfc6->Create(ES_AUTOHSCROLL | WS_VISIBLE,r6,m_pWndMFC,IDC_TIMES);
   mfc6->toQWidget()->setToolTip("Loops to Play");
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_TIMES,mfc6);
//       CONTROL         "",IDC_SPIN_LOOP,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,105,17,11,17
   CSpinButtonCtrl* mfc7 = new CSpinButtonCtrl(m_pWndMFC);
   CRect r7(CPoint((qrect.width()/2)+1,0),CSize(16,14));
   mfc7->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r7,m_pWndMFC,IDC_SPIN_LOOP);
   mfc7->toQWidget()->setToolTip("Loops to Play");
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_SPIN_LOOP,mfc7);
   
   m_pWndMFC->OnInitDialog();
   
   ui->sampleWindow->installEventFilter(this);
   
   ui->current->completer()->setCompletionMode(QCompleter::PopupCompletion);
   QObject::connect(ui->current->lineEdit(),SIGNAL(returnPressed()),this,SLOT(current_returnPressed()));
   ui->current->setInsertPolicy(QComboBox::NoInsert);
   
   ui->visuals->setChecked(settings.value("Visuals",true).toBool());
   on_visuals_toggled(settings.value("Visuals",true).toBool());
   
   restoreGeometry(settings.value("WindowGeometry").toByteArray());
   restoreState(settings.value("WindowState").toByteArray());
   
   if ( settings.value("PlaylistFile").toString().isEmpty() )
   {
      updateUiFromINI(false);
   }
   else
   {
      updateUiFromPlaylist(false);
   }

   m_bTimeLimited = settings.value("TimeLimiting",true).toBool();   
   m_bLoopLimited = settings.value("LoopLimiting",true).toBool();
   ui->limit->setChecked(m_bTimeLimited || m_bLoopLimited);
   
   m_pLimitMenu = new QMenu;
   QAction* action = m_pLimitMenu->addAction("Time-limiting");
   action->setData(0);
   action->setCheckable(true);
   action = m_pLimitMenu->addAction("Loop-limiting");
   action->setData(1);
   action->setCheckable(true);
   ui->limit->setMenu(m_pLimitMenu);
   QObject::connect(m_pLimitMenu,SIGNAL(aboutToShow()),this,SLOT(limitMenu_aboutToShow()));
   QObject::connect(m_pLimitMenu,SIGNAL(triggered(QAction*)),this,SLOT(limitMenu_triggered(QAction*)));
   
   m_iCurrentShuffleIndex = 0;
   ui->shuffle->setChecked(settings.value("Shuffle",false).toBool());
   on_shuffle_toggled(settings.value("Shuffle",false).toBool());
   
   ui->repeat->setChecked(settings.value("Repeat",false).toBool());
   on_repeat_toggled(settings.value("Repeat",false).toBool());

   ui->playOnStart->setChecked(settings.value("PlayOnStart",false).toBool());
   if ( ui->playStop->isEnabled() && settings.value("PlayOnStart",false).toBool() )
   {
      on_playStop_clicked();
   }
   
   QObject::connect(m_pTimer,SIGNAL(timeout()),this,SLOT(onIdleSlot()));
   m_pTimer->start();
   
   QObject::connect(m_pSettleTimer,SIGNAL(timeout()),this,SLOT(settleTimer_timeout()));
}

MainWindow::~MainWindow()
{   
   delete ui;
   delete m_pTimer;
   delete m_pSettleTimer;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
   if ( object == ui->position && event->type() == QEvent::MouseButtonPress )
   {
      m_bDraggingPosition = true;
      return false;
   }
   else if ( object == ui->position && event->type() == QEvent::MouseButtonRelease )
   {
      m_bDraggingPosition = false;
      return false;
   }
   else if ( object == ui->sampleWindow && event->type() == QEvent::MouseButtonPress )
   {
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      pMainFrame->GetVisualizerWindow()->SendMessage(WM_LBUTTONDOWN);
   }
   else if ( object == ui->sampleWindow && event->type() == QEvent::Paint )
   {      
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      
      QPainter p;
      QRect rect = pMainFrame->GetVisualizerWindow()->toQWidget()->rect().adjusted(3,2,-3,-3);
      QPixmap pixmap(rect.size());
      p.begin(&pixmap);
      pMainFrame->GetVisualizerWindow()->toQWidget()->render(&p,QPoint(),QRegion(3,3,rect.width(),rect.height()));
      p.end();
      p.begin(ui->sampleWindow);
      p.drawPixmap(0,0,pixmap.scaled(p.window().size(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
      p.end();
      return true;
   }
   return false;
}

void MainWindow::settleTimer_timeout()
{
   QObject::connect(m_pTimer,SIGNAL(timeout()),this,SLOT(onIdleSlot()));
   m_pSettleTimer->stop();
}

void MainWindow::onIdleSlot()
{
   CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();   
   CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
   static int lastFrame = -1;
   unsigned int rowCount;

   ui->sampleWindow->update();

   // Move to next song if playing and the current song has reached a stop.
   if ( m_bPlaying && !m_bChangeSong )
   {
      // Check if time is at time limit and if so, advance to next song.
      int timeLimit = m_pWndMFC->GetTimeLimit();
      CString playTime;
      int totalPlayTime;

      pMainFrame->GetDescendantWindow(AFX_IDW_STATUS_BAR)->GetDlgItemText(ID_INDICATOR_TIME,playTime);
      totalPlayTime = m_pWndMFC->ConvertTime(playTime);

      if ( lastFrame != pApp->GetSoundGenerator()->GetPlayerFrame() )
      {
         lastFrame = pApp->GetSoundGenerator()->GetPlayerFrame();
         m_iFramesPlayed++;
      }

      if ( m_bTimeLimited &&
           (timeLimit == totalPlayTime) )
      {
         // Force stop...
         m_bPlaying = false;
         pApp->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
         m_bChangeSong = true;

         // Create a bit of a delay between songs.
         m_pTimer->start(500);
      }
      else if ( m_bLoopLimited &&
                m_iFramesPlayed > pDoc->ScanActualLength(pMainFrame->GetSelectedTrack(),m_pWndMFC->GetFrameLoopCount(),rowCount) )
      {
         // Force stop...
         m_bPlaying = false;
         pApp->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
         m_bChangeSong = true;

         // Create a bit of a delay between songs.
         m_pTimer->start(500);
      }
      if ( !pApp->GetSoundGenerator()->IsPlaying() )
      {
         m_bPlaying = false;
         m_bChangeSong = true;

         // Create a bit of a delay between songs.
         m_pTimer->start(500);
      }
   }
   // Wait until player starts playing before turning the above logic back on.
   else if ( m_bChangeSong )
   {
      on_playStop_clicked();
      if ( !ui->repeat->isChecked() )
      {
         on_next_clicked();
      }
      m_bChangeSong = false;
      ui->position->setValue((pApp->GetSoundGenerator()->GetPlayerFrame()*pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()))+pApp->GetSoundGenerator()->GetPlayerRow());
      startSettleTimer();
      m_pTimer->start(0);
   }
   if ( m_bDraggingPosition )
   {
      // FF/RW
      pView->SelectFrame(ui->position->value()/pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()));
//      pView->SelectRow(ui->position->value()%pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()));
   }
   else
   {
      ui->frames->setText(QString::number(m_iFramesPlayed)+"/"+QString::number(pDoc->ScanActualLength(pMainFrame->GetSelectedTrack(),m_pWndMFC->GetFrameLoopCount(),rowCount)));
      ui->position->setValue((pApp->GetSoundGenerator()->GetPlayerFrame()*pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()))+pApp->GetSoundGenerator()->GetPlayerRow());
   }
}

void MainWindow::startSettleTimer()
{
   QObject::disconnect(m_pTimer,SIGNAL(timeout()),this,SLOT(onIdleSlot()));
   m_pSettleTimer->start(200);
}

void MainWindow::documentClosed()
{
   // TODO: Handle unsaved documents or other pre-close stuffs
   theApp.ExitInstance();

   exit(0);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
   if ( event->mimeData()->hasUrls() )
   {
      event->acceptProposedAction();
   }
}

void MainWindow::dropEvent(QDropEvent *event)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QList<QUrl> fileUrls;
   QString     fileName;
   QFileInfo   fileInfo;
   QStringList extensions;

   if ( event->mimeData()->hasUrls() )
   {
      fileUrls = event->mimeData()->urls();

      foreach ( QUrl fileUrl, fileUrls )
      {
         fileName = fileUrl.toLocalFile();

         fileInfo.setFile(fileName);

         if ( !fileInfo.suffix().compare("fpl",Qt::CaseInsensitive) )
         {
            bool wasPlaying = m_bPlaying;
            if ( wasPlaying )
            {
               on_playStop_clicked();
            }
            settings.setValue("PlaylistFile",fileInfo.filePath());
            updateUiFromPlaylist(wasPlaying);
            event->acceptProposedAction();
         }
      }
   }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   
   settings.setValue("WindowGeometry",saveGeometry());
   settings.setValue("WindowState",saveState());

   // CP: Force synchronization because we're terminating in OnClose and the settings object
   // can't synchronize to disk if we wait for that.
   settings.sync();
   
   AfxGetMainWnd()->OnClose();

   // Ignore the close event.  "MFC" will close the document which will trigger app closure.
   event->ignore();
}

void MainWindow::on_playStop_clicked()
{
   AfxGetMainWnd()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
   m_bPlaying = !m_bPlaying;
   if ( m_bPlaying )
   {
      ui->playStop->setIcon(QIcon(":/resources/Pause-icon.png"));
   }
   else
   {
      ui->playStop->setIcon(QIcon(":/resources/Actions-arrow-right-icon.png"));
   }
}

void MainWindow::on_previous_clicked()
{
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();   
   bool wasPlaying = m_bPlaying;

   if ( wasPlaying )
   {
      on_playStop_clicked();
   }

   if ( pMainFrame->GetSelectedTrack() > 0 )
   {
      ui->subtune->setCurrentIndex(ui->subtune->currentIndex()-1);
   }
   else
   {
      if ( ui->shuffle->isChecked() )
      {
         if ( m_iCurrentShuffleIndex > 0 )
         {
            m_iCurrentShuffleIndex--;
         }
         else
         {
            m_iCurrentShuffleIndex = m_shuffleListFolder.count()-1;
         }
         if ( m_iCurrentShuffleIndex < m_shuffleListFolder.count() )
         {
            ui->paths->setCurrentIndex(ui->paths->findText(m_shuffleListFolder.at(m_iCurrentShuffleIndex)));
            ui->current->setCurrentIndex(ui->current->findText(m_shuffleListSong.at(m_iCurrentShuffleIndex)));
         }
      }
      else
      {
         if ( ui->current->currentIndex() > 0 )
         {
            ui->current->setCurrentIndex(ui->current->currentIndex()-1);
         }
         else
         {
            if ( ui->paths->currentIndex() > 0 )
            {
               ui->paths->setCurrentIndex(ui->paths->currentIndex()-1);
            }
            else
            {
               ui->paths->setCurrentIndex(ui->paths->count()-1);
            }
            ui->current->setCurrentIndex(ui->current->count()-1);
         }
      }
      ui->subtune->setCurrentIndex(ui->subtune->count()-1);
   }

   m_iFramesPlayed = 0;

   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
}

void MainWindow::on_next_clicked()
{
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   bool wasPlaying = m_bPlaying;

   if ( wasPlaying )
   {
      on_playStop_clicked();
   }

   if ( pMainFrame->GetSelectedTrack() < pDoc->GetTrackCount()-1 )
   {
      ui->subtune->setCurrentIndex(ui->subtune->currentIndex()+1);
   }
   else
   {
      if ( ui->shuffle->isChecked() )
      {
         if ( m_iCurrentShuffleIndex < m_shuffleListFolder.count()-1 )
         {
            m_iCurrentShuffleIndex++;
         }
         else
         {
            m_iCurrentShuffleIndex = 0;
         }
         if ( m_iCurrentShuffleIndex < m_shuffleListFolder.count() )
         {
            ui->paths->setCurrentIndex(ui->paths->findText(m_shuffleListFolder.at(m_iCurrentShuffleIndex)));
            ui->current->setCurrentIndex(ui->current->findText(m_shuffleListSong.at(m_iCurrentShuffleIndex)));
         }
      }
      else
      {
         if ( ui->current->currentIndex() < ui->current->count()-1 )
         {
            ui->current->setCurrentIndex(ui->current->currentIndex()+1);
         }
         else
         {
            if ( ui->paths->currentIndex() < ui->paths->count()-1 )
            {
               ui->paths->setCurrentIndex(ui->paths->currentIndex()+1);
            }
            else
            {
               ui->paths->setCurrentIndex(0);
            }
            ui->current->setCurrentIndex(0);
         }
      }
      ui->subtune->setCurrentIndex(0);
   }
   
   m_iFramesPlayed = 0;     

   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
}

void MainWindow::on_paths_currentIndexChanged(const QString &arg1)
{
   bool wasPlaying = m_bPlaying;
   
   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
   
   changeFolder(arg1);
   
   m_iFramesPlayed = 0;     
   
   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
}

void MainWindow::on_current_currentIndexChanged(const QString &arg1)
{
   bool wasPlaying = m_bPlaying;
   
   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
   
   changeSong(arg1);
   
   m_iFramesPlayed = 0;     
   
   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
}

void MainWindow::changeFolder(QString newFolderPath)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList playlist = settings.value("Playlist").toStringList();
   
   settings.setValue("CurrentFolder",newFolderPath);

   ui->current->clear();
   ui->subtune->clear();
   foreach ( QString file, playlist )
   {
      QFileInfo fileInfo(file);
      if ( fileInfo.path() == newFolderPath )
      {
         ui->current->addItem(fileInfo.fileName(),fileInfo.filePath());
      }
   }
   if ( !ui->current->count() )
   {
      ui->paths->removeItem(ui->paths->currentIndex());
   }
}

void MainWindow::changeSong(QString newSongPath)
{
   if ( ui->current->currentIndex() != -1 )
   {
      QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
      QFileInfo fileInfo(ui->current->itemData(ui->current->currentIndex()).toString());
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
      
      loadFile(fileInfo.filePath());
      ui->current->setToolTip(fileInfo.filePath());
      ui->current->lineEdit()->setCursorPosition(0);
      updateSubtuneText();
   }
}

void MainWindow::updateSubtuneText()
{   
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   QString subtune;
   int song;

   ui->subtune->clear();
   for ( song = 0; song < pDoc->GetTrackCount(); song++ )
   {
      subtune.sprintf("#%d %s",song+1,(LPCTSTR)pDoc->GetTrackTitle(song));
      ui->subtune->addItem(subtune);
   }
}

void MainWindow::loadFile(QString file)
{   
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();

   startSettleTimer();
   pDoc = (CFamiTrackerDoc*)openFile(file);
   
   if ( pDoc )
   {
      settings.setValue("CurrentFile",file);
      
      m_iFramesPlayed = 0;
      
      ui->position->setRange(0,(pDoc->GetFrameCount(pMainFrame->GetSelectedTrack())*pDoc->GetPatternLength(pMainFrame->GetSelectedTrack())));
      ui->position->setPageStep(pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()));
   }
   else
   {
      QString str = "Could not find file:\n\n"+file;
      int result = QMessageBox::warning(this,"File load error!",str,"Cancel","Next Track","Previous Track",0);
      if ( result == 1 )
      {
         on_next_clicked();
      }
      else if ( result == 2 )
      {
         on_previous_clicked();
      }
   }
}

void MainWindow::on_subtune_currentIndexChanged(int index)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   
   if ( index != -1 )
   {
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
      CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();   
      int current = pMainFrame->GetSelectedTrack();
      bool wasPlaying = m_bPlaying;
      
      if ( current < index )
      {
         if ( wasPlaying )
         {
            on_playStop_clicked();
         }
         while ( current != index )
         {
            AfxGetMainWnd()->OnCmdMsg(ID_NEXT_SONG,0,0,0);
            current++;
         }
         if ( wasPlaying )
         {
            on_playStop_clicked();
         }
      }      
      else
      {
         if ( wasPlaying )
         {
            on_playStop_clicked();
         }
         while ( current != index )
         {
            AfxGetMainWnd()->OnCmdMsg(ID_PREV_SONG,0,0,0);
            current--;
         }
         if ( wasPlaying )
         {
            on_playStop_clicked();
         }
      }
      
      pView->SelectFrame(0);
//      pView->SelectRow(0);
      
      m_iFramesPlayed = 0;     
      
      ui->position->setRange(0,(pDoc->GetFrameCount(pMainFrame->GetSelectedTrack())*pDoc->GetPatternLength(pMainFrame->GetSelectedTrack())));
      ui->position->setPageStep(pDoc->GetPatternLength(pMainFrame->GetSelectedTrack()));
   }
}

void MainWindow::current_returnPressed()
{
   if ( ui->current->findText(ui->current->lineEdit()->text()) == -1 )
   {
      ui->current->setCurrentIndex(ui->current->currentIndex());
   }
}

void MainWindow::on_visuals_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   int visualsHeight = settings.value("VisualsHeight",100).toInt();
   
   settings.setValue("Visuals",checked);
   if ( checked )
   {
      setMaximumSize(16777215,16777215);
      setBaseSize(rect().width(),visualsHeight+rect().height());
   }
   else
   {
      settings.setValue("VisualsHeight",ui->sampleWindow->rect().height());
      setFixedHeight(minimumHeight());
   }
   
   ui->sampleWindow->setVisible(checked);
   
   QWidget *w = ui->sampleWindow->parentWidget();
   while (w) {
     w->adjustSize();
     w = w->parentWidget();
   }
}

void MainWindow::createShuffleLists()
{
   int path;
   int song;
   int subtune;
   int idx1, idx2;
   int moveIdx;
   QString pathSave;
   QString songSave;
   
   m_shuffleListFolder.clear();
   m_shuffleListSong.clear();
   
   for ( path = 0; path < ui->paths->count(); path++ )
   {
      QDir dir(ui->paths->itemText(path));
      QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
      for ( song = 0; song < fileInfos.count(); song++ )
      {
         m_shuffleListFolder.append(ui->paths->itemText(path));
         m_shuffleListSong.append(fileInfos.at(song).fileName());
      }
   }

   // Shuffle seven times for good luck.   
   for ( idx1 = 0; idx1 < 7; idx1++ )
   {
      for ( idx2 = 0; idx2 < m_shuffleListFolder.count(); idx2++ )
      {
         moveIdx = rand()%m_shuffleListFolder.count();
         pathSave = m_shuffleListFolder.takeAt(0);
         m_shuffleListFolder.insert(moveIdx,pathSave);
         songSave = m_shuffleListSong.takeAt(0);
         m_shuffleListSong.insert(moveIdx,songSave);
      }
   }
   
   if ( m_shuffleListFolder.count() )
   {
      m_iCurrentShuffleIndex = rand()%m_shuffleListFolder.count();
   }
   else
   {
      m_iCurrentShuffleIndex = 0;
   }
}

void MainWindow::on_playOnStart_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.setValue("PlayOnStart",checked);
}

void MainWindow::on_repeat_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.setValue("Repeat",checked);    
}

void MainWindow::on_shuffle_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.setValue("Shuffle",checked);
   if ( checked )
   {
      createShuffleLists();
      on_next_clicked();
   }
}

void MainWindow::updateUiFromINI(bool wasPlaying)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList playlist = settings.value("Playlist").toStringList();
   QStringList folderList;
   QString currentFolder = settings.value("CurrentFolder").toString();
   QString currentFile = settings.value("CurrentFile").toString();
   
   ui->paths->clear();
   ui->current->clear();
   ui->subtune->clear();

   foreach ( QString file, playlist )
   {
      QFileInfo fileInfo(file);
      folderList.append(fileInfo.path());
   }
   folderList.removeDuplicates();

   ui->paths->addItems(folderList);
   
   if ( (!currentFolder.isEmpty()) &&
        (ui->paths->findText(currentFolder,Qt::MatchExactly) != -1) )
   {
      ui->paths->setCurrentIndex(ui->paths->findText(currentFolder,Qt::MatchExactly));
   }
   else
   {
      ui->paths->setCurrentIndex(0);
   }
   
   QFileInfo fileInfo(currentFile);
   if ( (!currentFile.isEmpty()) &&
        (ui->current->findText(fileInfo.fileName(),Qt::MatchExactly) != -1) )
   {
      ui->current->setCurrentIndex(ui->current->findText(fileInfo.fileName(),Qt::MatchExactly));
   }
   else
   {
      ui->current->setCurrentIndex(0);
   }
   createShuffleLists();
   
   // If the last folder in the playlist was deleted, or if the playlist has 
   // no songs for some reason, revert to 'factory' configuration.
   if ( !ui->current->count() )
   {
      AfxGetApp()->OnCmdMsg(ID_FILE_NEW,0,0,0);
      ui->playStop->setEnabled(false);
   }
   else
   {      
      ui->playStop->setEnabled(true);
      
      if ( wasPlaying )
      {
         on_playStop_clicked();
      }   
   }
   
   ui->info->setText("No playlist loaded.");
}

void MainWindow::updateUiFromPlaylist(bool wasPlaying)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QString currentFolder = settings.value("CurrentFolder").toString();
   QString currentFile = settings.value("CurrentFile").toString();

   if ( !settings.value("PlaylistFile").toString().isEmpty() )
   {
      PlaylistEditorDialog ped;
      QStringList files = ped.playlist();
      
      ui->paths->clear();
      ui->current->clear();
      ui->subtune->clear();
      
      foreach ( QString file, files )
      {
         QFileInfo fileInfo(file);
         if ( fileInfo.exists() )
         {
            if ( ui->paths->findText(fileInfo.path(),Qt::MatchExactly) == -1 )
            {
               ui->paths->addItem(fileInfo.path());            
            }
            ui->current->addItem(fileInfo.fileName(),fileInfo.filePath());
         }
      }
      ui->info->setText("Playlist: "+settings.value("PlaylistFile").toString());
   }
   else
   {
      ui->info->setText("No playlist loaded.");
   }

   if ( (!currentFolder.isEmpty()) &&
        (ui->paths->findText(currentFolder,Qt::MatchExactly) != -1) )
   {
      ui->paths->setCurrentIndex(ui->paths->findText(currentFolder,Qt::MatchExactly));
   }
   else
   {
      ui->paths->setCurrentIndex(0);
   }

   QFileInfo fileInfo(currentFile);
   if ( (!currentFile.isEmpty()) &&
        (ui->current->findText(fileInfo.fileName(),Qt::MatchExactly) != -1) )
   {
      ui->current->setCurrentIndex(ui->current->findText(fileInfo.fileName(),Qt::MatchExactly));
   }
   else
   {
      ui->current->setCurrentIndex(0);
   }
   createShuffleLists();
   
   // If the last folder in the playlist was deleted, or if the playlist has 
   // no songs for some reason, revert to 'factory' configuration.
   if ( !ui->current->count() )
   {
      AfxGetApp()->OnCmdMsg(ID_FILE_NEW,0,0,0);
      ui->playStop->setEnabled(false);
   }
   else
   {      
      ui->playStop->setEnabled(true);
      
      if ( wasPlaying )
      {
         on_playStop_clicked();
      }   
   }
}

void MainWindow::on_help_clicked()
{
   AboutDialog ad;
   ad.exec();
}

void MainWindow::on_playlist_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   bool wasPlaying = m_bPlaying;
   
   if ( wasPlaying )
   {
      on_playStop_clicked();
   }
   
   PlaylistEditorDialog ped;
   
   ped.exec();
   if ( settings.value("PlaylistFile").toString().isEmpty() )
   {
      updateUiFromINI(wasPlaying);
   }
   else
   {
      updateUiFromPlaylist(wasPlaying);               
   }
   
   // Play-state is restored by update functions.
}

void MainWindow::limitMenu_aboutToShow()
{
   m_pLimitMenu->actions().at(0)->setChecked(m_bTimeLimited);
   m_pLimitMenu->actions().at(1)->setChecked(m_bLoopLimited);
}

void MainWindow::limitMenu_triggered(QAction* action)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   switch ( action->data().toInt() )
   {
   case 0:
      m_bTimeLimited = action->isChecked();
      break;
   case 1:
      m_bLoopLimited = action->isChecked();
      break;
   }
   settings.setValue("TimeLimiting",m_bTimeLimited);
   settings.setValue("LoopLimiting",m_bLoopLimited);
   ui->limit->setChecked(m_bTimeLimited || m_bLoopLimited);
}
