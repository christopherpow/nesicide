#ifndef CDEBUGGERREGISTERDISPLAYMODEL_H
#define CDEBUGGERREGISTERDISPLAYMODEL_H

#include <QAbstractTableModel>

#include "cregisterdata.h"

class CDebuggerRegisterDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CDebuggerRegisterDisplayModel(regDBFunc regDB,QObject* parent = 0);
   virtual ~CDebuggerRegisterDisplayModel();
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
   bool memoryContains(uint32_t addr) const;

public slots:
   void update(void);

private:
   regDBFunc          m_regDBFunc;
   CRegisterDatabase *m_regDB;
   char              *m_modelStringBuffer;
};

#endif // CDEBUGGERREGISTERDISPLAYMODEL_H
