#ifndef CDEBUGGERREGISTERDISPLAYMODEL_H
#define CDEBUGGERREGISTERDISPLAYMODEL_H

#include <QAbstractItemModel>

#include "cregisterdata.h"

#include "emulator_core.h"

class CDebuggerRegisterDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
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
};

#endif // CDEBUGGERREGISTERDISPLAYMODEL_H
