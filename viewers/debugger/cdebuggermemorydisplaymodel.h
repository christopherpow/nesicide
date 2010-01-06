#ifndef CDEBUGGERMEMORYDISPLAYMODEL_H
#define CDEBUGGERMEMORYDISPLAYMODEL_H

#include <QAbstractTableModel>
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

#include "cnesicidecommon.h"

class CDebuggerMemoryDisplayModel : public QAbstractTableModel
{
public:
   CDebuggerMemoryDisplayModel(QObject* parent, eMemoryType display);
   virtual ~CDebuggerMemoryDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData ( const QModelIndex & index, const QVariant & value, int );
   QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   void layoutChangedEvent();
private:
   eMemoryType m_display;
   unsigned int m_offset;
   CRegisterData** m_tblRegisters;
};

#endif // CDEBUGGERMEMORYDISPLAYMODEL_H
