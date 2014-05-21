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
   ui->setupUi(this);
   
   populateTreeFromINI();
   populateTreeFromFile();
}

PlaylistEditorDialog::~PlaylistEditorDialog()
{
   delete ui;
}

void PlaylistEditorDialog::on_playlist_itemChanged(QTreeWidgetItem *item, int column)
{
   int children = item->childCount();
   int child;
   
   if ( !item->parent() )
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
   setWindowModified(true);
}

void PlaylistEditorDialog::on_load_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QString fileName = QFileDialog::getOpenFileName(this,"Load Playlist",QDir::currentPath(),
                      "FamiPlayer Playlist (*.fpl)");
   if ( !fileName.isEmpty() )
   {
      settings.setValue("PlaylistFile",fileName);
      populateTreeFromFile();
   }
}

void PlaylistEditorDialog::on_save_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QString fileName = QFileDialog::getSaveFileName(this,"Save Playlist",QDir::currentPath(),
                      "FamiPlayer Playlist (*.fpl)");
   if ( !fileName.isEmpty() )
   {
      settings.setValue("PlaylistFile",fileName);      
      dumpTreeToFile(fileName);
      populateTreeFromFile();
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
      if ( !(*twi)->parent() )
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
      setWindowModified(false);
   }
   else
   {
      QMessageBox::critical(this,"Error!","Could not open playlist file!");
   }
}

void PlaylistEditorDialog::populateTreeFromFile()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList fileFullList;
   QFile playlistFile(settings.value("PlaylistFile").toString());
   QString fileToPlay;
   QFileInfo fileInfo;

   if ( !settings.value("PlaylistFile").toString().isEmpty() )
   {
      playlistFile.open(QIODevice::ReadOnly);
      if ( playlistFile.isOpen() )
      {
         ui->playlist->clear();
         
         while ( !(fileToPlay = playlistFile.readLine()).isEmpty() )
         {
            fileFullList.append(QDir::fromNativeSeparators(fileToPlay));
         }
         fileFullList.replaceInStrings(QRegExp("[\r\n]"),"");         
         fileFullList.removeDuplicates();
         
         settings.setValue("Playlist",fileFullList);
         populateTreeFromINI();
         
         QString title;
         title = "Playlist Editor: [*]";
         title += settings.value("PlaylistFile").toString();
         setWindowTitle(title);

         playlistFile.close();
         setWindowModified(false);
      }
      else
      {
         QMessageBox::critical(this,"Error!","Could not open playlist file!");
      }
   }
}

void PlaylistEditorDialog::populateTreeFromINI()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList playlist = settings.value("Playlist").toStringList();
   QStringList folderList;
   QList<QTreeWidgetItem*> twis;

   setWindowTitle("Playlist Editor: [*]No playlist loaded. (Changes will affect INI)");
   
   ui->playlist->clear();
   
   foreach ( QString file, playlist )
   {
      QFileInfo fileInfo(file);
      folderList.append(fileInfo.path());
   }
   folderList.removeDuplicates();

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

   foreach ( QString file, playlist )
   {
      QFileInfo fileInfo(file);
      twis = ui->playlist->findItems(fileInfo.path(),Qt::MatchExactly);
      if ( twis.count() == 1 )
      {
         QTreeWidgetItem* twic = new QTreeWidgetItem(QStringList(fileInfo.fileName()));
         twic->setFlags(twic->flags()|Qt::ItemIsUserCheckable);
         twic->setCheckState(0,Qt::Checked);
         twic->setData(0,Qt::UserRole,fileInfo.path());
         if ( !fileInfo.exists() )
         {
            twic->setDisabled(true);
         }
         twis.at(0)->addChild(twic);
      }
   }
   setWindowModified(false);
}

void PlaylistEditorDialog::on_rescan_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QTreeWidgetItemIterator twi(ui->playlist);
   QStringList playlist = settings.value("Playlist").toStringList();
   QDir dir;
   QString file;
   QStringList files;
   QFileInfoList fileInfos;
   
   while ( *twi )
   {
      if ( !(*twi)->parent() )
      {
         // root == folder
         dir.setPath((*twi)->text(0));
         fileInfos = dir.entryInfoList(QStringList("*.ftm"));
         
         foreach ( QFileInfo fileInfo, fileInfos )
         {
            playlist.append(fileInfo.filePath());
         }
         playlist.removeDuplicates();
      }
      ++twi;
   }
   
   settings.setValue("Playlist",playlist);
   populateTreeFromINI();

   // CP: Fix window title from call to populateTreeFromINI if a file is actually loaded.   
   if ( !settings.value("PlaylistFile").toString().isEmpty() )
   {
      QString title;
      title = "Playlist Editor: [*]";
      title += settings.value("PlaylistFile").toString();
      setWindowTitle(title);
   }
   setWindowModified(true);
}

QStringList PlaylistEditorDialog::playlist()
{
   return dumpTreeToList();
}

void PlaylistEditorDialog::on_ok_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   settings.setValue("Playlist",dumpTreeToList());
   if ( isWindowModified() && !settings.value("PlaylistFile").toString().isEmpty() )
   {
      int result = QMessageBox::warning(this,"Save playlist?",
                           "You have made changes to the loaded playlist file.  Do you "
                           "want to save them?",QMessageBox::Yes,QMessageBox::No);
      if ( result == QMessageBox::Yes )
      {
         dumpTreeToFile(settings.value("PlaylistFile").toString());
      }
   }
   accept();
}

void PlaylistEditorDialog::on_useINI_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   if ( isWindowModified() && !settings.value("PlaylistFile").toString().isEmpty() )
   {
      int result = QMessageBox::warning(this,"Save playlist?",
                           "You have made changes to the loaded playlist file.  Do you "
                           "want to save them?",QMessageBox::Yes,QMessageBox::No);
      if ( result == QMessageBox::Yes )
      {
         dumpTreeToFile(settings.value("PlaylistFile").toString());
      }
   }
   settings.remove("PlaylistFile");
   accept();
}

void PlaylistEditorDialog::on_addPath_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiPlayer");
   QStringList playlist = settings.value("Playlist").toStringList();
   QString lastFolder = settings.value("LastFolder").toString();   

   lastFolder = QFileDialog::getExistingDirectory(this,"Select a folder of FTMs...",lastFolder,0);
   if ( !lastFolder.isEmpty() )
   {
      settings.setValue("LastFolder",lastFolder);
      
      // CP: Add item to tree view.
      QTreeWidgetItem* twi = new QTreeWidgetItem(QStringList(lastFolder));
      twi->setFlags(twi->flags()|Qt::ItemIsUserCheckable);
      twi->setCheckState(0,Qt::Checked);
      ui->playlist->addTopLevelItem(twi);
      
      QDir dir(lastFolder);
      QFileInfoList fileInfos = dir.entryInfoList(QStringList("*.ftm"));
      foreach ( QFileInfo fileInfo, fileInfos )
      {
         QTreeWidgetItem* twic = new QTreeWidgetItem(QStringList(fileInfo.fileName()));
         twic->setFlags(twic->flags()|Qt::ItemIsUserCheckable);
         twic->setCheckState(0,Qt::Checked);
         twic->setData(0,Qt::UserRole,lastFolder);
         twi->addChild(twic);
      }
      
      settings.setValue("Playlist",dumpTreeToList());
      setWindowModified(true);
   }
}
