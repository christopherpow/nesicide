#include "musiceditorform.h"
#include "ui_musiceditorform.h"

#include <QScrollBar>

MusicEditorForm::MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   int width;
   int col;

   ui->setupUi(this);

   data = new CMusicFamiTrackerData();
   // Save us a bit of typing...
   pDoc = data->GetDocument();

   // Connect buried signals.
   QObject::connect(pDoc,SIGNAL(setModified(bool)),this,SIGNAL(editor_modified(bool)));

   framesModel = new CMusicFamiTrackerFramesModel(data->GetDocument());
   entryDelegate = new CDebuggerNumericItemDelegate();

   patternsModel = new CMusicFamiTrackerPatternsModel(data->GetDocument());

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
   for ( col = 0; col < 5; col++ )
   {
      width += ui->songFrames->columnWidth(col);
   }
   width += ui->songFrames->verticalScrollBar()->width();
   ui->songFrames->setFixedWidth(width);
   ui->songFrames->setStyleSheet("QTableView { background: #000000; color: #ffffff }");

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

   ui->songPatterns->setStyleSheet("QTableView { background: #000000; color: #ffffff }");

   ui->songs->addItem(data->GetDocument()->GetTrackTitle(0));

   setMusicData(musicData);
}

MusicEditorForm::~MusicEditorForm()
{
   delete ui;
   delete framesModel;
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
   pDoc->OnOpenDocument((LPCTSTR)"HELLLAAH");
}

void MusicEditorForm::on_frameInc_clicked()
{
}

void MusicEditorForm::on_frameDec_clicked()
{
}

void MusicEditorForm::on_speed_valueChanged(int arg1)
{

}

void MusicEditorForm::on_tempo_valueChanged(int arg1)
{

}

void MusicEditorForm::on_numRows_valueChanged(int arg1)
{
   data->GetDocument()->SetPatternLength(arg1);
   patternsModel->update();
   ui->songPatterns->resizeRowsToContents();
}

void MusicEditorForm::on_numFrames_valueChanged(int arg1)
{
   data->GetDocument()->SetFrameCount(arg1);
   framesModel->update();
   ui->songFrames->resizeRowsToContents();
}

void MusicEditorForm::on_songs_currentIndexChanged(int index)
{
   data->GetDocument()->SelectTrack(index);
   framesModel->update();
   ui->songFrames->resizeRowsToContents();
   patternsModel->update();
   ui->songPatterns->resizeRowsToContents();
}
