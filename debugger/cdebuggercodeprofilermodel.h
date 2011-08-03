#ifndef CDEBUGGERCODEPROFILERMODEL_H
#define CDEBUGGERCODEPROFILERMODEL_H

#include <QAbstractTableModel>
#include <QList>

struct ProfiledItem
{
   QString file;
   QString symbol;
   QString address;
   QString calls;
   unsigned int count;
};

class CDebuggerCodeProfilerModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit CDebuggerCodeProfilerModel(QObject *parent = 0);
   virtual ~CDebuggerCodeProfilerModel();
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QModelIndex index(int row, int column, const QModelIndex &parent) const;
   QVariant data(const QModelIndex& index, int role) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

   QList<ProfiledItem> getItems() { return m_items; }

signals:

public slots:
   void update();
   void sort(int column, Qt::SortOrder order);

private:
   QList<ProfiledItem> m_items;
   int m_currentSortColumn;
   Qt::SortOrder m_currentSortOrder;
};

#endif // CDEBUGGERCODEPROFILERMODEL_H
