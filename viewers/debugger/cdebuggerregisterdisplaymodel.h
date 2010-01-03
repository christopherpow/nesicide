#ifndef CDEBUGGERREGISTERDISPLAYMODEL_H
#define CDEBUGGERREGISTERDISPLAYMODEL_H

#include <QAbstractItemModel>
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

#include "cregisterdata.h"

#include "cnesicidecommon.h"

class CDebuggerRegisterDisplayModel : public QAbstractTableModel
{
public:
   CDebuggerRegisterDisplayModel(QObject* parent = 0, eMemoryType display = eMemory_PPUregs);
   virtual ~CDebuggerRegisterDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData ( const QModelIndex & index, const QVariant & value, int role);
   QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   void setRegister ( int newRegister ) { m_register = newRegister; layoutChangedEvent(); }
   void layoutChangedEvent();
private:
   eMemoryType m_display;
   int         m_register;
   CRegisterData** m_tblRegisters;
   unsigned int m_offset;
};

#endif // CDEBUGGERREGISTERDISPLAYMODEL_H
