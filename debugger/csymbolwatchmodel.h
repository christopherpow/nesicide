#ifndef CSYMBOLWATCHMODEL_H
#define CSYMBOLWATCHMODEL_H

#include <QAbstractTableModel>
#include <QList>

struct WatchedItem
{
   QString symbol;
   int     segment;
};

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
   void removeRow(int row, const QModelIndex &parent);
   void insertRow(QString text, const QModelIndex &parent);

   QList<WatchedItem> getItems() { return m_items; }
   void setItems(QList<WatchedItem> items) { m_items = items; }

   int resolveSymbol(QString text);

public slots:
   void update();
   void sort(int column, Qt::SortOrder order);

private:
   QList<WatchedItem> m_items;
   int m_currentSortColumn;
   Qt::SortOrder m_currentSortOrder;
};

#endif // CSYMBOLWATCHMODEL_H
