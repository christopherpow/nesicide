#ifndef CPROJECTLISTVIEWMODEL_H
#define CPROJECTLISTVIEWMODEL_H

#include <QAbstractItemModel>

class CProjectListViewModel : public QAbstractItemModel
{
public:
   CProjectListViewModel(QObject* parent = 0);
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   void layoutChangedEvent();
   bool setData ( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
private:
};

#endif // CPROJECTLISTVIEWMODEL_H
