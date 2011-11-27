#ifndef CPROPERTYLISTMODEL_H
#define CPROPERTYLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "propertyeditordialog.h"
#include "cpropertyitem.h"

enum
{
   PropertyCol_Name = 0,
   PropertyCol_Type,
   PropertyCol_Value,
   PropertyCol_MAX
};

class CPropertyListModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit CPropertyListModel(bool editable,QObject *parent = 0);
   virtual ~CPropertyListModel();
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant data(const QModelIndex& index, int role) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role);
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   void removeRow(int row, const QModelIndex &parent = QModelIndex());
   bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
   void insertRow(QString text, const QModelIndex &parent = QModelIndex());

   QList<PropertyItem> getItems() { return m_items; }
   void setItems(QList<PropertyItem> items) { m_items = items; }

public slots:
   void update();
   void sort(int column, Qt::SortOrder order);

private:
   QList<PropertyItem> m_items;
   int m_currentSortColumn;
   Qt::SortOrder m_currentSortOrder;
   int m_currentItemCount;
   bool m_editable;
};

#endif // CPROPERTYLISTMODEL_H
