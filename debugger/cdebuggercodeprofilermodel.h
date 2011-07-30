#ifndef CDEBUGGERCODEPROFILERMODEL_H
#define CDEBUGGERCODEPROFILERMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

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

   QStringList getSymbols() { return m_symbols; }

signals:

public slots:
   void update();

private:
   QStringList m_symbols;
};

#endif // CDEBUGGERCODEPROFILERMODEL_H
