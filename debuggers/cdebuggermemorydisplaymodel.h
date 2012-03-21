#ifndef CDEBUGGERMEMORYDISPLAYMODEL_H
#define CDEBUGGERMEMORYDISPLAYMODEL_H

#include <QAbstractTableModel>

#include "emulator_core.h"

#include "cregisterdata.h"

class CDebuggerMemoryDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CDebuggerMemoryDisplayModel(eMemoryType display,QObject* parent = 0);
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
   eMemoryType memoryType() const
   {
      return m_display;
   }
   unsigned int memoryBottom() const
   {
      return m_offset;
   }
   unsigned int memoryTop() const
   {
      return m_offset+m_length-1;
   }

public slots:
   void update(void);

private:
   eMemoryType m_display;
   int m_offset;
   int m_length;
   CRegisterDatabase* m_dbRegisters;
};

#endif // CDEBUGGERMEMORYDISPLAYMODEL_H
