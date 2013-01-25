#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QToolBar>

#include "famitracker/FamiTrackerDoc.h"
#include "famitracker/Action.h"
#include "famitracker/FrameEditor.h"
#include "famitracker/PatternEditor.h"

#include "cmusicfamitrackerpatternsmodel.h"
#include "cmusicfamitrackerinstrumentsmodel.h"

namespace Ui {
class CMainFrame;
}

enum {PASTE_MODE_NORMAL, PASTE_MODE_OVERWRITE, PASTE_MODE_MIX};

class CMainFrame;
typedef void (CMainFrame::*actionHandler)();

#define m_pPatternView ui->songPatterns
#define m_pFrameView ui->songFrames

class CMainFrame : public QWidget
{
   Q_OBJECT
   
public:
   explicit CMainFrame(QString fileName,QWidget *parent = 0);
   ~CMainFrame();
   
   CFrameEditor* GetFrameEditor();
   CPatternEditor* GetPatternEditor();
   CFamiTrackerDoc* GetActiveDocument() { return m_pDocument; }
   CFamiTrackerDoc* GetDocument() { return m_pDocument; }
   
   void UpdateControls() { qDebug("UpdateControls"); }
   void UpdateEditor(UINT lHint);
   
protected:
   void showEvent(QShowEvent *);
   void hideEvent(QHideEvent *);
   
private:
   Ui::CMainFrame *ui;
   QToolBar* toolBar;
   CActionHandler* m_pActionHandler;
   QMap<QAction*,actionHandler> trackerActions;
   CMusicFamiTrackerInstrumentsModel *instrumentsModel;
   CFamiTrackerDoc* m_pDocument;
   QString m_fileName;
   
   bool AddAction(CAction *pAction);
   CAction *GetLastAction() const;
   
   void SetRowCount(int Count);
   void SetFrameCount(int Count);
   
   void trackerAction_newDocument();
   void trackerAction_openDocument();
   void trackerAction_saveDocument();
   void trackerAction_editCut();
   void trackerAction_editCopy();
   void trackerAction_editPaste();
   void trackerAction_about();
   void trackerAction_help();
   void trackerAction_addFrame();
   void trackerAction_removeFrame();
   void trackerAction_moveFrameDown();
   void trackerAction_moveFrameUp();
   void trackerAction_duplicateFrame();
   void trackerAction_moduleProperties();
   void trackerAction_play();
   void trackerAction_playLoop();
   void trackerAction_stop();
   void trackerAction_editMode();
   void trackerAction_previousTrack();
   void trackerAction_nextTrack();
   void trackerAction_settings();
   void trackerAction_createNSF();
   
public slots:
   void frameEditor_selectFrame(unsigned int Frame);
   void trackerAction_triggered();
   void updateViews(long hint);
   void on_title_textEdited(const QString &arg1);
   void on_author_textEdited(const QString &arg1);
   void on_copyright_textEdited(const QString &arg1);
   void on_frameInc_clicked();
   void on_frameDec_clicked();
   void on_speed_valueChanged(int arg1);
   void on_tempo_valueChanged(int arg1);
   void on_numRows_valueChanged(int arg1);
   void on_numFrames_valueChanged(int arg1);
   void on_songs_currentIndexChanged(int index);
   
signals:
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void editor_modificationChanged(bool m);
};

#endif // MAINFRAME_H
