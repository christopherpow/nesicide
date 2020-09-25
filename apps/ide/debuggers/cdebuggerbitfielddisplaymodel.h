#ifndef CDEBUGGERBITFIELDDISPLAYMODEL_H
#define CDEBUGGERBITFIELDDISPLAYMODEL_H

#include <QAbstractItemModel>

#include "cregisterdata.h"

class CDebuggerBitfieldDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CDebuggerBitfieldDisplayModel(regDBFunc regDB,QObject* parent = 0);
   virtual ~CDebuggerBitfieldDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   bool setData ( const QModelIndex& index, const QVariant& value, int role);
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   void setRegister ( int newRegister )
   {
      m_register = newRegister;
   }

public slots:
   void update(void);

private:
   regDBFunc  m_regDB;
   int        m_register;
   char      *m_modelStringBuffer;
};

#endif // CDEBUGGERBITFIELDDISPLAYMODEL_H
