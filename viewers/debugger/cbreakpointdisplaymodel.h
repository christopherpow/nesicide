#ifndef CBREAKPOINTDISPLAYMODEL_H
#define CBREAKPOINTDISPLAYMODEL_H

#include <QAbstractListModel>

class CBreakpointDisplayModel : public QAbstractListModel
{
   Q_OBJECT
public:
   CBreakpointDisplayModel(QObject* parent = 0);
   virtual ~CBreakpointDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   void layoutChangedEvent();
};

#endif // CBREAKPOINTDISPLAYMODEL_H
