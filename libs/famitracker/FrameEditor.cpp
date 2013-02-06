#include "FrameEditor.h"
#include "ui_FrameEditor.h"

#include <QScrollBar>

CFrameEditor::CFrameEditor(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::CFrameEditor)
{
   ui->setupUi(this);
   
   m_pDocument = NULL;
}

CFrameEditor::~CFrameEditor()
{
   delete ui;
   delete framesModel;
}

void CFrameEditor::AssignDocument(CFamiTrackerDoc *pDoc)
{
   int width;
   int col;

   m_pDocument = pDoc;
   
   QObject::connect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
   
   framesModel = new CMusicFamiTrackerFramesModel(m_pDocument);

   ui->songFrames->setModel(framesModel);
   
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
   
   ui->songFrames->verticalHeader()->setResizeMode(QHeaderView::Fixed);
   ui->songFrames->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

   width = ui->songFrames->verticalHeader()->width()+2;
   for ( col = 0; col < m_pDocument->GetChannelCount(); col++ )
   {
      width += ui->songFrames->columnWidth(col);
   }
   width += ui->songFrames->verticalScrollBar()->width();
   ui->songFrames->setFixedWidth(width);
   ui->songFrames->setStyleSheet("QTableView { background: #000000; color: #ffffff; }");

   ui->songFrames->verticalScrollBar()->setPageStep(2);
   
   QObject::connect(ui->songFrames->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(songFrames_scrolled(int)));
   QObject::connect(framesModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(framesModel_dataChanged(QModelIndex,QModelIndex)));
   QObject::connect(ui->songFrames->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(songFrames_currentChanged(QModelIndex,QModelIndex)));
}

void CFrameEditor::updateViews(long hint)
{
   framesModel->update();
}

void CFrameEditor::framesModel_dataChanged(QModelIndex topLeft,QModelIndex bottomRight)
{
//   int frame = topLeft.row();
//   int channel = topLeft.column();
//   int idx;

//   if ( ui->frameChangeAll->isChecked() )
//   {
//      for ( idx = 0; idx < m_pDocument->GetChannelCount(); idx++ )
//      {
//         if ( idx != channel )
//         {
//            m_pDocument->SetPatternAtFrame(frame,idx,topLeft.data().toInt());
//         }
//      }
//   }

//   framesModel->update();
}

void CFrameEditor::songFrames_scrolled(int value)
{
//   ui->songFrames->setCurrentIndex(framesModel->index(value,ui->songFrames->currentIndex().column()));
}

void CFrameEditor::songFrames_currentChanged(QModelIndex index, QModelIndex)
{
   int frame = ui->songFrames->currentIndex().row();
   ui->songFrames->scrollTo(index,QAbstractItemView::PositionAtCenter);
   emit selectFrame(frame);
}

void CFrameEditor::on_songFrames_pressed(const QModelIndex &index)
{
   ui->songFrames->setCurrentIndex(index);
}

int CFrameEditor::GetSelectedFrame()
{
   return ui->songFrames->currentIndex().row();
}

int CFrameEditor::GetSelectedChannel()
{
   return ui->songFrames->currentIndex().column();
}

void CFrameEditor::SelectFrame(unsigned int Frame)
{
   ui->songFrames->selectRow(Frame);
}

void CFrameEditor::SelectChannel(unsigned int Channel)
{
}
