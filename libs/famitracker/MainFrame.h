#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QFrame>
#include <QToolBar>
#include <QLabel>
#include <QComboBox>

#include "FamiTrackerView.h"
#include "FamiTrackerDoc.h"
#include "FrameEditor.h"
#include "PatternEditor.h"
#include "Action.h"

#include "cmusicfamitrackerinstrumentsmodel.h"

#include <QTableView>

namespace Ui {
class CMainFrame;
}

class CMainFrame;
typedef void (CMainFrame::*actionHandler)();

enum FRAME_EDIT_POS { FRAME_EDIT_POS_TOP, FRAME_EDIT_POS_LEFT };

class CMainFrame : public CFrameWnd
{
   Q_OBJECT

public:
   CMainFrame(QWidget* parent = 0);
   ~CMainFrame();
   
   // Attributes
public:
   CFrameEditor *GetFrameEditor() const;
   void setFileName(QString fileName);
      
   // CFrameWnd stuff
   virtual CView* GetActiveView();
   virtual CFamiTrackerDoc* GetDocument() { return m_pDocument; }
   virtual CFamiTrackerDoc* GetActiveDocument() { return m_pDocument; }
   
   // Operations
   public:
   
   void	SetStatusText(LPCTSTR Text,...);
   void	ChangeNoteState(int Note);
   
   void	UpdateTrackBox();
   void	ChangedTrack();
   void	ResizeFrameWindow();

   void	UpdateControls();
   
   void OnPrevSong();
   void OnNextSong();

   // Instrument
   void	UpdateInstrumentList();
   void	CloseInstrumentEditor();
   void	SelectInstrument(int Index);
   void	ClearInstrumentList();
   void	AddInstrument(int Index);
   void	RemoveInstrument(int Index);
   void	NewInstrument(int ChipType);

   void	SetIndicatorTime(int Min, int Sec, int MSec);
   void	SetSongInfo(char *Name, char *Artist, char *Copyright);
   void	SetupColors(void);
   void	DisplayOctave();

   void	SetHighlightRow(int Rows);
   void	SetSecondHighlightRow(int Rows);

   int		GetSelectedInstrument() const;
   int		GetSelectedTrack() const;

   bool	AddAction(CAction *pAction);
   CAction *GetLastAction() const;
   void	ResetUndo();

   void	SetRowCount(int Count);
	void	SetFrameCount(int Count);
   
protected:
   void showEvent(QShowEvent *);
   void hideEvent(QHideEvent *);

private:
   // Qt stuff
   Ui::CMainFrame *ui;
   QToolBar* toolBar;
   QLabel* octaveLabel;
   QComboBox* octaveComboBox;
   QMap<QAction*,actionHandler> trackerActions;
   CFamiTrackerDoc* m_pDocument;
   CFamiTrackerView* m_pView;
   CMusicFamiTrackerInstrumentsModel *instrumentsModel;
   QString m_fileName;

   CActionHandler* m_pActionHandler;
   int					m_iFrameEditorPos;
      
private:
	// State variables (to be used)
	int		m_iInstrument;				// Selected instrument
	int		m_iTrack;					// Selected track

   CFrameEditor		*m_pFrameEditor;
   
public:
   afx_msg void OnModuleInsertFrame();
	afx_msg void OnModuleRemoveFrame();
	afx_msg void OnModuleDuplicateFrame();
	afx_msg void OnModuleDuplicateFramePatterns();
	afx_msg void OnModuleChannels();
	afx_msg void OnModuleComments();
	afx_msg void OnModuleModuleproperties();
	afx_msg void OnModuleMoveframedown();
	afx_msg void OnModuleMoveframeup();
   
public slots:
   void on_frameInc_clicked();   
   void on_frameDec_clicked();
   void on_speed_valueChanged(int arg1);
   void on_tempo_valueChanged(int arg1);
   void on_numRows_valueChanged(int NewRows);
   void on_numFrames_valueChanged(int NewFrames);
   void on_songs_currentIndexChanged(int index);
   void on_title_textEdited(const QString &arg1);   
   void on_author_textEdited(const QString &arg1);
   void on_copyright_textEdited(const QString &arg1);
   void trackerAction_triggered();
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
   
signals:
   void addToolBarWidget(QToolBar* toolBar);
   void removeToolBarWidget(QToolBar* toolBar);
   void editor_modificationChanged(bool m);
};

// Global DPI functions
int  SX(int pt);
int  SY(int pt);
void ScaleMouse(CPoint &pt);

#endif // MAINFRAME_H
