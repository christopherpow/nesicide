#include "cmusicfamitrackerpatternsmodel.h"

#include "stdio.h"

static char modelStringBuffer [ 16 ];
static int  modelStringBufferPos;

CMusicFamiTrackerPatternsModel::CMusicFamiTrackerPatternsModel(CFamiTrackerDoc* pDoc,QObject*)
{
   m_pDocument = pDoc;
   m_frame = 0;
}

CMusicFamiTrackerPatternsModel::~CMusicFamiTrackerPatternsModel()
{
}

void CMusicFamiTrackerPatternsModel::DrawChar(int x, int y, TCHAR c, COLORREF Color, CDC *pDC) const
{
   modelStringBuffer[modelStringBufferPos]=(char)c;
   modelStringBufferPos++;
}

const int CHAR_WIDTH = 10;

void CMusicFamiTrackerPatternsModel::DrawCell(int PosX, int Column, int Channel, bool bInvert, stChanNote *pNoteData, CDC *pDC, RowColorInfo_t *pColorInfo) const
{
	const char NOTES_A[] = {'C', 'C', 'D', 'D', 'E', 'F', 'F', 'G', 'G', 'A', 'A', 'B'};
	const char NOTES_B[] = {'-', '#', '-', '#', '-', '-', '#', '-', '#', '-', '#', '-'};
	const char NOTES_C[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	const char HEX[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

	const CTrackerChannel *pTrackerChannel = m_pDocument->GetChannel(Channel);

	int EffNumber = 0, EffParam;

	switch (Column) {
		case 4: case 5: case 6:
			EffNumber	= pNoteData->EffNumber[0];
			EffParam	= pNoteData->EffParam[0];
			break;
		case 7: case 8: case 9:
			EffNumber	= pNoteData->EffNumber[1];
			EffParam	= pNoteData->EffParam[1];
			break;
		case 10: case 11: case 12:
			EffNumber	= pNoteData->EffNumber[2];
			EffParam	= pNoteData->EffParam[2];
			break;
		case 13: case 14: case 15:
			EffNumber	= pNoteData->EffNumber[3];
			EffParam	= pNoteData->EffParam[3];
			break;
	}

//	// Detect invalid note data
//	if (pNoteData->Note < 0 || pNoteData->Note > HALT || EffNumber >= EF_COUNT || pNoteData->Instrument > MAX_INSTRUMENTS) {
//		if (Column == 0/* || Column == 4*/) {
//			CString Text;
//			Text.Format(_T("(invalid)"));
//			pDC->SetTextColor(RED(255));
//			pDC->TextOut(PosX, -1, Text);
//		}
//		return;
//	}

	COLORREF InstColor = pColorInfo->Instrument;

	// Make non-available instruments red in the pattern editor
	if (pNoteData->Instrument < MAX_INSTRUMENTS && (!m_pDocument->IsInstrumentUsed(pNoteData->Instrument))) {
		InstColor = RED(255);
	}

	int PosY = -2;
	PosX -= 1;

//#define BAR(x, y) pDC->FillSolidRect((x) + 3, (y) + (m_iRowHeight / 2) + 1, CHAR_WIDTH - 7, 1, pColorInfo->Shaded)
#define BAR(x,y) DrawChar(x,y,'-', RGB(255,255,255),pDC)

	switch (Column) {
		case 0:
			// Note and octave
			switch (pNoteData->Note) {
				case NONE:
					BAR(PosX, PosY);
					BAR(PosX + CHAR_WIDTH, PosY);
					BAR(PosX + CHAR_WIDTH * 2, PosY);
					return;
				case HALT:
					// Note stop
//					GradientBar(pDC, PosX + 5, (m_iRowHeight / 2) - 2, CHAR_WIDTH * 3 - 11, m_iRowHeight / 4, pColorInfo->Note, pColorInfo->Back);
					return;
				case RELEASE:
					// Note release
//					pDC->FillSolidRect(PosX + 5, 3, CHAR_WIDTH * 3 - 11, 2, pColorInfo->Note);
//					pDC->FillSolidRect(PosX + 5, 7, CHAR_WIDTH * 3 - 11, 2, pColorInfo->Note);
					return;
				default:
					if (pTrackerChannel->GetID() == CHANID_NOISE) {
						// Noise
						char NoiseFreq = (pNoteData->Note - 1 + pNoteData->Octave * 12) & 0x0F;
						DrawChar(PosX, PosY, HEX[NoiseFreq], pColorInfo->Note, pDC);
						DrawChar(PosX + CHAR_WIDTH, PosY, '-', pColorInfo->Note, pDC);
						DrawChar(PosX + CHAR_WIDTH * 2, PosY, '#', pColorInfo->Note, pDC);
					}
					else {
						// The rest
						DrawChar(PosX, PosY, NOTES_A[pNoteData->Note - 1], pColorInfo->Note, pDC);
						DrawChar(PosX + CHAR_WIDTH, PosY, NOTES_B[pNoteData->Note - 1], pColorInfo->Note, pDC);
						DrawChar(PosX + CHAR_WIDTH * 2, PosY, NOTES_C[pNoteData->Octave], pColorInfo->Note, pDC);
					}
					return;
			}
			return;
		case 1:
			// Instrument x0
			if (pNoteData->Instrument == MAX_INSTRUMENTS || pNoteData->Note == HALT || pNoteData->Note == RELEASE)
				BAR(PosX, PosY);
			else
				DrawChar(PosX, PosY, HEX[pNoteData->Instrument >> 4], InstColor, pDC);
			return;
		case 2:
			// Instrument 0x
			if (pNoteData->Instrument == MAX_INSTRUMENTS || pNoteData->Note == HALT || pNoteData->Note == RELEASE)
				BAR(PosX, PosY);
			else
				DrawChar(PosX, PosY, HEX[pNoteData->Instrument & 0x0F], InstColor, pDC);
			return;
		case 3:
			// Volume
			if (pNoteData->Vol == 0x10 || pTrackerChannel->GetID() == CHANID_DPCM)	// TODO: replace 0x10
				BAR(PosX, PosY);
			else
				DrawChar(PosX, PosY, HEX[pNoteData->Vol & 0x0F], pColorInfo->Volume, pDC);
			return;
		case 4: case 7: case 10: case 13:
			// Effect type
			if (EffNumber == 0)
				BAR(PosX, PosY);
			else {
				DrawChar(PosX, PosY, EFF_CHAR[EffNumber - 1], pColorInfo->Effect, pDC);
			}
			return;
		case 5: case 8: case 11: case 14:
			// Effect param x
			if (EffNumber == 0)
				BAR(PosX, PosY);
			else
				DrawChar(PosX, PosY, HEX[(EffParam >> 4) & 0x0F], pColorInfo->Note, pDC);
			return;
		case 6: case 9: case 12: case 15:
			// Effect param y
			if (EffNumber == 0)
				BAR(PosX, PosY);
			else
				DrawChar(PosX, PosY, HEX[EffParam & 0x0F], pColorInfo->Note, pDC);
			return;
	}
}

int CMusicFamiTrackerPatternsModel::GetChannelColumns(int Channel) const
{
	// Return number of available columns in a channel
	return m_pDocument->GetEffColumns(Channel) * 3 + COLUMNS;
}

QVariant CMusicFamiTrackerPatternsModel::data(const QModelIndex& index, int role) const
{
   int Columns	 = GetChannelColumns(index.column());
   RowColorInfo_t colorInfo;
   int col;

   colorInfo.Back = RGB(0,0,0);
   colorInfo.Effect = RGB(255,0,0);
   colorInfo.Instrument = RGB(255,255,0);
   colorInfo.Note = RGB(255,255,255);
   colorInfo.Shaded = RGB(180,180,180);
   colorInfo.Volume = RGB(0,255,255);

   if (!index.isValid())
   {
      return QVariant();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   stChanNote NoteData;

   m_pDocument->GetNoteData(m_frame, index.column(), index.row(), &NoteData);

   memset(modelStringBuffer,0,sizeof(modelStringBuffer));
   modelStringBufferPos = 0;
   for ( col = 0; col < Columns; col++ )
   {
      DrawCell(0,col,index.column(),false,&NoteData,NULL,&colorInfo);
   }

   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CMusicFamiTrackerPatternsModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QVariant CMusicFamiTrackerPatternsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Vertical )
   {
      sprintf ( modelStringBuffer, "%02X", section );
   }
   else
   {
      return QVariant((char*)m_pDocument->GetChannel(section)->GetChannelName());
   }

   return QVariant(modelStringBuffer);
}

bool CMusicFamiTrackerPatternsModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   unsigned int data;
   bool ok = false;

   data = value.toString().toInt(&ok,16);

   if ( ok )
   {
      // CPTODO: set order when music data is ported.
      emit dataChanged(index,index);
   }

   return ok;
}

QModelIndex CMusicFamiTrackerPatternsModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      // CPTODO: return orders row data here when music data is ported.
      return createIndex(row,column,row);
   }

   return QModelIndex();
}

int CMusicFamiTrackerPatternsModel::rowCount(const QModelIndex&) const
{
   return m_pDocument->GetPatternLength();
}

int CMusicFamiTrackerPatternsModel::columnCount(const QModelIndex& /*parent*/) const
{
   return m_pDocument->GetChannelCount();
}

void CMusicFamiTrackerPatternsModel::update()
{
   emit layoutChanged();
//   emit dataChanged(QModelIndex(),QModelIndex());
}
