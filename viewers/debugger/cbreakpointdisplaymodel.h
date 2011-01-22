#ifndef CBREAKPOINTDISPLAYMODEL_H
#define CBREAKPOINTDISPLAYMODEL_H

#include <QAbstractTableModel>

class CBreakpointDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CBreakpointDisplayModel(QObject* parent = 0);
   virtual ~CBreakpointDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   
public slots:
   void update();
};

#endif // CBREAKPOINTDISPLAYMODEL_H
