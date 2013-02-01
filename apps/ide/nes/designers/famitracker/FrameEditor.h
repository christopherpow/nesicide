#ifndef FRAMEEDITOR_H
#define FRAMEEDITOR_H

#include <QWidget>

#include "cmusicfamitrackerframesmodel.h"
#include "cdebuggernumericitemdelegate.h"

#include "famitracker/FamiTrackerDoc.h"

namespace Ui {
class CFrameEditor;
}

class CFrameEditor : public QWidget
{
   Q_OBJECT
   
public:
   explicit CFrameEditor(QWidget *parent = 0);
   ~CFrameEditor();
   
   void AssignDocument(CFamiTrackerDoc* pDoc);
   CFamiTrackerDoc* GetDocument() { return m_pDocument; }
   
   int GetSelectedFrame();
   int GetSelectedChannel();
   
   void SelectFrame(unsigned int Frame);
   void SelectChannel(unsigned int Channel);
   
private:
   Ui::CFrameEditor *ui;
   CMusicFamiTrackerFramesModel *framesModel;
   CDebuggerNumericItemDelegate *entryDelegate;
   CFamiTrackerDoc* m_pDocument;
   
public slots:
   void updateViews(long hint);
   void framesModel_dataChanged(QModelIndex topLeft,QModelIndex bottomRight);
   void songFrames_scrolled(int value);   
   void songFrames_currentChanged(QModelIndex index,QModelIndex);
   
signals:
   void selectFrame(unsigned int Frame);
private slots:
   void on_songFrames_pressed(const QModelIndex &index);
};

#endif // FRAMEEDITOR_H
