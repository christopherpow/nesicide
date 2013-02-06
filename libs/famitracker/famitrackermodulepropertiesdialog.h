#ifndef FAMITRACKERMODULEPROPERTIESDIALOG_H
#define FAMITRACKERMODULEPROPERTIESDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStringListModel>

#include "FamiTrackerDoc.h"

namespace Ui {
class FamiTrackerModulePropertiesDialog;
}

class FamiTrackerModulePropertiesDialog : public QDialog
{
   Q_OBJECT

public:
   explicit FamiTrackerModulePropertiesDialog(CFamiTrackerDoc* pDoc, QWidget *parent = 0);
   ~FamiTrackerModulePropertiesDialog();
   
   QStringList tracks();
   
private slots:
   void on_addSong_clicked();
   void on_removeSong_clicked();
   void on_moveSongUp_clicked();
   void on_moveSongDown_clicked();
   void on_importFile_clicked();
   void on_expansionSound_currentIndexChanged(int index);

   void on_tracks_clicked(const QModelIndex &index);
   
   void on_buttonBox_accepted();
   
   void on_channelsN163_valueChanged(int value);
   
private:
   Ui::FamiTrackerModulePropertiesDialog *ui;
   CFamiTrackerDoc* m_pDocument;
   QStringListModel* tracksModel;
   
   void updateButtons();
};

#endif // FAMITRACKERMODULEPROPERTIESDIALOG_H
