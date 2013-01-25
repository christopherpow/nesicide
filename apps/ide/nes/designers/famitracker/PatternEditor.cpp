#include "PatternEditor.h"
#include "ui_PatternEditor.h"

#include "famitracker/Settings.h"
#include "famitracker/SoundGen.h"

#include <QScrollBar>

enum {COLUMN_NOTE, COLUMN_INSTRUMENT, COLUMN_VOLUME, COLUMN_EFF1, COLUMN_EFF2, COLUMN_EFF3, COLUMN_EFF4};
const int VOL_COLUMN_MAX = 0x10;      // TODO: move this

CPatternEditor::CPatternEditor(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::CPatternEditor)
{
   ui->setupUi(this);
   
   m_pDocument = NULL;
}

CPatternEditor::~CPatternEditor()
{
   delete ui;
   delete patternsModel;
}

void CPatternEditor::AssignDocument(CFamiTrackerDoc *pDoc)
{
   m_pDocument = pDoc;
   
   QObject::connect(m_pDocument,SIGNAL(updateViews(long)),this,SLOT(updateViews(long)));
   
   patternsModel = new CMusicFamiTrackerPatternsModel(m_pDocument);

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
   
   ui->songPatterns->verticalHeader()->setResizeMode(QHeaderView::Fixed);
   ui->songPatterns->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

   ui->songPatterns->setStyleSheet("QTableView { background: #000000; color: #ffffff; }");
      
   QObject::connect(ui->songPatterns->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(songPatterns_currentChanged(QModelIndex,QModelIndex)));
   QObject::connect(ui->songPatterns->verticalScrollBar(),SIGNAL(actionTriggered(int)),this,SLOT(songPatterns_actionTriggered(int)));
}

void CPatternEditor::updateViews(long hint)
{
   patternsModel->update();
}

int CPatternEditor::GetFrame() const
{
   return patternsModel->frame();
}

int CPatternEditor::GetChannel() const
{
   qDebug("reimplement GetChannel?");
   return ui->songPatterns->currentIndex().column(); 
//	return m_cpCursorPos.m_iChannel;
}

int CPatternEditor::GetRow() const
{
   qDebug("reimplement GetRow?");
   return ui->songPatterns->currentIndex().row();
//	return m_cpCursorPos.m_iRow;
}

int CPatternEditor::GetColumn() const
{
   qDebug("reimplement GetColumn?");
   return 0;
//	return m_cpCursorPos.m_iColumn;
}

int CPatternEditor::GetPlayFrame() const
{
	return m_iPlayFrame;
}

int CPatternEditor::GetPlayRow() const
{
	return m_iPlayRow;
}

CSelection CPatternEditor::GetSelection() const
{
	return m_selection;
}

void CPatternEditor::SetSelection(CSelection &selection)
{
	m_selection = selection;
	m_bSelecting = true;
}

bool CPatternEditor::IsSelecting() const
{
	return m_bSelecting;
}

void CPatternEditor::Interpolate()
{
	stChanNote NoteData;
	int StartRow = m_selection.GetRowStart();
	int EndRow = m_selection.GetRowEnd();
	float StartVal, EndVal, Delta;
	int i, j, k;
	int Effect;

	if (!m_bSelecting)
		return;

	for (i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); i++) {
		for (k = 0; k < (signed)m_pDocument->GetEffColumns(i) + 2; k++) {
			switch (k) {
				case 0:	// Volume
					if (!IsColumnSelected(COLUMN_VOLUME, i))
						continue;
					m_pDocument->GetNoteData(m_iCurrentFrame, i, StartRow, &NoteData);
					if (NoteData.Vol == 0x10)
						continue;
					StartVal = (float)NoteData.Vol;
					m_pDocument->GetNoteData(m_iCurrentFrame, i, EndRow, &NoteData);
					if (NoteData.Vol == 0x10)
						continue;
					EndVal = (float)NoteData.Vol;
					break;
				case 1:	// Effect 1
				case 2:
				case 3:
				case 4:
					if (!IsColumnSelected(COLUMN_EFF1 + k - 1, i))
						continue;
					m_pDocument->GetNoteData(m_iCurrentFrame, i, StartRow, &NoteData);
					if (NoteData.EffNumber[k - 1] == EF_NONE)
						continue;
					StartVal = (float)NoteData.EffParam[k - 1];
					Effect = NoteData.EffNumber[k - 1];
					m_pDocument->GetNoteData(m_iCurrentFrame, i, EndRow, &NoteData);
					if (NoteData.EffNumber[k - 1] == EF_NONE)
						continue;
					EndVal = (float)NoteData.EffParam[k - 1];
					break;
			}

			Delta = (EndVal - StartVal) / float(EndRow - StartRow);

			for (j = StartRow; j < EndRow; j++) {
				m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);
				switch (k) {
					case 0: 
						NoteData.Vol = (int)StartVal; 
						break;
					case 1: 
					case 2: 
					case 3: 
					case 4:
						NoteData.EffNumber[k - 1] = Effect;
						NoteData.EffParam[k - 1] = (int)StartVal; 
						break;
				}
				StartVal += Delta;
				m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &NoteData);
			}
		}
	}
}

void CPatternEditor::Reverse()
{
	stChanNote ReverseBuffer[MAX_PATTERN_LENGTH];
	stChanNote NoteData;
	int StartRow = m_selection.GetRowStart();
	int EndRow = m_selection.GetRowEnd();
	int i, j, k, m;

	if (!m_bSelecting)
		return;

	for (i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); i++) {
		// Copy the selected rows
		for (j = StartRow, m = 0; j < EndRow + 1; j++, m++) {
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, ReverseBuffer + m);
		}
		// Paste reversed
		for (j = EndRow, m = 0; j > StartRow - 1; j--, m++) {
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);
			if (IsColumnSelected(COLUMN_NOTE, i)) {
				NoteData.Note = ReverseBuffer[m].Note;
				NoteData.Octave = ReverseBuffer[m].Octave;
			}
			if (IsColumnSelected(COLUMN_INSTRUMENT, i))
				NoteData.Instrument = ReverseBuffer[m].Instrument;
			if (IsColumnSelected(COLUMN_VOLUME, i))
				NoteData.Vol = ReverseBuffer[m].Vol;
			for (k = 0; k < 4; k++) {
				if (IsColumnSelected(k + COLUMN_EFF1, i)) {
					NoteData.EffNumber[k] = ReverseBuffer[m].EffNumber[k];
					NoteData.EffParam[k] = ReverseBuffer[m].EffParam[k];
				}
			}
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &NoteData);
		}
	}
}

void CPatternEditor::DragPaste(stClipData *pClipData, CSelection *pDragTarget, bool bMix)
{
	// Paste drag'n'drop selections

	// Set cursor location
	m_cpCursorPos = pDragTarget->m_cpStart;

	if (bMix)
		PasteMix(pClipData);
	else
		Paste(pClipData);

	// Update selection
	m_selection.SetStart(pDragTarget->m_cpStart);
	m_selection.SetEnd(pDragTarget->m_cpEnd);
}

void CPatternEditor::ExpandPattern()
{
	int Rows = m_selection.GetRowEnd() - m_selection.GetRowStart();
	int StartRow = m_selection.GetRowStart();

	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		for (int j = Rows / 2; j >= 0; --j) {
			stChanNote Note;
			m_pDocument->GetNoteData(m_iCurrentFrame, i, StartRow + j, &Note);
			m_pDocument->SetNoteData(m_iCurrentFrame, i, StartRow + j * 2, &Note);
			m_pDocument->ClearRow(m_iCurrentFrame, i, StartRow + j * 2 + 1);
		}
	}
}

void CPatternEditor::ShrinkPattern()
{
	int Rows = m_selection.GetRowEnd() - m_selection.GetRowStart();
	int StartRow = m_selection.GetRowStart();

	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		for (int j = 0; j <= Rows; ++j) {
			if (j <= Rows / 2) {
				stChanNote Note;
				m_pDocument->GetNoteData(m_iCurrentFrame, i, StartRow + j * 2, &Note);
				m_pDocument->SetNoteData(m_iCurrentFrame, i, StartRow + j, &Note);
			}
			else {
				m_pDocument->ClearRow(m_iCurrentFrame, i, StartRow + j);
			}
		}
	}
}

void CPatternEditor::ReplaceInstrument(int Instrument)
{
	stChanNote Note;

	if (!m_bSelecting)
		return;

	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		for (int j = m_selection.GetRowStart(); j <= m_selection.GetRowEnd(); ++j) {
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &Note);
			if (Note.Instrument != MAX_INSTRUMENTS) {
				Note.Instrument = Instrument;
			}
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &Note);
		}
	}
}

void CPatternEditor::Transpose(int Type)
{
	stChanNote Note;

	int RowStart = m_selection.GetRowStart();
	int RowEnd = m_selection.GetRowEnd();
	int ChanStart = m_selection.GetChanStart();
	int ChanEnd = m_selection.GetChanEnd();

	if (!m_bSelecting) {
		RowStart = m_cpCursorPos.m_iRow;
		RowEnd = m_cpCursorPos.m_iRow;
		ChanStart = m_cpCursorPos.m_iChannel;
		ChanEnd = m_cpCursorPos.m_iChannel;
	}

	for (int i = ChanStart; i <= ChanEnd; i++) {
		if (!IsColumnSelected(COLUMN_NOTE, i))
			continue;
		for (int j = RowStart; j <= RowEnd; j++) {
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &Note);
			switch (Type) {
				case TRANSPOSE_DEC_NOTES:
					if (Note.Note > 0 && Note.Note != HALT && Note.Note != RELEASE) {
						if (Note.Note > 1) 
							Note.Note--;
						else {
							if (Note.Octave > 0) {
								Note.Note = B;
								Note.Octave--;
							}
						}
					}
					break;
				case TRANSPOSE_INC_NOTES:
					if (Note.Note > 0 && Note.Note != HALT && Note.Note != RELEASE) {
						if (Note.Note < B)
							Note.Note++;
						else {
							if (Note.Octave < 7) {
								Note.Note = C;
								Note.Octave++;
							}
						}
					}
					break;
				case TRANSPOSE_DEC_OCTAVES:
					if (Note.Octave > 0) 
						Note.Octave--;
					break;
				case TRANSPOSE_INC_OCTAVES:
					if (Note.Octave < 7)
						Note.Octave++;
					break;
			}
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &Note);
		}
	}
}

void CPatternEditor::ScrollValues(int Type)
{
	stChanNote Note;

	int RowStart = m_selection.GetRowStart();
	int RowEnd = m_selection.GetRowEnd();
	int ChanStart = m_selection.GetChanStart();
	int ChanEnd = m_selection.GetChanEnd();
	int i, j, k;

	if (!m_bSelecting) {
		RowStart = m_cpCursorPos.m_iRow;
		RowEnd = m_cpCursorPos.m_iRow;
		ChanStart = m_cpCursorPos.m_iChannel;
		ChanEnd = m_cpCursorPos.m_iChannel;
	}

	for (i = ChanStart; i <= ChanEnd; i++) {
		for (k = 1; k < 7; k++) {
			if (!IsColumnSelected(k, i))
				continue;
			for (j = RowStart; j <= RowEnd; j++) {
				m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &Note);
				switch (k) {
					case COLUMN_INSTRUMENT:
						if (Note.Instrument != MAX_INSTRUMENTS) {
							if ((Type < 0 && Note.Instrument > 0) || (Type > 0 && Note.Instrument < MAX_INSTRUMENTS - 1))
								Note.Instrument += Type;
						}
						break;
					case COLUMN_VOLUME:
						if (Note.Vol != 0x10) {
							if ((Type < 0 && Note.Vol > 0) || (Type > 0 && Note.Vol < 0x0F))
								Note.Vol += Type;
						}
						break;
					case COLUMN_EFF1:
					case COLUMN_EFF2:
					case COLUMN_EFF3:
					case COLUMN_EFF4:
						if (Note.EffNumber[k - COLUMN_EFF1] != EF_NONE) {
							if ((Type < 0 && Note.EffParam[k - COLUMN_EFF1] > 0) || (Type > 0 && Note.EffParam[k - COLUMN_EFF1] < 255))
								Note.EffParam[k - COLUMN_EFF1] += Type;
						}
						break;
				}
				m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &Note);
			}
		}
	}
}

// Copy and paste ///////////////////////////////////////////////////////////////////////////////////////////

void CPatternEditor::CopyEntire(stClipData *pClipData)
{
	pClipData->Channels	= m_pDocument->GetAvailableChannels();
	pClipData->Rows		= m_pDocument->GetPatternLength();

	for (int i = 0; i < pClipData->Channels; ++i) {
		for (int j = 0; j < pClipData->Rows; ++j) {
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &pClipData->Pattern[i][j]);
		}
	}
}

void CPatternEditor::Copy(stClipData *pClipData)
{
	// Copy selection

	int Channel = 0;

	pClipData->Channels		= m_selection.GetChanEnd() - m_selection.GetChanStart() + 1;
	pClipData->StartColumn	= GetSelectColumn(m_selection.GetColStart());
	pClipData->EndColumn	= GetSelectColumn(m_selection.GetColEnd());
	pClipData->Rows			= m_selection.GetRowEnd() - m_selection.GetRowStart() + 1;

	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		if (i < 0 || i >= (signed)m_pDocument->GetAvailableChannels())
			continue;
		
		int Row = 0;

		for (int j = m_selection.GetRowStart(); j <= m_selection.GetRowEnd(); ++j) {
			if (j < 0 || j >= m_iPatternLength)
				continue;

			stChanNote NoteData;
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);

			if (IsColumnSelected(COLUMN_NOTE, i)) {
				pClipData->Pattern[Channel][Row].Note = NoteData.Note;
				pClipData->Pattern[Channel][Row].Octave = NoteData.Octave;
			}
			if (IsColumnSelected(COLUMN_INSTRUMENT, i)) {
				pClipData->Pattern[Channel][Row].Instrument = NoteData.Instrument;
			}
			if (IsColumnSelected(COLUMN_VOLUME, i)) {
				pClipData->Pattern[Channel][Row].Vol = NoteData.Vol;
			}
			if (IsColumnSelected(COLUMN_EFF1, i)) {
				pClipData->Pattern[Channel][Row].EffNumber[0] = NoteData.EffNumber[0];
				pClipData->Pattern[Channel][Row].EffParam[0] = NoteData.EffParam[0];
			}
			if (IsColumnSelected(COLUMN_EFF2, i)) {
				pClipData->Pattern[Channel][Row].EffNumber[1] = NoteData.EffNumber[1];
				pClipData->Pattern[Channel][Row].EffParam[1] = NoteData.EffParam[1];
			}
			if (IsColumnSelected(COLUMN_EFF3, i)) {
				pClipData->Pattern[Channel][Row].EffNumber[2] = NoteData.EffNumber[2];
				pClipData->Pattern[Channel][Row].EffParam[2] = NoteData.EffParam[2];
			}
			if (IsColumnSelected(COLUMN_EFF4, i)) {
				pClipData->Pattern[Channel][Row].EffNumber[3] = NoteData.EffNumber[3];
				pClipData->Pattern[Channel][Row].EffParam[3] = NoteData.EffParam[3];
			}

			++Row;
		}
		++Channel;
	}
}

void CPatternEditor::Cut()
{
	// Cut selection
}

void CPatternEditor::PasteEntire(stClipData *pClipData)
{
	// Paste entire
	int Channels = m_pDocument->GetAvailableChannels();

	for (int i = 0; i < pClipData->Channels; ++i) {
		for (int j = 0; j < pClipData->Rows; ++j) {
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &pClipData->Pattern[i][j]);
		}
	}
}

void CPatternEditor::Paste(stClipData *pClipData)
{
	// Paste
	stChanNote NoteData;
	int Channels = m_pDocument->GetAvailableChannels();

	// Special, single channel and effect columns only
	if (pClipData->Channels == 1 && pClipData->StartColumn >= COLUMN_EFF1) {
		for (int j = 0; j < pClipData->Rows; ++j) {
			if ((j + m_cpCursorPos.m_iRow) < 0 || (j + m_cpCursorPos.m_iRow) >= m_iPatternLength)
				continue;

			m_pDocument->GetNoteData(m_iCurrentFrame, m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);

			for (int i = pClipData->StartColumn - COLUMN_EFF1; i < (pClipData->EndColumn - COLUMN_EFF1 + 1); ++i) {
				int Offset = (GetSelectColumn(m_cpCursorPos.m_iColumn) - COLUMN_EFF1) + (i - (pClipData->StartColumn - COLUMN_EFF1));
				if (Offset < 4 && Offset >= 0) {
					NoteData.EffNumber[Offset] = pClipData->Pattern[0][j].EffNumber[i];
					NoteData.EffParam[Offset] = pClipData->Pattern[0][j].EffParam[i];
				}
			}

			m_pDocument->SetNoteData(m_iCurrentFrame, m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);
		}
		return;
	}	

	for (int i = 0; i < pClipData->Channels; ++i) {
		if ((i + m_cpCursorPos.m_iChannel) < 0 || (i + m_cpCursorPos.m_iChannel) >= Channels)
			continue;

		for (int j = 0; j < pClipData->Rows; ++j) {
			if ((j + m_cpCursorPos.m_iRow) < 0 || (j + m_cpCursorPos.m_iRow) >= m_iPatternLength)
				continue;

			m_pDocument->GetNoteData(m_iCurrentFrame, i + m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);

			if ((i != 0 || pClipData->StartColumn <= COLUMN_NOTE) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_NOTE)) {
				NoteData.Note = pClipData->Pattern[i][j].Note;
				NoteData.Octave = pClipData->Pattern[i][j].Octave;
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_INSTRUMENT) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_INSTRUMENT)) {
				NoteData.Instrument = pClipData->Pattern[i][j].Instrument;
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_VOLUME) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_VOLUME)) {
				NoteData.Vol = pClipData->Pattern[i][j].Vol;
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF1) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF1)) {
				NoteData.EffNumber[0] = pClipData->Pattern[i][j].EffNumber[0];
				NoteData.EffParam[0] = pClipData->Pattern[i][j].EffParam[0];
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF2) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF2)) {
				NoteData.EffNumber[1] = pClipData->Pattern[i][j].EffNumber[1];
				NoteData.EffParam[1] = pClipData->Pattern[i][j].EffParam[1];
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF3) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF3)) {
				NoteData.EffNumber[2] = pClipData->Pattern[i][j].EffNumber[2];
				NoteData.EffParam[2] = pClipData->Pattern[i][j].EffParam[2];
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF4) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF4)) {
				NoteData.EffNumber[3] = pClipData->Pattern[i][j].EffNumber[3];
				NoteData.EffParam[3] = pClipData->Pattern[i][j].EffParam[3];
			}

			m_pDocument->SetNoteData(m_iCurrentFrame, i + m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);
		}
	}
}

void CPatternEditor::PasteMix(stClipData *pClipData)
{
	// Paste and mix
	stChanNote NoteData;

	// Special case, single channel and effect columns only
	if (pClipData->Channels == 1 && pClipData->StartColumn >= COLUMN_EFF1) {
		int EffOffset, Col;

		for (int j = 0; j < pClipData->Rows; ++j) {
			if ((j + m_cpCursorPos.m_iRow) < 0 || (j + m_cpCursorPos.m_iRow) >= m_iPatternLength)
				continue;

			m_pDocument->GetNoteData(m_iCurrentFrame, m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);

			for (int i = pClipData->StartColumn - COLUMN_EFF1; i < (pClipData->EndColumn - COLUMN_EFF1 + 1); ++i) {
				EffOffset = (GetSelectColumn(m_cpCursorPos.m_iColumn) - COLUMN_EFF1) + (i - (pClipData->StartColumn - COLUMN_EFF1));
				Col = i;

				if (EffOffset < 4 && EffOffset >= 0 && pClipData->Pattern[0][j].EffNumber[Col] > EF_NONE && NoteData.EffNumber[EffOffset] == EF_NONE) {
					NoteData.EffNumber[EffOffset] = pClipData->Pattern[0][j].EffNumber[Col];
					NoteData.EffParam[EffOffset] = pClipData->Pattern[0][j].EffParam[Col];
				}
			}
			m_pDocument->SetNoteData(m_iCurrentFrame, m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);
		}
		return;
	}	

	for (int i = 0; i < pClipData->Channels; ++i) {
		if ((i + m_cpCursorPos.m_iChannel) < 0 || (i + m_cpCursorPos.m_iChannel) >= (signed)m_pDocument->GetAvailableChannels())
			continue;

		for (int j = 0; j < pClipData->Rows; ++j) {
			if ((j + m_cpCursorPos.m_iRow) < 0 || (j + m_cpCursorPos.m_iRow) >= m_iPatternLength)
				continue;

			m_pDocument->GetNoteData(m_iCurrentFrame, i + m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);

			// Note & octave
			if ((i != 0 || pClipData->StartColumn <= COLUMN_NOTE) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_NOTE)) {
				if (pClipData->Pattern[i][j].Note > 0 && NoteData.Note == NONE) {
					NoteData.Note = pClipData->Pattern[i][j].Note;
					NoteData.Octave = pClipData->Pattern[i][j].Octave;
				}
			}
			// Instrument
			if ((i != 0 || pClipData->StartColumn <= COLUMN_INSTRUMENT) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_INSTRUMENT)) {
				if (pClipData->Pattern[i][j].Instrument < MAX_INSTRUMENTS && NoteData.Instrument == MAX_INSTRUMENTS) {
					NoteData.Instrument = pClipData->Pattern[i][j].Instrument;
				}
			}
			// Volume
			if ((i != 0 || pClipData->StartColumn <= COLUMN_VOLUME) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_VOLUME)) {
				if (pClipData->Pattern[i][j].Vol < VOL_COLUMN_MAX && NoteData.Vol == VOL_COLUMN_MAX) {
					NoteData.Vol = pClipData->Pattern[i][j].Vol;
				}
			}
			// Effects
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF1) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF1)) {
				if (pClipData->Pattern[i][j].EffNumber[0] != EF_NONE && NoteData.EffNumber[0] == EF_NONE) {
					NoteData.EffNumber[0] = pClipData->Pattern[i][j].EffNumber[0];
					NoteData.EffParam[0] = pClipData->Pattern[i][j].EffParam[0];
				}
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF2) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF2)) {
				if (pClipData->Pattern[i][j].EffNumber[1] != EF_NONE && NoteData.EffNumber[0] == EF_NONE) {
					NoteData.EffNumber[1] = pClipData->Pattern[i][j].EffNumber[1];
					NoteData.EffParam[1] = pClipData->Pattern[i][j].EffParam[1];
				}
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF3) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF3)) {
				if (pClipData->Pattern[i][j].EffNumber[2] != EF_NONE && NoteData.EffNumber[0] == EF_NONE) {
					NoteData.EffNumber[2] = pClipData->Pattern[i][j].EffNumber[2];
					NoteData.EffParam[2] = pClipData->Pattern[i][j].EffParam[2];
				}
			}
			if ((i != 0 || pClipData->StartColumn <= COLUMN_EFF4) && (i != (pClipData->Channels - 1) || pClipData->EndColumn >= COLUMN_EFF4)) {
				if (pClipData->Pattern[i][j].EffNumber[3] != EF_NONE && NoteData.EffNumber[0] == EF_NONE) {
					NoteData.EffNumber[3] = pClipData->Pattern[i][j].EffNumber[3];
					NoteData.EffParam[3] = pClipData->Pattern[i][j].EffParam[3];
				}
			}

			m_pDocument->SetNoteData(m_iCurrentFrame, i + m_cpCursorPos.m_iChannel, j + m_cpCursorPos.m_iRow, &NoteData);
		}
	}
}

void CPatternEditor::DeleteSelectionRows(CSelection &selection)
{
	// Delete selection including rows
	for (int i = selection.GetChanStart(); i <= selection.GetChanEnd(); ++i) {
		if (i < 0 || i > (signed)m_pDocument->GetAvailableChannels())
			continue;

		for (int j = selection.GetRowStart(); j <= selection.GetRowEnd(); ++j) {
			if (j < 0 || j > m_iPatternLength)
				continue;

			m_pDocument->PullUp(m_iCurrentFrame, i, selection.GetRowStart());
		}
	}
}

void CPatternEditor::DeleteSelection(CSelection &selection)
{
	// Delete selection
	for (int i = selection.GetChanStart(); i <= selection.GetChanEnd(); ++i) {
		if (i < 0 || i > (signed)m_pDocument->GetAvailableChannels())
			continue;

		for (int j = selection.GetRowStart(); j <= selection.GetRowEnd(); ++j) {
			if (j < 0 || j > m_iPatternLength)
				continue;

			stChanNote NoteData;
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);

			if (IsColumnSelected(COLUMN_NOTE, i)) {
				NoteData.Note = 0;
				NoteData.Octave = 0;
			}
			if (IsColumnSelected(COLUMN_INSTRUMENT, i)) {
				NoteData.Instrument = 0x40;
			}
			if (IsColumnSelected(COLUMN_VOLUME, i)) {
				NoteData.Vol = 0x10;
			}
			if (IsColumnSelected(COLUMN_EFF1, i)) {
				NoteData.EffNumber[0] = NoteData.EffParam[0] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF2, i)) {
				NoteData.EffNumber[1] = NoteData.EffParam[1] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF3, i)) {
				NoteData.EffNumber[2] = NoteData.EffParam[2] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF4, i)) {
				NoteData.EffNumber[3] = NoteData.EffParam[3] = 0;
			}
		
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &NoteData);
		}
	}
}

void CPatternEditor::Delete()
{
	// Delete selection
	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		if (i < 0 || i > (signed)m_pDocument->GetAvailableChannels())
			continue;

		for (int j = m_selection.GetRowStart(); j <= m_selection.GetRowEnd(); ++j) {
			if (j < 0 || j > m_iPatternLength)
				continue;

			stChanNote NoteData;
			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);

			if (IsColumnSelected(COLUMN_NOTE, i)) {
				NoteData.Note = 0;
				NoteData.Octave = 0;
			}
			if (IsColumnSelected(COLUMN_INSTRUMENT, i)) {
				NoteData.Instrument = 0x40;
			}
			if (IsColumnSelected(COLUMN_VOLUME, i)) {
				NoteData.Vol = 0x10;
			}
			if (IsColumnSelected(COLUMN_EFF1, i)) {
				NoteData.EffNumber[0] = NoteData.EffParam[0] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF2, i)) {
				NoteData.EffNumber[1] = NoteData.EffParam[1] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF3, i)) {
				NoteData.EffNumber[2] = NoteData.EffParam[2] = 0;
			}
			if (IsColumnSelected(COLUMN_EFF4, i)) {
				NoteData.EffNumber[3] = NoteData.EffParam[3] = 0;
			}
		
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &NoteData);
		}
	}
}

void CPatternEditor::RemoveSelectedNotes()
{
	stChanNote NoteData;

	for (int i = m_selection.GetChanStart(); i <= m_selection.GetChanEnd(); ++i) {
		if (i < 0 || i > (signed)m_pDocument->GetAvailableChannels())
			continue;

		for (int j = m_selection.GetRowStart(); j <= m_selection.GetRowEnd(); ++j) {
			if (j < 0 || j > m_iPatternLength)
				continue;

			m_pDocument->GetNoteData(m_iCurrentFrame, i, j, &NoteData);

			if (IsColumnSelected(COLUMN_NOTE, i)) {
				NoteData.Note = 0;
				NoteData.Octave = 0;
			}
			if (IsColumnSelected(COLUMN_INSTRUMENT, i)) {
				NoteData.Instrument = 0x40;
			}
			if (IsColumnSelected(COLUMN_VOLUME, i)) {
				NoteData.Vol = 0x10;
			}
			if (IsColumnSelected(COLUMN_EFF1, i)) {
				NoteData.EffNumber[0] = 0x0;
				NoteData.EffParam[0] = 0x0;
			}
			if (IsColumnSelected(COLUMN_EFF2, i)) {
				NoteData.EffNumber[1] = 0x0;
				NoteData.EffParam[1] = 0x0;
			}
			if (IsColumnSelected(COLUMN_EFF3, i)) {
				NoteData.EffNumber[2] = 0x0;
				NoteData.EffParam[2] = 0x0;
			}
			if (IsColumnSelected(COLUMN_EFF4, i)) {
				NoteData.EffNumber[3] = 0x0;
				NoteData.EffParam[3] = 0x0;
			}
		
			m_pDocument->SetNoteData(m_iCurrentFrame, i, j, &NoteData);
		}
	}
}

void CPatternEditor::MoveToRow(int Row)
{
	if (theApp.IsPlaying() && m_bFollowMode)
		return;

	if (Row < 0) {
		if (theApp.GetSettings()->General.bWrapFrames) {
			MoveToFrame(m_iCurrentFrame - 1);
			Row = m_pDocument->GetPatternLength() + Row;
		}
		else {
			if (theApp.GetSettings()->General.bWrapCursor)
				Row = m_iPatternLength - 1;
			else
				Row = 0;
		}
	}
	else if (Row > m_iPatternLength - 1) {
		if (theApp.GetSettings()->General.bWrapFrames) {
			MoveToFrame(m_iCurrentFrame + 1);
			Row = Row - m_iPatternLength;
		}
		else {
			if (theApp.GetSettings()->General.bWrapCursor)
				Row = 0;
			else
				Row = m_iPatternLength - 1;
		}
	}

	if (m_bSelecting)
		m_bForceFullRedraw = true;

	if (Row < 0)
		Row = 0;
	if (Row > (int)m_pDocument->GetPatternLength())
		Row = m_pDocument->GetPatternLength();

	m_cpCursorPos.m_iRow = Row;
}

void CPatternEditor::MoveToFrame(int Frame)
{
	if (m_iCurrentFrame == Frame)
		return;

	if (Frame < 0) {
		if (theApp.GetSettings()->General.bWrapFrames) {
			Frame = m_pDocument->GetFrameCount() - 1;
		}
		else
			Frame = 0;
	}
	if (Frame > (signed)m_pDocument->GetFrameCount() - 1) {
		if (theApp.GetSettings()->General.bWrapFrames) {
			Frame = 0;
		}
		else
			Frame = (signed)m_pDocument->GetFrameCount() - 1;
	}
	
	if (theApp.IsPlaying() && m_bFollowMode) {
		if (m_iPlayFrame != Frame) {
			m_iPlayFrame = Frame;
			m_iPlayRow = 0;
			m_iPlayPatternLength = GetCurrentPatternLength(m_iPlayFrame);
			theApp.GetSoundGenerator()->ResetTempo();
		}
	}

	m_iCurrentFrame = Frame;
	m_bSelecting = false;
	m_bForceFullRedraw = true;
   
   patternsModel->setFrame(Frame);
}

void CPatternEditor::MoveToChannel(int Channel)
{
	if (Channel < 0) {
		if (theApp.GetSettings()->General.bWrapCursor)
			Channel = m_pDocument->GetAvailableChannels() - 1;
		else
			Channel = 0;
	}
	else if (Channel > (signed)m_pDocument->GetAvailableChannels() - 1) {
		if (theApp.GetSettings()->General.bWrapCursor)
			Channel = 0;
		else
			Channel = m_pDocument->GetAvailableChannels() - 1;
	}
	m_cpCursorPos.m_iChannel = Channel;
	m_cpCursorPos.m_iColumn = 0;
}

void CPatternEditor::MoveToColumn(int Column)
{
	m_cpCursorPos.m_iColumn = Column;
}

void CPatternEditor::NextFrame()
{
	MoveToFrame(m_iCurrentFrame + 1);
	m_bSelecting = false;
}

void CPatternEditor::PreviousFrame()
{
	MoveToFrame(m_iCurrentFrame - 1);
	m_bSelecting = false;
}

bool CPatternEditor::IsColumnSelected(int Column, int Channel) const
{
	int SelColStart = m_selection.GetColStart();
	int SelColEnd	= m_selection.GetColEnd();
	int SelStart, SelEnd;

	if (Channel > m_selection.GetChanStart() && Channel < m_selection.GetChanEnd())
		return true;

	// 0 = Note (0)
	// 1, 2 = Instrument (1)
	// 3 = Volume (2)
	// 4, 5, 6 = Effect 1 (3)
	// 7, 8, 9 = Effect 1 (4)
	// 10, 11, 12 = Effect 1 (5)
	// 13, 14, 15 = Effect 1 (6)

	switch (SelColStart) {
		case 0:						SelStart = 0; break;
		case 1: case 2:				SelStart = 1; break;
		case 3:						SelStart = 2; break;
		case 4: case 5: case 6:		SelStart = 3; break;
		case 7: case 8: case 9:		SelStart = 4; break;
		case 10: case 11: case 12:	SelStart = 5; break;
		case 13: case 14: case 15:	SelStart = 6; break;
	}

	switch (SelColEnd) {
		case 0:						SelEnd = 0; break;
		case 1: case 2:				SelEnd = 1; break;
		case 3:						SelEnd = 2; break;
		case 4: case 5: case 6:		SelEnd = 3; break;
		case 7: case 8: case 9:		SelEnd = 4; break;
		case 10: case 11: case 12:	SelEnd = 5; break;
		case 13: case 14: case 15:	SelEnd = 6; break;
	}

	
	if (Channel == m_selection.GetChanStart() && Channel == m_selection.GetChanEnd()) {
		if (Column >= SelStart && Column <= SelEnd)
			return true;
	}
	else if (Channel == m_selection.GetChanStart()) {
		if (Column >= SelStart)
			return true;
	}
	else if (Channel == m_selection.GetChanEnd()) {
		if (Column <= SelEnd)
			return true;
	}

	return false;
}

int CPatternEditor::GetSelectColumn(int Column) const
{
	// Translates cursor column to select column

	switch (Column) {
		case 0:	
			return COLUMN_NOTE;
		case 1: case 2:	
			return COLUMN_INSTRUMENT;
		case 3:	
			return COLUMN_VOLUME;
		case 4: case 5: case 6:	
			return COLUMN_EFF1;
		case 7: case 8: case 9:
			return COLUMN_EFF2;
		case 10: case 11: case 12:
			return COLUMN_EFF3;
		case 13: case 14: case 15:
			return COLUMN_EFF4;
	}

	return 0;
}

int CPatternEditor::GetCurrentPatternLength(int Frame) const
{
	stChanNote Note;
	int i, j, k;
	int Channels = m_pDocument->GetAvailableChannels();
	int PatternLength = m_pDocument->GetPatternLength();	// default length

	if (!theApp.GetSettings()->General.bFramePreview)
		return PatternLength;

	if (Frame < 0 || Frame >= (signed)m_pDocument->GetFrameCount())
		return PatternLength;

	for (j = 0; j < PatternLength; j++) {
		for (i = 0; i < Channels; i++) {
			m_pDocument->GetNoteData(Frame, i, j, &Note);
			for (k = 0; k < (signed)m_pDocument->GetEffColumns(i) + 1; k++) {
				switch (Note.EffNumber[k]) {
					case EF_SKIP:
					case EF_JUMP:
					case EF_HALT:
						return j + 1;
				}
			}
		}
	}

	return PatternLength;
}

void CPatternEditor::songPatterns_actionTriggered(int action)
{
   switch ( action )
   {
   case QAbstractSlider::SliderNoAction:
      break;
   case QAbstractSlider::SliderSingleStepAdd:
   case QAbstractSlider::SliderPageStepAdd:
      break;
   case QAbstractSlider::SliderSingleStepSub:
   case QAbstractSlider::SliderPageStepSub:
      break;
   case QAbstractSlider::SliderToMinimum:
      break;
   case QAbstractSlider::SliderToMaximum:
      break;
   case QAbstractSlider::SliderMove:
      break;
   }
   
   qDebug("vs value %d, max %d",ui->songPatterns->verticalScrollBar()->value(),ui->songPatterns->verticalScrollBar()->maximum());
   
//   ui->songPatterns->setCurrentIndex(patternsModel->index(value,ui->songPatterns->currentIndex().column()));
}

void CPatternEditor::songPatterns_currentChanged(QModelIndex, QModelIndex)
{
}

