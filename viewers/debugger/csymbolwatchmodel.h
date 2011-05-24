#ifndef CSYMBOLWATCHMODEL_H
#define CSYMBOLWATCHMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class CSymbolWatchModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit CSymbolWatchModel(QObject *parent = 0);
   virtual ~CSymbolWatchModel();
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant data(const QModelIndex& index, int role) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role);
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
   void update();

private:
   QStringList symbols;
};

#endif // CSYMBOLWATCHMODEL_H
