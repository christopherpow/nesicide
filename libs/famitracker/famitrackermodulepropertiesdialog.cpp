#include "famitrackermodulepropertiesdialog.h"
#include "ui_famitrackermodulepropertiesdialog.h"

#include "ChannelMap.h"
#include "FamiTracker.h"
#include "famitrackermoduleimportdialog.h"

#include <QFileDialog>
#include <QSettings>

static LPCTSTR TRACK_FORMAT = _T("#%02i %s");

FamiTrackerModulePropertiesDialog::FamiTrackerModulePropertiesDialog(CFamiTrackerDoc* pDoc, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::FamiTrackerModulePropertiesDialog)
{
   int idx;
   CString TrackTitle;

   ui->setupUi(this);

   m_pDocument = pDoc;

   CChannelMap* pChannelMap = theApp.GetChannelMap();
   for ( idx = 0; idx < pChannelMap->GetChipCount(); idx++ )
   {
#ifdef UNICODE
      ui->expansionSound->addItem(QString::fromWCharArray(pChannelMap->GetChipName(idx)));
#else
      ui->expansionSound->addItem(QString(pChannelMap->GetChipName(idx)));
#endif
      ui->expansionSound->setItemData(idx,pChannelMap->GetChipIdent(idx));
   }

   tracksModel = new QStringListModel();
   
   QStringList songs;
   for ( idx = 0; idx < pDoc->GetTrackCount(); idx++ )
   {      
      TrackTitle.Format(TRACK_FORMAT, idx, pDoc->GetTrackTitle(idx));
      songs.append(TrackTitle);
   }
   tracksModel->setStringList(songs);
   
   ui->tracks->setModel(tracksModel);
   
   ui->expansionSound->setCurrentIndex(pChannelMap->GetChipIndex(pDoc->GetExpansionChip()));
   
   if ( pChannelMap->GetChipIdent(pDoc->GetExpansionChip()) == SNDCHIP_N163)
   {
      ui->channelsN163->setEnabled(true);
   }
   else
   {
      ui->channelsN163->setEnabled(false);
   }
   
   ui->vibratoStyle->setCurrentIndex(m_pDocument->GetVibratoStyle());
   
   updateButtons();
}

FamiTrackerModulePropertiesDialog::~FamiTrackerModulePropertiesDialog()
{
   delete ui;
   delete tracksModel;
}

QStringList FamiTrackerModulePropertiesDialog::tracks()
{
   return tracksModel->stringList();
}

void FamiTrackerModulePropertiesDialog::on_addSong_clicked()
{
   CString TrackTitle;

	// Try to add a track
	if (!m_pDocument->AddTrack())
		return;
	
	// New track is always the last one
	int NewTrack = m_pDocument->GetTrackCount() - 1;
	
   TrackTitle.Format(TRACK_FORMAT, NewTrack, "New song");
   
   QStringList songs = tracksModel->stringList();
   songs.append(TrackTitle);
   tracksModel->setStringList(songs);

   ui->tracks->setCurrentIndex(tracksModel->index(songs.count()-1));
   
   updateButtons();
}

void FamiTrackerModulePropertiesDialog::on_removeSong_clicked()
{
   CString TrackTitle;

	unsigned Count;
   int Song = ui->tracks->currentIndex().row();   

	// Display warning first
//	if (AfxMessageBox(IDS_SONG_DELETE, MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
//		return;

	m_pDocument->RemoveTrack(Song);

	Count = m_pDocument->GetTrackCount();	// Get new track count

   QStringList songs = tracksModel->stringList();
   songs.removeAt(Song);
   tracksModel->setStringList(songs);

   if ( Song == Count ) Song -= 1;
   ui->tracks->setCurrentIndex(tracksModel->index(Song));
   
   updateButtons();
}

void FamiTrackerModulePropertiesDialog::on_moveSongUp_clicked()
{
   CString Text;
   int Song = ui->tracks->currentIndex().row();

	if (Song == (m_pDocument->GetTrackCount() - 1))
		return;

	m_pDocument->MoveTrackUp(Song);

   QStringList songs = tracksModel->stringList();
   songs.swap(Song,Song-1);
   tracksModel->setStringList(songs);

   ui->tracks->setCurrentIndex(tracksModel->index(Song-1));
   
   updateButtons();
}

void FamiTrackerModulePropertiesDialog::on_moveSongDown_clicked()
{
   CString Text;
   int Song = ui->tracks->currentIndex().row();

	if (Song == (m_pDocument->GetTrackCount() - 1))
		return;

	m_pDocument->MoveTrackDown(Song);

   QStringList songs = tracksModel->stringList();
   songs.swap(Song,Song+1);
   tracksModel->setStringList(songs);

   ui->tracks->setCurrentIndex(tracksModel->index(Song+1));
   
   updateButtons();
}

void FamiTrackerModulePropertiesDialog::on_importFile_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString importFile = QFileDialog::getOpenFileName(this,"Import file",settings.value("LastFamiTrackerImportPath",QDir::currentPath()).toString(),"FamiTracker Modules (*.ftm);;");
   CString TrackTitle;
   int idx;
   
   if ( !importFile.isEmpty() )
   {
      QFileInfo importFileInfo(importFile);
      FamiTrackerModuleImportDialog dlg(m_pDocument);
      
      settings.setValue("LastFamiTrackerImportPath",importFileInfo.path());

      bool loadedOk = dlg.loadFile(importFile);
      
      if ( loadedOk )
      {
         dlg.exec();
         
         // Update song list
         QStringList songs;
         for ( idx = 0; idx < m_pDocument->GetTrackCount(); idx++ )
         {      
            TrackTitle.Format(TRACK_FORMAT, idx, m_pDocument->GetTrackTitle(idx));
            songs.append(TrackTitle);
         }
         tracksModel->setStringList(songs);         
      }
   }
}

void FamiTrackerModulePropertiesDialog::on_tracks_clicked(const QModelIndex &index)
{
   updateButtons();
}

void FamiTrackerModulePropertiesDialog::on_expansionSound_currentIndexChanged(int index)
{   
   int chip = ui->expansionSound->itemData(index).toInt();

   if ( chip == SNDCHIP_N163)
   {
      ui->channelsN163->setEnabled(true);
      ui->channelsN163->setValue(0);
      
      QString str;
      str = "Channels: 1";
      ui->channelsN163Group->setTitle(str);
   }
   else
   {
      ui->channelsN163->setValue(0);
      ui->channelsN163->setEnabled(false);
      ui->channelsN163Group->setTitle("Channels: N/A");
   }
}

void FamiTrackerModulePropertiesDialog::on_buttonBox_accepted()
{
   int chip = ui->expansionSound->itemData(ui->expansionSound->currentIndex()).toInt();

   // Expansion chip may have changed...
   m_pDocument->SelectExpansionChip(chip);
   
   m_pDocument->SetModifiedFlag();
}

void FamiTrackerModulePropertiesDialog::on_channelsN163_valueChanged(int value)
{
   QString str;
   str = "Channels: ";
   str += QString::number(value);
   ui->channelsN163Group->setTitle(str);
}

void FamiTrackerModulePropertiesDialog::updateButtons()
{
   QModelIndex index = ui->tracks->currentIndex();
   
   if ( tracksModel->rowCount() < MAX_TRACKS )
   {
      ui->importFile->setEnabled(true);
   }
   else
   {
      ui->importFile->setEnabled(false);
   }
   if ( index.isValid() )
   {
      if ( tracksModel->rowCount() > 1 )
      {
         ui->removeSong->setEnabled(true);
      }
      else
      {
         ui->removeSong->setEnabled(false);
      }
      if ( index.row() == 0 )
      {
         ui->moveSongUp->setEnabled(false);
      }
      else
      {
         ui->moveSongUp->setEnabled(true);
      }
      if ( index.row() == tracksModel->rowCount()-1 )
      {
         ui->moveSongDown->setEnabled(false);
      }
      else
      {
         ui->moveSongDown->setEnabled(true);
      }
   }
   else
   {
      ui->moveSongDown->setEnabled(false);
      ui->moveSongUp->setEnabled(false);
      ui->removeSong->setEnabled(false);
   }
}
