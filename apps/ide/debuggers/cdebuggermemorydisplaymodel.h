#ifndef CDEBUGGERMEMORYDISPLAYMODEL_H
#define CDEBUGGERMEMORYDISPLAYMODEL_H

#include <QAbstractTableModel>

#include "cmemorydata.h"

class CDebuggerMemoryDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CDebuggerMemoryDisplayModel(memDBFunc memDB,QObject* parent = 0);
   virtual ~CDebuggerMemoryDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   bool setData ( const QModelIndex& index, const QVariant& value, int );
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;

   int memoryType() const;
   int memoryBottom() const;
   int memoryTop() const;

public slots:
   void update(void);

private:
   memDBFunc        m_memDBFunc;
   CMemoryDatabase *m_memDB;
   char            *m_modelStringBuffer;
};

#endif // CDEBUGGERMEMORYDISPLAYMODEL_H
