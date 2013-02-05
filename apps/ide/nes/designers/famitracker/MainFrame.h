#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QToolBar>
#include <QLabel>
#include <QComboBox>

#include "famitracker/FamiTrackerDoc.h"
#include "famitracker/Action.h"
#include "famitracker/FrameEditor.h"
#include "famitracker/PatternEditor.h"

#include "cmusicfamitrackerinstrumentsmodel.h"

#include <QTableView>

namespace Ui {
class CMainFrame;
}

class CFamiTrackerFrameTableView : public QTableView
{
public:
   CFamiTrackerFrameTableView(QWidget* parent = 0);
   
   void AssignDocument(CFamiTrackerDoc* pDoc) { m_pDocument = pDoc; }
   
   void setModel(QAbstractItemModel *model);
   
protected:
   void resizeEvent(QResizeEvent *event);
   
private:
   CFamiTrackerDoc* m_pDocument;
};

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
   
   bool		 GetEditMode() const { return m_bEditEnable; };
   
   void UpdateControls() { qDebug("UpdateControls"); }

   // General
	void		 SoloChannel(unsigned int Channel);
	void		 ToggleChannel(unsigned int Channel);
	void		 UnmuteAllChannels();
	bool		 IsChannelSolo(unsigned int Channel);
	bool		 IsChannelMuted(unsigned int Channel);
   
   // Note handling
	void	PlayNote(unsigned int Channel, unsigned int Note, unsigned int Octave, unsigned int Velocity);
	void	ReleaseNote(unsigned int Channel);
	void	HaltNote(unsigned int Channel);

   void UpdateEditor(UINT lHint);
   
protected:
   void showEvent(QShowEvent *);
   void hideEvent(QHideEvent *);
   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
   
private:
   Ui::CMainFrame *ui;
   QToolBar* toolBar;
   QLabel* octaveLabel;
   QComboBox* octaveComboBox;
   CActionHandler* m_pActionHandler;
   QMap<QAction*,actionHandler> trackerActions;
   CMusicFamiTrackerInstrumentsModel *instrumentsModel;
   CFamiTrackerDoc* m_pDocument;
   QString m_fileName;

   // Cursor & editing
	unsigned int		m_iMoveKeyStepping;						// Number of rows to jump when moving
	unsigned int		m_iInsertKeyStepping;					// Number of rows to move when inserting notes
   unsigned int		m_iInstrument;							// Selected instrument
	unsigned int		m_iOctave;								// Selected octave	
   
   bool				m_bEditEnable;							// Edit is enabled
	bool				m_bSwitchToInstrument;
	bool				m_bFollowMode;							// Follow mode, default true
	bool				m_bShiftPressed, m_bControlPressed;		// Shift and control keys
	bool				m_bChangeAllPattern;					// All pattern will change
	bool				m_bMaskInstrument;
	int					m_iPasteMode;
   
   void		 FeedNote(int Channel, stChanNote *NoteData);
   unsigned int GetInstrument() const;
   
   // Playing
	bool				m_bMuteChannels[MAX_CHANNELS];
	unsigned int		m_iPlayTime;
	int					m_iFrameQueue;
	int					m_iSwitchToInstrument;

	// Auto arpeggio
	char				m_iAutoArpNotes[128];
	int					m_iAutoArpPtr, m_iLastAutoArpPtr;
	int					m_iAutoArpKeyCount;

   // Drawing
	bool				m_bForceRedraw;
	bool				m_bUpdateBackground;

   int					m_iMenuChannel;
   
   // Input
	char				m_cKeyList[256];
	unsigned int		m_iKeyboardNote;
	int					m_iLastNote;
	int					m_iLastInstrument, m_iLastVolume;
	int					m_iLastEffect, m_iLastEffectParam;

   // Input handling
	void	KeyIncreaseAction();
	void	KeyDecreaseAction();
	
   void	OnKeyDelete();
	void	OnKeyInsert();
	void	OnKeyBack();
	void	OnKeyHome();
	void	OnKeyEnd();
   
   void RemoveWithoutDelete();
   void StepDown();
   
   void OnEditDelete();
   
   void SetStepping(int Step); 
   
   void		 SetOctave(unsigned int iOctave);

   bool AddAction(CAction *pAction);
   CAction *GetLastAction() const;
   
   void SetRowCount(int Count);
   void SetFrameCount(int Count);
   
   bool	PreventRepeat(unsigned char Key, bool Insert);
	void	RepeatRelease(unsigned char Key);
   
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
private slots:
   void on_editStep_valueChanged(int arg1);
};

#endif // MAINFRAME_H
