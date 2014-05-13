#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cqtmfc_famitracker.h"

#include "Source/FamiTracker.h"
#include "Source/FamiTrackerDoc.h"
#include "Source/FamiTrackerView.h"
#include "Source/PatternEditor.h"
#include "Source/MainFrm.h"
#include "Source/SoundGen.h"
#include "Source/SampleWindow.h"

#include <QFileInfo>
#include <QCompleter>
#include <QUrl>

IMPLEMENT_DYNAMIC(CWndMFC,CDialog)

BEGIN_MESSAGE_MAP(CWndMFC,CDialog)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIME, OnDeltaposSpinTime)
END_MESSAGE_MAP()

BOOL CWndMFC::OnInitDialog()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   CString str;
   int Minutes, Seconds;
   int Time = settings.value("PlayTime",180).toInt();
   if ( Time == 0 )
   {
      Time = 10;
   }

	if (Time < 1)
		Time = 1;
//	if (Time > MAX_PLAY_TIME)
//		Time = MAX_PLAY_TIME;

	Seconds = Time % 60;
	Minutes = Time / 60;

	str.Format(_T("%02i:%02i"), Minutes, Seconds);
	SetDlgItemText(IDC_SECONDS, str);
}

int CWndMFC::GetTimeLimit()
{
	int Minutes, Seconds, Time;
	TCHAR str[256];

	GetDlgItemText(IDC_SECONDS, str, 256);
	_stscanf(str, _T("%u:%u"), &Minutes, &Seconds);
	Time = (Minutes * 60) + (Seconds % 60);

	if (Time < 1)
		Time = 1;
//	if (Time > MAX_PLAY_TIME)
//		Time = MAX_PLAY_TIME;

	return Time;
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
   QStringList folderList = settings.value("FolderList").toStringList();
   QString currentFolder = settings.value("CurrentFolder").toString();
   QString currentFile = settings.value("CurrentFile").toString();
   
   m_pTimer = new QTimer;
   QObject::connect(m_pTimer,SIGNAL(timeout()),this,SLOT(onIdleSlot()));
   m_pTimer->start();
   
   m_pSettleTimer = new QTimer;
   QObject::connect(m_pSettleTimer,SIGNAL(timeout()),this,SLOT(settleTimer_timeout()));
   m_bCheck = true;

   ui->setupUi(this);

   ui->paths->setDuplicatesEnabled(false);

   // Initialize the app...
   backgroundifyFamiTracker();
   qtMfcInit(this);
   theApp.InitInstance();

   if ( !folderList.isEmpty() )
   {
      ui->paths->addItems(folderList);
   }
   
   if ( !currentFolder.isEmpty() )
   {
      changeFolder(currentFolder);
   }

   if ( !currentFile.isEmpty() )
   {
      QFileInfo fileInfo(currentFile);
      ui->current->setCurrentIndex(ui->current->findText(fileInfo.fileName()));
      loadFile(currentFile);
   }
   
   ui->indicators->layout()->addWidget(AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR)->GetDlgItem(ID_INDICATOR_TIME)->toQWidget());

   m_bChangeSong = false;
   
   // Enable grabbing the slider while playing.
   ui->position->installEventFilter(this);
   
   m_bDraggingPosition = false;
   
   // Create play-time widget...from MFC stuff.
//       EDITTEXT        IDC_SECONDS,53,37,44,12,ES_AUTOHSCROLL
   m_pWndMFC = new CWndMFC();   
   m_pWndMFC->Create(0,AfxGetMainWnd());
   QRect qrect = QRect(0,0,66,21);
   m_pWndMFC->setFixedSize(qrect.width(),qrect.height());
   CRect rect(0,0,qrect.width(),qrect.height());
   CEdit* mfc9 = new CEdit(m_pWndMFC);
   mfc9->Create(ES_AUTOHSCROLL | WS_VISIBLE,rect,m_pWndMFC,IDC_SECONDS);
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_SECONDS,mfc9);
//       CONTROL         "",IDC_SPIN_TIME,"msctls_updown32",UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS,93,36,11,14
   CSpinButtonCtrl* mfc10 = new CSpinButtonCtrl(m_pWndMFC);
   CRect r10(CPoint(0,0),CSize(16,14));
   mfc10->Create(UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_VISIBLE,r10,m_pWndMFC,IDC_SPIN_TIME);   
   m_pWndMFC->mfcToQtWidgetMap()->insert(IDC_SPIN_TIME,mfc10);
   ui->playTime->layout()->addWidget(m_pWndMFC->toQWidget());
   
   m_pWndMFC->OnInitDialog();
   
   ui->sampleWindow->installEventFilter(this);
   
   ui->current->completer()->setCompletionMode(QCompleter::PopupCompletion);
   QObject::connect(ui->current->lineEdit(),SIGNAL(returnPressed()),this,SLOT(current_returnPressed()));
   ui->current->setInsertPolicy(QComboBox::NoInsert);
   
   m_bPlaying = false;
   ui->playOnStart->setChecked(settings.value("PlayOnStart",false).toBool());
   if ( settings.value("PlayOnStart",false).toBool() )
   {
      on_playStop_clicked();
   }
   
   ui->visuals->setChecked(settings.value("Visuals",true).toBool());
   on_visuals_toggled(settings.value("Visuals",true).toBool());
   
   ui->timeLimit->setChecked(settings.value("TimeLimit",true).toBool());
   on_timeLimit_toggled(settings.value("TimeLimit",true).toBool());
   
   m_iCurrentShuffleIndex = 0;
   ui->shuffle->setChecked(settings.value("Shuffle",false).toBool());
   on_shuffle_toggled(settings.value("Shuffle",false).toBool());
   
   ui->repeat->setChecked(settings.value("Repeat",false).toBool());
   on_repeat_toggled(settings.value("Repeat",false).toBool());
   
   restoreGeometry(settings.value("WindowGeometry").toByteArray());
   restoreState(settings.value("WindowState").toByteArray());
}

MainWindow::~MainWindow()
{   
   delete ui;
   delete m_pTimer;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
   if ( object == ui->position && event->type() == QEvent::MouseButtonPress )
   {
      m_bDraggingPosition = true;
   }
   else if ( object == ui->position && event->type() == QEvent::MouseButtonRelease )
   {
      m_bDraggingPosition = false;
   }
   else if ( object == ui->sampleWindow && event->type() == QEvent::Paint )
   {      
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
      CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
      
      QPainter p;
      QRect rect = pMainFrame->GetSampleWindow()->toQWidget()->rect().adjusted(3,3,-3,-3);
      QPixmap pixmap(rect.size());
      p.begin(&pixmap);
      pMainFrame->GetSampleWindow()->toQWidget()->render(&p,QPoint(),QRegion(3,3,rect.width(),rect.height()));
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
   m_bCheck = true;
   m_pSettleTimer->stop();
}

void MainWindow::onIdleSlot()
{
   CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();   
   CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   CFamiTrackerView* pView = (CFamiTrackerView*)pMainFrame->GetActiveView();
   
   ui->sampleWindow->update();

   if ( m_bCheck )
   {
      // Move to next song if playing and the current song has reached a stop.
      if ( m_bCheck && m_bPlaying && !m_bChangeSong )
      {
         // Check if time is at time limit and if so, advance to next song.
         CString timeLimit;
         CString totalPlayTime;
         
         m_pWndMFC->GetDlgItemText(IDC_SECONDS,timeLimit);
         AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR)->GetDlgItemText(ID_INDICATOR_TIME,totalPlayTime);
         
         // Strip millisecs.
         totalPlayTime = totalPlayTime.Left(totalPlayTime.GetLength()-3);
         
         if ( (ui->timeLimit->isChecked()) && 
              (timeLimit == totalPlayTime.Right(timeLimit.GetLength())) )
         {
            // Force stop...
            m_bPlaying = false;
            AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
            m_bChangeSong = true;
            
            // Create a bit of a delay between songs.
            m_pTimer->start(500);
         }
         else if ( !pApp->GetSoundGenerator()->IsPlaying() )
         {
            m_bPlaying = false;
            m_bChangeSong = true;
            
            // Create a bit of a delay between songs.
            m_pTimer->start(500);
         }
         else
         {
            if ( m_bDraggingPosition )
            {
               // FF/RW
               pView->GetPatternView()->JumpToFrame(ui->position->value()/pDoc->GetPatternLength());
               pView->GetPatternView()->JumpToRow(ui->position->value()%pDoc->GetPatternLength());
            }
            else
            {
               ui->position->setValue((pView->GetPatternView()->GetPlayFrame()*pDoc->GetPatternLength())+pView->GetPatternView()->GetPlayRow());
            }
         }
      }
      // Wait until player starts playing before turning the above logic back on.
      else if ( m_bChangeSong )
      {
         if ( !ui->repeat->isChecked() )
         {
            on_next_clicked();
         }
         on_playStop_clicked();
         pView->GetPatternView()->JumpToFrame(0);
         pView->GetPatternView()->JumpToRow(0);
         m_bChangeSong = false;
         m_bCheck = false;
         m_pSettleTimer->start(800);
         m_pTimer->start(0);
      }
   }
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

         if ( !fileInfo.suffix().compare("ftm",Qt::CaseInsensitive) )
         {
            loadFile(fileName);
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

void MainWindow::on_browse_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList folderList = settings.value("FolderList").toStringList();
   QString lastFolder = settings.value("LastFolder").toString();
   lastFolder = QFileDialog::getExistingDirectory(0,"Select a folder of FTMs...",lastFolder,0);
   if ( !lastFolder.isEmpty() )
   {
      ui->paths->addItem(lastFolder);
      settings.setValue("LastFolder",lastFolder);

      QDir dir(lastFolder);
      QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
      if ( fileInfos.count() )
      {
         folderList.append(lastFolder);
         folderList.removeDuplicates();
         settings.setValue("FolderList",folderList);
         changeFolder(lastFolder);
         createShuffleLists();
      }
   }
}

void MainWindow::on_playStop_clicked()
{
   AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
   m_bPlaying = !m_bPlaying;
   if ( m_bPlaying )
   {
      ui->playStop->setIcon(QIcon(":/resources/Pause-icon.png"));
   }
   else
   {
      ui->playStop->setIcon(QIcon(":/resources/stock_media-play.png"));
   }
}

void MainWindow::on_previous_clicked()
{
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   
   if ( pDoc->GetSelectedTrack() > 0 )
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
         ui->paths->setCurrentIndex(ui->paths->findText(m_shuffleListFolder.at(m_iCurrentShuffleIndex)));
         ui->current->setCurrentIndex(ui->current->findText(m_shuffleListSong.at(m_iCurrentShuffleIndex)));
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
      pMainFrame->SelectTrack(pDoc->GetTrackCount()-1);
   }
   
   ui->position->setRange(0,(pDoc->GetFrameCount()*pDoc->GetPatternLength()));
   ui->position->setPageStep(pDoc->GetPatternLength());
}

void MainWindow::on_next_clicked()
{
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();

   if ( pDoc->GetSelectedTrack() < pDoc->GetTrackCount()-1 )
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
         ui->paths->setCurrentIndex(ui->paths->findText(m_shuffleListFolder.at(m_iCurrentShuffleIndex)));
         ui->current->setCurrentIndex(ui->current->findText(m_shuffleListSong.at(m_iCurrentShuffleIndex)));
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
      pMainFrame->SelectTrack(0);
   }
   
   ui->position->setRange(0,(pDoc->GetFrameCount()*pDoc->GetPatternLength()));
   ui->position->setPageStep(pDoc->GetPatternLength());
}

void MainWindow::on_paths_currentIndexChanged(const QString &arg1)
{
   changeFolder(arg1);
   if ( m_bPlaying )
   {
      AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
   }   
}

void MainWindow::on_current_currentIndexChanged(const QString &arg1)
{
   changeSong(arg1);
   if ( m_bPlaying )
   {
      AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
   }
}

void MainWindow::changeFolder(QString newFolderPath)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   
   settings.setValue("CurrentFolder",newFolderPath);

   ui->paths->setCurrentIndex(ui->paths->findText(newFolderPath));
   
   QDir dir(ui->paths->currentText());
   QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
   ui->current->clear();
   foreach ( QFileInfo fileInfo, fileInfos )
   {
      ui->current->addItem(fileInfo.fileName(),fileInfo.filePath());
   }
   
   ui->current->setCurrentIndex(0);
   
   QFileInfo fileInfo(ui->current->itemData(ui->current->currentIndex()).toString());
   loadFile(fileInfo.filePath());
   updateSubtuneText();
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
      subtune.sprintf("#%d %s",song+1,pDoc->GetTrackTitle(song));
      ui->subtune->addItem(subtune);
   }
}

void MainWindow::loadFile(QString file)
{   
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();
   
   openFile(file);
   settings.setValue("CurrentFile",file);
   
   ui->position->setRange(0,(pDoc->GetFrameCount()*pDoc->GetPatternLength()));
   ui->position->setPageStep(pDoc->GetPatternLength());
}

void MainWindow::on_subtune_currentIndexChanged(int index)
{
   if ( index != -1 )
   {
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
      CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)pMainFrame->GetActiveDocument();   
      int current = pDoc->GetSelectedTrack();
      
      if ( current < index )
      {
         while ( current != index )
         {
            AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
            AfxGetMainWnd()->OnCmdMsg(ID_NEXT_SONG,0,0,0);
            AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
            current++;
         }
      }      
      else
      {
         while ( current != index )
         {
            AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
            AfxGetMainWnd()->OnCmdMsg(ID_PREV_SONG,0,0,0);
            AfxGetApp()->OnCmdMsg(ID_TRACKER_TOGGLE_PLAY,0,0,0);
            current--;
         }
      }
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
   int idx;
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
   
   for ( idx = 0; idx < m_shuffleListFolder.count(); idx++ )
   {
      moveIdx = rand()%m_shuffleListFolder.count();
      pathSave = m_shuffleListFolder.takeAt(idx);
      m_shuffleListFolder.insert(moveIdx,pathSave);
      songSave = m_shuffleListSong.takeAt(idx);
      m_shuffleListSong.insert(moveIdx,songSave);
   }
   
   m_iCurrentShuffleIndex = rand()%m_shuffleListFolder.count();
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
   }
}

void MainWindow::on_timeLimit_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.setValue("TimeLimit",checked);
}
