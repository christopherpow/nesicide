#ifndef CMUSICFAMITRACKERPATTERNSMODEL_H
#define CMUSICFAMITRACKERPATTERNSMODEL_H

#include <QAbstractTableModel>

#include "cqtmfc.h"
#include "famitracker/FamiTrackerDoc.h"
#include "famitracker/TrackerChannel.h"
#include "famitracker/PatternData.h"

class CMusicFamiTrackerPatternsModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CMusicFamiTrackerPatternsModel(CFamiTrackerDoc* pDoc,QObject* parent = 0);
   virtual ~CMusicFamiTrackerPatternsModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   bool setData ( const QModelIndex& index, const QVariant& value, int );
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
   void update(void);
   void setFrame(int frame) { m_frame = frame; }

private:
   CFamiTrackerDoc* m_pDocument;
   int m_frame;

   void DrawCell(int PosX, int Column, int Channel, bool bInvert, stChanNote *pNoteData, CDC *pDC, RowColorInfo_t *pColorInfo) const;
   void DrawChar(int x, int y, TCHAR c, COLORREF Color, CDC *pDC) const;
   int  GetChannelColumns(int Channel) const;
};

#endif // CMUSICFAMITRACKERPATTERNSMODEL_H
