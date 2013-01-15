#include "musiceditorform.h"
#include "ui_musiceditorform.h"

#include <QScrollBar>

MusicEditorForm::MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   int width;
   int col;
   int song;

   ui->setupUi(this);

   data = new CMusicFamiTrackerData();
   // Save us a bit of typing...
   pDoc = data->GetDocument();

   // Connect buried signals.
   QObject::connect(pDoc,SIGNAL(setModified(bool)),this,SIGNAL(editor_modified(bool)));

   pDoc->OnOpenDocument((TCHAR*)fileName.toAscii().constData());

   framesModel = new CMusicFamiTrackerFramesModel(pDoc);
   entryDelegate = new CDebuggerNumericItemDelegate();

   patternsModel = new CMusicFamiTrackerPatternsModel(pDoc);

   instrumentsModel = new CMusicFamiTrackerInstrumentsModel(pDoc);

   on_songs_currentIndexChanged(0);

   ui->songFrames->setModel(framesModel);
   ui->songFrames->setItemDelegate(entryDelegate);

#ifdef Q_WS_MAC
   ui->songFrames->setFont(QFont("Monaco",9));
#endif
#ifdef Q_WS_X11
   ui->songFrames->setFont(QFont("Monospace",8));
#endif
#ifdef Q_WS_WIN
   ui->songFrames->setFont(QFont("Consolas",9));
#endif

   ui->songFrames->resizeColumnsToContents();
   ui->songFrames->resizeRowsToContents();

   width = ui->songFrames->verticalHeader()->width()+2;
   for ( col = 0; col < pDoc->GetChannelCount(); col++ )
   {
      width += ui->songFrames->columnWidth(col);
   }
   width += ui->songFrames->verticalScrollBar()->width();
   ui->songFrames->setFixedWidth(width);
   ui->songFrames->setStyleSheet("QTableView { background: #000000; color: #ffffff; }");

   ui->songFrames->verticalScrollBar()->setPageStep(1);

   ui->songPatterns->setModel(patternsModel);

#ifdef Q_WS_MAC
   ui->songPatterns->setFont(QFont("Monaco",9));
#endif
#ifdef Q_WS_X11
   ui->songPatterns->setFont(QFont("Monospace",8));
#endif
#ifdef Q_WS_WIN
   ui->songPatterns->setFont(QFont("Consolas",9));
#endif

   ui->songPatterns->resizeColumnsToContents();
   ui->songPatterns->resizeRowsToContents();

   ui->songPatterns->setStyleSheet("QTableView { background: #000000; color: #ffffff; }");

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

   for ( song = 0; song < pDoc->GetTrackCount(); song++ )
   {
      ui->songs->addItem(pDoc->GetTrackTitle(song));
   }

   ui->title->setText(pDoc->GetSongName());
   ui->author->setText(pDoc->GetSongArtist());
   ui->copyright->setText(pDoc->GetSongCopyright());
   ui->tempo->setValue(pDoc->GetSongTempo());
   ui->speed->setValue(pDoc->GetSongSpeed());
   ui->numRows->setValue(pDoc->GetPatternLength());
   ui->numFrames->setValue(pDoc->GetFrameCount());

   QObject::connect(ui->songFrames->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(songFrames_scrolled(int)));
   QObject::connect(framesModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(framesModel_dataChanged(QModelIndex,QModelIndex)));
}

MusicEditorForm::~MusicEditorForm()
{
   delete ui;
   delete framesModel;
   delete patternsModel;
   delete instrumentsModel;
   delete entryDelegate;
   delete data;
}

QByteArray MusicEditorForm::musicData()
{
   // CPTODO: implement
   qDebug("::musicData");
   return QByteArray();
}

void MusicEditorForm::setMusicData(QByteArray data)
{
}

void MusicEditorForm::on_frameInc_clicked()
{
   QModelIndex index = ui->songFrames->currentIndex();

   if ( index.isValid() )
   {
      int frame = index.row();
      int channel;

      if ( ui->frameChangeAll->isChecked() )
      {
         for ( channel = 0; channel < pDoc->GetChannelCount(); channel++ )
         {
            pDoc->SetPatternAtFrame(frame,channel,pDoc->GetPatternAtFrame(frame,channel)+1);
         }
      }
      else
      {
         channel = index.column();
         pDoc->SetPatternAtFrame(frame,channel,pDoc->GetPatternAtFrame(frame,channel)+1);
      }
   }
   framesModel->update();
   patternsModel->update();
}

void MusicEditorForm::on_frameDec_clicked()
{
   QModelIndex index = ui->songFrames->currentIndex();

   if ( index.isValid() )
   {
      int frame = index.row();
      int channel;

      if ( ui->frameChangeAll->isChecked() )
      {
         for ( channel = 0; channel < pDoc->GetChannelCount(); channel++ )
         {
            pDoc->SetPatternAtFrame(frame,channel,pDoc->GetPatternAtFrame(frame,channel)-1);
         }
      }
      else
      {
         channel = index.column();
         pDoc->SetPatternAtFrame(frame,channel,pDoc->GetPatternAtFrame(frame,channel)-1);
      }
   }
   framesModel->update();
   patternsModel->update();
}

void MusicEditorForm::on_speed_valueChanged(int arg1)
{

}

void MusicEditorForm::on_tempo_valueChanged(int arg1)
{

}

void MusicEditorForm::on_numRows_valueChanged(int arg1)
{
   pDoc->SetPatternLength(arg1);
   patternsModel->update();
   ui->songPatterns->resizeRowsToContents();
}

void MusicEditorForm::on_numFrames_valueChanged(int arg1)
{
   pDoc->SetFrameCount(arg1);
   framesModel->update();
   ui->songFrames->resizeRowsToContents();
}

void MusicEditorForm::on_songs_currentIndexChanged(int index)
{
   pDoc->SelectTrack(index);
   framesModel->update();
   ui->songFrames->resizeRowsToContents();
   patternsModel->update();
   ui->songPatterns->resizeRowsToContents();
   instrumentsModel->update();
}

void MusicEditorForm::on_songFrames_clicked(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      patternsModel->setFrame(index.row());
      patternsModel->update();
   }
}

void MusicEditorForm::on_songFrames_activated(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      patternsModel->setFrame(index.row());
      patternsModel->update();
   }
}

void MusicEditorForm::on_songFrames_entered(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      patternsModel->setFrame(index.row());
      patternsModel->update();
   }
}

void MusicEditorForm::on_songFrames_pressed(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      patternsModel->setFrame(index.row());
      patternsModel->update();
   }
}

void MusicEditorForm::framesModel_dataChanged(QModelIndex topLeft,QModelIndex bottomRight)
{
   int frame = topLeft.row();
   int channel = topLeft.column();
   int idx;

   if ( ui->frameChangeAll->isChecked() )
   {
      for ( idx = 0; idx < pDoc->GetChannelCount(); idx++ )
      {
         if ( idx != channel )
         {
            pDoc->SetPatternAtFrame(frame,idx,topLeft.data().toInt());
         }
      }
   }

   framesModel->update();
   patternsModel->update();
}

void MusicEditorForm::songFrames_scrolled(int value)
{
   ui->songFrames->setCurrentIndex(framesModel->index(value,ui->songFrames->currentIndex().column()));
   patternsModel->setFrame(value);
   patternsModel->update();
}
