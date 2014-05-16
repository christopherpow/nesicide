#include "playlisteditordialog.h"
#include "ui_playlisteditordialog.h"

#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTreeWidgetItem>

PlaylistEditorDialog::PlaylistEditorDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::PlaylistEditorDialog)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList folderList = settings.value("FolderList").toStringList();
   QList<QTreeWidgetItem*> twis;

   ui->setupUi(this);
   
   setWindowTitle("Playlist Editor: No playlist loaded.");
   
   foreach ( QString folder, folderList )
   {
      QTreeWidgetItem* twi = new QTreeWidgetItem(QStringList(folder));
      twi->setFlags(twi->flags()|Qt::ItemIsUserCheckable);
      twi->setCheckState(0,Qt::Checked);
      twis.append(twi);
   }

   ui->playlist->addTopLevelItems(twis);
   
   foreach ( QTreeWidgetItem* twi, twis )
   {
      QDir dir(twi->text(0));
      QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
      foreach ( QFileInfo fileInfo, fileInfos )
      {
         QTreeWidgetItem* twic = new QTreeWidgetItem(QStringList(fileInfo.fileName()));
         twic->setFlags(twi->flags()|Qt::ItemIsUserCheckable);
         twic->setCheckState(0,Qt::Checked);
         twi->addChild(twic);
      }
   }
   
   ui->rescan->setEnabled(false);   
   if ( !settings.value("PlaylistFile").toString().isEmpty() )
   {
      populateTreeFromFile(settings.value("PlaylistFile").toString());
   }
}

PlaylistEditorDialog::~PlaylistEditorDialog()
{
   delete ui;
}

void PlaylistEditorDialog::on_playlist_itemChanged(QTreeWidgetItem *item, int column)
{
   int children = item->childCount();
   int child;
   
   if ( children > 0 )
   {
      for ( child = 0; child < children; child++ )
      {
         item->child(child)->setCheckState(0,item->checkState(0));
      }
   }
   else
   {
      if ( item->checkState(0) == Qt::Checked )
      {
         ui->playlist->blockSignals(true);
         item->parent()->setCheckState(0,Qt::Checked);
         ui->playlist->blockSignals(false);
      }
   }
}

void PlaylistEditorDialog::on_load_clicked()
{
   QString fileName = QFileDialog::getOpenFileName(this,"Load Playlist",QDir::currentPath(),
                      "FamiPlayer Playlist (*.fpl)");
   if ( !fileName.isEmpty() )
   {
      populateTreeFromFile(fileName);
   }
}

void PlaylistEditorDialog::on_save_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(this,"Save Playlist",QDir::currentPath(),
                      "FamiPlayer Playlist (*.fpl)");
   if ( !fileName.isEmpty() )
   {
      dumpTreeToFile(fileName);
   }
}

void PlaylistEditorDialog::on_cancel_clicked()
{
   reject();    
}

QStringList PlaylistEditorDialog::dumpTreeToList()
{
   QTreeWidgetItemIterator twi(ui->playlist);
   QDir folder;
   QString file;
   QStringList files;
   
   while ( *twi )
   {
      if ( (*twi)->childCount() )
      {
         // root == folder
         if ( (*twi)->checkState(0) == Qt::Checked )
         {
            folder.setPath((*twi)->text(0));
         }
      }
      else
      {
         if ( (*twi)->checkState(0) == Qt::Checked )
         {
            file = folder.filePath((*twi)->text(0));
            files.append(file.toLatin1());
         }
      }
      ++twi;
   }
   return files;
}

void PlaylistEditorDialog::dumpTreeToFile(QString playlistFileName)
{
   QFile playlistFile(playlistFileName);
   QStringList files = dumpTreeToList();
   
   playlistFile.open(QIODevice::WriteOnly);
   if ( playlistFile.isOpen() )
   {  
      foreach ( QString file, files )
      {
         playlistFile.write(QString(file+"\r\n").toLatin1());
      }
      playlistFile.close();
   }
   else
   {
      QMessageBox::critical(this,"Error!","Could not open playlist file!");
   }
}

void PlaylistEditorDialog::populateTreeFromFile(QString playlistFileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList fileFullList;
   QStringList folderList;
   QList<QTreeWidgetItem*> twis;
   QFile playlistFile(playlistFileName);
   QString fileToPlay;
   QFileInfo fileInfo;
   
   playlistFile.open(QIODevice::ReadOnly);
   if ( playlistFile.isOpen() )
   {
      ui->playlist->clear();
      while ( !(fileToPlay = playlistFile.readLine()).isEmpty() )
      {
         fileFullList.append(QDir::fromNativeSeparators(fileToPlay));
         fileInfo.setFile(fileToPlay);
         folderList.append(QDir::fromNativeSeparators(fileInfo.path()));
      }
      folderList.removeDuplicates();
      fileFullList.removeDuplicates();
      
      foreach ( QString folder, folderList )
      {
         QFileInfo folderInfo(folder);      
         QTreeWidgetItem* twi = new QTreeWidgetItem(QStringList(folder));
         twi->setFlags(twi->flags()|Qt::ItemIsUserCheckable);
         twi->setCheckState(0,Qt::Checked);
         if ( !folderInfo.exists() )
         {
            twi->setDisabled(true);
         }
         twis.append(twi);
      }
   
      ui->playlist->addTopLevelItems(twis);
      
      foreach ( QTreeWidgetItem* twi, twis )
      {
         foreach ( QString file, fileFullList )
         {
            file.remove(QRegExp("[\r\n]"));
            fileInfo.setFile(file);
            QTreeWidgetItem* twic = new QTreeWidgetItem(QStringList(fileInfo.fileName()));
            twic->setFlags(twi->flags()|Qt::ItemIsUserCheckable);
            twic->setCheckState(0,Qt::Checked);
            twis = ui->playlist->findItems(fileInfo.path(),Qt::MatchExactly);
            if ( twis.count() == 1 )
            {
               twis.at(0)->addChild(twic);
            }
            if ( !fileInfo.exists() )
            {
               twic->setDisabled(true);
            }
         }
      }
      
      settings.setValue("PlaylistFile",playlistFileName);
      ui->rescan->setEnabled(true);
      
      QString title;
      title = "Playlist Editor: ";
      title += playlistFileName;
      setWindowTitle(title);
      
      playlistFile.close();
   }
   else
   {
      QMessageBox::critical(this,"Error!","Could not open playlist file!");
   }
}

void PlaylistEditorDialog::populateTreeFromDisk()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QList<QTreeWidgetItem*> twis;
   QString fileToPlay;
   QFileInfo fileInfo;
   QTreeWidgetItemIterator twi(ui->playlist);
   
   while ( (*twi) )
   {
      if ( !(*twi)->parent() )
      {         
         QFileInfo folderInfo((*twi)->text(0));      
         (*twi)->setDisabled(false);
         (*twi)->takeChildren();
         if ( !folderInfo.exists() )
         {
            (*twi)->setDisabled(true);
         }
         else
         {
            QDir dir((*twi)->text(0));
            QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
            foreach ( QFileInfo fileInfo, fileInfos )
            {
               QTreeWidgetItem* twic = new QTreeWidgetItem(QStringList(QDir::fromNativeSeparators(fileInfo.fileName())));
               twic->setFlags((*twi)->flags()|Qt::ItemIsUserCheckable);
               twic->setCheckState(0,Qt::Checked);
               if ( !fileInfo.exists() )
               {
                  twic->setDisabled(true);
               }
               (*twi)->addChild(twic);
            }
         }
      }
      ++twi;
   }
}

void PlaylistEditorDialog::on_rescan_clicked()
{
   populateTreeFromDisk();
}

QStringList PlaylistEditorDialog::playlist()
{
   return dumpTreeToList();
}

void PlaylistEditorDialog::on_ok_clicked()
{
   accept();
}

void PlaylistEditorDialog::on_useINI_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.remove("PlaylistFile");
   accept();
}
