#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include <QWidget>
#include <QTableView>
#include <QPaintEvent>

#include "cmusicfamitrackerpatternsmodel.h"
#include "cmusicfamitrackerpatternitemdelegate.h"

#include "famitracker/FamiTrackerDoc.h"

enum {TRANSPOSE_DEC_NOTES, TRANSPOSE_INC_NOTES, TRANSPOSE_DEC_OCTAVES, TRANSPOSE_INC_OCTAVES};

namespace Ui {
class CPatternEditor;
}

class CPatternEditor : public QWidget
{
   Q_OBJECT
   
public:
   explicit CPatternEditor(QWidget *parent = 0);
   ~CPatternEditor();
   
   void AssignDocument(CFamiTrackerDoc* pDoc);
   CFamiTrackerDoc* GetDocument() { return m_pDocument; }
   
   int GetFrame() const;
	int GetChannel() const;
	int GetRow() const;
	int GetColumn() const;
	int GetPlayFrame() const;
	int GetPlayRow() const;

   CSelection GetSelection() const;
	void SetSelection(CSelection &selection);
   bool IsSelecting() const;

   void DragPaste(stClipData *pClipData, CSelection *pDragTarget, bool bMix);
   
   void ExpandPattern();
	void ShrinkPattern();

   void MoveToRow(int Row);
	void MoveToFrame(int Frame);
	void MoveToChannel(int Channel);
	void MoveToColumn(int Column);
	void NextFrame();
	void PreviousFrame();

   void Interpolate();
	void Reverse();
	void ReplaceInstrument(int Instrument);

   bool IsColumnSelected(int Column, int Channel) const;
	int  GetSelectColumn(int Column) const;

   // Various
	void Transpose(int Type);
	void ScrollValues(int Type);

   // Edit: Copy & paste, selection
	void CopyEntire(stClipData *pClipData);
	void Copy(stClipData *pClipData);
	void Cut();
	void PasteEntire(stClipData *pClipData);
	void Paste(stClipData *pClipData);
	void PasteMix(stClipData *pClipData);
	void DeleteSelectionRows(CSelection &selection);
	void DeleteSelection(CSelection &selection);
	void Delete();
	void RemoveSelectedNotes();

   // Other
	int GetCurrentPatternLength(int Frame) const;

private:
   Ui::CPatternEditor *ui;
   CMusicFamiTrackerPatternsModel *patternsModel;
   CMusicFamiTrackerPatternItemDelegate *patternDelegate;
   CFamiTrackerDoc* m_pDocument;
   CSelection m_selection;
   bool m_bSelecting;

   // Edit cursor
	CCursorPos m_cpCursorPos;			// Cursor position

   int	m_iCurrentFrame;
	int m_iPatternLength;
	int	m_iPrevPatternLength;
	int	m_iNextPatternLength;
	int m_iPlayPatternLength;
   
   // Play cursor
	int m_iPlayRow;
	int m_iPlayFrame;
	bool m_bForcePlayRowUpdate;

	bool m_bFollowMode;					// Follow mode enable/disable

   bool m_bForceFullRedraw;
   
public slots:
   void updateViews(long hint);
   void songPatterns_actionTriggered(int action);   
   void songPatterns_currentChanged(QModelIndex index,QModelIndex);
private slots:
   void on_songPatterns_pressed(const QModelIndex &index);
};

#endif // PATTERNEDITOR_H
