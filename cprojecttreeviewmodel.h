#ifndef CPROJECTTREEVIEWMODEL_H
#define CPROJECTTREEVIEWMODEL_H

#include <QAbstractItemModel>
#include "iprojecttreeviewitem.h"

class CProjectTreeViewModel : public QAbstractItemModel
{
public:
    CProjectTreeViewModel(QObject * parent = 0, IProjectTreeViewItem *root = 0);
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    void layoutChangedEvent();
private:
    IProjectTreeViewItem *rootItem;
};

#endif // CPROJECTTREEVIEWMODEL_H
