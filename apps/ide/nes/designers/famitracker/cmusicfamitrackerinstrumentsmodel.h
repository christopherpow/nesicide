#ifndef CMUSICFAMITRACKERINSTRUMENTSMODEL_H
#define CMUSICFAMITRACKERINSTRUMENTSMODEL_H

#include <QAbstractListModel>

#include "famitracker/FamiTrackerDoc.h"

class CMusicFamiTrackerInstrumentsModel : public QAbstractListModel
{
   Q_OBJECT
public:
   CMusicFamiTrackerInstrumentsModel(CFamiTrackerDoc* pDoc,QObject* parent = 0);
   virtual ~CMusicFamiTrackerInstrumentsModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
   void update(void);

private:
   CFamiTrackerDoc* m_pDocument;
   QMap<int,int> m_instrumentMap;
};

#endif // CMUSICFAMITRACKERPATTERNSMODEL_H
