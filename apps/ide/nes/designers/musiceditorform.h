#ifndef MUSICEDITORFORM_H
#define MUSICEDITORFORM_H

#include "cdesignereditorbase.h"
#include "cdesignercommon.h"
#include "cmusicfamitrackerframesmodel.h"
#include "cmusicfamitrackerpatternsmodel.h"
#include "cmusicfamitrackerinstrumentsmodel.h"
#include "cdebuggernumericitemdelegate.h"
#include "cmusicfamitrackerdata.h"

#include <QWidget>
#include <QByteArray>
#include <QString>

namespace Ui {
class MusicEditorForm;
}

class MusicEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~MusicEditorForm();

   QByteArray musicData();
   void setMusicData(QByteArray data);

   // ICenterWidgetItem interface

private:
   Ui::MusicEditorForm *ui;
   CMusicFamiTrackerData *data;
   CMusicFamiTrackerFramesModel *framesModel;
   CMusicFamiTrackerPatternsModel *patternsModel;
   CMusicFamiTrackerInstrumentsModel *instrumentsModel;
   CDebuggerNumericItemDelegate *entryDelegate;
   CFamiTrackerDoc* pDoc;

private slots:
   void updateTargetMachine(QString /*target*/) {}
   void on_frameInc_clicked();
   void on_frameDec_clicked();
   void on_speed_valueChanged(int arg1);
   void on_tempo_valueChanged(int arg1);
   void on_numRows_valueChanged(int arg1);
   void on_numFrames_valueChanged(int arg1);
   void on_songs_currentIndexChanged(int index);
   void on_songFrames_clicked(const QModelIndex &index);
   void on_songFrames_activated(const QModelIndex &index);
   void on_songFrames_entered(const QModelIndex &index);
   void on_songFrames_pressed(const QModelIndex &index);
   void framesModel_dataChanged(QModelIndex topLeft,QModelIndex bottomRight);
   void songFrames_scrolled(int value);
};

#endif // MUSICEDITORFORM_H
