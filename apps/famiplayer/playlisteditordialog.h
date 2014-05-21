#ifndef PLAYLISTEDITORDIALOG_H
#define PLAYLISTEDITORDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class PlaylistEditorDialog;
}

class PlaylistEditorDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit PlaylistEditorDialog(QWidget *parent = 0);
   ~PlaylistEditorDialog();
   QStringList playlist();
   
protected:
   void populateTreeFromFile();
   void populateTreeFromINI();
   QStringList dumpTreeToList();
   void dumpTreeToFile(QString playlistFileName);
   
private slots:
   void on_playlist_itemChanged(QTreeWidgetItem *item, int column);
   
   void on_load_clicked();
   void on_save_clicked();
   void on_cancel_clicked();
   void on_rescan_clicked();
   void on_ok_clicked();
   void on_useINI_clicked();   
   void on_addPath_clicked();
   
private:
   Ui::PlaylistEditorDialog *ui;
};

#endif // PLAYLISTEDITORDIALOG_H
