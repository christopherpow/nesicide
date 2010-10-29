#ifndef CCODEBROWSERDISPLAYMODEL_H
#define CCODEBROWSERDISPLAYMODEL_H

#include <QAbstractTableModel>

class CCodeBrowserDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CCodeBrowserDisplayModel(QObject* parent);
   virtual ~CCodeBrowserDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   void layoutChangedEvent();

};

#endif // CCODEBROWSERDISPLAYMODEL_H
