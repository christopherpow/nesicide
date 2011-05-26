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
   void removeRow(int row, const QModelIndex &parent);
   void insertRow(QString text, const QModelIndex &parent);

   QStringList getSymbols() { return m_symbols; }
   void setSymbols(QStringList symbols) { m_symbols = symbols; }

public slots:
   void update();

private:
   QStringList m_symbols;
};

#endif // CSYMBOLWATCHMODEL_H
